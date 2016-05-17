
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void write()
{

}

void* serverThread(void* socket_fd)
{
    int socket = *(int*)socket_fd;
    if (socket < 0)
        error("ERROR on accept");
    int n;
    char name[256];
    bzero(name,256);
    if (write(socket, "Enter your name: ", 17); < 0) 
    error("ERROR writing to socket");
    if (read(socket, name, 255) < 0) 
        error("Error reading from socket");
    printf("Player's name is: %s\n",name);
    int number = rand() % 10000;
    if (write(socket,"Turn: 1\nEnter a guess: ",23) < 0) 
        error("ERROR writing to socket");
    if (read(socket, name, 255) < 0) 
        error("Error reading from socket");
    close(socket);
    return 0;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     // char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     // int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     pthread_t thread_id;
     while ((newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen)))
    {
        if (pthread_create(&thread_id, NULL, serverThread, (void*) &newsockfd) < 0)
            error("could not create thread");
        printf("Thread created!\n");
    }
     close(sockfd);
     return 0; 
}