#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {
  currTotal = LinuxParser::Jiffies();
  currIdle = LinuxParser::IdleJiffies();
  prevTotal = LinuxParser::Jiffies();
  prevIdle = LinuxParser::IdleJiffies();
  cpuUtil = 0;
}

float Processor::Utilization() {  // Return the aggregate CPU utilization
  float totaled = 0;
  float idled = 0;

  currIdle = LinuxParser::IdleJiffies();
  currTotal = LinuxParser::Jiffies();

  totaled = float(currTotal) - float(prevTotal);
  idled = float(currIdle) - float(prevIdle);

  float actived = (totaled - idled);
  // avoid to divite by 0
  if (actived > 0) {
    cpuUtil = actived / totaled;
  }

  prevIdle = currIdle;
  prevTotal = currTotal;

  return cpuUtil;
}