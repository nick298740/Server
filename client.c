#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int sockfd, n;
int len = sizeof(struct sockaddr);
char recvline[40960];
char buffer[2000];
struct sockaddr_in servaddr;

void readMessage(){
   bzero(buffer,2000);
   n = read( sockfd,buffer,2000 );

   if (n < 0) {
	  perror("ERROR reading from socket");
	  exit(1);
   }
}


void writeMessage(){
   n = write( sockfd,buffer,2000 );
   bzero(buffer,2000);

   if (n < 0) {
	  perror("ERROR reading from socket");
	  exit(1);
   }
}

void connectToServer(){
	    /* AF_INET - IPv4 IP , Type of socket, protocol*/
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr,sizeof(servaddr));
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22006); // Server port number
 
    /* Convert IPv4 and IPv6 addresses from text to binary form */
	inet_pton(AF_INET,"129.120.151.94",&(servaddr.sin_addr));
 
    /* Connect to the server */
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
 
}

int main(int argc,char **argv){
	connectToServer();

	printf("\n\nPlease enter the url to retrieve from the proxy server: \n\n");
	scanf("%s", buffer);
	printf("\n\nHere is the message to write to the buffer: %s\n\n", buffer);
	writeMessage();
		
	readMessage();
	
	printf("\n\nHere is the message from the server: %s\n\n", buffer);

	close(sockfd);
 
}