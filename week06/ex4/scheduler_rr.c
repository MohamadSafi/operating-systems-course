#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <limits.h>
#define PS_MAX 10
#define MAX_TIME 100
#define FILENAME "data.txt"


int quantum; 
int last_execution_time = 0; 

typedef struct{
	int idx; 
	int at, bt, rt, wt, ct, tat; 
	int burst; 
    int remaining_quantum;

} ProcessData;

int running_process = -1; 

unsigned total_time; 

ProcessData data[PS_MAX];

pid_t ps[PS_MAX];

unsigned data_size;

void read_file(FILE* file){
    int i = 0;
    while(fscanf(file, "%d %d %d", &data[i].idx, &data[i].at, &data[i].bt) != EOF) {
        data[i].burst = data[i].bt; 
        ps[i] = 0; 
        i++;
    }
    data_size = i;
    fclose(file);
}

void resume(pid_t process) {
    if (process > 0) {
        kill(process, SIGCONT);
    }
}
void suspend(pid_t process) {
    if (process > 0) {
        kill(process, SIGTSTP);
    }
}

void terminate(pid_t process) {
    if (process > 0) {
        kill(process, SIGTERM);
        waitpid(process, NULL, 0);
    }
}

void create_process(int new_process) {
    if(running_process != -1) {
        suspend(ps[running_process]);
    }

    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"./worker", NULL, NULL};
        char idx_str[10];
        sprintf(idx_str, "%d", new_process);
        args[1] = idx_str;
        execvp(args[0], args);
        exit(EXIT_FAILURE);
    } else if(pid > 0) {
        ps[new_process] = pid;
    } else {
        printf("Fork failed!\n");
        exit(EXIT_FAILURE);
    }

    running_process = new_process;
}

ProcessData find_next_process() {
    if (running_process != -1 && data[running_process].remaining_quantum > 0 && data[running_process].burst > 0) {
        return data[running_process];
    }

    int idx = (running_process + 1) % data_size; 
    int full_cycle = 0; 

    while (full_cycle < data_size) {
        if (data[idx].burst > 0 && data[idx].at <= total_time) {
            return data[idx];
        }
        idx = (idx + 1) % data_size;
        full_cycle++;
    }

    ProcessData none;
    none.idx = -1;
    return none;
}


void report(){
	printf("Simulation results.....\n");
	int sum_wt = 0;
	int sum_tat = 0;
	for (int i=0; i< data_size; i++){
		printf("process %d: \n", i);
		printf("	at=%d\n", data[i].at);
		printf("	bt=%d\n", data[i].bt);
		printf("	ct=%d\n", data[i].ct);
		printf("	wt=%d\n", data[i].wt);
		printf("	tat=%d\n", data[i].tat);
		printf("	rt=%d\n", data[i].rt);
		sum_wt += data[i].wt;
		sum_tat += data[i].tat;
	}

	printf("data size = %d\n", data_size);
	float avg_wt = (float)sum_wt/data_size;
	float avg_tat = (float)sum_tat/data_size;
	printf("Average results for this run:\n");
	printf("	avg_wt=%f\n", avg_wt);
	printf("	avg_tat=%f\n", avg_tat);
}

void check_burst(){

	for(int i = 0; i < data_size; i++)
		if (data[i].burst > 0)
		    return;

    report();

	  exit(EXIT_SUCCESS);
}


void schedule_handler(int signum) {
    total_time++;

    if(running_process != -1) {
        data[running_process].remaining_quantum--;
        data[running_process].burst--;
        
        printf("Scheduler: Runtime: %u seconds\n", total_time);
        printf("Process %d is running with %d seconds left\n", running_process, data[running_process].burst);

        if(data[running_process].burst == 0 || data[running_process].remaining_quantum == 0) {
            if(data[running_process].burst == 0) {
                printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
                data[running_process].ct = total_time;
                data[running_process].tat = data[running_process].ct - data[running_process].at;
                data[running_process].wt = data[running_process].tat - data[running_process].bt;
                data[running_process].rt = data[running_process].tat - data[running_process].burst;
                terminate(ps[running_process]);
                ps[running_process] = 0;
                running_process = -1;
            } else {
                // Quantum expired but process not completed
                printf("Scheduler: Stopping Process %d due to Quantum expiration (Remaining Time: %d)\n", running_process, data[running_process].burst);
                data[running_process].remaining_quantum = quantum; // Reset quantum for next time
                suspend(ps[running_process]);
                running_process = -1; 
            }
        }
    }

    check_burst();

    ProcessData next_process_data = find_next_process();
    int next_process = next_process_data.idx;
    if(running_process != next_process) {
        if(running_process != -1) {
            printf("Scheduler: Stopping Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
            suspend(ps[running_process]);
        }
        
        printf("Scheduler: Running Process %d (Remaining Time: %d)\n", next_process, data[next_process].burst);
        if (ps[next_process] == 0) {
            create_process(next_process);
        } else {
            resume(ps[next_process]);
        }
        running_process = next_process;
    }

    if (total_time >= MAX_TIME) {
        for (int i = 0; i < data_size; i++) {
            terminate(ps[i]);
        }

        printf("Scheduler: All processes terminated.\n");
        exit(0);
    }

    signal(SIGALRM, schedule_handler);
    alarm(1);
}

int compare_arrival_time(const void *a, const void *b) {
    ProcessData *processA = (ProcessData *)a;
    ProcessData *processB = (ProcessData *)b;

    return processA->at - processB->at;
}

void sort_data() {
    qsort(data, data_size, sizeof(ProcessData), compare_arrival_time);
}


int main() {
    FILE* file = fopen(FILENAME, "r");
    if(!file) {
        printf("Error: Unable to open file.\n");
        exit(EXIT_FAILURE);
    }

    read_file(file);
    sort_data();

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    for(int i = 0; i < data_size; i++) {
        data[i].remaining_quantum = quantum;
    }

    signal(SIGALRM, schedule_handler);
    alarm(1);

    while (1) {
        pause();
    }

    return 0;
}
