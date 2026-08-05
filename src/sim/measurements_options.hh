/* src/sim/intervalsize_l3.hh : This object is used to pass value to global variable intervalsize used in src/sim/mypriority.cc.  
 */

#ifndef __SIM_MEASUREMENTS_OPTIONS_HH__
#define __SIM_MEASUREMENTS_OPTIONS_HH__

#include "params/MeasurementsOptions.hh"
#include "sim/sim_object.hh"
#include "sim/measurements.hh"

namespace gem5
{
	class MeasurementsOptions : public SimObject
	{
	  public: 
	        typedef MeasurementsOptionsParams Param;
	        MeasurementsOptions(const MeasurementsOptionsParams &p);	       
	  
	  public:
		int num_cores;		// BW Partitioning algorithm id
		int num_llcs;		// interval size after which algorithm adjusts priorities
		int llc_dump_interval;		// reset and dump stats after 'dumpStats' number of instructions
	};
} //namespace gem5
#endif
