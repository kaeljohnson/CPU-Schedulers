#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

class GantChart;
class Process;
void firstComeFirstServe(vector<Process> processes);
void roundRobbin(vector<Process> processes, int timeQuantum);
void bubbleSortByArrival(vector<Process> &processes, int n);
void swap(vector<Process> &processes, int swapIdx1, int swapIdx2);
void createProcesses(vector<int> &vecOfNums, vector<Process> &vecOfProcesses);
void getNumberFromString(string &processesString, vector<int> &vecOfNums);
void parseFileToString(string &processesString, ifstream &processesFile);


class GantChart {
  public:
    vector<vector<int>> m_processStream;
    string m_type;

    GantChart(vector<vector<int>> &processStream, string type) {
      m_processStream = processStream;
      m_type = type;
    }

    void printChart() {
      cout << "        times      |     pid     |   duration  |   time remaining\n";
      cout << "-------------------------------------------------------------\n";
      if (m_type == "fcfs") {
        for (auto p : m_processStream) {
          cout << setw(11) << p[1] << setw(9) 
             << "|" << setw(7) << p[2] << setw(7) 
             << "|" << setw(7) << p[3] << setw(7) 
             << "|" << setw(7) << p[4] << "\n";
        }
      }
      if (m_type == "rr") {
        for (auto p : m_processStream) {
          cout << setw(9) << p[1]<< setw(2) << "-" << setw(2) << p[0] << setw(7) 
             << "|" << setw(7) << p[2] << setw(7) 
             << "|" << setw(7) << p[3] << setw(7) 
             << "|" << setw(7) << p[4] << "\n";
        }
      }
    }
};

class Process {
  public:
    int pid;
    int arrivalTime;
    int burstTime;
    int timeRemaining;

    Process(int currPid, int currArrivalTime, int currBurstTime) {
      pid = currPid;
      arrivalTime = currArrivalTime;
      burstTime = currBurstTime;
      timeRemaining = currBurstTime;
    }
};


void parseFileToString(string &processesString, ifstream &processesFile) {
  string tempFile;
  while (getline (processesFile, tempFile)) {
    processesString.append(tempFile);
  }
}


void getNumberFromString(string &processesString, vector<int> &vecOfNums) {
   stringstream ss;
   ss << processesString; 
   string tempString;

   int temp_int;
   while(!ss.eof()) {
      ss >> tempString; 
      if(stringstream(tempString) >> temp_int) { 
         vecOfNums.push_back(temp_int);
      }
      tempString = "";
   }
}


void createProcesses(vector<int> &vecOfNums, vector<Process> &vecOfProcesses) {
  for (int idx = 0; idx < vecOfNums.size(); idx+=3) {
    Process temp(vecOfNums[idx], vecOfNums[idx+1], vecOfNums[idx+2]);
    vecOfProcesses.push_back(temp);
  }
}


void swap(vector<Process> &processes, int swapIdx1, int swapIdx2) {
  Process temp = processes[swapIdx1];
  processes[swapIdx1] = processes[swapIdx2];
  processes[swapIdx2] = temp;
}


void bubbleSortByArrival(vector<Process> &processes, int n) {
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - i - 1; j++)
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime)
                swap(processes, j, j + 1);
    for (Process temp : processes) {
      cout << "PID: " << temp.pid << " ARRIVAl TIME: " << temp.arrivalTime << " BURST TIME: " << temp.burstTime << "\n";
    }
}


void firstComeFirstServe(vector<Process> processes) {
  vector<Process> &fcfsProcesses = processes;
  bubbleSortByArrival(fcfsProcesses, fcfsProcesses.size());
  vector<vector<int>> gant;

  int time = 0;
  int duration = 0;
  for (auto process : fcfsProcesses) {
    duration = process.burstTime;
    time += duration;
    gant.push_back({0, time, process.pid, duration, 0});
  }

  GantChart fcfsGantChart(gant, "fcfs");
  fcfsGantChart.printChart();
}

void roundRobbin(vector<Process> processes, int timeQuantum) {
  vector<vector<int>> display;
  bubbleSortByArrival(processes, processes.size());
  vector<Process> processQueue;
  int time = 0;

  while (processes.size() || processQueue.size()) {
    
    if (processes.size() && processes[0].arrivalTime <= time) {
      processQueue.push_back(processes[0]);
      processes.erase(processes.begin());
    } 
    if (!processQueue.size()) {
      time+=timeQuantum;
      display.push_back({time, time-timeQuantum+1, -1, -1, -1});
      continue;
    }

    Process &currProcess = processQueue[0];

    int timeQorTimeRemainingLower = currProcess.timeRemaining < timeQuantum ? currProcess.timeRemaining : timeQuantum;

    time += timeQuantum;
    currProcess.timeRemaining -= timeQorTimeRemainingLower;
    
    display.push_back({time, time-timeQuantum+1, currProcess.pid, timeQorTimeRemainingLower, currProcess.timeRemaining});

    if (currProcess.timeRemaining > 0) {
      processQueue.push_back(currProcess);
    }
    processQueue.erase(processQueue.begin());
  }
  
  GantChart roundRobbinGantChart(display, "rr");
  roundRobbinGantChart.printChart();

}

int main (int argc, char *argv[]) {
  
  string processesString;
  vector<int> vecOfNums;
  vector<Process> vecOfProcesses;
  
  ifstream processesFile;
  processesFile.open (argv[1]);
  if (!processesFile.is_open()) {
    cerr << "File not found!";
    exit(1);
  }
  if (!argv[2]) {
    cerr << "Missing Time Quantum!";
    exit(1);
  } 
  int timeQuantum = atoi(argv[2]);

  parseFileToString(processesString, processesFile);
  getNumberFromString(processesString, vecOfNums);
  createProcesses(vecOfNums, vecOfProcesses);
  firstComeFirstServe(vecOfProcesses);
  roundRobbin(vecOfProcesses, timeQuantum);
    
  processesFile.close();
  return 0;
}