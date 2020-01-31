/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 */

#ifndef WGETX_H_
#define WGETX_H_

#define B_SIZE 1024 * 5000

/**
 * \brief write the content to a file
 * \param path the path and name of the file
 * \param data the pointer of the buffer that to be written.
 */
void write_data(const char *path, const char *data);

/**
 * \brief download a page for a file through http protocol
 * \param info the url information
 * \param buff the buffe for keeping the downloaded file
 * \return the pointer to the downloaded file
 */
char* download_page(url_info info, char *buff);

/**
 * \brief return astring with a get http request 
 * \param path the url path
 * \param host the url host
 * \return the pointer to the get http request
 */
char* http_get_request(char* path, char* host);

/**
 * \brief process the http reply from server
 * \param recv_buf_t pointer to the reply buffer
 * \return the pointer to the requested doc
 */
char* read_http_reply(char* recv_buf_t);

#endif /* WGETX_H_ */
