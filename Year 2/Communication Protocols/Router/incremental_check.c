#include "incremental_check.h"

void check_remove_old(uint16_t* HC, void* m, int offset)
{
	*HC += (*((uint16_t*)(m) + offset));
}

void check_add_new(uint16_t* HC, void* m, int offset)
{
	*HC -= (*((uint16_t*)(m) + offset));
}