// OpenCVServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "tcpServer.h"
#include "winsock2.h"
#include "windows.h"
#include <vector>
#pragma comment(lib,"ws2_32.lib")

using namespace std;
using namespace cv;

typedef struct _para{
	HANDLE thread;
	SOCKET socks;
}ThreadSock;

typedef struct _threadimg{
	char *pointer;
	int width,height;
}ThreadImg;

vector<ThreadSock> ThreadArr;
vector<HANDLE> TSendArr;
vector<SOCKET> SocketArr;
SOCKET sock;
HANDLE hSubThread;
CRITICAL_SECTION	m_cs;
int ImgReady;
void Server(){
	boost::asio::io_service io_service;
	tcp_server server1(io_service);
	udp_server server2(io_service);
	io_service.run();

}

bool IsSocketClosed(SOCKET clientSocket)  
{  
 bool ret = false;  
 HANDLE closeEvent = WSACreateEvent();  
 WSAEventSelect(clientSocket, closeEvent, FD_CLOSE);  
  
 DWORD dwRet = WaitForSingleObject(closeEvent, 0);  
   
 if(dwRet == WSA_WAIT_EVENT_0)  
  ret = true;  
 else if(dwRet == WSA_WAIT_TIMEOUT)  
  ret = false;  
  
 WSACloseEvent(closeEvent);  
 return ret;  
}  

void ReceieveSockets(LPVOID   pParam){
	
	SOCKET clientSocket=(SOCKET)pParam;
	struct sockaddr_in clientAddress;//用来和客户端通信的套接字地址
    memset(&clientAddress,0,sizeof(clientAddress));
    int addrlen = sizeof(clientAddress);
    char buf[307200];
	while(1){
        //接收数据0
        int bytes;

        if((bytes=recv(clientSocket,buf,sizeof(buf),0))==SOCKET_ERROR){
            printf("接收数据失败!\n");
            exit(-1);
        }
		if(bytes==0){

			vector<ThreadSock>::iterator it=ThreadArr.begin();
			for(int i=0;i<ThreadArr.size();i++){
				
				ThreadSock tmp=*it;
				if(tmp.socks==clientSocket){
					ThreadArr.erase(it);
					break;
				}
				++it;
			}
			return;
		}
		

		printf("len:%d",bytes);
        buf[bytes]='\0';
		
		vector<ThreadSock>::iterator it=ThreadArr.begin();
		for(int i=0;i<ThreadArr.size();i++){
			
			ThreadSock tmp=*it;
			if(send(tmp.socks,buf,bytes,0)==SOCKET_ERROR){
				printf("发送数据失败！");
				exit(-1);
			}
			++it;
		}
        printf("Message from %s: %s\n",inet_ntoa(clientAddress.sin_addr),buf);
        
		
	}
}

void CreateServer(){
	
	short port=atoi("8080");
	
	sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock==INVALID_SOCKET){
		printf("Error");
	}
	struct sockaddr_in serverAddress;
    memset(&serverAddress,0,sizeof(sockaddr_in));
    serverAddress.sin_family=AF_INET;
    serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    //绑定
    if(bind(sock,(sockaddr*)&serverAddress,sizeof(serverAddress))==SOCKET_ERROR){
        printf("套接字绑定到端口失败！端口: %d/n",port);
        exit(-1);
    }
    //进入侦听状态
    if(listen(sock,SOMAXCONN)==SOCKET_ERROR){
        printf("侦听失败！");
        exit(-1);
    }
    printf("Server %d is listening......\n",port);
    SOCKET clientSocket;//用来和客户端通信的套接字
    struct sockaddr_in clientAddress;//用来和客户端通信的套接字地址
    memset(&clientAddress,0,sizeof(clientAddress));
    int addrlen = sizeof(clientAddress);
    //接受连接
	while(1){
		if((clientSocket=accept(sock,(sockaddr*)&clientAddress,&addrlen))==INVALID_SOCKET){
			printf("接受客户端连接失败！");
			exit(-1);
		}else{
			
			vector<HANDLE>::iterator it=TSendArr.begin();
			for(int i=0;i<TSendArr.size();i++){
				
				HANDLE tmp=(HANDLE)*it;
				SuspendThread (tmp);
				++it;
			}

			HANDLE hSubThread = CreateThread(NULL, 0, ( LPTHREAD_START_ROUTINE )ReceieveSockets,(LPVOID)clientSocket, 0, NULL);
			ThreadSock ts;
			ts.socks=clientSocket;
			ts.thread=hSubThread;
			ThreadArr.push_back(ts);
			it=TSendArr.begin();
			for(int i=0;i<TSendArr.size();i++){
				
				HANDLE tmp=*it;
				ResumeThread (tmp);
				++it;
			}

		}
		printf("Accept connection from %s %d \n",inet_ntoa(clientAddress.sin_addr),ThreadArr.size());
	}
    

}

void SendData(LPVOID param){
	while(1){
		if(ImgReady){
			ThreadImg *edges=(ThreadImg*)param;
			char *pt=(char*)malloc(edges->width*edges->height);
			memcpy(pt,edges->pointer,edges->width*edges->height);
			int picsizeh=edges->height;
			int picsizew=edges->width;
			vector<ThreadSock> Tmp=ThreadArr;

			vector<ThreadSock>::iterator it=Tmp.begin();
			for(int i=0;i<Tmp.size();i++){
				
				ThreadSock tmp=*it;
				const char *tmpc=pt;
				if(send(tmp.socks,tmpc,picsizeh*picsizew,0)==SOCKET_ERROR){
					printf("发送数据失败！");
					exit(-1);
				}
				++it;
			}
			free(pt);
			HANDLE handle=GetCurrentThread();
			vector<HANDLE>::iterator its=TSendArr.begin();
			for (int i=0;i<TSendArr.size();i++){
				HANDLE tmp=*its;
				if(handle==tmp){
					TSendArr.erase(its);
					return;
				}
				++its;
			}	
			ImgReady=0;
		}
	}
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	CvCapture*  capture;
	IplImage*   img0;
	IplImage*   img1;
	capture = cvCaptureFromCAM(0);
	
	img0 = cvQueryFrame(capture);
    img1 = cvCreateImage(cvGetSize(img0), IPL_DEPTH_8U, 1);
 
    cvZero(img1);

    cvNamedWindow("stream_server", CV_WINDOW_AUTOSIZE);

	 WSADATA wsa;
    //初始化套接字DLL
    if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){
        printf("套接字初始化失败!");
        exit(-1);
    }
	ImgReady=0;
	hSubThread = CreateThread(NULL, 0, ( LPTHREAD_START_ROUTINE )CreateServer,(LPVOID)NULL, 0, NULL);
	ThreadImg img;
	HANDLE SenThread = CreateThread(NULL, 0, ( LPTHREAD_START_ROUTINE )SendData,(LPVOID)&img, 0, NULL);
	
	int i=0;
    for(;;)
    {
        img0 = cvQueryFrame(capture);
        if (!img0) break;
 
        img0->origin = 0;
        cvFlip(img0, img0, 1);

        cvCvtColor(img0, img1, CV_BGR2GRAY);
		
		img.pointer=reinterpret_cast<char*>(img1->imageData);
		img.width=img1->width;
		img.height=img1->height;
		ImgReady=1;

        cvShowImage("stream_server", img0);
        if(waitKey(30) >= 0) break;
		
    }
	WSACleanup();
	return 0;
}

