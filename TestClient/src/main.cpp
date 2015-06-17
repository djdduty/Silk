#include <iostream>
#include <stdio.h>
#include <cstdlib>

#ifdef WIN32
    //#include <vld.h>
    #include <Windows.h>
#endif

int main(i32 ArgC,Literal ArgV[])
{
    printf("Hello World\n");
    return 0;
}
