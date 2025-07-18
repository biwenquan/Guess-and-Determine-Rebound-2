#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define Nb 8
#define BLOCKLEN 32
#define Nk 8
#define Nr 14
#define keyExpSize 480


static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

static const uint8_t Rcon[20] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
  0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63};
static const uint8_t Diff_X[5][32] = {
 //0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31 
{0x0e,0x98,0x00,0x00,0x0e,0x98,0x00,0x00,0x0e,0xa8,0xe3,0x00,0x0e,0xa8,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x4b,0x4b,0xe0,0x00,0xfb,0xbc,0x00,0x0c,0xc8,0x0d,0x4e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0xfa,0x00,0xab},
{0x45,0x00,0x00,0x00,0x00,0xa6,0xe7,0x00,0x00,0x00,0xf9,0x3e,0x00,0x00,0x19,0x85,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};
static const uint8_t Diff_Y[5][32] = {
 //0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31 
{0x4b,0xf4,0x00,0x00,0x70,0xaf,0x00,0x00,0x0a,0x90,0x07,0x00,0xc8,0xff,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x43,0xfb,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x02,0xa8,0xa5,0x00,0x6c,0x06,0x00,0x56,0x74,0xb4,0xa8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4e,0x1f,0x00,0x51,0x00,0xa6},
{0x0e,0x00,0x00,0x00,0x00,0xa8,0xfa,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0xe3,0x58,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};
static const uint8_t Diff_K[5][32] = {
 //0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31 
{0x0e,0x98,0x00,0x00,0x0e,0x98,0x00,0x00,0x0e,0xa8,0xe3,0x00,0x0e,0xa8,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0x00,0x00},
{0x0e,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0xa8,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0xfa,0x58},
{0x0e,0xa8,0xe3,0x00,0x0e,0xa8,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0x00,0x00},
{0x0e,0xa8,0xe3,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x58,0x00,0x00,0xfa,0x58},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

static int value_X[5][32] = {0};
static int value_Y[5][32] = {0};
static int value_Z[5][32] = {0};
static int value_W[5][32] = {0};
static int value_K[5][32] = {0};

static int value_X_1[5][32] = {0};
static int value_Y_1[5][32] = {0};
static int value_Z_1[5][32] = {0};
static int value_W_1[5][32] = {0};
static int value_K_1[5][32] = {0};

static int value_all_X[5][32][4] = {0}; 
static int value_all_Y[5][32][4] = {0};
static int value_all_K[5][32][4] = {0};

int diff_num[5][32] = {0};
int diff_k131_num = 0;
int diff_k130_num = 0;

static uint8_t test_state[32] = {0};
static uint8_t test_key192[32] = {0};
static uint8_t RoundKey[keyExpSize];

typedef struct{
    int num;
    uint8_t term[4][2];
} node;
int sbox_ddt_ID[256][256] = {0};
node sbox_ddt_term[256][256] = {0};

int gen_sbox_ddt() {
    int res = 0;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_ID[i][j] = 0;
        }
    }
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_ID[i^j][sbox[i]^sbox[j]]++;
        }
    }
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(sbox_ddt_ID[i][j] > res && sbox_ddt_ID[i][j] != 256) {
                res = sbox_ddt_ID[i][j];
            }
        }
    }
    printf("sbox_ddt_ID[0][0] %d\n", sbox_ddt_ID[0][0]);
    printf("aes sbox diff max num is %d\n", res);
    return res;
}

void gen_sbox_diff_term() {
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_term[i][j].num = 0;
        }
    }
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(i == j) continue;
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].term[sbox_ddt_term[i^j][sbox[i]^sbox[j]].num][0] = (uint8_t)i;
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].term[sbox_ddt_term[i^j][sbox[i]^sbox[j]].num][1] = (uint8_t)sbox[i];
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].num++;
        }
    }
    int res = 0;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(sbox_ddt_term[i][j].num > res && sbox_ddt_term[i][j].num != 256) {
                res = sbox_ddt_term[i][j].num;
            }
        }
    }
    printf("sbox_ddt_term[0][0].num %d\n", sbox_ddt_term[0][0].num);
    printf("aes sbox diff max num is %d\n", res);
    return;

}

void get_diff_nums() {
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 32; j++) {
            diff_num[i][j] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].num;
        }
    }
    for(int i = 0; i < 5; i++) {
        printf("aes %d  round sbox diff num is \n", i);
        for(int j = 0; j < 32; j++)  {
            printf("%d  ", diff_num[i][j]);
        }printf("\n");
    }
}

void get_diff_values(){
    memset(value_all_X, -1, 5*32*4*sizeof(int));
    memset(value_all_Y, -1, 5*32*4*sizeof(int));
    memset(value_all_K, -1, 5*32*4*sizeof(int));
    for(int i = 0; i < 5; i++) {
        for (int j = 0; j < 32; j++) {
            for(int k = 0; k < diff_num[i][j]; k++){
                value_all_X[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][0];
                value_all_Y[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][1];
            }
        }
    }
}

void get_diff_k_nums() {
    diff_k130_num = sbox_ddt_term[Diff_K[1][30]][Diff_K[1][1] ^ Diff_K[2][1]].num;
    printf("aes key1 sbox diff num is ");
    printf("%d  \n", diff_k130_num);
    printf("aes key1 sbox diff num is %d\n",sbox_ddt_term[0xfa][0x98^0xa8].num);

    diff_k131_num = sbox_ddt_term[Diff_K[1][31]][Diff_K[1][2] ^ Diff_K[2][2]].num;
    printf("aes key3 sbox diff num is ");
    printf("%d  \n", diff_k131_num);
    printf("aes key3 sbox diff num is %d\n",sbox_ddt_term[0x58][0xe3].num);

}

void get_diff_K_values(){
    memset(value_all_K, -1, 5*32*4*sizeof(int));
    
    for(int k = 0; k < diff_k130_num; k++){
        value_all_K[1][30][k] = sbox_ddt_term[Diff_K[1][30]][Diff_K[1][1] ^ Diff_K[2][1]].term[k][0];
    }
    for(int k = 0; k < diff_k131_num; k++){
        value_all_K[1][31][k] = sbox_ddt_term[Diff_K[1][31]][Diff_K[1][2] ^ Diff_K[2][2]].term[k][0];
    }

}


uint8_t mul1(uint8_t x){
    uint8_t out = x;
    return out;
}

uint8_t mul2(uint8_t x){
    uint8_t out = ((x<<1) ^ (((x>>7) & 1) * 0x1b));
    return out;
}

uint8_t mul3(uint8_t x){
    uint8_t out = mul2(x) ^ x;
    return out;
}

uint8_t mul4(uint8_t x){
    uint8_t out = mul2(mul2(x));
    return out;
}

uint8_t mul5(uint8_t x){
    uint8_t out = mul4(x) ^ (x);
    return out;
}

uint8_t mul6(uint8_t x){
    uint8_t out = mul2(mul3(x));
    return out;
}

uint8_t mul7(uint8_t x){
    uint8_t out = mul4(x) ^ mul3(x);
    return out;
}

uint8_t mul8(uint8_t x){
    uint8_t out = mul2(mul2(mul2(x)));
    return out;
}

uint8_t mul9(uint8_t x){
    uint8_t out = mul8(x) ^ x;
    return out;
}

uint8_t mulB(uint8_t x){
    uint8_t out = mul8(x) ^ mul2(x) ^ x;
    return out;
}

uint8_t mulD(uint8_t md_in){
    uint8_t out = mul8(md_in) ^ mul4(md_in) ^ md_in;
    return out;
}

uint8_t mulE(uint8_t me_in){
    uint8_t out = mul8(me_in) ^ mul4(me_in) ^ mul2(me_in);
    return out;
}

uint8_t mulE1(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul2(x))) ^ x;
    return out;
}

uint8_t mul4F(uint8_t x){
    uint8_t out = mul8(mul8(x)) ^ mul4(mul3(x)) ^ mul3(x);
    return out;
}

uint8_t mulCB(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ (mul8(x)) ^ (mul3(x));
    return out;
}

uint8_t mulF6(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul3(x))) ^ mul2(mul3(x));
    return out;
}

uint8_t mul8D(uint8_t x){
    uint8_t out = mul8(mul8(mul2(x))) ^ (mul4(mul3(x))) ^ (x);
    return out;
}

uint8_t mul52(uint8_t x){
    uint8_t out = mul8(mul8(x)) ^ (mul4(mul4(x))) ^ mul2(x);
    return out;
}

uint8_t mulD1(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ (mul4(mul4(x))) ^ (x);
    return out;
}


uint8_t mulE5(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul2(x))) ^ mul4(x) ^ (x);
    return out;
}

uint8_t mulC0(uint8_t x){
    uint8_t out = mul8(mul8(mul2(x))) ^ (mul8(mul8(x)));
    return out;
}


void State2Matrix(uint8_t state[4][Nb], const uint8_t input[32]) { 
    int i,j;
    for (i = 0; i < Nb; ++i) {
        for (j = 0; j < 4; ++j) {
            state[j][i] = *input++;
        }
    }
}

void Matrix2State(uint8_t output[32], uint8_t state[4][Nb]) { 
    int i,j;
    for (i = 0; i < Nb; ++i) {
        for (j = 0; j < 4; ++j) {
            *output++ = state[j][i];
        }
    }
}


