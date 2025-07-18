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
{0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x56,0x00,0x00,0x6c,0xea,0xd0,0xd3,0x1a,0x2e,0x34,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xab,0x3b,0x90,0x90},
{0xca,0xda,0x46,0x46,0x01,0x71,0x00,0x00,0x1d,0x83,0x83,0x9e,0x03,0x03,0x05,0x06,0x03,0x05,0x22,0x03,0x05,0x05,0x2b,0x0a,0x83,0xfa,0x4c,0x64,0xb0,0x02,0x94,0x3f},
{0x00,0x4f,0x02,0x05,0x00,0x00,0x00,0x24,0x00,0x00,0xc9,0x00,0x00,0x00,0x00,0x27,0x47,0x00,0x00,0x00,0x07,0x4c,0x00,0x00,0x00,0x38,0x00,0x00,0x75,0x00,0x3a,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x16,0x00}
};
static const uint8_t Diff_Y[5][32] = {
 //0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31 
{0x00,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xd2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x47,0x00,0x00,0x38,0x46,0x01,0x05,0x83,0x91,0x7b,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0xd9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x7b,0x03,0x03},
{0xa8,0x75,0x39,0xd8,0x1f,0xd8,0x00,0x00,0xd2,0x8f,0x68,0x1c,0x04,0x52,0x3a,0xeb,0x08,0x98,0x57,0x02,0xa8,0x1e,0x92,0xe3,0x1c,0xda,0x4a,0xdb,0xb9,0x1c,0xad,0xf1},
{0x00,0x8a,0xe3,0x1f,0x00,0x00,0x00,0x1f,0x00,0x00,0xc4,0x00,0x00,0x00,0x00,0xa6,0x8f,0x00,0x00,0x00,0x8f,0x57,0x00,0x00,0x00,0x57,0x00,0x00,0xfe,0x00,0xe3,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00}
};
static const uint8_t Diff_K[6][32] = {
 //0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29   30   31 
{0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x56,0x00,0x00,0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x56,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
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
int diff_k128_num = 0;
int diff_k230_num = 0;
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
    diff_k230_num = sbox_ddt_term[Diff_K[2][30]][Diff_K[2][1] ^ Diff_K[3][1]].num;
    printf("aes key3 sbox diff num is ");
    printf("%d  \n", diff_k230_num);
    printf("aes key3 sbox diff num is %d\n",sbox_ddt_term[0x24][0x56].num);

}

void get_diff_K_values(){
    memset(value_all_K, -1, 5*32*4*sizeof(int)); 
    for(int k = 0; k < diff_k230_num; k++){
        value_all_K[2][30][k] = sbox_ddt_term[Diff_K[2][30]][Diff_K[2][1] ^ Diff_K[3][1]].term[k][0];
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
    uint8_t tstate1[32];
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
    
    for(int i = 0; i < 32; i++) {
        tstate1[i] = Z[2][i];
    }
    MixColumn16(tstate1);
    for(int i = 0; i < 20; i++) {
        W[2][i] = tstate1[i];
    }
    for(int i = 28; i < 32; i++) {
        W[2][i] = tstate1[i];
    }

    for(int i = 0; i < 32; i++) {
        tstate[i] = Y[3][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 32; i++) {
        Z[3][i] = tstate[i];
    }
    for(int i = 0; i < 32; i++) {
        tstate1[i] = Z[3][i];
    }
    MixColumn16_1column(tstate1, 4);
    MixColumn16_1column(tstate1, 5);
    MixColumn16_1column(tstate1, 7);
    for(int i = 16; i < 24; i++) {
        W[3][i] = tstate1[i];
    }
    for(int i = 28; i < 32; i++) {
        W[3][i] = tstate1[i];
    }
    K[3][1] = X[3][1] ^ W[2][1];
    K[3][2] = X[3][2] ^ W[2][2];
    K[3][3] = X[3][3] ^ W[2][3];
    K[3][7] = X[3][7] ^ W[2][7];
    K[3][10] = X[3][10] ^ W[2][10];
    K[3][15] = X[3][15] ^ W[2][15];
    K[3][16] = X[3][16] ^ W[2][16];
    K[3][28] = X[3][28] ^ W[2][28];
    K[3][30] = X[3][30] ^ W[2][30];
    W[1][30] = X[2][30] ^ K[2][30];
 
    Z[1][31] = mulF6(W[1][30] ^ Z[1][28] ^ Z[1][29] ^ mul2(Z[1][30]));
    W[1][28] = Z[1][30] ^ Z[1][31] ^ mul2(Z[1][28]) ^ mul3(Z[1][29]);
    W[1][29] = Z[1][31] ^ Z[1][28] ^ mul2(Z[1][29]) ^ mul3(Z[1][30]);
    W[1][31] = Z[1][29] ^ Z[1][30] ^ mul2(Z[1][31]) ^ mul3(Z[1][28]);

    K[2][28] = X[2][28] ^ W[1][28];
    K[2][29] = X[2][29] ^ W[1][29];
    K[2][31] = X[2][31] ^ W[1][31];
    Y[1][15] = Z[1][31];
    X[1][15] = rsbox[Y[1][15]];

    K[2][1] = K[3][1] ^ sbox[K[2][30]];
    K[2][2] = K[3][2] ^ sbox[K[2][31]];
    K[2][3] = K[3][3] ^ sbox[K[2][28]];
    K[2][7] = K[3][7] ^ K[3][3];
    K[3][24] = K[3][28] ^ K[2][28];
    K[3][26] = K[3][30] ^ K[2][30];

    W[1][1] = X[2][1] ^ K[2][1];
    W[1][2] = X[2][2] ^ K[2][2];
    W[1][3] = X[2][3] ^ K[2][3];
    
    W[1][0] = mul4F(Z[1][1] ^ mulE(W[1][1]) ^ mulB(W[1][2]) ^ mulD(W[1][3]));
    Z[1][0] = mulD(W[1][2]) ^ mul9(W[1][3]) ^ mulE(W[1][0]) ^ mulB(W[1][1]);
    Z[1][2] = mulD(W[1][0]) ^ mul9(W[1][1]) ^ mulE(W[1][2]) ^ mulB(W[1][3]);
    Z[1][3] = mulD(W[1][1]) ^ mul9(W[1][2]) ^ mulE(W[1][3]) ^ mulB(W[1][0]);

    K[2][0] = X[2][0] ^ W[1][0];
    Y[1][0] = Z[1][0];
    X[1][0] = rsbox[Y[1][0]];
    Y[1][14] = Z[1][2];
    X[1][14] = rsbox[Y[1][14]];
    Y[1][19] = Z[1][3];
    X[1][19] = rsbox[Y[1][19]];

}


void guess_K2_24(int i, int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    K[2][24] = i;

    K[3][20] = K[3][24] ^ K[2][24];
    K[2][20] = K[3][20] ^ K[3][16];
    K[3][0] = K[2][0] ^ sbox[K[2][29]] ^ Rcon[3];

    W[1][20] = X[2][20] ^ K[2][20];
    W[1][24] = X[2][24] ^ K[2][24];
    W[2][20] = X[3][20] ^ K[3][20];

    X[3][0] = W[2][0] ^ K[3][0];
    Y[3][0] = sbox[X[3][0]];
    Z[3][0] = Y[3][0];

    Z[1][24] = mul8D(W[1][24] ^ Z[1][27] ^ Z[1][26] ^ mul3(Z[1][25]));
    W[1][25] = Z[1][27] ^ Z[1][24] ^ mul2(Z[1][25]) ^ mul3(Z[1][26]);
    W[1][26] = Z[1][24] ^ Z[1][25] ^ mul2(Z[1][26]) ^ mul3(Z[1][27]);
    W[1][27] = Z[1][25] ^ Z[1][26] ^ mul2(Z[1][27]) ^ mul3(Z[1][24]);

    K[2][25] = X[2][25] ^ W[1][25];
    K[2][26] = X[2][26] ^ W[1][26];
    K[2][27] = X[2][27] ^ W[1][27];
    Y[1][24] = Z[1][24];
    X[1][24] = rsbox[Y[1][24]];

    Z[2][23] = W[2][20] ^ Z[2][22] ^ mul2(Z[2][20]) ^ mul3(Z[2][21]);
    W[2][21] = Z[2][20] ^ Z[2][23] ^ mul2(Z[2][21]) ^ mul3(Z[2][22]);
    W[2][22] = Z[2][20] ^ Z[2][21] ^ mul2(Z[2][22]) ^ mul3(Z[2][23]);
    W[2][23] = Z[2][21] ^ Z[2][22] ^ mul2(Z[2][23]) ^ mul3(Z[2][20]);
    K[3][21] = X[3][21] ^ W[2][21];
    Y[2][7] = Z[2][23];
    X[2][7] = rsbox[Y[2][7]];
    W[1][7] = X[2][7] ^ K[2][7];

    K[3][25] = K[3][21] ^ K[2][25];
    K[3][29] = K[3][25] ^ K[2][29];
    K[3][22] = K[3][26] ^ K[2][26];
    W[2][25] = X[3][25] ^ K[3][25];
    X[3][29] = W[2][29] ^ K[3][29];
    Y[3][29] = sbox[X[3][29]];
    Z[3][25] = Y[3][29];
    X[3][22] = W[2][22] ^ K[3][22];
    Y[3][22] = sbox[X[3][22]];
    Z[3][10] = Y[3][22];

    Z[1][7] = mul8D(W[1][7] ^ Z[1][5] ^ Z[1][6] ^ mul3(Z[1][4]));
    W[1][4] = Z[1][6] ^ Z[1][7] ^ mul2(Z[1][4]) ^ mul3(Z[1][5]);
    W[1][5] = Z[1][7] ^ Z[1][4] ^ mul2(Z[1][5]) ^ mul3(Z[1][6]);
    W[1][6] = Z[1][4] ^ Z[1][5] ^ mul2(Z[1][6]) ^ mul3(Z[1][7]);
    Y[1][23] = Z[1][7];
    X[1][23] = rsbox[Y[1][23]];

    Z[2][26] = mulF6(Z[2][24] ^ W[2][25] ^ mul2(Z[2][25]) ^ Z[2][27]);
    W[2][24] = Z[2][26] ^ Z[2][27] ^ mul2(Z[2][24]) ^ mul3(Z[2][25]);
    W[2][26] = Z[2][24] ^ Z[2][25] ^ mul2(Z[2][26]) ^ mul3(Z[2][27]);
    W[2][27] = Z[2][25] ^ Z[2][26] ^ mul2(Z[2][27]) ^ mul3(Z[2][24]);

    Y[2][6] = Z[2][26];
    X[2][6] = rsbox[Y[2][6]];
    K[2][6] = X[2][6] ^ W[1][6];
    K[2][5] = X[2][5] ^ W[1][5];
    K[2][4] = X[2][4] ^ W[1][4];

    X[3][24] = W[2][24] ^ K[3][24];
    Y[3][24] = sbox[X[3][24]];
    Z[3][24] = Y[3][24];
    X[3][26] = W[2][26] ^ K[3][26];
    Y[3][26] = sbox[X[3][26]];
    Z[3][14] = Y[3][26];

    K[3][4] = K[3][0] ^ K[2][4];
    K[3][5] = K[3][1] ^ K[2][5];
    K[3][6] = K[3][2] ^ K[2][6];
    K[2][10] = K[3][10] ^ K[3][6];
    W[1][10] = X[2][10] ^ K[2][10];
    X[3][4] = W[2][4] ^ K[3][4];
    X[3][5] = W[2][5] ^ K[3][5];
    X[3][6] = W[2][6] ^ K[3][6];
    Y[3][4] = sbox[X[3][4]];
    Y[3][5] = sbox[X[3][5]];
    Y[3][6] = sbox[X[3][6]];
    Z[3][4] = Y[3][4];
    Z[3][1] = Y[3][5];
    Z[3][26] = Y[3][6];

}

void guess_K2_8_9_16_17_18(int i, int j, int p, int q, int t, int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    K[2][8] = i;
    K[2][9] = j;
    K[2][16] = p;
    K[2][17] = q;
    K[2][18] = t;

    W[1][8] = X[2][8] ^ K[2][8];
    W[1][9] = X[2][9] ^ K[2][9];
    W[1][16] = X[2][16] ^ K[2][16];
    W[1][17] = X[2][17] ^ K[2][17];
    W[1][18] = X[2][18] ^ K[2][18];
    
    W[1][11] = mul4F(Z[1][8] ^ mulD(W[1][10]) ^ mulE(W[1][8]) ^ mulB(W[1][9]));
    Z[1][9] = mulD(W[1][11]) ^ mul9(W[1][8]) ^ mulE(W[1][9]) ^ mulB(W[1][10]);
    Z[1][10] = mulD(W[1][8]) ^ mul9(W[1][9]) ^ mulE(W[1][10]) ^ mulB(W[1][11]);
    Z[1][11] = mulD(W[1][9]) ^ mul9(W[1][10]) ^ mulE(W[1][11]) ^ mulB(W[1][8]);

    W[1][19] = mulC0(Z[1][18] ^ mul9(W[1][17]) ^ mulE(W[1][18]) ^ mulD(W[1][16]));
    Z[1][16] = mulD(W[1][18]) ^ mul9(W[1][19]) ^ mulE(W[1][16]) ^ mulB(W[1][17]);
    Z[1][17] = mulD(W[1][19]) ^ mul9(W[1][16]) ^ mulE(W[1][17]) ^ mulB(W[1][18]);
    Z[1][19] = mulD(W[1][17]) ^ mul9(W[1][18]) ^ mulE(W[1][19]) ^ mulB(W[1][16]);

    K[2][11] = X[2][11] ^ W[1][11];
    K[2][19] = X[2][19] ^ W[1][19];
 
    Y[1][13] = Z[1][9];
    Y[1][22] = Z[1][10];
    Y[1][27] = Z[1][11];
    X[1][13] = rsbox[Y[1][13]];
    X[1][22] = rsbox[Y[1][22]];
    X[1][27] = rsbox[Y[1][27]];

    Y[1][16] = Z[1][16];
    Y[1][21] = Z[1][17];
    Y[1][3] = Z[1][19];
    X[1][16] = rsbox[Y[1][16]];
    X[1][21] = rsbox[Y[1][21]];
    X[1][3] = rsbox[Y[1][3]];
 
}


void guess_K2_14(int i, int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    K[2][14] = i;
    K[3][8] = K[2][8] ^ K[3][4];
    K[3][9] = K[2][9] ^ K[3][5];
    K[3][11] = K[2][11] ^ K[3][7];
    K[3][12] = rsbox[K[2][16] ^ K[3][16]];
    K[2][12] = K[3][12] ^ K[3][8];
    K[2][15] = K[3][15] ^ K[3][11];
    K[3][14] = K[2][14] ^ K[3][10];
    K[3][18] = K[2][18] ^ sbox[K[3][14]];
    K[3][19] = K[2][19] ^ sbox[K[3][15]];
    K[2][22] = K[3][22] ^ K[3][18];

    W[1][12] = X[2][12] ^ K[2][12];
    W[1][14] = X[2][14] ^ K[2][14];
    W[1][15] = X[2][15] ^ K[2][15];
    W[1][22] = X[2][22] ^ K[2][22];

    W[1][13] = mulE1(Z[1][15] ^ mul9(W[1][14]) ^ mulE(W[1][15]) ^ mulB(W[1][12]));
    Z[1][12] = mulD(W[1][14]) ^ mul9(W[1][15]) ^ mulE(W[1][12]) ^ mulB(W[1][13]);
    Z[1][13] = mulD(W[1][15]) ^ mul9(W[1][12]) ^ mulE(W[1][13]) ^ mulB(W[1][14]);
    Z[1][14] = mulD(W[1][12]) ^ mul9(W[1][13]) ^ mulE(W[1][14]) ^ mulB(W[1][15]);
    K[2][13] = X[2][13] ^ W[1][13];
    Y[1][12] = Z[1][12];
    Y[1][17] = Z[1][13];
    Y[1][26] = Z[1][14];
    X[1][12] = rsbox[Y[1][12]];
    X[1][26] = rsbox[Y[1][26]];
    X[1][17] = rsbox[Y[1][17]];

    K[3][13] = K[2][13] ^ K[3][9];
    K[3][17] = K[2][17] ^ sbox[K[3][13]];
    K[2][21] = K[3][21] ^ K[3][17];
    W[1][21] = X[2][21] ^ K[2][21];

    W[1][23] = mulE5(Z[1][23] ^ mul9(W[1][22]) ^ mulD(W[1][21]) ^ mulB(W[1][20]));
    Z[1][20] = mulD(W[1][22]) ^ mul9(W[1][23]) ^ mulE(W[1][20]) ^ mulB(W[1][21]);
    Z[1][21] = mulD(W[1][23]) ^ mul9(W[1][20]) ^ mulE(W[1][21]) ^ mulB(W[1][22]);
    Z[1][22] = mulD(W[1][20]) ^ mul9(W[1][21]) ^ mulE(W[1][22]) ^ mulB(W[1][23]);
    K[2][23] = X[2][23] ^ W[1][23];
    Y[1][20] = Z[1][20];
    Y[1][25] = Z[1][21];
    Y[1][2] = Z[1][22];
    X[1][2] = rsbox[Y[1][2]];
    X[1][20] = rsbox[Y[1][20]];
    X[1][25] = rsbox[Y[1][25]];
    
    K[3][23] = K[2][23] ^ K[3][19];
    K[3][27] = K[2][27] ^ K[3][23];
    K[3][31] = K[2][31] ^ K[3][27];

}


void compute_to_start_point(int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32], uint8_t P[32]){
    K[1][4] = K[2][4] ^ K[2][0];
    K[1][5] = K[2][5] ^ K[2][1];
    K[1][6] = K[2][6] ^ K[2][2];
    K[1][7] = K[2][7] ^ K[2][3];
    K[1][8] = K[2][8] ^ K[2][4];
    K[1][9] = K[2][9] ^ K[2][5];
    K[1][10] = K[2][10] ^ K[2][6];
    K[1][11] = K[2][11] ^ K[2][7];
    K[1][12] = K[2][12] ^ K[2][8];
    K[1][13] = K[2][13] ^ K[2][9];
    K[1][14] = K[2][14] ^ K[2][10];
    K[1][15] = K[2][15] ^ K[2][11];
    K[1][16] = K[2][16] ^ sbox[K[2][12]];
    K[1][17] = K[2][17] ^ sbox[K[2][13]];
    K[1][18] = K[2][18] ^ sbox[K[2][14]];
    K[1][19] = K[2][19] ^ sbox[K[2][15]];
    K[1][20] = K[2][20] ^ K[2][16];
    K[1][21] = K[2][21] ^ K[2][17];
    K[1][22] = K[2][22] ^ K[2][18];
    K[1][23] = K[2][23] ^ K[2][19];
    K[1][24] = K[2][24] ^ K[2][20];
    K[1][25] = K[2][25] ^ K[2][21];
    K[1][26] = K[2][26] ^ K[2][22];
    K[1][27] = K[2][27] ^ K[2][23];
    K[1][28] = K[2][28] ^ K[2][24];
    K[1][29] = K[2][29] ^ K[2][25];
    K[1][30] = K[2][30] ^ K[2][26];
    K[1][31] = K[2][31] ^ K[2][27];
    K[1][0] = K[2][0] ^ sbox[K[1][29]] ^ Rcon[2];
    K[1][1] = K[2][1] ^ sbox[K[1][30]];
    K[1][2] = K[2][2] ^ sbox[K[1][31]];
    K[1][3] = K[2][3] ^ sbox[K[1][28]];

    K[0][4] = K[1][4] ^ K[1][0];
    K[0][5] = K[1][5] ^ K[1][1];
    K[0][6] = K[1][6] ^ K[1][2];
    K[0][7] = K[1][7] ^ K[1][3];
    K[0][8] = K[1][8] ^ K[1][4];
    K[0][9] = K[1][9] ^ K[1][5];
    K[0][10] = K[1][10] ^ K[1][6];
    K[0][11] = K[1][11] ^ K[1][7];
    K[0][12] = K[1][12] ^ K[1][8];
    K[0][13] = K[1][13] ^ K[1][9];
    K[0][14] = K[1][14] ^ K[1][10];
    K[0][15] = K[1][15] ^ K[1][11];
    K[0][16] = K[1][16] ^ sbox[K[1][12]];
    K[0][17] = K[1][17] ^ sbox[K[1][13]];
    K[0][18] = K[1][18] ^ sbox[K[1][14]];
    K[0][19] = K[1][19] ^ sbox[K[1][15]];
    K[0][20] = K[1][20] ^ K[1][16];
    K[0][21] = K[1][21] ^ K[1][17];
    K[0][22] = K[1][22] ^ K[1][18];
    K[0][23] = K[1][23] ^ K[1][19];
    K[0][24] = K[1][24] ^ K[1][20];
    K[0][25] = K[1][25] ^ K[1][21];
    K[0][26] = K[1][26] ^ K[1][22];
    K[0][27] = K[1][27] ^ K[1][23];
    K[0][28] = K[1][28] ^ K[1][24];
    K[0][29] = K[1][29] ^ K[1][25];
    K[0][30] = K[1][30] ^ K[1][26];
    K[0][31] = K[1][31] ^ K[1][27];
    K[0][0] = K[1][0] ^ sbox[K[0][29]] ^ Rcon[1];
    K[0][1] = K[1][1] ^ sbox[K[0][30]];
    K[0][2] = K[1][2] ^ sbox[K[0][31]];
    K[0][3] = K[1][3] ^ sbox[K[0][28]];

    for(int i = 0; i < 32; i++) {
        W[0][i] = X[1][i] ^ K[1][i];
    }
    uint8_t tstate1[32];
    for(int i = 0; i < 32; i++) {
        tstate1[i] = W[0][i];
    }
    InvMixColumn16(tstate1);
    for(int i = 0; i < 32; i++) {
        Z[0][i] = tstate1[i];
    }
    InvShiftRow16(tstate1);
    for(int i = 0; i < 32; i++) {
        Y[0][i] = tstate1[i];
    }
    InvSubByte16(tstate1);
    for(int i = 0; i < 32; i++) {
        X[0][i] = tstate1[i];
    }
    for(int i = 0; i < 32; i++) {
        P[i] = tstate1[i] ^ K[0][i];
    }


    for(int i = 0; i < 32; i++) {
        tstate1[i] = W[2][i] ^ K[3][i];
    }
    for(int i = 0; i < 32; i++) {
        X[3][i] = tstate1[i];
    }
    subByte16(tstate1);
    for(int i = 0; i < 32; i++) {
        Y[3][i] = tstate1[i];
    }
    ShiftRow16(tstate1);
    for(int i = 0; i < 32; i++) {
        Z[3][i] = tstate1[i];
    }
    MixColumn16(tstate1);
    for(int i = 0; i < 32; i++) {
        W[3][i] = tstate1[i];
    }
}


int deduce_filter_value(int X[5][32], int Y[5][32], int Z[5][32], int W[5][32], int K[5][32]){
    int tstate[32];
    for(int i = 0; i < 32; i++) {
        tstate[i] = Y[1][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 32; i++) {
        Z[1][i] = tstate[i];
    }

    K[2][22] = K[2][14] ^ sbox[K[0][22]];
    K[2][20] = K[2][12] ^ sbox[K[0][20]];
    W[1][22] = X[2][22] ^ K[2][22];
    
    Z[1][20] = W[1][22] ^ Z[1][21] ^ mul2(Z[1][22]) ^ mul3(Z[1][23]);
    W[1][20] = Z[1][22] ^ Z[1][23] ^ mul2(Z[1][20]) ^ mul3(Z[1][21]);
    W[1][21] = Z[1][23] ^ Z[1][20] ^ mul2(Z[1][21]) ^ mul3(Z[1][22]);
    W[1][23] = Z[1][21] ^ Z[1][22] ^ mul2(Z[1][23]) ^ mul3(Z[1][20]);
      
    int tmp1;
    tmp1 = X[2][20] ^ K[2][20];
    if((tmp1 == W[1][20])) {
        return 1;
    } else {
        return 0;
    }

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
    for(int i = 1; i < 4; i++) {
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

    value_K[2][30] = value_all_K[2][30][0];
    value_K_1[2][30] = value_K[2][30] ^ Diff_K[2][30];


    for(int index0 = 0; index0 < 256; index0++){
        uint8_t mn0[32] = {0};
        int a0[14] = {1,4,5,6,7,8,9,10,11,18,28,29,30,31};
        int tt0 = index0 * 8;
        for(int s0 = 0; s0 < 14; s0++){
            mn0[a0[s0]] = tt0 % 2;
            tt0 = tt0 >> 1;
        }
        for(int s0 = 0; s0 < 32; s0++) {
            if(diff_num[1][s0]) {
                value_X[1][s0] = value_all_X[1][s0][mn0[s0]];
                value_Y[1][s0] = value_all_Y[1][s0][mn0[s0]];
                value_X_1[1][s0] = Diff_X[1][s0] ^ value_X[1][s0];
                value_Y_1[1][s0] = Diff_Y[1][s0] ^ value_Y[1][s0];
            }
        }

        for(int index1 = 0; index1 < 16; index1++){ 
            uint8_t mn1[32] = {0};
            int a1[30] = {0,1,2,3,4,5,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
            int tt1 = index1 << 26;
            for(int s1 = 0; s1 < 30; s1++){
                mn1[a1[s1]] = tt1 % 2;
                tt1 = tt1 >> 1;
            }
            for(int s1 = 0; s1 < 32; s1++) {
                if(diff_num[2][s1]) {
                    value_X[2][s1] = value_all_X[2][s1][mn1[s1]];
                    value_Y[2][s1] = value_all_Y[2][s1][mn1[s1]];
                    value_X_1[2][s1] = Diff_X[2][s1] ^ value_X[2][s1];
                    value_Y_1[2][s1] = Diff_Y[2][s1] ^ value_Y[2][s1];
                }
            }
            for(int index2 = 0; index2 < 8; index2++){ 
                uint8_t mn2[32] = {0};
                int a2[12] = {1,2,3,7,10,15,16,20,21,25,28,30};
                int tt2 = index2 * 8;
                for(int s2 = 0; s2 < 12; s2++){
                    mn2[a2[s2]] = tt2 % 2;
                    tt2 = tt2 >> 1;
                }
                for(int s2 = 0; s2 < 32; s2++) {
                    if(diff_num[3][s2]) {
                        value_X[3][s2] = value_all_X[3][s2][mn2[s2]];
                        value_Y[3][s2] = value_all_Y[3][s2][mn2[s2]];
                        value_X_1[3][s2] = Diff_X[3][s2] ^ value_X[3][s2];
                        value_Y_1[3][s2] = Diff_Y[3][s2] ^ value_Y[3][s2];
                    }
                }

                for(int index3 = 0; index3 < diff_k230_num; index3++) {
                    value_K[2][30] = value_all_K[2][30][index3];
                    value_K_1[2][30] = value_K[2][30] ^ Diff_K[2][30];  


                    compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
                    compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

                    for(int k224 = 0; k224 < 256; k224++) {  
                        guess_K2_24(k224, value_X, value_Y, value_Z, value_W, value_K);
                        guess_K2_24(k224, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                        for(int k28 = 0; k28 < 256; k28++) {
                            for(int k29 = 0; k29 < 1; k29++) {
                                for(int k216 = 0; k216 < 1; k216++) {
                                    for(int k217 = 0; k217 < 1; k217++) {
                                        for(int k218 = 0; k218 < 1; k218++) {
                                            guess_K2_8_9_16_17_18(k28,k29,k216,k217, k218, value_X, value_Y, value_Z, value_W, value_K);
                                            guess_K2_8_9_16_17_18(k28,k29,k216,k217, Diff_K[2][18] ^ k218, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                                            for(int k214 = 0; k214 < 1; k214++) {
                                                guess_K2_14(k214, value_X, value_Y, value_Z, value_W, value_K);
                                                guess_K2_14(k214, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

                                                count++;
                                                uint8_t ttemp[32] = {0}; 
                                                uint8_t ttemp1[32] = {0};    
                                                uint8_t P0[32] = {0}; 
                                                uint8_t P1[32] = {0};                              
                                                compute_to_start_point(value_X, value_Y, value_Z, value_W, value_K, ttemp);
                                                compute_to_start_point(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1, ttemp1);
                                                for(int i = 0; i < 32; i++){
                                                    P0[i] = ttemp[i];
                                                    P1[i] = ttemp1[i];
                                                }

                                                uint8_t tmp[32];
                                                for(int i = 0; i < 32; i++){
                                                    tmp[i] = value_K[i/32][i%32];
                                                }
                                                AES256encrypt16(ttemp, tmp, 5);

                                                uint8_t tmp1[32];
                                                for(int i = 0; i < 32; i++){
                                                    tmp1[i] = value_K_1[i/32][i%32];
                                                }
                                                AES256encrypt16(ttemp1, tmp1, 5);

                                                bool test_collision = true;
                                                for(int i = 0; i < 32; i++) {
                                                    if((ttemp1[i] ^ ttemp[i]) || (P0[i] ^ P1[i])) {
                                                        test_collision = false;
                                                    }
                                                }


                                                if(test_collision) {
                                                    count2++;
                                                    printf("collison find:%d\n",count);                                    
                                                    printf("index0 %d,index1 %d,index2 %d,index3 %d,k224 %d,k28%d,k29 %d,k216 %d, k217 %d, k218 %d, k214 %d\n", index0,index1,index2,index3,k224, k28, k29, k216, k217, k218, k214);
                                                    printf("P0\n");
                                                    for(int i = 0; i < 32; i++) {
                                                        printf("%x, ", P0[i]);
                                                    }printf("\n");
                                                    printf("K0\n");
                                                    for(int i = 0; i < 32; i++) {
                                                        printf("%x, ", tmp[i]);
                                                    }printf("\n");
                                                    printf("P1\n");
                                                    for(int i = 0; i < 32; i++) {
                                                        printf("%x, ", P1[i]);
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
                }
            }
        }
    }
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);
}


void verify_collision(){
    uint8_t P1[1][32] = {{0x52,0x3b,0xe7,0xe8,0xdc,0x57,0xc8,0xf6,0x16,0x6d,0xa7,0x65,0x93,0xb5,0xb4,0x3c,0xdb,0x31,0xf7,0xa6,0x2a,0x73,0xdc,0x3a,0x9e,0x7c,0xc6,0x66,0x1c,0xf7,0xc3,0xd},
    };

    uint8_t P2[1][32] = {{0x52,0x3b,0xe7,0xe8,0xdc,0x57,0xc8,0xf6,0x16,0x6d,0xa7,0x65,0x93,0xb5,0xb4,0x3c,0xdb,0x31,0xf7,0xa6,0x2a,0x73,0xdc,0x3a,0x9e,0x7c,0xc6,0x66,0x1c,0xf7,0xc3,0xd},
    };

    uint8_t K1[1][32] = {{0x56,0x2b,0x3f,0x8f,0x19,0xfd,0x14,0x17,0xec,0x7f,0x4f,0xd5,0xa0,0x4a,0x3e,0x66,0x0,0xf1,0x65,0x5d,0x3b,0xe0,0xb5,0xa5,0x42,0xec,0x55,0x8a,0xc4,0x1f,0xdc,0x7d},
    };
    
    uint8_t K2[1][32] = {{0x56,0x7d,0x3f,0x8f,0x19,0xfd,0x14,0x17,0xec,0x29,0x4f,0xd5,0xa0,0x4a,0x3e,0x66,0x0,0xf1,0x41,0x5d,0x3b,0xe0,0x91,0xa5,0x42,0xec,0x55,0x8a,0xc4,0x1f,0xdc,0x7d},
    };

 
    for(int k = 0; k < 1; k++) {
        uint8_t ttemp[32] = {0};
        uint8_t ttemp1[32] = {0};
        for(int i = 0; i < 32; i++){
            ttemp[i] = P1[k][i];
            ttemp1[i] = P2[k][i];
        }

        uint8_t tmp[32];
        for(int i = 0; i < 32; i++){
            tmp[i] = K1[k][i];
        }
        printf("P0\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n");
        AES256encrypt16(ttemp, tmp, 5);
        printf("K0\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", tmp[i]);
        }printf("\n"); 

        uint8_t tmp1[32];
        for(int i = 0; i < 32; i++){
            tmp1[i] = K2[k][i];
        }
        printf("P1\n");
        for(int i = 0; i < 32; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n");
        AES256encrypt16(ttemp1, tmp1, 5);
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
    //collision_search();
    //filter_search();
    verify_collision();
    printf("end main\n");
}