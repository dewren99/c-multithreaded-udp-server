// Insprations: 
// https://www.educative.io/edpresso/how-to-implement-udp-sockets-in-c
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// http://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf Chapter 6
// https://coursys.sfu.ca/2021su-cmpt-300-d1/pages/tut_sockets/view
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

    int res;
    const char client_msg[] = "HELLO";

    //create socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_socket < 0){
        printf("Socket could not be created\n");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof server_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY; //use the IP of the local machine

    

    res = sendto(client_socket, client_msg, strlen(client_msg), 0, (const struct sockaddr *)&server_addr, sizeof server_addr);
    if(res < 0){
        printf("Message could not be sent\n");
    }
    printf("Message sent!\n");

    close(client_socket);
    return 0;
}