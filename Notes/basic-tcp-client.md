# Creating a basic TCP client
- create a socket
- check if the socket creation was successful
- set members of sockaddr_in to store the address and port
	- use htons to convert port number from little-endian to big-endian(host byte order to network byte order) 
	- use inet_aton to convert address from little-endian "numbers and dots" format to numeric binary big-endian(host byte order to network byte order) 
- connect to the server
    - check if connection was successful
- send info and receive info
