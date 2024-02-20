#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;
enum Algorithm {
    FCFS = 1,
    SJF = 2,
    SRTF = 3,
    RR = 4,
    PRIORITY = 5,
    MLQ = 6
};

vector<int> calculate_waiting_time(vector<vector<int>>& processes, Algorithm algorithm) {
    int n = processes.size();
    vector<int> wt(n, 0);

    switch (algorithm) {
        case FCFS:
            for (int i = 1; i < n; i++)
                wt[i] = processes[i - 1][1] + wt[i - 1] - processes[i][0];
            break;
        case SJF:
            sort(processes.begin(), processes.end(), [](const vector<int>& a, const vector<int>& b) {
                return a[1] < b[1];
            });
            for (int i = 1; i < n; i++)
                wt[i] = processes[i - 1][1] + wt[i - 1] - processes[i][0];
            break;
        case SRTF: {
            int current_time = 0;
            while (true) {
                bool done = true;
                int shortest = -1;
                for (int i = 0; i < n; i++) {
                    if (processes[i][1] > 0 && (shortest == -1 || processes[i][1] < processes[shortest][1])) {
                        shortest = i;
                        done = false;
                    }
                }
                if (done)
                    break;
                processes[shortest][1]--;
                current_time++;
                for (int i = 0; i < n; i++) {
                    if (i != shortest && processes[i][1] > 0)
                        wt[i]++;
                }
            }
            break;
        }
        case RR: {
            cout << "-----------------------------------" << endl;
            int quantum;
            cout << "Enter time quantum: ";
            cin >> quantum;
            queue<int> rr_queue;
            int current_time = 0;
            vector<int> remaining_time(n);
            for (int i = 0; i < n; i++)
                remaining_time[i] = processes[i][1];
            while (true) {
                bool done = true;
                for (int i = 0; i < n; i++) {
                    if (remaining_time[i] > 0) {
                        done = false;
                        if (remaining_time[i] > quantum) {
                            current_time += quantum;
                            remaining_time[i] -= quantum;
                        } else {
                            current_time += remaining_time[i];
                            wt[i] = current_time - processes[i][0] - processes[i][1];
                            remaining_time[i] = 0;
                            rr_queue.push(i);
                        }
                    }
                }
                if (done)
                    break;
            }
            break;
        }
        case PRIORITY: {
            int current_time = 0;
            while (true) {
                bool done = true;
                int highest = -1;
                for (int i = 0; i < n; i++) {
                    if (processes[i][1] > 0 && (highest == -1 || processes[i][2] > processes[highest][2])) {
                        highest = i;
                        done = false;
                    }
                }
                if (done)
                    break;
                processes[highest][1]--;
                current_time++;
                for (int i = 0; i < n; i++) {
                    if (i != highest && processes[i][1] > 0)
                        wt[i]++;
                }
            }
            break;
        }
        case MLQ: {
            cout << "-----------------------------------" << endl;
            int num_queues;
            cout << "Enter the number of queues: ";
            cin >> num_queues;
            vector<int> quantum(num_queues);
            vector<queue<int>> mlq_queues(num_queues);
            vector<int> quantum_remaining(num_queues);
            int current_time = 0;
            for (int i = 0; i < num_queues; ++i) {
                cout << "Enter time quantum for queue " << i + 1 << ": ";
                cin >> quantum[i];
                quantum_remaining[i] = quantum[i];
            }
            while (true) {
                bool done = true;
                for (int i = 0; i < n; i++) {
                    if (processes[i][0] <= current_time && processes[i][1] > 0) {
                        done = false;
                        int queue_level = processes[i][3];
                        if (processes[i][1] > quantum_remaining[queue_level]) {
                            current_time += quantum_remaining[queue_level];
                            processes[i][1] -= quantum_remaining[queue_level];
                            quantum_remaining[queue_level] = 0;
                        } else {
                            current_time += processes[i][1];
                            wt[i] = current_time - processes[i][0] - processes[i][1];
                            processes[i][1] = 0;
                            mlq_queues[queue_level].push(i);
                        }
                    }
                }
                if (done)
                    break;
            }
            break;
        }
        default:
            cout << "Invalid algorithm" << endl;
    }
    return wt;
}

vector<int> calculate_turnaround(vector<vector<int>>& processes, vector<int>& wt) {
    int n = processes.size();
    vector<int> tat(n);
    for (int i = 0; i < n; i++)
        tat[i] = processes[i][1] + wt[i];
    return tat;
}

vector<int> calculate_completion_time(vector<vector<int>>& processes, vector<int>& tat) {
    int n = processes.size();
    vector<int> ct(n);
    for (int i = 0; i < n; i++)
        ct[i] = tat[i] + processes[i][0];
    return ct;
}

void calculate_avg_times(vector<int>& wt, vector<int>& tat, float& avg_wt, float& avg_tat) {
    int n = wt.size();
    avg_wt = 0;
    avg_tat = 0;
    for (int i = 0; i < n; i++) {
        avg_wt += wt[i];
        avg_tat += tat[i];
    }
    avg_wt /= n;
    avg_tat /= n;
}

