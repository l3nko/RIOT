/*
 * random.c
 *
 *  Created on: Aug 3, 2015
 *      Author: l3nko
 */

#include "random.h"

#define MERSENNE_MODE	0
#define TINY_MT_MODE 	1

#ifndef RANDOM_MODE
	#pragma message ("RANDOM_MODE set to default value MERSENNE_MODE")
	#define RANDOM_MODE MERSENNE_MODE
#endif

#if RANDOM_MODE == MERSENNE_MODE
/* nothing to do */

#elif RANDOM_MODE == TINY_MT_MODE
#include "tinymt32.h"

#define DEFAULT_SEED 5489UL
static tinymt32_t tiny32;

#define STATE_INITIALIZED 		0xAA
#define STATE_UNINITIALIZED 	0x00
static uint8_t state = STATE_UNINITIALIZED;


void genrand_init(uint32_t s)
{
	tinymt32_init(&tiny32, s);
	state = STATE_INITIALIZED;
}

void genrand_init_by_array(uint32_t init_key[], int key_length)
{
	tinymt32_init_by_array(&tiny32, init_key, key_length);
	state = STATE_INITIALIZED;
}

uint32_t genrand_uint32(void)
{
	if(state == STATE_UNINITIALIZED) {
		tinymt32_init(&tiny32, DEFAULT_SEED);
		state = STATE_INITIALIZED;
	}

	return tinymt32_generate_uint32(&tiny32);
}


#else
	#error "RANDOM_MODE value not correct"
#endif
