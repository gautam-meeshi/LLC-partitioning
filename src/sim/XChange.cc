#include "XChange.hh"
#include <cmath>



XChange::XChange(int numcores, int numllcs, int l2assoc, int ucpSetsLen, int stackSize){
    this->num_cores = numcores;
    this->num_llcs = numllcs;
    this->l2_assoc = l2assoc;
    this->ucp_sets_len = ucpSetsLen;
    this->stack_size = stackSize;
    //initialize the recency_list [llc][core][set][l2_assoc]
    recency_list.resize(num_llcs, std::vector<std::vector<std::vector<uint64_t>>>(
        num_cores, std::vector<std::vector<uint64_t>>(ucp_sets_len, std::vector<uint64_t>(stack_size, 0))));

    //initiliaze the hit counters [llc][core][l2_assoc+1] = 0
    ucp_hit_counter.resize(num_llcs, std::vector<std::vector<uint64_t>>(num_cores, std::vector<uint64_t>(stack_size+1,0)));

    //initialize the ways of core
    ways_of_core.resize(num_llcs, std::vector<int>(num_cores,l2_assoc/num_cores));
    hit_counter.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    miss_counter.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    hit_latency.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    miss_latency.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    critical_path_counter.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    t_cpu.resize(num_cores, 0);
    
}

//record the LLC access, shuffle the LRU list accordingly
void 
XChange::recordLLCaccess(int llc, int core, uint64_t set, uint64_t address){
    /*Find, check whether the address is in recency_list[llc][core][set]*/
    int posi=stack_size;
    for(int i=0;i<stack_size;i++){
        if(recency_list[llc][core][set][i] == address){
            posi=i;
            break;
        }
    }    
    
    /*move the other addresses one place to maintain the recency order*/
    if(posi<stack_size){
        for(int i=posi;i>0;i--){
            recency_list[llc][core][set][i] = recency_list[llc][core][set][i-1];
        }
    }else{
        for(int i=posi-1;i>0;i--){
            recency_list[llc][core][set][i] = recency_list[llc][core][set][i-1];
        }
    }
    //put the address at the MRU position
    recency_list[llc][core][set][0] = address;
    ucp_hit_counter[llc][core][posi]++;
}



void XChange::recordHitLatency(int llc, int core, uint64_t latency){
    //below is the averaging operation
    hit_latency[llc][core] = (hit_latency[llc][core]*hit_counter[llc][core]+latency)/(hit_counter[llc][core]+1);
    //increment hit counter
    hit_counter[llc][core]++;
}

void XChange::recordMissLatency(int llc, int core, uint64_t latency){
    miss_latency[llc][core] =  (miss_latency[llc][core]*miss_counter[llc][core] + latency)/(miss_counter[llc][core]+1);
    miss_counter[llc][core]++;
}
//
uint64_t 
XChange::getCpGlobal(int llc, int core){
    return critical_path_counter[llc][core];
}

bool 
XChange::updateCpGlobal(int llc, int core, uint64_t cp_new){
    //uint64_t cp_new = cp_local+delta_t, delta_t = time take for miss to be served from LLC-MEM-LLC
    if(cp_new>critical_path_counter[llc][core]){
        critical_path_counter[llc][core] = cp_new;
        return true;
    }
    return false;
}

void 
XChange::countTCPU(int core){
    //the counter is in cycles - approximation of tcpu
    t_cpu[core]++;
}

//number of ways allocated to the core at llc
int 
XChange::getWaysOfCore(int llc, int core){
    assert(llc>=0 && llc<num_llcs);
    assert(core>=0 && core<num_cores);
    return ways_of_core[llc][core];
}

//update the partition when called
void 
XChange::updatePartition(int method=1){
    printStats();
    for (int llc=0; llc<num_llcs; llc++){
        std::vector<double> budget;
        for (int core=0;core<num_cores;core++){
            budget.push_back(getBudget(llc,core));
        }
        //get budgetSum
        double budgetSum = 0.0;
        for(int core=0;core<num_cores;core++){
            budgetSum+=budget[core];
        }

        //allocate the ways according to the budget
        std::cout<<"At allocation llc "<<llc<<'\n';
        for (int core=0;core<num_cores;core++){
            ways_of_core[llc][core] = std::round(budget[core]*l2_assoc/budgetSum);
            std::cout<<ways_of_core[llc][core]<<','<<budget[core]<<','<<budgetSum<<'\n';
        }

        //SANITY check - sum equals the associativity and each core has atleast 1 way
        int waySum = 0;
        for(int core=0;core<num_cores;core++){
            if(ways_of_core[llc][core] == 0){//if it is zero give it one way
                ways_of_core[llc][core]=1;
            }
            waySum+=ways_of_core[llc][core];
        }
        while(waySum<l2_assoc){
            //the ways allocated does not sum to l2_assoc
            //assign the core have minimum ways one more way
            int core_with_min_ways = -1;
            int min_ways = l2_assoc;
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
            }
        }
        while(waySum > l2_assoc){
            //ways allocated is more than l2_assoc
            //find the core having maximum ways, reduce it by 1
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
            }
        }
        
    }
    resetCounters();
}

