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

void handleInterrupt(int sig){

    return;

}

int main(int argc, char* argv[]){

    struct sigaction sa = {
    handleInterrupt,
    SA_RESTART
    };

    if (sigaction(SIGINT, &sa, NULL) == -1){
    perror("sigaction() failed");
    exit(EXIT_FAILURE);
    }

    vector<pair<string, string>> dict;

    if(argc != 2){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    int port = atoi(argv[1]);
    int accept_res;
    int bytes_read;
    size_t position;
    size_t chunk_size;

    string response;

    cout << "Server1 Started On Port: " << port << endl;

    parseTsv("dictionary.tsv", dict);

    char client_word[BUF_SIZE];
    int my_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in channel; /* holds IP address */
    channel.sin_family = AF_INET;
    channel.sin_addr.s_addr = htonl(INADDR_ANY); // addr = 0.0.0.0
    channel.sin_port = htons(port);
    int bind_res = bind(my_socket, (struct sockaddr *)&channel, sizeof(channel));
    int listen_res = listen(my_socket, 1);

    while(1){

        cout << "Accepting new connection...\n\n";

        accept_res = accept(my_socket, 0, 0);

        if(accept_res < 0){
            break;
        }

        cout << "Connected with client socket number " << accept_res << "\n\n";

        while(1){

            response = "The server could not find the requested word. Please try another.\n";
            position = 0;
    
            bytes_read = recv(accept_res, client_word, BUF_SIZE, 0);

            if(bytes_read <= 0){
                cout << "Client disconnected\n";
                close(accept_res);
                break;
            }
    
            cout << "Client socket " << accept_res << " sent: " << client_word << "\n\n";
    
            for(size_t i = 0; i < dict.size(); i++){
                if(strncmp(dict[i].first.c_str(), client_word, BUF_SIZE) == 0){
                    response = dict[i].second;
                }
            }
    
            cout << "Sending Reply: " << response << "\n\n";
    
            while(position < response.size()){
                chunk_size = min((size_t) BUF_SIZE, (response.size() + 1) - position);
                send(accept_res, response.c_str() + position, chunk_size, 0); 
                position += BUF_SIZE;
            }
            
        }


    }

    close(accept_res);
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