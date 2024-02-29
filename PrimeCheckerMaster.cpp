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
    
    // creating sockets
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKET slaveSocket = INVALID_SOCKET;  
  
    // specifying the addresses 
    sockaddr_in serverAddress, slaveAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8080); 
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    slaveAddress.sin_family = AF_INET; 
    slaveAddress.sin_port = htons(5555); 
    slaveAddress.sin_addr.s_addr = inet_addr("127.0.0.2"); 

    if(useSlaves){ //Connect to slave servers
      slaveSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      connect(slaveSocket, (struct sockaddr*)&slaveAddress, sizeof(slaveAddress));
    }
  
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

      if(!useSlaves)//Replace with !useSlaves after slave servers implemented
        //If the user at server start decides not to use slave servers
        primeChecker(1, result_bound, result_threads);
      else{
        //Else use slave servers
        primeChecker(1, result_bound/2, result_threads); //First half calc here

        //Convert ints to char[]
        std::string s = std::to_string((result_bound/2)+1);
        char const *lower = s.c_str();
        s = std::to_string(result_bound);
        char const *upper = s.c_str();
        s = std::to_string(result_threads);
        char const *thread = s.c_str();

        //Send data of second half to slave
        send(slaveSocket, lower, sizeof(buffer), 0);
        send(slaveSocket, upper, sizeof(buffer), 0);
        send(slaveSocket, thread, sizeof(buffer), 0);

        //Receive result from slave, add to numPrimes
        recv(slaveSocket, buffer, sizeof(buffer), 0);
        numPrimes += atoi(buffer);
      }

      //Send the results back to client
      std::string s = std::to_string(numPrimes);
      numPrimes = 0; //Reset
      char const *res = s.c_str();
      send(clientSocket, res, sizeof(buffer), 0);
    }

    // closing the socket. 
    closesocket(serverSocket); 
  
    return 0; 
}