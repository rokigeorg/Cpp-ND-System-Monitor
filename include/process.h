#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int id, std::string cmd, std::string uid, std::string userName,
          std::string ram, long uptime, long activJiffies,
          long systemUptime_sec);

  int Pid();                               // Done: See src/process.cpp
  std::string User();                      // Done: See src/process.cpp
  std::string Command();                   // Done: See src/process.cpp
  float CpuUtilization() const;            // Done: See src/process.cpp
  std::string Ram();                       // Done: See src/process.cpp
  long int UpTime();                       // Done: See src/process.cpp
  bool operator<(Process const& a) const;  // Done: See src/process.cpp
  long GetRam_Mb() const;
  // Done: Declare any necessary private members
 private:
  int id_;
  std::string cmdLine_;
  std::string uid_;
  std::string user_;
  std::string ramSize_;
  long uptime_;
  long totalActivTime_;
  long systemUptime_sec_;
  long ramMb_;
  float cpuUtil_;
};

#endif