#include "skel.h"

//Pentru calcularea checksum dupa algoritmul din rfc1624

//Din rfc1624: HC = HC - ~m
void check_remove_old(uint16_t* HC, void* m, int offset);
//Din rfc1624: HC = HC - m'
void check_add_new(uint16_t* HC, void* m, int offset);