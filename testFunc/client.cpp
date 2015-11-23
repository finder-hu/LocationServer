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
	location["rssi"] = "e4:d3:32:db:25:fe,-64;38:83:45:96:c7:6c,-67;e4:d3:32:db:31:c2,-62;44:2b:03:8b:80:c8,-71;e4:d3:32:db:27:04,-66;e4:d3:32:db:26:3c,-77;e4:d3:32:db:26:fc,-77;30:49:3b:09:6b:49,-61;30:49:3b:09:6b:4b,-79;30:49:3b:09:68:27,-77;e4:d3:32:85:87:30,-75;5c:63:bf:37:27:6c,-78;e4:d3:32:db:26:f2,-91;20:dc:e6:6d:13:0e,-89;88:dc:96:1b:87:1c,-84;ec:26:ca:f4:b3:82,-84;14:75:90:58:fa:ec,-91;00:71:cc:94:06:3d,-91;30:49:3b:09:68:25,-91;ec:88:8f:63:63:88,-92;38:83:45:47:51:84,-87;e4:d3:32:e3:fc:38,-86";
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
