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
	//Force http so getadrinfo works for wgetX.c
	if (strcmp(url, "http") != 0){
		exit_with_error("Use http");
	} else {
		info->protocol = url;
	}
	/*
		(To be completed: store info->protocol)	
	*/
	//info->protocol = url;

	//printf("host_name_path: %s\n", host_name_path);
	char *token1 = strtok(host_name_path, ":"); //search the ":" in the host_name_path
	char *token2 = strtok(NULL, ":"); //putting the first argument as NULL means continuing the same search
	
	/*
		(To be completed: store info->host and info->port)	
	*/

    if(token2) {
        info->host = token1; 
        char *tmp = strtok(token2, "/");
        info->port = atoi(tmp);
		//checks if the port is actually a number due to atoi() ambiguity 
		//e.g atoi("hello") = atoi("0")
		if (info->port == 0){
			for (int i=0; i<strlen(tmp); i++){
				if (isdigit(tmp[i]) == 0){
					exit_with_error("INVALID\n");
				}
			}			
		}
    } else { //if theres no port number use the defaults
        info->port = 80;
        char *host= strtok(token1, "/");
        info->host = host;
    }
	
	char *path_token = strtok(NULL, ""); //all the rest; get the path
	/*
		(To be completed: store info->path)	
	*/
	//printf("%c",path_token);
	if(path_token) { 
		// string = "/" + path
		char* string = (char *)calloc(strlen(path_token)+1, sizeof(char));
		strncpy(string, "/", 1);
		strncat(string, path_token, strlen(path_token));
		info->path = string;
	} else { //if path is emptu string
		char* string = (char *)calloc(1, sizeof(char));
		strncpy(string, "/", 1); //add "/" to make it valid
		info->path = string;
	}
	printf("\nVALID\n"); //validate url
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
