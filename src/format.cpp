#include <string>

#include "format.h"

using std::string;

static std::string AddZeroPrefix(int n)
{
  if(n < 10)
  {
    return "0"+std::to_string(n);
  }
  return std::to_string(n);
}

// Done: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hour = seconds / 3600;
  int second = seconds % 3600;
  int minute = second / 60;
  second = second % 60;
  string hhmmss = AddZeroPrefix(hour) + ":" + AddZeroPrefix(minute) + ":" +
                  AddZeroPrefix(second);
  return hhmmss;
}