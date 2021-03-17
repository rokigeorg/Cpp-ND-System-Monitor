#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Done: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

vector<Process>&
System::Processes() {  // Return a container composed of the system's processes

  vector<int> listPids = LinuxParser::Pids();
  for (int id : listPids) {
    Process p = Process(id, LinuxParser::Command(id), LinuxParser::Uid(id),
                        LinuxParser::User(id), LinuxParser::Ram(id),
                        LinuxParser::UpTime(id), LinuxParser::ActiveJiffies(id),
                        LinuxParser::UpTime());

    processes_.push_back(p);
  }
  std::sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

// Done: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Done: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Done: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Done: Return the number of seconds since the system started running
long int System::UpTime() {
  long x = LinuxParser::UpTime();
  return x;
}