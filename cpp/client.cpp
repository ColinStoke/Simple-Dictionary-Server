
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#define BUF_SIZE 4096	 /* block transfer size */
using namespace std;

int main(int argc, char* argv[]){

    if(argc != 3){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    cout << "IP: " << argv[1] << " Port: " << argv[2] << endl;

    int port = atoi(argv[2]); //get port num

    string word; //word for the server to define
	string response; //server response

    int connect_res; //return of connect() function
	int socket_fd; //fd of socket
	char c_response[BUF_SIZE];	/* buffer for incoming file */
	struct hostent *h;			/* info about server */
	struct sockaddr_in channel; /* holds IP address */

	h = gethostbyname(argv[1]); /* look up host's IP address */
	if (!h)
	{
		printf("gethostbyname failed to locate %s\n", argv[1]);
		exit(-1);
	}

	socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (socket_fd < 0){
		cout << "Socket call failed\n";
		exit(-1);
	}

	memset(&channel, 0, sizeof(channel));
	channel.sin_family = AF_INET;
	// NOTE: below line was changed from original textbook:
	// memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
	memcpy(&channel.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	channel.sin_port = htons(port);
	connect_res = connect(socket_fd, (struct sockaddr *)&channel, sizeof(channel));
	if(connect_res < 0){
		cout << "Error connecting to the socket\n";
		exit(-1);
	}

    cout << "Connected to server\n\n";

	while (1){
		response = "";

		cout << "Please enter the word you need defined: ";
        cin >> word;

		if(word == "/exit"){
			break;
		}

		//send the word to the server
		send(socket_fd, word.c_str(), word.size() + 1, 0);

		//revieve chunks of BUF_SIZE from the server until the amount of bytes recieved is less than that, indicating the server is done.
		while(recv(socket_fd, c_response, BUF_SIZE, 0) == BUF_SIZE){
			response.append(c_response);
		}

		response.append(c_response);

		cout << "Server replied: " << response << "\n\n";

	}

	close(socket_fd);

    return 0;
}