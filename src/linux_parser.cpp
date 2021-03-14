#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include "linux_parser.h"

using std::stol;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 

  float memTotal, memFree, memory_utilization;
  //float memAvailable, memBuffers, memCached;

  std::string key, value, unit, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
            memTotal = std::stof(value);
        } 
        else if (key == "MemFree:") {
            memFree = std::stof(value);
        }
      }      
    }
  }
  // calculation of mem usage
  memory_utilization = ((memTotal - memFree) / memTotal);
  return memory_utilization; 
}

//DONE Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime_tString;
  long int uptime_min;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_tString;
  }
  int up_s = std::stof(uptime_tString);
  uptime_min = ((int) up_s) /60;
  return uptime_min; 
}

long LinuxParser::Jiffies() 
{ //Read and return the number of jiffies for the system
  long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> cpuUtils = CpuUtilization();

  string juser = cpuUtils[CPUStates::kUser_];
  string jnice = cpuUtils[CPUStates::kNice_];
  string jsystem = cpuUtils[CPUStates::kSystem_];
  string jidle = cpuUtils[CPUStates::kIdle_];
  string jiowait = cpuUtils[CPUStates::kIOwait_];
  string jirq = cpuUtils[CPUStates::kIRQ_];
  string jsoftirq = cpuUtils[CPUStates::kSoftIRQ_];
  string jsteal = cpuUtils[CPUStates::kSteal_];
  string jguest = cpuUtils[CPUStates::kGuest_];
  string jguest_nice = cpuUtils[CPUStates::kGuestNice_];

  user =  std::stol(juser);
  nice = std::stol(jnice);
  system = std::stol(jsystem);
  idle = std::stol(jidle);
  iowait = std::stol(jiowait);
  irq = std::stol(jirq);
  softirq = std::stol(jsoftirq);
  steal = stol(jsteal);
  guest = stol(jguest);
  guest_nice = stol(jguest_nice);


  long usertime = user - guest;     // As you see here, it subtracts guest from user time
  long nicetime = nice - guest_nice; // and guest_nice from nice time
  long idlealltime = idle + iowait;  // ioWait is added in the idleTime
  long systemalltime = system + irq + softirq;
  long virtalltime = guest + guest_nice;
  long totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

  return  totaltime;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) 
{ 
  /*
  std::string cpu, line, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> list = {};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
    // cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
    linestream >> cpu >> user >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice; 

    list.push_back(user);
    list.push_back(system);
    list.push_back(idle);
    list.push_back(iowait);
    list.push_back(irq);
    list.push_back(softirq);
    list.push_back(guest);
    list.push_back(guest_nice);
  }
  */
  return 0;   
}


long LinuxParser::ActiveJiffies() 
{ // Read and return the number of active jiffies for the system 

  long user, nice, system, irq, softirq, steal;
  vector<string> cpuUtils = CpuUtilization();

  user = std::stol(cpuUtils[CPUStates::kUser_]);
  nice = std::stol(cpuUtils[CPUStates::kNice_]);
  system = std::stol(cpuUtils[CPUStates::kSystem_]);
  irq = std::stol(cpuUtils[CPUStates::kIRQ_]);
  softirq = std::stol(cpuUtils[CPUStates::kSoftIRQ_]);
  steal = std::stol(cpuUtils[CPUStates::kSteal_]);

  return user + nice + system + irq + softirq + steal;
}

long LinuxParser::IdleJiffies() 
{ // Read and return the number of idle jiffies for the system

  long idle, iowait;
  vector<string> cpuUtils = CpuUtilization();

  idle = std::stol(cpuUtils[CPUStates::kIdle_]);
  iowait = std::stol(cpuUtils[CPUStates::kIOwait_]);

  long totalIdle = idle + iowait;

  return totalIdle; 
}

// TODO: Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() 
{ 
  std::string cpu, line, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> list;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
    // cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice; 

    list.push_back(user);
    list.push_back(nice);
    list.push_back(system);
    list.push_back(idle);
    list.push_back(iowait);
    list.push_back(irq);
    list.push_back(softirq);
    list.push_back(steal);
    list.push_back(guest);
    list.push_back(guest_nice);
  }
  return list;   
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  int totalProcesses = 0;
  std::string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
            totalProcesses = std::stoi(value);
            break;
        } 
      }  
    }
  }
  return totalProcesses;   
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{
  int runningProcesses = 0;
  std::string key, value, line;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
            runningProcesses = std::stoi(value);
            break;
        } 
      }  
    }
  }
  return runningProcesses;   
}


string LinuxParser::Command(int pid) 
{ // Read and return the command associated with a process
  //Read file /proc/pid/cmdline
  std::string cmdLine, line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open())
  {
    if(std::getline(filestream, line))
    {
      cmdLine = line;
    }
  }
  return cmdLine; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) 
{ 
  
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