void subBytes(uint8_t state[4][Nb]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < Nb; ++j) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}
void invSubBytes(uint8_t state[4][Nb]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < Nb; ++j) {
            state[i][j] = rsbox[state[i][j]];
        }
    }
}
void ShiftRows(uint8_t state[4][Nb]){ 
    int i,j;
    uint8_t temp[4][Nb];
    for(i=0;i<4;i++){
        for(j=0;j<Nb;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<Nb;j++){
            int n = (i < 2) ? i : (i+1);
            state[i][j]=temp[i][(j+n)%Nb]; 
        }
    }
}
void InShiftRows(uint8_t state[4][Nb]){  
    int i,j;
    uint8_t temp[4][Nb];
    for(i=0;i<4;i++){
        for(j=0;j<Nb;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<Nb;j++){
            int n = (i < 2) ? i : (i+1);
            state[i][j]=temp[i][(Nb+j-n)%Nb]; 
        }
    }
}

uint8_t XTIME(uint8_t x) {  
	return ((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00)); 
}

uint8_t multiply(uint8_t a, uint8_t b) {
	unsigned char temp[8] = { a };
    uint8_t tempmultiply = 0x00;
	int i;
	for (i = 1; i < 8; i++) {
		temp[i] = XTIME(temp[i - 1]); 
	}
	tempmultiply = (b & 0x01) * a;
	for (i = 1; i <= 7; i++) {
		tempmultiply ^= (((b >> i) & 0x01) * temp[i]); 
	}
	return tempmultiply;
}

