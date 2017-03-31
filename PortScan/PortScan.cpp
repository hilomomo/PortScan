#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <winbase.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

#define MAXThreadCount 100//设置最大线程数
#define SumScanCount 65535//设置扫描端口总数
struct sockaddr_in server;
int ThreadCount=0; 
int dwThrdParam = 1; //CreateThread的参数，提到前面好计算

unsigned int resolve(char *name)
{
    struct hostent *he;
    unsigned int ip;

    if((ip=inet_addr(name))==(-1))
    {
        if((he = gethostbyname(name))==0)
  {
   printf("ERROR: Don't find the %s .\n",name);
            exit(0);
  }
        memcpy(&ip,he->h_addr,4);
    }
    return ip;
}

DWORD WINAPI Scan(LPVOID lpParam ) 
{ 
 int mysock,code,port=(int)lpParam ;
 mysock=socket(AF_INET,SOCK_STREAM,0);
 if(mysock < 0) { printf("socket error!"); }
 
 
 server.sin_port = htons(port);
 
 if(connect(mysock,(struct sockaddr *) & server,sizeof(server))!=0)
 {
  code=GetLastError();
  if(code==10061)printf("port %d allown open\n",port);
 }
 else
 {
  printf("port %d openning\n",port);
 }
 closesocket(mysock);
 ThreadCount--;
    return 0; 
} 
void thread(int port)
{
 DWORD dwThreadId;
    HANDLE hThread; 
 WSADATA ws;
  if   (WSAStartup( MAKEWORD(2,2), &ws )!=0)
  {
   printf(" [-] WSAStartup() error\n");
   exit(0);
  }
 
    hThread = CreateThread( 
        NULL,                        // no security attributes 
        0,                           // use default stack size  
        Scan,                  // thread function 
        (LPVOID)port,                // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier 
   if (hThread == NULL) 
     printf( "CreateThread failed." ); 
 dwThrdParam++;
 ThreadCount++;
 Sleep(200);     //延时，否则CPU会用满……
  CloseHandle(hThread);
}


void main(int argc, char* argv[]) 
{ 
 if(argc!=2)
 {
  printf("\n- This program find port that firewall allow open  -\n");
  printf("- Only for test by lake2 - \n");
  printf("Usage: %s IP\n",argv[0]);
  exit(0);
 }
 server.sin_family = AF_INET;
 server.sin_addr.s_addr = resolve( argv[1] );
 printf("Starting and waiting..............\n");
 while( dwThrdParam <= SumScanCount )
 {
  if( ThreadCount < MAXThreadCount ){ thread(dwThrdParam); }
 }
 while( ThreadCount!=0 ){ }
 WSACleanup();
 printf("Done!");
}