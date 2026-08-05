#include "MLM.hh"
#include <cmath>
#include <fstream>

//following is a map to facilitate the memoization of states used for generating the all states using dynamic programming
std::map<std::vector<int>, std::vector<std::vector<int>>> all_states;


MLM::MLM(int numcores, int numllcs, int l2assoc, int privateWays, double lr, double discnt, std::string qFilePath){
    this->num_cores = numcores;
    this->num_llcs = numllcs;
    this->l2_assoc = l2assoc;
    this->private_ways = privateWays;
    this->q_file_path = qFilePath;
    this->learning_rate = lr;
    this->discount = discnt;
    save_counter=0;

    initQtable();//doubt will the Qtable receive the latest config values correctly

    dir_access_log.resize(num_llcs, 0);

    //initialize the ways of core
    ways_of_core.resize(num_llcs, std::vector<int>(num_cores,private_ways/num_cores));
    for(int llc=0;llc<num_llcs;llc++){
        ensureSanity(llc);
    }
    latest_action.resize(num_llcs, std::vector<int>(num_cores,0));

    srand(0);
    
}

int sumVector(std::vector<int> & v){
    int n=v.size();
    int tot=0;
    for(int i=0;i<n;i++){
        tot+= v[i];
    }
    return tot;
}


void MLM::getAllStates(int assoc, int cores){
    /*returns all possible actions*/
    std::vector<std::vector<int>> res;
    if(assoc<1 || cores<1 || assoc<cores || all_states.find({assoc, cores})!=all_states.end()){
        return;
    }
    else if(cores==1){
        all_states[{assoc,cores}] = {{assoc}};
    }
    else if(assoc==cores){
        res.push_back(std::vector<int>(cores, 1));
        all_states[{assoc,cores}] = res;
    }else{
        for (int i=1;i<assoc+2-cores;i++){
            if(assoc-i<1 || cores-1<1 || assoc-i<cores-1){
                continue;
            }
            if(all_states.find({assoc-i, cores-1}) == all_states.end()){
                getAllStates(assoc-i, cores-1);
            }
            std::vector<std::vector<int>> sub = all_states[{assoc-i, cores-1}];
            for (int j=0;j<sub.size();j++){
                std::vector<int> temp;
                for (int k=0;k<sub[j].size();k++){
                    temp.push_back(sub[j][k]);
                }
                temp.push_back(i);
                res.push_back(temp);
            }
        }
        all_states[{assoc,cores}] = res;
    }

}

std::vector<std::vector<int>> MLM::getAllActions(int cores){
    /*returns all the actions, some are invalid (do not sum to 0) - have to filtered*/
    std::vector<std::vector<int>> actions;
    if(cores == 1){
        actions.push_back({1});
        actions.push_back({0});
        actions.push_back({-1});
        return actions;
    }else{
        std::vector<std::vector<int>> sub = getAllActions(cores-1);
        for(int i=0;i<sub.size();i++){
            actions.push_back(sub[i]);
            actions.back().push_back(1);
            actions.push_back(sub[i]);
            actions.back().push_back(0);
            actions.push_back(sub[i]);
            actions.back().push_back(-1);
        }
        return actions;
    }
}

