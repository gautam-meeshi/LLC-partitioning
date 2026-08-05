#ifndef __SIM_CPA_HH__
#define __SIM_CPA_HH__

#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <utility>

/*
Defintions -
Behavior - core behavior (sensitive, medium, non-critical, bully, squanderer)
Class - the CLOS, each Class is assigned a range, a class contains multiple cores
Occupancy - Memory consumed by the core at the LLC in bytes

Integer mapping that I have assumed to the behavior
1 - non-critical
2 - sensitive
3 - medium 
4 - bully
5 - squanderer
*/

class CPA{
    public:
        //initializer
        CPA(int numcores, int numllcs, int l2assoc, uint64_t l2Size, uint64_t space_partitioning_interval, int maxClasses, double icovThresh);

        //range of set that the core can use
        std::pair<int,int> getRangeOfCore(int llc, int core);
        
        //update the partition when called
        void updatePartition(int algo);
        
        //records the number of instructions committed - called every time instruction is committed - required to calculate IPC
        void recordInstruction(int core);

        //records the hits of core at llc
        void recordHits(int llc, int core);
        
        //records the hits of core at llc
        void recordMisses(int llc, int core);

        //increment the occupancy of core at llc - occupancy is the actual memory consumed by the core at the LLC in bytes 
        void incrementOccupancy(int llc, int core);//increment on allocate

        //decrement the occupancy of core at llc - occupancy is the actual memory consumed by the core at the LLC in bytes
        void decrementOccupancy(int llc, int core);//decrement on deallocate
        
        void printStats();

        //Class this core belongs
        std::vector<int> getCoresOfClass(int llc, int Class);
        
        //Following are the utility function

        //utility that GAUTAM created to decide whether there are more than 3 critical cores, more than 2 squanderer cores
        int getPartitioningState(int llc);

        //counts the number of cores having the behavior at llc
        int getCountOfBehavior(int llc, int behavior);

        //returns the number of ways assigned to the critical cores
        int getCriticalAssoc(int llc);

    private:
        int num_cores=8;
        int num_llcs=2;
        int l2_assoc=16;
        int max_classes=16;//maximum number of classes allowed
        uint64_t updates =0;
        uint64_t adjusted = 0;//last update index when the partitions were adjusted
        int adjustment_winner = 0;
        double icov_thresh=0.2;
        uint64_t l2_size=4194304;

        double ipc_L = 0.4;
        double ipc_M = 0.4;
        double ipc_VL = 0.2;
        double mpki_VH = 10.0;
        double hpki_VL = 0.5;
        double hpki_VH = 10.0;

        std::vector<bool> first_interval;

        std::vector<bool> update_clos;

        //number of instructions executed by each core - can be counted at the ROB
        std::vector<uint64_t> instructions;

        //number of cycles the core was active - can be counted in CPU.tick
        uint64_t cycles = 0;

        //number of LLC hits of core at each llc
        std::vector<std::vector<uint64_t>> num_llc_hits;
        
        //number of LLC misses of core at each llc
        std::vector<std::vector<uint64_t>> num_llc_misses;

        //maintains the actual occupancy of the core at llc in bytes 
        std::vector<std::vector<uint64_t>> llc_occupancy;

        //IPC of each core
        std::vector<double> ipcs;

        std::vector<std::vector<double>> mpki;

        std::vector<std::vector<double>> hpki;

        //num_cores size vector containing the class number of each core
        std::vector<std::vector<int>> class_of_core;
        
        //1-non_critical, 2-sensitive, 3-medium, 4-bully, 5-squanderer
        std::vector<std::vector<int>> core_behavior;
        //Way range allocated to the class - ranges are inclusive
        std::vector<std::vector<std::pair<int,int>>> range_of_class;
        
        std::vector<double> prev_ipc;
        std::vector<double> running_ipc_average;
        std::vector<uint64_t> times_ipc_recorded;

        std::vector<std::vector<double>> last_10_MPKIs;
        
        //returns true is the ICOV is greater than icov_thresh
        bool isPhaseChange(void);
        
        //returns the current ICOV
        double getICOV(int core);

        //MPKI_H threshold is calculated dynamically based on the MPKIs of the core
        double getMPKI_H(int llc);

        //Returns the behovior of the core
        int getBehaviorOfCore(int llc, int core);

        //The sum of memory occupied by all the critical cores at the llc in bytes
        uint64_t getCriticalOccupancy(int llc);
        
        //The size of non critical region allocated in bytes
        uint64_t getSizeOfNonCriticalRegion(int llc);

        //number of classes used in the LLC
        int getClassesCount(int llc);

        //isolate the non critical apps that are overusing the LLC and making no improvement to a smaller space (2 ways per app)
        void isolateNonCriticalApps(int llc);

        //allocate class to each core depending on the behavior as suggested in the paper
        void allocateClass(int llc);

        //allocate a seperate class for the core
        void allocateExtraClass(int llc, int core);

        //allocate the range to each class in use
        void allocateRanges(int llc);

        //reset the stats counters
        void resetCounters(void);
};

#endif