#include <iostream>
#include <stdio.h>
#include <cstdlib>

#ifdef WIN32
    //#include <vld.h>
    #include <Windows.h>
#endif

#include <Silk.h>

int main(int ArgC,char *ArgV[])
{
    printf("Hello World\n");
    return 0;
}
