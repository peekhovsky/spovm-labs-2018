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
#include <time.h>
#include <sys/sem.h>


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

int main(int argc, char* argv[]) {

	srand(time(NULL));
	initscr();

	int number_of_process = atoi(argv[0]);

	char process_name[BUFFER_SIZE] = {'\0'};
	sprintf(process_name,"Process %s%s%s: size - %d", argv[0], argv[0], argv[0], argc);

	int random_num;
	for (int i = 1; i < argc; i++) {
        	random_num = rand() % 3;
        	if (random_num > 1) {
				strcat(process_name, ", ");
				strcat(process_name, argv[i]);
			}
    }

	int sem_id = semget(3000, 0, 0777);
	int shm_id = shmget(2000, BUFFER_SIZE, 0777);

	if (shm_id == -1 || sem_id == -1) {
		printw("Error! Cannot open sem_id or shm_id (child)");
		refresh();
	}
	void* buffer = shmat(shm_id, NULL, 0);
	char empty_buffer_template[BUFFER_SIZE] = { '\0' };
	int counter = 0;

	while (true) {
		if (wait_semaphore(sem_id, (number_of_process + SHIFT)) != -1) {
			strncpy((char*)buffer, empty_buffer_template, BUFFER_SIZE);
			strncpy((char*)buffer, process_name, strlen(process_name));
		}

		if (wait_semaphore(sem_id, number_of_process) != -1) {
			release_semaphore(sem_id, number_of_process);
			break;
		}
	}

	return 0;
}
