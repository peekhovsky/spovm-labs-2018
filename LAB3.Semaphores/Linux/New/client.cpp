#include <iostream>
#include <stdio.h>
#include <termios.h> 
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <math.h>
#include <sstream>

using namespace std;

string toString(int val);
void putMenu();
int WaitSemaphore(int textSemId, int num, int waitTime);
int ReleaseSemaphore(int textSemId, int num);


int main(int argc, char *argv[])
{  
	initscr ();
	curs_set (0);

	string amountOfMoney = "60";
	string currencyOfMoney = "BYN";

	int mainPid;
	int textSemKey;

	char strRes[256];

	char arrOfString[256][256] = { "But I would walk 500 miles.", "And I would walk 500 more.",
		"Just to be the man who walks a thousand miles.",
		"To fall down at your door.", "500 MILES." };
	int currentProcess = atoi(argv[1]);
	if (currentProcess == 1)
		sprintf(strRes, "%s ", arrOfString[0]);
	else if (currentProcess == 2)
		sprintf(strRes, "%s ", arrOfString[1]);
	else if (currentProcess == 3)
		sprintf(strRes, "%s ", arrOfString[2]);
	else if (currentProcess == 4)
		sprintf(strRes, "%s ", arrOfString[3]);
	else if (currentProcess == 5)
		sprintf(strRes, "%s ", arrOfString[4]);

	if(argv[2])
	{
		mainPid = atoi(argv[2]);
	}
	else
	{
		return 0;
	}
	
	int bufferSize = 64; 
	int shmId = shmget((key_t)mainPid, bufferSize, 0666); 

	if (shmId < 0) 
	{
		printf("shmget error\n");
		return(0);
	}    

	int mainSemId = semget(getpid(), 0, IPC_CREAT|0666);

	  semctl(mainSemId, 0, SETALL, 0);
	  
	  if(mainSemId < 0)
	  {
	    printf("Main Semaphores is not created.");
	    return 0;
	  }

	if(argv[3])
	{
		textSemKey = atoi(argv[3]);
	}
	else
	{
		return 0;
	}

	int textSemId = semget((key_t)textSemKey, 0, IPC_CREAT|0666);

	semctl(textSemId, 0, SETALL, 0);
	if(textSemId < 0)
	{
		printf("Text Semaphores is not connected.");
		return 0;
	}

	string message;
	message = amountOfMoney + " " + currencyOfMoney + '\0';

	char *shm, *s;
	shm = (char*)shmat(shmId, NULL, 0);
	if (shm == (char *) -1) 
	{
	    perror("shmat");
	    return 1;
	}

	ReleaseSemaphore(textSemId, 2);		
	s = shm;
    strncpy(s, message.c_str(), sizeof(message));

    WaitSemaphore(textSemId, 1, -1);

    char tempMessage[bufferSize];
	  WaitSemaphore(textSemId, 2, -1);
	  int p = 0;
	  for(s = shm; *s != NULL; s++, p++)
	  {
	    tempMessage[p] = *s;
	  refresh();
	  }

	  ReleaseSemaphore(textSemId, 1); 

	char putString[256];

	if(argv[1])
	{
	  sprintf(putString, "%s", strRes);
 	}	
	else
	{
	    return 0;
	}

      while(WaitSemaphore(mainSemId, 3, 1) != 0)
      { 
	    if(WaitSemaphore(mainSemId, 1, 1) == 0)
	    {
	      for(int i = 0; i < strlen(putString); i++)
	      {
		        if(WaitSemaphore(mainSemId, 3, 1) == 0)
		        {
		          ReleaseSemaphore(mainSemId, 2);
		          return 0;
		        }  
		        printw ("%c", putString[i]);
			    refresh();
			    usleep(70000);
	      }
	      refresh();
	      ReleaseSemaphore(mainSemId, 2);
	    }
      }  		  
	return 0;
}

int WaitSemaphore(int textSemId, int num, int tempTime)
{
	long long int waitTime = tempTime * 100000;
	const struct timespec time = {0, waitTime};
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	if(waitTime < 0)
	{
		return semop(textSemId, &buf, 1);	
	}
	else 
	{
		return semtimedop(textSemId, &buf, 1, &time);	
	}
}

int ReleaseSemaphore(int textSemId, int num)
{
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	return semtimedop(textSemId, &buf, 1, NULL); 
}


string toString(int val)
{
    stringstream stream;
    stream << val;
    return stream.str();
}