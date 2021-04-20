#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <mutex>
#include <unistd.h>
#include <string.h>


typedef struct{
	char name[32];
	int votes;
}Candidate;

Candidate cand[20];
//mutex mut, mut2;
pthread_mutex_t mut;
void error(char *msg)
{
perror(msg);
exit(1);
}


int numCand;
void fileRead();
void addVote(int index);
void cleanUp();
void *connectionHandler(void *);

        int sockfd, newsockfd, portno, clilen, *new_sock;
        char buffer[256];
        char buffer_votos[4048];
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        int voteSel; 


main(int argc, char *argv[]){
	int i;
	fileRead();


/*	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}       
*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) 
	{               
		error("ERROR opening socket");
	}       

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = 54013;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		error("ERROR on binding");
	} 



	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	while(1){
		//need to kill other clients

		signal(SIGINT, cleanUp); //catch ctrl-c and cleanUp

		while(newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen)){

	
		pthread_t s_thread;
		new_sock = malloc(1);
		*new_sock = newsockfd;
		if(pthread_create(&s_thread, NULL, connectionHandler, (void*) new_sock)){
			error("Could not create thread");
		}
		pthread_join(s_thread,NULL);
		}

		if (newsockfd < 0)
		{
			error("ERROR on accept");

		}

//	}
}
	close(sockfd);
	close(newsockfd);	

}


void fileRead(){
	int i;
	char eatUp[32];
	FILE *candFile = fopen("candidatos.txt", "r");

	if(candFile == NULL){
		printf("Arquivo nao encontrado. \n");
		exit(0);
	}
	else{
		fscanf(candFile, "%d", &numCand);
		fgets(eatUp, 32, candFile);
		for(i=0; i<numCand; i++){
			fgets(cand[i].name, 32, candFile);
			cand[i].votes = 0;
		}
		fclose(candFile);
	}
}

void addVote(int index){
//	mut2.lock();
		pthread_mutex_lock(&mut);;
	cand[index].votes +=1;
		pthread_mutex_unlock(&mut);;
//	mut2.unlock();
}

void cleanUp(){
	int i;
	printf("\n");
	for(i=0; i<numCand; i++){
		printf("%s %d voto(s).\n", cand[i].name, cand[i].votes);
	}

	pthread_mutex_destroy(&mut);	
	exit(0);
}

void *connectionHandler(void *sockfd){
		//pthread_mutex_lock(&mut);;
		bzero(buffer,256);
                bzero(buffer_votos,256);

		n = read(newsockfd,buffer,255);

		if (n < 0)
		{
			error("ERROR reading from socket");
		}
		voteSel = atoi(buffer);
                
                if (voteSel == -1) {
                    for(int i=0; i<numCand; i++){
                        bzero(buffer,256);
                        sprintf(buffer, "%s %d voto(s)\n",cand[i].name, cand[i].votes);
                        strcat(buffer_votos, buffer);
                    }
                    n = write(newsockfd,buffer_votos,4048);
                }
                else {
            
                    addVote(voteSel);
                    n = write(newsockfd,"\nVOTO COMPUTADO\n",15);

                }

		if (n < 0)
		{
			error("ERROR writing to socket");
		}
		//pthread_mutex_unlock(&mut);
}
