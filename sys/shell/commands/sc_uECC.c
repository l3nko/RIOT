/*
 *	by l3nko
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "config.h"

#include "uECC.h"
#include "crypto/aes.h"
#include "crypto/rc5.h"

#define ECC_KEY_SIZE uECC_BYTES*2

static uint8_t sc_private[uECC_BYTES];

static uint8_t sc_public[ECC_KEY_SIZE];
static uint8_t sc_dest_public[ECC_KEY_SIZE];

static cipher_context_t sc_context;

/* CRC 32 */
uint32_t crc32(uint32_t crc, const void *buf, size_t size);

void printBuffer(uint8_t *ptr, uint8_t length, uint8_t withCRC)
{
    for (int i=0; i<length; i++) {
        printf("%02x",ptr[i]);
    }
    if (withCRC == 1) {
        uint32_t calcCRC = crc32(0, ptr, length);
        printf(" (0x%08X)", calcCRC);
    }
    printf("\n");
}

/* checked for type safety */
int _uECC_get_set_public_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("My public key: ");
        printBuffer(sc_public, ECC_KEY_SIZE, 1);
    }
    else {
        //check string arg length
        size_t argLen = strlen(argv[1]);
        if (argLen < (ECC_KEY_SIZE*2)) {
            puts("Public key deve essere lunga 40 bytes (80 hex-characters)");
            return 1;
        }
        
        //copy key (from hex string)
        char *hexStr = argv[1];
        //uint8_t acc;
        //printf("\nreading: ");
        for(int i=0; i < ECC_KEY_SIZE; i++) {
            sscanf(hexStr, "%2hhx", &sc_dest_public[i]);
            hexStr += 2;
            
            //printf("%02x", acc);
            //sc_dest_public[i] = acc;
        }
        //printf("\n");
        
        //calculate crc32
        uint32_t dest_crc = crc32(0, sc_dest_public, ECC_KEY_SIZE);
        printf("Calculated crc: 0x%X\n", dest_crc);
    }
    return 0;
}

int _uECC_get_set_private_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Current private key:");
        for(int i=0; i<ECC_KEY_SIZE; i++)
            printf("%c", sc_private[i]);
        printf("\n");
    }
    else {
        printf("Setting new private key\n");
        
        //check string arg length
        size_t argLen = strlen(argv[1]);
        if (argLen < ECC_KEY_SIZE) {
            puts("Private key deve essere lunga 40 bytes");
            return 1;
        }
        
        //copy key
        //const uint8_t *arg = (uint8_t*)argv[1];
        //memcpy(sc_private, arg, ECC_KEY_SIZE);		// -----> TODO fare come public_key
    }
    return 0;
}

int _uECC_generate_keys(int argc, char **argv)
{
    if (argc > 1) {
        puts(argv[1]);
        puts("Too much arguments. Usage: genecc");
    }
    else {
        random_init();
        int resECC = uECC_make_key(sc_public, sc_private);
        if (resECC<0) {
            puts("Error during generation of keys");
            return 1;
        }
    }
    puts("Keys generated");
    printf("My public key: ");
    printBuffer(sc_public, ECC_KEY_SIZE, 1);
    
    return 0;
}

int _uECC_encrypt(int argc, char **argv)
{
    if (argc < 2) {
        puts("Error at least 2 parameters");
        return 1;
    }
    else {
        //make shared key
        uint8_t sharedKey[uECC_BYTES];
        int res = uECC_shared_secret(sc_dest_public, sc_private, sharedKey);
        if (res<0) {
            puts("Error creating shared key");
            return 1;
        }
        else {
            printf("Current sahred key:");
            printBuffer(sharedKey, uECC_BYTES, 1);
        }
        //aes_interface  OR rc5_interface
        //set symmetric-key
        int resInit = aes_interface.BlockCipher_init(&sc_context, AES_BLOCK_SIZE, uECC_BYTES, sharedKey);
        if (resInit == 0) {
            puts("Error initializing BlockCipher");
            return 1;
        }
        
        uint8_t plainText[AES_BLOCK_SIZE];
        memset(plainText, 0, AES_BLOCK_SIZE);
        memcpy(plainText, argv[1], strlen(argv[1]));

        printf("Plain text: ");
        printBuffer(plainText, AES_BLOCK_SIZE, 0);

        //use cipher to enc plaintext
        uint8_t cipherText[AES_BLOCK_SIZE];
        aes_interface.BlockCipher_encrypt(&sc_context, plainText, cipherText);

        printf("Cipher text: ");
        printBuffer(cipherText, AES_BLOCK_SIZE, 0);
    }
    return 0;
}

int _uECC_decrypt(int argc, char **argv)
{
    if (argc < 2) {
        puts("Error at least 2 parameters");
        return 1;
    }
    else {
        //make shared key
        uint8_t sharedKey[uECC_BYTES];
        int res = uECC_shared_secret(sc_dest_public, sc_private, sharedKey);
        if (res<0) {
            puts("Error creating shared key");
            return 1;
        }
        else {
            printf("Current sahred key:");
            for(int i=0; i<ECC_KEY_SIZE; i++)
                printf("%c", sharedKey[i]);
            printf("\n");
        }
        //set symmetric-key
        aes_interface.setupKey(&sc_context, sharedKey, AES_KEY_SIZE);
        
        //use AES(128) to enc plaintext
        uint8_t plaintext[100];
        aes_interface.BlockCipher_decrypt(&sc_context, plaintext, (uint8_t*)argv[1]);
        for (int i=0; i<(int)strlen(argv[1]); i++) {
            printf("%2x",plaintext[i]);
        }
    }
    return 0;
}


