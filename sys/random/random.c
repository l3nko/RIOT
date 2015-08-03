/*
 * random.c
 *
 *  Created on: Aug 3, 2015
 *      Author: l3nko
 */

#include "random.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define MERSENNE_MODE	0
#define TINY_MT_MODE 	1

#ifndef RANDOM_MODE
	#error ("RANDOM_MODE not setted")
	#define RANDOM_MODE 0xFF
#endif

#if RANDOM_MODE == MERSENNE_MODE
#pragma message ("RANDOM_MODE set to value MERSENNE_MODE")
void random_init(uint32_t s)
{
	DEBUG("Calling mersenne genrand_init with seed %"PRIu32 "\n", s);
	genrand_init(s);
}

uint32_t random_uint32(void)
{
	DEBUG("Calling mersenne genrand_uint32\n");
	return genrand_uint32();
}

#elif RANDOM_MODE == TINY_MT_MODE
#pragma message ("RANDOM_MODE set to value TINY_MT_MODE")

#include "tinymt32.h"

#define DEFAULT_SEED 5489UL
static tinymt32_t tiny32;

#define STATE_INITIALIZED 		0xAA
#define STATE_UNINITIALIZED 	0x00
static uint8_t state = STATE_UNINITIALIZED;


void random_init(uint32_t s)
{
	DEBUG("Calling tinymt32_init with seed %"PRIu32 "\n", s);

	tinymt32_init(&tiny32, s);
	state = STATE_INITIALIZED;
}

//void genrand_init_by_array(uint32_t init_key[], int key_length)
//{
//	DEBUG("Calling tinymt32_init_by_array with key length %d\n", key_length);
//	tinymt32_init_by_array(&tiny32, init_key, key_length);
//	state = STATE_INITIALIZED;
//}

uint32_t random_uint32(void)
{
	if(state == STATE_UNINITIALIZED) {
		tinymt32_init(&tiny32, DEFAULT_SEED);
		state = STATE_INITIALIZED;
	}
	DEBUG("Calling tinymt32_generate_uint32\n");
	return tinymt32_generate_uint32(&tiny32);
}


#else
	#error "RANDOM_MODE value not correct"
#endif
