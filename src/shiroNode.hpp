#include <vector>
#include <math.h> 
#include "shiroEinstein.hpp"


#ifndef SHIRO_NODE
#define SHIRO_NODE



class shiro_node{
    public:
        //how to get here from parent
        static double C;
        static double PP_EFFECTIVE_NUM;
        static double RD;
        static double SIGMA_E;
        int num = -9;
        int dir = -9;
        BOARD board;
        bool parentIsMaxNode;
        double winRate = 0;
        int win = 0;
        int lose = 0;
        int draw = 0;
        int Ntotal = 0;
        double sqrtNtotal = 0;

        bool pruned = false;
        double mean = 0; 
        double sd = 0;
        double sum1 = 0;
        double sum2 = 0;

        int NtotalChild = 0;
        double logNtotalChild = 0;
        double sqrtLogNtotalChild = 0;
        
          
        shiro_node* parent = nullptr;
        std::vector<shiro_node*> childs;
        
        //constructor
        shiro_node(BOARD* parent_board, bool isMaxNode){ 
            board = BOARD(parent_board);
            parentIsMaxNode = isMaxNode;
        };

        shiro_node(BOARD* parent_board, bool isMaxNode, int n, int d){ 
            board = BOARD(parent_board);
            parentIsMaxNode = isMaxNode;
            num = n;
            dir = d;
        };

        shiro_node(){
        };

        void update(int* w, int* l, int* d, double*, double*);
        void leaf_update();
        double getUCBScore(int* childnum);
        void update_sum(int);
        shiro_node* find_move();
        shiro_node* find_pv(); // (isLeaf) ? return nullptr : PV_node
        void progressivePruning();
        void update_mean_sd();
};

#endif //SHIRO_NODE