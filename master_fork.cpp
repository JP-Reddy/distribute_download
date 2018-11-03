
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

#pragma pack(1)
typedef struct download_url
{
    std::string url;
    int index;
    std::string range;
}download_url;
#pragma pack(0)

int main(int argc, char * argv[])
{
    int writefd, n,readfd;
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
    inet_pton(AF_INET, "192.168.0.6", &servaddr.sin_addr);    
    //Sending the masters ip address. 
    
    if( (connect(writefd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        cout<<"connect error"<<endl;
        exit(0);
    }

    download_url url_struct;
    url_struct.url = "http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv";
    url_struct.index = 0;
    url_struct.range = "0-1000000";

    int size = url_struct.url.length() + sizeof(int) + url_struct.range.length();
    std::cout<<size<<std::endl;
    int bytes;
    if((bytes = write(writefd, (void *)&url_struct, size)) < size)
    {
        cout<<"Error writing.";
    }
    std::cout<<"Sent the url\n";
    char buf[500];
        if((bytes = read(writefd,buf, size)) < 0)
        {
            cout<<"Error reading.";
        }
        
    std::cout<<buf<<std::endl;

    close(writefd);  

/*    if( (readfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout<<"Not able to open socket."<<std::endl;
        exit(0);
    }
    else
    {
        std::cout<<"Socket created to get the file data.";
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9516);
    inet_pton(AF_INET, "192.168.0.6", &servaddr.sin_addr);    
	//Sending the masters ip address. 
    
    if( (connect(readfd, (sockaddr*)&servaddr, sizeof(servaddr))) < 0)
    {
        cout<<"Connect error for receiving"<<endl;
        exit(0);
    }
    
    // Receive checksum first before getting the actual file.
    int checksum_size;
    char checksum[1000];
    if((checksum_size = read(readfd,checksum,sizeof(checksum)))<=0){
        printf("Checksum Error\n");
        exit(0);
    }

    
*/
}
