#include "CPA.hh"
#include <cmath>
CPA::CPA(int numcores, int numllcs, int l2assoc, uint64_t l2Size,uint64_t space_partitioning_interval, int maxClasses, double icovThresh){
    this->num_cores = numcores;
    this->num_llcs = numllcs;
    this->l2_assoc = l2assoc;
    this->max_classes = maxClasses;
    this->icov_thresh = icovThresh;
    this->l2_size = l2Size;
    this->cycles = space_partitioning_interval/500;
    first_interval.resize(num_llcs, true);
    update_clos.resize(num_llcs, false);

    instructions.resize(num_cores, 0);
    num_llc_hits.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    num_llc_misses.resize(num_llcs, std::vector<uint64_t>(num_cores, 0));
    llc_occupancy.resize(num_llcs, std::vector<uint64_t>(num_cores,0));
    mpki.resize(num_llcs, std::vector<double>(num_cores, 0));
    ipcs.resize(num_cores, 0.0);
    hpki.resize(num_llcs, std::vector<double>(num_cores, 0));


    /*at the start of simulation each core is in same class*/
    class_of_core.resize(num_llcs, std::vector<int>(num_cores, 1));//initially everybody belongs to a single class

    core_behavior.resize(num_llcs, std::vector<int>(num_cores, 1));//all are non critical initially
    range_of_class.resize(num_llcs,std::vector<std::pair<int,int>>(max_classes+1, {0,l2_assoc-1}));
    
    running_ipc_average.resize(num_cores, 0.0);
    times_ipc_recorded.resize(num_cores, 0);
    prev_ipc.resize(num_cores, 0.0);

    last_10_MPKIs.resize(num_llcs, std::vector<double>());

    
}

//asks how many ways have to allowed for the core at llc
std::pair<int,int> 
CPA::getRangeOfCore(int llc, int core){
    return range_of_class[llc][class_of_core[llc][core]];
}

double 
CPA::getICOV(int core){
    double this_ipc = (double)instructions[core]/(double)cycles;//current IPC
    double numerator = std::abs(this_ipc - running_ipc_average[core]);//diff with running average
    double this_avg = 
    (running_ipc_average[core]*times_ipc_recorded[core] + this_ipc)
    /(double)(times_ipc_recorded[core]+1);//current running average

    return numerator/this_avg;
}

double CPA::getMPKI_H(int llc){
    //last_10_MPKIs contains the MPKI average of all the cores for last 10 intervals
    //mu - average over the last 10 entries
    //std - standard deviation over last 10 entries
    //mpki_h = mu+1.5*std
    double mu = 0.0;
    int s = 10;
    int size = last_10_MPKIs[llc].size();
    if (size < s){
        s = size;
    }
    for(int i=size-1; i>=size - s; i--){
        mu += last_10_MPKIs[llc][i];
    }
    assert(s>0 && "CPA:67");
    mu = mu/(double)s;
    double std = 0.0;
    for(int i=size-1; i>=size - s; i--){
        std+=(last_10_MPKIs[llc][i] - mu)*(last_10_MPKIs[llc][i] - mu);
    }
    std = sqrt(std/(double)s);
    return std::max(1.0, mu+1.5*std);
}

int CPA::getBehaviorOfCore(int llc,int core){
    double mpki_H = getMPKI_H(llc);
    int res = 1;//non critical
    if(mpki[llc][core] > mpki_H && hpki[llc][core] >= hpki_VL){
        res = 2;//critical - sensitive or medium will be decided in step 3
    }
    if(ipcs[core] <=ipc_VL && mpki[llc][core] >= mpki_VH && hpki[llc][core] >=hpki_VH){
        res = 4;//bully
    }
    if(mpki[llc][core] >= mpki_H && hpki[llc][core]<hpki_VL){
        res = 5;//squanderer
    }
    return res;
}

uint64_t CPA::getCriticalOccupancy(int llc){
    //sum the occupancy over the critical cores
    uint64_t total = 0;
    for (int core=0; core<num_cores; core++){
        if(core_behavior[llc][core] == 2 || core_behavior[llc][core] == 3){
            total += llc_occupancy[llc][core];
        }
    }
    return total;
}

