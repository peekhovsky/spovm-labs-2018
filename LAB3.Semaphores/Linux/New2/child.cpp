#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
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
#include <time.h>

#define BUFF_SIZE 64

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

	int numberOfProcess = atoi(argv[0]);

	char nameOfProcess[BUFF_SIZE] = "Process: ";
	strcat(nameOfProcess, argv[0]);
	strcat(nameOfProcess, argv[0]);
	strcat(nameOfProcess, argv[0]);
	strcat(nameOfProcess, argv[0]);

	srand(time(NULL));
	
	for (int i = 1; i < argc; i++) {
		int r = rand() % 2;
		if (r == 0) {
			strcat(nameOfProcess, ", ");
			strcat(nameOfProcess, argv[i]);
		}
	}

	strcat(nameOfProcess, "\n");

	int sem_id = semget(2018, 0, 0777);
	int shm_id = shmget(1018, BUFF_SIZE, 0777);

	if (shm_id == -1 || sem_id == -1) {
		printw("Error! Cannot create sem_id or shm_id! (child)");
		refresh();
	}
	void* buffer = shmat(shm_id, NULL, 0);
	char empty[BUFF_SIZE];
	int counter = 0;
	memset(empty, '\0', BUFF_SIZE);

	while (true) {
		if (wait_semaphore(sem_id, (numberOfProcess + 10)) != -1) {
			strncpy((char*)buffer, empty, BUFF_SIZE);
			strncpy((char*)buffer, nameOfProcess, strlen(nameOfProcess));
		}

		if (wait_semaphore(sem_id, numberOfProcess) != -1) {
			release_semaphore(sem_id, numberOfProcess);
			return 0;
		}
	}
}
