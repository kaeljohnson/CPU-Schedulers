import sys
import os
from operator import itemgetter, attrgetter


def formating(importantVals, algo, ganttArr, burstSum, sPs):            #Function to format output
    numPs = importantVals.pop(0)
    importantValString = ""
    print("----------------- {} -----------------".format(algo))        #Print each processes waiting times and 
    print("Process ID     | Waiting Time     | Turnaround Time")        #turnaround times
    sortedVals = sorted(importantVals, key=lambda vals: vals[0])
    for i in sortedVals:
        print("     {:<5}     |     {:<5}        |     {:<5}     ".format(i[0], i[1], i[2]))



    print("Gantt Chart is:")                                            #print the gantt chart
    ganttString = ""
    for i in ganttArr:
        
        ganttString+="[ {:<5} ]-- {:<5} --[ {:<5} ]\n".format(i[0], i[1], i[2])
    print(ganttString)



    avgWt = 0                                                           #calculate average times and print them
    wtCounter = 0
    for i in importantVals:
        avgWt+=i[1]
        wtCounter+=1
    avgWt = avgWt/wtCounter
    avgtAt = 0
    tAtCounter = 0
    for i in importantVals:
        avgtAt+=i[2]
        tAtCounter+=1
    avgtAt = avgtAt/tAtCounter

    print("Average Waiting time: {}".format(avgWt))
    print("Average Turnaround time: {}".format(avgtAt))
    print("Throughput: {}".format(numPs/burstSum))


class Process:                                                          #process class
    latestStartTime = 0                                               
    waitTime = 0
    turnAroundTime = 0
    origBurstTime = 0
    burstTime = 0
    def __init__(self, pid, arrivalTime, burstTime):
        self.pid = pid
        self.arrivalTime = arrivalTime
        self.burstTime = burstTime
        self.origBurstTime = burstTime

class SJFP:                                                             #sjfp class
    sPs = []
    sPsLen = 0
    def __init__(self, sortedProcesses):
        
        self.sPs = sorted(sortedProcesses, key=attrgetter('arrivalTime', 'burstTime', 'pid'))  #sort array of processes
        self.sPsLen = len(sortedProcesses)
    def getVals(self):                          #function that calls sjf algorithm
        shortestJobFirstP(self.sPs)

def shortestJobFirstP(sPs):                     #sjfp algorithm
    currTime = 0
    readyQ = []
    ganttArr = []
    importantVals = [len(sPs)]
    #The first element in the readyQ should always be the running Ps    
    
    while len(readyQ) != 0 or len(sPs) != 0:
        #if next process in sPs has arrival time equal to current time, put it in readyQ
        if len(sPs) != 0 and sPs[0].arrivalTime == currTime:
            readyQ.append(sPs[0])
            sPs.pop(0)
            continue 
        #if theres nothing in the readyQ, there will be idle time between current time and next 
        #qualified process    
        if len(readyQ) == 0:
            ganttArr.append([currTime, "IDLE", sPs[0].arrivalTime]) 
            currTime = sPs[0].arrivalTime
            readyQ.append(sPs[0])
            readyQ[0].latestStartTime = currTime
            sPs.pop(0)
            
            continue
        #if the current process is done executing, it can leave the readyQ and post its final data
        #current process becomes first element in readyQ
        if readyQ[0].burstTime == 0:
            ganttArr.append([readyQ[0].latestStartTime, readyQ[0].pid, currTime])
            turnAroundTime = currTime-readyQ[0].arrivalTime
            waitTime = turnAroundTime - readyQ[0].origBurstTime
            importantVals.append([readyQ[0].pid, waitTime, turnAroundTime]) 
            readyQ.pop(0)
            if len(readyQ) != 0:
                readyQ[0].latestStartTime = currTime
            continue
        elif len(readyQ) == 1:
            readyQ[0].burstTime-=1
            currTime+=1
            continue
        #if the next process in the ready Q has a burst time smaller than the current processes burst time
        #the process in the readyQ pre-empts the running process
        elif readyQ[1].burstTime < readyQ[0].burstTime and readyQ[1] != readyQ[0]:
            #if they need to be swapped, account for most recent interval in gantt chart
            if readyQ[0].latestStartTime != currTime:
                ganttArr.append([readyQ[0].latestStartTime, readyQ[0].pid, currTime])
            temp = readyQ[0]
            readyQ[0] = readyQ[1]
            readyQ[1] = temp
            readyQ[0].latestStartTime = currTime
            tempSort = sorted(readyQ, key=attrgetter('burstTime', 'pid'))
            readyQ = tempSort
            continue
        tempSort = sorted(readyQ, key=attrgetter('burstTime', 'pid'))
        readyQ = tempSort    
        readyQ[0].burstTime-=1
        currTime+=1
    
    formating(importantVals, "SJFP", ganttArr, currTime, sPs)

    return   





