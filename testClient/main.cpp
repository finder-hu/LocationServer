//HML code
#include "TcpClient.h"
#include "DBConnpool.h"

/**
 * 生成 N个客户端，每个客户端使用select监听套接字的接收数据，
 * 另有发送线程，每隔SendDuration s发送一次定位请求，持续发送的时间为TestDuration结束
 * 返回每次的定位请求相应时间，并作记录
 * 结束时候，各个客户端将自己的响应时间统一记录到RequestLocationResponseTime.txt中
 ****/
static int connec=0;

int main(void)                               //主进程主要为了创建clientnum个进程
{
	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = inet_addr(IP);
    
  
	int clientnum=0;
	while(clientnum<CLIENTNUM)
	{

		if(0==fork())
		{
			
			//cout<<"child's process ID:"<<getpid()<<endl;
			TcpClient *user=new TcpClient();
           // cout<<"GetInstance"<<endl;
			user->pool = ConnPool::GetInstance();
			user->clientInit(serveraddr);
			delete user;

			exit(0);
		}

		clientnum++;
		
	}
  //  cout<<"clientNum= "<<clientnum<<endl;
   // cout << "Client(" << getpid() << "): I am the main thread!" << endl;

    int ress = mkfifo("data/guide", 0777);  //create FIFO in comunication with process

    if(ress != 0)            
    {  
        fprintf(stderr, "Could not create fifo\n");   
        return 0;
    }
    else
    {
        cout<<"Create FIFO files success"<<endl;
    }  


    
    
    
    char buffer[PIPE_BUF+1];
    int res=0;
    int bytes_write=0;
    int bytes_read=0;
    memset(buffer,'\0',sizeof(buffer));

   // cout<<"HML"<<endl;
    int pipe_fd=-1;
    pipe_fd=open("data/guide",O_RDONLY);   //judge open are block ?
   // cout<<"HML123"<<endl;

    char *filename=(char*)malloc(100);
    time_t Record_t=time(NULL);
    #if  LOCATION
    sprintf(filename,"data/location_%d_%d_%5.3f_%s.txt" ,TESTDURATION,CLIENTNUM,//create the name of file
    ((float)PERIODSENDMSGSEC+PERIODSENDMSGUSEC/1000000),ctime(&Record_t));
    #else
    sprintf(filename,"data/guide_%d_%d_%5.3f_%s.txt" ,TESTDURATION,CLIENTNUM,
    ((float)PERIODSENDMSGSEC+PERIODSENDMSGUSEC/1000000),ctime(&Record_t));
    #endif


    int data_fd=open(filename,O_WRONLY|O_CREAT, 0644);//file fd that write data to file(linux )
    fstream outfile;                                  //filestream outfile that write data to file(C++ Stream)
    outfile.open(filename, fstream::app | fstream::out);
    
    cout<<outfile.is_open()<<endl;
    if(!outfile.is_open())
    {
    	cout<<"open flie failed"<<endl;
    }

    int resNum=0;
    int numByte=MAX_LINE*clientnum;
    int numRead=0;
    //cout<<"numByte="<<numByte<<endl;
    while(pipe_fd!=-1)
    {
        res = read(pipe_fd, buffer, MAX_LINE);
        resNum+=res;
       // cout<<"res="<<res<<endl;
        if(res>0)  
        {  
            //读取FIFO中的数据，并把它保存在文件filename文件中  
            bytes_write = write(data_fd, buffer, res);  //fd write data
           // outfile.write(buffer,res);       //filestream write data
            numRead++;
            cout<<"numRead="<<numRead<<endl;
        } 
       // if(res==0)
       //     break;
        //cout<<"resNum="<<resNum<<endl;

        if(resNum==numByte)                 //By judging recv data equal data that son process produce data 
           break;
    }
    
    
    cout<<"FIFO read is ready!"<<endl;    //clear other resource
    close(pipe_fd); 
    outfile.close();
    cout<<"hml is endl"<<endl;
	return 0;
}
