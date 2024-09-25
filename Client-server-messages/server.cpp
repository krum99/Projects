#include <iostream>
#include <winsock2.h>
#include <vector>
#include <algorithm>
#include <thread>

using namespace std;

#pragma comment(lib, "ws2_32.lib")


void selectionSort(vector<int>& arr, int start, int end) {
    for (int i = start; i < end; ++i) {
        int minIndex = i;
        for (int j = i + 1; j < end; ++j) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        swap(arr[i], arr[minIndex]);
    }
}

// Функция за сливане на два сортирани масива
void merge(vector<int>& arr, int start, int mid, int end) {
    vector<int> temp(end - start);
    int i = start, j = mid, k = 0;

    while (i < mid && j < end) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i < mid) {
        temp[k++] = arr[i++];
    }

    while (j < end) {
        temp[k++] = arr[j++];
    }

    copy(temp.begin(), temp.end(), arr.begin() + start);
}

// Функция за паралелен Selection Sort и сливане на резултатите
void parallelSelectionSort(vector<int>& arr, int numThreads) {
    int size = arr.size();
    int chunkSize = size / numThreads;

    vector<thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? size : start + chunkSize;
        threads.emplace_back(selectionSort, ref(arr), start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int mergeSize = chunkSize; mergeSize < size; mergeSize *= 2) {
        for (int start = 0; start < size - mergeSize; start += mergeSize * 2) {
            int mid = start + mergeSize;
            int end = min(start + mergeSize * 2, size);
            merge(arr, start, mid, end);
        }
    }
}
int main() {
// Инициализиране на Winsock
WSADATA wsaData;
WORD version = MAKEWORD(2, 2);
int error = WSAStartup(version, &wsaData);
if (error != 0) {
cout << "Initialize error with the winsock: " << error << endl;
return 1;
}

// Създаване на сокет
SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (listenSocket == INVALID_SOCKET) {
    cout << "Creation error of the socket: " << WSAGetLastError() << endl;
    WSACleanup();
    return 1;
}

// Свързване на сокета към порт 8888
sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(8888);
serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

error = bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
if (error == SOCKET_ERROR) {
    cout << "Connection error of the socket: " << WSAGetLastError() << endl;
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}

// Очакване на входящи връзки от клиенти
error = listen(listenSocket, SOMAXCONN);
if (error == SOCKET_ERROR) {
    cout << "Listening error of the socket: " << WSAGetLastError() << endl;
closesocket(listenSocket);
WSACleanup();
return 1;
}
cout << "The server is ready to receive connections." << endl;

// Приемане на входяща връзка от клиент
SOCKET clientSocket;
sockaddr_in clientAddr;
int clientAddrSize = sizeof(clientAddr);
clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
if (clientSocket == INVALID_SOCKET) {
    cout << "Error with accpeting input connection: " << WSAGetLastError() << endl;
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}

// Получаване на броя на нишките и масива от клиента
int numThreads;
error = recv(clientSocket, (char*)&numThreads, sizeof(numThreads), 0);
if (error == SOCKET_ERROR) {
    cout << "Error with receiveng the num threads: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}

int arrSize;
error = recv(clientSocket, (char*)&arrSize, sizeof(arrSize), 0);
if (error == SOCKET_ERROR) {
    cout << "Error with receiving the array size: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}

// Получаване на масива от клиента
vector<int> arr(arrSize);
error = recv(clientSocket, (char*)arr.data(), sizeof(int) * arrSize, 0);
if (error == SOCKET_ERROR) {
    cout << "Error with receiving the array: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}
parallelSelectionSort(arr, numThreads);
//for(int i = 0; i < arr.size();i++)
//{
//    cout<<arr[i];
//}
cout<<endl;
// Изпращане на сортирания масив обратно на клиента
error = send(clientSocket, (char*)arr.data(), sizeof(int) * arrSize, 0);
if (error == SOCKET_ERROR) {
    cout << "Error with sending the sorted array: " << WSAGetLastError() << endl;
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();
    return 1;
}

// Затваряне на връзката с клиента и на слушащия сокет
closesocket(clientSocket);
closesocket(listenSocket);

// Изчистване на Winsock
WSACleanup();

return 0;
}