void MLM::initQtable(){
    std::cout<<"Inside Qtable constructor "<<num_llcs<<','<<num_cores<<','<<l2_assoc<<private_ways<<'\n';

    getAllStates(private_ways, num_cores);
    std::vector<std::vector<int>>& states = all_states[{private_ways, num_cores}];
    std::cout<<"Total number of states "<<states.size()<<'\n';
    for(int i=0;i<states.size();i++){
        state2int[states[i]] = i;
    }


    std::vector<std::vector<int>> actions = getAllActions(num_cores);
    //remove invalid actions
    std::vector<int> valid_list;
    for(int i=0;i<actions.size();i++){
        if(sumVector(actions[i]) == 0){
            valid_list.push_back(i);
        }
    }
    std::cout<<"Total number of actions "<<valid_list.size()<<'\n';
    for(int i=0;i<valid_list.size();i++){
        action2int[actions[valid_list[i]]] = i;
    }

    //Q table can be initialised with random numbers to start with
    q_table.resize(num_llcs, std::vector<std::vector<int>>(states.size(), std::vector<int>(actions.size(), -600)));
    if(q_file_path==""){
        return;
    }
    std::ifstream qfile;
    qfile.open(q_file_path);
    if(qfile){
        int nl=0;
        int ns=0;
        int na=0;
        std::string data;
        qfile>>data;
        nl = std::stoi(data);
        qfile>>data;
        ns = std::stoi(data);
        qfile>>data;
        na = std::stoi(data);
        if(nl==num_llcs && ns == state2int.size() &&  na == action2int.size()){
            std::cout<<"Loading q file from "<<q_file_path<<'\n';
            for(int llc=0;llc<num_llcs;llc++){
                for(int s=0;s<ns;s++){
                    for(int a=0;a<na;a++){
                        qfile>>data;
                        q_table[llc][s][a] = std::stoi(data);
                    }
                }
            }
        }
    }
}

void MLM::recordOffChipMsg(int llc){
    dir_access_log[llc]++;
}

int MLM::getReward(int llc){
    return -1*dir_access_log[llc];
}

//asks how many ways have to be allowed for the core at llc
int 
MLM::getWaysOfCore(int llc, int core){
    assert(llc>=0 && llc<num_llcs);
    assert(core>=0 && core<num_cores);
    return ways_of_core[llc][core];
}

void 
MLM::ensureSanity(int llc){
    //SANITY check - sum equals the associativity and each core has atleast 1 way
    int waySum = 0;
    for(int core=0;core<num_cores;core++){
        if(ways_of_core[llc][core] == 0){//if it is zero give it one way
            ways_of_core[llc][core]=1;
        }
        waySum+=ways_of_core[llc][core];
    }
    while(waySum<private_ways){
        //assign the minimum one more way
        int core_with_min_ways = -1;
        int min_ways = private_ways;
        for (int core=0;core<num_cores;core++){
            if(ways_of_core[llc][core] < min_ways){
                min_ways = ways_of_core[llc][core];
                core_with_min_ways = core;
            }
        }
        if(core_with_min_ways!=-1){
            ways_of_core[llc][core_with_min_ways]++;
            waySum++;
        }else{
            std::cout<<"PANIC : The waySum is lesser than l2 assoc, no core to add a way\n";
            break;
        }
    }
    while(waySum > private_ways){
        //find the core having ways, reduce it by 1
        int core_with_max_ways = -1;
        int max_ways = 1;
        for (int core=0;core<num_cores;core++){
            if(ways_of_core[llc][core] > max_ways){
                max_ways = ways_of_core[llc][core];
                core_with_max_ways = core;
            }
        }
        if(core_with_max_ways!=-1){
            ways_of_core[llc][core_with_max_ways]--;
            waySum--;
        }else{
            std::cout<<"PANIC : The waySum is greater than l2 assoc and there is no core with more than 1 way allocated\n";
            break;
        }
    }
}

bool MLM::checkSanity(int llc, std::vector<int> action){
    //check whether appyling action will lead to an invalid state (state with a core having <= 0 ways)
    for(int core=0;core<num_cores;core++){
        if(ways_of_core[llc][core]+action[core] <= 0){
            return false;
        }
    }
    return true;
}

