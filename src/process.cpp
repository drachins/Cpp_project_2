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

void Process::Pid(int pid_id){
    this->process_id = pid_id;
}


int Process::Pid() { return Process::process_id; }


float Process::CpuUtilization() { 

    long active_jiffs = LinuxParser::ActiveJiffies(this->process_id);
    long uptime = LinuxParser::UpTime(this->process_id);
    this->currcpu_util = (float)active_jiffs/(float)uptime;

    return currcpu_util;

}


string Process::Command() { return LinuxParser::Command(this->process_id);}


string Process::Ram() { 

    string ram_str = LinuxParser::Ram(this->process_id);
    string::size_type sz;

    long ram = stol(ram_str, &sz)/1000;

    return to_string(ram);
    }


string Process::User() { return LinuxParser::User(this->process_id);}


long int Process::UpTime() { return LinuxParser::UpTime(this->process_id);}


bool Process::operator<(Process const& a) const {return this->currcpu_util < a.currcpu_util;}