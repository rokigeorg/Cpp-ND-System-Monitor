#include <dirent.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
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

float LinuxParser::MemoryUtilization() {  // Read and return the system memory
                                          // utilization
  float memTotal, memFree, memory_utilization;
  std::string key, value, unit, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          memTotal = std::stof(value);
        } else if (key == "MemFree:") {
          memFree = std::stof(value);
        }
      }
    }
  }
  // calculation of mem usage
  memory_utilization = ((memTotal - memFree) / memTotal);
  return memory_utilization;
}

long LinuxParser::UpTime() {  // Read and return the system uptime
  string uptime_tString;
  long int uptime_sec;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_tString;
  }
  uptime_sec = std::stol(uptime_tString);

  return uptime_sec;
}

long LinuxParser::Jiffies() {  // Read and return the number of jiffies for the
                               // system
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

  user = std::stol(juser);
  nice = std::stol(jnice);
  system = std::stol(jsystem);
  idle = std::stol(jidle);
  iowait = std::stol(jiowait);
  irq = std::stol(jirq);
  softirq = std::stol(jsoftirq);
  steal = stol(jsteal);
  guest = stol(jguest);
  guest_nice = stol(jguest_nice);

  long usertime =
      user - guest;  // As you see here, it subtracts guest from user time
  long nicetime = nice - guest_nice;  // and guest_nice from nice time
  long idlealltime = idle + iowait;   // ioWait is added in the idleTime
  long systemalltime = system + irq + softirq;
  long virtalltime = guest + guest_nice;
  long totaltime =
      usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;

  return totaltime;
}

long LinuxParser::ActiveJiffies(
    int pid) {  // Read and return the number of active jiffies for a PID

  std::string value, line;
  vector<string> list = {};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    while (linestream >> value) {
      list.push_back(value);
    }
  }

  long utime = 0, stime = 0, cutime = 0, cstime = 0;

  if (std::all_of(list[13].begin(), list[13].end(), isdigit))
    utime = stol(list[13]);
  if (std::all_of(list[14].begin(), list[14].end(), isdigit))
    stime = stol(list[14]);
  if (std::all_of(list[15].begin(), list[15].end(), isdigit))
    cutime = stol(list[15]);
  if (std::all_of(list[16].begin(), list[16].end(), isdigit))
    cstime = stol(list[16]);

  long totaltime = utime + stime + cutime + cstime;
  return totaltime / sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies() {  // Read and return the number of active
                                     // jiffies for the system

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

long LinuxParser::IdleJiffies() {  // Read and return the number of idle jiffies
                                   // for the system

  long idle, iowait;
  vector<string> cpuUtils = CpuUtilization();

  idle = std::stol(cpuUtils[CPUStates::kIdle_]);
  iowait = std::stol(cpuUtils[CPUStates::kIOwait_]);

  long totalIdle = idle + iowait;

  return totalIdle;
}

std::vector<std::string>
LinuxParser::CpuUtilization() {  // Read and return CPU utilization
  std::string cpu, line, user, nice, system, idle, iowait, irq, softirq, steal,
      guest, guest_nice;
  vector<string> list;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //     user    nice   system  idle      iowait irq   softirq  steal  guest
    //     guest_nice
    // cpu  74608   2520   24433   1117073   6176   4054  0        0      0 0
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;

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

int LinuxParser::TotalProcesses() {  // Read and return the total number of
                                     // processes
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

int LinuxParser::RunningProcesses() {  // Read and return the number of running
                                       // processes
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

string LinuxParser::Command(
    int pid) {  // Read and return the command associated with a process
  // Read file /proc/pid/cmdline
  std::string cmdLine, line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      cmdLine = line;
    }
  }
  return cmdLine;
}

string LinuxParser::Ram(
    int pid) {  // Read and return the memory used by a process
  std::string key, value, line;
  std::string ramSize = "0";
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          ramSize = value;
          break;
        }
      }
    }
  }
  return ramSize;
}

string LinuxParser::Uid(
    int pid) {  // Read and return the user ID associated with a process
  std::string key, value, line, uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          uid = std::stoi(value);
          break;
        }
      }
    }
  }

  return uid;
}

string LinuxParser::User(
    int pid) {  // Read and return the user associated with a process
  std::string username, x, uidNumber, line;
  std::string myUser = "Default";
  std::ifstream filestream(kPasswordPath);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> x >> uidNumber) {
        if (uidNumber == std::to_string(pid)) {
          myUser = username;
          break;
        }
      }
    }
  }

  return myUser;
}

long LinuxParser::UpTime(int pid) {  // Read and return the uptime of a process
  std::string key, value, line;
  vector<string> values;
  long starttime = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  try {
    starttime = stol(values[21]) / sysconf(_SC_CLK_TCK);
  } catch (...) {
    starttime = 0;
  }
  return UpTime() - starttime;
}
