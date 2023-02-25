//./client localhost port#
//./server port#
#include <vector>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "pthread.h"
#include <map>
using namespace std;
struct sockaddr_in serv_addr; //Declare this globally so we dont have to pass it through threads
struct hostent *server;

struct datas {
    char name;
    double prob;
    double fx;
    double prevfx; // prev fx + 1/2 prob
    string fxbin;
};

void* clienthread(void* args)
{
    int n, size;
    struct datas &d = *(struct datas *)args;
	int network_socket;

	// Create a stream socket
	network_socket = socket(AF_INET,SOCK_STREAM, 0);

	// Initiate a socket connection
	int connection_status = connect(network_socket,
		(struct sockaddr*)&serv_addr,
		sizeof(serv_addr));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error\n");
		return 0;
	}

	// Send data to the socket
    n = write(network_socket, &d.prob, sizeof(double));
    if (n < 0)
    {
        cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(network_socket, &d.prevfx, sizeof(double));
    if (n < 0)
    {
        cerr << "ERROR writing to socket";
        exit(1);
    }
    n = write(network_socket, &d.name, 1);
    if (n < 0)
    {
        cerr << "ERROR writing to socket";
        exit(1);
    }
    //Receive binary code from server
    int nsize; //Size of string
    n = read(network_socket, &nsize, sizeof(int));
    if (n < 0)
    {
        cerr << "ERROR reading from socket";
        exit(1);
    }
    char *buffer = new char[nsize + 1];
    bzero(buffer, nsize + 1); //Initialize buffer byte size to 0
    n = read(network_socket, buffer, nsize);
    d.fxbin = buffer;
    delete [] buffer;

	// Close the connection
	close(network_socket);
	pthread_exit(NULL);
	return 0;
}

int main(int argc, char *argv[])
{
    int portno, n;

    if (argc < 3)
    {
        cerr << "usage " << argv[0] << "hostname port\n";
        exit(0);
    }
    portno = atoi(argv[2]);
    //portno = 1235;
    server = gethostbyname(argv[1]); 
    if (server == NULL)
    {
        cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    //Probability calculation
    string s;
    s = "AAAAABBBBBCCCCCDDDDDAAAAAAAAAABBBBBAAAAA";
    string temp;
    vector <datas> data;
    int count = 0; //number of threads needed
    //cin >> s; //Get input from user 
    map<char, double >m;
    map<char, double >::iterator itr;

    for (int i = 0; i < s.length(); i++) 
    {
        m[s[i]]++; //Calcuate frequency 
    }
    for (itr = m.begin(); itr != m.end(); itr++) 
    {
        double p = itr->second/s.length(); //Calculate prob
        data.push_back({itr->first, p}); //Push the pair data into struct
        count++;     
    }
    //Calculate Fx
    for (int i = 0; i < count; i++)
    {
        if (i == 0)
        {
            data[i].fx = 0 + data[i].prob;
            data[i].prevfx = 0;
        }
        else
        {
            data[i].fx = data[i - 1].fx + data[i].prob;
            data[i].prevfx = data[i - 1].fx;
        }
    }
    //Threads creation and join
    pthread_t thread_id[count];

    for (int i = 0; i < count; i++)
    {
        pthread_create(&thread_id[i], NULL, clienthread, &data[i]);
    }

    for (int j = 0; j < count; j++)
    {
        pthread_join(thread_id[j], NULL);
    }
    cout << "SHANNON-FANO-ELIAS Codes:\n";
    for (int i = 0; i < count; i++)
    {
        cout << "Symbol " << data[i].name << ", Code: " << data[i].fxbin << endl;
    }

    return 0;
}
