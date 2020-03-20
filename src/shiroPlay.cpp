#include <cstdlib>
#include <ctime>
#include <cmath>
#include "unistd.h"
#include  <stdio.h>
#include  <stdlib.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <chrono>
#include <vector>
#include <queue>
#include "shiroEinstein.hpp"
#include "shiroNode.hpp"

// DEFINE
using PII = std::pair<int, int>;
using VII = std::vector<PII>;
//#define LOG
//#define LOG_STD
#define DEBUG

// PARAMETERS
const static int COUNT = 3600;
const static double TIME_SETTING = 9;








auto timer = [] ( bool reset = false ) -> double {
    static decltype(std::chrono::steady_clock::now()) tick, tock;
    if ( reset ) {
        tick = std::chrono::steady_clock::now();
        return (0);
    }
    tock = std::chrono::steady_clock::now();
    return (std::chrono::duration_cast<std::chrono::duration<double>>(tock-tick).count());
};


int main()
{
    
    do{
        /* get initial positions */
        char init[2][NUM_CUBE+1] = {};
        char start;
        
        for ( int i=0; i<2; ++i ) { 
			for ( int j=0; j<NUM_CUBE; ++j ) {
				init[i][j] = getchar();
			}
		}
        init[0][NUM_CUBE] = init[1][NUM_CUBE] = '\0';
		start = getchar();

        /* game start s*/
        BOARD* game_board;
        bool myturn; // first round -> true:RED   false: BLUE
		game_board = new BOARD(init[0], init[1]);
        game_board->copy_initial_to_current_board();
        game_board->init_static_member();
        //PII m;
        for(myturn=(start=='f'); game_board->_winner==3; myturn = !myturn){
            if( myturn ){
                timer(true); // reset timer

                shiro_node* root = new shiro_node(game_board, false);
                root->board.copy_initial_to_current_board();
                VII moves; // variable for grow tree
                do{
                    // Find PV from root to leaf
                    shiro_node* pv_node = root; 
                    shiro_node* tmp = pv_node;
                    while(tmp!=nullptr){      
                        tmp = pv_node->find_pv();
                        if(tmp!=nullptr)pv_node = tmp;
                    }
                    if(pv_node->board._winner!=3) break;
                    // End find PV
                    
                    // Grow tree
                    moves = pv_node->board.move_list();
                    
                    for(int i = 0; i < moves.size(); ++i){
                        shiro_node* newNode;
                        if(pv_node->board._winner!=3){ //end game
                            newNode = new shiro_node(&(pv_node->board),pv_node->parentIsMaxNode,moves[i].first,moves[i].second); 
                        }else{ // common case
                            newNode = new shiro_node(&(pv_node->board),!pv_node->parentIsMaxNode,moves[i].first,moves[i].second); 
                        }
                        newNode->board.copy_initial_to_current_board();

                        newNode->parent = pv_node;
                        newNode->board.do_real_move(&(moves[i].first), &(moves[i].second));
                        pv_node->childs.push_back(newNode);
                    }
                    // End grow tree

                    // Simulation
                    int result;
                    int c;
                    int score;

                    for(int i = 0; i < pv_node->childs.size(); ++i){
                        c = 0;
                        if(pv_node->childs[i]->board._winner==3){
                            while(c<(COUNT/pv_node->childs.size())){

                                result = pv_node->childs[i]->board.random_simulate();

                                if((result==0 && start=='f') || (result==1 && start=='s')){
                                    score = pv_node->childs[i]->board.new_get_board_score(); // update score for PP
                                    pv_node->childs[i]->update_sum(score); // update score for PP
                                    (pv_node->childs[i]->win)++;
                                }else if(result==2){
                                    (pv_node->childs[i]->draw)++;
                                }else{
                                    score = pv_node->childs[i]->board.new_get_board_score(); // update score for PP
                                    pv_node->childs[i]->update_sum(score*-1); // update score for PP
                                    (pv_node->childs[i]->lose)++;
                                }

                                pv_node->childs[i]->board.copy_initial_to_current_board(); // reset board
                                c++;
                            }
                        }else if(pv_node->childs[i]->board._winner==2){
                            pv_node->childs[i]->draw += COUNT;
                        }else if(((pv_node->childs[i]->board._winner==1)&&(start=='s')) || ((pv_node->childs[i]->board._winner==0)&&(start=='f'))){
                            pv_node->childs[i]->win += COUNT;
                        }else{
                            pv_node->childs[i]->lose += COUNT;
                        }
                        pv_node->childs[i]->leaf_update(); // update for UCB
                    }
                    // after simulation, do PP
                    pv_node->progressivePruning();  

                }while(timer()<TIME_SETTING);     

                // Find real move
                shiro_node* candidate = root->find_move();
                // Do real move
                game_board->do_real_move(&(candidate->num), &(candidate->dir));
                //Send move to enemy
                std::cout << game_board->send_move(candidate->num, candidate->dir) << std::flush;
                //To do : clean tree

            }else{
                // Enemy move
                int n, dir;
                n = int(getchar()-'0');
				dir = int(getchar()-'0');
                game_board->do_real_move(&n, &dir);
            }
        }
        delete game_board;
    }while(getchar()=='y');

    return 0;
}