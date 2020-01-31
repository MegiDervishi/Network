#ifndef URL_H
#define URL_H

/* information of an URL*/
struct url_info
{
	char* url; //full url
	char* protocol; // protocol type: http, ftp, etc...
	char* host; // host name
	int port; 	//port number
	char* path; //path
};
typedef struct url_info url_info;

static const char P_HTTP[] = "http";

void parse_url(char* url, url_info *info);

void exit_with_error(char* message);

void print_url_info(url_info info);

#endif //URL_H
