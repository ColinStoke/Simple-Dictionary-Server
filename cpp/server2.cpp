#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#define BUF_SIZE 4096	 /* block transfer size */
using namespace std;

void parseTsv(const string inFile, vector<pair<string, string>>& dict);

//does nothing but override sigint
void handleInterrupt(int sig){

    return;

}

int main(int argc, char* argv[]){

    //handles interrupts
    struct sigaction sa = {
    handleInterrupt,
    SA_RESTART
    };

    if (sigaction(SIGINT, &sa, NULL) == -1){
    perror("sigaction() failed");
    exit(EXIT_FAILURE);
    }

    vector<pair<string, string>> dict; //dictionary for parsing

    if(argc != 2){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    int port = atoi(argv[1]); //port number passed on command line
    int socket_fd; //fd of the socket
    int bytes_read; //num of bytes read from the client
    size_t position; //write position to keep track of how many bytes to write
    size_t chunk_size; //int to keep track of the biggest number of bytes to send back to the client

    string response; //full response back to the client
    char client_word[BUF_SIZE]; //word the client sent to us

    parseTsv("dictionary.tsv", dict);

    int my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in channel; /* holds IP address */
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY); // addr = 0.0.0.0
    channel.sin_port = htons(port);

    int bind_res = bind(my_socket, (struct sockaddr *)&channel, sizeof(channel));

    cout << "Server2 Started On Port: " << port << endl;

    int listen_res = listen(my_socket, 10);

    //main server controller loop
    while(1){

        cout << "Listening for new connections...\n\n";

        socket_fd = accept(my_socket, 0, 0); //wait for connections

        if(socket_fd < 0){
            goto exit;
        }

        //fork a new server
        pid_t child = fork();

        if(child < 0){
            cout << "Fork error\n";
            exit(-1);
        }
        else if(child == 0){
            cout << "Detected connection. Forking process " << getpid() << "\n\n";
            goto mainloop;
        }
        
        sleep(1); //to let the child message play first and line things up nicely
    }

mainloop: 

    //slave child loop
    while(1){

        response = "The server could not find the requested word. Please try another.";
        position = 0;

        bytes_read = recv(socket_fd, client_word, BUF_SIZE, 0); //recieve client word

        if(bytes_read <= 0){
            cout << "Client disconnected\n";
            close(socket_fd);
            break;
        }

        cout << "Client socket " << socket_fd << " sent: " << client_word << "\n\n";

        //find defenition
        for(size_t i = 0; i < dict.size(); i++){
            if(strncmp(dict[i].first.c_str(), client_word, BUF_SIZE) == 0){
                response = dict[i].second;
            }
        }

        cout << "Sending Reply: " << response << "\n\n";

        //send reply
        while(position < response.size()){
            chunk_size = min((size_t) BUF_SIZE, (response.size() + 1) - position);
            send(socket_fd, response.c_str() + position, chunk_size, 0); 
            position += BUF_SIZE;
        }
        
    }

exit:

    close(socket_fd);
    return 0;
}

void parseTsv(const string inFile, vector<pair<string, string>>& dict){

    string line;

    ifstream file(inFile);

    stringstream ss;

    pair<string, string> temp;

    if(!file){
        cout << "Error opening file.\n";
        exit(-1);
    }

    while(!file.eof()){
        getline(file, line, '\n'); //get full line

        ss.clear();
        ss.str(line); //put line into stringstream

        getline(ss, temp.first, '\t'); //get word

        getline(ss, temp.second, '\n'); //get defenition

        dict.push_back(temp); //put into main dict object
    }

}