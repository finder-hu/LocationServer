//HML code
#include "TcpClient.h"


const char *database="project";

coordinate_t TcpClient::recvdata() {
	cout<<"test 20151009"<<endl;

	Value ResponseData;
	Reader reader;
	//coordinate_t recvcoordinate;
	char recvdata[MAX_LINE];
	if (0 != (recv(this->clientsock, recvdata, MAX_LINE, 0))) {

		time_t recvtime_t=time(NULL);
		cout<<"Client("<<getpid()<<") recv at the time "<<ctime(&recvtime_t)<<endl;
		timeval tv;
		gettimeofday(&tv,NULL);
		long recvtime_tt=(long)(tv.tv_sec*1000000+((float)tv.tv_usec));
		recvtime.push_back(recvtime_tt);

	   /* string recvmsg = recvdata;
	    int flag;
		int typecode=ResponseData["typecode"].asInt();
		#if LOCATION
            if(typecode==1040)
            	flag=1;
        #else
            if(typecode==1010)
            	flag=1;
        #endif
		if (reader.parse(recvmsg, ResponseData)&&flag) {
			
		    #if LOCATION
		    
		    int x_coordinate = ResponseData["x"].asInt();
			int y_coordinate = ResponseData["y"].asInt();
			recvcoordinate = make_pair(x_coordinate, y_coordinate);
			#endif
			//cout << "x:" << x_coordinate << " y:" << y_coordinate << endl;
		}*/
	}
	//return recvcoordinate;
}


int TcpClient::WriteByPipe()
{
	cout<<"Write data"<<endl;
	char sendbuf[MAX_LINE];
	memset(sendbuf,0,MAX_LINE);
	//char* sendbuf;
	vector<long>::iterator iter1;
	vector<long>::iterator iter2;
	int i=0;
	for(iter1=sendtime.begin(),iter2=recvtime.begin();iter1!=sendtime.end();iter1++,i++,iter2++)
	{
		
		sprintf(sendbuf+i*20,"%20ld\n",*iter2-*iter1);

	}
   // cout<<"sendbuf "<<sendbuf<<endl;
    //sprintf(sendbuf+SENDTIMES*20,"\n");
    sprintf(sendbuf+SENDTIMES*20,"\n");
    //cout<<"1hml"<<endl;
    int pipe_fd = open("data/guide",O_WRONLY); 
    write(pipe_fd,sendbuf,sizeof(sendbuf));
    cout<<"sendbuf="<<sendbuf<<endl;
    //write(this->pipeWrite,sendbuf,sizeof(sendbuf));
   // cout<<"hml"<<endl;
   // cout<<"Client "<<getpid()<<" write data end"<<endl;
    close(pipe_fd);
    return 0;

}


void TcpClient::senddata(string &msg) {
	short int datalen = msg.length();
	datalen = htons(datalen);
	//cout<<"datalen is "<<datalen<<endl;
	send(this->clientsock, (char*) &datalen, 2, 0);
	send(this->clientsock, msg.c_str(), msg.length(), 0);
}



void TcpClient::clientInit(sockaddr_in addr) {
	//初始化互斥锁
	pthread_mutex_init(&(this->mutex), NULL);
	//初始后状态字
	this->setstatus(false);
   // cout<<"clientInit"<<endl;
	this->clientsock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->clientsock == -1) {
		cout << "create client error" << endl;
	}

	connect(this->clientsock, (sockaddr *) &addr, sizeof(sockaddr_in));

	//ofstream out;
	//out.open("datarecord.txt",ofstream::out|ofstream::app);

	//创建需要发送的消息sendMsg
	//clientMsg();

	//创建周期性发数据的线程
	pthread_create(&(this->tid), NULL, PerTransFun,(void* )this);

	//创建select来接收到来的包，并调用函数进行分析
	fd_set rset;
	FD_ZERO(&rset);
    int numRecv=0;

	while (1) {
		FD_SET(this->clientsock, &rset);
		//cout<<"This is befor select"<<endl;
		
		select((this->clientsock) + 1, &rset, NULL, NULL, NULL);
		//cout<<"THis is after select"<<endl;
        numRecv++;
        if (numRecv && FD_ISSET(this->clientsock,&rset)) {
			recvdata();
			
			
		} 
		cout<<"numRecv="<<numRecv<<endl;
		if(numRecv==SENDTIMES)
		{
			break;
		}

	}
    cout<<"recv is end"<<endl;
	pthread_join(this->tid, NULL);
	WriteByPipe();
	
}

