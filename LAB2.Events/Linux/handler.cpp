#include <csignal>
#include <ncurses.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>

#define MAX_QUANTITY_OF_PROCESSES 10
#define STRING_SIZE 20
using namespace std;

struct sigaction print_signal;
struct sigaction end_signal;

int process_quantity = 0;
int performing_process = 0;
int flag = 0;
int print_flag = 0;
int end_flag = 1;
pid_t pids[MAX_QUANTITY_OF_PROCESSES];


void add_process() {
	if (process_quantity < MAX_QUANTITY_OF_PROCESSES) {
	
        pids[process_quantity] = fork();
		
        switch (pids[process_quantity]) {
        case 0: {
			char number_of_process[STRING_SIZE];
			    sprintf(str, "%d", process_quantity);
			if (execlp("./parent", number_of_process, NULL) == -1) {
                //cout << "Error. Cannot create process! (case 0)";
	    	}
            break;
        }
        case -1: {
            //cout << "Error. Cannot create process! (case -1)";
            break;
        }
        default: {
            process_quantity++;
            break;
        }
        }
	}
}

void print_process() {
	if (end_flag && (process_quantity > 0)) {
		end_flag = 0;
		if (performing_process >= process_quantity - 1) {
			performing_process = 0;
        }
		else if (!flag) {
            performing_process++;
        }
		flag = 0;
		kill(pids[performing_process], SIGUSR1);
	}
}

void delete_last_process() {
	if (process_quantity > 0) {
		kill(pids[process_quantity - 1], SIGUSR2);
		waitpid(pids[process_quantity - 1], NULL, NULL);

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

	print_signal.sa_handler = set_flag_print;
	sigaction(SIGUSR1, &print_signal, NULL);

	end_signal.sa_handler = set_flag_exit;
	sigaction(SIGUSR2, &end_signal, NULL);
    
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
