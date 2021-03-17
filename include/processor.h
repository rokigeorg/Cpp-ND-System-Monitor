#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();  // Done: See src/processor.cpp

  // Done: Declare any necessary private members
 private:
  long currTotal;
  long prevTotal;
  long currIdle;
  long prevIdle;
  float cpuUtil;
};

#endif