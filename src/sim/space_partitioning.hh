#ifndef __SIM_CACHE_PARTITIONING_HH__
#define __SIM_CACHE_PARTITIONING_HH__

#include <vector>
#include <unordered_map>
#include <string>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include "UCP.hh"
#include "XChange.hh"
#include "MLM.hh"
#include "CPA.hh"

namespace SpacePartitioning{
    extern int getWaysOfCore(int llc, int coreid);//returns the number of ways assigned at the llc for the core

    extern bool isSpacePartitioningOn(void);

    extern bool setSpacePOptions(int Algo, uint64_t intervalSize, int numCores, int numLlcs, int l2Assoc,
     int dssSize, int stackSize,
     double lr, double discnt, int prvt_ways, std::string q_file_path,
     uint64_t l2_size, int max_classes, double icov_thresh);

    extern void changeSpacePartition(uint64_t curTick);

    extern int readCoreId(std::string name);

    extern int getSpacePartitioningAlgo(void);

    //following are used by UCP and MLM
    extern void recordLLCaccess(int llc, int core, uint64_t set, uint64_t address);

    extern int getDssSize(void);

    //folowing are required by the Xchange
    extern void recordHitLatency(int llc, int core, uint64_t latency);

    extern void recordMissLatency(int llc, int core, uint64_t latency);

    extern uint64_t getCpGlobal(int llc, int core);

    extern bool updateCpGlobal(int llc, int core, uint64_t cp_new);

    extern void countTCPU(int core);

    //following are required by MLM
    extern void recordOffChipMsg(int llc);

    //following are required by CPA

    extern std::pair<int,int> getRangeOfCore(int llc, int core);
    
    extern void recordInstruction(int core);

    extern void recordMisses(int llc, int core);

    extern void recordHits(int llc, int core);

    extern void incrementOccupancy(int llc, int core);

    extern void decrementOccupancy(int llc, int core);

}
#endif
