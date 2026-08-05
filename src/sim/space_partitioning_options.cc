#include "sim/space_partitioning_options.hh"

#include<iostream>

#include "base/trace.hh"
#include "base/statistics.hh"

namespace gem5
{

SpacePartitioningOptions::SpacePartitioningOptions(const SpacePartitioningOptionsParams &p) : 
	SimObject(p),
	algo(p.algo),
	interval_size(p.interval_size),
	num_cores(p.num_cores),
    num_llcs(p.num_llcs),
	l2_assoc(p.l2_assoc),
	dss_size(p.dss_size),
	stack_size(p.stack_size),
	lr(p.learning_rate),
	discnt(p.discount),
	prvt_ways(p.private_ways),
	q_file_path(p.q_file_path),
	l2_size(p.l2_size),
	max_classes(p.max_classes),
	icov_thresh(p.icov_thresh)
{
	SpacePartitioning::setSpacePOptions((int)algo, (uint64_t)interval_size,
	(int)num_cores, (int)num_llcs, (int)l2_assoc, (int) dss_size, (int) stack_size,
	(double)lr, (double)discnt, (int)prvt_ways, q_file_path,
	(uint64_t)l2_size, (int)max_classes, (double)icov_thresh );
}
}//namespace gem5

