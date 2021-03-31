
#include <iostream>
#include <regex>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

enum CPUStates {
  kUser_ = 1,
  kNice_ = 2,
  kSystem_ = 3,
  kIdle_ = 4,
  kIOwait_ = 5,
  kIRQ_ = 6,
  kSoftIRQ_ = 7,
  kSteal_ = 8,
  kGuest_ = 9,
  kGuestNice_ = 10
};

using std::string;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

long UpTime() { 
  
  string line, uptime;
  std::ifstream stream(kProcDirectory+kUptimeFilename);

  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  string::size_type sz;
  
  return stol(uptime, &sz); 
  
  }
float MemoryUtilization() { 

  string line, key, value;
  float memTotal, memFree, memBuffer;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value;
    if(key == "MemTotal:"){
      memTotal = std::stof(value);
    }
    else if(key == "MemFree:"){
      memFree = std::stof(value);
    }
    else if(key == "Buffers:"){
      memBuffer = std::stof(value);
      break; 
    }
  }

  return 1.0 - memFree/(memTotal-memBuffer); 
}

long Jiffies() { 

  string line, value, key;
  float jiffies_total;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    for(int i ; i < 10; i++){
      linestream >> value;
      jiffies_total += std::stof(value);
    }
  }

  return jiffies_total; 
}


long ActiveJiffies(int pid) { 
  string line, value;
  long active_jiffs;
  int cmajflt = 13;
  int priority = 18;

  std::ifstream stream(kProcDirectory+"/"+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 52; i++){
      linestream >> value;
      if(i > cmajflt && i < priority){
        string::size_type sz;
        active_jiffs += stol(value, &sz);
      }
    }
  }
  return active_jiffs; 
}

long ActiveJiffies() { 
  string line, value;
  long active_jiffies_sys;
  int utime = 1;
  int idle = 4;
  int iowait = 5;
  int guest_nice = 10;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    string::size_type sz;
    for(int i = 0; i < guest_nice; i++){
      linestream >> value;
      if((i >= utime && i < idle) || i > iowait){
        active_jiffies_sys += stol(value, &sz);
      }
    }
  }

  return active_jiffies_sys; 
  }

long IdleJiffies() { 
  string line, value;
  long idle_jiffies_sys;
  int idle = 4;
  int iowait = 5;
  

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    string::size_type sz;
    for(int i = 0; i <= iowait; i++){
      linestream >> value;
      if(i == idle || i == iowait){
        idle_jiffies_sys += stol(value, &sz);
      }
    }
  }

  return idle_jiffies_sys; 
}

vector<string> CpuUtilization() { 
  string line, value;
  vector<string> cpu_jiffies;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    for(int i = 0; i < CPUStates::kGuestNice_; i++){
      linestream >> value;
      if(i >= CPUStates::kUser_){
        cpu_jiffies.push_back(value);
      }
    }
  }

  return cpu_jiffies; 
  }

int TotalProcesses() { 
  string key, value, line;
  int processes;

  std::ifstream stream(kProcDirectory+kStatFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> key >> value;
    if(key == "processes"){
      string::size_type sz;
      processes = stoi(value, &sz);
      break;
    }
  }
  return processes;
  }

int RunningProcesses() { 
  string key, value, line;
  int processes_running;

  std::ifstream stream(kProcDirectory+kStatFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> key >> value;
    if(key == "procs_running"){
      string::size_type sz;
      processes_running = stoi(value, &sz);
      break;
    }
  }
  return processes_running;
  }

string Command(int pid) { 
  string line, command;

  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  
  return command; 
}

string Ram(int pid) { 
  string line, key, ram;

  std::ifstream stream(kProcDirectory+"/"+to_string(pid)+kStatusFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> key >> ram;
    if(key == "VmSize:"){
      break;
    }
  }

  return ram; 
  }

string Uid(int pid) { 
  string line, key, uid;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> uid;
      if(key == "Uid:"){
        break;
        }
      }

    }

  

  return uid; 
}

string User(int pid) { 
  string line, user, x, uid;

  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> uid;
      if(Uid(pid) == uid){
        break;
      }
    }
  }

  return user; 

}

string OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

long UpTime(int pid) { 

  string line, value;
  float pid_time;
  const float cpu_freq = 2.3e9; 
  const int stat_indice = 22;

  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i = 1;
    string::size_type sz;
    while(linestream >> value){
      if(i == stat_indice){
        pid_time = stof(value)/cpu_freq;
        break;
      }
      i++;
    }
  }

  return UpTime() - (long)pid_time; 
  }

float Utilization() { 

    float prev_idle = 0;
    float idle = 0;
    float prev_active = 0;
    float active = 0;
    float prev_total = 0;
    float total = 0;

    vector<string> old_jiffs = CpuUtilization();
    sleep(1);
    vector<string> new_jiffs = CpuUtilization();

    for(int i = 1; i < CPUStates::kSteal_; i++){
        if(i == CPUStates::kIdle_ || i == CPUStates::kIOwait_){
            prev_idle += stof(old_jiffs[i-1]);
            idle += stof(new_jiffs[i-1]);
        }
        else{
            prev_active += stof(old_jiffs[i-1]);
            active += stof(new_jiffs[i-1]);
        }
    }

    
    prev_total = prev_active + prev_idle;
    total = active + idle;

    float total_diff = total - prev_total;
    float idle_diff = idle - prev_idle; 
    std::cout << active << " " << prev_active << " " << idle << " " << prev_idle << "\n";
    std::cout << total_diff << " " << idle_diff << "\n";

    return 100 * (total_diff - idle_diff)/total_diff; 
}

int main(){
    
  /*vector<string> cpu_util{CpuUtilization()};

  for(int i = 0 ; i < cpu_util.size() ;i++){
    std::cout << cpu_util[i] << "\n";
  }*/

  std::cout << Utilization() << "\n";
}