double
XChange::getBudget(int llc, int core){
    double t_min = getExeTime(llc, core, l2_assoc);
    double t_max = getExeTime(llc,core,1);
    if(t_max==0.0){
        return 0.01;
    }
    double res = 1.01- (t_min/t_max);
    std::cout<<"Budget "<<llc<<' '<<core<<' '<<t_min<<' '<<t_max<<' '<<res<<'\n';
    if(res<=0){
        res=0.01;
    }
    return res;
}

double
XChange::getCpuTime(int core){
    //assuming the cpu phase time independent of l2 assoc
    return (double)t_cpu[core];
}

double
XChange::getMemTime(int llc, int core, int ways){
    //current memory level parallelism
    if(ways>stack_size){
        //because we have info of stack_size only
        ways=stack_size;
    }
    if(critical_path_counter[llc][core]==0){
        return 0;
    }
    
    double MLP = (double)(hit_counter[llc][core]*hit_latency[llc][core] + miss_counter[llc][core]*miss_latency[llc][core])
    /((double)critical_path_counter[llc][core]);//can take th and tm constant for now
    uint64_t Nh = 0;//number of hits if it was assigned "ways"
    uint64_t Nm = 0;//misses
    for(int way=0;way<=stack_size;way++){
        if(way<ways){
            Nh+=ucp_hit_counter[llc][core][way];
        }else{
            Nm+=ucp_hit_counter[llc][core][way];
        }
    }
    //scale the number of hits and misses to current numbers
    if(Nh+Nm != 0){
        uint64_t total_accesses = hit_counter[llc][core]+miss_counter[llc][core];
        Nh = Nh*total_accesses/(Nh+Nm);
        Nm = total_accesses - Nh;
    }
    if(MLP==0){
        return 0;//No LLC accesses therefore have to say 0 mem time
    }
    double CPnew = (double)(Nh*hit_latency[llc][core] + Nm*miss_latency[llc][core])/(double)MLP;
    std::cout<<"Mem time calc "<<llc<<' '<<core<<' '<<ways<<' '<<Nh<<' '<<Nm<<' '<<MLP<<' '<<CPnew<<'\n';
    return CPnew;
}

double
XChange::getExeTime(int llc,int core, int ways){
    double t_cpu = getCpuTime(core);//estimate of time spent in the cpu phase under current resources
    double t_mem = getMemTime(llc,core,ways);//estimate of time spent in the mem phase under current resources
    return t_cpu+t_mem;
}

void 
XChange::print2Darray(std::vector<std::vector<uint64_t>> &vec, std::string name){
    std::cout<<name<<'\n';
    for (int llc=0;llc<num_llcs;llc++){
        std::cout<<"LLC "<<llc<<' ';
        for(int core=0;core<num_cores;core++){
            std::cout<<vec[llc][core]<<' ';
        }
        std::cout<<'\n';
    }
}

void
XChange::printStats(void){
    print2Darray(critical_path_counter, "Critical path counter/mem time");
    print2Darray(hit_counter, "Hit counter");
    print2Darray(miss_counter, "Miss counter");
    print2Darray(hit_latency, "Hit latency");
    print2Darray(miss_latency, "Miss latency");
    //clear the cpu time
    std::cout<<"CPU TIME ";
    for(int core=0; core<num_cores; core++){
        std::cout<<t_cpu[core]<<' ';
    }
    std::cout<<"\nUCP hit counter\n";
    for(int llc=0; llc<num_llcs; llc++){
        std::cout<<"LLC "<<llc<<'\n';
        for(int core=0; core<num_cores; core++){
            std::cout<<"core "<<core<<' ';
            for(int way=0; way<=stack_size; way++){
                std::cout<<ucp_hit_counter[llc][core][way]<<' ';
            }
            std::cout<<'\n';
        }
    }
}

void
XChange::resetCounters(void){
    //half the ucp_hit_counter
    for (int llc=0; llc<num_llcs; llc++){
        for(int core=0;core<num_cores;core++){
            for (int way=0;way<=stack_size;way++){
                ucp_hit_counter[llc][core][way] = ucp_hit_counter[llc][core][way]/2;
            }
        }
    }
    //clear the critical path counter, hit counter, miss counter, hit latency, miss latency
    for(int llc=0;llc<num_llcs;llc++){
        for(int core=0; core<num_cores; core++){
            critical_path_counter[llc][core]=0;
            hit_counter[llc][core]=0;
            miss_counter[llc][core]=0;
            hit_latency[llc][core]=0;
            miss_latency[llc][core]=0;
        }
    }
    //clear the cpu time
    for(int core=0; core<num_cores; core++){
        t_cpu[core]=0;
    }
    
}