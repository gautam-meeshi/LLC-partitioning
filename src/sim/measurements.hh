#ifndef __SIM_MEASUREMENTS_HH__
#define __SIM_MEASUREMENTS_HH__

#include <vector>
#include <unordered_map>
#include <string>
#include <inttypes.h>
#include <cassert>
#include <limits>

namespace measurements{

    extern void setMeasurementsOptions(int cores, int llcs, int llc_dump_interval);

    extern void measure(uint64_t curTick);

    extern void LLCRequestQueueSize(int llc, int size);

    extern void countWaitingTime(int llc, uint64_t time);

    extern void countLLCmisses(int llc, int core);

    extern void countLLChits(int llc, int core);

    extern void countLLCRequests(int llc, int core);

    extern void countDirRequests(int llc, int core);

    extern void countResponsesReceived(int llc);

    extern void countUnblocks(int llc);

    void LLCstatsPrint(void);

}
#endif
