/* This object is used to pass value to global variable num-llc, num-cores, partitioning interval, is on
 */

#ifndef __SIM_SPACE_PARTITIONING_OPTIONS_HH__
#define __SIM_SPACE_PARTITIONING_OPTIONS_HH__

#include "params/SpacePartitioningOptions.hh"
#include "sim/sim_object.hh"
#include "sim/space_partitioning.hh"

namespace gem5
{
	class SpacePartitioningOptions : public SimObject
	{
	  public: 
	        typedef SpacePartitioningOptionsParams Param;
	        SpacePartitioningOptions(const SpacePartitioningOptionsParams &p);	       
	  
	  public:
		int algo;		//Partitioning on/off
		uint64_t interval_size;		//interval in which the paritioning changes
		int num_cores;
        int num_llcs;
		int l2_assoc;
		int dss_size;
		int stack_size;
		double lr;
		double discnt; 
		int prvt_ways;
		std::string q_file_path;
		uint64_t l2_size;
		int max_classes;
		double icov_thresh;
	};
} //namespace gem5
#endif
