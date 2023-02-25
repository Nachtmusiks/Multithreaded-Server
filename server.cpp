// A simple server in the internet domain using TCP
// The port nu1mber is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <algorithm>
#include <math.h>
#include <vector>
using namespace std;
struct symbol
{
    char name;
    double prob;
    double fx;
    double fxbar; // prev fx + 1/2 prob
    string fxbin;
    double prevfx;
};
string decBin(double num, int binL) // Decimal fraction to binary
{
    string binary = "";
    int inte = num;
    double fractional = num - inte;
    while (inte)
    {
        int rem = inte % 2;
        binary.push_back(rem + '0');
        inte /= 2;
    }
    reverse(binary.begin(), binary.end());
    binary.push_back('.');
    while (binL--)
    {
        fractional *= 2;
        int fract_bit = fractional;

        if (fract_bit == 1)
        {
            fractional -= fract_bit;
            binary.push_back(1 + '0');
        }
        else
        {
            binary.push_back(0 + '0');
        }
    }
    return binary;
}
void SFE(struct symbol &d) // Shannon Fano Elias encoding
{
    //Since struct is global no need to return anything
    d.fxbar = d.prob / 2 + d.prevfx; // pref fx
    int binL = ceil(log2(1 / d.prob)) + 1; // Find length of binary
    d.fxbin = decBin(d.fxbar, binL);
    d.fxbin = d.fxbin.erase(0, 1); // Erase leading 0 
    return;
}

void fireman(int)
{
   while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
    int network_socket, newsockfd, portno, clilen; //internet sockets and length of client address
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD, fireman); 
    if (argc < 2)
    {
        cerr << "ERROR, no port provided\n";
        exit(1);
    }
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket < 0)
    {
        cerr << "ERROR opening socket";
        exit(1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(network_socket, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
    {
        cerr << "ERROR on binding";
        exit(1);
    }
    listen(network_socket, 5); //5 is the amount of backlog for queues
    clilen = sizeof(cli_addr);
    pid_t pid;

    while (true)
    {
        newsockfd = accept(network_socket, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0)
        {
            struct symbol data;   
            if (newsockfd < 0)
            {
                cerr << "ERROR on accept";
                exit(1);
            }
            n = read(newsockfd, &data.prob, sizeof(double));
            if (n < 0)
            {
                cerr << "ERROR reading from socket";
                exit(1);
            }
            n = read(newsockfd, &data.prevfx, sizeof(double)); //find fx
            if (n < 0)
            {
                cerr << "ERROR reading from socket";
                exit(1);
            }
            n = read(newsockfd, &data.name, 1); //data.name has to be char or char* the size is 1 because only 1 char
            if (n < 0)
            {
                cerr << "ERROR reading from socket";
                exit(1);
            }
            SFE(data);

            int slen = data.fxbin.length(); //Get the string length
            char binCode[slen + 1]; //Create a new char array
            strcpy(binCode, data.fxbin.c_str()); //Conver string to char arrays

            n = write(newsockfd, &slen, sizeof(int)); //Send the size of the possible binCode to the client so they can create an approriate char* buffer size
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            n = write(newsockfd, &binCode, slen); //Sending binCode over
            if (n < 0)
            {
                std::cerr << "ERROR writing to socket";
                exit(1);
            }
            close(newsockfd);
            _exit(0);
        }
    }
    close(network_socket);
    return 0;
}