void MixColumns(uint8_t state[4][Nb]){
    int i,j;
    uint8_t temp[4][Nb];
    uint8_t M[4][4]={
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < Nb; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < Nb; ++j) { 
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void InMixColumns (uint8_t state[4][Nb]){
    int i,j;
    uint8_t temp[4][Nb];
    uint8_t M[4][4]={
        {0x0E, 0x0B, 0x0D, 0x09},
        {0x09, 0x0E, 0x0B, 0x0D},
        {0x0D, 0x09, 0x0E, 0x0B},
        {0x0B, 0x0D, 0x09, 0x0E}}; 
    for(i = 0; i < 4; ++i) {
        for (j = 0; j < Nb; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < Nb; ++j) {
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void AddRoundKey(uint8_t state[4][Nb],uint8_t W[4][Nb]){  
    int i,j;
    for(i=0;i<Nb;i++){
        for(j=0;j<4;j++){
            state[j][i]^=W[j][i];
        }
    }
}

static void KeyExpansion16(uint8_t *Key)
{
    uint32_t i, k;
    uint8_t tempa[4]; 

    for (i = 0; i < Nk; ++i)
    {
        RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
        RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
        RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
        RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
    }

    for (i = Nk; i < Nb * (Nr + 1); ++i)
    {
        {
        tempa[0]=RoundKey[(i-1) * 4 + 0];
        tempa[1]=RoundKey[(i-1) * 4 + 1];
        tempa[2]=RoundKey[(i-1) * 4 + 2];
        tempa[3]=RoundKey[(i-1) * 4 + 3];
        }
        if((Nk == 8) && (i % Nk == 4)) {
            tempa[0] = sbox[tempa[0]];
            tempa[1] = sbox[tempa[1]];
            tempa[2] = sbox[tempa[2]];
            tempa[3] = sbox[tempa[3]];
        } else if (i % Nk == 0) {
            {
                k = tempa[0];
                tempa[0] = tempa[1];
                tempa[1] = tempa[2];
                tempa[2] = tempa[3];
                tempa[3] = k;
            }
            {
                tempa[0] = sbox[tempa[0]];
                tempa[1] = sbox[tempa[1]];
                tempa[2] = sbox[tempa[2]];
                tempa[3] = sbox[tempa[3]];
            }

            tempa[0] =  tempa[0] ^ Rcon[i/Nk];
        } 
        RoundKey[i * 4 + 0] = RoundKey[(i - Nk) * 4 + 0] ^ tempa[0];
        RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] ^ tempa[1];
        RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] ^ tempa[2];
        RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] ^ tempa[3];
    }
}


static void KeyExpansion(uint8_t *key,uint8_t w[][4][Nb]){
    int i,j,r;
    KeyExpansion16(key);
    for(r = 0; r <= Nr; r++) {
        for(i=0;i<4;i++){
            for(j=0;j<Nb;j++){
                w[r][i][j]=RoundKey[r*32+i+j*4];
            }
        }
    } 
}


void subByte16(uint8_t *RoundText){
    for(int i=0;i<32;i++)
        RoundText[i]=sbox[RoundText[i]];
}
void InvSubByte16(uint8_t *RoundText){
    for(int i=0;i<32;i++)
        RoundText[i]=rsbox[RoundText[i]];
}
void InvSubByte16_int(int *RoundText){
    for(int i=0;i<32;i++)
        RoundText[i]=rsbox[RoundText[i]];
}
void ShiftRow16(uint8_t *RoundText) {
    uint8_t t;
    //row1
    t=RoundText[1];RoundText[1]=RoundText[5];RoundText[5]=RoundText[9];RoundText[9]=RoundText[13];RoundText[13]=RoundText[17]; 
    RoundText[17]=RoundText[21]; RoundText[21]=RoundText[25];RoundText[25]=RoundText[29];RoundText[29]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[14];RoundText[14]=RoundText[26];RoundText[26]=RoundText[6];RoundText[6]=RoundText[18];
    RoundText[18]=RoundText[30];RoundText[30]=RoundText[10];RoundText[10]=RoundText[22];RoundText[22]=t;
    //row3
    t=RoundText[3];RoundText[3]=RoundText[19];RoundText[19]=t;
    t=RoundText[7];RoundText[7]=RoundText[23]; RoundText[23]=t;
    t=RoundText[11];RoundText[11]=RoundText[27]; RoundText[27]=t;
    t=RoundText[15];RoundText[15]=RoundText[31]; RoundText[31]=t;
}
void ShiftRow16_int(int *RoundText) {
    int t;
    //row1
    t=RoundText[1];RoundText[1]=RoundText[5];RoundText[5]=RoundText[9];RoundText[9]=RoundText[13];RoundText[13]=RoundText[17]; 
    RoundText[17]=RoundText[21]; RoundText[21]=RoundText[25];RoundText[25]=RoundText[29];RoundText[29]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[14];RoundText[14]=RoundText[26];RoundText[26]=RoundText[6];RoundText[6]=RoundText[18];
    RoundText[18]=RoundText[30];RoundText[30]=RoundText[10];RoundText[10]=RoundText[22];RoundText[22]=t;
    //row3
    t=RoundText[3];RoundText[3]=RoundText[19];RoundText[19]=t;
    t=RoundText[7];RoundText[7]=RoundText[23]; RoundText[23]=t;
    t=RoundText[11];RoundText[11]=RoundText[27]; RoundText[27]=t;
    t=RoundText[15];RoundText[15]=RoundText[31]; RoundText[31]=t;
}
void InvShiftRow16_int(int *RoundText){
    int t;
    //row1
    t=RoundText[29];RoundText[29]=RoundText[25];RoundText[25]=RoundText[21];RoundText[21]=RoundText[17]; 
    RoundText[17]=RoundText[13];RoundText[13]=RoundText[9];RoundText[9]=RoundText[5];RoundText[5]=RoundText[1];RoundText[1]=t;
    //row2
    t=RoundText[22];RoundText[22]=RoundText[10];RoundText[10]=RoundText[30];RoundText[30]=RoundText[18];
    RoundText[18]=RoundText[6];RoundText[6]=RoundText[26];RoundText[26]=RoundText[14];RoundText[14]=RoundText[2];RoundText[2]=t;
    //row3
    t=RoundText[3];RoundText[3]=RoundText[19];RoundText[19]=t;
    t=RoundText[7];RoundText[7]=RoundText[23]; RoundText[23]=t;
    t=RoundText[11];RoundText[11]=RoundText[27]; RoundText[27]=t;
    t=RoundText[15];RoundText[15]=RoundText[31]; RoundText[31]=t;
}
void InvShiftRow16(uint8_t *RoundText){
    uint8_t t;
    //row1
    t=RoundText[29];RoundText[29]=RoundText[25];RoundText[25]=RoundText[21];RoundText[21]=RoundText[17]; 
    RoundText[17]=RoundText[13];RoundText[13]=RoundText[9];RoundText[9]=RoundText[5];RoundText[5]=RoundText[1];RoundText[1]=t;
    //row2
    t=RoundText[22];RoundText[22]=RoundText[10];RoundText[10]=RoundText[30];RoundText[30]=RoundText[18];
    RoundText[18]=RoundText[6];RoundText[6]=RoundText[26];RoundText[26]=RoundText[14];RoundText[14]=RoundText[2];RoundText[2]=t;
    //row3
    t=RoundText[3];RoundText[3]=RoundText[19];RoundText[19]=t;
    t=RoundText[7];RoundText[7]=RoundText[23]; RoundText[23]=t;
    t=RoundText[11];RoundText[11]=RoundText[27]; RoundText[27]=t;
    t=RoundText[15];RoundText[15]=RoundText[31]; RoundText[31]=t;
}
void MixColumn16(uint8_t* RoundText){
    uint8_t temp[4];
    for(int i=0;i<Nb;i++){
        temp[0]=mul2(RoundText[4*i])^mul3(RoundText[1+4*i])^RoundText[2+4*i]^RoundText[3+4*i];
        temp[1]=RoundText[4*i]^mul2(RoundText[1+4*i])^mul3(RoundText[2+4*i])^RoundText[3+4*i];
        temp[2]=RoundText[4*i]^RoundText[1+4*i]^mul2(RoundText[2+4*i])^mul3(RoundText[3+4*i]);
        temp[3]=mul3(RoundText[4*i])^RoundText[1+4*i]^RoundText[2+4*i]^mul2(RoundText[3+4*i]);
        RoundText[4*i]=temp[0];
        RoundText[1+4*i]=temp[1];
        RoundText[2+4*i]=temp[2];
        RoundText[3+4*i]=temp[3];
    }
}
void MixColumn16_1column(uint8_t* RoundText, int i){
    uint8_t temp[4];
    temp[0]=mul2(RoundText[4*i])^mul3(RoundText[1+4*i])^RoundText[2+4*i]^RoundText[3+4*i];
    temp[1]=RoundText[4*i]^mul2(RoundText[1+4*i])^mul3(RoundText[2+4*i])^RoundText[3+4*i];
    temp[2]=RoundText[4*i]^RoundText[1+4*i]^mul2(RoundText[2+4*i])^mul3(RoundText[3+4*i]);
    temp[3]=mul3(RoundText[4*i])^RoundText[1+4*i]^RoundText[2+4*i]^mul2(RoundText[3+4*i]);
    RoundText[4*i]=temp[0];
    RoundText[1+4*i]=temp[1];
    RoundText[2+4*i]=temp[2];
    RoundText[3+4*i]=temp[3];
}

void InvMixColumn16_1column(uint8_t* RoundText, int i){
    uint8_t temp[4]; 
    temp[0]=mulE(RoundText[4*i])^mulB(RoundText[1+4*i])^mulD(RoundText[2+4*i])^mul9(RoundText[3+4*i]);
    temp[1]=mul9(RoundText[4*i])^mulE(RoundText[1+4*i])^mulB(RoundText[2+4*i])^mulD(RoundText[3+4*i]);
    temp[2]=mulD(RoundText[4*i])^mul9(RoundText[1+4*i])^mulE(RoundText[2+4*i])^mulB(RoundText[3+4*i]);
    temp[3]=mulB(RoundText[4*i])^mulD(RoundText[1+4*i])^mul9(RoundText[2+4*i])^mulE(RoundText[3+4*i]);
    RoundText[4*i]=temp[0];
    RoundText[1+4*i]=temp[1];
    RoundText[2+4*i]=temp[2];
    RoundText[3+4*i]=temp[3];
}

void InvMixColumn16(uint8_t* RoundText) {
    uint8_t temp[4];
    for(int i=0;i<Nb;i++){
        temp[0]=mulE(RoundText[4*i])^mulB(RoundText[1+4*i])^mulD(RoundText[2+4*i])^mul9(RoundText[3+4*i]);
        temp[1]=mul9(RoundText[4*i])^mulE(RoundText[1+4*i])^mulB(RoundText[2+4*i])^mulD(RoundText[3+4*i]);
        temp[2]=mulD(RoundText[4*i])^mul9(RoundText[1+4*i])^mulE(RoundText[2+4*i])^mulB(RoundText[3+4*i]);
        temp[3]=mulB(RoundText[4*i])^mulD(RoundText[1+4*i])^mul9(RoundText[2+4*i])^mulE(RoundText[3+4*i]);
        RoundText[4*i]=temp[0];
        RoundText[1+4*i]=temp[1];
        RoundText[2+4*i]=temp[2];
        RoundText[3+4*i]=temp[3];
    }
}
void AddRoundKey16(uint8_t* RoundText,int round){
    for(int i=0;i<32;i++)
        RoundText[i]^=RoundKey[round*32+i];
}
void AES256encrypt16(uint8_t* RoundText, uint8_t *key, int R){
    KeyExpansion16(key);
    int round=0;
    AddRoundKey16(RoundText,round);
    for(round=1;round<R;round++){
        subByte16(RoundText);
        ShiftRow16(RoundText);
        MixColumn16(RoundText);
        AddRoundKey16(RoundText,round);
    }
    subByte16(RoundText);
    ShiftRow16(RoundText);
    AddRoundKey16(RoundText,round);
}

void aes_256_enc(uint8_t output[32], uint8_t key[32], int round, const uint8_t input[32]) {
    int i,j,k;
    uint8_t state[4][Nb];
    uint8_t w[15][4][Nb];
    KeyExpansion(key,w);
    State2Matrix(state,input); 
    AddRoundKey(state,w[0]);  
    for(i=1;i<=round;i++){
        subBytes(state);
        ShiftRows(state);
        if(i!=round)
            MixColumns(state);
        AddRoundKey(state,w[i]);
    }
    Matrix2State(output,state);
}

void aes_256_dec(uint8_t output[32], uint8_t key[32], int round, const uint8_t input[32]) {
    int i,j,k;
    uint8_t state[4][Nb];
    uint8_t w[15][4][Nb];
    KeyExpansion(key,w);
    State2Matrix(state,input);  
    for(i=round;i>=1;i--){
        AddRoundKey(state,w[i]);
        if(i!=round)
            InMixColumns(state);
        InShiftRows(state);
        invSubBytes(state);
    }
    AddRoundKey(state,w[0]);
    Matrix2State(output,state);
}
void AES256decrypt16(uint8_t* RoundText, uint8_t *key, int R){
    KeyExpansion16(key);
    int round=R;
    AddRoundKey16(RoundText,round);
    InvShiftRow16(RoundText);
    InvSubByte16(RoundText);
    for(round=R-1;round>0;round--){
        AddRoundKey16(RoundText,round);
        InvMixColumn16(RoundText);
        InvShiftRow16(RoundText);
        InvSubByte16(RoundText);
    }
    AddRoundKey16(RoundText,round);
}

void test_aes_256(){
    uint8_t in[32]  = { 0 };
    uint8_t key[32] = { 0 };
    uint8_t state[32] = {0};
    uint8_t out[32] = { 0 };
    //aes_256_dec(state, key, Nr, out);
    //AES256decrypt16(state, key, Nr);
    //aes_256_enc(state, key, Nr, in);
    AES256encrypt16(state, key, Nr);
    printf("cipher\n");
    for(int i = 0; i < 32; i++) {
        printf("%02x, ", state[i]);
    }printf("\n");
    
}

void compute_forward_first(int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    int tstate[32];
    for(int i = 0; i < 32; i++) {
        tstate[i] = Y[0][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 32; i++) {
        Z[0][i] = tstate[i];
    }
    for(int i = 0; i < 32; i++) {
        tstate[i] = Y[1][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 32; i++) {
        Z[1][i] = tstate[i];
    }
    for(int i = 0; i < 32; i++) {
        tstate[i] = Y[2][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 32; i++) {
        Z[2][i] = tstate[i];
    }
    
    W[0][31] = X[1][31] ^ K[1][31];

}


int guess_K1_1(int i, int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    K[1][1] = i;

    K[0][30] = rsbox[K[1][1] ^ K[0][1]];
    K[1][5] = K[1][1] ^ K[0][5];
    K[1][9] = K[1][5] ^ K[0][9];
    K[1][13] = K[1][9] ^ K[0][13];
    K[1][26] = K[1][30] ^ K[0][30];
    K[1][22] = K[1][26] ^ K[0][26];
    K[2][1] = K[1][1] ^ sbox[K[1][30]];
    K[2][5] = K[2][1] ^ K[1][5];
    K[2][9] = K[2][5] ^ K[1][9];
    K[2][13] = K[2][9] ^ K[1][13];

    W[0][5] = X[1][5] ^ K[1][5];
    W[0][9] = X[1][9] ^ K[1][9];
    W[0][13] = X[1][13] ^ K[1][13];
    W[0][26] = X[1][26] ^ K[1][26];
    W[1][5] = X[2][5] ^ K[2][5];
    X[0][30] = K[0][30];
    Y[0][30] = sbox[X[0][30]];
    Z[0][18] = Y[0][30];

    Z[0][10] = mulF6(Z[0][8] ^ W[0][9] ^ mul2(Z[0][9]) ^ Z[0][11]);
    W[0][8] = Z[0][10] ^ Z[0][11] ^ mul2(Z[0][8]) ^ mul3(Z[0][9]);
    W[0][10] = Z[0][8] ^ Z[0][9] ^ mul2(Z[0][10]) ^ mul3(Z[0][11]);
    W[0][11] = Z[0][9] ^ Z[0][10] ^ mul2(Z[0][11]) ^ mul3(Z[0][8]);
    K[1][11] = X[1][11] ^ W[0][11];
    K[1][8] = X[1][8] ^ W[0][8];
    Y[0][22] = Z[0][10];
    X[0][22] = rsbox[Y[0][22]];
    K[0][22] = X[0][22];
 
    K[1][4] = K[1][8] ^ K[0][8];
    K[1][0] = K[1][4] ^ K[0][4];
    K[0][29] = rsbox[K[1][0] ^ K[0][0] ^ Rcon[1]];
    K[1][12] = K[1][8] ^ K[0][12];
    K[1][18] = K[1][22] ^ K[0][22];
    W[0][4] = X[1][4] ^ K[1][4];
    W[0][12] = X[1][12] ^ K[1][12];
    X[0][29] = K[0][29];
    Y[0][29] = sbox[X[0][29]];
    Z[0][25] = Y[0][29];

    Z[0][6] = mul8D(W[0][4] ^ W[0][5] ^ Z[0][5] ^ mul3(Z[0][4]));
    Z[0][7] = W[0][4] ^ Z[0][6] ^ mul2(Z[0][4]) ^ mul3(Z[0][5]);
    W[0][6] = Z[0][4] ^ Z[0][5] ^ mul2(Z[0][6]) ^ mul3(Z[0][7]);
    W[0][7] = Z[0][5] ^ Z[0][6] ^ mul2(Z[0][7]) ^ mul3(Z[0][4]);

    Z[0][13] = W[0][12] ^ W[0][13] ^ mul2(Z[0][14]) ^ mul3(Z[0][12]);
    Z[0][15] = W[0][12] ^ Z[0][14] ^ mul2(Z[0][12]) ^ mul3(Z[0][13]);
    W[0][14] = Z[0][12] ^ Z[0][13] ^ mul2(Z[0][14]) ^ mul3(Z[0][15]);
    W[0][15] = Z[0][13] ^ Z[0][14] ^ mul2(Z[0][15]) ^ mul3(Z[0][12]);

    Y[0][18] = Z[0][6];
    X[0][18] = rsbox[Y[0][18]];
    K[0][18] = X[0][18];

    Y[0][23] = Z[0][7];
    X[0][23] = rsbox[Y[0][23]];
    K[0][23] = X[0][23];
    
    Y[0][17] = Z[0][13];
    X[0][17] = rsbox[Y[0][17]];
    K[0][17] = X[0][17];

    Y[0][31] = Z[0][15];
    X[0][31] = rsbox[Y[0][31]];
    K[0][31] = X[0][31];


    K[1][6] = X[1][6] ^ W[0][6];
    K[1][14] = X[1][14] ^ W[0][14]; 
    
    int tmp = sbox[K[1][14]] ^ K[0][18];
    bool filter1 = true;
    if(tmp == K[1][18]) {
        filter1 = true;
    } else {
        filter1 = false;
    }

    K[1][10] = K[1][6] ^ K[0][10];
    int tmp2 = K[1][14] ^ K[0][14];
    bool filter2 = true;
    if(tmp2 == K[1][10]) {
        filter2 = true;
    } else {
        filter2 = false;
    }
    K[1][17] = K[0][17] ^ sbox[K[1][13]];
    K[1][27] = K[1][31] ^ K[0][31];
    K[1][23] = K[1][27] ^ K[0][27];
    K[1][19] = K[1][23] ^ K[0][23];
    K[2][17] = K[1][17] ^ sbox[K[2][13]];
    W[0][27] = X[1][27] ^ K[1][27];
    X[1][10] = W[0][10] ^ K[1][10];
    Y[1][10] = sbox[X[1][10]];
    Z[1][30] = Y[1][10];
    if(filter1 && filter2) {
        return 1;
    } else {
        return 0;
    }

}

int guess_K0_2(int i, int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    K[0][2] = i;
    K[1][2] = K[0][2] ^ sbox[K[0][31]];
    K[2][2] = K[1][2] ^ sbox[K[1][31]];
    K[0][6] = K[1][6] ^ K[1][2];
    K[2][6] = K[1][6] ^ K[2][2];
    K[2][10] = K[2][6] ^ K[1][10];
    K[2][14] = K[2][10] ^ K[1][14];
    K[2][18] = K[1][18] ^ sbox[K[2][14]];
    K[2][22] = K[2][18] ^ K[1][22];
    K[2][26] = K[2][22] ^ K[1][26];
    K[2][30] = K[2][26] ^ K[1][30];

    W[1][6] = X[2][6] ^ K[2][6];
    W[1][10] = X[2][10] ^ K[2][10];
    W[1][14] = X[2][14] ^ K[2][14];

    X[0][2] = K[0][2];
    X[0][6] = K[0][6];
    Y[0][2] = sbox[X[0][2]];
    Y[0][6] = sbox[X[0][6]];
    Z[0][22] = Y[0][2];
    Z[0][26] = Y[0][6];

    Z[0][27] = mulD1(W[0][27] ^ mul3(W[0][26]) ^ mul2(Z[0][25]) ^ mul7(Z[0][26]));
    Z[0][24] = W[0][26] ^ Z[0][25] ^ mul2(Z[0][26]) ^ mul3(Z[0][27]);
    W[0][24] = Z[0][26] ^ Z[0][27] ^ mul2(Z[0][24]) ^ mul3(Z[0][25]);
    W[0][25] = Z[0][27] ^ Z[0][24] ^ mul2(Z[0][25]) ^ mul3(Z[0][26]);

    Z[1][6] = mulD1(W[1][6] ^ mul3(W[1][5]) ^ mul2(Z[1][4]) ^ mul7(Z[1][5]));
    Z[1][7] = W[1][5] ^ Z[1][4] ^ mul2(Z[1][5]) ^ mul3(Z[1][6]);
    W[1][4] = Z[1][6] ^ Z[1][7] ^ mul2(Z[1][4]) ^ mul3(Z[1][5]);
    W[1][7] = Z[1][5] ^ Z[1][6] ^ mul2(Z[1][7]) ^ mul3(Z[1][4]);

    Z[1][10] = mul8D(Z[1][8] ^ Z[1][9] ^ W[1][10] ^ mul3(Z[1][11]));
    W[1][8] = Z[1][10] ^ Z[1][11] ^ mul2(Z[1][8]) ^ mul3(Z[1][9]);
    W[1][9] = Z[1][11] ^ Z[1][8] ^ mul2(Z[1][9]) ^ mul3(Z[1][10]);
    W[1][11] = Z[1][9] ^ Z[1][10] ^ mul2(Z[1][11]) ^ mul3(Z[1][8]);

    Z[1][13] = W[1][14] ^ Z[1][12] ^ mul2(Z[1][14]) ^ mul3(Z[1][15]);
    W[1][12] = Z[1][14] ^ Z[1][15] ^ mul2(Z[1][12]) ^ mul3(Z[1][13]);
    W[1][13] = Z[1][12] ^ Z[1][15] ^ mul2(Z[1][13]) ^ mul3(Z[1][14]);
    W[1][15] = Z[1][13] ^ Z[1][14] ^ mul2(Z[1][15]) ^ mul3(Z[1][12]);

    K[2][11] = X[2][11] ^ W[1][11];
    K[2][15] = X[2][15] ^ W[1][15];
    X[2][13] = W[1][13] ^ K[2][13];
    X[2][9] = W[1][9] ^ K[2][9];

    Y[0][24] = Z[0][24];
    X[0][24] = rsbox[Y[0][24]];
    K[0][24] = X[0][24];

    Y[0][11] = Z[0][27];
    X[0][11] = rsbox[Y[0][11]];
    K[0][11] = X[0][11];

    Y[1][18] = Z[1][6];
    Y[1][23] = Z[1][7];
    Y[1][22] = Z[1][10];
    Y[1][17] = Z[1][13];
    X[1][17] = rsbox[Y[1][17]];
    X[1][18] = rsbox[Y[1][18]];
    X[1][22] = rsbox[Y[1][22]];
    X[1][23] = rsbox[Y[1][23]];
    W[0][17] = X[1][17] ^ K[1][17];
    W[0][18] = X[1][18] ^ K[1][18];
    W[0][22] = X[1][22] ^ K[1][22];
    W[0][23] = X[1][23] ^ K[1][23];

    K[1][7] = K[1][11] ^ K[0][11];
    K[2][7] = K[2][11] ^ K[1][11];
    K[2][3] = K[2][7] ^ K[1][7];
    K[1][15] = K[2][11] ^ K[2][15];
    K[2][19] = K[1][19] ^ sbox[K[2][15]];
    K[2][23] = K[2][19] ^ K[1][23];
    K[2][27] = K[2][23] ^ K[1][27];
    K[2][31] = K[2][27] ^ K[1][31];
    K[0][15] = K[1][11] ^ K[1][15];
    K[0][19] = K[1][19] ^ sbox[K[1][15]];

    X[0][15] = K[0][15];
    X[0][19] = K[0][19];
    Y[0][15] = sbox[X[0][15]];
    Y[0][19] = sbox[X[0][19]];
    Z[0][31] = Y[0][15];
    Z[0][3] = Y[0][19];

    X[2][7] = W[1][7] ^ K[2][7];
    X[1][7] = W[0][7] ^ K[1][7];
    X[1][15] = W[0][15] ^ K[1][15];
    Y[1][15] = sbox[X[1][15]];
    Y[1][7] = sbox[X[1][7]];
    Z[1][31] = Y[1][15];
    Z[1][23] = Y[1][7];

    uint8_t tstate1[32];
    for(int i = 0; i < 4; i++) {
        tstate1[i] = Z[0][i];
    }
    MixColumn16_1column(tstate1, 0);
    for(int i = 0; i < 4; i++) {
        W[0][i] = tstate1[i];
    }

    Z[0][28] = mulF6(W[0][31] ^ Z[0][29] ^ Z[0][30] ^ mul2(Z[0][31]));
    W[0][28] = Z[0][30] ^ Z[0][31] ^ mul2(Z[0][28]) ^ mul3(Z[0][29]);
    W[0][29] = Z[0][31] ^ Z[0][28] ^ mul2(Z[0][29]) ^ mul3(Z[0][30]);
    W[0][30] = Z[0][28] ^ Z[0][29] ^ mul2(Z[0][30]) ^ mul3(Z[0][31]);
    K[1][29] = X[1][29] ^ W[0][29];

    Y[0][28] = Z[0][28];
    X[0][28] = rsbox[Y[0][28]];
    K[0][28] = X[0][28];
    
    X[1][0] = W[0][0] ^ K[1][0];
    X[1][1] = W[0][1] ^ K[1][1];
    X[1][2] = W[0][2] ^ K[1][2];
    X[1][30] = W[0][30] ^ K[1][30];
    Y[1][0] = sbox[X[1][0]];
    Y[1][1] = sbox[X[1][1]];
    Y[1][2] = sbox[X[1][2]];
    Y[1][30] = sbox[X[1][30]];
    Z[1][0] = Y[1][0];
    Z[1][29] = Y[1][1];
    Z[1][22] = Y[1][2];
    Z[1][18] = Y[1][30];

    K[1][25] = K[1][29] ^ K[0][29];
    K[2][0] = K[1][0] ^ sbox[K[1][29]] ^ Rcon[2];
    K[2][4] = K[1][4] ^ K[2][0];
    K[2][8] = K[1][8] ^ K[2][4];
    K[2][12] = K[1][12] ^ K[2][8];
    
    X[1][25] = W[0][25] ^ K[1][25];
    Y[1][25] = sbox[X[1][25]];
    Z[1][21] = Y[1][25];

    X[2][12] = W[1][12] ^ K[2][12];
    X[2][8] = W[1][8] ^ K[2][8];
    X[2][4] = W[1][4] ^ K[2][4];
    W[1][0] = X[2][0] ^ K[2][0];

    Z[1][3] = W[1][0] ^ Z[1][2] ^ mul2(Z[1][0]) ^ mul3(Z[1][1]);
    W[1][1] = Z[1][0] ^ Z[1][3] ^ mul2(Z[1][1]) ^ mul3(Z[1][2]);
    W[1][2] = Z[1][0] ^ Z[1][1] ^ mul2(Z[1][2]) ^ mul3(Z[1][3]);
    W[1][3] = Z[1][1] ^ Z[1][2] ^ mul2(Z[1][3]) ^ mul3(Z[1][0]);

    X[2][1] = W[1][1] ^ K[2][1];
    X[2][2] = W[1][2] ^ K[2][2];
    X[2][3] = W[1][3] ^ K[2][3];
    Y[1][19] = Z[1][3];
    X[1][19] = rsbox[Y[1][19]];
    W[0][19] = X[1][19] ^ K[1][19];

    W[0][16] = mulE1(Z[0][18] ^ mul9(W[0][17]) ^ mulE(W[0][18]) ^ mulB(W[0][19]));
    Z[0][16] = mulD(W[0][18]) ^ mul9(W[0][19]) ^ mulE(W[0][16]) ^ mulB(W[0][17]);
    Z[0][17] = mulD(W[0][19]) ^ mul9(W[0][16]) ^ mulE(W[0][17]) ^ mulB(W[0][18]);
    Z[0][19] = mulD(W[0][17]) ^ mul9(W[0][18]) ^ mulE(W[0][19]) ^ mulB(W[0][16]);
    Y[0][16] = Z[0][16];
    X[0][16] = rsbox[Y[0][16]];
    K[0][16] = X[0][16];
    Y[0][21] = Z[0][17];
    X[0][21] = rsbox[Y[0][21]];
    K[0][21] = X[0][21];
    Y[0][3] = Z[0][19];
    X[0][3] = rsbox[Y[0][3]];
    K[0][3] = X[0][3];

    K[1][3] = K[0][3] ^ sbox[K[0][28]];
    K[0][7] = K[1][7] ^ K[1][3];
    K[1][16] = K[0][16] ^ sbox[K[1][12]];
    K[1][21] = K[0][21] ^ K[1][17];
    K[0][25] = K[1][25] ^ K[1][21];
    K[1][28] = rsbox[K[2][3] ^ K[1][3]];
    K[2][16] = K[1][16] ^ sbox[K[2][12]];
    K[2][21] = K[1][21] ^ K[2][17];
    K[2][25] = K[1][25] ^ K[2][21];
    K[2][29] = K[1][29] ^ K[2][25];
    K[1][24] = K[0][28] ^ K[1][28];
    K[1][20] = K[0][24] ^ K[1][24];
    K[0][20] = K[1][20] ^ K[1][16];
    K[2][20] = K[1][20] ^ K[2][16];
    K[2][24] = K[1][24] ^ K[2][20];
    K[2][28] = K[1][28] ^ K[2][24];

    X[0][7] = K[0][7];
    Y[0][7] = sbox[X[0][7]];
    Z[0][23] = Y[0][7];
    X[0][20] = K[0][20];
    Y[0][20] = sbox[X[0][20]];
    Z[0][20] = Y[0][20];
    X[0][25] = K[0][25];
    Y[0][25] = sbox[X[0][25]];
    Z[0][21] = Y[0][25];

    X[1][3] = W[0][3] ^ K[1][3];
    X[1][16] = W[0][16] ^ K[1][16];
    X[1][24] = W[0][24] ^ K[1][24];
    X[1][28] = W[0][28] ^ K[1][28];
    Y[1][3] = sbox[X[1][3]];
    Y[1][16] = sbox[X[1][16]];
    Y[1][24] = sbox[X[1][24]];
    Y[1][28] = sbox[X[1][28]];
    Z[1][16] = Y[1][16];
    Z[1][24] = Y[1][24];
    Z[1][28] = Y[1][28];
    Z[1][19] = Y[1][3];

    for(int i = 24; i < 32; i++) {
        tstate1[i] = Z[1][i];
    }
    MixColumn16_1column(tstate1, 6);
    MixColumn16_1column(tstate1, 7);
    for(int i = 24; i < 32; i++) {
        W[1][i] = tstate1[i];
    }

    for(int i = 20; i < 24; i++) {
        tstate1[i] = Z[0][i];
    }
    MixColumn16_1column(tstate1, 5);
    for(int i = 20; i < 22; i++) {
        W[0][i] = tstate1[i];
    }

    bool filter1 = true;
    if(tstate1[22] == W[0][22]) {
        filter1 = true;
    } else {
        filter1 = false;
    }

    bool filter2 = true;
    if(tstate1[23] == W[0][23]) {
        filter2 = true;
    } else {
        filter2 = false;
    }
    
    int res;
    if(filter1 && filter2) {
        res = 1;
    } else {
        res = 0;
    }

    if(res == 0) {
        return 0;
    } else {
        X[1][20] = W[0][20] ^ K[1][20];
        X[1][21] = W[0][21] ^ K[1][21];
        Y[1][20] = sbox[X[1][20]];
        Y[1][21] = sbox[X[1][21]];
        Z[1][20] = Y[1][20];
        Z[1][17] = Y[1][21];

        for(int i = 16; i < 24; i++) {
            tstate1[i] = Z[1][i];
        }
        MixColumn16_1column(tstate1, 4);
        MixColumn16_1column(tstate1, 5);
        for(int i = 16; i < 24; i++) {
            W[1][i] = tstate1[i];
        }

        for(int i = 0; i < 32; i++) {
            X[2][i] = W[1][i] ^ K[2][i];
        }

        for(int i = 0; i < 32; i++) {
            Y[2][i] = sbox[X[2][i]];
        }

        return 1;
    }

}


void filter_search2(){
    memset(value_X, -1, 5*32*sizeof(int));
    memset(value_Y, -1, 5*32*sizeof(int));
    memset(value_Z, -1, 5*32*sizeof(int));
    memset(value_W, -1, 5*32*sizeof(int));
    memset(value_K, -1, 5*32*sizeof(int));
    memset(value_X_1, -1, 5*32*sizeof(int));
    memset(value_Y_1, -1, 5*32*sizeof(int));
    memset(value_Z_1, -1, 5*32*sizeof(int));
    memset(value_W_1, -1, 5*32*sizeof(int));
    memset(value_K_1, -1, 5*32*sizeof(int));
    uint64_t count = 0; 
    int count2 = 0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 32; j++){
            if ((diff_num[i][j]))
            {
                value_X[i][j] = value_all_X[i][j][0];
                value_Y[i][j] = value_all_Y[i][j][0];
                value_X_1[i][j] = Diff_X[i][j] ^ value_X[i][j];
                value_Y_1[i][j] = Diff_Y[i][j] ^ value_Y[i][j];
            }
        }
    }

    int mm = 8;
    value_X[0][mm] = value_all_X[0][mm][1];
    value_Y[0][mm] = value_all_Y[0][mm][1];
    value_X_1[0][mm] = Diff_X[0][mm] ^ value_X[0][mm];
    value_Y_1[0][mm] = Diff_Y[0][mm] ^ value_Y[0][mm];

    for(int j = 0; j < 32; j++){
        if ((diff_num[0][j])){
            value_K[0][j] = value_X[0][j];
            value_K_1[0][j] = value_X_1[0][j];
        }
    }

    value_K[1][30] = value_all_K[1][30][0];
    value_K_1[1][30] = value_K[1][30] ^ Diff_K[1][30];

    value_K[1][31] = value_all_K[1][31][0];
    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];

    value_X[1][4] = value_all_X[1][4][1];
    value_Y[1][4] = value_all_Y[1][4][1];
    value_X_1[1][4] = Diff_X[1][4] ^ value_X[1][4];
    value_Y_1[1][4] = Diff_Y[1][4] ^ value_Y[1][4];

    int a0[3] = {4,6,9};

    for(int s0 = 0; s0 < 3; s0++) {
        value_X[1][a0[s0]] = value_all_X[1][a0[s0]][1];
        value_Y[1][a0[s0]] = value_all_Y[1][a0[s0]][1];
        value_X_1[1][a0[s0]] = Diff_X[1][a0[s0]] ^ value_X[1][a0[s0]];
        value_Y_1[1][a0[s0]] = Diff_Y[1][a0[s0]] ^ value_Y[1][a0[s0]];
    }

    compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
    compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

    int tt1 = guess_K1_1(159, value_X, value_Y, value_Z, value_W, value_K);
    int tt2 = guess_K1_1(Diff_K[1][1] ^ 159, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
    printf("tt1: %d, tt2: %d, \n", tt1 , tt2);

    for(int index0 = 0; index0 < 2; index0++){ 
        uint8_t mn0[32] = {0};
        int a0[1] = {0};
        int tt0 = index0;
        for(int s0 = 0; s0 < 1; s0++){
            mn0[a0[s0]] = tt0 % 2;
            tt0 = tt0 >> 1;
        }
        for(int s0 = 0; s0 < 1; s0++) {
            value_X[0][a0[s0]] = value_all_X[0][a0[s0]][mn0[a0[s0]]];
            value_Y[0][a0[s0]] = value_all_Y[0][a0[s0]][mn0[a0[s0]]];
            value_X_1[0][a0[s0]] = Diff_X[0][a0[s0]] ^ value_X[0][a0[s0]];
            value_Y_1[0][a0[s0]] = Diff_Y[0][a0[s0]] ^ value_Y[0][a0[s0]];
        }

        for(int s0 = 0; s0 < 1; s0++) {
            value_K[0][a0[s0]] = value_X[0][a0[s0]];
            value_K_1[0][a0[s0]] = value_X_1[0][a0[s0]];
        }

        for(int flag0 = 0; flag0 < 2; flag0++){ 
        for(int index1 = 0; index1 < 16; index1++){ 
            uint8_t mn1[32] = {0};
            int a1[4] = {26,27,29,31};
            int tt1 = index1;
            for(int s1 = 0; s1 < 4; s1++){
                mn1[a1[s1]] = tt1 % 2;
                tt1 = tt1 >> 1;
            }
            for(int s1 = 0; s1 < 4; s1++) {
                value_X[1][a1[s1]] = value_all_X[1][a1[s1]][mn1[a1[s1]]];
                value_Y[1][a1[s1]] = value_all_Y[1][a1[s1]][mn1[a1[s1]]];
                value_X_1[1][a1[s1]] = Diff_X[1][a1[s1]] ^ value_X[1][a1[s1]];
                value_Y_1[1][a1[s1]] = Diff_Y[1][a1[s1]] ^ value_Y[1][a1[s1]];
            }
            if(flag0 == 1) {
                value_X[1][26] = value_all_X[1][26][mn0[26]+2];
                value_Y[1][26] = value_all_Y[1][26][mn0[26]+2];
                value_X_1[1][26] = Diff_X[1][26] ^ value_X[1][26];
                value_Y_1[1][26] = Diff_Y[1][26] ^ value_Y[1][26];
            }

            for(int flag1 = 0; flag1 < 2; flag1++){ 
            for(int index2 = 0; index2 < 128; index2++) { 
                uint8_t mn2[32] = {0};
                int a2[7] = {0,5,6,10,11,14,15};
                int tt2 = index2;
                for(int s2 = 0; s2 < 7; s2++){
                    mn2[a2[s2]] = tt2 % 2;
                    tt2 = tt2 >> 1;
                }
                for(int s2 = 0; s2 < 7; s2++) {
                    value_X[2][a2[s2]] = value_all_X[2][a2[s2]][mn2[a2[s2]]];
                    value_Y[2][a2[s2]] = value_all_Y[2][a2[s2]][mn1[a2[s2]]];
                    value_X_1[2][a2[s2]] = Diff_X[2][a2[s2]] ^ value_X[2][a2[s2]];
                    value_Y_1[2][a2[s2]] = Diff_Y[2][a2[s2]] ^ value_Y[2][a2[s2]];
                }
                if(flag1 == 1) {
                    value_X[2][10] = value_all_X[2][10][mn0[10]+2];
                    value_Y[2][10] = value_all_Y[2][10][mn0[10]+2];
                    value_X_1[2][10] = Diff_X[2][10] ^ value_X[2][10];
                    value_Y_1[2][10] = Diff_Y[2][10] ^ value_Y[2][10];
                }

                for(int index3 = 0; index3 < 2; index3++) {
                    value_K[1][31] = value_all_K[1][31][index3];
                    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];

                    for(int k02 = 0; k02 < 256; k02++) { 
                        int t1 = guess_K0_2(k02, value_X, value_Y, value_Z, value_W, value_K);
                        int t2 = guess_K0_2(k02, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

                        count++;
                        uint8_t ttemp[32] = {0}; 
                        uint8_t ttemp1[32] = {0};    
                        
                        uint8_t tmp[32];
                        for(int i = 0; i < 32; i++){
                            tmp[i] = value_K[i/32][i%32];
                        }
                        AES256encrypt16(ttemp, tmp, 3);

                        uint8_t tmp1[32];
                        for(int i = 0; i < 32; i++){
                            tmp1[i] = value_K_1[i/32][i%32];
                        }
                        AES256encrypt16(ttemp1, tmp1, 3);

                        bool test_collision = true;
                        for(int i = 0; i < 32; i++) {
                            if((ttemp1[i] ^ ttemp[i])) {
                                test_collision = false;
                            }
                        }
                        
                        if(test_collision) {
                            count2++;
                            printf("collison find:%d\n",count);                                    
                            printf("k02 %d, index0 %d, index1 %d, index2 %d, index3 %d,\n", k02, index0, index1, index2, index3);
                            
                            printf("K0\n");
                            for(int i = 0; i < 32; i++) {
                                printf("%x, ", tmp[i]);
                            }printf("\n");
                            
                            printf("K1\n");
                            for(int i = 0; i < 32; i++) {
                                printf("%x, ", tmp1[i]);
                            }printf("\n");
                        }
                                    
                    }
                }
            }
            }
        }
        }
    }
    
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);
}


void filter_search3(){
    memset(value_X, -1, 5*32*sizeof(int));
    memset(value_Y, -1, 5*32*sizeof(int));
    memset(value_Z, -1, 5*32*sizeof(int));
    memset(value_W, -1, 5*32*sizeof(int));
    memset(value_K, -1, 5*32*sizeof(int));
    memset(value_X_1, -1, 5*32*sizeof(int));
    memset(value_Y_1, -1, 5*32*sizeof(int));
    memset(value_Z_1, -1, 5*32*sizeof(int));
    memset(value_W_1, -1, 5*32*sizeof(int));
    memset(value_K_1, -1, 5*32*sizeof(int));
    uint64_t count = 0; 
    int count2 = 0;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 32; j++){
            if ((diff_num[i][j]))
            {
                value_X[i][j] = value_all_X[i][j][0];
                value_Y[i][j] = value_all_Y[i][j][0];
                value_X_1[i][j] = Diff_X[i][j] ^ value_X[i][j];
                value_Y_1[i][j] = Diff_Y[i][j] ^ value_Y[i][j];
            }
        }
    }

    int mm = 4; 
    value_X[0][mm] = value_all_X[0][mm][1];
    value_Y[0][mm] = value_all_Y[0][mm][1];
    value_X_1[0][mm] = Diff_X[0][mm] ^ value_X[0][mm];
    value_Y_1[0][mm] = Diff_Y[0][mm] ^ value_Y[0][mm];

    for(int j = 0; j < 32; j++){
        if ((diff_num[0][j])){
            value_K[0][j] = value_X[0][j];
            value_K_1[0][j] = value_X_1[0][j];
        }
    }
    value_K[1][30] = value_all_K[1][30][0];
    value_K_1[1][30] = value_K[1][30] ^ Diff_K[1][30];

    value_K[1][31] = value_all_K[1][31][0];
    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];

    int a0[5] = {5,8,9,12,13};

    for(int s0 = 0; s0 < 5; s0++) {
        value_X[1][a0[s0]] = value_all_X[1][a0[s0]][1];
        value_Y[1][a0[s0]] = value_all_Y[1][a0[s0]][1];
        value_X_1[1][a0[s0]] = Diff_X[1][a0[s0]] ^ value_X[1][a0[s0]];
        value_Y_1[1][a0[s0]] = Diff_Y[1][a0[s0]] ^ value_Y[1][a0[s0]];
    }

    compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
    compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

    int tt1 = guess_K1_1(104, value_X, value_Y, value_Z, value_W, value_K);
    int tt2 = guess_K1_1(Diff_K[1][1] ^ 104, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
    printf("tt1: %d, tt2: %d, \n", tt1 , tt2);

    for(int index0 = 0; index0 < 2; index0++){ 
        uint8_t mn0[32] = {0};
        int a0[1] = {0};
        int tt0 = index0;
        for(int s0 = 0; s0 < 1; s0++){
            mn0[a0[s0]] = tt0 % 2;
            tt0 = tt0 >> 1;
        }
        for(int s0 = 0; s0 < 1; s0++) {
            value_X[0][a0[s0]] = value_all_X[0][a0[s0]][mn0[a0[s0]]];
            value_Y[0][a0[s0]] = value_all_Y[0][a0[s0]][mn0[a0[s0]]];
            value_X_1[0][a0[s0]] = Diff_X[0][a0[s0]] ^ value_X[0][a0[s0]];
            value_Y_1[0][a0[s0]] = Diff_Y[0][a0[s0]] ^ value_Y[0][a0[s0]];
        }

        for(int s0 = 0; s0 < 1; s0++) {
            value_K[0][a0[s0]] = value_X[0][a0[s0]];
            value_K_1[0][a0[s0]] = value_X_1[0][a0[s0]];
        }

        for(int flag0 = 0; flag0 < 2; flag0++){ 
        for(int index1 = 0; index1 < 16; index1++){ 
            uint8_t mn1[32] = {0};
            int a1[4] = {26,27,29,31};
            int tt1 = index1;
            for(int s1 = 0; s1 < 4; s1++){
                mn1[a1[s1]] = tt1 % 2;
                tt1 = tt1 >> 1;
            }
            for(int s1 = 0; s1 < 4; s1++) {
                value_X[1][a1[s1]] = value_all_X[1][a1[s1]][mn1[a1[s1]]];
                value_Y[1][a1[s1]] = value_all_Y[1][a1[s1]][mn1[a1[s1]]];
                value_X_1[1][a1[s1]] = Diff_X[1][a1[s1]] ^ value_X[1][a1[s1]];
                value_Y_1[1][a1[s1]] = Diff_Y[1][a1[s1]] ^ value_Y[1][a1[s1]];
            }
            if(flag0 == 1) {
                value_X[1][26] = value_all_X[1][26][mn0[26]+2];
                value_Y[1][26] = value_all_Y[1][26][mn0[26]+2];
                value_X_1[1][26] = Diff_X[1][26] ^ value_X[1][26];
                value_Y_1[1][26] = Diff_Y[1][26] ^ value_Y[1][26];
            }

            for(int flag1 = 0; flag1 < 2; flag1++){ 
            for(int index2 = 0; index2 < 128; index2++) { 
                uint8_t mn2[32] = {0};
                int a2[7] = {0,5,6,10,11,14,15};
                int tt2 = index2;
                for(int s2 = 0; s2 < 7; s2++){
                    mn2[a2[s2]] = tt2 % 2;
                    tt2 = tt2 >> 1;
                }
                for(int s2 = 0; s2 < 7; s2++) {
                    value_X[2][a2[s2]] = value_all_X[2][a2[s2]][mn2[a2[s2]]];
                    value_Y[2][a2[s2]] = value_all_Y[2][a2[s2]][mn1[a2[s2]]];
                    value_X_1[2][a2[s2]] = Diff_X[2][a2[s2]] ^ value_X[2][a2[s2]];
                    value_Y_1[2][a2[s2]] = Diff_Y[2][a2[s2]] ^ value_Y[2][a2[s2]];
                }
                if(flag1 == 1) {
                    value_X[2][10] = value_all_X[2][10][mn0[10]+2];
                    value_Y[2][10] = value_all_Y[2][10][mn0[10]+2];
                    value_X_1[2][10] = Diff_X[2][10] ^ value_X[2][10];
                    value_Y_1[2][10] = Diff_Y[2][10] ^ value_Y[2][10];
                }

                for(int index3 = 0; index3 < 2; index3++) {
                    value_K[1][31] = value_all_K[1][31][index3];
                    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];

                    for(int k02 = 0; k02 < 256; k02++) { 
                        int t1 = guess_K0_2(k02, value_X, value_Y, value_Z, value_W, value_K);
                        int t2 = guess_K0_2(k02, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                            
                        count++;
                        uint8_t ttemp[32] = {0}; 
                        uint8_t ttemp1[32] = {0};    
                        
                        uint8_t tmp[32];
                        for(int i = 0; i < 32; i++){
                            tmp[i] = value_K[i/32][i%32];
                        }
                        AES256encrypt16(ttemp, tmp, 3);

                        uint8_t tmp1[32];
                        for(int i = 0; i < 32; i++){
                            tmp1[i] = value_K_1[i/32][i%32];
                        }
                        AES256encrypt16(ttemp1, tmp1, 3);

                        bool test_collision = true;
                        for(int i = 0; i < 32; i++) {
                            if((ttemp1[i] ^ ttemp[i])) {
                                test_collision = false;
                            }
                        }
                        
                        if(test_collision) {
                            count2++;
                            printf("collison find:%d\n",count);                                    
                            printf("k02 %d, index0 %d, index1 %d, index2 %d, index3 %d,\n", k02, index0, index1, index2, index3);
                            
                            printf("K0\n");
                            for(int i = 0; i < 32; i++) {
                                printf("%x, ", tmp[i]);
                            }printf("\n");
                            
                            printf("K1\n");
                            for(int i = 0; i < 32; i++) {
                                printf("%x, ", tmp1[i]);
                            }printf("\n");
                        }
                    
                
                    }
                }
            }
            }
        }
        }
    }
    
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);


}



