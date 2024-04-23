#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUF_SIZE 1024

int main() {
		printf("Welcome to 'Hello World as a Service'!\n");

		int server_fd, new_socket, valread;
		struct sockaddr_in address;
		int opt = 1;
		int addrlen = sizeof(address);
		char buffer[BUF_SIZE];

		printf("Creating socket... ");
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
				printf("FAILED!\n");
				perror("socket failed");
				exit(EXIT_FAILURE);
		}
		printf("Done!\n");

		printf("Setting socket options... ");
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
					   &opt, sizeof(opt))) {
				printf("FAILED!\n");
				perror("setsockopt");
				exit(EXIT_FAILURE);
		}
		printf("Done!\n");

		printf("Binding socket to port %d... ", PORT);
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);

		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
				printf("FAILED!\n");
				perror("bind failed");
				exit(EXIT_FAILURE);
		}
		printf("Done!\n");

		printf("Listening for clients (Ctrl-c to quit)...\n");
		if (listen(server_fd, 3) < 0) {
				perror("listen");
				exit(EXIT_FAILURE);
		}

		while (true) {
				if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
										 (socklen_t*)&addrlen))<0) {
						printf("FAILED!\n");
						perror("accept");
						exit(EXIT_FAILURE);
				}

				char ip[16];
				inet_ntop(AF_INET, &address.sin_addr, ip, sizeof(ip));

				printf("%s connected\n", ip);
				if (!fork()) {
						close(server_fd);
						if (-1 != write(new_socket, "hello ", 6)) {
								if (-1 != write(new_socket, ip, strlen(ip)))
								{
										if (-1 == write(new_socket, "\n", 1)) {
												perror("write");
												exit(EXIT_FAILURE);
										}
								}
						}
						close(new_socket);
						exit(0);
				}
				close(new_socket);
		}

		close(server_fd);
		exit(0);
}