uint64_t CPA::getSizeOfNonCriticalRegion(int llc){
    //amount of bytes allocated to the non critical cores
    uint64_t num_sets = l2_size/64/l2_assoc;
    uint64_t non_crit_size = 
    num_sets * 
    (range_of_class[llc][1].second - range_of_class[llc][1].first+1) * 64;
    return non_crit_size;
}

int CPA::getClassesCount(int llc){
    //count the number of unique classes in use
    std::vector<int> marker(max_classes+1,0);
    int class_count=0;
    for(int core=0; core<num_cores; core++){
        marker[class_of_core[llc][core]] = 1;
    }
    
    for(int i=1;i<=num_cores; i++){
        if(marker[i] == 1){
            class_count++;
        }
    }
    return class_count;
}

std::vector<int> CPA::getCoresOfClass(int llc, int Class){
    //list of cores belonging to this class
    std::vector<int> res;
    for(int core=0; core<num_cores; core++){
        if(class_of_core[llc][core] == Class){
            res.push_back(core);
        }
    }
    return res;
}

void CPA::isolateNonCriticalApps(int llc){
    //isolate the non-critical cores that 
    std::vector<int> classes;
    for (int core=0;core<num_cores;core++){
        if((class_of_core[llc][core] != 1 && core_behavior[llc][core] == 1)){
            classes.push_back(class_of_core[llc][core]);
        }
    }
    int count = classes.size();
    int lower_limit = std::max(l2_assoc/2, l2_assoc-2*count);
    if(count>0){
        assert(lower_limit<l2_assoc && "CPA:143");
        for(int i=0; i<classes.size(); i++){
            //if there are multiple provide more ways
            range_of_class[llc][classes[i]] = {lower_limit, l2_assoc-1};
        }
    }
    //all such apps are sharing the space
}



void CPA::allocateExtraClass(int llc, int core){
    std::vector<int> marker(max_classes+1, 0);
    for(int core=0;core<num_cores; core++){
        marker[class_of_core[llc][core]] = 1;
    }
    bool alloted = false;
    if(core_behavior[llc][core] == 1){
        /*this non critical app is consuming too much space have to confine it
        lesser ways, give a seperate CLOS >6*/
        for(int i=7;i<marker.size();i++){
            if(marker[i] == 0){
                class_of_core[llc][core] = i;
                marker[i] = 1;
                alloted = true;
                break;
            }
        }
    }
    if(!alloted){
        assert(false && "CPA:176");
    }
}

int CPA::getCountOfBehavior(int llc, int behavior){
    int count=0;
    for(int core=0; core<num_cores; core++){
        if(core_behavior[llc][core] == behavior){
            count++;
        }
    }
    return count;
}

int CPA::getCriticalAssoc(int llc){
    int crit_count = getCountOfBehavior(llc, 2) + getCountOfBehavior(llc, 3);
    int crit = 0; 
    if(crit_count == 1){
        //60% crit, 50% non crit
        crit = (int)(0.6*(double)l2_assoc);
    }else if(crit_count == 2){
        //65% crit, 45% non crit
        crit = (int)(0.65*(double)l2_assoc);
    }else if(crit_count == 3){
        //70% crit, 40% non crit
        crit = (int)(0.7*(double)l2_assoc);
    }
    return crit;
}

void CPA::allocateClass(int llc){
    //allocate a class to each core
    std::vector<int> marker(max_classes+1, 0);
    int crit_count=0;
    int squanderer_count=0;
    for(int core=0;core<num_cores; core++){
        //marker[class_of_core[llc][core]] = 1;
        if(core_behavior[llc][core]==2 || core_behavior[llc][core]==3){
            crit_count++;
        }
        if(core_behavior[llc][core] == 5){
            squanderer_count++;
        }
    }
    for (int core=0; core<num_cores; core++){
        if(core_behavior[llc][core] == 1){
            class_of_core[llc][core] = 1;
            marker[1] =1;
        }
        else if(core_behavior[llc][core]==2 || core_behavior[llc][core]==3){
            //critical
            if(crit_count>3){
                class_of_core[llc][core] = 1;
            }else{
                /*if(class_of_core[llc][core]>1 && class_of_core[llc][core]<5){
                    continue;//already has a valid allocation
                }*/
                if(marker[2] == 0){
                    class_of_core[llc][core] = 2;
                    marker[2] = 1;
                }else if(marker[3] == 0){
                    class_of_core[llc][core] = 3;
                    marker[3] = 1;
                }else if(marker[4] == 0){
                    class_of_core[llc][core] = 4;
                    marker[4] = 1;
                }else{
                    assert(false && "CPA:236");
                }
            }
        }
        else if(core_behavior[llc][core] == 4){
            //bully
            class_of_core[llc][core] = 1;
            marker[1] = 1;
        }
        else if(core_behavior[llc][core] == 5){
            //squanderer
            if(squanderer_count>2){
                class_of_core[llc][core] = 1;
                marker[1]=1;
            }else{
                /*if(class_of_core[llc][core] == 5 || class_of_core[llc][core] == 6){
                    continue;
                }*/
                if(marker[5] == 0){
                    class_of_core[llc][core] = 5;
                    marker[5]=1;
                }else if(marker[6] == 0){
                    class_of_core[llc][core] = 6;
                    marker[6]=1;
                }else{
                    assert(false && "CPA:261");
                }
            }
        }
    }
}

