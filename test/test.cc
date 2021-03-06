#include "Client.h"
#include <ctime>

const int REQ = 1000000;    // 1000，10000,100000,1000000

int main()
{
    Client client(9527, "127.0.0.1");
    client.Connect();
    int i = 0;
    //set
    clock_t startTime1 = clock();
    while(1) 
    {
        for(i = 0; i < REQ; ++i) 
        {
            char buf[10];
            snprintf(buf, 5, "req%d", i);
            std::string t = "set name ";
            t += string(buf);
            client.handleRequest(t);
           
        }
        if(i == REQ) break;
    }
    clock_t endTime1 = clock();

    //get    
    clock_t startTime2 = clock();
    while(1) 
    {
        for(i = 0; i < REQ; ++i) 
        {
            client.handleRequest("get name");       
        }
        if(i == REQ) break;
    }
    clock_t endTime2 = clock();

    std::cout << "set time costs: " << (double)(endTime1 - startTime1) / CLOCKS_PER_SEC << std::endl;
    std::cout << "get time costs: " << (double)(endTime2 - startTime2) / CLOCKS_PER_SEC << std::endl;
    return 0;
}

