#ifndef SHIRO_EINSTEIN_HPP
#define SHIRO_EINSTEIN_HPP


#include <cstdio>
#include <cstdint>
#include <cassert>
#include <iostream> // cin, cout, cerr
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <random> // default_random_engine
#include <chrono> // system_clock
#include <algorithm> // shuffle
#include <set>


using PII = std::pair<int, int>;
using VII = std::vector<PII>;

static std::random_device my_rd;    
static std::mt19937_64 my_random_engine(my_rd());
static std::uniform_int_distribution<> num(0, 719);
static std::uniform_int_distribution<> rand_dir(0, 719); 
static const int NUM_CUBE = 6;
static const int NUM_POSITION = 36;
static const int BOARD_SZ = 6;
static const int NUM_PLAYER = 2;
static const int R_PLAYER = 0;
static const int B_PLAYER = 1;
static int NUM_ARRAY[720][6];



template<typename E> typename std::underlying_type<E>::type constexpr enum2int ( E enumerator ) noexcept {
    return (static_cast<typename std::underlying_type<E>::type>(enumerator));
}



// [0] = Player0, [1] = Player1
const int dx[2][3] = {{1, 0, 1}, {-1, 0, -1}};
const int dy[2][3] = {{0, 1, 1}, {0, -1, -1}};


/*
            0  1  2  3  4  5
player R : -1 -2 -3 -4 -5 -6 (left top)
player B :  1  2  3  4  5  6 (right down)

*/

class CUBE{
    public:
        int x;
        int y;
        int num;
        CUBE(int px, int py, int pos) : x(px), y(py), num(pos){};
        CUBE() : x(7), y(7) , num(0){};
};

class BOARD{

    public:    
        static int NUMS[6]; 
        static int DIR_ARRAY[6][3];
        static int NODE_SCORE[2][6];
        int _winner = 3;
        int score = 0;
        int steps = 0;  //min:11  max:78
        bool _turn = 0; // 0 : R moves       1 : B moves 

        bool initial_turn = 0;
        int initial_winner = 3;
        int initial_steps = 0;
        int initial_num_cubes[2] = {NUM_CUBE, NUM_CUBE};
        CUBE  initial_cube[2][6]; // -1 -2 -3 -4 -5 -6
                                  //  1  2  3  4  5  6-
        
        CUBE* current_board[6][6];
        CUBE  current_cube[2][6];
        int current_num_cubes[2] = {NUM_CUBE, NUM_CUBE}; // both start with num_cubes cubes


        //for get random move 
        int ply, i, j, k, xx, yy, random_node_num, random_dir_num;
        bool isNewNum;
        std::set<int>::iterator iter;

        BOARD(){
            
        };

        BOARD(std::string a, std::string b){
            for(int i = 0; i < 6; ++i){
                for(int j = 0; j < 6 ; ++j){
                    current_board[i][j] = nullptr;
                }
            }
            //a : playerR location
            initial_cube[0][a[0]-'0'] = CUBE(0,0,convert_cube_num_R(&a[0]));
            initial_cube[0][a[1]-'0'] = CUBE(0,1,convert_cube_num_R(&a[1]));
            initial_cube[0][a[2]-'0'] = CUBE(0,2,convert_cube_num_R(&a[2]));
            initial_cube[0][a[3]-'0'] = CUBE(1,0,convert_cube_num_R(&a[3]));
            initial_cube[0][a[4]-'0'] = CUBE(1,1,convert_cube_num_R(&a[4]));
            initial_cube[0][a[5]-'0'] = CUBE(2,0,convert_cube_num_R(&a[5]));


            //b : playerB location
            initial_cube[1][b[0]-'0'] = CUBE(3,5,convert_cube_num_B(&b[0]));
            initial_cube[1][b[1]-'0'] = CUBE(4,4,convert_cube_num_B(&b[1]));
            initial_cube[1][b[2]-'0'] = CUBE(4,5,convert_cube_num_B(&b[2]));
            initial_cube[1][b[3]-'0'] = CUBE(5,3,convert_cube_num_B(&b[3]));
            initial_cube[1][b[4]-'0'] = CUBE(5,4,convert_cube_num_B(&b[4]));
            initial_cube[1][b[5]-'0'] = CUBE(5,5,convert_cube_num_B(&b[5]));

        };

        BOARD(BOARD* b){
            for(int i = 0; i < 6; ++i){
                for(int j = 0; j < 6 ; ++j){
                    current_board[i][j] = nullptr;
                }
            }
            for(int i = 0; i < 6 ; ++i){
                initial_cube[0][i] = b->current_cube[0][i];
                initial_cube[1][i] = b->current_cube[1][i];
            }
            initial_turn = b->_turn;
            initial_winner = b->_winner;
            initial_steps = b->steps;
            initial_num_cubes[0] = b->current_num_cubes[0];
            initial_num_cubes[1] = b->current_num_cubes[1];

        };

        //init
        int convert_cube_num_B(char*);
        int convert_cube_num_R(char*);
        void permute_NUM_ARRAY(int*, int, int); // for random array
        void init_static_member(); // for random array
        
        //pp score
        int get_board_score(bool); //步數計分
        int new_get_board_score(); //子力計分

        //grow tree
        VII move_list(); 
        VII special_moves();

        //simulation
        int random_simulate();
        PII get_random_move();
        PII special_move();
        int* get_num_order();
        int* get_dir_order();

        //basic function
        void do_move(int* , int* );
        void do_move(PII);
        void do_real_move(int*, int*);
        void update();
        bool out ( int const &, int const &);
        PII find(int* who, int* num);
        void copy_initial_to_current_board();
        
        std::string send_move (int num, int dir){
            std::string m;
            m += (char)(num+'0');
            m += (char)(dir+'0');
            return (m);
	    }
};



#endif