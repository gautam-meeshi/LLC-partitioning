# Copyright (c) 2010 Advanced Micro Devices, Inc.
#               2016 Georgia Institute of Technology
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from m5.params import *
from m5.objects import *

from common import FileSystemConfig

from topologies.BaseTopology import SimpleTopology

# Creates a generic Mesh assuming an equal number of cache
# and directory controllers.
# XY routing is enforced (using link weights)
# to guarantee deadlock freedom.

class config2(SimpleTopology):
    description='config2'
    '''
        
    C0-C4-NA----C8-Cc
    |  |   |    |  |
    C1-C5-L0----C9-Cd
    |  |   | \  |  |
    C2-C6-L1- D Ca-Ce
    |  |  |     |  |
    C3-C7-NA- --Cb-Cf
    '''
    def __init__(self, controllers):
        self.nodes = controllers

    # Makes a generic mesh
    # assuming an equal number of cache and directory cntrls

    def makeTopology(self, options, network, IntLink, ExtLink, Router):
        nodes = self.nodes
        print("TOPOLOGY - Config2")
        assert(options.num_cpus == 16 and options.num_l2caches==2)
        assert(options.num_dirs==1)

        num_rows = 4
        num_columns = 5
        num_routers = num_rows*num_columns
        # default values for link latency and router latency.
        # Can be over-ridden on a per link/router basis
        link_latency = options.link_latency # used by simple and garnet
        router_latency = options.router_latency # only used by garneT
        

        # Create the routers in the mesh
        routers = [Router(router_id=i, latency = router_latency) \
            for i in range(num_routers)]
        network.routers = routers

        # link counter to set unique link ids
        link_count = 0

        # Add all but the remainder nodes to the list of nodes to be uniformly
        # distributed across the network.
        L0_indices = []
        L1_indices = []
        L2_indices = []
        Dir_indices = []

        for (node_index,node) in enumerate(nodes):
            if(node.type == 'L0Cache_Controller'):
                L0_indices.append(node_index)
            elif(node.type == 'L1Cache_Controller'):
                L1_indices.append(node_index)
            elif(node.type == 'L2Cache_Controller'):
                L2_indices.append(node_index)
            elif(node.type == 'Directory_Controller'):
                Dir_indices.append(node_index)
            else:
                print(f'PANIC UNKNOWN NODE - {node.type}')

        # Connect each node to the appropriate router
        ext_links = []
        cpus = [0,5,10,15,1,6,11,16,3,8,13,18,4,9,14,19]
        l0i=0
        l1i=0
        for i in cpus:
            ext_links.append(ExtLink(link_id=link_count, ext_node=nodes[L0_indices[l0i]],
                                    int_node=routers[i],
                                    latency = link_latency))
            l0i+=1
            link_count+=1
            ext_links.append(ExtLink(link_id=link_count, ext_node=nodes[L1_indices[l1i]],
                                    int_node=routers[i],
                                    latency = link_latency))
            l1i+=1
            link_count+=1
        ext_links.append(ExtLink(link_id=link_count, ext_node=nodes[L2_indices[0]],
                                    int_node=routers[7],
                                    latency = link_latency))
        link_count+=1
        ext_links.append(ExtLink(link_id=link_count, ext_node=nodes[L2_indices[1]],
                                    int_node=routers[12],
                                    latency = link_latency))
        link_count+=1
        ext_links.append(ExtLink(link_id=link_count, ext_node=nodes[Dir_indices[0]],
                                int_node=routers[7],
                                latency = link_latency))
        link_count+=1
            


        network.ext_links = ext_links

        # Create the mesh links.
        int_links = []

        # East output to West input links (weight = 1)
        for row in range(num_rows):
            for col in range(num_columns):
                if (col + 1 < num_columns):
                    east_out = col + (row * num_columns)
                    west_in = (col + 1) + (row * num_columns)
                    int_links.append(IntLink(link_id=link_count,
                                             src_node=routers[east_out],
                                             dst_node=routers[west_in],
                                             src_outport="East",
                                             dst_inport="West",
                                             latency = link_latency,
                                             weight=1))
                    link_count += 1

        # West output to East input links (weight = 1)
        for row in range(num_rows):
            for col in range(num_columns):
                if (col + 1 < num_columns):
                    east_in = col + (row * num_columns)
                    west_out = (col + 1) + (row * num_columns)
                    int_links.append(IntLink(link_id=link_count,
                                             src_node=routers[west_out],
                                             dst_node=routers[east_in],
                                             src_outport="West",
                                             dst_inport="East",
                                             latency = link_latency,
                                             weight=1))
                    link_count += 1

        # North output to South input links (weight = 2)
        for col in range(num_columns):
            for row in range(num_rows):
                if (row + 1 < num_rows):
                    north_out = col + (row * num_columns)
                    south_in = col + ((row + 1) * num_columns)
                    int_links.append(IntLink(link_id=link_count,
                                             src_node=routers[north_out],
                                             dst_node=routers[south_in],
                                             src_outport="North",
                                             dst_inport="South",
                                             latency = link_latency,
                                             weight=2))
                    link_count += 1

        # South output to North input links (weight = 2)
        for col in range(num_columns):
            for row in range(num_rows):
                if (row + 1 < num_rows):
                    north_in = col + (row * num_columns)
                    south_out = col + ((row + 1) * num_columns)
                    int_links.append(IntLink(link_id=link_count,
                                             src_node=routers[south_out],
                                             dst_node=routers[north_in],
                                             src_outport="South",
                                             dst_inport="North",
                                             latency = link_latency,
                                             weight=2))
                    link_count += 1


        network.int_links = int_links

    # Register nodes with filesystem
    def registerTopology(self, options):
        for i in range(options.num_cpus):
            FileSystemConfig.register_node([i],
                    MemorySize(options.mem_size) // options.num_cpus, i)
