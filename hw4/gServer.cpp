
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>

using namespace std;

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}



void* serverThread(void* socket_fd)
{
    int socket = *(int*)socket_fd;
    if (socket < 0)
        error("ERROR on accept");
    int n;
    char name[256];
    bzero(name,256);
    write(socket, "Enter your name: ", 17);
    read(socket, name, 255);

    pthread_mutex_lock(&printMutex);
    printf("Player's name is: %s\n",name);
    pthread_mutex_unlock(&printMutex);

    int number = rand() % 10000;

    pthread_mutex_lock(&printMutex);
    cout << "Target number is " << number << endl;
    pthread_mutex_unlock(&printMutex);

    int32_t tmp, guess;
    int32_t turn, tmpTurn;
    turn = 1;
    /*
    tmpTurn = htonl(turn);
    write(socket, &tmpTurn, sizeof(tmpTurn));
    read(socket, &tmp, sizeof(tmp));
    guess = ntohl(tmp);
    
    pthread_mutex_lock(&printMutex);
    cout << "Guess was " << guess << endl;
    pthread_mutex_unlock(&printMutex);
    */

    while (guess != number)
    {
        pthread_mutex_lock(&printMutex);
        cout << name << " guessed " << guess << endl;
        pthread_mutex_unlock(&printMutex);

        tmpTurn = htonl(turn);
        write(socket, &tmpTurn, sizeof(tmpTurn));
        if (guess < number) 
        {
            write(socket,"Your guess was too low!", 23);
        }
        else
        {
            write(socket,"Your guess was too high!", 24);
        }
        read(socket, &tmp, sizeof(tmp));
        guess = ntohl(tmp);
        turn++;
    }
    turn = -1;
    tmpTurn = htonl(turn);
    write(socket, &tmpTurn, sizeof(tmpTurn));
    write(socket, "You guessed correctly!\nGoodbye!", 31);

    pthread_mutex_lock(&printMutex);
    cout << name << " guessed correctly!" << endl;
    pthread_mutex_unlock(&printMutex);

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