#include<iostream>
//#include<curl/curl.h>
using namespace std;
int main(){ 
    int pos;
	string s = "https://www.jp.com/url/scotttiger.jpg";
	string delimiter = "/";
	string token = s.substr(0, s.find(delimiter));
    size_t last = 0; 
    size_t next = 0; 
    while ((next = s.find(delimiter, last)) != string::npos) {
    	cout << s.substr(last, next-last) << endl; 
    	last = next + 1; 
    } 
    cout << s.substr(last) << endl;
    string filename=s.substr(last)+"-"inet_ntop()+".part"+index;
	return 0;

}

