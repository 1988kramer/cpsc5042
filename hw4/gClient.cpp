#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>

using namespace std;

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    // printf("Please enter the message: ");
    bzero(buffer,256);
    // read request for name from server
    read(sockfd, buffer, 255);

    // print request for name
    pthread_mutex_lock(&printMutex);
    printf("%s", buffer);
    pthread_mutex_unlock(&printMutex);

    string name;
    cin >> name;
    // write name to socket
    write(sockfd, name.c_str(), strlen(name.c_str()));
    int turn = 1;

    int32_t guess, tmpGuess, diff, tmpDiff;
    while (diff != 0) 
    {
        pthread_mutex_lock(&printMutex);
        cout << "Turn: " << turn << endl;
        cout << "Enter a guess: ";
        pthread_mutex_unlock(&printMutex);

        cin >> guess;
        tmpGuess = htonl(guess);
        write(sockfd, &tmpGuess, sizeof(tmpGuess));

        read(sockfd, &tmpDiff, 255);
        diff = ntohl(tmpDiff);

        pthread_mutex_lock(&printMutex);
        cout << "Result of guess: " << diff << endl;
        pthread_mutex_unlock(&printMutex);
        turn++;
    }
    close(sockfd);
    return 0;
}