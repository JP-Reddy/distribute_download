//Socket headers.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

//C++ headers. 
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

//C headers. 
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <curl/curl.h>
#include <stdlib.h>


#define WRITE_PORT 9515
#define READ_PORT 9516

typedef struct download_url
{
    std::string url;
    std::string index;
    std::string range;
}download_url;

struct user_data
{
    int size;
};



using std::cout;
using std::endl;
using std::cin;

size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *usr_data_temp)
{
    user_data *usr_data = (struct user_data *)usr_data_temp;
    
    std::istringstream iss(buffer);
    std::string s;
    while ( std::getline( iss, s, ' ' ) ) {
        if(s == "Content-Length:")
        {
            std::getline(iss, s, ' ');
            std::istringstream temp(s);
            temp>>(usr_data->size);
            // cout<<s<<endl;
            // cout<<buffer<<endl;
        }
        
    }    
    // cout<<buffer<<endl;
    return strlen(buffer);
}



int get_file_size(download_url url_struct)
{
    user_data usr_data;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL * curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_struct.url.c_str());
    curl_easy_setopt(curl_handle,CURLOPT_NOBODY ,1 );
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &usr_data);
    CURLcode res = curl_easy_perform(curl_handle);


    if(res != CURLE_OK)
    {
        cout<<curl_easy_strerror(res)<<endl;
    }
    
    return usr_data.size;
}

int main()
{
    // cout<<"hi"<<endl;
    download_url url_struct;
    
    // std::cout<<"Enter the download url:"<<std::endl;
    // std::cin>>url_struct.url;
    url_struct.url = "http://dl1.irani-dl.com/serial/The%20Legend%20of%20Korra/Season%201/The%20Legend%20of%20Korra-S01E01E02.720p.WEB-DL.x264(www.irani-dl.ir).mkv";

    int file_size = get_file_size(url_struct);
    cout<<file_size<<endl;
    std::vector< std::string > ip_address = {"10.16.160.74"};

    int num_slaves = ip_address.size();
    int size_per_chunk = file_size / num_slaves;

    for(int i = 0; i < num_slaves; i++)
    {
        std::string index = std::to_string(i);
        int start_num = i * size_per_chunk, end_num = std::min(start_num + size_per_chunk - 1, file_size);
        std::string start = std::to_string(start_num), end = std::to_string(end_num);
        std::string range = start+"-"+end;       
        //1 - ip address, 2 - url, 3 - range, 4 - index (for argv)
        execl("./master_fork", ip_address[i].c_str(), url_struct.url.c_str(),\
         range.c_str(),index.c_str(), (char *)0);

    }
    

    return 0;
}