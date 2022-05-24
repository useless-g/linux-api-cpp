#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sched.h>

using namespace std;

static int child(void* arg){
	cout << "Hello from child!\n";
//	setuid(0);
	pid_t child_uid = getuid();
	pid_t child_euid = geteuid();
	pid_t child_gid = getgid();
	cout << "Child uid is: " << child_uid << "\nChild euid is: " 
	<< child_euid << "\nChild gid is: " << child_gid << endl;
	return 11;
}

static void write_file(char path[100], char line[100])
{
    FILE *f = fopen(path, "w");

    if (f == NULL) {
        exit(1);
    }

    if (fwrite(line, 1, strlen(line), f) < 0) {
        exit(1);
    }

    if (fclose(f) != 0) {
        exit(1);
    }
}

static void prepare_userns(int pid)
{
    char path[100];
    char line[100];

    int uid = 1000;

    sprintf(path, "/proc/%d/uid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);

    sprintf(path, "/proc/%d/setgroups", pid);
    sprintf(line, "deny");
    write_file(path, line);

    sprintf(path, "/proc/%d/gid_map", pid);
    sprintf(line, "0 %d 1\n", uid);
    write_file(path, line);
}

#define STACK_SIZE (1024 * 1024)

int main(int argc, char* argv[])
{
	cout << "Hello from parent!\n";
	char *stack;
	char *stackTop;
	stack = (char*)malloc(STACK_SIZE);
	stackTop = stack + STACK_SIZE;
	
	//здесь выполняются действия, связанные с выводом
	//параметров объектов родительского пространства имен
	pid_t parent_uid = getuid();
	pid_t parent_euid = geteuid();
	pid_t parent_gid = getgid();
	cout << "Parent uid is: " << parent_uid << "\nParent euid is: " 
	<< parent_euid <<"\nParent gid is: " << parent_gid << endl;
	
	//создание потомка
	int child_pid = clone(child, stackTop, CLONE_NEWUSER | SIGCHLD, NULL);
	if (child_pid == -1){
		perror("clone");
		exit(0);
	}
	
	prepare_userns(child_pid);
	
	//здесь выполняются действия, связанные с выводом
	//параметров объектов родительского пространства имен
	parent_uid = getuid();
	parent_euid = geteuid();
	parent_gid = getgid();
	cout << "Parent uid is: " << parent_uid << "\nParent euid is: " 
	<< parent_euid <<"\nParent gid is: " << parent_gid << endl;

	int status = 1;
	waitpid(child_pid, &status, 0);
	cout << "Child complete working, exit status: " << WEXITSTATUS(status) << endl << flush;

	//здесь выполняются действия, связанные с выводом
	//параметров объектов родительского пространства имен
	parent_uid = getuid();
	parent_euid = geteuid();
	parent_gid = getgid();
	cout << "Parent uid is: " << parent_uid << "\nParent euid is: " 
	<< parent_euid <<"\nParent gid is: " << parent_gid << endl;

	cout << "Parent complete working" << endl;
	return 0;
}

