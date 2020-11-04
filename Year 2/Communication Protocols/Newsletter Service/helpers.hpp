#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <string.h>
#include <cstdio>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

typedef __uint32_t uint32_t;
typedef __uint16_t uint16_t;
typedef __uint8_t uint8_t;
typedef __int8_t int8_t;

#define DIE(assertion, call_description)  \
	do                                    \
	{                                     \
		if (assertion)                    \
		{                                 \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(call_description);     \
			exit(EXIT_FAILURE);           \
		}                                 \
	} while (0)

#define BUFLEN 700 //lungime buffer
#define MAX_CLIENTS 5 //numarul maxim de clienti in asteptare

typedef struct
{
	int8_t sf;
	string id;
} subscriber;

#define TOPICLEN 50 //lungime topic
#define CONTENTLEN 1500 //lungime continut

//mesaj trimis de la un client udp la server
typedef struct
{
	char topic[TOPICLEN];
	uint8_t data_type;
	char content[CONTENTLEN];
} notification;

//creaza un sir de caractere in functie de tipul si continutul notificarii
char *parse_notification_value(notification notif);

//reprezinta numele tipurilor de date din notificare
extern const char *identifier_name[];

//marimea maxima pentru fiecare tip de date
extern const int data_size[];

//mesaj trimis de la server la subscriber
typedef struct
{
	// -2 - id-ul a fost luat deja
	// -1 - inteogheaza id-ul clientului
	// 0 - notificare
	// 1 - subscribe
	// 2 - unsubscribe
	int8_t message_type;
	struct sockaddr_in source;
	notification notif;
} server_message;

//offset de dupa data_type de la server_message
const int offset_after_type = offsetof(server_message, notif)
	+ offsetof(notification, data_type) + 1;

//marimea pentru tipul de mesaj primit de la server
int smsg_size(int8_t message_type, int8_t data_type);

//mesaj trimis de la subscriber la server
typedef struct
{
	// -1 - id-ul meu este...
	// 0 - subscribe SF = 0
	// 1 - subscribe SF = 1
	// 2 - unsubscribe
	int8_t message_type;
	// client_id/topic
	char content[TOPICLEN];
} client_message;

//marimea maxima pentru tipul de mesaj primit de la subscriber
int cmsg_size(int8_t i);

//buffer in care serverul stocheaza informatiile primite de la un subscriber
typedef struct
{
	int current_bytes;
	char new_bytes_buffer[sizeof(client_message)];
	client_message msg;
} input_buffer;

#endif
