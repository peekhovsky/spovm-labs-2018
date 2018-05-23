#include <csignal>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>

#define MAX_QUANTITY_OF_PROCESSES 10
#define STRING_SIZE 20

#define PRINT_SEM_NAME "PRINT_SEM"
#define END_SEM_NAME "END_SEM"
sem_t* print_sem;
sem_t* end_sem;

int process_quantity = 0;
int performing_process = 0;
int flag = 0;
int print_flag = 0;
int end_flag = 1;

pid_t pids[MAX_QUANTITY_OF_PROCESSES];
sem_t* print_sems[MAX_QUANTITY_OF_PROCESSES];
sem_t* end_sems[MAX_QUANTITY_OF_PROCESSES];

using namespace std;

void add_process() {
	if (process_quantity < MAX_QUANTITY_OF_PROCESSES) {
        pids[process_quantity] = fork();
		
        switch (pids[process_quantity]) {
        case 0: {
			char number_of_process[STRING_SIZE];
			sprintf(number_of_process, "%d", process_quantity);
			if (execlp("./parent", number_of_process, NULL) == -1) {
                cout << "Error. Cannot create process! (case 0)";
	    	}
            break;
        }
        case -1: {
            cout << "Error. Cannot create process! (case -1)";
            break;
        }
        default: {
			process_quantity++;
			char buffer1[15] = {'\0'};
			sprintf(buffer1, "/%d%s",  pids[process_quantity], PRINT_SEM_NAME);
			if ((print_sems[process_quantity] = sem_open(buffer1, O_CREAT, 0777, 0)) == SEM_FAILED ) {
        		perror("sem_open1");
			}
			char buffer2[15] = {'\0'};
			sprintf(buffer2, "/%d%s",  pids[process_quantity], END_SEM_NAME);
			if ((end_sems[process_quantity] = sem_open(buffer2, O_CREAT, 0777, 1)) == SEM_FAILED ) {
        		perror("sem_open2");
			}
			
        	break;
        }
        }
	}
}

bool is_end() {
	for (int i = 0; i < process_quantity; i++) {
		if (sem_trywait(end_sems[process_quantity])) {
			return true;
		}
	}
	return false;
}

bool is_print() {
	for (int i = 0; i < process_quantity; i++) {
		if (sem_trywait(print_sems[process_quantity])) {
			return true;
		}
	}
	return false;
}

void print_process() {
	if (end_sems[performing_process] && (process_quantity > 0)) {
		if (performing_process >= process_quantity - 1) {
			performing_process = 0;
        }
		else if (!flag) {
            performing_process++;
        }
		flag = 0;
		sem_post(print_sems[performing_process]);

	}
}

void delete_last_process() {
	if (process_quantity > 0) {
		sem_post(end_sems[process_quantity - 1]);

		waitpid(pids[process_quantity - 1], NULL, NULL);
		sem_close(end_sems[process_quantity - 1]);

		process_quantity--;

		if (performing_process >= process_quantity) {
			performing_process = 0;
			flag = 1;
			end_flag = 1;
		}
	}
}

void delete_all_processes()
{
	if (pids[process_quantity - 1] != 0)
		while (process_quantity >= 0)
		{
			sem_close(end_sems[process_quantity - 1]);
			kill(pids[process_quantity - 1], SIGUSR2);
			waitpid(pids[process_quantity - 1], NULL, NULL);
			process_quantity--;
		}
}

void set_flag_print(int signo)
{
	print_flag = 1;
}

void set_flag_exit(int signo)
{
	end_flag = 1;
}

int main(int argc, char* argv[]) {
	
    initscr();
	curs_set(0);
	clear();
	noecho();
	refresh();
    
    char choise = '.';
	while (true)
	{
		choise = getchar();
        if (choise == '+' || choise == '=') {
            add_process();
        }
        else if (choise == '-') {
            delete_last_process();
        }
        else if (choise == 'q') {
            break;
        }
		print_process();
	}

	delete_all_processes();


	endwin();
	return 0;
}