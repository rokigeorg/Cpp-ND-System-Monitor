#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // Done: See src/system.cpp
  std::vector<Process>& Processes();  // Done: See src/system.cpp
  float MemoryUtilization();          // Done: See src/system.cpp
  long UpTime();                      // Done: See src/system.cpp
  int TotalProcesses();               // Done: See src/system.cpp
  int RunningProcesses();             // Done: See src/system.cpp
  std::string Kernel();               // Done: See src/system.cpp
  std::string OperatingSystem();      // Done: See src/system.cpp

  // Done: Define any necessary private members
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
};

#endif