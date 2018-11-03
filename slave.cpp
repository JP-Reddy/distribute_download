#include<iostream>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include <cstring>


int main()
{
	int sock0,connfd;
	struct sockaddr_in addr,client;
	socklen_t len;
	sock0=socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9515);
	addr.sin_addr.s_addr=INADDR_ANY;
	bind(sock0, (struct sockaddr *) &addr,sizeof(addr));
	listen(sock0,5);
	for(;;)
	{
		
		len=sizeof(client);
		connfd=accept(sock0,(struct sockaddr *) &client,&len);
		// write(connfd,"HELL",4);
		char *buf =new char[12];
		char *buf_temp=buf;
		read(connfd,buf,12);

/*		int *url_length=(int *)buf;
		std::cout<<*url_length<<std::endl;
		buf=buf+4;			
		int *range_length=(int *)buf;
		std::cout<<*range_length<<std::endl;
		buf=buf+4;			
		int *index_length=(int *)buf;
		std::cout<<*index_length<<std::endl;
*/
		int *url_length=new int;
		int *range_length=new int;
		int *index_length=new int;
		memcpy(url_length,buf,4);
		buf=buf+4;			
		memcpy(range_length,buf,4);
		buf=buf+4;			
		memcpy(index_length,buf,4);

		int total_length=*url_length+*range_length+*index_length;

		int n;
		char *new_buf=new char[total_length];
		char *new_buf_temp=new_buf;
		int total = 0;
		unsigned char temp1[200];
		while((n=read(connfd,temp1,200))!=0){
			std::memcpy(new_buf+total, temp1, n);
			total += n;
		}

		std::cout<<total<<std::endl;
		char *url=new char[*url_length+1];
		char *range=new char[*range_length+1];
		char *index=new char[*index_length+1];
	
		url[*url_length]='\0';
		range[*range_length]='\0';
		index[*index_length]='\0';

		memcpy(url,new_buf,*url_length);
		new_buf+=*url_length;
		memcpy(range,new_buf,*range_length);
		new_buf+=*range_length;
		memcpy(index,new_buf,*index_length);

/*		url=(char *)new_buf;
		new_buf=new_buf+(*url_length)+1;
		range=(char *)new_buf;		
		new_buf=new_buf+(*range_length)+1;
		index=(char *)new_buf;		

*/		std::cout<<url<<std::endl;
		std::cout<<range<<std::endl;
		std::cout<<index<<std::endl;
		// int *
		delete[] buf_temp;
		delete[] new_buf_temp;

		close(connfd);
	}
	close(sock0);
}


// http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv0-10000