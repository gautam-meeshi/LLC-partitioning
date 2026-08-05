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
// extern uint64_t dumpStatsAfter = 20000000;
namespace measurements{

    uint64_t LLCstatsDumpInterval = 100000*500;//2M cycles
    int num_cores = 8;
    int num_LLCs = 4;


    //STATS counters
    uint64_t prevLLCprintTime = 0;
    std::vector<uint64_t> BufferSizeSum;
    std::vector<uint64_t> timesMessageBufferSizeCalled;
    std::vector<std::vector<uint64_t>> LLCmisses;
    std::vector<std::vector<uint64_t>> LLCrequests;
    std::vector<std::vector<uint64_t>> dirRequests;
    std::vector<std::vector<uint64_t>> LLChits;
    std::vector<uint64_t> waitingTimeSum;
    std::vector<uint64_t> timesWaitingTimeReported;
    std::vector<uint64_t> maxQueueLength;
    std::vector<uint64_t> unblocks;
    std::vector<uint64_t> responses;


    void setMeasurementsOptions(int cores, int llcs, int llc_dump_interval){
        num_cores = cores;
        num_LLCs = llcs;
        LLCstatsDumpInterval = llc_dump_interval;
        BufferSizeSum.resize(llcs,0);
        timesMessageBufferSizeCalled.resize(llcs, 0);
        waitingTimeSum.resize(llcs,0);
        timesWaitingTimeReported.resize(llcs,0);
        maxQueueLength.resize(llcs,0);
        unblocks.resize(llcs,0);
        responses.resize(llcs,0);
        for (int i=0;i<llcs;i++){
            LLCmisses.push_back(std::vector<uint64_t>(cores, 0));
            LLCrequests.push_back(std::vector<uint64_t>(cores, 0));
            dirRequests.push_back(std::vector<uint64_t>(cores, 0));
            LLChits.push_back(std::vector<uint64_t>(cores, 0));
        }
    }



    //calculates the priority of the CPUs in which messages must be serviced
    //stores the priority in an array
    void LLCstatsPrint(void){
        std::cout<<"Average LLC request queue size\n";
        for (int i=0;i<num_LLCs;i++){
            std::cout<<(double)BufferSizeSum[i]/timesMessageBufferSizeCalled[i]<<' ';
            BufferSizeSum[i]=0;
            timesMessageBufferSizeCalled[i]=0;
        }
        std::cout<<"\nMaximum queue length\n";
        for (int i=0;i<num_LLCs;i++){
            std::cout<<maxQueueLength[i]<<' ';
            maxQueueLength[i]=0;
        }
        std::cout<<"\nAverage waiting time\n";
        for (int i=0;i<num_LLCs;i++){
            std::cout<<(double)waitingTimeSum[i]/timesWaitingTimeReported[i]/500<<' ';
            waitingTimeSum[i]=0;
            timesWaitingTimeReported[i] =0;
        }
        std::cout<<"\nNumber of unblock messages\n";
        for (int i=0;i<num_LLCs;i++){
            std::cout<<unblocks[i]<<' ';
            unblocks[i]=0;
        }
        std::cout<<"\nNumber of DRAM responses\n";
        for (int i=0;i<num_LLCs;i++){
            std::cout<<responses[i]<<' ';
            responses[i]=0;
        }
        std::cout<<"\nnumber of LLC requests\n";
        for (int core =0; core<num_cores; core++){
            std::cout<<"CORE "<<core<<' ';
            for (int i=0;i<num_LLCs; i++){
                std::cout<<LLCrequests[i][core]<<' ';
                LLCrequests[i][core]=0;
            }
            std::cout<<'\n';
        }
        std::cout<<"number of LLC misses\n";
        for (int core =0; core<num_cores; core++){
            std::cout<<"CORE "<<core<<' ';
            for (int i=0; i<num_LLCs ;i++){
                std::cout<<LLCmisses[i][core]<<' ';
                LLCmisses[i][core]=0;
            }
            std::cout<<'\n';
        }
        std::cout<<"number of LLC hits\n";
        for (int core =0; core<num_cores; core++){
            std::cout<<"CORE "<<core<<' ';
            for (int i=0; i<num_LLCs ;i++){
                std::cout<<LLChits[i][core]<<' ';
                LLChits[i][core]=0;
            }
            std::cout<<'\n';
        }
        std::cout<<"number of Directory requests\n";
        for (int core =0; core<num_cores; core++){
            std::cout<<"CORE "<<core<<' ';
            for (int i=0; i<num_LLCs ;i++){
                std::cout<<dirRequests[i][core]<<' ';
                dirRequests[i][core]=0;
            }
            std::cout<<'\n';
        }
    }
    
    void measure(uint64_t curTick){
        if(curTick - prevLLCprintTime > LLCstatsDumpInterval){
            measurements::LLCstatsPrint();
            prevLLCprintTime = curTick;
        }
    }




    void LLCRequestQueueSize(int llc, int size){
        if (size>maxQueueLength[llc]){
            maxQueueLength[llc] = size;
        }
        BufferSizeSum[llc] += size;
        timesMessageBufferSizeCalled[llc]++;
    }

    void countWaitingTime(int llc, uint64_t time){
        waitingTimeSum[llc] += time;
        timesWaitingTimeReported[llc]++;
    }

    void countLLCmisses(int llc, int core){
        LLCmisses[llc][core]++;
    }

    void countLLChits(int llc, int core){
        LLChits[llc][core]++;
    }

    void countDirRequests(int llc, int core){
        dirRequests[llc][core]++;
    }

    void countLLCRequests(int llc, int core){
        LLCrequests[llc][core]++;
    }

    void countResponsesReceived(int llc){
        responses[llc]++;
    }

    void countUnblocks(int llc){
        unblocks[llc]++;
    }

    

}


