#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


//Tori Nguyen
//Proj 3
//CPSC 405
//client.c

typedef struct{
	char name[32];
	int votes;
}Candidate;

Candidate cand[20];

void error(char *msg)
{
	perror(msg);
	exit(0);
}


int numCand; //number of candidates
void fileRead();

int voteCand;//the candidate voted for

int main(int argc, char *argv[]){
	int i;
	fileRead();

	int sockfd, portno, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        char buffer[256];

        if (argc < 2)
        {
                fprintf(stderr,"usage %s hostname\n", argv[0]);
                exit(0);
        }


	portno = 54013;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd<0){
		error("ERROR opening socket");
	}
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,&serv_addr, sizeof(serv_addr))<0){
		error("ERROR connecting");
	}


	for(i = 0; i<numCand; i++){
		printf("%d %s", i, cand[i].name);
	}
	printf("Please enter the number of the candidate that you wish to vote for: ");
	/*	scanf("%d", &voteCand);
		while(voteCand < 0 || voteCand >= numCand){
		printf("Please enter a valid candidate number:\n");
		scanf("%d", &voteCand);
		}
		*/	
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0)
	{
		error("ERROR writing to socket");
	}
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0)
	{
		error("ERROR reading from socket");
	}
	printf("%s\n",buffer);

	return 0;


}


void fileRead(){
	int i;
	char eatUp[32];//eats up first input in txt
	FILE *candFile = fopen("candidates.txt", "r");

	if(candFile == NULL){
		printf("Input file not foudn! \n");
		exit(0);
	}
	else{
		fscanf(candFile, "%d", &numCand);
		fgets(eatUp, 32, candFile);
		for(i=0; i<numCand; i++){
			fgets(cand[i].name, 32, candFile);
		}
		fclose(candFile);
	}
}
