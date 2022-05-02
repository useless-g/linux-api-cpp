#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std; 

int main (int argc, char* argv[]){
	cout << "Hello from child! My args is: ";	
	for (int i = 0; i < argc; ++i) {
		cout << argv[i];
		usleep (500000);
	}
	cout << endl;
	pid_t my_pid = getpid();
	pid_t parent_pid = getppid();
	cout << "My pid is: " << my_pid << "; My parent's pid is: " << parent_pid << endl;
	return 11;
}
 
