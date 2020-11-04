#pragma once
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ip.h>
#include "skel.h"

struct route_table_entry {
	uint32_t prefix;
	uint32_t next_hop;
	uint32_t mask;
	int interface;
} __attribute__((packed));

// adauga o linie noua in tabela arp
void add_arp_entry(__u32 ip, uint8_t *mac);
// intoarce o linie din tabela arp in functie de ip
struct route_table_entry *get_best_route(uint32_t dest_ip);
struct arp_entry *get_arp_entry(uint32_t ip);

//	intoarce numarul de linii al tabelei de rutare
int get_rtable_size();
/* 	citeste din fisier tabela de rutare si
	intoarce numarul de linii citite
*/
int read_rtable(struct route_table_entry *rtable);