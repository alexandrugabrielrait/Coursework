#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <list>
#include "requests.h"
#include "buffer.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define HOST_IP "3.8.116.10"
#define PORT 8080

#define RESP_OK 200
#define RESP_CREATED 201
#define RESP_UNAUTHORIZED 401
#define RESP_TOO_MANY 429
#define RESP_INT_SERV_ERR 500

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)

#define BUFLEN 4096
#define LINELEN 1000

extern int sockfd;

// daca clientul este conectat la server
extern bool connected;

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char *host_ip, int portno,
                                        int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

// comenzi
void register_user(string username, string password);
void login(string username, string password);
void enter_library();
void get_books();
void get_book(int id);
void add_book(string title, string author, string genre,
                                            string publisher, int page_count);
void delete_book(int id);
void logout(bool silent);

// ia id-ul mesajului de raspuns si il transforma in int
int get_response_id(char *message);

// intoarce primul cookie din mesaj
string get_cookie(char *message);

// intoarce continutul json al mesajului
json get_json_content(char *message);