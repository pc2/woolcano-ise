#include <stdio.h>
#include <memory.h>
#include "sha.h"

#define VERIFY

unsigned char rawData[512] =
{
    0xFC, 0x38, 0x5F, 0x0D, 0xE9, 0x2D, 0xF7, 0x6C, 0x72, 0xB2, 0x1E, 0x3A, 0xBD, 0xEE, 0x37, 0x28, 
    0x25, 0x69, 0xB2, 0x5B, 0x7C, 0x64, 0xB0, 0x86, 0x92, 0xDB, 0xB6, 0xC0, 0x32, 0x33, 0xB4, 0x5E, 
    0xD4, 0xD4, 0xA2, 0x5D, 0x92, 0x37, 0x95, 0xE8, 0xEF, 0xAC, 0xB9, 0xD0, 0xF8, 0xD2, 0xA7, 0x17, 
    0xBB, 0x99, 0xBB, 0xA7, 0x9F, 0x50, 0x7D, 0xE8, 0xD6, 0x21, 0x81, 0x20, 0x40, 0x71, 0x46, 0xC8, 
    0xCB, 0x73, 0xE3, 0x34, 0x79, 0x5C, 0xEE, 0x71, 0xC9, 0x64, 0xF8, 0xEF, 0xDF, 0x73, 0x4B, 0x23, 
    0xFD, 0x73, 0xAE, 0xFC, 0xA3, 0x78, 0xD5, 0xD2, 0xCC, 0x56, 0x5F, 0xD9, 0x9B, 0x88, 0xED, 0xD5, 
    0xE9, 0xAA, 0xD7, 0x35, 0x1E, 0x3F, 0x39, 0x11, 0x45, 0x04, 0x99, 0xD8, 0x92, 0xA6, 0x00, 0x9D, 
    0xB7, 0x8C, 0xEC, 0xC3, 0x1C, 0x2C, 0x66, 0xC6, 0xAA, 0x4E, 0x9C, 0xCE, 0xC3, 0xA5, 0x64, 0x72, 
    0xEA, 0xE3, 0xCD, 0x77, 0xBC, 0x65, 0xBC, 0xFA, 0xFB, 0xE8, 0x5D, 0xB7, 0x6F, 0xBD, 0x52, 0x60, 
    0x1F, 0xB8, 0xFD, 0xA9, 0x4C, 0x6B, 0xD2, 0xA0, 0xDC, 0x34, 0xE4, 0xEC, 0xAF, 0xB7, 0xA3, 0x1C, 
    0x20, 0xA3, 0xA3, 0xD1, 0xA0, 0xB9, 0x64, 0xCC, 0xE2, 0x31, 0xEA, 0xE0, 0xA0, 0xCA, 0x88, 0x75, 
    0x17, 0xA9, 0xDE, 0x54, 0x10, 0x4A, 0x13, 0xC5, 0xA8, 0x06, 0x59, 0x22, 0xA8, 0xED, 0xEF, 0xBF, 
    0x9E, 0x15, 0x36, 0x8D, 0x08, 0x11, 0x00, 0x4B, 0x09, 0xA2, 0xBB, 0xF3, 0xF8, 0xEA, 0xD9, 0x24, 
    0xBB, 0xBD, 0x05, 0x3F, 0x2B, 0x3C, 0xC5, 0x48, 0x94, 0x5E, 0x9C, 0x70, 0xD1, 0x9F, 0x6F, 0x1F, 
    0x5C, 0xCB, 0xAE, 0xCB, 0x67, 0xA2, 0xB9, 0xB1, 0xB0, 0x36, 0x5D, 0x99, 0x57, 0xE1, 0x28, 0x0D, 
    0xC0, 0x49, 0x6E, 0xE2, 0xA3, 0x04, 0xA0, 0xA8, 0xA2, 0xCB, 0x74, 0x75, 0x71, 0x26, 0xC0, 0x18, 
    0xAA, 0x0A, 0xEB, 0x38, 0x24, 0x9F, 0xBA, 0x36, 0x66, 0x66, 0xAA, 0x4E, 0x17, 0x35, 0xB6, 0xD4, 
    0xD8, 0x08, 0x24, 0x08, 0x8D, 0x25, 0x36, 0xCB, 0x48, 0x62, 0x21, 0x20, 0x28, 0x26, 0x1A, 0x09, 
    0x46, 0x03, 0x12, 0xE1, 0xE3, 0x46, 0xF4, 0x13, 0x5E, 0x3C, 0xB8, 0x4C, 0x45, 0x0B, 0xC4, 0x32, 
    0xD5, 0x00, 0xB7, 0xAB, 0x78, 0x78, 0xAB, 0xD0, 0x76, 0x55, 0x8F, 0x9E, 0x99, 0xF0, 0xB0, 0xF3, 
    0xDD, 0xE0, 0x33, 0x90, 0xAB, 0x3F, 0x87, 0x85, 0xC7, 0xAD, 0x20, 0xC0, 0xC7, 0x1B, 0x7D, 0x04, 
    0x35, 0xC8, 0x4F, 0xC8, 0xD1, 0x76, 0x68, 0x28, 0xCE, 0x29, 0x5C, 0x64, 0xE4, 0x44, 0x2A, 0x9F, 
    0x40, 0xEE, 0x6B, 0x9D, 0x43, 0x9A, 0x18, 0xEF, 0xD9, 0x3C, 0xDA, 0xFA, 0x1F, 0x54, 0xD1, 0xD8, 
    0x6D, 0x74, 0x15, 0x5E, 0x4E, 0xB8, 0xBC, 0x25, 0x2E, 0x24, 0xE4, 0xF8, 0x3F, 0x85, 0xDE, 0xAC, 
    0x9A, 0x76, 0x92, 0x3A, 0x7C, 0xCB, 0x33, 0x75, 0x50, 0x26, 0x0A, 0x3E, 0x27, 0xD9, 0x25, 0x15, 
    0x93, 0x21, 0xAD, 0x9D, 0xB4, 0x83, 0x24, 0x21, 0xBD, 0x1B, 0x14, 0x9C, 0x15, 0xCB, 0x47, 0x32, 
    0x60, 0xEC, 0x24, 0x5F, 0xE3, 0x58, 0xD0, 0x66, 0x04, 0x3E, 0x11, 0xCE, 0x48, 0x1B, 0x55, 0x69, 
    0xA6, 0x51, 0xA6, 0x8B, 0x8C, 0xE9, 0x90, 0x9E, 0x15, 0x65, 0x22, 0xB1, 0x5E, 0x4F, 0x6C, 0x87, 
    0x26, 0x78, 0x8F, 0xF5, 0x60, 0x3C, 0x51, 0x58, 0x4E, 0xAD, 0x7B, 0x0A, 0x5D, 0x07, 0x4A, 0x5D, 
    0xA0, 0x18, 0xBA, 0x5A, 0x69, 0x2A, 0xD2, 0x57, 0x48, 0x71, 0x3F, 0x7E, 0x87, 0x2B, 0xF3, 0xCF, 
    0xC2, 0xEC, 0x3D, 0x47, 0x4C, 0x86, 0x37, 0x4D, 0xD3, 0x01, 0x31, 0x68, 0xF1, 0x04, 0x14, 0xCC, 
    0x43, 0xA8, 0x0E, 0x17, 0xD4, 0x06, 0x35, 0xF7, 0x3F, 0x75, 0xC9, 0x8F, 0xD8, 0x1D, 0x7B, 0x3F
};

#ifdef VERIFY
unsigned char hash[20] =
{
    0x33, 0xFE, 0x12, 0x21, 0x46, 0x8B, 0x93, 0x2C, 0x49, 0x05, 0xAA, 0xFF, 0x30, 0xCE, 0x00, 0x4A,
    0x71, 0xB5, 0x41, 0xED
};
#endif

int main(int argc, char *argv[])
{
    SHA_INFO sha_info;
    sha_init(&sha_info);
	sha_update(&sha_info, rawData, sizeof(rawData));
    sha_final(&sha_info);
#ifdef VERIFY
    if (memcmp(sha_info.digest, hash, sizeof(hash)) != 0)
    {
        printf("verification failed!\n");
        sha_print(&sha_info);
    }
#endif
    return 0;
}
