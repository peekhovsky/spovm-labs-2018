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

void sleep(int num) {
	usleep(num);
}


#define BUFF_SIZE 64

int main(int argc, char** argv) {
	initscr();

	int numberOfProcess = atoi(argv[0]);
	
	int sem_id = semget(2018, 0, 0777);
	int shm_id = shmget(1018, BUFF_SIZE, 0777);
	if (shm_id == -1 || sem_id == -1)
	{
		printw("Error! Cannot create sem_id or shm_id! (parent)");
		refresh();
	}

	char outputCurrentProcessName[BUFF_SIZE];
	void* buffer = shmat(shm_id, NULL, 0);

	pid_t pid;
	switch (pid = fork())
	{
	case -1:
		printw("Error! Cannot create new process! (parent)");
		refresh();
		break;

	case 0:
		if (execl("./child", argv[0], "PHYSICS", "MATHS", "ENGLISH", NULL) == -1)
		{
			printw("Error! Cannot open file! (parent)");
			refresh();
		}

	default:
		break;
	}

	int status;
	release_semaphore(sem_id, (numberOfProcess + 10));
	sleep(200000);

	while (true) {
		sleep(50000);
		if (wait_semaphore(sem_id, 0) != -1)
		{	
			strncpy(outputCurrentProcessName, (char*)buffer, BUFF_SIZE);
			release_semaphore(sem_id, (numberOfProcess + 10));
			clear();

			for (int j = 0; j < strlen(outputCurrentProcessName); j++)
			{
				if (wait_semaphore(sem_id, numberOfProcess) != -1)
				{
					refresh();
					release_semaphore(sem_id, numberOfProcess);
					release_semaphore(sem_id, 0);
					waitpid(pid, 0, 0);
					shmctl(shm_id, IPC_RMID, NULL);
					return 0;
				}
				printw("%c", outputCurrentProcessName[j]);
				refresh();
				sleep(50000);
			}
			refresh();
			release_semaphore(sem_id, 0);
		}

		if (wait_semaphore(sem_id, numberOfProcess) != -1) {
			release_semaphore(sem_id, numberOfProcess);
			waitpid(pid, 0, 0);
			shmctl(shm_id, IPC_RMID, NULL);
			break;
		}
	}
	return 0;
}
