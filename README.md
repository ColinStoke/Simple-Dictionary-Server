Author: Colin Stokes  
Date: 5/2/2025

# Project Description

This project was created to fulfull the reuirements for the Socket Programming project in UMBC's CMSC481 Computer Networks class. 

This project serves as an introduction to socket programming. The goal is to create 4 processes which are either dictionary servers (servers who have dictionary word information) or a client process which queries those servers. 

The client process sends individual words to the server process and the server replies with the defenition of the word through berkeley sockets.

Server1 is configured to only accept one client process at a time to communicate, keeping any other clients in a queue or denying them.

Server2 is configured to fork a new server process whenever a client requests connection rather than block or refuse connection.

Server3 is configured to use the select() syscall to probe the sockets and respond to client connection requests and client word defenition requests.

# Instructions For Running

Run make in the terminal to compile all files into executables.

All server* type executables take one command line argument like:

    ./server* portnum 

where portnum is the number of the port to bind to.

To exit a server process, run cntrl+c after closing all client processes currently connected to it.

The client process takes 2 command line arguments:

    ./client ipaddr portnum

where ipaddr is the address of the server process machine and the port number it is listening on.

To exit a client process, type '/exit' into the program when it asks for the next word to send to the server. 
