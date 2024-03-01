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

    //Get Addresses and ports
    string serverip, slaveip ="127.0.0.2";
    int serverport, slaveport = 8080;
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

    if(useSlaves){ //Get slave ip and port
      cout << "Slave IP Address(Default=127.0.0.2): ";
      cin.getline(buffer, BUFFERSIZE);
      if(strcmp(buffer, "") == 0)
        slaveip = string("127.0.0.2");
      else
        slaveip = string(buffer);

      cout << "Slave Port(Default=5555): ";
      cin.getline(buffer, BUFFERSIZE);
      if(strcmp(buffer, "") == 0)
        slaveport = 5555;
      else
        slaveport = atoi(buffer);
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
    serverAddress.sin_port = htons(serverport); 
    serverAddress.sin_addr.s_addr = inet_addr(serverip.c_str());

    slaveAddress.sin_family = AF_INET; 
    slaveAddress.sin_port = htons(slaveport); 
    slaveAddress.sin_addr.s_addr = inet_addr(slaveip.c_str()); 

    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
  
    // listening to the assigned socket 
    listen(serverSocket, 5);
    cout << endl << "PrimeCheckerMaster is running..." << endl;

    while(true){ //Make server continuously accept requests until it is closed in cmd
      // accepting connection request
      int clientSocket = accept(serverSocket, nullptr, nullptr);
      cout << endl << "Client request received!" << endl;
    
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

      if(!useSlaves){//Replace with !useSlaves after slave servers implemented
        //If the user at server start decides not to use slave servers
        cout << endl << "Calculating with master only..." << endl;
        primeChecker(result_start, result_bound, result_threads);
        cout << endl << "Result: " << numPrimes << endl;
      }
      else{
        //Else use slave servers
        cout << endl << "Calculating with master and slave..." << endl;
        primeChecker(result_start, (result_bound+result_start)/2, result_threads); //First half calc here
        cout << endl << "Master Result: " << numPrimes << endl;

        //Connect to slave server
        slaveSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        connect(slaveSocket, (struct sockaddr*)&slaveAddress, sizeof(slaveAddress));

        //Convert ints to char[] and send data of second half to slave
        std::string s = std::to_string(((result_bound+result_start)/2)+1);
        send(slaveSocket, s.c_str(), sizeof(buffer), 0);
        s = std::to_string(result_bound);
        send(slaveSocket, s.c_str(), sizeof(buffer), 0);
        s = std::to_string(result_threads);
        send(slaveSocket, s.c_str(), sizeof(buffer), 0);
        cout << endl << "Parameters sent to slave!" << endl;

        //Receive result from slave, add to numPrimes
        recv(slaveSocket, buffer, sizeof(buffer), 0);
        int res = atoi(buffer);
        cout << endl << "Slave Result: " << res << endl;

        numPrimes += res;
        cout << endl << "Overall Result: " << numPrimes << endl;

        //Close slave socket connection
        closesocket(slaveSocket);
      }

      //Send the results back to client
      std::string s = std::to_string(numPrimes);
      numPrimes = 0; //Reset 
      send(clientSocket, s.c_str(), sizeof(buffer), 0);
      cout << endl << "Client request answered! Waiting for next request..." << endl;

      closesocket(clientSocket); //Close client socket connection
    }

    // closing the socket. 
    closesocket(serverSocket); 
  
    return 0; 
}