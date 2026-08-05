#ifndef __SIM_REAL_HH__
#define __SIM_REAL_HH__

#include <vector>
#include <unordered_map>
#include <string>
#include <inttypes.h>
#include <cassert>
#include <limits>

namespace real{

    extern void setBWPOptions(uint64_t interSize, int algorithm, int numcores, int numllcs, float ifrth, float imrth, uint64_t dumpStatsAfter);

    extern bool isBandwidthPartitioningOn(void);

    extern uint64_t getDumpStatsAfter(void);

    //returns the CORE whose message must be serviced
    extern int getCoreToServe(int LLC, std::vector<int>& readyCores);

    //calculates the priority of the CPUs in which messages must be serviced
    //stores the priority in an array
    extern void calculatePriority(uint64_t curTick);

    //reset the counters to 0
    void resetCounters();

    //increments the counter for number of LLC accesses that of a core
    extern void incrementLLC(uint64_t physAddr, std::string rob_name);

    //increments the counter for number of non LLC accesses
    extern void incrementNonLLC(uint64_t physAddr, std::string rob_name);

    extern void incrementStoreLLC(uint64_t physAddr, std::string lsq_name);

    extern void incrementStoreNonLLC(uint64_t physAddr, std::string lsq_name);

    //increment instruction fetch that go to the LLC
    extern void incrementILLC(int llc, int core);

    //increment Instruction miss at LLC
    extern void incrementIFM(int llc, int core);

    extern int getCoreFromSequencerName(std::string name);

    extern int getNumCores(void);

    void print(void);

}
#endif
