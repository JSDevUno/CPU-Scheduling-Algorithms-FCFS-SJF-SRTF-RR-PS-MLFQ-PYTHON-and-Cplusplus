from enum import Enum

class Algorithm(Enum):
    FCFS = 1
    SJF = 2
    SRTF = 3
    RR = 4
    PRIORITY = 5
    MLQ = 6

def calculate_waiting_time(processes, algorithm):
    n = len(processes)
    wt = [0] * n
    if algorithm == Algorithm.FCFS:
        for i in range(1, n):
            wt[i] = processes[i-1][1] + wt[i-1] - processes[i][0]
    elif algorithm == Algorithm.SJF:
        processes.sort(key=lambda x: x[1])
        for i in range(1, n):
            wt[i] = processes[i-1][1] + wt[i-1] - processes [i][0]
    elif algorithm == Algorithm.SRTF:
        current_time = 0
        while True:
            done = True
            shortest = None
            for i in range(n):
                if processes[i][1] > 0:
                    if (shortest is None) or (processes[i][1] < processes[shortest][1]):
                        shortest = i
                    done = False
            if done:
                break
            processes[shortest][1] -= 1
            current_time += 1
            for i in range(n):
                if (processes[i][1] > 0) and (i != shortest):
                    wt[i] += 1
        for i in range(n):
            wt[i] = wt[i] - processes[i][0]
    elif algorithm == Algorithm.RR:
        print('-----------------------------------')
        quantum = int(input("Enter time quantum: "))
        current_time = 0
        rr_queue = []
        while True:
            done = True
            for i in range(n):
                if processes[i][0] <= current_time and processes[i][1] > 0:
                    done = False
                    if processes[i][1] > quantum:
                        current_time += quantum
                        processes[i][1] -= quantum
                    else:
                        current_time += processes[i][1]
                        wt[i] = current_time - processes[i][0] - processes[i][1]
                        processes[i][1] = 0
                    rr_queue.append(i)
            if done:
                break
        for i in range(n):
            if processes[i][1] > 0:
                wt[i] = current_time - processes[i][0] - processes[i][1]
    elif algorithm == Algorithm.PRIORITY:
        current_time = 0
        while True:
            done = True
            highest = None
            for i in range(n):
                if processes[i][1] > 0:
                    if (highest is None) or (processes[i][2] > processes[highest][2]):
                        highest = i
                    done = False
                if done:
                    break
                processes[highest][1] -= 1
                current_time += 1
                for i in range(n):
                    if (processes[i][1] > 0) and (i != highest):
                        wt[i] += 1
                for i in range(n):
                    wt[i] = wt[i] - processes[i][0]
    elif algorithm == Algorithm.MLQ:
        print('-----------------------------------')
        quantum = int(input("Enter time quantum: "))
        current_time = 0
        rr_queue = []
        while True:
            done = True
            for i in range(n):
                if processes[i][0] <= current_time and processes[i][1] > 0:
                    done = False
                    if processes[i][1] > quantum:
                        current_time += quantum
                        processes[i][1] -= quantum
                    else:
                        current_time += processes[i][1]
                        wt[i] = current_time - processes[i][0] - processes[i][1]
                        processes[i][1] = 0
                    rr_queue.append(i)
            if done:
                break
        for i in range(n):
            if processes[i][1] > 0:
                wt[i] = current_time - processes[i][0] - processes[i][1]
    return wt


def calculate_turnaround(processes, wt):
    n = len(processes)
    tat = [0] * n
    for i in range(n):
        tat[i] = processes[i][1] + wt[i]
    return tat

def calculate_completion_time(processes, tat):
    n = len(processes)
    ct = [0] * n
    for i in range(n):
        ct[i] = tat[i] + processes[i][0]
    return ct

def calculate_avg_times(wt, tat):
    n = len(wt)
    return sum(wt) / n, sum(tat) / n

def multilevel_scheduling(processes, queues):
    n = len(processes)
    quantum = [None] * len(queues)
    timestamp = 0
    waiting_time = 0
    total_waiting_time = 0
    queue_interval = [0] * (len(queues) + 1)
    turn_around = 0
    # assign a process to a queue
    for i in range (n):
        queueNo = None
        for j in range (len(queues)):
            if processes[i][2] <= queues[j][1]:
                queueNo = j
                break
    
        queues[queueNo][2].append([processes[i], 0])
    
    # initialise timestamps
    for i in range (len(queues)):
        if len(queues[i][2]) > 0:
            timestamp = queues[i][2][0][1]
        queue_interval[i+1] = timestamp
        quantum[i] = queues[i][0]
    
    while True:
        done = True
        for i in range (len(queues)):
            for j in range (len(queues[i][2])):
                done = False
                if queues[i][2][j][0][2] > 0:   # Confirm if process is done or not
                    if queues[i][2][j][1] + quantum[i] < queues[i][2][j][0][2]:
                        timestamp = queues[i][2][j][1] + quantum[i]
                    else: 
                        timestamp = queues[i][2][j][0][2]
    
                    waiting_time = timestamp - (queues[i][2][j][0][1] + queues[i][2][j][1]) 
                    queues[i][2][j][1] = timestamp
                    total_waiting_time += waiting_time
    
                    # Change to next queue if process is completed
                    # Note: since this is the innermost loop it will start finding the 
                    # processes of next queue right after a process is completed
                    if queues[i][2][j][1] == queues[i][2][j][0][2]:
                        queues[i+1][2].append(queues[i][2][j])
                        queues[i][2].remove(queues[i][2][j])
            queue_interval[i+1] = timestamp
    
        # Break from the outermost loop if all the processes are done
        if done:
            break
    
    # Calculate Turn Around Time
    for i in range (n):
        turn_around[i] = processes[i][2] + total_waiting_time - processes[i][1]
    
    return turn_around
    
def main():
    print("CPU Scheduling Algorithms:")
    print("1. First Come First Serve (FCFS)")
    print("2. Shortest Job First (SJF)")
    print("3. Shortest Remaining Time First (SRTF)")
    print("4. Round Robin (RR)")
    print("5. Priority Scheduling")
    print("6. Multilevel Queue Scheduling (MLQ)")

    while True:
        print('-----------------------------------')
        algorithm = int(input("Select an algorithm (1-6): "))
        if algorithm in range(1,7):
            break
        print("Invalid command. Please select an algorithm between 1 and 6.")
    print('-----------------------------------')
    n = int(input("Enter the number of processes: "))
    processes = []
    for i in range(n):
        print('-----------------------------------')
        arrival_time = int(input("Enter arrival time for process {}: ".format(i + 1)))
        print('-----------------------------------')
        burst_time = int(input("Enter burst time for process {}: ".format(i + 1)))
        processes.append([arrival_time, burst_time])

    wt = calculate_waiting_time(processes, Algorithm(algorithm))
    tat = calculate_turnaround(processes, wt)
    avg_wt, avg_tat = calculate_avg_times(wt, tat)
    ct = calculate_completion_time(processes, tat)
    print('-----------------------------------')
    print("Average Waiting Time:", avg_wt)
    print('-----------------------------------')
    print("Average Turnaround Time:", avg_tat)
    print('-----------------------------------')
    print("Process\t|Arrival Time|\t|Burst Time|\t|Waiting Time|\t|Turnaround Time|\t|Completion Time|")
    for i in range(n):
        print("P{}\t\t{}\t\t{}\t\t{}\t\t{}\t\t\t{}".format(i + 1, processes[i][0], processes[i][1], wt[i], tat[i], ct[i]))

    avg_wt, avg_tat = calculate_avg_times(wt, tat)

main()
