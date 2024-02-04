#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#define PORT "21202" // for my birthday 02/12/2002
#define IP "10.0.2.15"

int get_and_connect_to_server_socket(char *ip, char *port) {
  // get socket file descriptor
  struct addrinfo server_information;

  // set configuration for getaddrinfo():
  server_information.ai_family = AF_UNSPEC;       // IPv4 or IPv6
  server_information.ai_socktype = SOCK_STREAM;   // TCP Stream Socket
  server_information.ai_flags = AI_PASSIVE;       // automatically fill in host IP

  struct addrinfo *server_address_info;

  printf("Getting host address information... ");

  int result = getaddrinfo(
    ip,
    port,
    &server_information,
    &server_address_info
  );

  // success and error messages
  if (result == 0) {
    printf("Success\n");
  }
  else {
    printf("%s\n", gai_strerror(result));
    exit(EXIT_FAILURE);
  }

  printf("Getting socket file descriptor... ");

  int server_socket = socket(server_address_info -> ai_family, server_address_info -> ai_socktype, server_address_info -> ai_protocol);   // get server socket descriptor

  // success and error messages
  if (server_socket == -1) {
    printf("Failed\n");
    printf("Error Number: %i\n", errno);
    exit(EXIT_FAILURE);
  }
  else { 
    printf("Success\n");
  }

  // connect to server
  
  int connecting = connect(server_socket, server_address_info -> ai_addr, server_address_info -> ai_addrlen);   // establish connection to server socket

  // success and error messages
  if (connecting == -1) {
    printf("Failed\n");
    printf("Error Number: %i\n", errno);
    exit(EXIT_FAILURE);
  }
  else { 
    printf("Success\n");
  }

  return server_socket;
}

void get_connection_details(int server_socket, char *details) {
  printf("Getting connection details... \n");
  recv(server_socket, details, 100, 0);
  printf("%s", details);
}

int main(void) {
  int server_socket;
  char connection_details[100];

  server_socket = get_and_connect_to_server_socket(IP, PORT);
  get_connection_details(server_socket, connection_details);


  // communicate with server
  while (1) {
    char receive_message[100];
    int receiving = recv(server_socket, receive_message, 100, 0);

    printf("%s", receive_message);

    char send_message[100];
    fgets(send_message, sizeof(send_message), stdin);

    int sending  = send(server_socket, send_message, sizeof(send_message), 0);

    // success and error messages
    if (sending == -1) {
      printf("Failed to send message to client\n");
      printf("Error Number: %i\n", errno);
    }
    else {
      printf("Successfully sent %i bytes\n", sending);
    }

    if (strcmp(send_message, "exit\n") == 0) {
      break;
    }
  }

  // close connection

  printf("Closing server socket... ");

  int closing = close(server_socket);             // close server socket to no longer send or recieve on connection with server

  // success and error messages
  if (closing == 0) {
    printf("Success\n");
  }
  else {
    printf("Failed to close server socket on port %s\n", PORT);
    printf("Error Number: %i", errno);
  }
}