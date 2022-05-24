#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>
#include <queue>
#include <sys/sysinfo.h>
#include <linux/kernel.h>

using namespace std;

std::queue<char*> requestQueue;
const double megabyte = 1024 * 1024;
struct sysinfo si;

pthread_mutex_t globalMutex;

int serverSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
int clientSocket = -1;
sockaddr_un serverAddr, clientAddr;
socklen_t slen = sizeof(struct sockaddr_un);
pthread_t idRequestThread;
pthread_t idResponseThread;

int threadRequestExitFlag = 0;
int threadResponseExitFlag = 0;


static void* sub_request_thread(void* Arg) 
{
    char req[100];

    cout << "Поток приема запросов начал работу\n" << endl;

    while (threadRequestExitFlag == 0) 
    {
    	memset(req, 0, 100);
        ssize_t reccount = recvfrom(serverSocket, req, sizeof(req), 0, (struct sockaddr*) &clientAddr, &slen);
        if (reccount == -1)
        {
            perror("recvfrom");
            sleep(1);   
        }
        else
        {
        	pthread_mutex_lock(&globalMutex);

			cout << req << " принят" << endl;
			requestQueue.push(req);
			
            pthread_mutex_unlock(&globalMutex);
        }
    }

    pthread_exit((void* )("Поток приема запросов закончил работу\n"));
}


static void* sub_response_handler_thread(void* Arg) 
{
    cout << "Поток обработки запросов и передачи ответов начал работу\n" << endl;
    char resp[100];

    
    while (threadResponseExitFlag == 0)
    {	   		
    	if (!requestQueue.empty())
    	{	
    		pthread_mutex_lock(&globalMutex);
    		requestQueue.pop();
    	    if(pthread_mutex_unlock(&globalMutex) != 0) 
    	        pthread_exit((void*)("Поток обработки запросов и передачи ответов закончил работу\n"));
   	
			sysinfo(&si);
			int len = sprintf(resp, "free RAM: %5.1f MB \n", si.freeram / megabyte);
			
    	    int sentcount = sendto(serverSocket, resp, len, 0,  (struct sockaddr*) &clientAddr, slen);
    	    if (sentcount == -1)
    	    {
    	        perror("sendto");
    	        pthread_exit((void*)("Поток обработки запросов и передачи ответов закончил работу\n"));
    	    	sleep(1);
    	    }
    	    else
	            cout << "Ответ: " << resp  <<  endl;
    	}
    	else
    	{
    		cout << "No requests" << endl;
    		sleep(1);
    	}
    }

    pthread_exit((void*)("Поток обработки запросов и передачи ответов закончил работу\n"));
}


int main()
{
    int status = 0;
    void* finish;
    struct sigaction sigattr = {SIG_IGN};
    sigaction(SIGPIPE, &sigattr, NULL);
    cout << "Программа начала работу" << endl;
    
    int optval = 1;
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
	memset(&serverAddr, 0, sizeof(struct sockaddr_un));  
    serverAddr.sun_family = AF_UNIX;
	strncpy(serverAddr.sun_path, "srsock.soc", sizeof(serverAddr.sun_path) - 1);
    status = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (status == -1)
    {
        perror("");
        return 1;
    }
    
    pthread_create( &idRequestThread, NULL, sub_request_thread, NULL);
    pthread_create( &idResponseThread, NULL, sub_response_handler_thread, NULL);

    cout << "Программа ждет нажатия клавиши" << endl;
    getchar();
    cout << "Клавиша нажата" << endl;
    threadRequestExitFlag = 1;
    threadResponseExitFlag = 1;
    
    pthread_join(idRequestThread, &finish);
    pthread_join(idResponseThread, &finish);
    close(serverSocket);
    close(clientSocket);
    unlink("srsock.soc");
    
    cout << "Программа завершила работу\n";
    return 0;
}


