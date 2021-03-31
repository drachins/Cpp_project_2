#include <vector>
#include <string>
#include <unistd.h>

#include "processor.h"
#include "linux_parser.h"

using namespace LinuxParser;
using std::string;
using std::vector;
using std::stof;



// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 

    float prev_idle = 0;
    float idle = 0;
    float prev_active = 0;
    float active = 0;
    float prev_total = 0;
    float total = 0;

    vector<string> old_jiffs = CpuUtilization();
    sleep(1);
    vector<string> new_jiffs = CpuUtilization();

    for(int i = 0; i < CPUStates::kSteal_; i++){
        if(i == CPUStates::kIdle_ || i == CPUStates::kIOwait_){
            prev_idle += stof(old_jiffs[i]);
            idle += stof(new_jiffs[i]);
        }
        else{
            prev_active += stof(old_jiffs[i]);
            active += stof(new_jiffs[i]);
        }
    }

    prev_total = prev_active + prev_idle;
    total = active + idle;

    float total_diff = total - prev_total;
    float idle_diff = idle - prev_idle; 

    return (total_diff - idle_diff)/total_diff; 
}