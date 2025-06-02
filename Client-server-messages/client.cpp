#include <iostream>
#include <winsock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);
    int error = WSAStartup(version, &wsaData);
    if (error != 0) {
        cout << "Initialize error of Winsock: " << error << endl;
        return 1;
    }

    // Socket creation
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Creation winsock error " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Choose server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Server connection
    error = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (error == SOCKET_ERROR) {
        cout << "Connection error " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    int numThreads;
    cout << "Input num of the threads ";
    cin >> numThreads;

    int arrSize;
    cout << "Input size of the array ";
    cin >> arrSize;

    int* arr = new int[arrSize];
    cout << "Input the elements of the array ";
    for (int i = 0; i < arrSize; i++) {
        cin >> arr[i];
    }

    // Send array and numThreads
    error = send(clientSocket, (char*)&numThreads, sizeof(numThreads), 0);
    if (error == SOCKET_ERROR) {
        cout << "Error with sending the num threads: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    error = send(clientSocket, (char*)&arrSize, sizeof(arrSize), 0);
    if (error == SOCKET_ERROR) {
        cout << "Error with sending the array size: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    error = send(clientSocket, (char*)arr, sizeof(int) * arrSize, 0);
    if (error == SOCKET_ERROR) {
    cout << "Error with sending the array: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    WSACleanup();
    return 1;
    }
// Receive sorted array
error = recv(clientSocket, (char*)arr, sizeof(int) * arrSize, 0);
if (error == SOCKET_ERROR) {
    cout << "Error with receiving the array: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    WSACleanup();
    return 1;
}

// Print sorted array
cout << "The sorted array is: ";
for (int i = 0; i < arrSize; i++) {
    cout << arr[i] << " ";
}
cout << endl;

closesocket(clientSocket);
WSACleanup();
delete[] arr;

return 0;
}
