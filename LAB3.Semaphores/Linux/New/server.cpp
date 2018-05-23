#include <iostream>
#include <stdio.h>
#include <termios.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <wait.h>
#include <semaphore.h>
#include <string.h>
#include <math.h>
#include <sstream>  
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

using namespace std;

class Cashmachine
{
public:
	float rate_USD;
	float rate_BYN;
	float rate_RUB;
	float balanceATM_USD;
	float balanceATM_BYN;
	float balanceATM_RUB;
	float balanceClient_USD;
	float balanceClient_BYN;
	float balanceClient_RUB;

	Cashmachine(int);
	Cashmachine();
	~Cashmachine();


	bool loadFromFileCurrency(fstream&);
	bool loadToFileCurrency(fstream&);
	void ouputInformationOfClient();
	int checkBalance(string, double);
	int withdrawMoneyWin(string, double);

};

Cashmachine::Cashmachine(int check)
{
	this->rate_USD = 1.96;
	this->rate_BYN = 1;
	this->rate_RUB = 3.45;
	this->balanceATM_USD = 2000;
	this->balanceATM_BYN = 2000;
	this->balanceATM_RUB = 2000;
	this->balanceClient_USD = 300;
	this->balanceClient_BYN = 310;
	this->balanceClient_RUB = 200;
}

Cashmachine::Cashmachine() {}

Cashmachine::~Cashmachine() {}

bool Cashmachine::loadFromFileCurrency(fstream& fileName)
{
	fileName >> this->rate_USD;
	fileName >> this->rate_BYN;
	fileName >> this->rate_RUB;
	fileName >> this->balanceATM_USD;
	fileName >> this->balanceATM_BYN;
	fileName >> this->balanceATM_RUB;
	fileName >> this->balanceClient_USD;
	fileName >> this->balanceClient_BYN;
	fileName >> this->balanceClient_RUB;
	return true;
}

bool Cashmachine::loadToFileCurrency(fstream& fileName)
{
	fileName << this->rate_USD << '\n';
	fileName << this->rate_BYN << '\n';
	fileName << this->rate_RUB << '\n';
	fileName << this->balanceATM_USD << '\n';
	fileName << this->balanceATM_BYN << '\n';
	fileName << this->balanceATM_RUB << '\n';
	fileName << this->balanceClient_USD << '\n';
	fileName << this->balanceClient_BYN << '\n';
	fileName << this->balanceClient_RUB << '\n';
	return true;
}

void Cashmachine::ouputInformationOfClient()
{
	cout << "==============================================\r\n";
	cout << "      INFORMATION ABOUT CLIENT:\r\n";
	cout << "                USD: " << this->balanceClient_USD << "\r\n";
	cout << "                BYN: " << this->balanceClient_BYN << "\r\n";
	cout << "                RUB: " << this->balanceClient_RUB << "\r\n";
	cout << "==============================================" << "\r\n";
	cout << "Exchange rate for today: USD:" << this->rate_USD << ", RUB:" << this->rate_RUB << "\r\n";
}

int Cashmachine::checkBalance(string currency, double amountOfMoney)
{
	if (amountOfMoney < 0) return 3;

	if (currency == "USD")
	{
		if (amountOfMoney > this->balanceClient_USD) return 1;
		if (amountOfMoney > this->balanceATM_USD) return 2;
		else return 0;
	}
	if (currency == "BYN")
	{
		if (amountOfMoney > this->balanceClient_BYN) return 1;
		if (amountOfMoney > this->balanceATM_BYN) return 2;
		else return 0;
	}
	if (currency == "RUB")
	{
		if (amountOfMoney > this->balanceClient_RUB) return 1;
		if (amountOfMoney > this->balanceATM_RUB) return 2;
		else return 0;
	}
	if (currency != "USD" && currency != "BYN" && currency != "RUB") return 3;
}

int Cashmachine::withdrawMoneyWin(string currency, double amountOfMoney)
{
	if (checkBalance(currency, amountOfMoney) == 1) return 1;
	if (checkBalance(currency, amountOfMoney) == 2) return 2;
	if (checkBalance(currency, amountOfMoney) == 3) return 3;
	if (currency == "USD")
	{
		this->balanceATM_USD -= amountOfMoney;
		this->balanceClient_USD -= amountOfMoney;
	}
	if (currency == "BYN")
	{
		this->balanceATM_BYN -= amountOfMoney;
		this->balanceClient_BYN -= amountOfMoney;
	}
	if (currency == "RUB")
	{
		this->balanceATM_RUB -= amountOfMoney;
		this->balanceClient_RUB -= amountOfMoney;
	}
	return 0;
}

