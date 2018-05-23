#include <stdio.h>
#include <list>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <ncurses.h>


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

#define BUFF_SIZE 64

using namespace std;

int main() {
	initscr();
	curs_set(0);
	clear();
	noecho();
	refresh();

	int shm_id = shmget(1018, BUFF_SIZE, IPC_CREAT | 0777);
	int sem_id = semget(2018, 21, IPC_CREAT | 0777);

	if(sem_id == -1) {
		printw("Error");
		refresh();	
	}
	release_semaphore(sem_id, 0);

	pid_t pid;
	list <pid_t> process_stack;
	char choise;
	char number[10];

	while (true) {
		usleep(50000);
		fflush(stdin);
		choise = getchar();

		if (choise == '+' || choise == '-') {
			if (process_stack.size() == 10) continue;
			pid = fork();
			process_stack.push_back(pid);
			sprintf(number, "%d", (int)process_stack.size());
			if (pid == 0) execl("./parent", number,  NULL);
		}

		if (choise == '-') {
			if (process_stack.size() == 0) continue;
			release_semaphore(sem_id, process_stack.size());
			waitpid(process_stack.back(), 0, 0);
			wait_semaphore(sem_id, process_stack.size());
			process_stack.pop_back();
		}
		if (choise == 'q') {
			while (process_stack.size()) {
				release_semaphore(sem_id, process_stack.size());
				waitpid(process_stack.back(), 0, 0);
				process_stack.pop_back();
			}
			break;
		}
	}

	semctl(sem_id, 0, IPC_RMID);
	clear();
	endwin();
	return 0;
}
