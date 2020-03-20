#include "shiroEinstein.hpp"

int BOARD::NUMS[6] = {0,1,2,3,4,5};
int BOARD::DIR_ARRAY[6][3] = {{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,1,0},{2,0,1}};
int BOARD::NODE_SCORE[2][6] = {{-1,-1,0,0,0,0},{1,1,0,0,0,0}};

int BOARD::convert_cube_num_R(char* c){
    /*
                0  1  2  3  4  5
    player R : -1 -2 -3 -4 -5 -6 (left top)
    
    */
    return (int((*c)-'0')+1)*-1;
};

int BOARD::convert_cube_num_B(char* c){
    /*
                0  1  2  3  4  5
    player B :  1  2  3  4  5  6 (right down)

    */
    return int((*c)-'/');
};

// 寫得很爛..以後再改
void BOARD::permute_NUM_ARRAY(int* a, int l, int r)  
{  
    // Base case  
    if (l == r) { 
        for(int x=0; x<6; ++x){
            NUM_ARRAY[xx][x] = a[x];
        }
        xx++;
    }
    else
    {  
        int tmp;
        // Permutations made  
        for (int i = l; i <= r; i++)  
        {  
            // Swapping done 
            tmp = a[l];
            a[l] = a[i];
            a[i] = tmp;
            // Recursion called  
            permute_NUM_ARRAY(a, l+1, r);  
            //backtrack  
            tmp = a[l];
            a[l] = a[i];
            a[i] = tmp; 
        }  
    }  
}; 

void BOARD::init_static_member(){
    xx=0;
    permute_NUM_ARRAY(NUMS,0,5);
};


void BOARD::copy_initial_to_current_board(){
    _turn = initial_turn;
    _winner = initial_winner;
    steps = initial_steps;
    current_num_cubes[0] = initial_num_cubes[0];
    current_num_cubes[1] = initial_num_cubes[1];
    //copy to current_board / current_cube
    for(int i = 0; i < 6; ++i){
        for(int j = 0; j < 6 ; ++j){
            current_board[i][j] = nullptr;
        }
    }
    for(int i=0 ; i<6; ++i){
        current_cube[0][i] = initial_cube[0][i];
        current_cube[1][i] = initial_cube[1][i];
     }
    for(int i=0; i<2 ; ++i){
        for(int j=0; j<6 ; ++j){
            if(current_cube[i][j].x==7){
                continue;
            }
            current_board[current_cube[i][j].x][current_cube[i][j].y] = &(current_cube[i][j]);
        }
    }
    return;
};

void BOARD::update(){
	// 0 = player R wins, 1 = player B wins, 2 = draw, 3 = not over
    if(current_num_cubes[0]==0) {
        _winner = 1;
        return;
    } //R all dead

    if(current_num_cubes[1]==0) {
        _winner = 0;
        return;
    } //B all dead

    if(current_board[0][0]!=nullptr && current_board[5][5]!=nullptr){
        if ( (current_board[0][0]->num)>0 && (current_board[5][5]->num) < 0) {
            if ( current_board[0][0]->num < ((current_board[5][5]->num)*-1)) {
                _winner = 1;
            }
            else if ( current_board[0][0]->num > ((current_board[5][5]->num)*-1)) {
                _winner = 0;
            }
            else {
                _winner = 2;
            }
        }
    }
    return;
};

//步數計分法
int BOARD::new_get_board_score(){
   return 50-steps > 0 ? 50-steps : 1;
};

//子力計分法
int BOARD::get_board_score(bool redWin){
/*
    _winner < 0  : red win
    _winner > 0  : blue win
    _winner == 0 : draw  <- never enter here

    node0 : 4
    node1 : 3
    node2 : 2
    node3 : 1
    node4 : 0
    node5 : 0
    node_num * 2
    紅色為負分 / 藍色為正分

    if red win
        return min(point,-1)
    else if blue win
        return max(point,1)
*/
    int Score = 0;
    //node score
    for(int i = 0 ; i < 4 ; ++i){
        // red
        if(current_cube[0][i].x!=7){
            Score += NODE_SCORE[0][i];
        }
        // blue
        if(current_cube[1][i].x!=7){
            Score += NODE_SCORE[1][i];
        }
    }
    //node num score
    Score -= current_num_cubes[0];
    Score += current_num_cubes[1];

    if(redWin){
        Score -= 5;
        return (Score < -1) ? Score : -1;
    }else{
        Score += 5;
        return (Score > 1) ? Score : 1;
    }

}


PII BOARD::find(int* who, int* num){
    return(std::make_pair(current_cube[*who][*num].x,current_cube[*who][*num].y));
};

