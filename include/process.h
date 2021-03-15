#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
    Process(
      int id, 
      std::string cmd, 
      std::string uid, 
      std::string userName ,
      std::string ramSize_);
    
    
    int Pid();                               // TODO: See src/process.cpp                     
    std::string User();                      // TODO: See src/process.cpp
    std::string Command();                   // TODO: See src/process.cpp
    float CpuUtilization();                  // TODO: See src/process.cpp
    std::string Ram();                       // TODO: See src/process.cpp
    long int UpTime();                       // TODO: See src/process.cpp
    bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
 int id_;
 int ramMb_;
 std::string cmdLine_;
 std::string ramSize_;
 std::string uid_;
 std::string user_;
};

#endif