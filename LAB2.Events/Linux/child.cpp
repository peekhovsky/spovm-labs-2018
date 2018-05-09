#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]) {
    srand(time(0));
	fstream stream;
	char process_name[255];
	stream.open("answers.txt", ios::out | ios::trunc);
    
    stream << "Quantity of subjects: " << argc << "\r\n";
   
    for (int i = 0; i < argc; i++) {
        int random_num = rand() % 2;
        if (random_num == 0) {
            stream << argv[i] << " has been accepted. \r\n";
        } else {
            stream << argv[i] << " has been canceled. \r\n";
        }
    }
	stream.close();

    return 0;
}