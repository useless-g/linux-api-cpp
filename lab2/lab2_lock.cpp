#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace std;

struct use_mutex_tag
{
	int flag;
	pthread_mutex_t mutex;
};

void *proc1(void *args){
	use_mutex_tag *arg = (use_mutex_tag*) args;
	int *flag1 = &(arg->flag);
	cout << "Поток 1 начал работу" << endl;	
	int rc = 0;
	while(*flag1)
	{
		rc = pthread_mutex_lock(&(arg->mutex));
		if ( rc == 0 )
		{
			for (int i = 0; i < 3; i++)
			{
				cout << "1" << flush;
				sleep(1);
			}
			pthread_mutex_unlock(&(arg->mutex));
			sleep(1);
		}
		else 
		{	
			cout << strerror(rc) << endl;
			sleep(1);
		}
	}
	cout << "Поток 1 закончил работу" << endl;
	int *status = new int(11);
	pthread_exit(status);
}
void *proc2(void *args){
	use_mutex_tag *arg = (use_mutex_tag*) args;
	int *flag2 = &(arg->flag);
	int rc = 0;
	cout<<"Поток 2 начал работу" << endl;	
	while(*flag2)
	{
		rc = pthread_mutex_lock(&(arg->mutex));
		if ( rc == 0 )
		{
			for (int i = 0; i < 3; i++)
			{
				cout << "2" << flush;
				sleep(1);
			}
			pthread_mutex_unlock(&(arg->mutex));
			sleep(1);
		}
		else 
		{
			cout << strerror(rc) << endl;
			sleep(1);
		}
	}
	cout << "Поток 2 закончил работу" << endl;
	int *status = new int(12);
	pthread_exit(status);
}

int main(){
	use_mutex_tag arg;
	arg.flag = 1;
	pthread_mutex_init(&(arg.mutex), NULL);
	int *status_code1 = 0; 
	int *status_code2 = 0;
	pthread_t id1;	
	pthread_t id2;
	pthread_create(&id1, NULL, proc1, (void *)&arg);
	pthread_create(&id2, NULL, proc2, (void *)&arg);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	arg.flag = 0;
	pthread_join(id1, (void**)&status_code1);
	pthread_join(id2, (void**)&status_code2); 
	pthread_mutex_destroy(&(arg.mutex));
	cout << "Поток 1: " << *status_code1 << endl;
	cout << "Поток 2: " << *status_code2 << endl;
	delete status_code1;
	delete status_code2;
	cout << "Программа закончила работу" << endl;	 		
}

