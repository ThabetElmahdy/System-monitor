#include "format.h"

#include <iomanip>
#include <string>

using std::setfill;
using std::setw;
using std::string;
using std::stringstream;

string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int second = seconds % 60;
  stringstream ss;
  ss << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << ":"
     << setw(2) << second;
  return ss.str();
}