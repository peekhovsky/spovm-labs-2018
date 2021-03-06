// LAB1ChildProcess.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include<iostream>
#include<string>
#include<climits>

using namespace std;


#define CHAR_SIZE 100
#define PIPE_LOCATION "\\\\.\\pipe\\MyPipe"

#define SEMAPHORE_NAME "SEMAPHORE"
HANDLE semaphore;
HANDLE pipe;

char getKey(char min, char max)
{
	char res;
	do
	{
		res = _getch();
		if (res > max || res < min) puts("Incorrect input. Try again");
		else break;
	} while (true);

	return res;
}

void close_handles() {
	CloseHandle(semaphore);
	CloseHandle(pipe);
}

BOOL WINAPI ConsoleHandler(DWORD CEvent) {

	switch (CEvent) {
	case CTRL_CLOSE_EVENT:
		if (semaphore != nullptr) {
			close_handles();
		}
	}
	return TRUE;
}

int main(int argc, char* argv[]) {
	
	//set console handler to close all processes if user closes console
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		printf("Unable to install handler!\n");
		return -1;
	}

	//open semaphore
	semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, TEXT(SEMAPHORE_NAME));

	//open pipe
	char szPipeName[256] = { '\0' };

	pipe = CreateFile(TEXT(PIPE_LOCATION), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	DWORD size = 0;

	//
	cout << "Hello Professor! Are you free for labs now? (1 - YES / 2 - NO)" << endl;
	char choice = getKey('1', '2');

	if (choice == '2') {
		return 0;
	}

	cout << "Enter that you want to assess (1 - YES / 2 - NO): " << endl;

	char* accepted_subjects = new char[CHAR_SIZE];
	accepted_subjects[0] = '\0';


	for (int i = 0; i < argc; i++) {
		cout << argv[i] << "? (1 - YES / 2 - NO)" << endl;
		choice = getKey('1', '2');
		if (choice == '1')
		{
			char* temp = new char[CHAR_SIZE];
			char* temp_str = new char[CHAR_SIZE];
			strcpy_s(temp_str, CHAR_SIZE, argv[i]);
			strcat_s(temp_str, CHAR_SIZE, " ");

			strcat_s(accepted_subjects, CHAR_SIZE, temp_str);
			size++;
		}
	}
	//write to pipe
	char   buffer[200] = { '\0' };
	strcat_s(buffer, 200, accepted_subjects);
	DWORD sizej;
	WriteFile(pipe, accepted_subjects, sizeof(buffer), &sizej, NULL);

	//release semaphore
	ReleaseSemaphore(semaphore, 1, NULL);
	cout << endl << sizej;
	cout << endl << "Accepted subjects: " << accepted_subjects;
	cout << endl << "Size: " << size;
	_getch();




	close_handles();
	return size;
}