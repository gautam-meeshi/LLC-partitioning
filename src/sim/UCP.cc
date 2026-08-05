#include "UCP.hh"


UCP::UCP(int numcores, int numllcs, int l2assoc, int ucpSetsLen){
    this->num_cores = numcores;
    this->num_llcs = numllcs;
    this->l2_assoc = l2assoc;
    this->ucp_sets_len = ucpSetsLen;
    //initialize the recency_list [llc][core][set][l2_assoc]
    recency_list.resize(num_llcs, std::vector<std::vector<std::vector<uint64_t>>>(
        num_cores, std::vector<std::vector<uint64_t>>(ucp_sets_len, std::vector<uint64_t>(l2_assoc, 0))));

    //initiliaze the hit counters [llc][core][l2_assoc+1] = 0
    hit_counter.resize(num_llcs, std::vector<std::vector<uint64_t>>(num_cores, std::vector<uint64_t>(l2_assoc+1,0)));

    //initialize the ways of core - initially each core is assigned equal number of ways at each llc - ASSUMPTION - l2_assoc DIVISIBLE BY num_cores
    ways_of_core.resize(num_llcs, std::vector<int>(num_cores,l2_assoc/num_cores));
}

//record the LLC access, put the MRU address at the front, shift the other addresses to the right
void 
UCP::recordLLCaccess(int llc, int core, uint64_t set, uint64_t address){
    /*Find check whether the address is in recency_list[llc][core][set]*/
    int posi=l2_assoc;
    for(int i=0;i<l2_assoc;i++){
        if(recency_list[llc][core][set][i] == address){
            posi=i;
            break;
        }
    }
    
    //move the addresses towards right by one place in the recency list
    if(posi<l2_assoc){
        for(int i=posi;i>0;i--){
            recency_list[llc][core][set][i] = recency_list[llc][core][set][i-1];
        }
    }else{
        for(int i=posi-1;i>0;i--){
            recency_list[llc][core][set][i] = recency_list[llc][core][set][i-1];
        }
    }
    //keep the address at the MRU position
    recency_list[llc][core][set][0] = address;
    hit_counter[llc][core][posi]++;
}

//returns the number of ways allocated to the core at llc
int 
UCP::getWaysOfCore(int llc, int core){
    assert(llc>=0 && llc<num_llcs);
    assert(core>=0 && core<num_cores);
    return ways_of_core[llc][core];
}

//update the partition when called - this is called once in the space partitioning interval duration
void 
UCP::updatePartition(int method=1){
    if(method==1){
        greedyAlgorithm();
    }
    else if(method==2){
        lookAheadAlgorithm();
    }else{
        return;
    }
    printHitCounter();
    for (int llc=0;llc<num_llcs;llc++){
        for(int core=0;core<num_cores;core++){
            for(int way=0;way<=l2_assoc;way++){
                hit_counter[llc][core][way] = hit_counter[llc][core][way]/2;
            }
        }
    }
}

uint64_t 
UCP::getUtilValue(int llc, int core, int low, int high){
    //number of misses reduced by increasing the associativity from low to high for the core, llc
    assert(high<=l2_assoc);
    uint64_t misses_reduction = 0;
    if(high>l2_assoc){
        std::cout<<"high is out of bound UCP.cc line 81";
    }
    for(int i=low+1;i<=high;i++){
        misses_reduction += hit_counter[llc][core][i];
    }
    return misses_reduction;
}

uint64_t 
UCP::getMuValue(int llc, int core, int low, int high){
    //Marginal utility - UtilValue normalized by the number of ways
    assert(low<high);
    return getUtilValue(llc, core, low, high)/(high-low);
}

std::vector<uint64_t> 
UCP::getMaxMu(int llc, int core,int cur_ways, int remaining_ways){
    //find the max marginal utility by iterating through all the possible allocations
    uint64_t max_mu=0;
    uint64_t max_i=1;
    for(int i=1;i<=remaining_ways;i++){
        int mu = getMuValue(llc,core,cur_ways, cur_ways+i);
        if(mu > max_mu){
            max_mu = mu;
            max_i=i;
        }
    }
    std::vector<uint64_t> res = {max_i, max_mu};
    return res;
}

