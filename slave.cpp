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

using std::cout;
using std::endl;

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

void download_file(char * url, char * range, char * index, struct sockaddr_in client)
{
	CURL *curl;
    FILE *fw,*fr;
    CURLcode res;
	int readfd,connfd,n;
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
}


int main()
{
	// char * url = "http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv";
	// char * range = "0-100";
	// char * index = "0";
	// download_file(url, range, index, client);

	int readfd,connfd;
	struct sockaddr_in addr,client;
	socklen_t len;
	
	readfd=socket(AF_INET,SOCK_STREAM,0);
	if(readfd < 0)
	{
		cout<<"socket created."<<endl;
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9515);
	addr.sin_addr.s_addr=INADDR_ANY;
	bind(readfd, (struct sockaddr *) &addr,sizeof(addr));
	cout<<"bind successfull"<<endl;
	listen(readfd,5);
	for(;;)
	{
		
		len=sizeof(client);
		connfd=accept(readfd,(struct sockaddr *) &client,&len);
		cout<<"connecteion accepted"<<endl;
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
		//Downloading the actual file.
		download_file(url, range, index, client);
	}
	close(readfd);

	
}


// http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv0-10000