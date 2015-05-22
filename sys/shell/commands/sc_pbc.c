/*
 *	by l3nko
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>
#include "relic.h"
#include "periph/random.h"


static char thisID[10];
static sokaka_t privateKey;
static bn_st masterKey;

/* CRC 32 */
uint32_t crc32(uint32_t crc, const void *buf, size_t size);

void printSokaka(sokaka_t t)
{
    //        fp_t x;
    //        /** The second coordinate. */
    //        fp_t y;
    //        /** The third coordinate (projective representation). */
    //        fp_t z;
    //        /** Flag to indicate that this point is normalized. */
    //        int norm;
    puts("g1");
    g1_print(t->s1);
    
    puts("g2");
    g2_print(t->s2);
}

static void printBuffer(uint8_t *ptr, size_t length, uint8_t withCRC)
{
    for (size_t i=0; i<length; i++) {
        printf("%02x",ptr[i]);
    }
    if (withCRC == 1) {
        uint32_t calcCRC = crc32(0, ptr, length);
        printf(" (0x%08X)", calcCRC);
    }
    printf("\n");
}

int pbc_start_handler(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: pbc_start NODE_ID");
        return 1;
    }

    snprintf(thisID, 10, "%s", argv[1]);
    
    //random generator
    char random_buffer[SEED_SIZE];
    random_init();
    random_poweron();
    random_read(random_buffer, SEED_SIZE);
    random_poweroff();
    
    puts("Random buffer:");
    printBuffer(random_buffer, SEED_SIZE, 0);
    
    if (core_init_riot(random_buffer) != STS_OK) {
//    if (core_init() != STS_OK) {
        puts("Relic core init failed");
        return 1;
    }

    if (pc_param_set_any() != STS_OK) {
        puts("Relic param set any failed");
        return 1;
    }

    /* Simulate PKG. The private key should be loaded statically in real deployments. */
    bn_init(&masterKey, BN_DIGS);
    bn_read_str(&masterKey, "123456789ABCDEF123456789ABCDEF123456789ABCDEF123456789ABCDEF", 64, 16);	//master key received from PKG
    
    //extern ssize_t (*real_read)(int fd, void *buf, size_t count);
    //nread = real_read(_native_tap_fd, &frame, sizeof(union eth_frame));
    
    if(cp_sokaka_gen_prv(privateKey, thisID, strlen(thisID), &masterKey) != STS_OK)
        puts("Relic sokake generation FAILED");
    else {
    	puts("RIOT-PBC: Private key generated");
        printf("size: %lu (%lu, %lu)\n", sizeof(privateKey), sizeof(privateKey->s1), sizeof(privateKey->s2));
        
//        printBuffer((uint8_t*)privateKey, sizeof(privateKey), 1);
        
        printSokaka(privateKey);
        
        printf("\n");
    }
    
    return 0;
}

int pbc_private_handler(int argc, char **argv) {
    if (argc < 2) {
//        //get private key
//        puts("RIOT-PBC private key: ");
//        for (int i = 0; i < sizeof(privateKey->s1); i++) {
//            printf("%02X", );
//        }
        
    }
    else {
        //check string arg length
        size_t argLen = strlen(argv[1]);
        if (argLen < sizeof(privateKey) ) {
            puts("Key size wrong");
            return 1;
        }
        
        printf("Reading private key...%lu\n", strlen);
        
        //copy key (from hex string)
        uint8_t buffer[1024];
        char *hexStr = argv[1];
        for(size_t i=0; i < sizeof(privateKey); i++) {
            sscanf(hexStr, "%2hhx", buffer[i]);
            hexStr += 2;
        }
        memcpy((void*)privateKey, buffer, sizeof(privateKey));
        uint32_t calcCRC = crc32(0, buffer, sizeof(privateKey));
        printf("CRC: 0x%08X \n", calcCRC);
        
    }
    
    
    
    return 0;
}

int pbc_agree_handler(int argc, char **argv) {
    
    if (argc < 2) {
        puts("Usage: pbc_agree NODE_ID");
        return 1;
    }
    
    int i;
    char nodeID[10];
    unsigned char sharedKey[MD_LEN];
    
    snprintf(nodeID, 10, "%s", argv[1]);
    cp_sokaka_key(sharedKey, MD_LEN, thisID, strlen(thisID), privateKey, nodeID, strlen(nodeID));
    
    puts("RIOT-PBC Shared key: ");
    for (i = 0; i < MD_LEN; i++) {
        printf("%02X", sharedKey[MD_LEN - i - 1]);
    }
    printf("\n");
    
    return 0;
}

//int relic_set_public_handler(int argc, char **argv)
//{
//    if (argc < 2) {
//        printf("My public key: ");
//        printBuffer(sc_public, ECC_KEY_SIZE, 1);
//    }
//    else {
//        //check string arg length
//        size_t argLen = strlen(argv[1]);
//        if (argLen < (ECC_KEY_SIZE*2)) {
//            puts("Public key deve essere lunga 40 bytes (80 hex-characters)");
//            return 1;
//        }
//        
//        //copy key (from hex string)
//        char *hexStr = argv[1];
//        //uint8_t acc;
//        //printf("\nreading: ");
//        for(int i=0; i < ECC_KEY_SIZE; i++) {
//            sscanf(hexStr, "%2hhx", &sc_dest_public[i]);
//            hexStr += 2;
//            
//            //printf("%02x", acc);
//            //sc_dest_public[i] = acc;
//        }
//        //printf("\n");
//        
//        //calculate crc32
//        uint32_t dest_crc = crc32(0, sc_dest_public, ECC_KEY_SIZE);
//        printf("Calculated crc: 0x%X\n", dest_crc);
//    }
//    return 0;
//}
