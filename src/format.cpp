#include <string>
#include <cmath>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    
    double hours;
    double minutes;

    string hours_str;
    string minutes_str;
    string seconds_str;

    hours = floor(seconds/3600);
    seconds -= hours*3600;

    minutes = floor(seconds/60);
    seconds -= minutes*60;


    return to_string(long(hours)) + ":" + to_string(int(minutes)) + ":" + to_string(int(seconds));
    
    }