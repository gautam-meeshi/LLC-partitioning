#include <iostream>
#include <math.h>
#include "space_partitioning.hh"
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <cstring>
/*GAUTAM*/
namespace SpacePartitioning{
    int algo = 0;
    int num_cores =8;
    int num_llcs = 2;
    uint64_t interval_size = 2500000000;//5M
    int l2_assoc = 16;
    uint64_t prevTick = 0;
    int changeIdx =0;
    int dss_size = 64;
    int stack_size = 16;
    //for MLM
    double learning_rate = 0.01;
    double discount = 0.01;
    int private_ways=12;
    //
    std::vector<std::vector<int>> waysOfCore;//
    UCP * ucp_obj = NULL;
    XChange * xchange_obj = NULL;
    MLM * mlm_obj = NULL;
    CPA * cpa_obj = NULL;
    /*
    algo
    0 - LRU
    1 - greedy UCP
    2 - lookahead UCP
    3 - XChange
    4 - MLM
    5 - CPA
    */
    bool setSpacePOptions(int Algo, uint64_t intervalSize, int numCores, int numLlcs,
        int l2Assoc, int dssSize, int stackSize,
        double lr, double discnt, int prvt_ways, std::string q_file_path,
        uint64_t l2_size, int max_classes, double icov_thresh){
        
        algo = Algo;
        interval_size = intervalSize;
        num_cores = numCores;
        num_llcs = numLlcs;
        l2_assoc = l2Assoc;
        dss_size = dssSize;
        stack_size = stackSize;
        learning_rate = lr;
        discount = discnt;
        private_ways = prvt_ways;
        if(algo==1 || algo==2){
            ucp_obj = new UCP(num_cores, num_llcs, l2_assoc, dss_size);
        }else if(algo == 3){
            xchange_obj = new XChange(num_cores, num_llcs, l2_assoc, dss_size, stack_size);
        }else if(algo == 4){
            mlm_obj = new MLM(num_cores, num_llcs, l2_assoc, private_ways, learning_rate, discount, q_file_path);
        }else if(algo == 5){
            cpa_obj = new CPA(num_cores, num_llcs, l2_assoc, l2_size, interval_size, max_classes, icov_thresh);
        }
        std::cout<<"Initializing space partitioning algo = "<<algo<<'\n';
        std::cout<<"\tnum_cores = "<<num_cores<<'\n';
        std::cout<<"\tnum_llcs = "<<num_llcs<<'\n';
        std::cout<<"\tL2 assoc = "<<l2_assoc<<'\n';
        if(algo >0 && algo<4){
            std::cout<<"\tDSS size = "<<dss_size<<'\n';
            std::cout<<"\tDSS stack size = "<<stack_size<<'\n';
        }
        if(algo == 4){
            std::cout<<"\tLearning rate = "<<learning_rate<<'\n';
            std::cout<<"\tDiscount = "<<discount<<'\n';
            std::cout<<"\tPrivate ways = "<<private_ways<<'\n';
        }
        return true;
    }
    void initialize(){//assigns equal ways to each core. Not in use
        assert(l2_assoc>=num_cores);
        for (int i=0; i<num_llcs; i++){
            waysOfCore.push_back(std::vector<int>(num_cores,l2_assoc/num_cores));
        }
    }

    int getWaysOfCore(int llc, int coreid){
        /*if(waysOfCore.size()==0){
            initialize();
        }
        return waysOfCore[llc][coreid];*/
        if(algo==1 || algo==2){
            return ucp_obj->getWaysOfCore(llc, coreid);
        }else if(algo==3){
            return xchange_obj->getWaysOfCore(llc, coreid);
        }else if(algo == 4){
            return mlm_obj->getWaysOfCore(llc,coreid);
        }
        assert(l2_assoc>num_cores);
        return l2_assoc/num_cores;
    }

    bool isSpacePartitioningOn(void){
        if(algo==0){
            return false;
        }
        return true;
    }

    void recordLLCaccess(int llc, int core, uint64_t set, uint64_t address){
        if(algo == 1 || algo==2){
            ucp_obj->recordLLCaccess(llc, core, set, address);
        }else if(algo == 3){
            xchange_obj->recordLLCaccess(llc, core, set, address);
        }
    }

