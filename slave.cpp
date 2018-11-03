//Socket headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//C++ headers. 
#include <string>
#include <iostream>
#include <curl/curl.h>

//C headers. 
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <fcntl.h>

using std::cout;
using std::endl;

#define LENGTH 1024*1024

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

std::string get_file_name(char * url, struct sockaddr_in client, char * index)
{
	int pos;
	char client_ip[40];
	std::string s(url);
	std::string delimiter = "/";
	std::string token = s.substr(0, s.find(delimiter));
	size_t last = 0; 
	size_t next = 0; 
	while ((next = s.find(delimiter, last)) != std::string::npos) {
		// cout << s.substr(last, next-last) << endl; 
		last = next + 1; 
	} 
	cout << "substring:"<<s.substr(last) << endl;
	inet_ntop(AF_INET,(void*)&client.sin_addr,client_ip,40);
	std::string client_ip_string(client_ip);
	cout<<"client ip string :"<<client_ip_string<<endl;
	std::string index_string(index);
	cout<<"index string"<<index<<endl;
	std::string filename= client_ip_string +".part"+index_string + +"-"+s.substr(last);
	// cout<<"filean:"<<filename<<endl;
	return filename;
}

std::string download_file(char * url, char * range, char * index, struct sockaddr_in client)
{
	CURL *curl;
	FILE *fw,*fr;
	CURLcode res;
	int readfd,connfd_read,n;
	std::string outfilename_string = get_file_name(url, client,index);
	const char * outfilename = outfilename_string.c_str();
	cout<<"outfilename:"<<outfilename<<endl;
	cout<<range<<endl;
	

	curl = curl_easy_init();
	 
	if (curl) 
	{
		fw = fopen(outfilename,"wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 100); 
		curl_easy_setopt(curl, CURLOPT_RANGE, range);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fw);
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		cout<<curl_easy_strerror(res)<<endl;
		fclose(fw);
	}
	cout<<"Done downloading."<<endl;
	return outfilename_string;
}

void send_file(int writefd,std::string fs_name){
	char *sdbuf=new char[LENGTH]; 
	int fd = open(fs_name.c_str(), 'r');
	int n;
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
	int total_file_size = 0;
	while((fs_block_sz = read(fd,sdbuf, LENGTH))!=0)
	{
		if((n=write(writefd, sdbuf, fs_block_sz) )< 0)
		{
			cout<<"ERROR: Failed to send file "<< fs_name<<".\n" ;
			break;
		}
		total_file_size += n;
		// printf("%d bytes sent\n",n);

		memset(sdbuf,0, LENGTH);
	}
	cout<<"File size sent: "<<total_file_size<<endl;
	cout<<"Ok File "<< fs_name<<" from Client was Sent!\n" ;

}

int main()
{
	// char * url = "http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv";
	// char * range = "0-100";
	// char * index = "0";
	// download_file(url, range, index, client);

	// Descriptors to receive the url details
	int readfd,connfd_read;
	struct sockaddr_in addr_read,client_read;

	// Descriptors to send the file back to the master
	int writefd, connfd_write;
	struct sockaddr_in addr_write,client_write;

	socklen_t len;
	
	readfd=socket(AF_INET,SOCK_STREAM,0);
	if(readfd < 0)
	{
		cout<<"[SLAVE] "<<"socket created."<<endl;
	}
	addr_read.sin_family=AF_INET;
	addr_read.sin_port=htons(9515);
	addr_read.sin_addr.s_addr=INADDR_ANY;
	bind(readfd, (struct sockaddr *) &addr_read,sizeof(addr_read));
	cout<<"[SLAVE] "<<"Bind successfull for reading on 9515\n"<<endl;
	listen(readfd,5);

	//Socket for writing on 9516
	writefd=socket(AF_INET,SOCK_STREAM,0);
	if(writefd < 0)
	{
		cout<<"[SLAVE] "<<"socket created."<<endl;
	}

	addr_write.sin_family=AF_INET;
	addr_write.sin_port=htons(9516);
	addr_write.sin_addr.s_addr=INADDR_ANY;
	bind(writefd, (struct sockaddr *) &addr_write,sizeof(addr_write));
	cout<<"[SLAVE] "<<"Bind successfull for write on 9516\n"<<endl;
	listen(writefd,5);

	for(;;)
	{
		// Accept connection to read on 9515
		len=sizeof(client_read);
		connfd_read=accept(readfd,(struct sockaddr *) &client_read,&len);
		cout<<"[SLAVE] "<<"Connection accepted on 9515"<<endl;
		char *buf =new char[12];
		char *buf_temp=buf;
		read(connfd_read,buf,12);

		// Accept connection to write on 9516
		len=sizeof(client_write);
		connfd_write=accept(writefd,(struct sockaddr *) &client_write,&len);
		cout<<"[SLAVE] "<<"Connection accepted on 9516"<<endl;

/*		int *url_length=(int *)buf;
		std::cout<<"[SLAVE] "<<*url_length<<std::endl;
		buf=buf+4;			
		int *range_length=(int *)buf;
		std::cout<<"[SLAVE] "<<*range_length<<std::endl;
		buf=buf+4;			
		int *index_length=(int *)buf;
		std::cout<<"[SLAVE] "<<*index_length<<std::endl;
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
		while((n=read(connfd_read,temp1,200))!=0){
			std::memcpy(new_buf+total, temp1, n);
			total += n;
		}

		std::cout<<"[SLAVE] "<<total<<std::endl;
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

*/		std::cout<<"[SLAVE] "<<url<<std::endl;
		std::cout<<"[SLAVE] "<<range<<std::endl;
		std::cout<<"[SLAVE] "<<index<<std::endl;
		// int *
		delete[] buf_temp;
		delete[] new_buf_temp;

		close(connfd_read);
		//Downloading the actual file.
		std::string file_name = download_file(url, range, index, client_read);
		// std::string file_name="/home/ubuntu/JP/hackpes/10.16.160.76.part0-bcompare-4.2.8.23479_amd64.deb";
		std::cout<<"[SLAVE] Sending file back to master\n";
		send_file(connfd_write,file_name);
		close(connfd_write);
	}
	close(readfd);
	close(writefd);

	
}