void CPA::allocateRanges(int llc){
    //allocate range for each class as suggested in the paper
    int crit_count=0;
    int squanderer_count=0;
    for(int core=0;core<num_cores; core++){
        if(core_behavior[llc][core]==2 || core_behavior[llc][core]==3){
            crit_count++;
        }
        if(core_behavior[llc][core] == 5){
            squanderer_count++;
        }
    }
    bool Default = false;
    if(crit_count==0 || crit_count>3){
        //default
        //assumes critical and non critical are assigned to class 1
        range_of_class[llc][1] = {0,l2_assoc-1};
        Default = true;
    }
    for(int core=0; core<num_cores;core++){
        if(core_behavior[llc][core] ==2 && !Default){
            int crit =0;
            int non_crit=l2_assoc;
            
            if(crit_count == 1){
                //60% crit, 50% non crit
                crit = (int)(0.6*(double)l2_assoc);
                non_crit = (int)(0.5*(double)l2_assoc);
            }else if(crit_count == 2){
                //65% crit, 45% non crit
                crit = (int)(0.65*(double)l2_assoc);
                non_crit = (int)(0.45*(double)l2_assoc);
            }else if(crit_count == 3){
                //70% crit, 40% non crit
                crit = (int)(0.7*(double)l2_assoc);
                non_crit = (int)(0.4*(double)l2_assoc);
            }
            assert(non_crit<l2_assoc && "CPA:300");
            range_of_class[llc][1] = {l2_assoc-non_crit, l2_assoc-1};
            if(crit_count>0 && crit_count<4){
                assert(crit>=l2_assoc/2 && "CPA:303");
                for(int c=2;c<=4;c++){
                    std::vector<int> cores = getCoresOfClass(llc, c);
                    if(cores.size() > 0 && core_behavior[llc][cores[0]] == 2){
                        range_of_class[llc][c] = {0, crit-1};
                    }
                }
            }
        }else if(core_behavior[llc][core] == 5){
            if(squanderer_count==1){
                //give 2 ways
                range_of_class[llc][5] = {l2_assoc-2, l2_assoc-1};
                range_of_class[llc][6] = {l2_assoc-2, l2_assoc-1}; 
            }else if(squanderer_count == 2){
                //give 2 ways each, these overlap with the non critical ranges
                range_of_class[llc][5] = {l2_assoc-4, l2_assoc-3};
                range_of_class[llc][6] = {l2_assoc-2, l2_assoc-1};
            }
        }
    }
}

