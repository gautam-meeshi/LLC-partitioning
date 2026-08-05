/* src/sim/intervalsize_l3.hh : This object is used to pass value to global variable intervalsize used in src/sim/mypriority.cc.  
 */

#ifndef __BW_PARTITIONING_OPTIONS_HH__
#define __BW_PARTITIONING_OPTIONS_HH__

#include "params/BWPartitioningOptions.hh"
#include "sim/sim_object.hh"
#include "sim/real.hh"

namespace gem5
{
	class BWPartitioningOptions : public SimObject
	{
	  public: 
	        typedef BWPartitioningOptionsParams Param;
	        BWPartitioningOptions(const BWPartitioningOptionsParams &p);	       
	  
	  public:
        uint64_t interSize;
        int algorithm;
        int numcores;
        int numllcs;
        float ifrth;
        float imrth;
        uint64_t dumpStatsAfter;
	};
} //namespace gem5
#endif
