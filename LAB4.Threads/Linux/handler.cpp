
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include <unistd.h>
#include <ctime>
#include <ncurses.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>
#include <sys/sem.h>


#define BUFF_SIZE 64
#define RANDOM_STRING_LENGTH 10

#define SUBJECT_QUANTITY 3
#define SUBJECTS "MATHS PHYSICS CHEMISTRY"

using namespace std;

pthread_mutex_t print_mutex;// = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t wait_execute_mutex;

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

struct MutexAndName {
    pthread_mutex_t* end_mutex;
    std::string event_name;
};

char* get_random_string(const int length) {
	static const char characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char* str = new char[length + 1];

	for (int i = 0; i < length; i++) {
		str[i] = characters[rand() % (sizeof(characters) - 1)];
	}
	str[length] = '\0';
	return str;
}

char* execute_client(string event_name) {	
	int shm_id = shmget(1018, BUFF_SIZE, IPC_CREAT | 0777);
	int sem_id = semget(2018, 1, IPC_CREAT | 0777);

	if (shm_id == -1 || sem_id == -1) {
		printw("Error! Cannot create sem_id or shm_id! (parent)");
		refresh();
	}

	void* buffer = shmat(shm_id, NULL, 0);
	char* identificator = get_random_string(10);
	pid_t pid;
	switch (pid = fork()) {		
	case -1:
		printw("Error! Cannot create new process! (parent)");
		refresh();
		break;

	case 0:
		if (execl("./child", "1", "PHYSICS", "MATHS", "ENGLISH", NULL) == -1) {
			printw("Error! Cannot open file! (parent)");
			exit(0);
			refresh();
		}

	default:
		break;
	}

	wait_semaphore(sem_id, 0);

	return (char*) buffer;
}

void* client_thread(void* args) {
	srand((unsigned)time(NULL));
	MutexAndName* mutex_and_name = (MutexAndName*)args; 
    //std::string output_string = mutex_and_name->event_name;
	pthread_mutex_lock(&wait_execute_mutex);
	std::string output_string =  mutex_and_name->event_name + ": accepted subjects are " + execute_client(mutex_and_name->event_name);
	pthread_mutex_unlock(&wait_execute_mutex);

	const char* output_char = output_string.c_str();
	do {
		pthread_mutex_lock(&print_mutex);
        clear();
        curs_set(0);
        move(0,0);
		refresh();
		for (unsigned i = 0; output_char[i] != '\0', i < output_string.length(); i++) {
            printw("%c", output_char[i]);
			refresh();
			//usleep(50000);
		}
		pthread_mutex_unlock(&print_mutex);
        //usleep(100000);
		if (pthread_mutex_trylock(mutex_and_name->end_mutex) == PTHREAD_MUTEX_NORMAL) {
			pthread_mutex_unlock(mutex_and_name->end_mutex);
		}
		else {
				break;
		}
	} while (true);
}



struct ThreadMutex {
    pthread_t* thread_id;
    pthread_attr_t* attr;
    pthread_mutex_t* end_mutex;
};

std::vector<ThreadMutex*> thread_stack;

int main() {
    initscr();
	curs_set(0);
	clear();
	noecho();
	refresh();
	
	print_mutex = PTHREAD_MUTEX_INITIALIZER;
	wait_execute_mutex = PTHREAD_MUTEX_INITIALIZER;

    while (true) {
		char in_character = getchar();
		switch (in_character) {
		//create new thread
		case '=':
		case '+': {
			char str[16];
			snprintf(str, sizeof(str), "%ld%ld%ld%ld", thread_stack.size(), thread_stack.size(), thread_stack.size(), thread_stack.size());
            std::string thread_number = string(str);

            pthread_t* thread_id = new pthread_t;
            pthread_attr_t* attr = new pthread_attr_t;
			pthread_mutex_t* end_mutex = new pthread_mutex_t; 
            MutexAndName* mutex_and_name = new MutexAndName;
            mutex_and_name->end_mutex = end_mutex;
            mutex_and_name->event_name = "Process " + thread_number;
					
            pthread_create(thread_id, NULL, client_thread, (void*) mutex_and_name);		
            ThreadMutex* thread_mutex = new ThreadMutex;
			thread_mutex->thread_id = thread_id;
			thread_mutex->attr = attr;
            thread_mutex->end_mutex = end_mutex;
			thread_stack.push_back(thread_mutex);
			break;
		}

		//delete last created thread
		case '-': {
			if (thread_stack.size() < 1) {
				break;
			}
			ThreadMutex* thread_mutex = thread_stack.back();
			thread_stack.pop_back();
            pthread_mutex_trylock(thread_mutex->end_mutex);
			break;
		}
		//quit program
		case 'q': {
			exit(0);
			break;
		}
			default: {

			}
		}
		usleep(1000000);
	}

    echo();
	endwin();
    return 0;
}