VII BOARD::move_list(){
    VII res;
    int xx, yy;
    int ply = int(_turn);
    

    //if there is at least one special move, just grow the special move.
    res = special_moves();
    if(res[0].first!=-1) return res;

    //no special moves
    VII res2, lowPriorityMove;
    for( int i = 0; i < NUM_CUBE; ++i){
        PII pos = find(&ply, &i);
        if(pos.first==7) continue;

        for(int dir=0; dir<3 ; ++dir){
            xx = pos.first+dx[ply][dir];
            yy = pos.second+dy[ply][dir];
            if(out(xx,yy)){
                continue;
            }

            //在對面還有比我大的號碼且，其他號碼尚未達陣時不落子達陣
            if(ply && (xx==0) && (yy==0)){ // ply : 1
                if((current_board[5][5] != nullptr) && (((current_board[5][5]->num)>>31) != ((current_cube[ply][i].num)>>31))){ 
                    if(abs(current_board[5][5]->num)>abs(current_cube[ply][i].num)){//必勝步
                        VII winMove;
                        winMove.emplace_back(i,dir);
                        return winMove;
                    }else{
                        lowPriorityMove.emplace_back(i,dir);
                    }
                }else{ //對方還沒達陣 我就不達陣
                    lowPriorityMove.emplace_back(i,dir);
                } 
            }else if(!ply && (xx==5) && (yy==5)){ // ply : 0
                if((current_board[0][0] != nullptr) && (((current_board[0][0]->num)>>31) != ((current_cube[ply][i].num)>>31))){ 
                    if(abs(current_board[0][0]->num)>abs(current_cube[ply][i].num)){ //必勝步
                        VII winMove;
                        winMove.emplace_back(i,dir);
                        return winMove;
                    }else{
                        lowPriorityMove.emplace_back(i,dir);
                    }
                }else{ //對方還沒達陣 我就不達陣
                    lowPriorityMove.emplace_back(i,dir);
                } 
            }else{
                //沒有遇到達陣步 or 不會吃到子 or 不會吃到自己人
                if(current_board[xx][yy]==nullptr || (lowPriorityMove.empty()) || (current_board[xx][yy]!=nullptr && (((current_board[xx][yy]->num)>>31) != ((current_cube[ply][i].num)>>31)))){
                    res2.emplace_back(i,dir);
                }else{
                    // 遇到達陣步的時候，check一下這步是否會踩爆自己的最小子 會踩爆的話不如讓最小子達陣算了
                    int min_node = 7;
                    for(int k=0 ; k<6 ; ++k){
                        if(current_cube[ply][k].x!=7){
                            min_node = current_cube[ply][k].num;
                            break;
                        }
                    }
                    if(current_board[xx][yy]->num!=min_node){
                        res2.emplace_back(i,dir);
                    }
                }  
            }
        }
    }

    //no legal move => pass
    if ( res2.empty() && lowPriorityMove.empty()){
        res2.emplace_back(15, 15);
        return res2;
    }

    
    if(res2.empty()) return lowPriorityMove;  //no normal move => go lowPriorityMove
    return res2; // normal move  	
};

bool BOARD::out( int const &x, int const &y ){
    return (x<0 || y<0 || x>=BOARD_SZ || y>=BOARD_SZ);
}


