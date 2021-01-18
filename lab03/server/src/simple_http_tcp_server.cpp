#include "common_utils.h"
#include <vector>
#include <fstream>
#include <sstream>

//Визначаємо коди помилок при відправці відповідей та файлів
//Код помилки при відправці НТТР-відповіді
#define ERR_SEND_HTTP_HEADER -1
//Код помилки при відкритті запрошуваного файлу
#define ERR_OPEN_FILE -2
//Код помилки при відправці запрошуваного файлу
#define ERR_SEND_FILE -3
using namespace std;
#ifdef OS_WINDOWS
//Для Win32 використовуємо критичні секції
CRITICAL_SECTION syncObj;
//Функція потоку
DWORD WINAPI clientThread(LPVOID);
#else
//Для Unix/Linux використовуємо м’ютекси
pthread_mutex_t syncObj;
#define HANDLE pthread_t
#define DWORD pthread_t
//Функція потоку
void* clientThread(void*);
#endif
//Шаблони запитів та відповіді серверу
#define DESCRIPTION "Simple web server demonstrates \
    work with simple HTTP-requests"
#define HTTP_HEADER "HTTP/1.1 200 OK\r\n" \
    "Content-Length: %d\r\n" \
    "Content-Type: text/html\r\n\r\n"
#define ERROR_FILE_FOUND_RESPONSE\
    "<html><body><H1>Error</H1><p>\
    File %s not found.</body></html>"
