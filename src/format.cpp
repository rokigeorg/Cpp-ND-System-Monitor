#include <string>

#include "format.h"

using std::string;

// Done: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hour = seconds / 3600;
  int second = seconds % 3600;
  int minute = second / 60;
  second = second % 60;
  string hhmmss = std::to_string(hour) + ":" + std::to_string(minute) + ":" +
                  std::to_string(second);
  return hhmmss;
}