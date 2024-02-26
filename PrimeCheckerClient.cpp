#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <cstring>
#include <winsock2.h> 
#include "winsock.h" 
#include <unistd.h> 

#define LIMIT 10000000

std::vector<int> primes;
std::mutex primes_mutex;


int main() {
  int upper_bound;
  int num_threads;
  
  // creating socket 
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0); 
  
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    
    // sending connection request 
    connect(clientSocket, (struct sockaddr*)&serverAddress, 
        sizeof(serverAddress)); 

  // user input for upper_bound
  std::cout << "Enter upper bound (default=LIMIT): ";
  std::string input;
  std::getline(std::cin, input);

  // Check if the user provided input for upper_bound
  if (!input.empty()) {
    try {
      upper_bound = std::stoi(input);
    } catch (std::invalid_argument&) {
      std::cerr << "Invalid input. Using default LIMIT." << std::endl;
      upper_bound = LIMIT;
    }
  } else {
    upper_bound = LIMIT;  
  }
  
  // sending data 
 //  const char* message = "Hello, server!"; 
    send(clientSocket, reinterpret_cast<const char*>(&upper_bound), sizeof(upper_bound), 0); 

  // user input for num_threads
  std::cout << "Enter the number of threads (default=1): ";
  std::cin >> num_threads;
  
  // sending data 
 //  const char* message = "Hello, server!"; 
    send(clientSocket, reinterpret_cast<const char*>(&num_threads), sizeof(num_threads), 0);
	
 // closing socket 
    close(clientSocket);  

  return 0;
}

