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
int numPrimes;

bool check_prime(const int &n) {
	
  for (int i = 2; i * i <= n; i++) {
    if (n % i == 0) {
      return false;
    }
  }
  
  return true;
}

/*
This function implements mutual exclusion for the `primes` list.

Parameters:
start : int - start of the range (inclusive)
end : int - end of the range (inclusive)
*/
void get_primes(int start, int end) {
	
  for (int current_num = start; current_num <= end; current_num++) {
    bool is_prime = check_prime(current_num);

    if (is_prime) {
      {
        std::lock_guard<std::mutex> lock(primes_mutex);
        primes.push_back(current_num);
      }
    }
  }
  
}


void primeChecker(int l, int u, int t) {
  int upper_bound = u;
  int num_threads = t;

  // split the range of integers across the specified number of threads
  std::vector<std::thread> threads;
  int range_size = (upper_bound - l) / num_threads;
  
  for (int i = 0; i < num_threads; i++) {
    int start = i * range_size + l;
    int end = (i == num_threads - 1) ? upper_bound : (i + 1) * range_size + l - 1;
    threads.emplace_back(get_primes, start, end);
  }

  // join threads
  for (auto &thread : threads) {
    thread.join();
  }
  
  numPrimes = primes.size(); //Save number of primes
  primes.clear(); //Empty vector
}


int main() 
{ 
    char buffer[BUFFERSIZE];

    //Get Addresses and ports
    string serverip;
    int serverport;
    cout << "Server IP Address(Default=127.0.0.2): ";
    cin.getline(buffer, BUFFERSIZE);
    if(strcmp(buffer, "") == 0)
      serverip = string("127.0.0.2");
    else
      serverip = string(buffer);

    cout << "Server Port(Default=5555): ";
    cin.getline(buffer, BUFFERSIZE);
    if(strcmp(buffer, "") == 0)
      serverport = 5555;
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
    
    // creating socket 
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
  
    // specifying the address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(serverport); 
    serverAddress.sin_addr.s_addr = inet_addr(serverip.c_str()); 
  
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // listening to the assigned socket 
    listen(serverSocket, 5);
    cout << endl << "PrimeCheckerSlave is running..." << endl;

    while(true){ //Make server continuously accept requests until it is closed in cmd
      // accepting connection request
      int clientSocket = accept(serverSocket, nullptr, nullptr);
      cout << endl << "Master request received!" << endl;
    
      // recieving data 
      int result_start = -1;
      int result_bound = -1;
      int result_threads = -1;

      // receive lower limit
      recv(clientSocket, buffer, sizeof(buffer), 0);
      result_start = atoi(buffer);
      
      // receive upper limit
      recv(clientSocket, buffer, sizeof(buffer), 0);
      result_bound = atoi(buffer);
      
      // receive threads count
      recv(clientSocket, buffer, sizeof(buffer), 0);
      result_threads = atoi(buffer);
    
      cout << endl << "Calculating..." << endl;
      primeChecker(result_start ,result_bound, result_threads); //Do Calcs
      cout << endl << "Result: " << numPrimes << endl;

      //Send the results back to client
      std::string s = std::to_string(numPrimes);
      numPrimes = 0; //Reset
      send(clientSocket, s.c_str(), sizeof(buffer), 0);
      cout << endl << "Master request answered! Waiting for next request..." << endl;

      closesocket(clientSocket); //Close client socket connection
    }

    // closing the socket. 
    closesocket(serverSocket); 
  
    return 0; 
}