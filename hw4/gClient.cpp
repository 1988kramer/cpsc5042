// Andrew Kramer
// CPSC 5042
// Homework 4
// 5/23/2016

// gClient.cpp

// compile using g++ using the following command line:
// g++ -std=c++11 gClient.cpp -o gClient

// run using the following command line
// ./gClient [hostname] [portno]

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

// prints welcome message and requests players name
// returns string containing player's name
string startGame()
{
    string name;
    pthread_mutex_lock(&printMutex);
    cout << "Welcome to Number Guessing Game!" << endl;
    cout << "Enter your name: ";
    getline(cin, name);
    cout << endl;
    pthread_mutex_unlock(&printMutex);
    return name;
}

int main(int argc, char *argv[])
{
    // socket initialization code
    // basically stolen from the linux howto on socket programming
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

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
    int32_t nameLen = strlen(name.c_str());

    // write name length to server
    nameLen = htonl(nameLen);
    write(sockfd, &nameLen, sizeof(nameLen));

    // write name to server 
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
        // make sure guess is valid
        while (guess < 0 || guess >= 10000) 
        {
            pthread_mutex_lock(&printMutex);
            cout << "Invalid guess, make another " << endl;
            cout << "guess between 0 and 10,000: ";
            pthread_mutex_unlock(&printMutex);
            cin >> guess;
        }
        // write guess to server
        tmpGuess = htonl(guess);
        write(sockfd, &tmpGuess, sizeof(tmpGuess));

        // read result of guess from server
        read(sockfd, &tmpDiff, sizeof(tmpDiff));
        diff = ntohl(tmpDiff);

        // print result of guess
        pthread_mutex_lock(&printMutex);
        cout << "Result of guess: " << diff << endl;
        cout << endl;
        pthread_mutex_unlock(&printMutex);
        turn++;
    }

    // read victory message length from server
    int32_t messageLen;
    read(sockfd, &messageLen, sizeof(messageLen));
    messageLen = ntohl(messageLen);

    // read victory message from server
    vector<char> message;
    message.reserve(messageLen + 1);
    if (read(sockfd, &message[0], messageLen) < 0)
        error("unable to read message");
    string victoryMessage = "";
    victoryMessage.append(&message[0], messageLen);

    // print victory message
    pthread_mutex_lock(&printMutex);
    cout << victoryMessage << endl;
    pthread_mutex_unlock(&printMutex);

    close(sockfd);
    return 0;
}