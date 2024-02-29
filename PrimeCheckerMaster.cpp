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
  int range_size = upper_bound / num_threads;
  
  for (int i = 0; i < num_threads; i++) {
    int start = i * range_size + l;
    int end = (i == num_threads - 1) ? upper_bound : (i + 1) * range_size;
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
    bool useSlaves = false;

    //Get input on if to use slaves
    while(true){
        cout << "Use slave servers?(Y/N): ";
        cin.getline(buffer, BUFFERSIZE);
        if(strcmp(buffer, "Y") == 0 || strcmp(buffer, "y") == 0){
            useSlaves = true;
            break;
        }   
        else if(strcmp(buffer, "N") == 0 || strcmp(buffer, "n") == 0){
            useSlaves = false;
            break;
        }  
        else
            cout << "Invalid input!\n";
    }

    if(useSlaves){ //Connect to slave servers

    }
    
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
    cout << endl << "PrimeCheckerMaster is running..." << endl;

    while(true){ //Make server continuously accept requests until it is closed in cmd
      // accepting connection request
      int clientSocket = accept(serverSocket, nullptr, nullptr); 
    
      // recieving data 
      int result_bound = -1;
      int result_threads = -1;

      // receive upper limit
      recv(clientSocket, buffer, sizeof(buffer), 0);
      result_bound = atoi(buffer);
      
      // receive threads count
      recv(clientSocket, buffer, sizeof(buffer), 0);
      result_threads = atoi(buffer);

      if(!useSlaves || true)//Replace with !useSlaves after slave servers implemented
        //If the user at server start decides not to use slave servers
        primeChecker(1, result_bound, result_threads);
      else{
        //Else use slave servers
      }
      //Send the results back to client
      std::string s = std::to_string(numPrimes);
      char const *res = s.c_str();
      send(clientSocket, res, sizeof(buffer), 0);
    }

    // closing the socket. 
    closesocket(serverSocket); 
  
    return 0; 
}