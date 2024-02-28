#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstring>
#include <winsock2.h> 
#include <winsock.h>
  
using namespace std; 

#define LIMIT 10000000

std::vector<int> primes;
std::mutex primes_mutex;


int main() 
{ 
    // creating socket 
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // specifying the address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
  
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // listening to the assigned socket 
    listen(serverSocket, 5); 
  
    // accepting connection request 
    int clientSocket = accept(serverSocket, nullptr, nullptr); 
  
    // recieving data 

    int result_bound;
    int result_threads;
    int value;
    int result;
    char* buffer = (char*)&value;   // for integer values receiving

    // receive upper limit
    result_bound = recv(clientSocket, buffer, sizeof(buffer), 0); 
    
    // receive threads count
    result_threads = recv(clientSocket, buffer, sizeof(buffer), 0); 

    // check if received
    cout << "Upper Bound from client: " << result_bound << endl; 
    cout << "Threads from client: " << result_threads << endl; 

    // closing the socket. 
     closesocket(serverSocket); 
  
    return 0; 
}