//implementation of lookahead algorithm given in UCP paper
void 
UCP::lookAheadAlgorithm(void){

    //for each llc
    for(int llc=0;llc<num_llcs;llc++){
        //assign each core 1 way 
        for(int core=0;core<num_cores;core++){
            ways_of_core[llc][core]=1;
        }
        int remaining_ways = l2_assoc-num_cores;
        assert(remaining_ways>=0);
        while(remaining_ways>0){
            std::vector<std::vector<uint64_t>> blocks;
            for(int core=0;core<num_cores;core++){
                uint64_t cur_ways = ways_of_core[llc][core];
                blocks.push_back(getMaxMu(llc, core, cur_ways, remaining_ways));
            }
            uint64_t max_mu=0;
            uint32_t min_ways_to_max = l2_assoc;
            //find the maximum marginal utility
            for(int core=0;core<num_cores;core++){
                if(blocks[core][1]> max_mu){
                    max_mu=blocks[core][1];
                }
            }
            //find the minimum number of ways that will be possible for maximum marginal utility
            for(int core=0;core<num_cores;core++){
                if(blocks[core][1]==max_mu){
                    if(blocks[core][0] < min_ways_to_max){
                        min_ways_to_max = blocks[core][0];
                    }
                }
            }
            std::vector<int> candidates;
            //find the winner - cores that have maximum marginal utility
            for(int core=0;core<num_cores;core++){
                if(blocks[core][1]==max_mu && blocks[core][0] == min_ways_to_max){
                    candidates.push_back(core);
                }
            }
            //pick the core that has the least number of ways
            std::vector<int> cores_with_less_ways;
            int min_ways = l2_assoc;
            for (auto core: candidates){
                if (ways_of_core[llc][core] < min_ways){
                    min_ways = ways_of_core[llc][core];
                }
            }
            for (auto core: candidates){
                if(ways_of_core[llc][core] == min_ways){
                    cores_with_less_ways.push_back(core);
                }
            }
            /*Arbitrarily break the tie*/
            int winner = cores_with_less_ways[std::rand()%cores_with_less_ways.size()];
            ways_of_core[llc][winner] += blocks[winner][0];
            remaining_ways -= blocks[winner][0];
        }
    }
}



//implementation of greedy algorithm given in UCP paper
void 
UCP::greedyAlgorithm(void){
    /*For each LLC*/

    for(int llc=0; llc<num_llcs; llc++){
        int remaining_ways = l2_assoc;
        /*Give each core 1 way atleast*/
        for(int core=0;core<num_cores;core++){
            ways_of_core[llc][core]=1;
            remaining_ways--;
        }
        /*There should be few remaining ways
        This will definitely fail when l2_assoc<num_cores*/
        assert(remaining_ways>=0);
        
        /*allocate ways greedily*/
        while(remaining_ways>0){
            int max_gain=0;
            std::vector<std::vector<int>> participants;
            for(int core=0;core<num_cores;core++){
                int cur_ways = ways_of_core[llc][core];
                int unext = getUtilValue(llc,core, cur_ways, cur_ways+1);
                participants.push_back({core,unext});
                if(unext>max_gain){
                    max_gain=unext;
                }
            }
            std::vector<int> cores_with_maxu;
            for(int i=0;i<participants.size();i++){
                if(participants[i][1] == max_gain){
                    cores_with_maxu.push_back(participants[i][0]);
                }
            }
            /*Choose the core with lesser ways*/
            std::vector<int> cores_with_less_ways;
            int min_ways = l2_assoc;
            for (auto core: cores_with_maxu){
                if (ways_of_core[llc][core] < min_ways){
                    min_ways = ways_of_core[llc][core];
                }
            }
            for (auto core: cores_with_maxu){
                if(ways_of_core[llc][core] == min_ways){
                    cores_with_less_ways.push_back(core);
                }
            }
            /*Arbitrarily break the tie*/
            int winner = cores_with_less_ways[std::rand()%cores_with_less_ways.size()];
            
            ways_of_core[llc][winner]++;
            remaining_ways--;
        }
    }
}

void
UCP::printHitCounter(void){
    for(int llc=0; llc<num_llcs; llc++){
        std::cout<<"LLC "<<llc<<'\n';
        for(int core=0; core<num_cores; core++){
            std::cout<<"core "<<core<<' ';
            for(int way=0; way<=l2_assoc; way++){
                std::cout<<hit_counter[llc][core][way]<<' ';
            }
            std::cout<<'\n';
        }
    }
}