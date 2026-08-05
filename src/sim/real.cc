#include <vector>
#include <unordered_map>
#include <iostream>
#include <math.h>
#include <string>
#include "real.hh"
#include <algorithm>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <cstdlib>
#include <utility>
/*GAUTAM*/
// extern uint64_t periodicDump = 50000000;
// extern uint64_t period = 50000000;

namespace real{

    uint64_t dump_stats_after = 50000000;

    int num_cores = 8;
    int num_LLCs = 4;
    float IFRth=0.02;
    float IMRth = 0.5;
    int interval_size = 700000*500;//specified in ticks

    const int priority_vector_size = 8;
    float priorities[priority_vector_size] = {1,2,3,4,5,6,7,8};


    int bandwidth_algo = 0;



    /*ADD COREID AND TYPE OF REQUEST TO THE MESSAGE OBJECT TO DIFFERENTIATE*/
    uint64_t prevWakeUpTick=0;
    bool First = true;
    bool slotsInitialised = false;
    //float LLC_acceses[num_LLCs][num_cores];//number of LLC accesses includes - read,write,ifetch
    std::vector<std::vector<double>> LLC_acceses;
    //float non_LLC[num_cores];//number of non LLC accesses - rob pops that happened very fast
    std::vector<double> non_LLC;
    //float ILLC[num_LLCs][num_cores];//number of ifetch that touched the LLC (includes DRAM)
    std::vector<std::vector<double>> ILLC;
    //float IFM[num_LLCs][num_cores];//number of ifetch that touched the DRAM
    std::vector<std::vector<double>> IFM;
    //float storeLLC[num_LLCs][num_cores];//number of stores that touched the LLC
    std::vector<std::vector<double>> storeLLC;
    //float storeNonLLC[num_cores];//number of stores that might not have touched LLC
    std::vector<double> storeNonLLC;

    std::vector<std::vector<std::pair<int,int>>> allocated_slots;//[num_LLCs]priority slots (core, priority) vector for each llc
    std::vector<std::vector<std::pair<int,int>>> remaining_slots;
    //std::vector<std::vector<int>> far;//New addition

    //STATS counters
    /*
    uint64_t prevLLCprintTime = 0;
    uint64_t BufferSizeSum[num_LLCs];
    uint64_t timesMessageBufferSizeCalled[num_LLCs];
    uint64_t LLCmisses[num_LLCs];
    uint64_t waitingTimeSum[num_LLCs];
    uint64_t timesWaitingTimeReported[num_LLCs];
    uint64_t maxQueueLength[num_LLCs];
    */

    void init_counters(void){
        LLC_acceses.resize(num_LLCs, std::vector<double>(num_cores, 0));
        non_LLC.resize(num_cores, 0);
        ILLC.resize(num_LLCs, std::vector<double>(num_cores, 0));
        IFM.resize(num_LLCs, std::vector<double>(num_cores, 0));
        storeLLC.resize(num_LLCs, std::vector<double>(num_cores, 0));
        storeNonLLC.resize(num_cores, 0);
        allocated_slots.resize(num_LLCs, std::vector<std::pair<int,int>>(num_cores));
        remaining_slots.resize(num_LLCs, std::vector<std::pair<int,int>>(num_cores));
    }

    void setBWPOptions(uint64_t interSize, int algorithm, int numcores, int numllcs, float ifrth, float imrth, uint64_t dumpStatsAfter) {
        interval_size = interSize*500;
        bandwidth_algo = algorithm;
        num_cores = numcores;
        num_LLCs = numllcs;
        IFRth = ifrth;
        IMRth = imrth;
        dump_stats_after = dumpStatsAfter;
        init_counters();
        std::cout<<"Bandwidth algo "<<bandwidth_algo<<'\n';
        
    }

    int getNumCores(void){
        return num_cores;
    }

    uint64_t getDumpStatsAfter(void){
        return dump_stats_after;
    }

    bool isBandwidthPartitioningOn(void){
        if(bandwidth_algo==0){
            return false;
        }
        return true;
    }

    bool comparator(std::pair<int,int> a, std::pair<int,int> b){//to sort the cores and slots
        if(a.second > b.second){
            return true;
        }
        return false;
    }

    //reset the counters to 0

    void resetCounters(){
        for(int llc=0;llc<num_LLCs;llc++){
            for(int i=0;i<num_cores;i++){
                LLC_acceses[llc][i]=0;//number of LLC accesses
                non_LLC[i]=0;//number of non LLC accesses
                ILLC[llc][i]=0;//number of instruction LLC
                IFM[llc][i]=0;
                storeLLC[llc][i]=0;
                storeNonLLC[i]=0;
            }
        }
    }

    void resetSlots(){
        for(int llc=0;llc<num_LLCs;llc++){
            remaining_slots[llc] = allocated_slots[llc];
        }
    }

