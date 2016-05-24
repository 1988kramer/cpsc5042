// Andrew Kramer
// CPSC 5042
// Homework 4
// 5/23/2016

// gServer.cpp

// compile using g++ using the following command line:
// g++ -std=c++11 gServer.cpp -o gServer -pthread

// run using the following command line
// ./gServer [portno]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <vector>

using namespace std;

struct Player
{
    string name;
    int32_t turns;
};

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t leaderBoardMutex = PTHREAD_MUTEX_INITIALIZER;

const int MAX_BUF_LENGTH = 255;
const int LEADER_BOARD_SIZE = 3;
Player leaderBoard[LEADER_BOARD_SIZE];
int32_t leaderCount = 0;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void threadError(const char *msg)
{
    perror(msg);
    pthread_exit(NULL);
}

int32_t calculateDiff(int target, int guess)
{
    int32_t ones = abs((target % 10) - (guess % 10));
    int32_t tens = abs(((target % 100) / 10) - ((guess % 100) / 10));
    int32_t hundreds = abs(((target % 1000) / 100) - ((guess % 1000) / 100));
    int32_t thousands = abs((target / 1000) - (guess / 1000));
    int32_t result = ones + tens + hundreds + thousands;
    return result;
}

// initializes leader board to default dummy values
// so the program doesn't segfault when it tries to access
// the leaderboard for the first 3 times
void initializeLeaderBoard() 
{
    for (int i = 0; i < LEADER_BOARD_SIZE; i++)
    {
        Player dummyPlayer;
        dummyPlayer.name = "dummy";
        dummyPlayer.turns = 0;
        leaderBoard[i] = dummyPlayer;
    }
}

// Preconditions: accepts a player struct as a parameter
// Postconditions: adds the struct to the leaderboard in the appropriate
// place if the player is in the top 3
void addToLeaderBoard(Player player)
{
    int i = 0;

    pthread_mutex_lock(&leaderBoardMutex);
    // find the appropriate place for the given player
    while (leaderBoard[i].turns > 0 
            && player.turns >= leaderBoard[i].turns 
            && i < LEADER_BOARD_SIZE)
        i++;

    // shuffle lower entries down the leader board if necessary
    for (int j = LEADER_BOARD_SIZE - 1; j > i; j--)
        leaderBoard[j] = leaderBoard[j - 1];

    // insert player into leaderboard if necessary
    if (i < LEADER_BOARD_SIZE)
    {
        leaderBoard[i] = player;
        if (leaderCount < LEADER_BOARD_SIZE)
            leaderCount++;
    }
    pthread_mutex_unlock(&leaderBoardMutex);
}

string buildVictoryMessage(int turns)
{
    string victoryMessage = "Congratulations! It took ";
    victoryMessage += to_string(turns);
    victoryMessage += " turns to guess the number!\n\n";
    victoryMessage += "Leader Board:\n";

    // prevent leader board from changing while building string
    pthread_mutex_lock(&leaderBoardMutex);
    for (int i = 0; i < leaderCount; i++)
    {
        victoryMessage += to_string(i + 1);
        victoryMessage += ". ";
        victoryMessage += leaderBoard[i].name; 
        victoryMessage += " "; 
        victoryMessage += to_string(leaderBoard[i].turns);
        victoryMessage += "\n";
    }
    pthread_mutex_unlock(&leaderBoardMutex);
    return victoryMessage;
}

void* serverThread(void* socket_fd)
{
    int socket = *(int*)socket_fd;
    if (socket < 0)
        threadError("ERROR on accept");

    // read expected name length from client
    int32_t nameLen;
    if (read(socket, &nameLen, sizeof(nameLen)) < 0)
        threadError("unable to read name length");
    nameLen = ntohl(nameLen);

    // read name from client
    vector<char> buffer;
    buffer.reserve(nameLen + 1);
    if (read(socket, &buffer[0], nameLen) < 0)
        threadError("unable to read name");
    string name = "";
    name.append(&buffer[0], nameLen);

    pthread_mutex_lock(&printMutex);
    cout << "name length " << nameLen << endl;
    cout << "The player's name is: " << name << endl; 
    pthread_mutex_unlock(&printMutex);

    int target = rand() % 10000; // generate targer number

    pthread_mutex_lock(&printMutex);
    cout << "Target number for " << name << " is " << target << endl;
    pthread_mutex_unlock(&printMutex);

    int32_t guess, tmpGuess;
    int32_t diff, tmpDiff;
    int turns = 0;
    diff = 1;

    while (diff != 0)
    {
        // read guess from client
        if (read(socket, &tmpGuess, sizeof(tmpGuess)) < 0)
            threadError("unable to read guess");
        guess = ntohl(tmpGuess);

        pthread_mutex_lock(&printMutex);
        cout << name << " guessed " << guess << endl;
        pthread_mutex_unlock(&printMutex);

        // send guess result to client
        diff = calculateDiff(target, guess);
        tmpDiff = htonl(diff);
        if (write(socket, &tmpDiff, sizeof(tmpDiff)) < 0)
            threadError("unable to write guess result");
        turns++;
    }

    pthread_mutex_lock(&printMutex);
    cout << name << " guessed correctly!" << endl;
    pthread_mutex_unlock(&printMutex);

    // create player struct to add to leader board
    Player thisPlayer;
    thisPlayer.name = name;
    thisPlayer.turns = turns;
    addToLeaderBoard(thisPlayer);

    string victoryMessage = buildVictoryMessage(turns);

    // send victory message length to client
    int32_t messageLen = strlen(victoryMessage.c_str());
    messageLen = htonl(messageLen);
    if (write(socket, &messageLen, sizeof(messageLen)) < 0)
        threadError("unable to write victory message length");

    // send victory message to client
    if (write(socket, victoryMessage.c_str(), strlen(victoryMessage.c_str())) < 0)
        threadError("unable to write victory message");
    
    pthread_mutex_lock(&printMutex);
    cout << victoryMessage << endl;
    pthread_mutex_unlock(&printMutex);
    
    pthread_exit(NULL);
    close(socket);
    return 0;
}

int main(int argc, char *argv[])
{
    initializeLeaderBoard();

    signal(SIGPIPE, SIG_IGN);

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

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
        pthread_detach(thread_id);
    }
    close(sockfd);
    return 0; 
}