/*char* TcpClient::MyWrite(const string &str) const
{
    NewDataArray newDataArray(2+str.size()+1);
    char* data = newDataArray.GetArrayPtr();
    short int len = str.size();
    memcpy(data,&len,2);
    memcpy(data+2,str.c_str(),len);
    return data;
    /* if(send(sock_fd,data,len+2,0) != len+2)  //加长度首部
    //if(send(sock_fd,str.c_str(),str.size(),0) != str.size())
    {
        debug("error occur in sending data");
    }
    debug("sended data:%s,",str.c_str());
}*/

void TcpClient::clientMsg() {
   	Value RequestLocation;
   	Value Position;
	FastWriter writer;
	//cout<<"clientMsg"<<endl;
    srand((int)time(NULL));
    
   // cout<<"clientMsg end"<<endl;
    Connection *conn = pool->GetConnection();
    conn->setSchema(database);
    PreparedStatement *pstmt;
	
  #if LOCATION

    RequestLocation["typecode"] = 1040;



    string selectsql1 = "SELECT * FROM FingerPrint WHERE apid=?";
    pstmt = conn->prepareStatement(selectsql1);
  
    int i=rand()%84+1;
    pstmt->setInt(1, i);
    string rssi;
    ResultSet *rs= pstmt->executeQuery();
    if(rs->next())
    {
  

        rssi=rs->getString(6);
      //  cout<<rssi<<endl;
    }
     RequestLocation["rssi"] =rssi;
     
  
    #else
    RequestLocation["typecode"] = 1010;
    string selectsql1 = "SELECT * FROM node WHERE nodeId=?";
    pstmt = conn->prepareStatement(selectsql1);
    int k=rand()%107;
   // cout<<"k="<<k<<endl;
    pstmt->setInt(1, k);
    ResultSet *rs= pstmt->executeQuery();
    int  x,y;
    if(rs->next())
    {
  

        
        x=rs->getInt(2);
        Position["x"]=x;
        y=rs->getInt(3);
        Position["y"]=y;
      //  cout<<rssi<<endl;
    }


    RequestLocation["sour"] =Position;

    int j=rand()%107;
    pstmt->setInt(1, j);
    rs= pstmt->executeQuery();
    
    if(rs->next())
    {
  

        x=rs->getInt(2);
        Position["x"]=x;
        y=rs->getInt(3);
        Position["y"]=y;
      //  cout<<rssi<<endl;
    }

   // cout<<"j="<<j<<endl;
     RequestLocation["dest"]=Position;
   #endif
     string str=writer.write(RequestLocation);
     this->sendMsg=str;
     cout<<"sendMsg= "<<sendMsg<<endl;
     pool->ReleaseConnection(conn);
     pool->DestroyConnPool();

  
}


void TcpClient::setstatus(bool value) {
	pthread_mutex_trylock(&(this->mutex));
	this->statue = value;
	pthread_mutex_unlock(&(this->mutex));
}



bool TcpClient::getstatus() {
	pthread_mutex_trylock(&(this->mutex));
	bool a=statue;
	pthread_mutex_unlock(&(this->mutex));
	return a;
}



void *TcpClient::PerTransFun(void *object) {


   // cout<<"send data"<<endl;
	TcpClient *pt = (TcpClient *) object;
	pt->setstatus(true);
	//time_t startTime = time(NULL);
	//time_t endTime = time(NULL);
    // int num_time=0;
	//cout << asctime(localtime(&endTime));
    int numSend=0;
    int numTime=SENDTIMES;
    //cout<<" SENDTIMES="<<SENDTIMES<<endl;
	while (numTime) {
		pt->period.tv_sec = PERIODSENDMSGSEC;
		pt->period.tv_usec = PERIODSENDMSGUSEC;
		//cout<<"period.tv_sec"<<pt->period.tv_sec<<endl;
        //cout<<"numSend before select is "<<numSend<<endl;
		int status = select(0, NULL, NULL, NULL, &(pt->period));

		//cout<<"This is after select"<<endl;
		time_t sendtime_t=time(NULL);
			cout << "Client(" << getpid() << "):Packet send begin at "
			<< asctime(localtime(&sendtime_t))<<endl;

        if (0 == status) {
		    
		    timeval tv;
			gettimeofday(&tv,NULL);
			long sendtime_tt=(long)(tv.tv_sec*1000000+((float)tv.tv_usec));
			pt->sendtime.push_back(sendtime_tt);
			pt->clientMsg();
	        pt->senddata(pt->sendMsg);
	        numSend++;
	        cout<<"numSend="<<numSend<<endl;
        }

	    numTime--;
	}
	pt->setstatus(false);
    cout<<"This pthread is end"<<endl;
	pthread_exit(0);
}
