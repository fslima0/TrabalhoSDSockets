#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
     
    struct candidato {
        char id;
        int votos;
    } c[1];
    
     c[0].id = '1';
     c[0].votos = 50; 

    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = 1025;
    
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) 
             error("ERROR on binding");

    listen(sockfd,5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, &clilen);
    
    if (newsockfd < 0) 
        error("ERROR on accept");

    printf("server: got connection from %s port %d\n",
           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
     
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
     
    if (!strcmp(buffer, "E")) {
        n = write(newsockfd,"1. Felipe Lima",255);
        
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        if (!strcmp(buffer, "1")) {
            c[0].votos += 1;
            printf("Nome: Felipe Lima - Votos atuais: %d\n", c[0].votos);
        }
    }
    else if (!strcmp(buffer, "O")) {
        for (int i = 0; i <= 1; i++) {
            n = write(newsockfd,"1. Felipe Lima - Numero total de votos: 41",255);

        }
    }   

     close(newsockfd);
     close(sockfd);
     return 0; 
}
