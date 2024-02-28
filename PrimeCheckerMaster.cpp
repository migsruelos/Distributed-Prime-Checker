#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstring>
#include <winsock2.h> 
  
using namespace std; 

#define LIMIT 10000000
#define BUFFERSIZE 1024

std::vector<int> primes;
std::mutex primes_mutex;


int main() 
{ 
    // Initialize WSA variables
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wsaerr != 0) {
        std::cout << "Startup error!" << std::endl;
        return 0;
    }
    
    // creating socket 
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
  
    // specifying the address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // listening to the assigned socket 
    listen(serverSocket, 5); 
  
    // accepting connection request 
    int clientSocket = accept(serverSocket, nullptr, nullptr); 
  
    // recieving data 

    int result_bound = -1;
    int result_threads = -1;
    char buffer[BUFFERSIZE];

    // receive upper limit
    recv(clientSocket, buffer, sizeof(buffer), 0);
    result_bound = atoi(buffer);
    
    // receive threads count
    recv(clientSocket, buffer, sizeof(buffer), 0);
    result_threads = atoi(buffer);

    // check if received
    cout << "Upper Bound from client: " << result_bound << endl; 
    cout << "Threads from client: " << result_threads << endl; 

    // closing the socket. 
    closesocket(serverSocket); 
  
    return 0; 
}