
//Socket headers.
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

using std::cout;
using std::endl;

typedef struct download_url
{
    std::string url;
    std::string index;
    std::string range;
}download_url;

//1 - ip address, 2 - url, 3 - range, 4 - index (for argv)
int main(int argc, char * argv[])
{
    cout<<"args 0:"<<argv[0]<<endl;
    int writefd, n;
    struct sockaddr_in servaddr;     

    if( (writefd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"Not able to open socket."<<std::endl;
        exit(0);
    }
    else
    {
        std::cout<<"Socket created.";
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9515);
    inet_pton(AF_INET, argv[0], &servaddr.sin_addr);    
	//Sending the masters ip address. 
    
    if( (connect(writefd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        cout<<"connect error"<<endl;
        exit(0);
    }
    else
    {
        cout<<"Connected"<<endl;
    }

    download_url url_struct;
    // url_struct.url = "http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv";
    // url_struct.range = "0-1000";
    // url_struct.index = "0";

    url_struct.url = std::string(argv[1]);
    url_struct.range = std::string(argv[2]);
    url_struct.index = std::string(argv[3]);

    int size = url_struct.url.length() + url_struct.index.length() + url_struct.range.length(),bytes;
    int url_length = url_struct.url.length();
    int range_length = url_struct.range.length();
    int index_length = url_struct.index.length();
    
    char * buffer = new char[size+1];
    memcpy(buffer, (void *)&url_length, 4);    
    memcpy(buffer+4, (void *)&range_length, 4);
    memcpy(buffer+8, (void *)&index_length,4);

    if((bytes = write(writefd, (void *)buffer,12)) < 12)
    {
        cout<<"Error writing.";
    }

    const char *url  = url_struct.url.c_str();
    const char *range = url_struct.range.c_str();
    const char *index = url_struct.index.c_str();
    
    memcpy(buffer, (void *)url, url_length);
    memcpy(buffer+url_length, (void *)range, range_length);
    memcpy(buffer+range_length+url_length, (void *)index, index_length);

    if((bytes = write(writefd, (void *)buffer,size)) < size)
    {
        cout<<"Error writing.";
    }
    else
    {
        cout<<"bytes written:"<<bytes<<endl;
    }
    delete[] buffer;
    close(writefd);  

    //Waiting for 

/*int checksum_size;
    char checksum[1000];
    if((checksum_size = read(readfd,checksum,sizeof(checksum)))<=0){
        printf("Checksum Error\n");
        exit(0);
    }*/

}
