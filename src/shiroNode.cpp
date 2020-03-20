#include "shiroNode.hpp"

double shiro_node::C = 1.18;
double shiro_node::PP_EFFECTIVE_NUM = 3000;
double shiro_node::SIGMA_E = 15;
double shiro_node::RD = 2;

void shiro_node::update(int* w, int* l, int* d, double* s1, double* s2){
    win += *w;
    lose += *l;
    draw += *d;
    Ntotal = win + lose + draw;
    sqrtNtotal = sqrt(Ntotal);
    NtotalChild += (*w + *l + *d);
    sum1 += (*s1);
    sum2 += (*s2);
    if(parentIsMaxNode){
        winRate = float(win)/float(Ntotal); 
    }
    else{
        winRate = float(lose)/float(Ntotal); 
    }

    logNtotalChild = log(NtotalChild);
    sqrtLogNtotalChild = sqrt(logNtotalChild);

    if((win+lose) >= PP_EFFECTIVE_NUM) update_mean_sd();

    if(parent!=nullptr) parent->update(w, l, d, s1, s2);
};

void shiro_node::update_sum(int score){
    sum1 += score;
    sum2 += score*score;
};

void shiro_node::leaf_update(){
    Ntotal = win + lose + draw; 
    sqrtNtotal = sqrt(Ntotal);
    if(parentIsMaxNode){
        winRate = float(win)/float(Ntotal); 
    }
    else{
        winRate = float(lose)/float(Ntotal); 
    }
     
    if((win+lose) >= PP_EFFECTIVE_NUM)update_mean_sd();
    
    if(parent!=nullptr)parent->update(&win, &lose, &draw, &sum1, &sum2);
};


double shiro_node::getUCBScore(int* childnum){
    return C*(sqrtLogNtotalChild/childs[*childnum]->sqrtNtotal);
};

shiro_node* shiro_node::find_move(){
    shiro_node* candidate = nullptr;
    double candidateWinRate = -1;
    for(int i = 0 ; i < childs.size() ; ++i){
        if(childs[i]->winRate > candidateWinRate){
            candidateWinRate = childs[i]->winRate;
            candidate = childs[i];
        }else if(childs[i]->winRate == candidateWinRate){
            double candidtateNonLossRate = double(candidate->win+candidate->draw)/double(candidate->Ntotal);
            double childNonLossRate = double(childs[i]->win+childs[i]->draw)/double(childs[i]->Ntotal);
            if(childNonLossRate >= candidtateNonLossRate){
                candidateWinRate = childs[i]->winRate;
                candidate = childs[i];
            }
        }
    }
    return candidate;
};

extern std::fstream flog;
shiro_node* shiro_node::find_pv(){
    
    if(childs.size()==0)return nullptr; //if reach leaf return nullptr

    shiro_node* pv_node = nullptr;
    double maxScore = 0;
    double score = 0;

    //pick highest score
    for(int i = 0; i < childs.size() ; ++i){
        if(childs[i]->pruned) continue;
        score = childs[i]->winRate + getUCBScore(&i);
        
        if(maxScore <= score){    
            maxScore = score;
            pv_node = childs[i];
        }
    }
    return pv_node;
};

void shiro_node::progressivePruning(){

    if(childs.size()<=1) return; //this is the only one move left for its parent

    double maxwin = 0;
    bool pim = true;
    

    if(parentIsMaxNode){ // I'm MinNode
        double smallestL = 10000;
            // find largestL
        for(int i = 0; i < childs.size() ; ++i){
            if(((childs[i]->win+childs[i]->lose) >= PP_EFFECTIVE_NUM) && (!childs[i]->pruned) && (childs[i]->sd < SIGMA_E) && ((childs[i]->mean + RD*childs[i]->sd)<smallestL)){
                smallestL = childs[i]->mean + RD*childs[i]->sd;
                maxwin = childs[i]->winRate;
                pim = childs[i]->parentIsMaxNode;
            }
        }
        // cut
        for(int i = 0; i < childs.size() ; ++i){
            if(((childs[i]->win+childs[i]->lose) >= PP_EFFECTIVE_NUM) && (!childs[i]->pruned) 
                && (childs[i]->sd < SIGMA_E) && ((childs[i]->mean - RD*childs[i]->sd) > smallestL)) childs[i]->pruned = true;
        }
    }else{ // I'm MaxNode
        double largestL = -10000;
            // find largestL
        for(int i = 0; i < childs.size() ; ++i){
            if(((childs[i]->win+childs[i]->lose)>= PP_EFFECTIVE_NUM) && (!childs[i]->pruned) && (childs[i]->sd < SIGMA_E) && ((childs[i]->mean - RD*childs[i]->sd)>largestL)){
                largestL = childs[i]->mean - RD*childs[i]->sd;
                maxwin = childs[i]->winRate;
                pim = childs[i]->parentIsMaxNode;
            }
        }
        // cut
        for(int i = 0; i < childs.size() ; ++i){
            if((childs[i]->win+childs[i]->lose) && (!childs[i]->pruned) 
                && (childs[i]->sd < SIGMA_E) && ((childs[i]->mean + RD*childs[i]->sd) < largestL)) childs[i]->pruned = true;
        }
    }

    if(parent!=nullptr) parent->progressivePruning();
};


void shiro_node::update_mean_sd(){
    mean = sum1/(win+lose);
    sd = sqrt((sum2-(mean*sum1*2))/(win+lose)+mean*mean);
};

