#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int ProcessesNumber = 5;
int msize, psize, Q, CS;

struct Process {
	int processID;
	int arrivalTime;
	int CPUBurst;
	int size;
};

void swapProcess(Process *a, Process *b) { 
	Process temp = *a; 
	*a = *b; 
	*b = temp; 
} 
  
void arrangeArrivals(Process processes[]) { 
	// Selection Sort
	int minIndex;
	for (int i = 0; i < ProcessesNumber - 1; i++) {
		minIndex = i;
		for (int j = i+1; j < ProcessesNumber; j++) {
			if(processes[j].arrivalTime < processes[minIndex].arrivalTime)
				minIndex = j;
        } 
		swapProcess(&processes[minIndex], &processes[i]);
    } 
} 

void getWaitingTime(Process processes[], int waitingTime[]) {
	int serviceTime[ProcessesNumber];
	serviceTime[0] = 0;
	waitingTime[0] = 0;
	// Calculating waiting time for all processes
	for (int i = 1; i < ProcessesNumber; i++) {
		// Add burst time of previous processes
		serviceTime[i] = serviceTime[i - 1] + processes[i - 1].CPUBurst  + CS;

		// Find waiting time for current process = sum - arrivalTime[i]
		waitingTime[i] = serviceTime[i] - processes[i].arrivalTime;
		// Check if the process is negative, it means it's in the queue
		if (waitingTime[i] < 0)
			waitingTime[i] = 0;
	}
}

void getTurnArroundTime(Process processes[], int waitingTime[], int turnArroundTime[], int completionTime[]) {
	// Calculating turnaround time by adding CPUBurst[i] + WaitingTime[i]
	// and completionTime = TurnArroundTime[i] + ArrivalTime[i]
	for (int i = 0; i < ProcessesNumber; i++) {
		turnArroundTime[i] = processes[i].CPUBurst + waitingTime[i];
		completionTime[i] = turnArroundTime[i] + processes[i].arrivalTime;
	}
}

void printGanttChart(Process processes[], int waitingTime[], int turnArroundTime[], int completionTime[]) {
	string separatorLine = "|==========";
	string middleLine = "|    p" + to_string(processes[0].processID) + "    ";
	string lastLine = "0          ";
	for (int i = 1; i < ProcessesNumber; i++) {
		separatorLine += "||XX||==========";
		middleLine += "||XX||    p" + to_string(processes[i].processID) + "    ";
		lastLine += to_string(completionTime[i-1]) + "  " + to_string(completionTime[i-1] + CS) + "          ";
	}
	separatorLine += "|\n";
	middleLine += "|\n";
	lastLine += to_string(completionTime[ProcessesNumber - 1]) + "\n";
	
	cout << "[*] Gantt Chart\n\n"
		 << separatorLine
		 << middleLine
		 << separatorLine
		 << lastLine << "\n";
}

void printResults(Process processes[], int waitingTime[], int turnArroundTime[], int completionTime[]) {
	int totalWaitingTime = 0, totalTurnArroundTime = 0;
	cout << "[*] Results\n\n";
	cout << "Processes "
		 << "  Burst Time "
		 << "    Arrival Time "
		 << "   Waiting Time "
		 << "  Turn-Around Time "
		 << " Finish Time \n";

	for (int i = 0; i < ProcessesNumber; i++) {
		totalWaitingTime += waitingTime[i];
		totalTurnArroundTime += turnArroundTime[i];
		cout << " p" << processes[i].processID << "\t\t" << processes[i].CPUBurst << "\t\t"
			 << processes[i].arrivalTime << "\t\t" << waitingTime[i] << "\t\t "
			 << turnArroundTime[i] << "\t\t " << completionTime[i] << endl;

	}

	// Calculating and printg averages
	cout << "Average waiting time: "
		<< (float)totalWaitingTime / (float)ProcessesNumber;
	cout << "\nAverage turn around time: "
		<< (float)totalTurnArroundTime / (float)ProcessesNumber;
	cout << "\nCPU utilization: "
		<< 100 * (float)completionTime[ProcessesNumber - 1] / ((float)completionTime[ProcessesNumber - 1] + ((ProcessesNumber - 1) * CS))
		<< "\n";
}

void FCFS(Process processes[]) {
	int waitingTime[ProcessesNumber], turnArroundTime[ProcessesNumber], completionTime[ProcessesNumber];
	
	// Re-arrange the processes due to arrival time
	arrangeArrivals(processes);

	// Calculate the waiting time for all processes
	getWaitingTime(processes, waitingTime);

	// Calculate the turn arround time for all procceses
	getTurnArroundTime(processes, waitingTime, turnArroundTime, completionTime);

	// Print the Gantt Chart
	printGanttChart(processes, waitingTime, turnArroundTime, completionTime);

	// Printing each process waiting, turn arround, and finish time
	printResults(processes, waitingTime, turnArroundTime, completionTime);
}

void SJF(Process processes[]) {
	int waitingTime[ProcessesNumber], turnArroundTime[ProcessesNumber], completionTime[ProcessesNumber];
	int temp, val;

	// Re-arrange the processes due to arrival time
	arrangeArrivals(processes);

	completionTime[0] = processes[0].arrivalTime + processes[0].CPUBurst;
	turnArroundTime[0] = completionTime[0] - processes[0].arrivalTime;
	waitingTime[0] = turnArroundTime[0] - processes[0].CPUBurst;

	for (int i = 1; i < ProcessesNumber; i++) {
		// Get the minimum burst
		temp = completionTime[i-1];
		int min = processes[i].CPUBurst;
		for (int j = i; j < ProcessesNumber; j++) {
        	if(temp >= processes[j].arrivalTime && min >= processes[j].CPUBurst) {
                min = processes[j].CPUBurst;
                val = j;
            }
        }
        completionTime[val] = temp + processes[val].CPUBurst + CS;
        turnArroundTime[val] = completionTime[val] - processes[val].arrivalTime;
        waitingTime[val] = turnArroundTime[val] - processes[val].CPUBurst;
        swapProcess(&processes[val], &processes[i]);
		swap(completionTime[val], completionTime[i]);
		swap(turnArroundTime[val], turnArroundTime[i]);
		swap(waitingTime[val], waitingTime[i]);
	}
	
	printGanttChart(processes, waitingTime, turnArroundTime, completionTime);

	printResults(processes, waitingTime, turnArroundTime, completionTime);
}

int main() {
	// Declaring variables
	ifstream processesFile;
	string fileName;
	Process processes[ProcessesNumber];

	// Read processes file
	do {
		cout << "Enter File Name: ";
		cin >> fileName;
		processesFile.open(fileName);
		if (!processesFile.is_open())
			cout << "File not found!\n";
	} while (!processesFile.is_open());

	// Reading static values
	processesFile >> msize;
	processesFile >> psize;
	processesFile >> Q;
	processesFile >> CS;

	// Reading processes information
	for (int i = 0; i < ProcessesNumber; i++) {
		processesFile >> processes[i].processID;
		processesFile >> processes[i].arrivalTime;
		processesFile >> processes[i].CPUBurst;
		processesFile >> processes[i].size;
	}
	processesFile.close();

	cout << "\n[-] ----------------------------------------FCFS---------------------------------------- [-]\n\n";

	Process copyOfProcesses[ProcessesNumber];
	copy(begin(processes), end(processes), begin(copyOfProcesses));
	FCFS(copyOfProcesses);
	
	cout << "\n[-] ----------------------------------------SJF---------------------------------------- [-]\n\n";
	copy(begin(processes), end(processes), begin(copyOfProcesses));
	SJF(copyOfProcesses);

	return 0;
}