def firstComeFirstServe(currTime, pNum, burstSum, sortedProcesses, ganttArr, importantVals):    #fcfs algorithm
        #base case. If every process has been checked, post data and return
        if pNum >= len(sortedProcesses):
            importantVals.insert(0, len(sortedProcesses))
            formating(importantVals, "FCFS", ganttArr, burstSum, sortedProcesses) 
            return 
        else:
            currPs = sortedProcesses[pNum]
        #if next process has not arrived, IDLE
        if currTime < currPs.arrivalTime:
            ganttArr.append([currTime, "IDLE", currPs.arrivalTime])
            burstSum+=currPs.arrivalTime-currTime  
            currTime = currPs.arrivalTime
            firstComeFirstServe(currTime, pNum, burstSum, sortedProcesses, ganttArr, importantVals)
        #If there is a process ready, run it until completion
        elif currTime > currPs.arrivalTime:
            ganttArr.append([currTime, currPs.pid, currTime+currPs.burstTime])
            turnAroundTime = currTime+currPs.burstTime-currPs.arrivalTime
            waitTime = turnAroundTime - currPs.burstTime
            importantVals.append([currPs.pid, waitTime, turnAroundTime])
            burstSum+=currPs.burstTime
            currTime+=currPs.burstTime
            firstComeFirstServe(currTime, pNum+1, burstSum, sortedProcesses, ganttArr, importantVals)
        #basically same thing, I don't know why i made this seperate
        else:
            ganttArr.append([currPs.arrivalTime, currPs.pid, currPs.arrivalTime+currPs.burstTime])
            turnAroundTime = currPs.arrivalTime+currPs.burstTime-currPs.arrivalTime
            waitTime = turnAroundTime - currPs.burstTime
            importantVals.append([currPs.pid, waitTime, turnAroundTime])
            burstSum+=currPs.burstTime            
            currTime+=currPs.burstTime
            firstComeFirstServe(currTime, pNum+1, burstSum, sortedProcesses, ganttArr, importantVals)
        return
class FCFS:                         #FCFS class
    sortedProcesses = []
    burstSum = 0
    ganttArr = []
    newGanttArr = []
    importantVals = []
    def __init__(self, sortedProcesses):
        self.sortedProcesses = sorted(psArray, key=attrgetter('arrivalTime','pid'))

    def getVals(self):              #function that calls fcfs algorithm
        firstComeFirstServe(0, 0, self.burstSum, self.sortedProcesses, self.ganttArr, self.importantVals) 



class RR:                           #RR class
    sPs = []
    sPsLen = 0
    timeQ = 0
    def __init__(self, sortedProcesses, timeQ):
        self.sPs = sorted(psArray1, key=attrgetter('arrivalTime', 'pid'))
        self.timeQ = timeQ
        self.sPsLen = len(sortedProcesses)
    def getVals(self):                          #function to run RR algorithm
        roundRobin(self.sPs, self.timeQ)

