#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string key;
  string line;
  float memTotal, memFree, value;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == filterMemTotalString) {
        memTotal = value;
      }
      if (key == filterMemFreeString) {
        memFree = value;
        break;
      }
    }
  }

  return (memTotal - memFree) / memTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime;

  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line, cpu;
  long jiffies, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
    jiffies = user + nice + system + idle + iowait + irq + softirq + steal +
              guest + guest_nice;
  }
  return jiffies;
}

//  Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    vector<string> jiff;

    while (linestream >> value) {
      jiff.push_back(value);
    }
    long utime = std::stol(jiff[13]);
    long ctime = std::stol(jiff[14]);
    long cutime = std::stol(jiff[15]);
    long cstime = std::stol(jiff[16]);

    return utime + ctime + cutime + cstime;
  }
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line, value;
  vector<string> Jiffies;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      Jiffies.push_back(value);
    }
  }
  long idle = std::stol(Jiffies[4]);
  long iowait = std::stol(Jiffies[5]);

  return idle + iowait;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, value, cpu;
  vector<string> values;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      values.push_back(value);
    }
    return values;
  }
  return {};
}

//  Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key;
  int total_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == filterProcesses) linestream >> total_processes;
    }
  }
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  int running_processes;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == filterRunningProcesses) linestream >> running_processes;
    }
  }
  return running_processes;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line, cmd;

  if (stream.is_open()) {
    std::getline(stream, line);
    std::stringstream linestrem(line);
    linestrem >> cmd;
    return cmd;
  }
  return string();
}

//  Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key;
  int ram;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == filterProcMem) {
        linestream >> ram;
      }
    }
    return to_string(ram / 1024);
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, uid;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == filterUID) {
        linestream >> uid;
        return uid;
        break;
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, value1, value2, user;
  string uid = Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> value1 >> value2;
      if (value2 == uid) {
        break;
      }
    }
    return user;
  }

  return string();
}
//  Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, value;
  long uptime;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    vector<string> tokens;
    while (linestream >> value) {
      tokens.push_back(value);
    }
    try {
      uptime = LinuxParser::UpTime() - stol(tokens[21]) / sysconf(_SC_CLK_TCK);

    } catch (std::invalid_argument& error) {
      return 0;
    }

    return uptime;
  }
  return 0;
}
