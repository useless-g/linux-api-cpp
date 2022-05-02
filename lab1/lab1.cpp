#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
using namespace std;

typedef struct 
{
	int flag;
	char sym;
}targs;

static void* proc1(void* arg)
{
	cout << "Поток 1 начал работу" << endl;
	targs *args = (targs*) arg;
	while(args->flag)
	{
		cout << args->sym << flush;
		sleep(1);
	}
	cout << "Поток 1 закончил работу" << endl;
	int * ret = new int;
	*ret = 1;
	pthread_exit(ret);
}

static void* proc2(void* arg)
{
	cout << "Поток 2 начал работу" << endl;
	targs *args = (targs*) arg;
	while(args->flag)
	{
		cout << args->sym << flush;
		sleep(1);
	}
	cout << "Поток 2 закончил работу" << endl;
	int * ret = new int;
	*ret = 2;
	pthread_exit(ret);
}

int main()
{
	cout << "Программа начала работу" << endl; 
	targs arg1;
	targs arg2;
	arg1.flag = 1;
	arg1.sym = '1';
	arg2.flag = 1;
	arg2.sym = '2';
	int *status_code1 = 0; 
	int *status_code2 = 0;
	pthread_t id1;
	pthread_t id2;
	pthread_create(&id1, NULL, proc1, &arg1);
	pthread_create(&id2, NULL, proc2, &arg2);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	arg1.flag = 0;
	arg2.flag = 0;
	pthread_join(id1, (void**)&status_code1);
	pthread_join(id2, (void**)&status_code2);
	cout << "Поток 1: " << *status_code1 << endl;
	cout << "Поток 2: " << *status_code2 << endl;
	delete status_code1;
	delete status_code2;
	cout << "Программа завершила работу" << endl;
}





