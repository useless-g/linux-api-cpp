#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

using namespace std;
struct use_mutex_time
{
	timespec t;
	pthread_mutex_t mutex;
};

struct use_mutex_tag
{
	int flag;
	pthread_mutex_t mutex;
};

struct thread_args
{	
	use_mutex_tag tag;
	use_mutex_time time;
};

void *proc1(void *args){
	thread_args *arg = (thread_args*) args;
	int *flag1 = &(arg->tag.flag);
	cout << "Поток 1 начал работу" << endl;	
	int rc1 = 0;
	int rc2 = 0;
	while(*flag1)
	{
		rc1 = pthread_mutex_lock(&(arg->time.mutex));
		if (rc1 == 0)
		{
			clock_gettime(CLOCK_REALTIME, &(arg->time.t));
			arg->time.t.tv_sec += 1;
			rc2 = pthread_mutex_timedlock(&(arg->tag.mutex), &(arg->time.t));
			if ( rc2 == 0 )
			{
				for (int i = 0; i < 3; i++)
				{
					cout << "1" << flush;
					usleep(100000);
				}
				pthread_mutex_unlock(&(arg->tag.mutex));
				sleep(1);
			}
			else 
			{	
				cout << strerror(rc2) << endl;
				sleep(1);
			}
			pthread_mutex_unlock(&(arg->time.mutex));
			sleep(1);
		}
		else
		{
			cout << strerror(rc1) << endl;
			sleep(1);
		}
	}
	cout << "Поток 1 закончил работу" << endl;
	int *status = new int(11);
	pthread_exit(status);
}
void *proc2(void *args){
	thread_args *arg = (thread_args*) args;
	int *flag2 = &(arg->tag.flag);
	cout << "Поток 2 начал работу" << endl;	
	int rc1 = 0;
	int rc2 = 0;
	while(*flag2)
	{
		rc1 = pthread_mutex_lock(&(arg->time.mutex));
		if (rc1 == 0)
		{
			clock_gettime(CLOCK_REALTIME, &(arg->time.t));
			(arg->time.t).tv_sec += 1;
			rc2 = pthread_mutex_timedlock(&(arg->tag.mutex), &(arg->time.t));
			if ( rc2 == 0 )
			{
				for (int i = 0; i < 3; i++)
				{
					cout << "2" << flush;
					usleep(100000);
				}
				pthread_mutex_unlock(&(arg->tag.mutex));
				sleep(1);
			}
			else 
			{	
				cout << strerror(rc2) << endl;
				sleep(1);
			}
			pthread_mutex_unlock(&(arg->time.mutex));
			sleep(1);
		}
		else
		{
			cout << strerror(rc1) << endl;
			sleep(1);
		}
	}	cout << "Поток 2 закончил работу" << endl;
	int *status = new int(12);
	pthread_exit(status);
}

int main(){
	thread_args arg;
	arg.tag.flag = 1;
	pthread_mutex_init(&(arg.tag.mutex), NULL);
	pthread_mutex_init(&(arg.time.mutex), NULL);
	int *status_code1 = 0; 
	int *status_code2 = 0;
	pthread_t id1;	
	pthread_t id2;
	pthread_create(&id1, NULL, proc1, (void *)&arg);
	pthread_create(&id2, NULL, proc2, (void *)&arg);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	arg.tag.flag = 0;
	pthread_join(id1, (void**)&status_code1);
	pthread_join(id2, (void**)&status_code2); 
	pthread_mutex_destroy(&(arg.tag.mutex));
	pthread_mutex_destroy(&(arg.time.mutex));
	cout << "Поток 1: " << *status_code1 << endl;
	cout << "Поток 2: " << *status_code2 << endl;
	delete status_code1;
	delete status_code2;
	cout << "Программа закончила работу" << endl;	 		
}

