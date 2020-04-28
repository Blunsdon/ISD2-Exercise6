 /*
 * Name		: main.cpp
 * Author	: Marc David Jensen Blunsdon
 * Version	: 1.0
 * Copyright	: Free for all
 * Description	: A server code, which echos back the received message
 * 				  can handle multiple socket connections with select and
 * 				  fd_set on Linux without the use of threads.
 */

#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define PORT 8888

int main(int argc, char *argv[]) {

	int opt = 1;
	int client_socket[10], max_clients = 10;
	int server_socket, new_socket;
	int activity, i, valread, sd, addrlen, max_sd;

	struct sockaddr_in address;

	// Data buffer
	char buffer[1025];

	// Set of file descriptors
	fd_set readfds;

	// The message from server
	char message[] = "***Server MSG*** \n";

	// Initialise all client_socket[] to 0
	for (i = 0; i < max_clients; i++) {
		client_socket[i] = 0;
	}

	// Create a server socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Set server socket to allow multiple connections
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*) &opt,
			sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	// Addresses for socket
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT);

	// Bind the socket to localhost and port
	if (bind(server_socket, (struct sockaddr*) &address,
			sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	// Info message
	printf("Listener on port %d \n", PORT);

	// Maximum of 3 pending connections for the server socket
	if (listen(server_socket, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	// Ready for incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");

	while (1) {

		// Clear the socket set
		FD_ZERO(&readfds);

		// Add server socket to set
		FD_SET(server_socket, &readfds);
		max_sd = server_socket;

		// Add child sockets to set
		for (i = 0; i < max_clients; i++) {
			// Socket descriptor
			sd = client_socket[i];
			// If valid socket descriptor then add to read list
			if (sd > 0)
				FD_SET(sd, &readfds);
			// Highest file descriptor number, needed for select function
			if (sd > max_sd)
				max_sd = sd;
		}

		// Wait for an activity on sockets, timeout is NULL, so indefinitely
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			printf("select error");
		}

		// If activity on the server socket, its an incoming connection
		if (FD_ISSET(server_socket, &readfds)) {
			if ((new_socket = accept(server_socket,
					(struct sockaddr*) &address,
					(socklen_t*) &addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}

			// Inform user of socket connection
			printf("New connection , socket fd is %d , ip is : %s , "
					"port : %d \n", new_socket, inet_ntoa(address.sin_addr),
					ntohs(address.sin_port));

			// Send new connection message to the new client
			if (send(new_socket, message, strlen(message), 0)
					!= strlen(message)) {
				perror("send");
			}

			puts("Welcome message sent successfully");

			// Add new socket to array of sockets
			for (i = 0; i < max_clients; i++) {
				// If position is empty
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n", i);

					break;
				}
			}
		} else {
			// Else its activity on some connected socket
			for (i = 0; i < max_clients; i++) {
				sd = client_socket[i];

				if (FD_ISSET(sd, &readfds)) {
					//Check if closing and read the incoming message
					if ((valread = read (sd, buffer, 1024)) == 0) {
						// Somebody disconnected , get his details and print
						getpeername(sd, (struct sockaddr*) &address,
								(socklen_t*) &addrlen);
						printf("Host disconnected , ip %s , port %d \n",
								inet_ntoa(address.sin_addr),
								ntohs(address.sin_port));
						// Close the socket and mark as 0 in list for reuse
						close(sd);
						client_socket[i] = 0;
					}else {
						// Echo back the message
						buffer[valread] = '\0';
						send (sd, buffer, strlen(buffer),0);
						memset(buffer, 0, 1024);
						printf("Echo sent to client\n");
					}
				}
			}
		}
	}

	return 0;
}

