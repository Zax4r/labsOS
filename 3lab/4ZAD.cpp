#include <curl/curl.h>
#include <iostream>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>
#include <fstream>
#include <fcntl.h>

using namespace std;
string log1 = "Logger.txt";

size_t write33(void* ptr, size_t size, size_t nmemb, FILE* file) {
    return fwrite(ptr, size, nmemb, file);
}

void download(string& path,int i,string& logger,sem_t* se)
{
sem_wait(se);
ofstream loger(logger,ios_base::app);
loger<<"Process "<< i << " opened loger"<< endl <<"PATH: "<<path<<endl;

string name="IMAGE"+to_string(i)+".jpg";

CURL* curl=curl_easy_init();

if (curl){
FILE* file=fopen(name.c_str(),"wb");

loger<<"Process "<< i << " create file for downloading"<< endl;

curl_easy_setopt(curl,CURLOPT_URL,path.c_str());
curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write33);
curl_easy_setopt(curl,CURLOPT_WRITEDATA,file);

CURLcode response = curl_easy_perform(curl);

if (response != CURLE_OK){
loger<<"Process "<< i << " Error downloading a file// "<< curl_easy_strerror(response)<<endl;
}

else
loger<<"Process "<< i << " successfully downloaded file"<< endl;

curl_easy_cleanup(curl);

fclose(file);
}
loger<<"Process "<< i << " close loger \n"<< endl;
loger.close();
sem_post(se);
}

int main()
{
    sem_t* sem_cons = sem_open("/sem_cons", O_CREAT | O_EXCL, 0666, 1);
    string url;
    cout<<"Enter URL of file to download: "<< endl;
    getline(cin,url);
    pid_t pid=fork();
    if (pid==0){
        download(url,1,log1,sem_cons);
        return 0;
    }
    pid_t pid2=fork();
    if (pid2==0){
        download(url,2,log1,sem_cons);
        return 0;
    }
    waitpid(pid,nullptr,0);
    waitpid(pid2,nullptr,0);
    sem_close(sem_cons);
    sem_unlink("/sem_cons");
    return 0;
}