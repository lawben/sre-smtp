
#include <cstdio>
#include <cstring>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <netinet/in.h>
#endif

#define SERVER_BUFFER_SIZE      1024
#define PORT 5555

int main() {
#ifdef WIN32
  WSAData wsaData = { 0 };
  WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  
  auto socketId = socket(PF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddr = { 0 };
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  auto error = bind(socketId, (sockaddr*) &serverAddr, sizeof(serverAddr));

  error = listen(socketId, 5);

  int finished = 0;
  while (!finished) {
    sockaddr_storage serverStorage;
    int addr_size = sizeof(serverStorage);
    auto connection = accept(socketId, (sockaddr*) &serverStorage, &addr_size);

    char buffer[SERVER_BUFFER_SIZE];
#ifdef WIN32
    auto get = recv(connection, buffer, SERVER_BUFFER_SIZE - 1, 0);
#else
    auto get = read(connection, buffer, SERVER_BUFFER_SIZE - 1);
#endif

    buffer[get] = '\0';
    fprintf(stdout, "%s\n", buffer);

#ifdef WIN32
    send(connection, "OK", 2, 0);
#else
    write(connection, "OK", 2);
#endif

    fprintf(stdout, "Message Complete\n");

#ifdef WIN32
    closesocket(connection);
#else
    close(connection);
#endif
  }

#ifdef WIN32
  closesocket(socketId);
#else
  close(socketId);
#endif
}