

from m5.SimObject import SimObject 
from m5.params import *

class BWPartitioningOptions(SimObject):
    type = 'BWPartitioningOptions'
    cxx_header = 'sim/bw_partitioning_options.hh'
    cxx_class="gem5::BWPartitioningOptions"

    dumpStats = Param.Int('1', "Reset and dump stats");
    interSize = Param.UInt64('700000', "Change the bw partitioning after this cycles");
    algorithm = Param.Int('1', "bw algo type");
    numcores = Param.Int('8', 'number of cores in the simulation');
    numllcs = Param.Int('2', 'number of LLC in the simulation');
    ifrth = Param.Float('0.02', 'intruction fetch threshold');
    imrth = Param.Float('0.5', 'instruction miss threshold');
    dumpStatsAfter = Param.UInt64('50000000', "Dump stats after this and reset the cache system");
