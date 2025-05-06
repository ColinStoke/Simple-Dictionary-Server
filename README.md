Author: Colin Stokes  
Date: 5/2/2025

# Project Description


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