//update the partition when called
void 
CPA::updatePartition(int algo){
    //this is called every fixed interval
    //but the class of core change depending on the icov - marks the phase change
    //STEP 1 - calculate the IPC, HPKI, MPKI
    updates+=1;
    if(updates == 1){
        return;
    }
    assert(cycles>0 && "CPA:335");
    for(int core=0;core<num_cores;core++){
        assert(instructions[core]>0 && "CPA:337");
        ipcs[core] = (double)instructions[core]/(double)cycles;
    }
    for(int llc=0;llc<num_llcs;llc++){
        for(int core=0;core<num_cores;core++){
            mpki[llc][core] = (double)num_llc_misses[llc][core]/(double)instructions[core]*1000.0;
            hpki[llc][core] = (double)num_llc_hits[llc][core]/(double)instructions[core]*1000.0;
            update_clos[llc]=false;
        }
    }
    if(updates>10){
        //STEP 2
        std::cout<<"Updating\n";
        for(int llc=0;llc<num_llcs; llc++){
            //check whether the partititioning has to be changed
            if(first_interval[llc]){
                first_interval[llc] = false;
                update_clos[llc] = true;
            }else{
                std::cout<<"LLC "<<llc<<" MPKI_H "<<getMPKI_H(llc)<<'\n';
                for(int core=0; core<num_cores; core++){
                    int behavior = getBehaviorOfCore(llc, core);
                    if(getICOV(core)>icov_thresh && behavior != core_behavior[llc][core]){
                        update_clos[llc] = true;
                        core_behavior[llc][core] = behavior;
                    }
                }
            }
            if(update_clos[llc]){
                //if change is required then allocate class to each core and range to each class
                allocateClass(llc);
                allocateRanges(llc);
            }
        }
        
        /*STEP 3 - Check whether the core is using more cache than it needs
        Critical cores are checked for medium behavior
        Medium application consuming 
        more than half of space given to critical apps is restricted to 
        proportional part - 1/3 if there are 3 critical apps
        if only 1 critical and it is medium, space of class 1 (non critical) is incremented
        if critical apps > 1 then atleast 1 should be sensitive
        */
        for(int llc=0;llc<num_llcs;llc++){
            //count the number of critical apps
            int sensitive_cores = getCountOfBehavior(llc, 2);
            int medium_cores = getCountOfBehavior(llc, 3);
            int critical_cores = sensitive_cores + medium_cores;
            if(critical_cores==0 || critical_cores>3){
                //in this case the critical cores are mapped into the non critical region. STEP 3 is not required
                continue;
            }
            uint64_t crit_occup = getCriticalOccupancy(llc);
            std::cout<<"LLC "<<llc<<" critical occupancy "<<crit_occup<<'\n';
            for(int core=0;core<num_cores;core++){//for all sensitive apps
                if((core_behavior[llc][core] == 2 && ipcs[core] >= ipc_L && llc_occupancy[llc][core] > crit_occup/2) 
                    || core_behavior[llc][core] == 3){
                    //reduce the ways
                    if(critical_cores - medium_cores == 1 && critical_cores>1){
                        //retain atleast 1 critical core
                        break;
                    }
                    std::cout<<"medium core "<<llc<<' '<<core<<'\n';
                    core_behavior[llc][core]=3;
                    medium_cores++;
                }
            }
            int sensitive_size = getCriticalAssoc(llc);
            int proportion = sensitive_size*medium_cores/critical_cores;
            for(int core=0; core<num_cores; core++){
                if(core_behavior[llc][core] == 3){
                    if(critical_cores == 1 && medium_cores == 1){
                        assert(0<=sensitive_size/2 && "CPA:402");
                        assert(class_of_core[llc][core]>1 && "CPA:403");
                        range_of_class[llc][class_of_core[llc][core]] = {0, sensitive_size/2};
                    }else{
                        assert(0 <= proportion-1 && "CPA:417");
                        assert(class_of_core[llc][core] > 1);
                        range_of_class[llc][class_of_core[llc][core]] = {0, proportion-1};
                    }
                }
            }
            if(critical_cores == 1 && medium_cores==1){
                assert(sensitive_size/2+1<=l2_assoc-1 && "CPA:430");
                range_of_class[llc][1] = {sensitive_size/2+1,l2_assoc-1};
            }
        }
        /*STEP 4 - Check non critical apps
        App occupying more than 1/3 of space given to class 1 and make no profit
        is given seperate class
        restricted to smaller range
        */
        for(int llc=0; llc<num_llcs; llc++){
            uint64_t non_crit_size = getSizeOfNonCriticalRegion(llc);
            std::cout<<"LLC "<<llc<<" non critical allocation "<<non_crit_size<<'\n';
            bool isolate = false;
            for(int core=0; core<num_cores; core++){
                if(core_behavior[llc][core]==1 
                && llc_occupancy[llc][core] > non_crit_size/3 
                && mpki[llc][core]<0.5 
                && hpki[llc][core]<0.5){
                    //isolate app in a few ways - How many, be specific
                    //2*number of such applications - inlcudes the squanderer
                    allocateExtraClass(llc, core);
                    isolate=true;
                }
            }
            if(isolate){
                isolateNonCriticalApps(llc);
            }
        }
        /*STEP 5 - Partitions are adjusted - 
        critical apps should not take too much space
        every time this adjustment happens
        the adjustment is not done for 5 intervals
        */
        if(updates - adjusted > 5){
            std::cout<<"Adjusting\n";
            for(int llc=0; llc<num_llcs; llc++){
                int state = getPartitioningState(llc);
                if(state == 2 || state == 3){
                    //number of critical cores is greater than 3 - crit and non crit are in same range
                    continue;
                }
                double prev_crit_ipc= 0.0;
                double prev_non_crit_ipc = 0.0;
                double crit_ipc= 0.0;
                double non_crit_ipc = 0.0;
                int sensitive_count=0;
                int end = 0;
                for(int core=0; core<num_cores; core++){
                    //have to remove overalapping based on the performance degradation
                    if(core_behavior[llc][core] == 2 || core_behavior[llc][core] == 3){
                        prev_crit_ipc += prev_ipc[core];
                        crit_ipc +=ipcs[core];
                    }else if(core_behavior[llc][core] == 1){
                        prev_non_crit_ipc+=prev_ipc[core];
                        non_crit_ipc+=ipcs[core];
                    }
                    if(class_of_core[llc][core]>1 && core_behavior[llc][core]==2){
                        sensitive_count++;
                        end = std::max(end, range_of_class[llc][class_of_core[llc][core]].second);
                    }
                }
                if(sensitive_count > 0){
                    assert(end>0 && end<l2_assoc && "CPA:485");
                    if(crit_ipc < prev_crit_ipc){
                        range_of_class[llc][1].first = end+1;
                        assert(range_of_class[llc][1].first <= range_of_class[llc][1].second && "CPA:493");
                        adjustment_winner=2;
                    }else if(non_crit_ipc < prev_non_crit_ipc){
                        int new_end = range_of_class[llc][1].first-1;
                        assert(new_end > 0 && "CPA:492");
                        for(int i=2; i<5; i++){
                            if(range_of_class[llc][i].second == end){
                                range_of_class[llc][i].second = new_end;
                            }
                            assert(range_of_class[llc][i].first <= range_of_class[llc][i].second && "CPA:501");
                        }
                        adjustment_winner=1;
                    }
                    adjusted = updates;
                }
            }
        }else{
            //reset the adjustment because it might have got over written
            for(int llc=0; llc<num_llcs; llc++){
                int state = getPartitioningState(llc);
                if(state==2 || state==3){
                    continue;
                }
                int end = 0;
                int sensitive_count=0;
                for(int core=0; core<num_cores; core++){
                    if(class_of_core[llc][core]>1 && core_behavior[llc][core]==2){
                        sensitive_count++;
                        end = std::max(end, range_of_class[llc][class_of_core[llc][core]].second);
                    }
                }
                if(sensitive_count>0){
                    assert(end>0 && end<l2_assoc);
                    if(adjustment_winner == 2){
                        range_of_class[llc][1].first = end+1;
                        assert(range_of_class[llc][1].first <= range_of_class[llc][1].second && "CPA:526");
                    }else{
                        int new_end = range_of_class[llc][1].first-1;
                        assert(new_end>0);
                        for(int i=2; i<5; i++){
                            if(range_of_class[llc][i].second == end){
                                //only sensitive cores
                                range_of_class[llc][i].second = new_end;
                            }
                            assert(range_of_class[llc][i].first <= range_of_class[llc][i].second && "CPA:533");
                        }
                    }
                }
            }
        }
    }
    /*Maintain last 10 MPKIs for calculating MPKI_H threshold*/
    for(int llc=0;llc<num_llcs;llc++){
        double mpki_sum = 0.0;
        for(int core=0;core<num_cores;core++){
            mpki_sum+=mpki[llc][core];
        }
        last_10_MPKIs[llc].push_back(mpki_sum/(double)num_cores);
        if(last_10_MPKIs[llc].size()>10){
            last_10_MPKIs[llc].erase(last_10_MPKIs[llc].begin(), last_10_MPKIs[llc].end()-10);
        }
    }
    printStats();
    resetCounters();
}