PII BOARD::get_random_move(){
    ply = int(_turn);
    
    // if there is a special move, do not random, just move!
    PII s_move = special_move();
    if(s_move.first!=15){
        xx = current_cube[ply][s_move.first].x+dx[ply][s_move.second];
        yy = current_cube[ply][s_move.first].y+dy[ply][s_move.second];
        if((xx==0 && yy==0)||(xx==5 && yy==5)){  // However, if this sepcial move will reach the goal, do not move.
            s_move = std::make_pair(15,15); 
        }else{
            return s_move;  // special move
        }
    }
        
    //no special move 有時間可以優化
    PII l_move = std::make_pair(15,15);
    int* num_order = get_num_order(); // get num array
    int* dir_order = get_dir_order(); // get dir array
    for(i=0 ; i < 3 ; ++i){
        for(j=0 ; j<6 ; ++j){
            if(current_cube[ply][num_order[j]].x==7) continue;
            xx = current_cube[ply][num_order[j]].x+dx[ply][DIR_ARRAY[dir_order[j]][i]];
            yy = current_cube[ply][num_order[j]].y+dy[ply][DIR_ARRAY[dir_order[j]][i]];
            if(out(xx,yy)) continue;

            if(ply && (xx==0) && (yy==0)){ // ply : 1  //大概有不必要的code
                if((current_board[5][5] != nullptr) && (((current_board[5][5]->num)>>31) != ((current_cube[ply][num_order[j]].num)>>31))){ 
                    if(abs(current_board[5][5]->num)>abs(current_cube[ply][num_order[j]].num)){//必勝步
                        return std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                    }else{//必不勝步                            
                        l_move = std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]); 
                    }
                }else{ //對方還沒達陣 我就不達陣
                        s_move = std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                } 
            }else if(!ply && (xx==5) && (yy==5)){ // ply : 0
                if((current_board[0][0] != nullptr) && (((current_board[0][0]->num)>>31) != ((current_cube[ply][num_order[j]].num)>>31))){ 
                    if(abs(current_board[0][0]->num)>abs(current_cube[ply][num_order[j]].num)){ //必勝步
                        return std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                    }else{//必不勝步
                        l_move = std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]); 
                    }
                }else{ //對方還沒達陣 我就不達陣
                    s_move = std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                } 
            }else{ 
                //沒有隨機到達陣步 or 不會吃到子 or 不會吃到自己人
                if(current_board[xx][yy]==nullptr || (s_move.first == 15 && l_move.first == 15) || (current_board[xx][yy]!=nullptr && (((current_board[xx][yy]->num)>>31) != ((current_cube[ply][num_order[j]].num)>>31)))){
                    return std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                }
                // 當先前有隨機到達陣步的時候，且會吃到自己人時，check一下這步是否會踩爆自己的最小子 會踩爆的話不如讓最小子達陣算了
                int min_node = 7;
                for(int k=0; k<6 ;++k){
                    if(current_cube[ply][k].x!=7){
                        min_node = k+1;
                        break;
                    }
                }
                if(abs(current_board[xx][yy]->num)!=min_node){ //若不會踩到，則走
                    return std::make_pair(num_order[j],DIR_ARRAY[dir_order[j]][i]);
                }  
            } 
        }
    }

    //沒有隨機步，全都是達陣步或踩爆最小子的步時
    if(s_move.first!=15){  
        return s_move;  //有提前達陣步走提前達陣步
    }
    return l_move; //否則走必不勝步 期望平手
};

PII BOARD::special_move(){

    if(current_num_cubes[ply] > 2){
        for( i = 5 ; i >= 0 ; --i){
            if(current_cube[ply][i].x==7) continue;
            for( j = 2 ; j >= 0 ; --j){
                xx = current_cube[ply][i].x+dx[ply][j];
                yy = current_cube[ply][i].y+dy[ply][j];
                if(out(xx,yy)) continue;
                auto target = current_board[xx][yy];
                if((target!=nullptr)&&(((target->num)>>31) != ((current_cube[ply][i].num)>>31))){ // if there is an enemy on move
                    if(j==2 || i==5){ // no way to avoid, just eat
                        return std::make_pair(i,j);
                    }
                    for(int k=0 ; k < 3; ++k){
                        xx = target->x+dx[ply][k];
                        yy = target->y+dy[ply][k];
                        if(out(xx,yy)) continue;
                        auto ambush = current_board[xx][yy]; 
                        if((ambush!=nullptr)&&(abs(target->num)>abs(current_cube[ply][i].num))&&(((ambush->num)>>31) != ((current_cube[ply][i].num)>>31))){
                            // however there is an ambush and my number is smaller than target
                            return std::make_pair(15,15);
                        }
                    }
                    // there is no ambush -> eat
                    return std::make_pair(i,j);
                }
            }
        }
    }
    return std::make_pair(15,15);
};

