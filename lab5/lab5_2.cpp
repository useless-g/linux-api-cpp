#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h> 
#include <string.h>
#include <cerrno>

using namespace std;

int main(){
	sem_t* semid = new sem_t;
	ofstream file;
	file.open("file.txt", ios::app);
	const char* sem_name = "/my_sem";
	semid = sem_open(sem_name, O_CREAT, 0644, 1);
	cout << semid << endl;	
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		
	cout << "Программа начала работу" << endl;
	cout << "Программа ждет нажатия клавиши" << endl	;
	
	while(1){
		sem_wait(semid);
		for (int i = 0; i < 10; ++i){
			file << "2" << flush;
			sleep(1);
		}
		cout << "Wrote 10 symbols '2' in the file" << endl << flush;
		sem_post(semid);
		sleep (2);
		if(getchar() != EOF){
			break;
		}
		
	}

	cout << "Программа 2 завершила свою работу" << endl;
	file.close();
	sem_close(semid);
	sem_unlink(sem_name);
	return 1;
}

