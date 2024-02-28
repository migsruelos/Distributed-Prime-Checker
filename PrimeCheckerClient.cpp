#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <cstring>
#include <winsock2.h>

#define BUFFERSIZE 1024
#define LIMIT "10000000"
#define THREAD_COUNT "1"

std::vector<int> primes;
std::mutex primes_mutex;


int main() {
  char buffer[BUFFERSIZE];

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
  
  //creating socket 
  SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  // specifying address 
  sockaddr_in serverAddress; 
  serverAddress.sin_family = AF_INET; 
  serverAddress.sin_port = htons(8080); 
  serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    
  // sending connection request 
  connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); 

  // user input for upper_bound
  std::cout << "Enter upper bound (default=10000000): ";
  std::cin.getline(buffer, BUFFERSIZE);
  if(strcmp(buffer, "") == 0)
    send(clientSocket, LIMIT, sizeof(buffer), 0);
  else
    send(clientSocket, buffer, sizeof(buffer), 0);     

  // user input for num_threads
  std::cout << "Enter the number of threads (default=1): ";
  std::cin.getline(buffer, BUFFERSIZE);
  if(strcmp(buffer, "") == 0)
    send(clientSocket, THREAD_COUNT, sizeof(buffer), 0);
  else
    send(clientSocket, buffer, sizeof(buffer), 0);   
	
  //closing socket 
  closesocket(clientSocket);  

  return 0;
}

