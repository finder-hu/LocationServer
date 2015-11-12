#include <iostream>
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>

using namespace std;
using namespace Json;

#define PORT 40010
#define IP  "192.168.1.142"
//#define IP  "192.168.1.108"

void senddata(string &msg, int client);
int main(void)
{
	int client;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if(client == -1)
	{
		cout<<"create client error"<<endl;
		return 0;
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);

	connect(client,(struct sockaddr*)&addr, sizeof(struct sockaddr));

	Value login,regist,changeinf,location,guide;
	FastWriter writer;

	regist["typecode"] = 9999; 
	//regist["typecode"] = 1;
	regist["username"] = "finder";
	regist["password"] = "123";
	//regist["email"] = "1397812937@qq.com";

	login["typecode"] = 200;
	login["username"] = "finder";
	login["password"] = "123";
	
	changeinf["typecode"] = 40;
	changeinf["username"] = "finder";
	changeinf["password"] = "456";

	location["typecode"] = 1040;
	location["rssi"] = "74:ea:3a:26:10:86,-92.0;20:dc:e6:6b:36:10,-86.0;30:49:3b:09:68:25,-67.14492753623189;50:bd:5f:06:72:84,-85.95;00:87:36:07:ef:4d,-79.70689655172414;30:49:3b:09:68:27,-52.971014492753625;d8:15:0d:38:5b:4e,-62.68115942028985;30:49:3b:09:6b:49,-78.08695652173913;30:49:3b:09:6a:45,-85.0925925925926;08:10:74:67:63:1a,-92.16666666666667;38:83:45:96:c7:6c,-64.4927536231884;44:2b:03:8b:80:c8,-55.82608695652174;38:83:45:47:51:84,-87.0754716981132;c8:3a:35:3f:4d:98,-84.32075471698113;74:ea:3a:2f:6d:0a,-64.7536231884058;e4:d3:32:e3:fc:38,-87.66666666666667;30:49:3b:09:6a:4f,-73.05084745762711;e4:d3:32:eb:f9:f2,-90.0;b0:48:7a:5d:f3:28,-35.57971014492754;5c:63:bf:37:27:6c,-75.08823529411765;e0:05:c5:b3:d3:9e,-88.26666666666667;30:49:3b:09:6a:4b,-87.85714285714286;78:a1:06:fd:41:84,-86.37037037037037;30:49:3b:09:6a:59,-89.54545454545455;20:dc:e6:88:5b:5e,-89.57142857142857;20:dc:e6:6d:13:0e,-77.61764705882354;00:23:cd:83:7d:a0,-90.0;30:49:3b:09:6b:4b,-85.91304347826087";
	Value temp;
	guide["typecode"] = 1010;
	// temp["x"] = 3966;
	// temp["y"] = 7193;
	temp["x"] = 542;
	temp["y"] = 337;
	temp["z"] = 1;
	guide["sour"] = temp;
	// temp["x"] = 3966;
	// temp["y"] = 5560;
	temp["x"] = 542;
	temp["y"] = 313;
	temp["z"] = 2;
	guide["dest"] = temp;

	string strregist = writer.write(regist);
	string strlogin = writer.write(login);
	string strchange = writer.write(changeinf);
	string strlocate = writer.write(location);
	string strguide = writer.write(guide);
	cout<<"1--regist   2--login   3--changeinf	4--location	  5--guide"<<endl;
	while(1)
	{
		cout<<"your choice:"<<endl;
		int i = 0;
		cin>>i;
		switch(i)
		{
			case 1:senddata(strregist,client); break;
			case 2:senddata(strlogin,client); break;
			case 3:senddata(strchange,client); break;
			case 4:senddata(strlocate,client); break;
			case 5:senddata(strguide,client);break;
			default:break;
		}
	}
	//close(client);		//close socket
	return 0;
}

void senddata(string &msg, int client)
{	
	short int datalen = msg.length();
	datalen = htons(datalen);
	send(client, (char*)&datalen,2, 0);
	send(client, msg.c_str(), msg.length(), 0);




	/*char* data = (char*)malloc(datalen);
	//short int len = htons(datalen);*************?*********
	*data = datalen;
	msg.copy(data+2,datalen,0);
	cout<<"send data:"<<(data+2)<<endl;
	cout<<"data length:"<<datalen<<endl;
	//cout<<"htons(datalen):"<<len<<endl;
	int sendlen = send(client,(char*)&datalen,2,0);
	cout<<"length of sended data:"<<sendlen<<endl;
	free(data);*/
}
/*void senddata(string &msg, int client)
{	
	short int datalen = msg.length();
	char* data = (char*)malloc(datalen+2);
	*data = htons(datalen);
	msg.copy(data+2,datalen,0);
	cout<<"send data:"<<(data+2)<<endl;
	//cout<<"htons(datalen):"<<htons(datalen)<<endl;
	cout<<"data length:"<<datalen<<endl;
	int sendlen = send(client,data,datalen+2,0);
	cout<<"length of sended data:"<<sendlen<<endl;
	free(data);
}*/
