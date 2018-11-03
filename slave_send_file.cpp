#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include <cstring>
#include <string.h>
#include<arpa/inet.h>
#include<string>
#include <fcntl.h>
#define LENGTH 1024*1024

using std::cout;
using std::endl;

int main(int argc,char* argv[])
{
	char * MASTER_ADDR="127.0.0.1";
	int sockfd,n;
	char buf[100];
	struct sockaddr_in addr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9516);
	inet_pton(AF_INET,MASTER_ADDR,&addr.sin_addr.s_addr);
	connect(sockfd,(struct sockaddr *)&addr,sizeof(addr));
	cout<<"Connected"<<std::endl;
	char* fs_name = "/home/ubuntu/Downloads/spark-2.3.1-bin-hadoop2.7.tgz";
	cout<<"[Client] Sending "<<fs_name<<" to the Server..."<<endl ;
	memset(buf,0,sizeof(buf));

	char *sdbuf=new char[LENGTH]; 
	// FILE *fs = fopen(fs_name, "r");
	int fd = open(fs_name, 'r');
	if(fd < 0)
	{
	    cout<<"ERROR: File "<<fs_name <<"not found.\n" ;
		exit(1);
	}
	cout<<"Writing file\n";
	memset(sdbuf,0,LENGTH);
	int fs_block_sz; 
	//int success = 0;
	// while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))!=0)
	while((fs_block_sz = read(fd,sdbuf, LENGTH))!=0)
    {
        if((n=write(sockfd, sdbuf, fs_block_sz) )< 0)
        {
            cout<<"ERROR: Failed to send file "<< fs_name<<".\n" ;
            break;
        }
        printf("%d bytes sent\n",n);

        memset(sdbuf,0, LENGTH);
    }
    cout<<"Ok File "<< fs_name<<" from Client was Sent!\n" ;
    //success = 1;
    // fclose(fs);
    close(fd);
    close(sockfd);
}