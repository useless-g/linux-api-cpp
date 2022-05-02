#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <fcntl.h>

using namespace std;

int rc = 0;
const double megabyte = 1024 * 1024;
struct sysinfo si;

int filedes[2];

static void* proc1(void* arg)
{
	int *flag1 = (int*) arg; 
	cout << "Поток 1 начал работу" << endl;
	char buf[100];
	size_t message = 0;
	while(*flag1)
	{
		sysinfo(&si);
		message = sprintf(buf, "free RAM: %5.1f MB \n", si.freeram / megabyte);
		write(filedes[1], buf, message);
		sleep(1);
	}
	cout << "Поток 1 закончил работу" << endl;
	int * ret = new int;
	*ret = 1;
	pthread_exit(ret);
}

static void* proc2(void* arg)
{
	int *flag2 = (int*) arg; 
	cout << "Поток 2 начал работу" << endl;
	char buf[100];
	while(*flag2)
	{
		memset(buf, 0, 100);
		rc = read(filedes[0], buf, 100);
		if (rc > 0)
		{
			cout << buf << flush << endl;
		}
		else 
		{
			sleep(1);
			perror("Empty pipe");
		}

	}
	cout << "Поток 2 закончил работу" << endl;
	int * ret = new int;
	*ret = 2;
	pthread_exit(ret);
}

int main(){
	cout << "Программа начала работу" << endl; 
	int *status_code1 = 0; 
	int *status_code2 = 0;
	int *flag1 = new int(1);
	int *flag2 = new int(1);
	pthread_t id1;
	pthread_t id2;
	pipe(filedes);
	fcntl(filedes[0], F_SETFL, O_NONBLOCK);
	fcntl(filedes[1], F_SETFL, O_NONBLOCK);
	pthread_create(&id1, NULL, proc1, (void*)flag1);
	pthread_create(&id2, NULL, proc2, (void*)flag2);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	*flag1 = 0;
	*flag2 = 0;
	pthread_join(id1, (void**)&status_code1);
	pthread_join(id2, (void**)&status_code2);
	close(filedes[0]);
	close(filedes[1]);
	cout << "Поток 1: " << *status_code1 << endl;
	cout << "Поток 2: " << *status_code2 << endl;
	delete flag1;
	delete flag2;
	delete status_code1;
	delete status_code2;
	cout << "Программа завершила работу" << endl;
}  

