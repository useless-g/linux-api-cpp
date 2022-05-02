#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <cerrno>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

const double megabyte = 1024 * 1024;
struct sysinfo si;


int flag = 1;
sem_t* wsemid;
sem_t* rsemid;
int mem_id = 0;
void* addr;	

static void* foo1(void* arg)
{
	
	int message = 0;
	while(flag)
	{
		sem_wait(wsemid);
		message = *((int*)addr);
		cout << "free RAM: " << message << " MB \n" << flush;
		sem_post(rsemid);
	}
	pthread_exit(NULL);
}


int main()
{
	pthread_t id1;
	const char* wsem_name = "MyWSem_1";
	const char* rsem_name = "MyRSem_1";
	key_t mykey = ftok("./myfile", 1);
	mem_id = shmget(mykey, sizeof(int), IPC_CREAT | 0666);
	addr = shmat(mem_id, addr, SHM_RDONLY);
	//cout << mykey << endl << mem_id << endl << addr << endl;
	wsemid = sem_open(wsem_name, O_CREAT, 0644, 0);
	rsemid = sem_open(rsem_name, O_CREAT, 0644, 0);
	cout << "Программа начала работу" << endl;
	pthread_create(&id1, NULL, foo1, NULL);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	flag = 0;
	pthread_join(id1, NULL);
	sem_close(rsemid);
	sem_unlink(rsem_name);
	sem_close(wsemid);
	sem_unlink(wsem_name);	
	shmdt(addr);
	struct shmid_ds *buf;
	shmctl(mem_id, IPC_RMID, buf);
	cout << "Программа завершила работу" << endl;
}






