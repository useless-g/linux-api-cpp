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
#include <vector>

using namespace std;

bool wasConnected = false;

pthread_t idRequestThread;
pthread_t idResponseThread;

int threadRequestExitFlag = 0;
int threadResponseExitFlag = 0;

sockaddr_un clientAddr, serverAddr;
int clientSocket;


static void* sub_request_thread(void* Arg) 
{
    int status = 0;
    
    string buf;
    int i = 0;
    string i1;
    while (threadRequestExitFlag == 0)
    {
    	i-=-1;
    	i1 = std::to_string(i);
     	buf = "Запрос " + i1;
        int sentcount = sendto(clientSocket, buf.c_str(), strlen(buf.c_str()), 0, (struct sockaddr*) &serverAddr, sizeof(struct sockaddr_un));
        if (sentcount == -1)
        {
            perror("sendto");
        	i -= 1;
        }
        else
            cout << buf.c_str() << " отправлен" << endl;
        sleep(1);
    }
    
    pthread_exit((void* )("End request thread\n"));
}


static void* sub_response_handler_thread(void* Arg) 
{

    cout << "Start response handler thread" << endl;
	int i = 0;
	
	char resp[100];
	
    while (threadResponseExitFlag == 0)
    {
    	i-=-1;
		ssize_t reccount = recvfrom(clientSocket, resp, 100, 0, NULL, NULL);
		if (reccount == -1)
		{
			perror("recvfrom");
			sleep(1);
			i -= 1;
			continue;
        }
        if (reccount == 0)
            cout << "Disconnected or nothing received" << endl;
        else
            cout << "ответ "  << i << " : " << reccount << " " << resp << endl;
    }

    pthread_exit((void*)("End response handler thread\n"));
}


int main()
{
    int status = 0;
    void* finish;
    cout << "Программа начала работу" << endl;
    struct sigaction sigattr = {SIG_IGN};
    sigaction(SIGPIPE, &sigattr, NULL);
	
	clientSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
	memset(&clientAddr, 0, sizeof(struct sockaddr_un));
    clientAddr.sun_family = AF_UNIX;
	strncpy(clientAddr.sun_path, "clsock.soc", sizeof(clientAddr.sun_path) - 1);
    status = bind(clientSocket, (struct sockaddr*) &clientAddr, sizeof(struct sockaddr_un));
    if (status == -1)
    {
        perror("bind");
        return 1;
    }
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    
    memset(&serverAddr, 0, sizeof(struct sockaddr_un));
    serverAddr.sun_family = AF_UNIX;
    strncpy(serverAddr.sun_path, "srsock.soc", sizeof(serverAddr.sun_path) - 1);

	
    pthread_create( &idRequestThread, NULL, sub_request_thread, NULL);
    pthread_create( &idResponseThread, NULL, sub_response_handler_thread, NULL);


    cout << "Программа ждет нажатия клавиши" << endl;
    getchar();
    cout << "Клавиша нажата" << endl;
    threadRequestExitFlag = 1;
    threadResponseExitFlag = 1;

    if (wasConnected)
    {
        pthread_join(idRequestThread, &finish);
        cout << (char*) finish;
        pthread_join(idResponseThread, &finish);
        cout << (char*) finish;
    }

    close(clientSocket);
      unlink("clsock.soc");
    cout << "Программа завершила работу\n";
    return 0;
}


