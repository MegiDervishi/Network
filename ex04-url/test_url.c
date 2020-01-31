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


int main(int argc, char* argv[])
{
	char* url = argv[1];
	url_info info;

	if(!url)
		exit_with_error("Please enter URL");
	parse_url(url, &info);

	print_url_info(info);
	//validate_url(info);

	return (EXIT_SUCCESS);
}