    int getDssSize(void){
        return dss_size;
    }

    int getSpacePartitioningAlgo(void){
        //returns the current algorithm number
        return algo;
    }
    void changeSpacePartition(uint64_t curTick){
        if(algo == 0){//no algorithm
            return;
        }
        if (curTick - prevTick > interval_size){
            std::cout<<curTick/500<<" Changing the partitions \n";
            prevTick = curTick;
            if(algo == 1 || algo ==2){
                ucp_obj->updatePartition(algo);

                for(int llc=0;llc<num_llcs; llc++){
                    std::cout<<"LLC "<<llc<<' ';
                    for(int core=0; core<num_cores; core++){
                        std::cout<<ucp_obj->getWaysOfCore(llc,core)<<' ';
                    }
                    std::cout<<'\n';
                }
            }else if(algo == 3){
                xchange_obj->updatePartition(algo);
                for(int llc=0;llc<num_llcs; llc++){
                    std::cout<<"LLC "<<llc<<' ';
                    for(int core=0; core<num_cores; core++){
                        std::cout<<xchange_obj->getWaysOfCore(llc,core)<<' ';
                    }
                    std::cout<<'\n';
                }
            }else if(algo == 4){
                mlm_obj->updatePartition(algo);
                for(int llc=0;llc<num_llcs; llc++){
                    std::cout<<"LLC "<<llc<<' ';
                    for(int core=0; core<num_cores; core++){
                        std::cout<<mlm_obj->getWaysOfCore(llc,core)<<' ';
                    }
                    std::cout<<'\n';
                }
            }else if(algo == 5){
                cpa_obj->updatePartition(algo);
            }
        }
    }

    void recordHitLatency(int llc, int core, uint64_t latency){
        if(algo==3){
            xchange_obj->recordHitLatency(llc, core, latency);
        }
    }

    void recordMissLatency(int llc, int core, uint64_t latency){
        if(algo==3){
            xchange_obj->recordMissLatency(llc,core, latency);
        }
    }

    uint64_t getCpGlobal(int llc, int core){
        assert(algo == 3);
        return xchange_obj->getCpGlobal(llc,core);
    }

    bool updateCpGlobal(int llc, int core, uint64_t cp_new){
        assert(algo == 3);
        return xchange_obj->updateCpGlobal(llc,core, cp_new);
    }

    void countTCPU(int core){
        assert(algo == 3);
        return xchange_obj->countTCPU(core);
    }

    void recordOffChipMsg(int llc){
        mlm_obj->recordOffChipMsg(llc);
    }

    int readCoreId(std::string name) {		//reading first integer from a string as a core id

        std::vector<char> v(name.length() + 1);
        std::strcpy(&v[0], name.c_str());
        char* p = &v[0];
        long int core = -1;
        while (*p) { // While there are more characters to process...
            if (isdigit(*p)) { // Upon finding a digit, ...
                core = strtol(p, &p, 10); // Read a number, ...
    //	        printf("CPU id %d\n", int(core)); // and print it.
                break;
            }
            else { // Otherwise, move on to the next character.
                p++;
            }
        }
        if(core > 9) {
            core = core - 10;
        }
        if((name.find(".cpu.") != std::string::npos) || (!name.compare("system.cpu"))) {
            return 0;
        }
        return int(core);
    }


    std::pair<int,int> getRangeOfCore(int llc, int core){
        assert(algo==5);//used in case of CPA only
        return cpa_obj->getRangeOfCore(llc, core);
    }
    
    void recordInstruction(int core){
        cpa_obj->recordInstruction(core);
    }

    void recordMisses(int llc, int core){
        cpa_obj->recordMisses(llc,core);
    }

    void recordHits(int llc, int core){
        cpa_obj->recordHits(llc,core);
    }

    void incrementOccupancy(int llc, int core){
        cpa_obj->incrementOccupancy(llc,core);
    }

    void decrementOccupancy(int llc, int core){
        cpa_obj->decrementOccupancy(llc,core);
    }
}







