##This object is used to pass value to global variable intervalsize used in src/sim/space_partitioning.cc. 


from m5.SimObject import SimObject 
from m5.params import *

class SpacePartitioningOptions(SimObject):
    type = 'SpacePartitioningOptions'
    cxx_header = 'sim/space_partitioning_options.hh'
    cxx_class="gem5::SpacePartitioningOptions"
    algo = Param.Int('0', 'algo to use in space partitioning');
    num_cores = Param.Int('8', "Number of cores being recorded");
    num_llcs = Param.Int('2', "Number of LLCs to record");
    interval_size = Param.UInt64('5000000', "Space partitioinig interval");
    l2_assoc = Param.Int('16', "LLC associativity");
    dss_size = Param.Int('64', "DSS size of UMON");
    stack_size = Param.Int('16', "Max number of addresses stored for a set in DSS");
    learning_rate = Param.Float('0.8', "Learning rate");
    discount = Param.Float('0.1',"discount rate in Q learning");
    private_ways = Param.Int('12', "Number of ways that are private to cores, rest will be shared always");
    q_file_path = Param.String("", 'The file path for the q_table to be loaded and saved to');
    l2_size = Param.UInt64('4194304', '1 LLC slice size');
    max_classes = Param.Int('16', 'Maximum number of classes in CPA');
    icov_thresh = Param.Float('0.2', 'interval coeff of variation thesh, used for phase change detection');
