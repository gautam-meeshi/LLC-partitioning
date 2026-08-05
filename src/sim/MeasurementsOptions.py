##This object is used to pass value to global variable intervalsize used in src/sim/mesurements.cc. 


from m5.SimObject import SimObject 
from m5.params import *

class MeasurementsOptions(SimObject):
    type = 'MeasurementsOptions'
    cxx_header = 'sim/measurements_options.hh'
    cxx_class="gem5::MeasurementsOptions"

    num_cores = Param.Int('8', "Number of cores being recorded");
    num_llcs = Param.Int('2', "Number of LLCs to record");
    llc_dump_interval = Param.Int('1000000', "LLC dump interval");
