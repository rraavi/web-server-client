/*
 * Client.c
 *
 *  Created on: Sep 18, 2015
 *      Author: Raghu
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
	perror(msg);
}
/*
 * This function maps 'p' with 'keep-alive' and 'np' with 'close',
 * sends the  details to the server through socket
 * receives the response back from socket and prints it.
 */
void sendreq(char *filename, char *conntype, int sockfd)
{
	int n, by_rv, i, sz;
	char buffer_s[2000], buffer_r[2000];
	bzero(buffer_s,2000);
	bzero(buffer_r,2000);
	if(strncmp(conntype,"np",strlen(conntype))==0)
		strcpy(conntype,"close");
	else if(strncmp(conntype,"p",strlen(conntype))==0)
		strcpy(conntype,"keep-alive");
	else
	{
		printf("Enter either 'p' for Persistent and 'np' for Non-Persistent Connections");
		exit(0);
	}
	/*
	 * sends the details to the server using send() call
	 */
	printf("Sending the details to the server, they are: %s %s\n", filename, conntype);
	sprintf(buffer_s,"GET /%s HTTP/1.1\r\nHost: \r\nConnection: %s\r\n\r\n", filename, conntype);
	n = send(sockfd, buffer_s, strlen(buffer_s),0);
	printf("Details sent\n");
	if(n<0)
	{
		error("ERROR SENDING TO THE SOCKET");
		exit(0);
	}
	by_rv = read(sockfd, buffer_r, 2000);
	if(by_rv<0)
	{
		error("ERROR RECEIVING FROM THE SOCKET");
	}
	printf("\nBytes received %d\n",by_rv);
	printf("%s", buffer_r);
}

int main(int argc, char *argv[])
{
	int sockfd, pno;
	char filename[100], conntype[100];
	char buf[2000];
	/*
	 * the structure is defined in <netinet/in.h>
	 */
	struct sockaddr_in s_addr;
	FILE *fp;
	/*
	 * checks for the number of arguments passed
	 */
	if (argc < 5)
	{
		fprintf(stderr,"enter %s Hostaddr Portno Connection-Type Filename\n",argv[0]);
	}
	pno = atoi(argv[2]);
	strcpy(filename,argv[4]);
	puts(filename);
	strcpy(conntype,argv[3]);
	puts(conntype);
	/*
	 * creating the socket
	 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
	{
		error("ERROR WHILE OPENING SOCKET");
		exit(0);
	}
	/*
	 * address/port structure
	 */
	bzero((char *) &s_addr, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[2]));
	s_addr.sin_addr.s_addr = inet_addr(argv[1]);
	/*
	 * connects to the server
	 */
	if(connect(sockfd,(struct sockaddr *)&s_addr, sizeof(s_addr))<0)
	{
		error("ERROR WHILE CONNECTING");
		exit(0);
	}
	bzero(buf,2000);
	fp=fopen(filename,"r");
	if(fp==NULL)
		printf("File not found on the server side/n");
	else
	{
	/*
	 * checks for the persistent connections or non persistent connections
	 */
	if(strncmp(conntype,"p", strlen(conntype))==0)
	{
		while (fgets(buf, 2000, fp)!= NULL )
		{
			strcpy(conntype,"p");
			sendreq(buf,conntype, sockfd);
		}
		fclose(fp);
		strcpy(conntype,"np");
		sendreq('\0',conntype, sockfd);
	}
	else if(strncmp(conntype,"np", strlen(conntype))==0)
	sendreq(filename, conntype, sockfd);
	close(sockfd);
	}
	return 0;
}
