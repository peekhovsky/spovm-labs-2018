#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#define QUANTITY_OF_SUBJECTS 2
#define SUBJECTS "Maths Physics"

using namespace std;

struct sigaction print_signal;
struct sigaction exit_signal;

int print_flag = 0;
int exit_flag = 1;

void set_flag_print(int signo) {
	print_flag = 1;
}

void set_flag_exit(int signo) {
	exit_flag = 1;
}

int main(int argc, char* argv[]) {

    srand(time(NULL));
	int balance = rand() % 100;
	int type = rand() % 2 + 1;
	int minutes = rand() % 5 + 1;
	char PROCESS_NAMES[5][20] = { "First process\n\r", "Second process\n\r", "Third process\n\r", "Fourth process\n\r", "Fifth process\n\r" };

    print_signal.sa_handler = set_flag_print;
	sigaction(SIGUSR1, &print_signal, NULL);

	exit_signal.sa_handler = set_flag_exit;
	sigaction(SIGUSR2, &exit_signal, NULL);
    
    char strWin[256];

	char eventID[30];

    int current_process = atoi(argv[0]);

    initscr();
	curs_set(0);
	
    char process_name[20] = { '\0' };
	sprintf(process_name, "Process %d%d%d%d%d", current_process, current_process, current_process, current_process, current_process );
    
    int pid = fork();

	switch (pid) {
	case -1: {
	    printw("Error (case -1)");		   
        break;
    }

	case 0: {
		if (execlp("./child", "MATHS", "PHYSICS", "PROGRAMMING", "ENGLISH", NULL) == -1)
			printw("Error (case 0)");
		break;
	}

	default: 
    {
		
        if (atoi(argv[0]) == 1) {
            kill(getppid(), SIGUSR2);
        }

		exit_flag = 0;

		while (!exit_flag) { 
			napms(100);
            if (print_flag) {
				
                print_flag = 0;
				clear();
                move(0,0);
                refresh();

                for (int i = 0; process_name[i] != '\0'; i++) {
					
                    if (exit_flag) {
						print_flag = 0;
						kill(getppid(), SIGUSR2);
						waitpid(pid, NULL, 0);
						return 0;
					}

					printw("%c", process_name[i]);
					refresh();
					napms(100);
				}

				print_flag = 0;
				refresh();
            
                waitpid(pid, NULL, 0);
				kill(getppid(), SIGUSR2);
			}
		}
	}
    }
	return 0;
}