//Шлях до виконуваної програми
char appPath[BUFFER_SIZE];
int main(int argc, char* argv[]) {

    SOCKET listenSocket, newClient;
    struct sockaddr_in serverAddr, clientAddr;
#ifdef OS_WINDOWS
    int clientAddrLen;
    //Ініціюємо критичну секцію для синхронізації
    InitializeCriticalSection(&syncObj);
#else
    unsigned int clientAddrLen;
    pthread_mutex_init(&syncObj, NULL);
#endif
    int nPort = 5150, ret;
    HANDLE hThread;
    DWORD dwThreadId;
    char strPort[6];
    //З командного рядка визначаємо заданий порт серверу
    if (getParameter(argv, argc, "-port", strPort, ':')) {
        int tempPort = atoi(strPort);
        if (tempPort > 0)
            nPort = tempPort;
        else {
            cout << "\nError command argument " << argv[0];
            cout << " -port:<integer value>\n";
            cout << "\nUsage " << argv[0] << " -port:<integer value>\n";
        }
    }
    memset(appPath, 0, BUFFER_SIZE);
    if (!extractFilePath(appPath, argv[0]))
        strcpy(appPath, ".\\");
    //Ініціюємо бібліотеку сокетів
    if (initSocketAPI()) {
        socketError(TRUE, "init socket API");
        return -1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Зв’язуємо сокет з адресою машини
    if (bind(listenSocket, (struct sockaddr*)&serverAddr,
        sizeof(serverAddr))) {
        socketError(TRUE, "bind socket");
        return -2;
    }
    listen(listenSocket, LINEQ);
    printInfo(argv[0], DESCRIPTION);
    cout << "Waiting incoming connections in port:" << nPort << endl;
    //Масив ідентифікаторів потоків
    vector<HANDLE> hThreads;
    vector<SOCKET> sockets;
    //Нескінченний цикл очікування запитів на підключення
    while (1) {
        //Призупиняємо виконання циклу на 100 мс
#ifdef OS_WINDOWS
        Sleep(100);
#else
        usleep(100000);
#endif
        clientAddrLen = sizeof(clientAddr);
        newClient = accept(listenSocket,
            (struct sockaddr*)&clientAddr,
            &clientAddrLen);
        if (newClient <= 0) {
            cout << "Connections error\n";
            break;
        }
        cout << "Client [";
        cout << inet_ntoa(clientAddr.sin_addr) << ":";
        cout << ntohs(clientAddr.sin_port) << "] connected\n";
        //Створюємо потік, якому передаємо клієнтський сокет
#ifdef OS_WINDOWS
        hThread = CreateThread(NULL, CREATE_SUSPENDED,
            clientThread, (LPVOID)newClient,
            0, &dwThreadId);
        if (hThread == NULL) {
#else
        if (pthread_create(&dwThreadId, NULL,
            clientThread, (void*)newClient)) {
#endif
            cout << "Error creating thread: ";
#ifdef OS_WINDOWS
            cout << GetLastError() << endl;
#else
            cout << strerror(errno) << endl;
#endif
            continue;
        }
        else {//Додаємо сокет у масив
            sockets.push_back(newClient);
#ifdef OS_WINDOWS
            hThreads.push_back(hThread);
            SetThreadPriority(hThread, THREAD_PRIORITY_BELOW_NORMAL);
            ResumeThread(hThread);
#else
            hThreads.push_back(dwThreadId);
#endif
        }
        }
    //Надсилаємо клієнтам повідомлення про завершення
    //роботи серверу.
    for (int i = 0; i < sockets.size(); ++i) {
        if (sockets[i] > 0)
            send(sockets[i], shutdownServerCmd,
                strlen(shutdownServerCmd) + 1, 0);
    }
    //Якщо є активні потоки, то чекаємо їх завершення
    //і вивільняємо ресурси
#ifdef OS_WINDOWS
    switch (WaitForMultipleObjects(hThreads.size(), &hThreads[0],
        TRUE, INFINITE)) {
    case WAIT_OBJECT_0:
        for (int i = 0; i < hThreads.size(); ++i) {
            CloseHandle(hThreads[i]);
        }
        break;
    case WAIT_TIMEOUT:
        cout << "Error finished child threads\n";
        break;
    }
    //Видаляємо критичну секцію
    DeleteCriticalSection(&syncObj);
#else
    for (int i = 0; i < hThreads.size(); ++i) {
        if (pthread_join(hThreads[i], NULL)) {
            cout << "Error finished child threads\n";
        }
        close(hThreads[i]);
    }
    pthread_mutex_destroy(&syncObj);
#endif
    //Закриваємо слухаючий сокет
    closeSocket(listenSocket);
    //Звільняємо ресурси системи
    deinitSocketAPI();
    return 0;
    }
//Функція для переведення 16-вих символів у 10-ві цифри
int HexCharToDecDig(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9')
        return hexChar - '0';
    else
        switch (hexChar) {
        case 'A':
            return 10;
        case 'B':
            return 11;
        case 'C':
            return 12;
        case 'D':
            return 13;
        case 'E':
            return 14;
        case 'F':
            return 15;
        default:
            return -1;
        }
}
int getFilenameFromHTTPrequest(char* request,
    char* fileName) {
    int i = 5, j = 0;
    //Згідно із стандартом URL ряд символів пробіл, #, кирилиця і
    //ін. замінюється на послідовність %код_символу (у 16-річній
    //системі). Наприклад, пробіл замінюється на %20.
    while (request[i] != ' ') {
        if (request[i] != '%')
            fileName[j++] = request[i++];
        else {
            fileName[j++] = HexCharToDecDig(request[++i]) * 16 +
                HexCharToDecDig(request[++i]);
            ++i;
        }
    }
    fileName[j] = '\0';
#ifdef OS_WINDOWS
    while (char* ch = strchr(fileName, '/'))
        *ch = PATH_DELIM;
#endif // OS_WINDOWS
    return 1;
}
//Функція для синхронізованого виведення повідомлень
//з потоку на консоль
void syncOutPutStr(stringstream & buffer) {
#ifdef OS_WINDOWS
    if (TryEnterCriticalSection(&syncObj)) {
#else
    pthread_mutex_lock(&syncObj);
#endif
    cout << buffer.str() << endl;
#ifdef OS_WINDOWS
    LeaveCriticalSection(&syncObj);
    }
#else
    pthread_mutex_unlock(&syncObj);
#endif
    buffer.str(string());
}
//Функція для синхронізованої передачі файлу клієнту
int syncSendFile(SOCKET sock, char* sendFileName,
    int& nTotalSend) {
    int result = 0;
#ifdef OS_WINDOWS
    if (TryEnterCriticalSection(&syncObj)) {
#else
    pthread_mutex_lock(&syncObj);
#endif
    //Відкриваємо файл
    ifstream iFile(sendFileName);
    if (iFile.good()) {
        char dataBuffer[MAX_BUFFER_SIZE];
        char HTTPResponse[MAX_BUFFER_SIZE];
        //Кількість зчитаних і переданих байт даних
        int nSend = 0;
        nTotalSend = 0;
        while (!iFile.eof()) {
            memset(dataBuffer, 0, MAX_BUFFER_SIZE);
            memset(HTTPResponse, 0, MAX_BUFFER_SIZE);
            iFile.read(dataBuffer, MAX_BUFFER_SIZE);
            nSend = iFile.gcount();
            if (nSend <= 0)
                break;
            sprintf(HTTPResponse, HTTP_HEADER, nSend);
            if (send(sock, HTTPResponse, strlen(HTTPResponse), 0) < 0) {
                result = ERR_SEND_HTTP_HEADER;
                break;
            }
            nSend = send(sock, dataBuffer, nSend, 0);
            if (nSend < 0) {
                result = ERR_SEND_FILE;
                break;
            }
            nTotalSend += nSend;
        }
    }
    else result = ERR_OPEN_FILE;
    iFile.close();
#ifdef OS_WINDOWS
    LeaveCriticalSection(&syncObj);
    }
#else
    pthread_mutex_unlock(&syncObj);
#endif
    return result;
}
#ifdef OS_WINDOWS
DWORD WINAPI clientThread(LPVOID lpParam) {
#else
void* clientThread(void* lpParam) {
#endif
    //Перетворимо параметр до типу SOCKET
    SOCKET sock = (SOCKET)lpParam;
    struct sockaddr_in clientAddr;
    int ret;
#ifdef OS_WINDOWS
    int caSize = sizeof(clientAddr);
#else
    unsigned int caSize = sizeof(clientAddr);
#endif
    //Отримаємо дані про сокет клієнту
    getpeername(sock, (struct sockaddr*)&clientAddr, &caSize);
    char dataBuffer[MAX_BUFFER_SIZE];
    char HTTPResponse[BUFFER_SIZE];
    char fileName[MAX_BUFFER_SIZE];
    char filePath[MAX_BUFFER_SIZE];
    stringstream ss;
    int totalsize = 0;
    do {
        memset(fileName, 0, MAX_BUFFER_SIZE);
        memset(filePath, 0, MAX_BUFFER_SIZE);
        strcpy(filePath, appPath);
        ret = recv(sock, dataBuffer, MAX_BUFFER_SIZE, 0);
        if (ret < 0)
            continue;
        //Отримуємо ім’я файлу з НТТР-запиту
        getFilenameFromHTTPrequest(dataBuffer, fileName);
        strcat(filePath, fileName);
        //Якщо файл не вказано, шукаємо файл index.html
        if (filePath[strlen(filePath) - 1] == PATH_DELIM)
            strcat(filePath, "index.html");
        //Якщо розширення не вказано, шукаємо файли.html
        if (!strchr(filePath, '.'))
            strcat(filePath, ".html");
        ss << "Requested file: " << filePath << endl;
        syncOutPutStr(ss);
        switch (syncSendFile(sock, filePath, totalsize)) {
        case ERR_SEND_HTTP_HEADER:
            ss << "Error send header" << endl;
            ss << "Successfully sent " << totalsize << " bytes\n";
            break;
        case ERR_SEND_FILE:
            ss << "Error send file: " << filePath << endl;
            ss << "Successfully sent " << totalsize << " bytes\n";
            break;
        case ERR_OPEN_FILE:
            sprintf(HTTPResponse, ERROR_FILE_FOUND_RESPONSE, fileName);
            sprintf(dataBuffer, HTTP_HEADER, strlen(HTTPResponse));
            send(sock, dataBuffer, strlen(dataBuffer), 0);
            send(sock, HTTPResponse, strlen(HTTPResponse), 0);
            ss << "Error send request file: " << filePath << endl;
            break;
        default:
            ss << "Successfully sent " << totalsize << " bytes";
            ss << " from requested file: " << filePath << endl;
        }
        syncOutPutStr(ss);
    } while (ret > 0);
    ss << "Client [" << inet_ntoa(clientAddr.sin_addr);
    ss << ":" << ntohs(clientAddr.sin_port);
    ss << "] disconnected" << endl;
    syncOutPutStr(ss);
    closeSocket(sock);
    sock = 0;
    return 0;
}