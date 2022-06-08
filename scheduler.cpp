#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct Process {
  int pid;
  int arrivalTime;
  int burstTime;
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

void createProcessStructs(vector<int> &vecOfNums, vector<Process> &vecOfProcesses) {
  for (int idx = 0; idx < vecOfNums.size(); idx+=3) {
    Process temp;
    temp.pid = vecOfNums[idx];
    temp.arrivalTime = vecOfNums[idx+1];
    temp.burstTime = vecOfNums[idx+2];
    vecOfProcesses.push_back(temp);
  }
}

void swap(vector<Process> &processes, int swapIdx1, int swapIdx2) {
  Process temp = processes[swapIdx1];
  processes[swapIdx1] = processes[swapIdx2];
  processes[swapIdx2] = temp;
}

void bubbleSortByArrival(vector<Process> &processes, int n)
{
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
  vector<Process> fcfsProcesses = processes;
  bubbleSortByArrival(fcfsProcesses, fcfsProcesses.size());

  int time = 0;
  int duration = 0;
  cout << "times  |  Process  |  duration\n";
  for (auto process : fcfsProcesses) {
    duration = process.burstTime;
    cout << time << "      |     " << process.pid << "     |      " << duration << "\n";
    time += duration;
  }

}

int main (int argc, char *argv[]) {
  
  string processesString;
  vector<int> vecOfNums;
  vector<Process> vecOfProcesses;
  
  ifstream processesFile;
  processesFile.open (argv[1]);
  if (!processesFile.is_open())
    cerr << "File not found!";

    
  parseFileToString(processesString, processesFile);
  getNumberFromString(processesString, vecOfNums);
  createProcessStructs(vecOfNums, vecOfProcesses);
  firstComeFirstServe(vecOfProcesses);

  for (Process temp : vecOfProcesses) {
      cout << "PID: " << temp.pid << " ARRIVAl TIME: " << temp.arrivalTime << " BURST TIME: " << temp.burstTime << "\n";
  }
    
  processesFile.close();
  return 0;
}