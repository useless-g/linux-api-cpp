#include <iostream>
#include <stdio.h>
#include <chrono>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ucontext.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

using namespace std;

#ifndef byte
#define byte unsigned char
#endif


#define DEF_STACK_SIZE 32000

using FIBER_SUB_PROTO = void(*)();

ucontext_t uctx_fiber1;
ucontext_t uctx_fiber2;
ucontext_t uctx_fiber_M;

bool kbhit();


bool CreateFiber(FIBER_SUB_PROTO Function, const size_t StackSize, ucontext_t* Context);
void FreeFiber(ucontext_t* Context);


void sub_fiber_1()
{
    cout << "Start fiber 1" << endl;
    
    while (1)
    {
        cout << "1" << endl;
        sleep(1); 
        swapcontext(&uctx_fiber1, &uctx_fiber2);
    }
    
}


void sub_fiber_2()
{
    cout << "Start fiber 2" << endl;
    
    while (1)
    {
        cout << "2" << endl;
        sleep(1);

        while(kbhit())
        {
            if (getchar() == 10) 
            {
             	cout << "Клавиша нажата" << endl;
                swapcontext(&uctx_fiber2, &uctx_fiber_M);
            }
        }
              
        swapcontext(&uctx_fiber2, &uctx_fiber1);
    }

}


int main()
{
    cout << "Программа начала работу" << endl;

    CreateFiber(sub_fiber_1, DEF_STACK_SIZE, &uctx_fiber1);
    CreateFiber(sub_fiber_2, DEF_STACK_SIZE, &uctx_fiber2);
    CreateFiber((FIBER_SUB_PROTO)main, DEF_STACK_SIZE, &uctx_fiber_M);
	
	cout << "Программа ждет нажатия клавиши" << endl;
		
    swapcontext(&uctx_fiber_M, &uctx_fiber1);

    FreeFiber(&uctx_fiber1);
    FreeFiber(&uctx_fiber2);
    FreeFiber(&uctx_fiber_M);
    
    cout << "Программа завершила работу" << endl;
    return 0;
}

bool CreateFiber(FIBER_SUB_PROTO Function, const size_t StackSize, ucontext_t* Context)
{
    byte* stackPointer = nullptr;

    if( (stackPointer = (byte*) malloc(StackSize)) == nullptr )
        return false;

    if (getcontext(Context) != 0) {
        cout << "getcontext error:" << strerror(errno) << endl;
        return 2;
    }
    Context->uc_stack.ss_sp = stackPointer;
    Context->uc_stack.ss_size = StackSize;
    makecontext(Context, Function, 0);

    return true;
}

;
void FreeFiber(ucontext_t* Context)
{
    free(Context->uc_stack.ss_sp);

    Context->uc_stack.ss_sp = nullptr;
    Context->uc_stack.ss_size = 0;
}

bool kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}
