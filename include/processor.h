#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  std::vector<float> CpuUtilParser();
  // TODO: Declare any necessary private members
 private:
    float oldcpu_active;
    float oldcpu_idle;
};

#endif