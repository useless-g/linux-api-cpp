#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <linux/kernel.h>
#include <signal.h>

using namespace std;

const double megabyte = 1024 * 1024;
struct sysinfo si;

int fd_fifo;
			

void sig_handler(int signo)
{
	printf("get SIGPIPE\n");
}

	

static void* read_proc(void* arg)
{
	int *flag1 = (int*) arg;
	cout << "Поток read начал работу" << endl;
	char buf[21];
	int r = 0;
	int a = 0;
	while(*flag1)
	{
		memset(buf, 0, 21);
		r = read(fd_fifo, buf, 21);
		if (r != 0)
		{
			cout << buf << flush << endl;
			sleep(1);
			a = 1;
		}
		else if (a == 1)
		{
			cout << "Программа ждет нажатия клавиши" << endl;
			break;
		}
	}
	cout << "Поток read закончил работу" << endl;
	int * ret = new int;
	*ret = 2;
	pthread_exit(ret);
}

int main()
{
	signal(SIGPIPE, sig_handler);
	cout << "Программа начала работу" << endl; 
	int *status_code1 = 0; 
	int *flag1 = new int(1);
	mkfifo("/tmp/named_pipe", 0644);
	fd_fifo = open("/tmp/named_pipe", O_RDONLY | O_NONBLOCK);
	pthread_t id1;
	pthread_create(&id1, NULL, read_proc, (void*)flag1);
	cout << "Программа ждет нажатия клавиши" << endl;
	getchar();
	cout << "Клавиша нажата" << endl;
	*flag1 = 0;
	pthread_join(id1, (void**)&status_code1);
	cout << "Поток read: " << *status_code1 << endl;
	delete flag1;
	delete status_code1;
	close(fd_fifo);
	unlink("/tmp/named_pipe");
	cout << "Программа завершила работу" << endl;
}  














