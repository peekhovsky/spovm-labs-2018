// LAB1ChildProcess.cpp : Defines the entry point for the console application.
//

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

DWORD main(int argc, char* argv[])
{
	SYSTEMTIME lt;
	
	DWORD size = 0;

	cout << "Hello Professor! Are you bizzy for labs yet? (1 - YES / 2 - NO)" << endl;
	char choice = getKey('1', '2');

	if (choice == '1')
	{
		return 0;
	}


	cout << "Enter that you want to assess (1 - YES / 2 - NO): " << endl;
	
	LPWSTR accepted_subjects = new TCHAR[CHAR_SIZE];
	accepted_subjects[0] = '\0';

	
	for (int i = 0; i < argc; i++)
	{
		cout << argv[i] << "? (1 - YES / 2 - NO)"<<endl;
		choice = getKey('1', '2');
		if (choice == '1')
		{
			LPWSTR temp = new TCHAR[CHAR_SIZE];
			char* temp_str = new char[CHAR_SIZE];
			strcpy_s(temp_str, CHAR_SIZE, argv[i]);
			strcat_s(temp_str, CHAR_SIZE, " ");
			
			mbstowcs_s(NULL, temp, CHAR_SIZE, temp_str, CHAR_SIZE);
			wcscat_s(accepted_subjects, CHAR_SIZE, temp);
			size++;
			}
		
	}

	
	wcout <<endl<< "Accepted subjects: "<< accepted_subjects;
	wcout << endl << "Size: " << size;

	rewind(stdin);
	_getch();
	
	return size;
}