void filter_search(){
    memset(value_X, -1, 5*32*sizeof(int));
    memset(value_Y, -1, 5*32*sizeof(int));
    memset(value_Z, -1, 5*32*sizeof(int));
    memset(value_W, -1, 5*32*sizeof(int));
    memset(value_K, -1, 5*32*sizeof(int));
    memset(value_X_1, -1, 5*32*sizeof(int));
    memset(value_Y_1, -1, 5*32*sizeof(int));
    memset(value_Z_1, -1, 5*32*sizeof(int));
    memset(value_W_1, -1, 5*32*sizeof(int));
    memset(value_K_1, -1, 5*32*sizeof(int));
    uint64_t count = 0; 
    int count2 = 0;

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 32; j++){
            if ((diff_num[i][j]))
            {
                value_X[i][j] = value_all_X[i][j][0];
                value_Y[i][j] = value_all_Y[i][j][0];
                value_X_1[i][j] = Diff_X[i][j] ^ value_X[i][j];
                value_Y_1[i][j] = Diff_Y[i][j] ^ value_Y[i][j];
            }
        }
    }

    for(int j = 0; j < 32; j++){
        if ((diff_num[0][j])){
            value_K[0][j] = value_X[0][j];
            value_K_1[0][j] = value_X_1[0][j];
        }
    }

    value_K[1][30] = value_all_K[1][30][0];
    value_K_1[1][30] = value_K[1][30] ^ Diff_K[1][30];

    value_K[1][31] = value_all_K[1][31][0];
    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];
    
    for(int index0 = 0; index0 < 16; index0++){ 
        uint8_t mn0[32] = {0};
        int a0[11] = {1,4,5,8,9,10,12,13,14,26,27};
        int tt0 = index0;
        for(int s0 = 0; s0 < 11; s0++){
            mn0[a0[s0]] = tt0 % 2;
            tt0 = tt0 >> 1;
        }
        for(int s0 = 0; s0 < 11; s0++) {
            value_X[0][a0[s0]] = value_all_X[0][a0[s0]][mn0[a0[s0]]];
            value_Y[0][a0[s0]] = value_all_Y[0][a0[s0]][mn0[a0[s0]]];
            value_X_1[0][a0[s0]] = Diff_X[0][a0[s0]] ^ value_X[0][a0[s0]];
            value_Y_1[0][a0[s0]] = Diff_Y[0][a0[s0]] ^ value_Y[0][a0[s0]];
        }

        for(int j = 0; j < 32; j++){
            if ((diff_num[0][j])){
                value_K[0][j] = value_X[0][j];
                value_K_1[0][j] = value_X_1[0][j];
            }
        }

        for(int index1 = 0; index1 < 256; index1++){ 
            uint8_t mn1[32] = {0};
            int a1[8] = {4,5,6,8,9,12,13,14};
            int tt1 = index1;
            for(int s1 = 0; s1 < 8; s1++){
                mn1[a1[s1]] = tt1 % 2;
                tt1 = tt1 >> 1;
            }
            for(int s1 = 0; s1 < 8; s1++) {
                value_X[1][a1[s1]] = value_all_X[1][a1[s1]][mn1[a1[s1]]];
                value_Y[1][a1[s1]] = value_all_Y[1][a1[s1]][mn1[a1[s1]]];
                value_X_1[1][a1[s1]] = Diff_X[1][a1[s1]] ^ value_X[1][a1[s1]];
                value_Y_1[1][a1[s1]] = Diff_Y[1][a1[s1]] ^ value_Y[1][a1[s1]];
            }

            for(int index2 = 0; index2 < 2; index2++) { 
                value_K[1][30] = value_all_K[1][30][index2];
                value_K_1[1][30] = value_K[1][30] ^ Diff_K[1][30];
                
                compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
                compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                    for(int k11 = 0; k11 < 256; k11++) { 
                        int t1 = guess_K1_1(k11, value_X, value_Y, value_Z, value_W, value_K);
                        int t2 = guess_K1_1(Diff_K[1][1] ^ k11, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                            
                        count++;
                        if(t1 && t2) {
                            count2++;
                            printf("collison find:%d\n",count);                                    
                            printf("k11 %d, index0 %d, index1 %d, index2 %d, \n", k11, index0, index1, index2);
                            
                        }
                    }
                
            }
        }
    }
    
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);
}


