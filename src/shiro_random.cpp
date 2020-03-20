#include <cstdlib>
#include <ctime>
#include <cmath>

#include <iostream>
#include <fstream>
#include <utility>
#include <chrono>

#include "shiroEinstein.hpp"
#include "shiroNode.hpp"

char start;
using PII = std::pair<int, int>;
using VII = std::vector<PII>;
int n, dir;
bool myturn; // first round -> true:RED   false: BLUE
const bool LOG = true;
BOARD* test_board;
char init[2][NUM_CUBE+1] = {};


// for test
std::fstream flog;
void logger ( std::string logfile ) {
	flog.open(logfile, std::fstream::out);
	if ( !flog.is_open() ) {
		throw std::runtime_error("error opening file\n");
	}
}




void printBoard(BOARD* b){
    for(int i=0 ; i<6 ; ++i){
        for(int j=0 ; j<6 ; ++j){
            if(b->current_board[i][j]==nullptr){
                flog << "_ ";
            }
            else{
                flog << b->current_board[i][j]->num;
            }
        }
        flog << std::endl;
    }

    for(int i=0; i<2; ++i){
        for(int j=0 ; j<6 ; ++j){
            flog << b->current_cube[i][j].num;
        }
        flog << std::endl;
    }
}

int main()
{
    
    do{
        /* get initial positions */
        for ( int i=0; i<2; ++i ) { 
			for ( int j=0; j<NUM_CUBE; ++j ) {
				init[i][j] = getchar();
			}
		}
        init[0][NUM_CUBE] = init[1][NUM_CUBE] = '\0';
		start = getchar();
        PII m;
        VII moves;
        if(LOG){
            logger("test_log.txt");
            flog << init[0] << " " << init[1] << std::endl;
		    flog << start << std::endl;
        }
        /* game start s*/
		test_board = new BOARD(init[0], init[1]);
        //printBoard(test_board);

        for(myturn=(start=='f'); test_board->_winner==3; myturn = !myturn){
            if( myturn ){
                // code here
                //timer
                //shiro_node* root = new shiro_node();
                //while
                    //find pv
                    
                    
                
                
                //moves = test_board->move_list();
                //std::vector<int> count;

                //grow tree
                //for(int i = 0; i < moves.size(); ++i){
                //    root->childs.emplace_back(new shiro_node(!root->isMax, moves[i].first, moves[i].second));
                //}

                //simulate
                m = test_board->get_random_move();
                
                
                
                test_board->do_move(&m.first, &m.second);

                //test_board->do_real_move(&m.first, &m.second);
                std::cout << test_board->send_move(m.first, m.second) << std::flush;
                //clean tree
                
            }else{
                n = int(getchar()-'0');
				dir = int(getchar()-'0');
                test_board->do_move(&n, &dir);
                //test_board->do_real_move(&n, &dir);
            }
        }
        delete test_board;
    }while(getchar()=='y');

    return 0;
}