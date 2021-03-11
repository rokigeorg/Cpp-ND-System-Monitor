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
    long totaled , idled;

    currIdle = LinuxParser::IdleJiffies();
    currTotal = LinuxParser::Jiffies();

    totaled = currTotal - prevTotal;
    idled = currIdle - prevIdle;

    long cpuUtil = (totaled - idled) / totaled ;

    prevIdle = currIdle;
    prevTotal = currTotal;  

    return float(cpuUtil); 
}