int CPA::getPartitioningState(int llc){
    int crit_count= getCountOfBehavior(llc, 2) + getCountOfBehavior(llc, 3);
    int squanderer_count= getCountOfBehavior(llc, 5);

    if(crit_count > 3 && squanderer_count <= 2){
        return 2;//squaderer restricted to 2 ways, other can take 0-15
    }
    else if(crit_count>3 && squanderer_count>2){
        return 3;//everyone is allocated 0-15
    }
    else if(crit_count<=3 && squanderer_count>2){
        return 4;//critical region exists but there is no special ways for squanderers
    }
    return 1;//normal partitioning algorithm as it is
}

void CPA::recordInstruction(int core){
    instructions[core]++;
}

void CPA::recordHits(int llc, int core){
    num_llc_hits[llc][core]++;
}

void CPA::recordMisses(int llc, int core){
    num_llc_misses[llc][core]++;
}

void CPA::incrementOccupancy(int llc, int core){
    llc_occupancy[llc][core]+=64;
}

void CPA::decrementOccupancy(int llc, int core){
    llc_occupancy[llc][core]-=64;
}

void CPA::resetCounters(void){
    for(int core=0; core<num_cores; core++){
        running_ipc_average[core] = (running_ipc_average[core]*times_ipc_recorded[core]+ipcs[core])/(double)(++times_ipc_recorded[core]);
    }
    for(int llc=0;llc<num_llcs;llc++){
        for(int core=0;core<num_cores;core++){
            update_clos[llc] = false;
            num_llc_hits[llc][core] = 0;
            num_llc_misses[llc][core] = 0;
            mpki[llc][core] = 0.0;
            hpki[llc][core] = 0.0;
        }
    }
    for(int core=0; core<num_cores; core++){
        instructions[core] = 0;
        prev_ipc[core] = ipcs[core];
        ipcs[core] = 0.0;
    }
}