def roundRobin(sPs, timeQ):                 #RR function
    ganttArr = []
    importantVals = [len(sPs)]
    readyQ = []
    currTime = 0
    burstSum = 0
    for i in sPs:                           #find burst sum so we know when to stop
        burstSum+=i.burstTime
    while burstSum != 0:
        
        if len(sPs) != 0:                   #check to see if next process has arrived
            
            while True:
                i = 0
                if len(sPs) == 0:
                    break
                if sPs[i].arrivalTime <= currTime:
                    readyQ.append(sPs[i])
                    sPs.pop(i)
                    continue
                else:
                    break
                

        #    readyQ.append(sPs[0])
        #    sPs.pop(0)
        #    continue
        if len(readyQ) == 0:                                    #if there are no processes ready, IDLE
            ganttArr.append([currTime, "IDLE", sPs[0].arrivalTime])
            currTime = sPs[0].arrivalTime
            continue
        if readyQ[0].burstTime <= timeQ:                        #if process ends during time quantum, post its data
            ganttArr.append([currTime, readyQ[0].pid, currTime+readyQ[0].burstTime])
            currTime += readyQ[0].burstTime

            turnAroundTime = currTime-readyQ[0].arrivalTime
            waitTime = turnAroundTime - readyQ[0].origBurstTime
            importantVals.append([readyQ[0].pid, waitTime, turnAroundTime])
            
            burstSum-=readyQ[0].burstTime
            readyQ.pop(0)
            continue
        else:                                                   #need to move on to other waiting processes and reload
            ganttArr.append([currTime, readyQ[0].pid, currTime+timeQ])            #current process in readyQ
            readyQ[0].burstTime -= timeQ
            moveToBack = readyQ.pop(0)
            currTime+=timeQ
            
            if len(sPs) != 0:                   #check to see if next process has arrived
                while True:
                    i = 0
                    if len(sPs) == 0:
                        break
                    if sPs[i].arrivalTime <= currTime:
                        readyQ.append(sPs[i])
                        sPs.pop(i)
                        continue
                    else:
                        break
            
            #if len(sPs) != 0 and sPs[0].arrivalTime <= currTime:
            #    readyQ.append(sPs[0])
            #    sPs.pop(0)
            readyQ.append(moveToBack)
            burstSum -= timeQ
    formating(importantVals, "RR", ganttArr, currTime, sPs)
        


if __name__ == "__main__":

    fileName = sys.argv[1]                      #Get file and time quantum   
    ps = open(fileName)
    tq = int(sys.argv[2])

    next(ps)
    masterArr = [] 
 
    for line in ps:                             #read lines in correctly
        line.strip()
        if not line.isspace():
            arr = []
            word = ""
            for char in line:
                if char == ',' or char == '\n':
                    num = int(word)
                    arr.append(num)
                    word = ""
                else:
                    word+=char
            if len(arr) != 0:
                masterArr.append(arr)
    
    ps.close()

    
    
    psArray = []                                #create 3 seperate processes and send them to their respective
    psArray1 = []                               #algorithms
    psArray0 = [] 
    i = 0
    while i <  len(masterArr):
        p = Process(masterArr[i][0], masterArr[i][1], masterArr[i][2])
        p0 = Process(masterArr[i][0], masterArr[i][1], masterArr[i][2])
        p1 = Process(masterArr[i][0], masterArr[i][1], masterArr[i][2])
        
        psArray.append(p)
        psArray1.append(p1)
        psArray0.append(p0)
        i+=1
       
    #FCFS
    fcfs = psArray
    newFCFS = FCFS(fcfs)
    newFCFS.getVals()
     
    #SJFP
    sjfp = psArray0
    newSJFP = SJFP(sjfp)
    newSJFP.getVals()


    #RR
    rr = psArray1
    newRR = RR(rr, tq)
    newRR.getVals()