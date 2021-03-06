
#include <stdio.h>
#include <memory.h>
#include "blowfish.h"

#define VERIFY

unsigned char key[8] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

unsigned char input[512] =
{
    0xC1, 0x4D, 0xA7, 0xE9, 0xDF, 0x09, 0x3B, 0xC9, 0x78, 0x00, 0x44, 0x0F, 0x5A, 0xE1, 0xC1, 0xD4, 
    0x6A, 0x80, 0xD8, 0x94, 0x62, 0xE3, 0x7C, 0x02, 0x2D, 0x70, 0xF0, 0x08, 0x73, 0xB5, 0x23, 0x19, 
    0xF6, 0xC4, 0x4C, 0xCB, 0xDC, 0x99, 0xE4, 0x0C, 0xE7, 0xC4, 0x8B, 0x69, 0x2F, 0xC3, 0xF7, 0x84, 
    0x22, 0x3D, 0x1F, 0xBB, 0xBC, 0xD2, 0x5F, 0xE4, 0x96, 0x8A, 0x40, 0xD2, 0xBD, 0x10, 0xE0, 0x21, 
    0x1C, 0x0B, 0xFB, 0xFE, 0x0F, 0xFF, 0xFF, 0xC5, 0x6D, 0x36, 0xC3, 0x54, 0x82, 0xB0, 0x90, 0x80, 
    0x30, 0xBD, 0x34, 0xE9, 0x45, 0x6E, 0xF7, 0xC5, 0x69, 0x1A, 0xAA, 0x91, 0xA2, 0x85, 0x4A, 0x54, 
    0xAA, 0x0E, 0xBB, 0xC3, 0xC6, 0xF4, 0xBA, 0x58, 0x75, 0x39, 0xAB, 0x18, 0xC6, 0x79, 0x64, 0x7E, 
    0x97, 0x80, 0x3E, 0x43, 0xF8, 0x9D, 0x9B, 0xB3, 0x46, 0x69, 0x7A, 0xD6, 0x71, 0x9A, 0xAB, 0x8E, 
    0xB2, 0x4E, 0x5E, 0x99, 0x39, 0x56, 0x20, 0x1E, 0x6D, 0x47, 0x6D, 0x92, 0x92, 0x21, 0x3B, 0x68, 
    0x2D, 0xE4, 0xD9, 0x9F, 0x13, 0x5B, 0x45, 0x9C, 0xD5, 0xAD, 0xE4, 0xE3, 0x6B, 0xAD, 0x37, 0x4A, 
    0xF8, 0x2F, 0xE6, 0xE0, 0xE1, 0xC1, 0xA0, 0xA3, 0x86, 0x7B, 0x80, 0x55, 0x0A, 0x2B, 0x8A, 0x8A, 
    0x9A, 0x3D, 0xAA, 0x5C, 0x2C, 0x14, 0x87, 0x33, 0xB1, 0xD7, 0xA9, 0x2A, 0xEC, 0x74, 0x58, 0x08, 
    0xFE, 0x43, 0xCE, 0xEA, 0x30, 0x17, 0x08, 0x0F, 0xDD, 0x66, 0x3F, 0xC4, 0xEA, 0xCA, 0x31, 0x5F, 
    0x50, 0xA4, 0x2A, 0x85, 0x3F, 0x31, 0x65, 0xD0, 0x97, 0x61, 0xB0, 0x32, 0xD8, 0x65, 0x59, 0x0B, 
    0x0E, 0x6D, 0xA2, 0xF6, 0x43, 0x2A, 0x64, 0xAA, 0x68, 0xDB, 0xED, 0x36, 0x56, 0x79, 0x96, 0x6B, 
    0x1D, 0x66, 0xA8, 0xD6, 0x52, 0xE8, 0x59, 0x32, 0x9E, 0x2C, 0x76, 0xBE, 0x7D, 0xE3, 0x55, 0x33, 
    0xBD, 0xB7, 0x98, 0x8E, 0x5F, 0x8F, 0xC1, 0xF5, 0xDF, 0xDC, 0x99, 0x2B, 0x5E, 0xEC, 0x5B, 0x39, 
    0xAF, 0x1F, 0x94, 0x81, 0x52, 0x65, 0x01, 0x41, 0x5A, 0xB0, 0x6D, 0x1A, 0x16, 0x16, 0x8B, 0x7E, 
    0x81, 0xF6, 0xDC, 0xFE, 0x03, 0x18, 0xD2, 0xE0, 0x5E, 0x36, 0x7A, 0xEE, 0xB7, 0xB8, 0x1F, 0xE5, 
    0x6C, 0xA7, 0xAE, 0x59, 0x2C, 0xB7, 0x84, 0x31, 0x21, 0x90, 0x51, 0x5D, 0x42, 0xBD, 0xB0, 0xAA, 
    0xC0, 0xD0, 0x04, 0x35, 0x99, 0x6C, 0x97, 0x5C, 0xC3, 0x32, 0x90, 0xE2, 0x8C, 0x47, 0x4E, 0x69, 
    0xF6, 0x89, 0x2D, 0x1D, 0xBE, 0x1C, 0x8C, 0x8D, 0x01, 0x77, 0x2D, 0x7C, 0xC4, 0xFB, 0x16, 0x7A, 
    0xB8, 0x5B, 0x94, 0xF2, 0x21, 0x91, 0x6B, 0x72, 0x45, 0x99, 0xCA, 0xCE, 0xCE, 0xD0, 0x95, 0x52, 
    0xFD, 0xCB, 0x19, 0x7A, 0x86, 0xD9, 0x26, 0xEA, 0xAC, 0x87, 0xD2, 0x8D, 0xB6, 0xED, 0x22, 0x35, 
    0x7D, 0xD2, 0x53, 0x23, 0x40, 0x8B, 0x11, 0x6A, 0xA5, 0xCB, 0xBA, 0xA0, 0xC3, 0x05, 0x8F, 0x0C, 
    0x20, 0x23, 0x55, 0x18, 0x94, 0x12, 0x4D, 0x0D, 0xFA, 0xEB, 0xF1, 0xD1, 0x6E, 0xCA, 0x02, 0x08, 
    0x25, 0xF4, 0xAA, 0x11, 0x34, 0x0D, 0xBD, 0x77, 0xD2, 0xF3, 0xDC, 0xCB, 0x6D, 0x05, 0x24, 0xA3, 
    0x34, 0x8B, 0x59, 0x01, 0x65, 0x12, 0xAF, 0x7B, 0x20, 0x10, 0xA1, 0x09, 0xB5, 0x06, 0xF2, 0x48, 
    0x61, 0x1F, 0x23, 0x54, 0x68, 0x7B, 0x9F, 0x63, 0x07, 0x16, 0x92, 0x4D, 0xFA, 0xB0, 0x06, 0x3E, 
    0x15, 0x49, 0x92, 0x6A, 0xEB, 0x88, 0x5D, 0xAC, 0xA8, 0xED, 0x35, 0x16, 0x2A, 0x0D, 0x84, 0x81, 
    0x60, 0xA1, 0x08, 0x60, 0x30, 0x0F, 0x61, 0x47, 0x65, 0x07, 0xC6, 0xF5, 0x52, 0x92, 0x84, 0xDE, 
    0xA8, 0x00, 0x8E, 0x8B, 0x1E, 0xE4, 0x74, 0x53, 0xB8, 0x53, 0x92, 0x6B, 0xC8, 0x28, 0xD6, 0xE6, 
} ;