void CPA::printStats(void){
    std::cout<<"CPA stats";
    std::cout<<"\nCycles "<<cycles;
    std::cout<<"\nCommitted instructions ";
    for(int core=0;core<num_cores;core++){
        std::cout<<instructions[core]<<' ';
    }
    std::cout<<"\nIPC ";
    for(int core=0;core<num_cores;core++){
        std::cout<<ipcs[core]<<' ';
    }
    for(int llc=0;llc<num_llcs;llc++){
        std::cout<<"\nLLC "<<llc;
        std::cout<<"\nLLC hits ";
        for(int core=0;core<num_cores;core++){
            std::cout<<num_llc_hits[llc][core]<<' ';
        }
        std::cout<<"\nLLC misses ";
        for(int core=0;core<num_cores;core++){
            std::cout<<num_llc_misses[llc][core]<<' ';
        }
        std::cout<<"\nHPKI ";
        for(int core=0;core<num_cores;core++){
            std::cout<<hpki[llc][core]<<' ';
        }
        std::cout<<"\nMPKI ";
        for(int core=0;core<num_cores;core++){
            std::cout<<mpki[llc][core]<<' ';
        }
        std::cout<<"\nOccupancy ";
        for(int core=0;core<num_cores;core++){
            std::cout<<llc_occupancy[llc][core]<<' ';
        }
        std::cout<<"\nCore behavior ";
        for(int core=0; core<num_cores; core++){
            std::cout<<core_behavior[llc][core]<<' ';
        }
        std::vector<int> classes;
        std::cout<<"\nClass of cores ";
        for(int core=0;core<num_cores;core++){
            std::cout<<class_of_core[llc][core]<<' ';
            classes.push_back(class_of_core[llc][core]);
        }
        std::cout<<"\nRange of classes\n";
        int lower_limit = l2_assoc;
        int upper_limt = 0;
        for(int c=0;c<classes.size();c++){
            assert(range_of_class[llc][classes[c]].first <= range_of_class[llc][classes[c]].second && "CPA::644");
            lower_limit = std::min(lower_limit, range_of_class[llc][classes[c]].first);
            upper_limt = std::max(upper_limt, range_of_class[llc][classes[c]].second);
            std::cout<<classes[c]<<' '<<range_of_class[llc][classes[c]].first<<' '<<range_of_class[llc][classes[c]].second<<'\n';
        }
        assert(lower_limit==0 && upper_limt==(l2_assoc-1) && "CPA:651");
    }
}