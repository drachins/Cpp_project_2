#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>


class Processor {
 public:

  float Utilization();  // TODO: See src/processor.cpp
  // TODO: Declare any necessary private members
 private:
  float oldcpu_active = 0;
  float oldcpu_idle = 0;

};

#endif