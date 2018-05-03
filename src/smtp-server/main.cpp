#include <netinet/in.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

#define SERVER_BUFFER_SIZE      1024
#define PORT 5555

int main() {
  int socketId = socket(PF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serverAddr;
  bzero((char*) &serverAddr, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  bind(socketId, (struct sockaddr*) &serverAddr, sizeof(serverAddr));

  listen(socketId, 5);

  int finished = 0;
  while (!finished) {
    struct sockaddr_storage serverStorage;
    socklen_t addr_size = sizeof serverStorage;
    int connection = accept(socketId, (struct sockaddr*) &serverStorage, &addr_size);

    char buffer[SERVER_BUFFER_SIZE];
    int get = read(connection, buffer, SERVER_BUFFER_SIZE - 1);

    buffer[get] = '\0';
    fprintf(stdout, "%s\n", buffer);

    write(connection, "OK", 2);

    fprintf(stdout, "Message Complete\n");

    close(connection);
  }

  close(socketId);
}