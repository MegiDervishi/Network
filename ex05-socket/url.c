/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"url.h"



/**
 * parse a URL and store the information in info.
 */

void parse_url(char* url, url_info *info)
{
	// url format: [http://]<hostname>[:<port>]/<path>
	// e.g. https://www.polytechnique.edu:80/index.php
	char *protocol, *host_name_path;

	protocol = strstr(url, "://");
	// it searches :// in the string. if exists,
	//return the pointer to the first char of the match string


	if(protocol){ //protocol type exists
		*protocol = '\0'; //end of the protocol type
		host_name_path = protocol + 3; //jump to the host name
	} else {	//no protocol type: using http by default.
		host_name_path = url;
		url = "http";
	}

	if (strcmp(url, P_HTTP) == 0) {
		info->protocol = url;
	} else {
		exit_with_error("Wrong protocol type. For the moment, only http is supported");
	}

//	printf("host_name_path: %s\n", host_name_path);
	char *token1 = strtok(host_name_path, ":"); //search the ":" in the host_name_path
	char *token2 = strtok(NULL, ":"); //putting the first argument as NULL means continuing the same search

	if(token2) {//port number exists
		info->host = token1; //host
		char *port_token = strtok(token2, "/"); //get the port number
		info->port = atoi(port_token);
	} else { // no port number
		info->port = 80; //use 80 by default
		char *host_token = strtok(token1, "/");
		info->host = host_token;
	}

	char *path_token = strtok(NULL, ""); //all the rest; get the path

	if(path_token) { //if the path exists
		char* path_string = malloc(sizeof(char)*(strlen(path_token) +1));
		strncpy(path_string, "/", 1);
		strncat(path_string, path_token, strlen(path_token));

		info->path = path_string;
	} else { // using the "/" if no path is specified.
		char* path_string = malloc(sizeof(char)*2);
		strncpy(path_string, "/", 1);
		info->path = path_string;
	}
}

/**
 * print the url info to std output
 */
void print_url_info(url_info info){
	printf("The URL contains following information: \n");
	printf("Protocol type:\t%s\n", info.protocol);
	printf("Host name:\t%s\n", info.host);
	printf("Port No.:\t%d\n", info.port);
	printf("Path:\t\t%s\n", info.path);
}

/**
 * exit with an error message
 */

void exit_with_error(char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/**
 * free the memory
 */
void free_url_info(url_info *info){
	free(info->path);
}
