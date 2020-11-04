#include "helpers.hpp"

void usage(char *file)
{
	fprintf(stderr, "Usage: %s client_id server_address server_port\n", file);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, n, ret;
	struct sockaddr_in serv_addr;
	char buffer[BUFLEN];
	server_message smsg;
	client_message cmsg;
	char *client_id;
	int current_bytes = 0;

	fd_set read_fds;
	fd_set tmp_fds;

	if (argc != 4)
	{
		usage(argv[0]);
	}

	if (strlen(argv[1]) > 10)
	{
		printf("Client id is too long\n");
		return 0;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");

	client_id = argv[1];
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	int enable = 1;
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
												&enable, sizeof(int)) == -1)
	{
		perror("setsocketopt");
		exit(1);
	}

	ret = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");

	FD_SET(0, &read_fds);
	FD_SET(sockfd, &read_fds);

	uint8_t client_online = 1;

	while (client_online)
	{
		tmp_fds = read_fds;

		ret = select(sockfd + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");
		if (FD_ISSET(0, &tmp_fds))
		{
			//citire de la stdin
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);
			strtok(buffer, " \n");
			if (strcmp(buffer, "exit") == 0)
			{
				break;
			}
			else if (strcmp(buffer, "subscribe") == 0)
			{
				char *topic = strtok(NULL, " \n");
				char *sf = strtok(NULL, " \n");
				if (sf == NULL)
				{
					printf("Usage: subscribe topic sf\n");
				}
				else
				{
					if (strcmp(sf, "0") && strcmp(sf, "1"))
					{
						printf("Invalid sf value: %s. Must be 0 or 1\n", sf);
					}
					else
					{
						cmsg.message_type = sf[0] - '0';
						memset(cmsg.content, 0, 50);
						strcpy(cmsg.content, topic);
						n = send(sockfd, &cmsg,
											cmsg_size(cmsg.message_type), 0);
						DIE(n < 0, "send");
					}
				}
			}
			else if (strcmp(buffer, "unsubscribe") == 0)
			{
				char *topic = strtok(NULL, " \n");
				if (topic == NULL)
				{
					printf("Usage: unsubscribe topic\n");
				}
				else
				{
					cmsg.message_type = 2;
					memset(cmsg.content, 0, 50);
					strcpy(cmsg.content, topic);
					n = send(sockfd, &cmsg, cmsg_size(cmsg.message_type), 0);
					DIE(n < 0, "send");
				}
			}
			else
			{
				printf("Invalid command: %s\n", buffer);
			}
		}
		if (FD_ISSET(sockfd, &tmp_fds))
		{
			//mesaj de la server
			memset(buffer, 0, BUFLEN);
			int bytes_received = recv(sockfd, buffer, BUFLEN, 0);
			DIE(bytes_received < 0, "recv");
			if (bytes_received == 0)
			{
				// conexiunea s-a inchis
				printf("Server has been closed.\n");
				break;
			}
			else
			{
				while(1)
				{
					//daca mesajul curent e gol, primul byte la cel din buffer
					if (current_bytes == 0)
						smsg.message_type = buffer[0];
					//calculeaza dimensiunea corespunzatoare tipului
					int size = smsg_size(smsg.message_type, 3);
					if (size == 1)
					{
						//marime 1
						memmove(buffer, buffer + 1, 1);
						bytes_received--;
					}
					else if (smsg.message_type > 0)
					{
						//mesaj fara continut
						if (current_bytes + bytes_received >= size)
						{
							int bytes_remaining = bytes_received - size
															+ current_bytes;
							memcpy(((char *)(&smsg)) + current_bytes, buffer,
														size - current_bytes);
							current_bytes = size;
							bytes_received = bytes_remaining;
						}
					}
					else if (current_bytes + bytes_received
											>= offset_after_type)
					{
						//mesaj cu continut, trece de data_type
						if (current_bytes < offset_after_type)
						{
							int bytes_remaining = bytes_received
										- (offset_after_type - current_bytes);
							memcpy((char *)(&smsg) + current_bytes, buffer,
											offset_after_type - current_bytes);
							memmove(buffer, buffer + offset_after_type
											- current_bytes, bytes_remaining);
							current_bytes = offset_after_type;
							size = smsg_size(smsg.message_type,
														smsg.notif.data_type);
							bytes_received = bytes_remaining;
						}
						else
							size = smsg_size(smsg.message_type,
														smsg.notif.data_type);
						//mai sus am calculat dimensiunea reala
						if (current_bytes + bytes_received >= size)
						{
							//daca putem termina mesajul
							int bytes_remaining = bytes_received - size
																+ current_bytes;
							memcpy((char *)(&smsg) + current_bytes, buffer,
														size - current_bytes);
							memmove(buffer, buffer + size - current_bytes,
															bytes_remaining);
							current_bytes = size;
							bytes_received = bytes_remaining;
						}
					}
					if (size == 1 || current_bytes == size)
					{
						//daca mesajul e complet, il procesam
						current_bytes = 0;
						switch (smsg.message_type)
						{
						case -2: //numele a fost luat deja
							printf("Client id \"%s\" was already taken.\n",
																	client_id);
							client_online = 0;
							break;
						case -1: //interogare a numelui
							cmsg.message_type = -1;
							memset(cmsg.content, 0, 50);
							memcpy(cmsg.content, client_id, 10);
							//se trimite mesaj la server
							n = send(sockfd, &cmsg,
											cmsg_size(cmsg.message_type), 0);
							DIE(n < 0, "send");
							break;
						case 0: //primire notificare
						{
							char *value = parse_notification_value(smsg.notif);
							printf("%s:%d - %s - %s - %s\n",
										inet_ntoa(smsg.source.sin_addr),
										ntohs(smsg.source.sin_port),
										smsg.notif.topic,
										identifier_name[smsg.notif.data_type],
										value);
							free(value);
							break;
						}
						case 1:
							if (smsg.notif.data_type == 2) //succes abonare
								printf("subscribed topic %s\n",
															smsg.notif.topic);
							else //success schimbare sf
								printf("updated topic %s to value %d\n",
										smsg.notif.topic, smsg.notif.data_type);
							break;
						case 2:
							if (smsg.notif.data_type == 1) //succes dezabonare
								printf("unsubscribed topic %s\n",
															smsg.notif.topic);
							else //daca nu suntem abonati la acel topic
								printf("you are not subscribed to topic %s\n",
															smsg.notif.topic);
							break;
						}
					}
					else
					{
						/*daca nu mai avem destui byti sa cream un mesaj
						punem restul de byti din buffer in mesaj
						*/
						memcpy((char *)(&smsg) + current_bytes, buffer,
																bytes_received);
						current_bytes += bytes_received;
						break;
					}
				}
			}
		}
	}
	close(sockfd);

	return 0;
}
