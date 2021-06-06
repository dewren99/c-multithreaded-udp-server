#include <stdio.h>
#include <sys/socket.h> //socket
#include <netinet/in.h>
#include <arpa/inet.h> //inet_ntoa
#include <string.h> //memset
#include <unistd.h> //close

#define PORT 8080
#define CONN_QUEUE 5

int main(){
    printf("[SERVER] \n");
    printf("Student Name: Deniz Evrendilek \n");
    printf("Student ID: 301340591 \n\n");

    //create socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in client_addr, server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; //use the IP of the local machine

    memset(&server_addr, '\0', sizeof server_addr);
    memset(&client_addr, '\0', sizeof client_addr);

    //bind socket
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof server_addr);

    listen(server_socket, CONN_QUEUE);

    int msg_recieved;
    char buf[1024];

    while(1){
        printf("[SERVER] waiting for connections...\n\n");
    
        //busy wait
        msg_recieved = recvfrom(server_socket, buf, 1024, 0, (struct sockaddr *)&client_addr, (int *)sizeof client_addr);

        const char* client_ipv4 = inet_ntoa(client_addr.sin_addr);
        const uint16_t client_port = ntohs(client_addr.sin_port);
        printf("[SERVER] received message from address: %s:%d\n", client_ipv4, client_port);
        printf("[SERVER] message recieved: %s\n", buf);
    }

    close(server_socket);
    return 0;
}