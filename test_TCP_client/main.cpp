 /*
 * Name		: main.cpp
 * Author	: Marc David Jensen Blunsdon
 * Version	: 1.0
 * Copyright	: Free for all
 * Description	: A client code, which connect too a socket and
 * 				  sleeps for 15 seconds then sends a message and
 * 				  tries to read a message from server and sleeps for 5 seconds
 * 				  then sends a bye message and closes socket.
 */
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8888

int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char hello[] = "Hello from client";
	char bye[] = "Bye from client";
	char buffer[1024];
	memset (buffer, 0, 1024);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
	sleep (15);
	send(sock , hello , strlen(hello) , 0 );
	printf("Message sent\n");
	// valread = read( sock , buffer, 1024);
	if (read (sock, buffer, 1024)< 0)
	{
		printf("\nFailed to recv!!\n");
		return -1;
	}
	printf("%s\n",buffer );
	sleep(5);
	send(sock , bye , strlen(bye) , 0 );
	close(sock);
	printf("socket closed\n");
	return 0;
}




