#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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

    GantChart(vector<vector<int>> &processStream) {
      m_processStream = processStream;
    }

    void printChart() {
      cout << "times   |    pid    |   duration   |    time remaining\n";
      for (auto p : m_processStream) {
        cout << p[0] << "       |     " << p[1] << "        |    " << p[2] << "   |          " << p[3] << "\n";
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
    gant.push_back({time, process.pid, duration, 0});
    
    time += duration;
  }

  GantChart fcfsGantChart(gant);
  fcfsGantChart.printChart();
}

void roundRobbin(vector<Process> processes, int timeQuantum) {
  vector<vector<int>> display;
  bubbleSortByArrival(processes, processes.size());
  vector<Process> processQueue;
  int time = 0;

  while (processes.size()) {
    if (processes[0].arrivalTime <= time) {
      processQueue.push_back(processes[0]);
      processes.erase(processes.begin());
    } else if (!processQueue.size()) {
      time+=timeQuantum;
      continue;
    }

    Process &currProcess = processQueue[0];
    
    time+= (currProcess.timeRemaining < timeQuantum ? currProcess.timeRemaining : timeQuantum);
    currProcess.timeRemaining -= (currProcess.timeRemaining < timeQuantum ? currProcess.timeRemaining : timeQuantum);
    display.push_back({time, currProcess.pid, timeQuantum, currProcess.timeRemaining});
    if (currProcess.timeRemaining > 0) {
      processQueue.push_back(currProcess);
    }
    processQueue.erase(processQueue.begin());
  }
  
  GantChart roundRobbinGantChart(display);
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