    void initSlots(){
        /*give every one 1 slot*/
        for(int llc=0;llc<num_LLCs;llc++){
            for(int core=0;core<num_cores;core++){
                allocated_slots[llc].push_back({core,2});
            }
        }
        resetSlots();
        /*New addition
        far.push_back({ 2, 3, 5, 6, 7 });//LLC0
        far.push_back({ 3, 4, 6, 7 });
        far.push_back({ 0, 4, 5, 7 });
        far.push_back({ 0, 1, 4, 5, 6 });
        far.push_back({ 1, 2, 3, 6, 7 });
        far.push_back({ 0, 2, 3 ,7});
        far.push_back({ 0, 1, 3, 4 });
        far.push_back({ 0, 1, 2, 4, 5 });
        New addition*/
    }

    int getCoreToServe(int LLC, std::vector<int>& readyCores){
        //returns the CORE whose message must be serviced
        /*ALWAYS START FROM BEGINNING CONSUME ALL THE REQUESTS OF A CORE BEFORE MOVING TO NEXT*/
        /*KEEP THE HIGH PRIORITY SLOTS FIRST*/
        if(!slotsInitialised){
            initSlots();
            slotsInitialised=true;
        }
        int res=-1;
        for(int i=0; i<num_cores; i++){
            if(remaining_slots[LLC][i].second > 0 
            && std::find(readyCores.begin(), readyCores.end(), remaining_slots[LLC][i].first)!=readyCores.end()){//has slots and is ready
                res = remaining_slots[LLC][i].first;
                remaining_slots[LLC][i].second = remaining_slots[LLC][i].second-1;
                break;
            }
        }
        if(res!=-1){//res non zero means it found a file
            return res;
        }else{
            //all the slots are exhausted, reset and re get the core to serve
            resetSlots();
            for(int i=0; i<num_cores; i++){
                if(remaining_slots[LLC][i].second > 0 
                && std::find(readyCores.begin(), readyCores.end(), remaining_slots[LLC][i].first)!=readyCores.end()){
                    res = remaining_slots[LLC][i].first;
                    remaining_slots[LLC][i].second = remaining_slots[LLC][i].second-1;
                    break;
                }
            }
            return res;
        }
    }


    //calculates the priority of the CPUs in which messages must be serviced
    //stores the priority in an array
    void calculatePriority(uint64_t curTick){
        if(First){
            resetCounters();
            First=false;
        }
        if(curTick - prevWakeUpTick < interval_size){
            return;
        }
        //std::cout<<"calculate "<<curTick <<',' << prevWakeUpTick<<'\n';
        prevWakeUpTick=curTick;
        for (int llc=0; llc<num_LLCs;llc++){
        //calculate the geometric mean of ISLs
            allocated_slots[llc].clear();
            //allocated_slots[llc] = std::vector<std::pair<int,int>>();
            float product = 1;
            for(int i=0;i<num_cores;i++){
                if(LLC_acceses[llc][i] > 0){
                    product*=non_LLC[i]/LLC_acceses[llc][i];
                }
            }
            float exp = (float)num_cores;
            exp = 1/exp;
            float ISL_GM = pow(product, exp);
            for(int core=0;core<num_cores;core++){
                //give higher priority to higher ISL=nonLLC/LLC
                //give higher priority to higher IFR
                //give higher priority to lower IMR
                int pridx = priority_vector_size-1;//define k
                float ISL = 0;
                if(LLC_acceses[llc][core]>0){
                    ISL=non_LLC[core]/LLC_acceses[llc][core];
                }
                if(ISL_GM>0){
                    while(ISL/ISL_GM < pridx && pridx>0){
                        pridx--;
                    }
                }
                float IFR = 0;
                if(LLC_acceses[llc][core]){
                    IFR = ILLC[llc][core]/LLC_acceses[llc][core];
                }
                if(IFR> IFRth){
                    float IMR = 0;
                    if(ILLC[llc][core] > 0){
                        IMR=IFM[llc][core]/ILLC[llc][core];
                    }
                    if(IMR> IMRth){
                        pridx++;
                    }else{
                        pridx=priority_vector_size-1;
                    }
                }
                /*GAUTAM
                if(pridx<2 && std::find(far[llc].begin(), far[llc].end(), core)!=far[llc].end()){//if it is a far LLC prioritize it, given it does not have high priority
                    pridx++;
                }
                GAUTAM*/
                allocated_slots[llc].push_back({core, priorities[pridx]});
            }
            std::sort(allocated_slots[llc].begin(), allocated_slots[llc].end(), comparator);
        }
        resetSlots();
        //print();
        resetCounters();
    }



    int getCoreFromROBName(std::string name){//needs special scrutiny
        int p = name.find(".rob");
        if(p== std::string::npos){
            throw std::invalid_argument("getCoreFromROBName unexpected argument = "+name);
            return -1;
        }else{
            try{
                return std::stoi(name.substr(p-1,1));
            }catch(...){
                throw std::invalid_argument( "Invalid argument stoi ROB name " +name );
            }
        }
        return -1;
    }

