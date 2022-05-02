#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[]){
	cout << "Hello from parent!"<< endl;
	pid_t ch_pid = fork();
	if ((ch_pid != 0) && (ch_pid != -1)){
		int status = 1;
		pid_t my_pid = getpid();
		pid_t parent_pid = getppid();
		cout << "My child's pid is: " << ch_pid <<"; My pid is: " << my_pid << "; My parent's pid is: " << parent_pid << endl;
		while(waitpid(ch_pid, &status, WNOHANG) == 0){
			cout << "Waiting..." << endl;
			usleep(500000);
			}
		cout << "Child complete working, exit status: " << WEXITSTATUS(status) << endl << flush;
		}
	else if (ch_pid == 0){
		char** args = new char*[3];
		for (int i = 0; i < sizeof(args) / sizeof(char**); ++i)
		{
			args[i] = new char[20];
		}
		args[0] = (char*)"Hello";
		args[1] = (char*)", ";
		args[2] = (char*)"World!";
		execl("lab4_child", args[0], args[1], args[2], NULL);
	}
	else perror("fork");
}

