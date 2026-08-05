#ifndef __SIM_UCP_HH__
#define __SIM_UCP_HH__

#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cstdlib>



class UCP{
    public:
        //initializer
        UCP(int numcores, int numllcs, int l2assoc, int ucpSetsLen);

        //record the LLC access, shuffle the LRU list accordingly
        void recordLLCaccess(int llc, int core, uint64_t set, uint64_t address);

        //returns the number of ways allocated to the core at llc
        int getWaysOfCore(int llc, int core);
        
        //update the partition when called
        void updatePartition(int algo);
        
        //UCP lookahead algorithm - Decides the number of ways to be given to each core
        void lookAheadAlgorithm(void);
        
        //UCP greedy algorithm - Decides the number of ways to be given to each core
        void greedyAlgorithm(void);

        void printHitCounter(void);
    private:
        int num_cores;
        int num_llcs;
        int l2_assoc;
        
        //number of sets to consider in DSS
        int ucp_sets_len;

        //4D vector [llc][core][set][l2_assoc] maintains the addresses accessed by each core at llc, set
        std::vector<std::vector<std::vector<std::vector<uint64_t>>>> recency_list;
        
        //number of ways allocated to each core
        std::vector<std::vector<int>> ways_of_core;
        
        //hit counter for each [llc][core][l2_assoc+1], +1 is for misses 
        std::vector<std::vector<std::vector<uint64_t>>> hit_counter;

        //Returns the utility of increasing the allocation from low to high for the core at llc
        uint64_t getUtilValue(int llc, int core, int low, int high);

        //Returns the marginal utility of increasing the allocation from low to high for the core at llc
        uint64_t getMuValue(int llc, int core, int low, int high);

        //Returns the maximum marginal utility for core at llc by iterating through all the marginal utilities
        std::vector<uint64_t> getMaxMu(int llc, int core,int cur_ways, int remaining_ways);

};

#endif