void collision_search(){
    memset(value_X, -1, 5*32*sizeof(int));
    memset(value_Y, -1, 5*32*sizeof(int));
    memset(value_Z, -1, 5*32*sizeof(int));
    memset(value_W, -1, 5*32*sizeof(int));
    memset(value_K, -1, 5*32*sizeof(int));
    memset(value_X_1, -1, 5*32*sizeof(int));
    memset(value_Y_1, -1, 5*32*sizeof(int));
    memset(value_Z_1, -1, 5*32*sizeof(int));
    memset(value_W_1, -1, 5*32*sizeof(int));
    memset(value_K_1, -1, 5*32*sizeof(int));
    uint64_t count = 0; 
    int count2 = 0;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 32; j++){
            if ((diff_num[i][j]))
            {
                value_X[i][j] = value_all_X[i][j][1];
                value_Y[i][j] = value_all_Y[i][j][1];
                value_X_1[i][j] = Diff_X[i][j] ^ value_X[i][j];
                value_Y_1[i][j] = Diff_Y[i][j] ^ value_Y[i][j];
            }
        }
    }

    for(int j = 0; j < 32; j++){
        if ((diff_num[0][j])){
            value_K[0][j] = value_X[0][j];
            value_K_1[0][j] = value_X_1[0][j];
        }
    }

    value_K[1][30] = value_all_K[1][30][0];
    value_K_1[1][30] = value_K[1][30] ^ Diff_K[1][30];

    value_K[1][31] = value_all_K[1][31][0];
    value_K_1[1][31] = value_K[1][31] ^ Diff_K[1][31];

    compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
    compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

    for(int k38 = 0; k38 < 16; k38++) {  
        for(int k312 = 0; k312 < 16; k312++) {
            for(int k313 = 0; k313 < 16; k313++) {
                guess_K1_1(k38, value_X, value_Y, value_Z, value_W, value_K);
                guess_K1_1(k38, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                for(int k212 = 0; k212 < 16; k212++) {
                    for(int k213 = 0; k213 < 8; k213++) {
                        guess_K0_2(k212, value_X, value_Y, value_Z, value_W, value_K);
                        guess_K0_2(k212, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                        for(int k30 = 0; k30 < 8; k30++) {
                            for(int k34 = 0; k34 < 8; k34++) {
                                count++;
                                uint8_t ttemp[16] = {0}; 
                                uint8_t ttemp1[16] = {0};    
                                uint8_t P0[16] = {0}; 
                                uint8_t P1[16] = {0};                              
                                
                                for(int i = 0; i < 16; i++){
                                    P0[i] = ttemp[i];
                                    P1[i] = ttemp1[i];
                                }
                                uint8_t tmp[24];
                                for(int i = 0; i < 24; i++){
                                    tmp[i] = value_K[i/16][i%16];
                                }
                                AES256encrypt16(ttemp, tmp, 7);

                                uint8_t tmp1[24];
                                for(int i = 0; i < 24; i++){
                                    tmp1[i] = value_K_1[i/16][i%16];
                                }
                                AES256encrypt16(ttemp1, tmp1, 7);

                                bool test_collision = true;
                                for(int i = 0; i < 16; i++) {
                                    if((ttemp1[i] ^ ttemp[i]) || (P0[i] ^ P1[i])) {
                                        test_collision = false;
                                    }
                                }
                                for(int i = 0; i < 16; i++) {
                                    printf("%x, ", ttemp1[i] ^ ttemp[i]);
                                }printf("\n");

                                if(test_collision) {
                                    count2++;
                                    printf("collison find:%d\n",count);                                    
                                    printf("k38 %d,k312 %d,k313 %d,k212 %d, k213 %d, k30 %d, k34 %d\n", k38, k312, k313, k212, k213, k30, k34);
                                    printf("P0\n");
                                    for(int i = 0; i < 16; i++) {
                                        printf("%x, ", P0[i]);
                                    }printf("\n");
                                    printf("K0\n");
                                    for(int i = 0; i < 24; i++) {
                                        printf("%x, ", tmp[i]);
                                    }printf("\n");
                                    printf("P1\n");
                                    for(int i = 0; i < 16; i++) {
                                        printf("%x, ", P1[i]);
                                    }printf("\n");
                                    printf("K1\n");
                                    for(int i = 0; i < 24; i++) {
                                        printf("%x, ", tmp1[i]);
                                    }printf("\n");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);
}


void verify_collision(){
    uint8_t K1[7][32] = {{0xa1,0x63,0x6e,0xa,0x73,0x45,0xac,0xf6,0xf,0x54,0x4d,0xab,0x0,0x68,0x0,0x8b,0x88,0x61,0x73,0x80,0x28,0xb8,0x2,0x6a,0xd2,0x7d,0x1a,0xbb,0x54,0xab,0x55,0xcd},
    {0xa1,0x63,0xfa,0x9e,0x73,0x45,0x38,0x71,0xf,0x54,0x4d,0xba,0x0,0x68,0x0,0xd1,0x2a,0x61,0x73,0x14,0x96,0xe5,0x2,0x6a,0x32,0x21,0x1a,0xbb,0x94,0xab,0x55,0xcd},
    {0xa1,0x63,0xc,0x5e,0x73,0x45,0x52,0x12,0xf,0x54,0x4d,0x46,0x0,0x68,0x0,0xd5,0xd,0xf2,0x6e,0x3,0x56,0x29,0x26,0xb7,0xac,0xc8,0x1a,0xbb,0x21,0x74,0xa,0xb0},
    {0xa1,0x63,0xf4,0xe9,0x7d,0x45,0xd0,0x2c,0x1,0x54,0x4d,0x39,0x0,0x68,0x0,0xaa,0xcf,0x44,0xed,0x40,0xd4,0xce,0xb5,0xfe,0x2b,0x12,0x1a,0xbb,0xb2,0x1d,0x9e,0xc6},
    {0xa1,0x63,0x35,0x6c,0x7d,0x45,0x11,0x5e,0x1,0x54,0x4d,0xa0,0x0,0x68,0x0,0x77,0xfd,0x44,0xed,0xca,0x12,0x98,0xb5,0xfe,0xd5,0x89,0x1a,0xbb,0x15,0x1d,0x9e,0xc6},
    {0xa1,0x63,0x4a,0xb0,0x7d,0x45,0x6e,0x8e,0x1,0x54,0x4d,0x21,0x0,0x68,0x0,0x4b,0xa6,0x44,0xed,0x62,0x78,0x24,0xb5,0xfe,0xe9,0xfe,0x1a,0xbb,0x19,0x1d,0x9e,0xc6},
    {0xa1,0x63,0xa9,0x97,0x7d,0x45,0x8d,0x85,0x1,0x54,0x4d,0x25,0x0,0x68,0x0,0x73,0x90,0x44,0xed,0xc3,0x36,0xab,0xb5,0xfe,0x93,0x65,0x1a,0xbb,0x55,0x1d,0x9e,0xc6}
    };
    
    uint8_t K2[7][32] = {{0xaf,0xfb,0x6e,0xa,0x7d,0xdd,0xac,0xf6,0x1,0xfc,0xae,0xab,0xe,0xc0,0xe3,0x8b,0x88,0x61,0x73,0x80,0x28,0xb8,0x2,0x6a,0xd2,0x7d,0xe0,0xe3,0x54,0xab,0x55,0xcd},
    {0xaf,0xfb,0xfa,0x9e,0x7d,0xdd,0x38,0x71,0x1,0xfc,0xae,0xba,0xe,0xc0,0xe3,0xd1,0x2a,0x61,0x73,0x14,0x96,0xe5,0x2,0x6a,0x32,0x21,0xe0,0xe3,0x94,0xab,0x55,0xcd},
    {0xaf,0xfb,0xc,0x5e,0x7d,0xdd,0x52,0x12,0x1,0xfc,0xae,0x46,0xe,0xc0,0xe3,0xd5,0xd,0xf2,0x6e,0x3,0x56,0x29,0x26,0xb7,0xac,0xc8,0xe0,0xe3,0x21,0x74,0xa,0xb0},
    {0xaf,0xfb,0xf4,0xe9,0x73,0xdd,0xd0,0x2c,0xf,0xfc,0xae,0x39,0xe,0xc0,0xe3,0xaa,0xcf,0x44,0xed,0x40,0xd4,0xce,0xb5,0xfe,0x2b,0x12,0xe0,0xe3,0xb2,0x1d,0x9e,0xc6},
    {0xaf,0xfb,0x35,0x6c,0x73,0xdd,0x11,0x5e,0xf,0xfc,0xae,0xa0,0xe,0xc0,0xe3,0x77,0xfd,0x44,0xed,0xca,0x12,0x98,0xb5,0xfe,0xd5,0x89,0xe0,0xe3,0x15,0x1d,0x9e,0xc6},
    {0xaf,0xfb,0x4a,0xb0,0x73,0xdd,0x6e,0x8e,0xf,0xfc,0xae,0x21,0xe,0xc0,0xe3,0x4b,0xa6,0x44,0xed,0x62,0x78,0x24,0xb5,0xfe,0xe9,0xfe,0xe0,0xe3,0x19,0x1d,0x9e,0xc6},
    {0xaf,0xfb,0xa9,0x97,0x73,0xdd,0x8d,0x85,0xf,0xfc,0xae,0x25,0xe,0xc0,0xe3,0x73,0x90,0x44,0xed,0xc3,0x36,0xab,0xb5,0xfe,0x93,0x65,0xe0,0xe3,0x55,0x1d,0x9e,0xc6}
    };

 
    for(int k = 0; k < 7; k++) {
        uint8_t ttemp[32] = {0};
        uint8_t ttemp1[32] = {0};

        uint8_t tmp[32];
        for(int i = 0; i < 32; i++){
            tmp[i] = K1[k][i];
        }
        
        AES256encrypt16(ttemp, tmp, 3);
        printf("K0\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", tmp[i]);
        }printf("\n"); 

        uint8_t tmp1[32];
        for(int i = 0; i < 32; i++){
            tmp1[i] = K2[k][i];
        }
        
        AES256encrypt16(ttemp1, tmp1, 3);
        printf("K1\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", tmp1[i]);
        }printf("\n"); 

        printf("C1\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n");
        printf("C0\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n");

        for(int i = 0; i < 32; i++) {
            printf("%x, ", ttemp1[i] ^ ttemp[i]);
        }
        printf("\n\n\n"); 
    }

}


void main()
{
    printf("begin main\n");
    int t = gen_sbox_ddt();
    gen_sbox_diff_term();
    get_diff_nums();
    get_diff_values();
    get_diff_k_nums();
    get_diff_K_values();
    test_aes_256();
    //filter_search();
    //filter_search3();
    verify_collision();
    printf("end main\n");
}