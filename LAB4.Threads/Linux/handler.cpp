
#include <iostream>
#include <vector>
#include <string>

#include <ctime>
#include "ncurses.h"
#include <phread.h>

#define RANDOM_STRING_LENGTH 10

#define SUBJECT_QUANTITY 3
#define SUBJECTS "MATHS PHYSICS CHEMISTRY"

phread_mutex_t print_mutex;

struct MutexAndName {
    phread_mutex_t* end_mutex;
    std::string event_name;
}

void client_thread(MutexAndName* mutex_and_name) {
    std::string output_string = mutex_and_name->event_name;
	//std::string output_string =  event_name + + ": accepted subjects are " + execute_client();

	do {
		phread_mutex_lock(&print_mutex);
        clear();
        curs_set(0);
        move(0,0);
		for (unsigned i = 0; output_string[i] != '\0', i < output_string.length(); i++) {
            printw(output_string[i]);
			Sleep(100);
		}
		phread_mutex_unlock(&print_mutex);
		napms(400);
        if (phread_mutex_trylock(mutex_and_name->end_mutex)) {
			phread_mutex_unlock(mutex_and_name->end_mutex);
		}
		else {
			break;
		}
	} while (true);
}



struct ThreadMutex {
    phread_t* thread_id;
    phread_attr_t* attr;
    phread_mutex_t* end_mutex;
}

std::vector<ThreadMutex*> thread_stack;

int main() {
    initscr();
	curs_set(0);
	clear();
	noecho();
	refresh();

    while (true) {
		char in_character = getchar();
		switch (in_character) {
		//create new thread
		case '=':
		case '+': {
            std::string thread_number = std::to_string(thread_stack.size());

            phread_t* thread_id = new phread_t;
            phread_attr_t* thread_id = new phread_attr_t;
			phread_mutex_t* end_mutex = new phread_mutex_t;
            MutexAndName* mutex_and_name = new MutexAndName;
            mutex_and_name.end_mutex = end_mutex;
            mutex_and_name.event_name = "Process " + thread_number + thread_number + thread_number + thread_number;
					
            pthread_create(phread_t, phread_attr_t, client_thread, mutex_and_name);
			
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
            phread_mutex_lock(thread_mutex->end_mutex);
			break;
		}
		//quit program
		case 'q': {
			exit(0);
			break;
		}
		}

		Sleep(400);
	}

    echo();
	endwin();
    return 0;
}