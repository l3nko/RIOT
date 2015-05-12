/*
 *	by l3nko
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "config.h"

#include "uECC.h"
#include "crypto/aes.h"

#define ECC_KEY_SIZE uECC_BYTES*2

static uint8_t sc_private[uECC_BYTES];

static uint8_t sc_public[ECC_KEY_SIZE];
static uint8_t sc_dest_public[ECC_KEY_SIZE];

static cipher_context_t sc_context;

/* checked for type safety */
int _uECC_get_set_public_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("My public key: ");
        for(int i=0; i<ECC_KEY_SIZE; i++)
            printf("%02x", sc_public[i]);
        printf("\n");
    }
    else {
        printf("Setting destination publick key\n");
        
        //check string arg length
        size_t argLen = strlen(argv[1]);
        if (argLen < (ECC_KEY_SIZE*2)) {
            puts("Public key deve essere lunga 40 bytes (80 hex-characters)");
            return 1;
        }
        
        //copy key (from hex string)
        char *hexStr = argv[1];
        uint8_t acc;
        printf("\nreading: ");
        for(int i=0; i < ECC_KEY_SIZE; i++) {
            sscanf(hexStr, "%2hhx", &acc);//&sc_dest_public[i]);
            hexStr += 2;
            
            printf("%02x", acc);
            sc_dest_public[i] = acc;
        }
        
//        printf("Copied key: ");
//        for(int i=0; i<ECC_KEY_SIZE; i++)
//            printf("%02x", sc_dest_public[i]);
//        printf("\n");
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
            for(int i=0; i<ECC_KEY_SIZE; i++)
                printf("%02x", sharedKey[i]);
            printf("\n");
        }
        //set symmetric-key
        aes_interface.setupKey(&sc_context, sharedKey, uECC_BYTES);

        //use AES(128) to enc plaintext
        uint8_t cipherText[100];
        aes_interface.BlockCipher_encrypt(&sc_context, (uint8_t*)argv[1], cipherText);
        printf("Cipher text: ");
        for (int i=0; i<(int)strlen(argv[1]); i++) {
            printf("%02x",cipherText[i]);
        }
        printf("\n");
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
        aes_interface.BlockCipher_decrypt(&sc_context, plaintext, argv[1]);
        for (int i=0; i<strlen(argv[1]); i++) {
            printf("%2x",plaintext[i]);
        }
    }
    return 0;
}


