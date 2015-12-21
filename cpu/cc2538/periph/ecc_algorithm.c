/*
 * ecc_algorithm.c
 *
 *  Created on: Dec 21, 2015
 *      Author: nicco
 */

#include "ecc_algorithm.h"

#if ECC_CURVE == MIPST_256
/* [NIST P-256, X9.62 prime256v1] */
static const uint32_t nist_p_256_p[8] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
                                          0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF };
static const uint32_t nist_p_256_n[8] = { 0xFC632551, 0xF3B9CAC2, 0xA7179E84, 0xBCE6FAAD,
                                          0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF };
static const uint32_t nist_p_256_a[8] = { 0xFFFFFFFC, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
                                          0x00000000, 0x00000000, 0x00000001, 0xFFFFFFFF };
static const uint32_t nist_p_256_b[8] = { 0x27D2604B, 0x3BCE3C3E, 0xCC53B0F6, 0x651D06B0,
                                          0x769886BC, 0xB3EBBD55, 0xAA3A93E7, 0x5AC635D8 };
static const uint32_t nist_p_256_x[8] = { 0xD898C296, 0xF4A13945, 0x2DEB33A0, 0x77037D81,
                                          0x63A440F2, 0xF8BCE6E5, 0xE12C4247, 0x6B17D1F2 };
static const uint32_t nist_p_256_y[8] = { 0x37BF51F5, 0xCBB64068, 0x6B315ECE, 0x2BCE3357,
                                          0x7C0F9E16, 0x8EE7EB4A, 0xFE1A7F9B, 0x4FE342E2 };
ecc_curve_info_t curve_info = {
  .name    = "NIST P-256",
  .size    = 8,
  .prime   = nist_p_256_p,
  .n       = nist_p_256_n,
  .a       = nist_p_256_a,
  .b       = nist_p_256_b,
  .x       = nist_p_256_x,
  .y       = nist_p_256_y
};
#else
/* [NIST P-192, X9.62 prime192v1] */
static const uint32_t nist_p_192_p[6] = { 0xffffffff, 0xffffffff, 0xfffffffe, 0xffffffff,
                                          0xffffffff, 0xffffffff };
static const uint32_t nist_p_192_a[6] = { 0xfffffffc, 0xffffffff, 0xfffffffe, 0xffffffff,
                                          0xffffffff, 0xffffffff };
static const uint32_t nist_p_192_b[6] = { 0xc146b9b1, 0xfeb8deec, 0x72243049, 0x0fa7e9ab,
                                          0xe59c80e7, 0x64210519 };
static const uint32_t nist_p_192_x[6] = { 0x82ff1012, 0xf4ff0afd, 0x43a18800, 0x7cbf20eb,
                                          0xb03090f6, 0x188da80e };
static const uint32_t nist_p_192_y[6] = { 0x1e794811, 0x73f977a1, 0x6b24cdd5, 0x631011ed,
                                          0xffc8da78, 0x07192b95 };
static const uint32_t nist_p_192_n[6] = { 0xb4d22831, 0x146bc9b1, 0x99def836, 0xffffffff,
                                          0xffffffff, 0xffffffff };
ecc_curve_info_t curve_info = {
  .name    		= "NIST P-192",
  .ui8Size    	= 6,
  .pui32Prime   = nist_p_192_p,
  .pui32N       = nist_p_192_n,
  .pui32A       = nist_p_192_a,
  .pui32B       = nist_p_192_b,
  .pui32Gx		= nist_p_192_x,
  .pui32Gy      = nist_p_192_y
};
#endif


uint8_t ecc_make_key(uint8_t *public_key, uint8_t *private_key)
{

	return 0;
}

uint8_t ecc_valid_public_key(const uint8_t *public_key)
{

	return 0;
}

uint8_t ecc_compute_public_key(const uint8_t *private_key, uint8_t *public_key)
{

	return 0;
}

uint8_t ecc_shared_secret(const uint8_t *public_key, const uint8_t *private_key, uint8_t *secret)
{

	return 0;
}

uint8_t ecc_sign(const uint8_t *private_key, const uint8_t *message_hash, unsigned hash_size, uint8_t *signature)
{

	return 0;
}

uint8_t ecc_verify(const uint8_t *private_key, const uint8_t *message_hash, unsigned hash_size, const uint8_t *signature)
{

	return 0;
}


