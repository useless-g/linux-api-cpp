#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <signal.h>

using namespace std;

const double megabyte = 1024 * 1024;
struct sysinfo si;

int fd_fifo;

int *flag1 = new int(1);
int *flag2 = new int(1);
	
	
void sig_handler(int signo)
{
	printf("get SIGPIPE\n");
	*flag2 = 0;
	cout << "Нажмите enter" << endl;
}


static void* write_proc(void* arg)
{
	int* flag2 = (int*) arg;
	cout << "Поток write начал работу" << endl;
	char buf[21];
	size_t message = 0;
	while(*flag2)
	{
		sysinfo(&si);
		message = sprintf(buf, "free RAM: %5.1f MB \n", si.freeram / megabyte);
		write(fd_fifo, buf, message);
		sleep(1);
	}
	cout << "Поток write закончил работу" << endl;
	int * ret = new int;
	*ret = 2;
	pthread_exit(ret);
}


static void* key_pressed(void* arg)
{
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	*flag1 = 0;
	*flag2 = 0;
	cout << "Клавиша нажата" << endl;
	pthread_exit(0);
}

		

int main()
{
	signal(SIGPIPE, sig_handler);
	cout << "Программа начала работу" << endl; 
	int *status_code = 0;
	mkfifo("/tmp/named_pipe", 0644);
	pthread_t id1;
	pthread_create(&id1, NULL, key_pressed, (void*)flag1);	
	while(*flag1)
	{
		fd_fifo = open("/tmp/named_pipe", O_WRONLY | O_NONBLOCK);
		if (fd_fifo == -1)
		{
			perror("");
			sleep(1);
			continue;
		}
		break;
	}
	if (*flag1 != 0)
	{
		pthread_t id2;
		pthread_create(&id2, NULL, write_proc, (void*)flag2);
		pthread_join(id2, (void**)&status_code);
		cout << "Поток write: " << *status_code << endl;
	}
	
	pthread_join(id1, (void**)&status_code);
	delete flag1;
	delete flag2;
	delete status_code;
	
	close(fd_fifo);
	unlink("/tmp/named_pipe");
	cout << "Программа завершила работу" << endl;
}  





