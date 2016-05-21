#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <vector>

using namespace std;

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
const int MAX_BUF_LENGTH = 255;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

string startGame()
{
    string name;
    pthread_mutex_lock(&printMutex);
    cout << "Welcome to Number Guessing Game!" << endl;
    cout << "Enter your name: ";
    cin >> name;
    cout << endl;
    pthread_mutex_unlock(&printMutex);
    return name;
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
    
    // start game and get player's name
    string name = startGame();
    // write name to socket
    write(sockfd, name.c_str(), strlen(name.c_str()));
    
    int turn = 1;
    int32_t guess, tmpGuess, diff, tmpDiff;
    diff = 1;
    while (diff != 0) 
    {
        pthread_mutex_lock(&printMutex);
        cout << "Turn: " << turn << endl;
        cout << "Enter a guess: ";
        pthread_mutex_unlock(&printMutex);

        cin >> guess;
        while (guess < 0 || guess >= 10000) 
        {
            pthread_mutex_lock(&printMutex);
            cout << "Invalid guess, make another " << endl;
            cout << "guess between 0 and 10,000: ";
            pthread_mutex_unlock(&printMutex);
            cin >> guess;
        }
        tmpGuess = htonl(guess);
        write(sockfd, &tmpGuess, sizeof(tmpGuess));

        read(sockfd, &tmpDiff, sizeof(tmpDiff));
        diff = ntohl(tmpDiff);

        pthread_mutex_lock(&printMutex);
        cout << "Result of guess: " << diff << endl;
        cout << endl;
        pthread_mutex_unlock(&printMutex);
        turn++;
    }

    bzero(buffer, 255);
    read(sockfd, &buffer, 255);
    string victoryMessage(buffer);

    pthread_mutex_lock(&printMutex);
    cout << victoryMessage << endl;
    cout << endl;
    pthread_mutex_unlock(&printMutex);

    int32_t leaders, tmpLeaders;
    read(sockfd, &tmpLeaders, sizeof(tmpLeaders));
    leaders = ntohl(tmpLeaders);

    pthread_mutex_lock(&printMutex);
    cout << "Leader board:" << endl;
    pthread_mutex_unlock(&printMutex);

    for (int i = 0; i < leaders; i++)
    {
        string leader = "";
        vector<char> buf(MAX_BUF_LENGTH);
        int bytesRecv = 0;
        do
        {
            bytesRecv = read(sockfd, buf.data(), buf.size());
            leader.append(buf.cbegin(), buf.cend());

        } while (bytesRecv == MAX_BUF_LENGTH);
        pthread_mutex_lock(&printMutex);
        cout << leader << endl;
        pthread_mutex_unlock(&printMutex);
    }

    close(sockfd);
    return 0;
}