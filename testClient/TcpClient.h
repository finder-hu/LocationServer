//HML code
#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include "define.h"
#include <iostream>
#include <fstream> 
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <vector>
#include <queue>
#include <unistd.h>     //fork()函数头文件
#include <stdlib.h>  
#include <fcntl.h>  
#include <limits.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <stdio.h>  
#include <string.h> 
#include <pthread.h>    //线程头文件
#include "DBConnpool.h"
#include <time.h>
#include <fstream>
#include <cstring>
#include <stdio.h>



using namespace std;
using namespace Json;


//extern ConnPool *pool;
#define TESTDURATION 5  	//设置每个客户端运行的时间,单位秒
#define CLIENTNUM 10	   	//设置需要模拟的客户端数目
#define PERIODSENDMSGSEC 1	//设置报文发送的时间间隔-秒
#define PERIODSENDMSGUSEC 0	//设置报文发送的时间间隔-毫秒
#define LOCATION 0		//1 for location, 0 for guide
#define SENDTIMES ((int)(TESTDURATION/(PERIODSENDMSGSEC+PERIODSENDMSGUSEC/1000000.0)))

#define MAX_LINE (SENDTIMES*20+1)
typedef pair<int, int> coordinate_t;

class TcpClient {
private:
	vector<coordinate_t> coordinate;					//用于收集返回的定位坐标
	//vector<float> responsetime;
     vector<long> sendtime;
     vector<long> recvtime;
     vector<int>respongTime;
	int clientsock;										//TcpClient的套接字
	//char recvdata[MAX_LINE];
	string sendMsg;										//TcpClient发送的消息
	pthread_t tid;										//发送线程的线程ID
	pthread_mutex_t mutex;								//设置statue的互斥锁
	struct timeval period;
	bool statue;
										//发送线程用于通知主线程结束发送，主线程回收发送线程的存储资源
  
public:
	TcpClient(){};
	~TcpClient(){
		cout<<"TcpClient "<<getpid()<<" sock close!!!"<<endl;
		close(clientsock);
		//exit(0);
	}
	static void *PerTransFun(void *object);				//发送线程的入口函数
	coordinate_t recvdata(void);						//接收返回的报文处理函数
	void senddata(string &msg);							//发送处理函数
	void clientMsg(void);								//设置TcpClient发送的消息-即string sendMsg
	void clientInit(sockaddr_in addr);					//TcpClient的主线程
	void clientConnect(sockaddr_in addr, int len);		//TcpClient连接服务器
	void setstatus(bool value);							//设置状态字-stauts
	bool getstatus();
	int  WriteByPipe();
	//char* MyWrite(const string &str) const;

	ConnPool *pool;
	// int pipeWrite;	//获取状态字-stauts
	/*class NewDataArray
	{
	public:
		explicit NewDataArray(int len)
		{ 
			data = new char[len];
			memset(data,'\0',len);
		}
		char* GetArrayPtr(void){ return data;}
		~NewDataArray(){ delete[] data;}
	private:
		char* data;
	};*/
								
};


#endif
