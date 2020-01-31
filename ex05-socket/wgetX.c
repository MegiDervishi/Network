/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "url.h"
#include "wgetX.h"

int main(int argc, char* argv[])
{

	url_info info;


	if (argc != 2) {
		exit_with_error("The wgetX must have exactly 1 parameter as input. \n");
	}
	char *url = argv[1];

	printf("Downloading %s \n", url);

	//get the url
	parse_url(url, &info);
	//print_url_info(info);

	//download page
	char *recv_buf_t;
	recv_buf_t = malloc(sizeof(char)*B_SIZE);
	bzero(recv_buf_t, sizeof(recv_buf_t));
	char *buff = download_page(info, recv_buf_t);
	puts(buff);
	//write to the file
	write_data("received_page", buff);

	free(recv_buf_t);

	puts("the file is saved in received_page.");
	return (EXIT_SUCCESS);
}

char* download_page(url_info info, char *recv_buf_t)
{	//this helped: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
	
	/*struct sockaddr_in destination;
	//printf("this is a request: %s",request);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&destination, '0', sizeof(destination));*/

	int sockfd;  
	struct addrinfo hints, *results, *p;
	int rv;
	char *request = http_get_request(info.path, info.host);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	char string_port[6] = { 0 }; //ill change this dont worry LOAN its just to see if it works okay!! im not a cheater
	sprintf(string_port, "%d", info.port);

	if ( (rv = getaddrinfo(info.host , string_port, &hints , &results)) != 0){ //error msg
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    	exit(1);
	}
	// loop through all the results and connect to the first we can
	for(p = results; p != NULL; p = p->ai_next) {
    	if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
        	perror("socket");
        	continue;
    	}
    	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        	perror("connect");
        	close(sockfd);
        	continue;
    	}
    	break; // if we get here, we must have connected successfully
	}
	if (p == NULL) {
    // looped off the end of the list with no connection
    	fprintf(stderr, "failed to connect\n");
    	exit(2);
	}
	send(sockfd, request, strlen(request), 0);
	int length = 0;
	int cl;
	char *originalbuff = recv_buf_t;

	while(1) {
		recv_buf_t = recv_buf_t + length;
		//*(recv_buf_t + length) = '\0';
		cl = recv(sockfd, recv_buf_t , B_SIZE , 0);
		if (cl <= 0) break;
		length += cl;
	}
	*(recv_buf_t + length) = '\0';
	close(sockfd);
	return originalbuff;
}

void write_data(const char *path, const char *data)
{
	FILE *myfile = fopen(path, "w+");
	if (myfile == NULL) exit_with_error("Couldn't open file");
	fputs(data, myfile);
	fclose(myfile);
}

char* http_get_request(char* path, char* host) {
	//char request_buffer[1024];
	char * request_buffer = (char *) malloc(1024);
	memset(request_buffer, 0, sizeof(*request_buffer));
	snprintf(request_buffer, 1024, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
			path, host);
	return request_buffer;
}

char* read_http_reply(char* recv_buf_t) {
	//first line, get the status code
	char *status_line = strstr(recv_buf_t, "\r\n");
	*status_line = '\0';
	//		puts(recv_buf_t);
	char status[4];
	memcpy(status, recv_buf_t + 9, 3); //get the status string
	status[3] = '\0';
	int status_code = atoi(status);

	recv_buf_t = status_line + 2; //now move to the next line.

	char* page;
	switch(status_code){
	case 200: //all ok

		page = strstr(recv_buf_t, "\r\n\r\n");
		*page = '\0';
		recv_buf_t = page + 4; //now the recv_buf pointer is pointing to the begin of the document
		break;
	case 302: //redirect
		//do the redirect here
		break;

	}
	return recv_buf_t;
}
