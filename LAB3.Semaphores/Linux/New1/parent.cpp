#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/stat.h>
#include <csignal>
#include <sys/sem.h>

#define QUANTITY_OF_SUBJECTS 2

#define BUFFER_SIZE 200
#define SEM_KEY 2018
#define SHM_KEY 1018
#define SHIFT 10


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

int main(int argc, char** argv) {
	initscr();

	int number_of_process = atoi(argv[0]);
	
	//open semaphore and buffer for output
	int sem_id = semget(3000, 0, 0777);
	int shm_id = shmget(2000, BUFFER_SIZE, 0777);


	if (shm_id == -1 || sem_id == -1) {
		printw("Error! Cannot create sem_id or shm_id! (parent)");
		getch();
		return -1;
	}
	
	//open buffer
	void* buffer = shmat(shm_id, NULL, 0);
	char process_name[BUFFER_SIZE] = { '\0' };


	//menu
	pid_t pid = fork();
	switch (pid) {
	case -1: {
		//printw("The new process is not created!");
		refresh();
		break;
	}
	case 0: {
		//if (execl("./child", argv[0], "MATHS", "PHYSICS", "PROGRAMMING", "ENGLISH", NULL) == -1) {
			if (execl("./child", argv[0], NULL) == -1) {
			printw("Cannot open file!");
			refresh();
		}
		break;
	}
	default: { 
		break;
	}
	}

	int status;
	release_semaphore(sem_id, (number_of_process + SHIFT));
	napms(300);	

	while (true) {
		napms(300);
		if (wait_semaphore(sem_id, 0) != -1) {	
			strncpy(process_name, (char*)buffer, BUFFER_SIZE);
			release_semaphore(sem_id, (number_of_process + SHIFT));	
			clear();
			curs_set(0);

			for (int j = 0; j < strlen(process_name); j++) {
				
				if (wait_semaphore(sem_id, number_of_process)  != -1) {
					refresh();
					release_semaphore(sem_id, number_of_process);
					release_semaphore(sem_id, 0);
					waitpid(pid, 0, 0);
					shmctl(shm_id, IPC_RMID, NULL);
					return 0;		
				}
				printw("%c", process_name[j]);
				napms(50);
				refresh();
			}
			refresh();
		} else {
			break;
		}

		if (wait_semaphore(sem_id, number_of_process) != -1) {
			release_semaphore(sem_id, number_of_process);
			waitpid(pid, 0, 0);
			shmctl(shm_id, IPC_RMID, NULL);
			break;
		}
	}
	return 0;
}
