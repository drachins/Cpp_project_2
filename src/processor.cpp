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

    float prev_total;
    float total;

    vector<float> new_jiffs = CpuUtilParser();

    prev_total = this->oldcpu_active + this->oldcpu_idle;
    total = new_jiffs[0] + new_jiffs[1];

    float total_diff = total - prev_total;
    float idle_diff = new_jiffs[0] - this->oldcpu_idle;

    this->oldcpu_active = new_jiffs[0];
    this->oldcpu_idle = new_jiffs[1]; 

    return (total_diff - idle_diff)/total_diff; 
}