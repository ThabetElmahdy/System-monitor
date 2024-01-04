#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid(pid) {}
// this process's ID
int Process::Pid() { return pid; }

// this process's CPU utilization
float Process::CpuUtilization() const {
  return float(LinuxParser::ActiveJiffies(pid) / LinuxParser::Jiffies());
}

// the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid); }

// the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& other) const {
  return this->CpuUtilization() < other.CpuUtilization();
}

void Process::Pid(int pid) { this->pid = pid; }