#ifdef VERIFY
unsigned char encrypted[512] = {
    0x62, 0xec, 0xe8, 0x24, 0x74, 0xb1, 0x0b, 0x74, 0xf5, 0xdd, 0x70, 0x91, 0x6f, 0xed, 0xf7, 0xdf, 
    0x89, 0xb9, 0x7d, 0x34, 0x7a, 0x3f, 0xe7, 0x92, 0x1d, 0x07, 0x36, 0xa3, 0x91, 0x26, 0xe8, 0x85, 
    0xba, 0x80, 0x1d, 0x1c, 0xac, 0x4e, 0xfa, 0x3b, 0xf0, 0x63, 0x64, 0xf3, 0x43, 0x23, 0x40, 0x3d, 
    0xca, 0xef, 0xbf, 0xb9, 0xce, 0x26, 0xbf, 0xd7, 0x59, 0xfc, 0x66, 0x91, 0xec, 0x06, 0x40, 0x87, 
    0xaf, 0xd8, 0xa2, 0xf9, 0xb0, 0xe1, 0x5a, 0x92, 0xff, 0x0c, 0x0b, 0xa1, 0xfb, 0xc0, 0x4a, 0x82, 
    0x3a, 0x75, 0x35, 0x56, 0x5c, 0x2f, 0x70, 0x08, 0xac, 0x55, 0xda, 0xe8, 0x53, 0x79, 0x3c, 0x63, 
    0xc7, 0x73, 0xed, 0x1d, 0xde, 0xe8, 0x6c, 0xa8, 0xf3, 0xd4, 0xfb, 0x6a, 0x52, 0xbd, 0xcb, 0xe2, 
    0x28, 0xe5, 0x8b, 0x2b, 0xfd, 0x0c, 0xb7, 0xe7, 0x56, 0x37, 0x1d, 0xd2, 0x49, 0xc9, 0x29, 0x0c, 
    0x07, 0x41, 0x17, 0x96, 0xb4, 0x2f, 0x08, 0x61, 0xf7, 0x64, 0x16, 0x5d, 0x7d, 0x49, 0x64, 0xa2, 
    0x67, 0xf5, 0x6a, 0x85, 0xb6, 0x5c, 0xeb, 0x90, 0xa5, 0x81, 0x1a, 0x96, 0xe6, 0x15, 0x27, 0xf6, 
    0x96, 0xa5, 0xdc, 0x5d, 0x8a, 0x9e, 0xf5, 0xff, 0x42, 0x16, 0xd5, 0x33, 0x9e, 0xc0, 0x69, 0x64, 
    0xd8, 0x7b, 0x6f, 0x8a, 0x72, 0x8a, 0x24, 0x20, 0x08, 0xc9, 0x8b, 0x3e, 0xfc, 0x60, 0xdd, 0x35, 
    0x27, 0xdc, 0xcd, 0xca, 0x1a, 0xcd, 0x64, 0x0c, 0xd5, 0x9e, 0x0e, 0xd9, 0x0c, 0x97, 0x24, 0x72, 
    0x66, 0xec, 0xc3, 0x56, 0x5c, 0xee, 0x7a, 0x40, 0xa3, 0x71, 0x4d, 0x41, 0x97, 0x9b, 0x9e, 0xdf, 
    0x63, 0xd4, 0xbc, 0x9b, 0xfa, 0x05, 0x4c, 0x95, 0x77, 0x5b, 0x51, 0x5b, 0xf8, 0xd2, 0xc3, 0xcc, 
    0xf1, 0x84, 0x57, 0x1c, 0xa1, 0x57, 0xc1, 0xe3, 0x23, 0xd8, 0x0b, 0x1c, 0xc2, 0xa5, 0x36, 0xba, 
    0x76, 0x4c, 0x09, 0x77, 0xcd, 0x70, 0x61, 0x26, 0xa8, 0x3a, 0xe0, 0x8f, 0x78, 0x90, 0x15, 0x66, 
    0xb4, 0x4d, 0x3f, 0x4c, 0xc2, 0xc8, 0x67, 0x9a, 0xea, 0x75, 0x69, 0x44, 0x49, 0xfa, 0x66, 0x93, 
    0x63, 0xc3, 0xc6, 0x34, 0x76, 0x23, 0xd4, 0x53, 0x9c, 0x2f, 0xc7, 0xbf, 0x8b, 0x4d, 0xee, 0x27, 
    0xbc, 0x91, 0x9d, 0xac, 0xea, 0xad, 0xd5, 0xa3, 0x6f, 0xcd, 0x98, 0xa1, 0xc6, 0x62, 0x86, 0x14, 
    0x71, 0x38, 0xea, 0x2d, 0x82, 0x04, 0xd1, 0x01, 0xc2, 0x20, 0xa6, 0xf8, 0x0b, 0x3d, 0x01, 0xc7, 
    0x71, 0x8a, 0xa3, 0x3d, 0x26, 0xd7, 0x17, 0x3d, 0x3c, 0xc9, 0xf8, 0x89, 0x77, 0xb2, 0x7e, 0x83, 
    0x49, 0x0a, 0xb9, 0xae, 0xf6, 0xe8, 0xd0, 0x3e, 0xd7, 0x87, 0xab, 0x49, 0x7f, 0xc1, 0xc8, 0xcd, 
    0xcc, 0x82, 0x52, 0xcd, 0x0e, 0xfd, 0x1b, 0x40, 0xef, 0xf9, 0xae, 0xea, 0x0e, 0x02, 0xe7, 0xe0, 
    0xa4, 0x48, 0x09, 0x8b, 0x3d, 0x49, 0xa0, 0x10, 0x68, 0x3c, 0xc0, 0x83, 0x9a, 0x22, 0x09, 0x21, 
    0x8a, 0xf3, 0xc1, 0xb3, 0x5d, 0x95, 0x3f, 0x89, 0xa3, 0x7b, 0xca, 0x70, 0xfc, 0x47, 0x1b, 0x29, 
    0x44, 0xe7, 0x06, 0xb8, 0x22, 0x71, 0x5e, 0x40, 0xaa, 0x52, 0x84, 0x71, 0x42, 0xf1, 0x25, 0x4e, 
    0x02, 0x8c, 0xae, 0x16, 0x45, 0x9c, 0x0d, 0xde, 0x6d, 0x86, 0x41, 0xf6, 0x69, 0xe6, 0xa8, 0x38, 
    0xac, 0x63, 0x04, 0x78, 0x50, 0x67, 0x9c, 0xb3, 0xcf, 0x2c, 0xc6, 0x0a, 0x1a, 0xa5, 0x8f, 0x2d, 
    0x15, 0xc4, 0xe9, 0x31, 0xc3, 0xb1, 0x1e, 0x2b, 0x5b, 0xd9, 0x38, 0x45, 0x06, 0xa8, 0x5b, 0xd0, 
    0xe6, 0x95, 0xa3, 0xb5, 0x7a, 0x49, 0xa1, 0x3a, 0x2d, 0xef, 0xb1, 0xfd, 0x0e, 0xf2, 0xb2, 0xf1, 
    0xe3, 0xb9, 0x52, 0xef, 0xe7, 0x9c, 0x7a, 0x88, 0x93, 0xb6, 0x57, 0xe8, 0x1c, 0x2f, 0xc0, 0x6a 
};
#endif

int main(int argc, char *argv[])
{
    unsigned int i;
    BF_KEY bf_key;
    unsigned char output[sizeof(input)];
    BF_set_key(&bf_key, 8, key);
    for (i = 0; i < sizeof(input); i += 8)
        BF_ecb_encrypt(&input[i], &output[i], &bf_key, BF_ENCRYPT);
#ifdef VERIFY
    if (memcmp(encrypted, output, sizeof(output)) != 0)
        printf("verification failed!\n");
#endif
    return 0;
}
