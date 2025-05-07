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

volatile sig_atomic_t sigint_received = 0; //global var to control main loop before and after cntrl c

void handleInterrupt(int sig){

    sigint_received = 1; //signal loop end

}

int main(int argc, char* argv[]){

    //sigint stuff
    struct sigaction sa = {
    handleInterrupt,
    };

    if (sigaction(SIGINT, &sa, NULL) == -1){
    perror("sigaction() failed");
    exit(EXIT_FAILURE);
    }

    vector<pair<string, string>> dict; //dictionary for parsing

    vector<int> socket_fds; //vector for holding all valid socket fds

    if(argc != 2){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    struct timeval selectTime; //time struct for timeouts

    int port = atoi(argv[1]); //port num passed by command line
    int select_ret; //return value for all select() statements
    int accept_res; //fd from accept()
    int bytes_read; //number of bytes read from the client word
    int max_fd;     //max fd from all valid accepted fds for accept()
    size_t position; //write position to keep track of how many bytes to write
    size_t chunk_size; //int to keep track of the biggest number of bytes to send back to the client

    string response; //full response back to the client
    char client_word[BUF_SIZE]; //word the client sent to us

    cout << "Server1 Started On Port: " << port << endl;

    parseTsv("dictionary.tsv", dict);

    int my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in channel; /* holds IP address */
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY); // addr = 0.0.0.0
    channel.sin_port = htons(port);
    int bind_res = bind(my_socket, (struct sockaddr *)&channel, sizeof(channel));
    int listen_res = listen(my_socket, 1);

    fd_set read_fds; //fd_set object for holding all fds to check with accept()

    while(!sigint_received){

        max_fd = 0; //reset vars

        //reset all select() objects
        selectTime.tv_sec = 0; 
        selectTime.tv_usec = 1000; //.001 seconds
        FD_ZERO(&read_fds);
        FD_SET(my_socket, &read_fds);

        select_ret = select(my_socket + 1, &read_fds, NULL, NULL, &selectTime); //probe socket for incoming connections

        if(select_ret == -1 && !sigint_received){
            cout << "Select() error during accept() check\n";
        }
        
        //if we find connections, accept them all
        for(int i = 0; i < select_ret; i++){
            accept_res = accept(my_socket, 0, 0);
            cout << "Accepted new connection with socket " << accept_res << "\n\n";
            socket_fds.push_back(accept_res);
        }

        //reset select() vars
        selectTime.tv_sec = 0;
        selectTime.tv_usec = 1000; //.001 seconds
        FD_ZERO(&read_fds);

        //load fd_set with all valid sockets
        for(size_t i = 0; i < socket_fds.size(); i++){

            if(socket_fds[i] > max_fd){
                max_fd = socket_fds[i];
            }

            FD_SET(socket_fds[i], &read_fds);
        }

        response = "The server could not find the requested word. Please try another.\n";
        position = 0;

        select_ret = select(max_fd + 1, &read_fds, NULL, NULL, &selectTime); //probe all valid sockets 

        if(select_ret == -1 && !sigint_received){
            cout << "Select() error during recv() check\n";
            continue;
        }

        //check all sockets
        for(size_t i = 0; i < socket_fds.size(); i++){

            //if the socket is in the list of sockets with data to read, read the data and respond
            if(FD_ISSET(socket_fds[i], &read_fds)){ 

                bytes_read = recv(socket_fds[i], client_word, BUF_SIZE, 0);

                if(bytes_read <= 0){
                    cout << "Client disconnected\n";
                    close(socket_fds[i]);
                    socket_fds.erase(socket_fds.begin() + i);
                    break;
                }

                cout << "Client socket " << socket_fds[i] << " sent: " << client_word << "\n\n";

                //find defenition
                for(size_t i = 0; i < dict.size(); i++){
                    if(strncmp(dict[i].first.c_str(), client_word, BUF_SIZE) == 0){
                        response = dict[i].second;
                    }
                }

                cout << "Sending Reply: " << response << "\n\n";

                //reply to client
                while(position < response.size()){
                    chunk_size = min((size_t) BUF_SIZE, (response.size() + 1) - position);
                    send(socket_fds[i], response.c_str() + position, chunk_size, 0); 
                    position += BUF_SIZE;
                }
            }
        }

    }

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