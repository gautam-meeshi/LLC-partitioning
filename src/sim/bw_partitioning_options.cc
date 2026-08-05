
#include "sim/bw_partitioning_options.hh"

#include<iostream>

#include "base/trace.hh"
#include "base/statistics.hh"

namespace gem5
{

BWPartitioningOptions::BWPartitioningOptions(const BWPartitioningOptionsParams &p) : 
	SimObject(p),
    interSize(p.interSize),
    algorithm(p.algorithm),
    numcores(p.numcores),
    numllcs(p.numllcs),
    ifrth(p.ifrth),
    imrth(p.imrth),
    dumpStatsAfter(p.dumpStatsAfter)
{
	real::setBWPOptions((uint64_t)interSize, (int)algorithm,(int)numcores, (int)numllcs, (float)ifrth, float(imrth), (uint64_t)dumpStatsAfter);
}
}//namespace gem5

