#ifndef __SIM_XCHANGE_HH__
#define __SIM_XCHANGE_HH__

#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cstdlib>

class XChange{
    public:
        //initializer
        XChange(int numcores, int numllcs, int l2assoc, int ucpSetsLen, int stackSize);

        //record the LLC access, shuffle the LRU list accordingly
        void recordLLCaccess(int llc, int core, uint64_t set, uint64_t address);
        
        //record the hit latency and the count the number of hits
        void recordHitLatency(int llc, int core, uint64_t latency);

        void recordMissLatency(int llc, int core, uint64_t latency);

        //cp - critical path, approximation of tmem - accumulation of time consumed by the requests from LLC to mem to LLC
        uint64_t getCpGlobal(int llc, int core);

        //updates the critical path counter
        bool updateCpGlobal(int llc, int core, uint64_t cp_new);

        //count the instructions, approximate the t_cpu
        void countTCPU(int core);

        //asks how many ways have to allowed for the core at llc
        int getWaysOfCore(int llc, int core);
        
        //update the partition when called
        void updatePartition(int algo);
        
        //following are utility functions to print the stats
        void print2Darray(std::vector<std::vector<uint64_t>> &vec, std::string name);

        void printStats(void);
    private:
        int num_cores;
        int num_llcs;
        int l2_assoc;
        
        //number of sets to consider in DSS
        int ucp_sets_len;
        //the depth (number of addresses) in each dss size, can be different from l2_assoc, preferably less than l2_assoc
        int stack_size;
        //per LLC, per core
        std::vector<std::vector<uint64_t>> critical_path_counter;//must be set to 0 at the interval

        //4D vector accessed by [llc][core][set][stack_size] maintains the addresses accessed by each core at llc, set
        std::vector<std::vector<std::vector<std::vector<uint64_t>>>> recency_list;
        
        //ways that need to be assigned to the cores
        std::vector<std::vector<int>> ways_of_core;
        
        //hit counter for each [llc][core][stack_size+1], +1 is for misses 
        std::vector<std::vector<std::vector<uint64_t>>> ucp_hit_counter;

        //number of hits for [llc][core]
        std::vector<std::vector<uint64_t>> hit_counter;
        
        //number of misses for [llc][core]
        std::vector<std::vector<uint64_t>> miss_counter;
        
        //hit latency for [llc][core]
        std::vector<std::vector<uint64_t>> hit_latency;
        
        //miss latency for [llc][core]
        std::vector<std::vector<uint64_t>> miss_latency;

        std::vector<uint64_t> t_cpu;

        /*utility is the defined as 1- tmin/tmax, 
        tmin - exe time when all the ways are allocated
        tmax - exe time whan only one way is allocated
        this requires estimate of misses and hits for the given ways and the average time consumed in each case
        */
        //here budget is equivalent to the % ways allocated
        double getBudget(int llc, int core);

        double getCpuTime(int core);

        double getMemTime(int llc, int core, int ways);

        double getExeTime(int llc,int core, int ways);//cpu_time+mem_time estimate for this combination

        void resetCounters(void);

};


#endif