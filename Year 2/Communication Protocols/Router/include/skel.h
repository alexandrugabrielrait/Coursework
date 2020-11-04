#pragma once
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>
/* ethheader */
#include <net/ethernet.h>
/* ether_header */
#include <arpa/inet.h>
/* icmphdr */
#include <netinet/ip_icmp.h>
/* arphdr */
#include <net/if_arp.h>
#include <asm/byteorder.h>
#include "queue.h"
#include "parser.h"
#include "incremental_check.h"

/* 
 *Note that "buffer" should be at least the MTU size of the 
 * interface, eg 1500 bytes 
 */
#define MAX_LEN 1600
#define ROUTER_NUM_INTERFACES 4

extern struct route_table_entry *rtable;
extern int rtable_size;

extern struct arp_entry *arp_table;
extern int arp_table_len;

#define DIE(condition, message) \
	do { \
		if ((condition)) { \
			fprintf(stderr, "[%d]: %s\n", __LINE__, (message)); \
			perror(""); \
			exit(1); \
		} \
	} while (0)

typedef struct {
	int len;
	char payload[MAX_LEN];
	int interface;
} packet;

extern int interfaces[ROUTER_NUM_INTERFACES];

int send_packet(int interface, packet *m);
int get_packet(packet *m);
char *get_interface_ip(int interface);
int get_interface_mac(int interface, uint8_t *mac);
void init();

/**
 * hwaddr_aton - Convert ASCII string to MAC address (colon-delimited format)
 * @txt: MAC address as a string (e.g., "00:11:22:33:44:55")
 * @addr: Buffer for the MAC address (ETH_ALEN = 6 bytes)
 * Returns: 0 on success, -1 on failure (e.g., string not a MAC address)
 */
int hwaddr_aton(const char *txt, uint8_t *addr);

struct arp_entry
{
	__u32 ip;
	uint8_t mac[6];
};

//adresele care se gasesc dupa headerul arp
struct arpaddr {
	uint8_t		ar_sha[6];	/* sender hardware address */
	uint8_t		ar_spa[4];	/* sender protocol address */
	uint8_t		ar_tha[6];	/* target hardware address */
	uint8_t		ar_tpa[4];	/* target protocol address */
};

uint16_t checksum(void *vdata, size_t length);

//Functii sender

/*Trimite un nou pachet ICMP:
m - pachetul IP cu problema
ip_addr - adresa ip interfetei pe care a venit pachetul
mac_addr - adresa mac interfetei pe care a venit pachetul
icmp_type - tipul de ICMP
*/
void send_ICMP_packet(packet m, uint32_t ip_addr, uint8_t *mac_addr, uint8_t icmp_type);

/*Proceseaza si trimite un pachet ip:
m - pachetul procesat
ip_addr - adresa ip interfetei pe care a venit pachetul
mac_addr - adresa mac interfetei pe care a venit pachetul
packet_queue - coada pe care punem pachetele in asteptare
shouldRequest - daca negasire unei adrese mac duce la trimiterea unui pachet arp request
Intoarce 1 daca pachetul a fost trimis, -1 daca a fost aruncat
*/
int process_IP_packet(packet m, uint32_t ip_addr, uint8_t *mac_addr, queue packet_queue, int shouldRequest);