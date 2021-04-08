#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using namespace LinuxParser;
using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return Process::process_id; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 

    long current_jiffies = LinuxParser::ActiveJiffies(this->process_id);
    long active_jiffs_delta = this->old_jiffies - current_jiffies;
    this->old_jiffies = current_jiffies;

    this->currcpu_util = (float)(100 * (active_jiffs_delta/sysconf(_SC_CLK_TCK))/Process::UpTime());
    return this->currcpu_util; 

    }

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->process_id); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->process_id); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->process_id); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(this->process_id); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 

    return this->currcpu_util > a.currcpu_util; 
    }