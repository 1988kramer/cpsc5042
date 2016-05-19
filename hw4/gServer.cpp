
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <vector>

using namespace std;

struct Player
{
    string name;
    int turns;
};

pthread_mutex_t printMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t leaderBoardMutex = PTHREAD_MUTEX_INITIALIZER;

const int MAX_BUF_LENGTH = 255;
const int LEADER_BOARD_SIZE = 3;
Player leaderBoard[LEADER_BOARD_SIZE];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int32_t calculateDiff(int target, int guess)
{
    int32_t ones = abs((target % 10) - (guess % 10));
    int32_t tens = abs(((target % 100) / 10) - ((guess % 100) / 10));
    int32_t hundreds = abs(((target % 1000) / 100) - ((guess % 1000) / 100));
    int32_t thousands = abs((target / 1000) - (guess / 1000));
    int32_t result = ones + (10 * tens) + (100 * hundreds) + (1000 * thousands);
    return result;
}

void addToLeaderBoard(Player player)
{
    int i = 0;
    pthread_mutex_lock(&leaderBoardMutex);

    // segfaults here
    // tries to read entries that are not initialized
    while (player.turns > leaderBoard[i].turns && i < LEADER_BOARD_SIZE)
    {
        i++;
    }
    leaderBoard[i] = player;
    pthread_mutex_unlock(&leaderBoardMutex);

    for (int i = 0; i < LEADER_BOARD_SIZE; i++)
    {
        cout << leaderBoard[i].name << " " << leaderBoard[i].turns << endl;
    }
}

void* serverThread(void* socket_fd)
{
    int socket = *(int*)socket_fd;
    if (socket < 0)
        error("ERROR on accept");
    string name;
    write(socket, "Enter your name: ", 17);
    vector<char> buffer(MAX_BUF_LENGTH);
    int bytesRecv = 0;
    do
    {
        bytesRecv = read(socket, buffer.data(), MAX_BUF_LENGTH - 1);
        name.append(buffer.cbegin(), buffer.cend());

    } while (bytesRecv == MAX_BUF_LENGTH);

    pthread_mutex_lock(&printMutex);
    cout << "The player's name is: " << name << endl; 
    pthread_mutex_unlock(&printMutex);

    int target = rand() % 10000;

    pthread_mutex_lock(&printMutex);
    cout << "Target number is " << target << endl;
    pthread_mutex_unlock(&printMutex);

    int32_t guess, tmpGuess;
    int32_t diff, tmpDiff;
    int turns = 0;
    diff = 1;

    while (diff != 0)
    {
        read(socket, &tmpGuess, sizeof(tmpGuess));
        guess = ntohl(tmpGuess);

        pthread_mutex_lock(&printMutex);
        cout << name << " guessed " << guess << endl;
        pthread_mutex_unlock(&printMutex);

        diff = calculateDiff(target, guess);

        tmpDiff = htonl(diff);
        cout << "writing diff" << endl;
        write(socket, &tmpDiff, sizeof(tmpDiff));
        cout << "diff written" << endl;
        turns++;
    }

    pthread_mutex_lock(&printMutex);
    cout << name << " guessed correctly!" << endl;
    pthread_mutex_unlock(&printMutex);

    Player thisPlayer;
    thisPlayer.name = name;
    thisPlayer.turns = turns;

    addToLeaderBoard(thisPlayer);

    close(socket);
    return 0;
}

int main(int argc, char *argv[])
{
    for(int i = i; i < LEADER_BOARD_SIZE; i++)
        cout << leaderBoard[i].turns << ", ";
    cout << endl;

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