using namespace std;

union semun 
{
  int              val;    /* Value for SETVAL */
  struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
  unsigned short  *array;  /* Array for GETALL, SETALL */
  struct seminfo  *__buf;  /* Buffer for IPC_INFO(Linux-specific) */
} semunion;

int WaitSemaphore(int textSemId, int num, int waitTime);
int ReleaseSemaphore(int textSemId, int num);
void parseMessage(string, char* &, char* &);

int main(int argc, char *argv[])
{  
  initscr ();
  curs_set (0);
  clear();
  noecho();
  refresh();

  char* amountOfMoney = new char[256];
  char* currencyOfMoney = new char[256];
  int bufferSize = 64; 

  Cashmachine cashmachine(1);
  fstream file;
  file.open("log.txt", ios::in);
  cashmachine.loadFromFileCurrency(file);
  file.close();

  //--------------------------------------------------------------------------

  srand ( time(NULL) );
  key_t shmkey = getpid() * (rand() % 100);
  int shmId = shmget(shmkey, bufferSize, IPC_CREAT | 0666); 

  if (shmId < 0) 
  {
    printf("shmget error\n");
    return 1;
  }    

  srand ( time(NULL) );
  key_t textSemKey = ftok("/client", 'A') * (rand() % 100);
  int textSemId = semget(textSemKey, 4, IPC_CREAT | 0666);

  semctl(textSemId, 0, SETALL, 0);
  if(textSemId < 0)
  {
    printf("Text Semaphores is not created.");
    return 0;
  }

  //--------------------------------------------------------------------------

  int pid = fork();
  switch(pid)
  {
  case -1:
    printf("New process is not created\n");
    return 0;
    
  case 0: 
    {
      stringstream strs;
      strs << (int)shmkey;
      string temp_str = strs.str();
      char* char_type = (char*) temp_str.c_str();
      stringstream strs1;
      strs1 << (int)textSemKey;
      string temp_str1 = strs1.str();
      char* char_type1 = (char*) temp_str1.c_str();
      if(execlp("./server", "server", char_type, char_type1, NULL) == -1)
        printf("Error.");
      break;
    }
    break;
    
  default: 
    {
      char *shm, *s;
      shm = (char*)shmat(shmId, NULL, 0);
      if (shm == (char *) -1) 
      {
          perror("shmat");
          return 1;
      }


      while(true)
      {
          WaitSemaphore(textSemId, 2, -1);
          int p = 0;
          char tempMessage[bufferSize];
          for(s = shm; *s != NULL; s++, p++)
          {
            tempMessage[p] = *s;
          }
          ReleaseSemaphore(textSemId, 1); 
          string message(tempMessage);
          if(message == "quit") break;

         parseMessage(message, amountOfMoney, currencyOfMoney);
         cashmachine.withdrawMoneyWin(currencyOfMoney, atof(amountOfMoney));

          ReleaseSemaphore(textSemId, 2);   
          s = shm;
          strncpy(s, message.c_str(), sizeof(message));
          WaitSemaphore(textSemId, 1, -1);
         }
       semctl(textSemId, 0, IPC_RMID, semunion);
       shmdt(shm);
    }
    break;
  }

//--------------------------------------------------------------------------
  clear();  
  endwin();

  file.open("log.txt", ios::out);
  cashmachine.loadToFileCurrency(file);
  file.close();

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

void parseMessage(string message, char* &amountOfMoney, char* &currencyOfMoney)
{
	char buffer[256];
    strcpy(buffer, message.c_str());

	int i = 0;
	while (buffer[i] != ' ')
	{
		amountOfMoney[i] = buffer[i];
		i++;
	}
	amountOfMoney[i] = '\0';

	while (buffer[i] != ' ')
		i++;

	i++;

	currencyOfMoney[0] = buffer[i];
	i++;
	currencyOfMoney[1] = buffer[i];
	i++;
	currencyOfMoney[2] = buffer[i];

	currencyOfMoney[3] = '\0';
}