VII BOARD::special_moves(){
    VII s_moves;
    int xx;
    int yy;
    int ply = int(_turn);
    if(current_num_cubes[ply] > 2 && current_num_cubes[!ply] > 2){
        for(int i = 0; i < 6 ; ++i){
            if(current_cube[ply][i].x==7){
                continue;
            }
            for(int j = 2 ; j >= 0 ; --j){
                xx = current_cube[ply][i].x+dx[ply][j];
                yy = current_cube[ply][i].y+dy[ply][j];
                if(out(xx,yy)){
                    continue;
                }
                auto target = current_board[xx][yy];
                if((target!=nullptr)&&(((target->num)>>31) != ((current_cube[ply][i].num)>>31))){
                    // if there is an enemy can be ate.
                    if(ply){ // however this eat move will reach the goal. then use normal moves
                        if(xx==0 && yy==0){
                            VII useNormalMove;
                            useNormalMove.emplace_back(-1,-1);
                            return useNormalMove;
                        }   
                    }else{
                        if(xx==5 && yy==5){
                            VII useNormalMove;
                            useNormalMove.emplace_back(-1,-1);
                            return useNormalMove;
                        }  
                    }
                    s_moves.emplace_back(i,j); // eat move
                    if(j!=2){
                        // find move to avoid 
                        for(int k=1 ; k>=0 ; --k){
                            xx = current_cube[ply][i].x+dx[ply][k];
                            yy = current_cube[ply][i].y+dy[ply][k];
                            if(out(xx,yy)){
                                continue;
                            }
                            bool canAvoid = true;
                            for(int l=2; l>=0; --l){
                                int xxx = xx+dx[ply][l];
                                int yyy = yy+dy[ply][l];
                                if(out(xxx,yyy)){
                                    continue;
                                }
                                auto guard = current_board[xxx][yyy];
                                if((guard!=nullptr)&&(((guard->num)>>31) != ((current_cube[ply][i].num)>>31))){
                                    //there is a guard on my avoid path
                                    canAvoid = false;
                                    break;
                                }
                            }
                            if(canAvoid==true && j!=k){
                                s_moves.emplace_back(i,k); // avoid move
                            }
                        }
                    }
                }
            }
        }
    }
    if(s_moves.empty()){
        s_moves.emplace_back(-1,-1);
    }
    return s_moves;
};


int* BOARD::get_num_order(){
    return(NUM_ARRAY[num(my_random_engine)]);
};

int* BOARD::get_dir_order(){
    return(NUM_ARRAY[rand_dir(my_random_engine)]);
};

void BOARD::do_move(PII m){
    do_move(&(m.first),&(m.second));
}

void BOARD::do_move(int* num, int* dir ){
    //flog << "do_move" << std::endl;
    if(((*num)==15) && ((*dir)==15)){ //pass
        update();
        steps += 1;
        _turn = !_turn;
        return;
    }

    int ply = int(_turn);
    PII pos = find(&ply, num);
    int now_x = pos.first;
    int now_y = pos.second;
    int new_x = now_x+dx[ply][*dir];
    int new_y = now_y+dy[ply][*dir]; 

    if(current_board[new_x][new_y]!=nullptr){
        //possible eat
        current_board[new_x][new_y]->x = 7; // 換成廢子
        current_board[new_x][new_y]->y = 7; // 換成廢子
        (current_board[new_x][new_y]->num > 0) ? current_num_cubes[1]-- : current_num_cubes[0]--; // 減少子
    }
    //move
    current_board[now_x][now_y]->x = new_x;
    current_board[now_x][now_y]->y = new_y;
    current_board[new_x][new_y] = current_board[now_x][now_y];
    current_board[now_x][now_y] = nullptr;
    //update
    update();
    steps += 1;
    _turn = !_turn;
    return;
};

void BOARD::do_real_move(int* num, int* dir){
    if((*num)==15 && (*dir)==15){ //pass
        _turn = !_turn;
        initial_turn = !initial_turn;
        update();
        initial_winner = _winner;
        steps += 1; // for step base score
        initial_steps = steps; // for step base score
        return;
    }
    int ply = int(_turn);
    PII pos = find(&ply, num);
    int now_x = pos.first;
    int now_y = pos.second;
    int new_x = now_x+dx[ply][*dir];
    int new_y = now_y+dy[ply][*dir];


    
    if(current_board[new_x][new_y]!=nullptr){
        int player = (current_board[new_x][new_y]->num>0)? 1:0;
        int n = abs(current_board[new_x][new_y]->num)-1;
        initial_cube[player][n].x = initial_cube[player][n].y = 7;
        //possible eat
        current_board[new_x][new_y]->x = 7; // 換成廢子
        current_board[new_x][new_y]->y = 7; // 換成廢子
        (current_board[new_x][new_y]->num > 0) ? --initial_num_cubes[1] : --initial_num_cubes[0];
        (current_board[new_x][new_y]->num > 0) ? --current_num_cubes[1] : --current_num_cubes[0]; // 減少子
    }
    //move
    initial_cube[ply][*num].x = new_x;
    initial_cube[ply][*num].y = new_y;
    current_board[now_x][now_y]->x = new_x;
    current_board[now_x][now_y]->y = new_y;
    current_board[new_x][new_y] = current_board[now_x][now_y];
    current_board[now_x][now_y] = nullptr;
    //update
    _turn = !_turn;
    initial_turn = !initial_turn;
    update();
    initial_winner = _winner;
    steps += 1; // for step base score
    initial_steps = steps; // for step base score
};


int BOARD::random_simulate(){
    while(_winner==3){
        
        do_move(get_random_move());
    }
    if(_winner==0){
        return 0;
    }else if(_winner==1){
        return 1;
    }else{
        return 2;
    }
};

