#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#define QUANTITY_OF_SUBJECTS 2
#define SUBJECTS "Maths Physics"

#define PRINT_SEM_NAME "PRINT_SEM"
#define END_SEM_NAME "END_SEM"


sem_t* print_sem;
sem_t* end_sem;
int exit_flag;

using namespace std;

int main(int argc, char* argv[]) {
    initscr();
	curs_set(0);

    srand(time(NULL));

	char buffer1[15] = {'\0'};
	sprintf(buffer1, "/%d%s", getpid(), PRINT_SEM_NAME);
	if ((print_sem = sem_open(buffer1, 0)) == SEM_FAILED ) {
		getch();
		printw("sem_open11");
	}
	char buffer2[15] = {'\0'};
	sprintf(buffer2, "/%d%s",  getpid(), END_SEM_NAME);
	if ((end_sem = sem_open(buffer2, 0)) == SEM_FAILED ) {
	   	printw("sem_open22");
		   getch();
	}

	print_sem = sem_open(buffer1, NULL);
	end_sem = sem_open(buffer2, NULL);
   	
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
            sem_post(end_sem);
        }
		sem_getvalue(end_sem, &exit_flag);

		while (!exit_flag) { 
			sem_getvalue(end_sem, &exit_flag);
			napms(100);

            if (sem_trywait(print_sem) == NULL) {
			        
				clear();
                move(0,0);
                refresh();

                for (int i = 0; process_name[i] != '\0'; i++) {
					sem_getvalue(end_sem, &exit_flag);
                    if (exit_flag) {
						sem_trywait(print_sem);
						sem_post(end_sem);
						waitpid(pid, NULL, 0);
						return 0;
					}

					printw("%c", process_name[i]);
					refresh();
					napms(100);
				}

				sem_trywait(print_sem);
				refresh();
            
                waitpid(pid, NULL, 0);
				sem_post(end_sem);
			}
			sem_getvalue(end_sem, &exit_flag);
		}
	}
    }
	return 0;
}