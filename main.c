#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(){
    printf("Student Name: Deniz Evrendilek \n");
    printf("Student ID: 301340591 \n\n");

    //create socket
    int socket_udp = socket(AF_INET, SOCK_DGRAM, 0);



    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY; //use the IP of the local machine
    memset(addr.sin_zero, '\0', sizeof addr.sin_zero);

    //bind socket
    bind(socket_udp, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    return 0;
}