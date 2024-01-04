#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using namespace std;

// the system's CPU
Processor& System::Cpu() { return cpu_; }

// the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  vector<int> ids = LinuxParser::Pids();
  for (auto pid : ids) {
    processes_.emplace_back(pid);
  }
  std::sort(processes_.begin(), processes_.end());

  return processes_;
}

// the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
