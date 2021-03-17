#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int id, std::string cmd, std::string uid, std::string userName,
                 std::string ram, long uptime, long activJiffies,
                 long systemUptime_sec)
    : id_(id),
      cmdLine_(cmd),
      uid_(uid),
      user_(userName),
      ramSize_(ram),
      uptime_(uptime),
      totalActivTime_(activJiffies),
      systemUptime_sec_(systemUptime_sec) {
  ramMb_ = std::stol(ramSize_);

  long seconds = systemUptime_sec_ - uptime_;
  try {
    cpuUtil_ = float(totalActivTime_) / float(seconds);

  } catch (...) {
    cpuUtil_ = 0;
  }
}

int Process::Pid() {  // Return this process's ID
  return id_;
}

float Process::CpuUtilization() const {  // Return this process's CPU
                                         // utilization
  return cpuUtil_;
}

string Process::Command() {  // Return the command that generated this process
  return cmdLine_;
}

string Process::Ram() {  // Return this process's memory utilization
  return ramSize_;
}

string Process::User() {  // Return the user (name) that generated this process
  return user_;
}

long int Process::UpTime() {  // Return the age of this process (in seconds)
  return uptime_;
}

long Process::GetRam_Mb() const { return ramMb_; }

bool Process::operator<(Process const& a) const {
  // Overload the "less than" comparison operator for Process objects
  return cpuUtil_ < a.CpuUtilization();
  // return ramMb_ < a.GetRam_Mb();
}