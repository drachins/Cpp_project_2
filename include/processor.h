#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

#include "system.h"

class Processor {
 public:

  float Utilization();  // TODO: See src/processor.cpp
  // TODO: Declare any necessary private members
 private:
  float oldcpu_active;
  float oldcpu_idle;

};

#endif