    int getCoreFromLSQName(std::string name){
        int p = name.find(".iew.lsq.");
        if(p== std::string::npos){
            throw std::invalid_argument("getCoreFromLSQName unexpected argument = "+name);
            return -1;
        }else{
        try{
                return std::stoi(name.substr(p-1,1));
            }catch(...){
                throw std::invalid_argument( "Invalid argument getCoreFromLSQName " +name );
            }
        }
        return -1;
    }

    int getCoreFromFetchName(std::string name){
        int p = name.find(".fetch");
        if(p== std::string::npos){
            throw std::invalid_argument("getCoreFromFetchName unexpected argument = "+name);
            return -1;
        }else{
            try{
                return std::stoi(name.substr(p-1,1));
            }catch(...){
                throw std::invalid_argument( "Invalid argument getCoreFromFetchName " +name );
            }
        }
        return -1;
    }

    int getCoreFromSequencerName(std::string name){
        int p = name.find(".sequencer");
        if(p== std::string::npos){
            throw std::invalid_argument("getCoreFromFetchName unexpected argument = "+name);
            return -1;
        }else{
            try{
                return std::stoi(name.substr(p-1,1));
            }catch(...){
                throw std::invalid_argument( "Invalid argument getCoreFromFetchName " +name );
            }
        }
        return -1;
    }

    //increments the counter for number of LLC accesses that of a core
    void incrementLLC(uint64_t physAddr, std::string rob_name){
        int llc = (physAddr/64)%num_LLCs;
        int core = getCoreFromROBName(rob_name);
        if(core==-1){
            std::cout<<"could not find the CPU index rob name "<<rob_name<<'\n';//let the code break
        }
        LLC_acceses[llc][core]++;
    }

    //increments the counter for number of non LLC accesses
    void incrementNonLLC(uint64_t physAddr, std::string rob_name){
        //int llc = (physAddr/64)%num_LLCs;
        int core = getCoreFromROBName(rob_name);
        if(core==-1){
            std::cout<<"could not find the CPU index rob name "<<rob_name<<'\n';//let the code break
        }
        non_LLC[core]++;
    }

    void incrementStoreLLC(uint64_t physAddr, std::string lsq_name){
        int llc = (physAddr/64)%num_LLCs;
        int core = getCoreFromLSQName(lsq_name);
        //std::cout<<"StoreLLC "<<llc<<core<<'\n';
        if(core==-1){
            std::cout<<"could not find the CPU index lsq name "<<lsq_name<<'\n';//let the code break
        }
        LLC_acceses[llc][core]++;
        storeLLC[llc][core]++;
    }

    void incrementStoreNonLLC(uint64_t physAddr, std::string lsq_name){
        //int llc = (physAddr/64)%num_LLCs;
        int core = getCoreFromLSQName(lsq_name);
        //std::cout<<"Store non LLC "<<core<<'\n';
        if(core==-1){
            std::cout<<"could not find the CPU index lsq name "<<lsq_name<<'\n';//let the code break
        }
        non_LLC[core]++;
        storeNonLLC[core]++;
    }


    //increment instruction fetch that go to the LLC
    void incrementILLC(int llc, int core){
        if(core==-1){
            std::cout<<"could not find the core : "<<core<<'\n';//let the code break
        }
        ILLC[llc][core]++;
        LLC_acceses[llc][core]++;
    }

    //increment Instruction miss at LLC
    void incrementIFM(int llc, int core){
        if(core==-1){
            std::cout<<"could not find  the core : "<<core<<'\n';//let the code break
        }
        IFM[llc][core]++;
    }

    void print(void){
        std::cout<<"LLC accesses \n";
        for(int i=0;i<num_LLCs;i++){
        for(int c=0; c<num_cores;c++){
        std::cout<<LLC_acceses[i][c]<<',';
        }
        std::cout<<'\n';
    }
    std::cout<<"non LLC accesses \n";
    for(int c=0; c<num_cores;c++){
        std::cout<<non_LLC[c]<<',';
        }
        std::cout<<'\n';
    std::cout<<"ILLC accesses \n";
        for(int i=0;i<num_LLCs;i++){
        for(int c=0; c<num_cores;c++){
        std::cout<<ILLC[i][c]<<',';
        }
        std::cout<<'\n';
    }
    std::cout<<"IFM accesses \n";
        for(int i=0;i<num_LLCs;i++){
        for(int c=0; c<num_cores;c++){
        std::cout<<IFM[i][c]<<',';
        }
        std::cout<<'\n';
    }
    std::cout<<"StoreLLC accesses \n";
        for(int i=0;i<num_LLCs;i++){
        for(int c=0; c<num_cores;c++){
        std::cout<<storeLLC[i][c]<<',';
        }
        std::cout<<'\n';
    }
    std::cout<<"Store non LLC accesses \n";
        for(int c=0; c<num_cores;c++){
        std::cout<<storeNonLLC[c]<<',';
        }
        std::cout<<'\n';

    std::cout<<"priorities\n";
        for(int i=0;i<num_LLCs;i++){
            for(int c=0; c<num_cores;c++){
                std::cout<<allocated_slots[i][c].first<<','<<allocated_slots[i][c].second << '|';
            }
            std::cout<<'\n';
        }
    }

}