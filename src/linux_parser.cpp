#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>

#include "linux_parser.h"

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
                /*
        // TODO extent memory usage calculation 
        // https://stackoverflow.com/questions/41224738/how-to-calculate-system-memory-usage-from-proc-meminfo-like-htop

         else if (key == "Buffers:") {
            memBuffers = std::stof(value);
        }
        else if (key == "MemAvailable:") {
            memAvailable = std::stof(value);
        }
        else if (key == "Cached:") {
            memCached = std::stof(value);
        }
        */
      }
      
    }
  }
  // calculation of mem usage
  memory_utilization = (((memTotal - memFree) * 100) / memTotal)  ;
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


struct ProcTime{
long int usertime;
long int nicetime ; 
unsigned long long int idlealltime;
unsigned long long int systemalltime;
unsigned long long int virtalltime;
unsigned long long int totaltime;
};
static ProcTime pT;

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies(ProcTime &pT) 
{ 
  long int userJ, niceJ, systemJ, idleJ, iowaitJ, irqJ, softirqJ, stealJ, guestJ, guestNiceJ;
  std::string key, line, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
 
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) 
    {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) 
      {
        if (key == "cpu") 
        {
          //     user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
          // cpu  74608   2520   24433   1117073   6176   4054  0        0      0      0
            userJ = std::stoi(user);
            niceJ = std::stoi(nice);
            systemJ = std::stoi(system);
            idleJ = std::stoi(idle);
            iowaitJ = std::stoi(idle);
            irqJ = std::stoi(irq);
            softirqJ = std::stoi(softirq);
            stealJ = std::stoi(steal);
            guestJ = std::stoi(guest);
            guestNiceJ = std::stoi(guest_nice);
            break;
        } 
      }  
    }
  }

  // Guest time is already accounted in usertime
  pT.usertime = userJ - guestJ;     // As you see here, it subtracts guest from user time
  pT.nicetime = niceJ - guestNiceJ; // and guest_nice from nice time
  pT.idlealltime = idleJ + iowaitJ;  // ioWait is added in the idleTime
  pT.systemalltime = systemJ + irqJ + softirqJ;
  pT.virtalltime = guestJ + guestNiceJ;
  pT.totaltime = pT.usertime + pT.nicetime + pT.systemalltime + pT.idlealltime + stealJ + pT.virtalltime;

  return pT.totaltime; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

static time_t* GetTime()
{
  time_t* pNow = nullptr;
  ctime(pNow);
  return pNow;
}


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  
  static ProcTime prevPt, currPt;

  Jiffies(prevPt);

  Jiffies(currPt);

  while((currPt.totaltime - prevPt.totaltime) > 10000)
  {
    Jiffies(currPt);
  }
  long totaled = currPt.totaltime - prevPt.totaltime;
  long idled = currPt.idlealltime - prevPt.idlealltime;

long activeJiffies_percent = (totaled - idled) / totaled *100;
  return activeJiffies_percent; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  Jiffies();
  return pT.idlealltime; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { return {}; }

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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
