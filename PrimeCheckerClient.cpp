#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstring>
#include <winsock2.h>

#define BUFFERSIZE 1024
#define LIMIT "10000000"
#define FLOOR "1"
#define THREAD_COUNT "1"

using namespace std;

int main() {
  char buffer[BUFFERSIZE];

  //Get Addresses and ports
    string serverip;
    int serverport;
    cout << "Server IP Address(Default=127.0.0.1): ";
    cin.getline(buffer, BUFFERSIZE);
    if(strcmp(buffer, "") == 0)
      serverip = string("127.0.0.1");
    else
      serverip = string(buffer);

    cout << "Server Port(Default=8080): ";
    cin.getline(buffer, BUFFERSIZE);
    if(strcmp(buffer, "") == 0)
      serverport = 8080;
    else
      serverport = atoi(buffer);

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
  serverAddress.sin_port = htons(serverport); 
  serverAddress.sin_addr.s_addr = inet_addr(serverip.c_str()); 
    
  // sending connection request 
  connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)); 

  // user input for lower_bound
  std::cout << "Enter lower bound (default=1): ";
  std::cin.getline(buffer, BUFFERSIZE);
  if(strcmp(buffer, "") == 0)
    send(clientSocket, FLOOR, sizeof(buffer), 0);
  else
    send(clientSocket, buffer, sizeof(buffer), 0);
  
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

  // start timer
  auto start_time = std::chrono::high_resolution_clock::now();

  //Receive results from server
  recv(clientSocket, buffer, sizeof(buffer), 0);

  // stop timer
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  // print output
  std::cout << "\nTotal Time Taken: " << duration << " ms" << std::endl;
  std::cout << buffer << " primes were found." << std::endl;
	
  //closing socket 
  closesocket(clientSocket);  

  return 0;
}

