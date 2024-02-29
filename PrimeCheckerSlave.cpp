#include <iostream>
#include <winsock2.h>

using namespace std;

#define BUFFERSIZE 1024

int primeCheckerSlave(int start, int end) {
    int count = 0;

    for (int current_num = start; current_num <= end; current_num++) {
        bool is_prime = true;

        for (int i = 2; i * i <= current_num; i++) {
            if (current_num % i == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            count++;
        }
    }

    return count;
}

int main() {
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

    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Specify server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // Get input for the range from the master
    char buffer[BUFFERSIZE];
    int start, end;

    cout << "Enter the start of the range: ";
    cin >> start;
    cout << "Enter the end of the range: ";
    cin >> end;

    // Send the range to the master
    send(clientSocket, to_string(start).c_str(), sizeof(buffer), 0);
    send(clientSocket, to_string(end).c_str(), sizeof(buffer), 0);

    // Receive the result from the master
    recv(clientSocket, buffer, sizeof(buffer), 0);
    int numPrimes = atoi(buffer);

    cout << "Number of primes in the range: " << numPrimes << endl;

    // Close the socket
    closesocket(clientSocket);

    return 0;
}

