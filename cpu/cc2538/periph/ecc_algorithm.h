/*
 * ecc_algorithm.h
 *
 *  Created on: Dec 21, 2015
 *      Author: nicco
 */

#ifndef ECC_ALGORITHM_H_
#define ECC_ALGORITHM_H_

#include <stdint.h>
#include "pka.h"

#define MIPST_192 0
#define MIPST_256 1


/*
 * make public-private key
 */
uint8_t ecc_make_key(uint8_t *public_key, uint8_t *private_key);


/* Util functions */
uint8_t ecc_valid_public_key(const uint8_t *public_key);
uint8_t ecc_compute_public_key(const uint8_t *private_key, uint8_t *public_key);


/*
 * Compute EC-DH to obtain a shared key
 */
uint8_t ecc_shared_secret(const uint8_t *public_key, const uint8_t *private_key, uint8_t *secret);

/*
 * EC-DSA: sign and verify
 */
uint8_t ecc_sign(const uint8_t *private_key, const uint8_t *message_hash, unsigned hash_size, uint8_t *signature);
uint8_t ecc_verify(const uint8_t *private_key, const uint8_t *message_hash, unsigned hash_size, const uint8_t *signature);


#endif /* ECC_ALGORITHM_H_ */
