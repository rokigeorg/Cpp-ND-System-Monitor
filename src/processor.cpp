#include "processor.h"
#include "linux_parser.h"

Processor::Processor()
{
    currTotal = LinuxParser::Jiffies();
    currIdle = LinuxParser::IdleJiffies();
    prevTotal = LinuxParser::Jiffies();
    prevIdle = LinuxParser::IdleJiffies();
}

float Processor::Utilization() 
{ //Return the aggregate CPU utilization
    float totaled , idled;

    currIdle = LinuxParser::IdleJiffies();
    currTotal = LinuxParser::Jiffies();

    totaled = float(currTotal) - float(prevTotal);
    idled = float(currIdle) - float(prevIdle);

    float cpuUtil = (totaled - idled) / totaled ;

    prevIdle = currIdle;
    prevTotal = currTotal;  

    return 0; 
}