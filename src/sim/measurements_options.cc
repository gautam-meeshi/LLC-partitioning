/* src/sim/intervalsize_l3.cc : 
This object is used to pass value to global variable intervalsize used in src/sim/mypriority.cc. 
 */

#include "sim/measurements_options.hh"

#include<iostream>

#include "base/trace.hh"
#include "base/statistics.hh"

namespace gem5
{

MeasurementsOptions::MeasurementsOptions(const MeasurementsOptionsParams &p) : 
	SimObject(p),
	num_cores(p.num_cores),
	num_llcs(p.num_llcs),
	llc_dump_interval(p.llc_dump_interval)
{
	measurements::setMeasurementsOptions((int)num_cores, (int)num_llcs,(int)llc_dump_interval);
}
}//namespace gem5

