#ifndef __SIM_MLM_HH__
#define __SIM_MLM_HH__

#include <vector>
#include <iostream>
#include <inttypes.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>


/*
NOTE - 

Objective is to reduce the number of off-chip accesses usign Q-learning
Off-chip access - number of messages shared between LLC and directory controller
State - vector of ways assigned to each core
Action - vector of size num_cores, representing the change in the ways assignment
MLM - out of 16 ways partition only in 12 ways keeping others shared
I call the 12 ways as "private_ways"

Rahul Jain used a python library for q-learning implementation,
Here we I have written it from scratch

Format of the Qtable file that will be stored at the "q_file_path" provided from the command line is 
*) Each line contains only one integer
1) first line is number of LLCs
2) second line is number of states
3) third line is number of unique actions
4) Following lines contain the q table such that 
    for all LLC
        for all states
            for all actions
                write(q_table[llc][state][action]+'\n')
Please refer to the function saveQtable in MLM.cc for the code
*/

//GAUTAM
class MLM{
    public:
        //initializer
        MLM(int numcores, int numllcs, int l2assoc, int privateWays, double learning_rate, double discount, std::string q_file_path);

        //asks how many ways have to allowed for the core at llc
        int getWaysOfCore(int llc, int core);
        
        //update the partition when called
        void updatePartition(int method);

        //checks the current allocation - number of ways assigned should be equal to the #private_ways,
        //each core should have atleast 1 way
        //adjusts the partitioning if it is not so
        void ensureSanity(int llc);

        //Returns bool
        //given the current state and the action, does it lead to an invalid state
        bool checkSanity(int llc, std::vector<int> action);
        
        //Recording offchip accesses of each llc
        //I am considering the number of messages received and sent by the dir controller as off chip accesses
        void recordOffChipMsg(int llc);

        void printStats(void);

    private:
        int num_cores;
        int num_llcs;
        int l2_assoc;
        int private_ways;
        double learning_rate;
        double discount;
        uint64_t save_counter;
        std::string q_file_path;

        //Stores state to integer mapping
        std::map<std::vector<int>, int> state2int;
        
        //Stores action to integer mapping
        std::map<std::vector<int>, int> action2int;

        //2D array 0th axis llc, 1st axis state, 2nd axis action
        std::vector<std::vector<std::vector<int>>> q_table;
    
        //This is the current state in the model
        std::vector<std::vector<int>> ways_of_core;

        //numllcs size array that stores the off chip access in the ongoing interval
        std::vector<uint64_t> dir_access_log;

        std::vector<std::vector<int>> latest_action;
        
        //returns all the valid states for the given config,
        //state is a vector of integers
        void getAllStates(int assoc, int cores);

        //returns all the valid actions for the given config
        //it will also contain the actions that do not sum to 0, such actions are filtered seperately
        std::vector<std::vector<int>> getAllActions(int cores);
        
        //creates the Qtable
        void initQtable(void);

        //called once in a partitioning interval, this updates the Q table, decides the new allocations
        void updateQtable(int llc);
        
        /*Returns the -1*(normalized bandwidth activity) in the previous interval
        This will be used to update the Q(s,a) of latest state and action
        */
        int getReward(int llc);

        //returns the best action or a random action for the llc, 
        //Force random, provides the random action always
        std::vector<int> getAction(int llc, bool force_random);

        void resetCounters(void);

        //saves the Q table
        void saveQtable(void);

};


#endif