std::vector<int> MLM::getAction(int llc, bool force_random){
    /*return the action with max Q given current state or a random action with P=0.3*/
    int best_ai = 0;
    if((double)rand()/(double)RAND_MAX<0.3 || force_random){
        /*RANDOM ACTION - EXPLORATION*/
        best_ai = rand()%action2int.size();
    }else{
        int cur_state = state2int[ways_of_core[llc]];
        int best_q = q_table[llc][cur_state][0];
        //find the maximum Q
        for(int i=0;i<action2int.size();i++){
            if(q_table[llc][cur_state][i] > best_q){
                best_q = q_table[llc][cur_state][i];
            }
        }
        //get all the actions that lead to max q from the current state
        std::vector<int> best_actions;
        for(int i=0;i<action2int.size();i++){
            if(q_table[llc][cur_state][i] == best_q){
                best_actions.push_back(i);
            }
        }
        //Break the tie between best actions randomly
        best_ai = best_actions[rand()%best_actions.size()];
    }
    std::vector<int> res(num_cores,0);
    //select the vector corresponding to the integer selected
    for(auto it: action2int){
        if(it.second == best_ai){
            res = it.first;
            break;
        }
    }
    //returns the best action vector
    return res;
}

//update the partition when called
void 
MLM::updatePartition(int method=4){
    printStats();
    save_counter++;
    if(save_counter%500 == 400){
        save_counter=0;
        saveQtable();
    }
    for (int llc=0; llc<num_llcs; llc++){
        //update a Q table with current statistics
        updateQtable(llc);
        latest_action[llc] = getAction(llc,false);
        while(checkSanity(llc, latest_action[llc])==false){
            //if an invalid action - action that leads to invalid state, reject the action
            int state = state2int[ways_of_core[llc]];
            int action = action2int[latest_action[llc]];
            //mark insane state action pairs with min q value so that they are never selected
            q_table[llc][state][action] = -2000000000;
            //select a new random action
            latest_action[llc] = getAction(llc, true);
        }
        //Apply the action
        for (int core=0;core<num_cores;core++){
            ways_of_core[llc][core]+=latest_action[llc][core];
        }
    }
    resetCounters();
}

void MLM::updateQtable(int llc){
    int qt_1 = state2int[ways_of_core[llc]];//current state
    std::vector<int> prev_state;
    //get previous state by reversing the latest action
    for(int core=0;core<num_cores;core++){
        prev_state.push_back(ways_of_core[llc][core]-latest_action[llc][core]);
    }
    int best_q = q_table[llc][qt_1][0];
    //iterate through all the actions to get the best Q of the given state
    for(int a=0;a<action2int.size();a++){
        if(best_q<q_table[llc][qt_1][a]){
            best_q = q_table[llc][qt_1][a];
        }
    }
    //integer id of previous state
    int qt = state2int[prev_state];
    //integer id of latest action
    int at = action2int[latest_action[llc]];

    //get the value to be added using Q learning formula
    double add_d = (learning_rate * ((double)getReward(llc) + discount*( (double)(best_q - q_table[llc][qt][at]) ) ));
    int add = (int)add_d;
    
    //update the Q table
    q_table[llc][qt][at] = q_table[llc][qt][at]+add;
    //std::cout<<"Q table update "<<llc<<' '<<qt<<' '<<at<<' '<<q_table[llc][qt][at]<<' '<<add_d<<' '<<add<<' '<<getReward(llc)<<' '<<learning_rate<<'\n';
}

void
MLM::resetCounters(void){
    //clear the critical path counter, hit counter, miss counter
    for(int llc=0;llc<num_llcs;llc++){
        dir_access_log[llc]=0;
    }
}

void 
MLM::printStats(void){
    std::cout<<"Off chip access\n";
    for(int llc=0;llc<num_llcs;llc++){
        std::cout<<dir_access_log[llc]<<' ';
    }
    std::cout<<'\n';
}

void
MLM::saveQtable(void){
    if(q_file_path==""){
        return;
    }
    std::cout<<"Saving q table at "<<q_file_path<<'\n';
    std::ofstream qfile;
    qfile.open (q_file_path);
    int ns = state2int.size();
    int na = action2int.size();
    //write num_llcs, num_states, num_actions
    qfile<<num_llcs<<'\n'<<ns<<'\n'<<na<<'\n';
    
    //write q table line by line
    for(int llc=0;llc<num_llcs;llc++){
        for(int s=0;s<ns;s++){
            for(int a=0;a<na;a++){
                qfile<<q_table[llc][s][a]<<'\n';
            }
        }
    }
    qfile.close();
}