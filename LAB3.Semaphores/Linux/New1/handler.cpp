#include <stdio.h>
#include <vector>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <ncurses.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
} semunion;


int wait_semaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = IPC_NOWAIT;
	buf.sem_num = num;
	return semop(sem_id, &buf, 1);
}

int release_semaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = IPC_NOWAIT;
	buf.sem_num = num;
	return semop(sem_id, &buf, 1);
}


void close_ncurses() {
	endwin();
}


using namespace std;

#define BUFFER_SIZE 200
#define SEM_KEY 2018
#define SHM_KEY 1018
#define SHIFT 10

int main(int argc, char** argv) {	
	//init ncursres
	initscr();
	curs_set(0);
	clear();
	noecho();
	refresh();
	
	//create semaphore and buffer for output
	int shm_id = shmget(2000, BUFFER_SIZE, IPC_CREAT | 0777);
	int sem_id = semget(3000, 21, IPC_CREAT | 0777);

	if (sem_id == -1 || shm_id == -1) {
		printw("Error! Cannot create sem_id or shm_id!");
		getch();
		close_ncurses();
		return -1;
	}
	release_semaphore(sem_id, 0);

	//menu 
	pid_t pid;
	vector <pid_t> process_stack;
	char char_number[10] = { '\0' };
	char choise;

	while (true) {
		usleep(50000);
		fflush(stdin);
		choise = getchar();
		
		if (choise == '+' || choise == '=') {
			if (process_stack.size() < 10) {
				sprintf(char_number, "%d", (int)process_stack.size());
				pid = fork();
				if (pid == 0) {
					if (execlp("./parent", char_number, NULL) == -1) {
						printw("\nError! Cannot create process! (execl)");
					}
				}
				else if (pid == -1) {
					printw("\nError! Cannot create process! (pid = -1)");
				}
				process_stack.push_back(pid);
			}
		}
		else if (choise == '-') {
			if (process_stack.size() > 0) {
				release_semaphore(sem_id, process_stack.size());
				waitpid(process_stack.back(), 0, 0);
				wait_semaphore(sem_id, process_stack.size());
				process_stack.pop_back();
			}
		}
		else if (choise == 'q') {
			while (process_stack.size()) {
				release_semaphore(sem_id, process_stack.size());
				waitpid(process_stack.back(), 0, 0);
				process_stack.pop_back();
			}
			break;
		} else {
		}
	}

	//delete semaphore
	semctl(sem_id, 0, IPC_RMID, semunion);

	close_ncurses();
	return 0;
}
