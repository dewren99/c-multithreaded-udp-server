#include <stdio.h>
#include <sys/socket.h> //socket
#include <netinet/in.h>
#include <arpa/inet.h> //inet_ntoa
#include <string.h> //memset
#include <unistd.h> //close

#define PORT 8080

int main(){
    printf("[CLIENT] \n");
    printf("Student Name: Deniz Evrendilek \n");
    printf("Student ID: 301340591 \n\n");

    const char client_msg[] = "HELLO";

    //create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; //use the IP of the local machine

    memset(&server_addr, '\0', sizeof server_addr);

    sendto(client_socket, client_msg, strlen(client_msg), 0, (const struct sockaddr *)&server_addr, sizeof server_addr);

    close(client_socket);
    return 0;
}