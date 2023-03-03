#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"


using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
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


string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() { 

  string line, key, value;
  float memTotal, memFree, memBuffer;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "MemTotal:"){
        memTotal = stof(value);
      }
      else if(key == "MemFree:"){
        memFree = stof(value);
      }
      else if(key == "Buffers:"){
        memBuffer = stof(value);
      }
    }
  }
  return 1.0 - memFree/(memTotal-memBuffer); 
}


long LinuxParser::UpTime() { 
  
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


long LinuxParser::Jiffies() { 

  string line, value, key;
  float jiffies_total;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    for(int i ; i < CPUStates::kGuestNice_; i++){
      linestream >> value;
      jiffies_total += std::stof(value);
    }
  }

  return jiffies_total; 
}


long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  long active_jiffs = 0;

  std::ifstream stream(kProcDirectory+"/"+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < ProcessStates::kCSTime_; i++){
      linestream >> value;
      if(i > ProcessStates::kMajFault_){
        string::size_type sz;
        active_jiffs += stol(value, &sz);
      }
    }
  }
  return active_jiffs; 
}

long LinuxParser::ActiveJiffies() { 
  string line, value;
  long active_jiffies_sys;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    string::size_type sz;
    for(int i = 0; i < CPUStates::kGuestNice_; i++){
      linestream >> value;
      if((i >= CPUStates::kUser_ && i < CPUStates::kIdle_) || i > CPUStates::kIOwait_){
        active_jiffies_sys += stol(value, &sz);
      }
    }
  }

  return active_jiffies_sys; 
  }



long LinuxParser::IdleJiffies() { 
  string line, value;
  long idle_jiffies_sys;


  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    string::size_type sz;
    for(int i = 0; i <= CPUStates::kIOwait_; i++){
      linestream >> value;
      if(i == CPUStates::kIdle_ || i == CPUStates::kIOwait_){
        idle_jiffies_sys += stol(value, &sz);
      }
    }
  }

  return idle_jiffies_sys; 
}


vector<string> LinuxParser::CpuUtilization() { 
  string line, value;
  vector<string> cpu_jiffies;

  std::ifstream stream(kProcDirectory+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    for(int i = 0; i < CPUStates::kGuestNice_; i++){
      linestream >> value;
      cpu_jiffies.push_back(value);
    }
  }

  return cpu_jiffies; 
  }


int LinuxParser::TotalProcesses() { 
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


int LinuxParser::RunningProcesses() { 
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

vector<float> LinuxParser::CpuUtilParser(){
    float active_jiffs;
    float idle_jiffs;
    std::vector<string> jiffs = CpuUtilization();

    for(int i = 0; i < CPUStates::kSteal_; i++){
      if(jiffs[i] != "cpu"){
        if(i == CPUStates::kIdle_ || i == CPUStates::kIOwait_){
          idle_jiffs += stof(jiffs[i]);
        }
        else{
          active_jiffs += stof(jiffs[i]);
        }
      }
    }

    return vector<float> {active_jiffs, idle_jiffs};
}


string LinuxParser::Command(int pid) { 
  string line, command;

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  
  return command; 
}


string LinuxParser::Ram(int pid) { 
  string line, key, ram;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  while(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> key >> ram;
    if(key == "VmSize:"){
      return ram;
    }
    if(stream.eof()){
      ram = "0";
      return ram;
    }
  }

  return "";
  }

string LinuxParser::Uid(int pid) { 
  string line, key, uid;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      linestream >> key >> uid;
      if(key == "Uid:"){
        return uid;
        }
      }

    }

  return ""; 
}

string LinuxParser::User(int pid) { 
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
  user.resize(4);
  return user; 

}

long LinuxParser::UpTime(int pid) { 

  string line, value;
  float pid_time;

  std::ifstream stream(kProcDirectory+to_string(pid)+kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i = 1;
    while(linestream >> value){
      if(i == ProcessStates::kStartTime_){
        pid_time = stof(value)/sysconf(_SC_CLK_TCK);
      }
      i++;
    }
  }

  return UpTime() - (long)pid_time; 
}