vector<int> multilevel_scheduling(vector<vector<int>>& processes, vector<int>& queues) {
    int n = processes.size();
    int quantum = queues.size();
    int timestamp = 0;
    int waiting_time = 0;
    int total_waiting_time = 0;
    vector<int> queue_interval(queues.size() + 1, 0);
    vector<int> turn_around(n, 0);

    for (int i = 0; i < n; i++) {
        int queueNo = -1;
        for (int j = 0; j < queues.size(); j++) {
            if (processes[i][2] <= queues[j]) {
                queueNo = j;
                break;
            }
        }
        if (queueNo != -1) {
            queues.erase(queues.begin(), queues.begin() + queueNo);
            queue_interval[queueNo] = timestamp;
            timestamp = queue_interval[queueNo];
            quantum = queues[queueNo];
        }
    }

    while (true) {
        bool done = true;
        for (int i = 0; i < queues.size(); i++) {
            for (int j = 0; j < processes.size(); j++) {
                done = false;
                if (processes[j][2] > 0) {
                    if (timestamp >= processes[j][2]) {
                        timestamp += quantum;
                        processes[j][2] -= quantum;
                        waiting_time = timestamp - (processes[j][1] + processes[j][2]);
                        total_waiting_time += waiting_time;
                        if (processes[j][2] == 0) {
                            processes[j][2] = -1;
                            turn_around[j] = timestamp;
                        }
                    }
                }
            }
        }
        if (done)
            break;
    }

    for (int i = 0; i < n; i++)
        turn_around[i] -= processes[i][1];

    return turn_around;
}

void print_gantt_chart(vector<vector<int>>& processes, vector<int>& ct) {
    cout << "\nGantt Chart:\n";
    int n = processes.size();
    for (int i = 0; i < n; i++) {
        cout << "+--------";
    }
    cout << "+" << endl;
    cout << "|";
    for (int i = 0; i < n; i++) {
        cout << "  P" << i + 1 << "\t|";
    }
    cout << endl;
    for (int i = 0; i < n; i++) {
        cout << "+--------";
    }
    cout << "+" << endl;
    cout << "0";
    for (int i = 0; i < n; i++) {
        printf("\t%d", ct[i]);
    }
    cout << endl;
}

int main() {
    cout << "CPU Scheduling Algorithms:" << endl;
    cout << "1. First Come First Serve (FCFS)" << endl;
    cout << "2. Shortest Job First (SJF)" << endl;
    cout << "3. Shortest Remaining Time First (SRTF)" << endl;
    cout << "4. Round Robin (RR)" << endl;
    cout << "5. Priority Scheduling" << endl;
    cout << "6. Multilevel Queue Scheduling (MLQ)" << endl;

    int algorithm;
    do {
        cout << "-----------------------------------" << endl;
        cout << "Select an algorithm (1-6): ";
        cin >> algorithm;
    } while (algorithm < 1 || algorithm > 6);

    cout << "-----------------------------------" << endl;
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;
    vector<vector<int>> processes(n, vector<int>(4)); 
    for (int i = 0; i < n; i++) {
        cout << "-----------------------------------" << endl;
        cout << "Enter arrival time for process " << i + 1 << ": ";
        cin >> processes[i][0];
        cout << "-----------------------------------" << endl;
        cout << "Enter burst time for process " << i + 1 << ": ";
        cin >> processes[i][1];
        cout << "-----------------------------------" << endl;
        cout << "Enter priority for process " << i + 1 << ": ";
        cin >> processes[i][2];
        cout << "-----------------------------------" << endl;
        cout << "Enter queue level for process " << i + 1 << ": ";
        cin >> processes[i][3];
    }

    vector<int> wt = calculate_waiting_time(processes, Algorithm(algorithm));
    vector<int> tat = calculate_turnaround(processes, wt);
    vector<int> ct = calculate_completion_time(processes, tat);

    float avg_wt, avg_tat;
    calculate_avg_times(wt, tat, avg_wt, avg_tat);
    cout<<endl;
    cout << "===================================" << endl;
    cout << "Average Waiting Time: " << avg_wt << endl;
    cout << "-----------------------------------" << endl;
    cout << "Average Turnaround Time: " << avg_tat << endl;
    cout << "-----------------------------------" << endl;
    float throughput = static_cast<float>(n) / ct[n - 1]; 
    cout << "Throughput: " << throughput << " processes per unit time" << endl;
    cout << "===================================" << endl;
    cout << "Process ID\t|Arrival Time|\t|Burst Time|\t|Priority|\t|Queue Level|\t|Waiting Time|\t|Turnaround Time|\t|Completion Time|" << endl;
    for (int i = 0; i < n; i++) {
        cout << "P" << i + 1 << "\t\t" << processes[i][0] << "\t\t" << processes[i][1] << "\t\t" << processes[i][2] << "\t\t" << processes[i][3] << "\t\t" << wt[i] << "\t\t" << tat[i] << "\t\t\t" << ct[i] << endl;
    }
     print_gantt_chart(processes, ct);
    return 0;
}
