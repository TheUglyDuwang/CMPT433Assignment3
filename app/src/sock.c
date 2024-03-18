#include <sys/socket.h>

int main(){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in cliaddr;

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(8089);



    if(bind(sock, (const struct sockaddr *)&cliaddr, sizeof(cliaddr)) < 0){
        printf("No binding");
    }

    socklen_t len = sizeof(cliaddr);

    int i = 0, n;
    char buffer[1024];
    while(i < 50){
        n = recvfrom(sockfd, (char *)buffer, 1024, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("Client received %s\n", buffer);
        sleep(1);
    }    

    return 0;
}