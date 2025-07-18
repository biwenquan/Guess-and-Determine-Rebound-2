#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>
#include <stdbool.h>

#define Nb 4
#define BLOCKLEN 16
#define Nk 4
#define Nr 10
#define keyExpSize 176


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

static const uint8_t Rcon[11] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
static const uint8_t Diff_X[3][16] = {
//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
{0x00, 0xda, 0x03, 0x8d, 0x00, 0x00, 0x33, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00},
{0x00, 0x00, 0x03, 0x8d, 0xaf, 0x00, 0x00, 0x00, 0xaf, 0x00, 0x00, 0x00, 0x00, 0x48, 0x48, 0x00},
{0x00, 0x00, 0x00, 0x8d, 0x00, 0x00, 0x00, 0x4e, 0x00, 0x00, 0xf6, 0x00, 0x00, 0x00, 0x00, 0xbd}
};

static const uint8_t Diff_Y[3][16] = {
{0x00, 0x18, 0xda, 0x75, 0x00, 0x00, 0x28, 0x75, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xda, 0x00},
{0x00, 0x00, 0x7b, 0x6e, 0x41, 0x00, 0x00, 0x00, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xec, 0x00},
{0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x8d}
};

static const uint8_t Diff_K[4][16] = {
//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
{0x00, 0xda, 0x03, 0x8d, 0x00, 0x00, 0x33, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00},
{0x00, 0x00, 0x03, 0x8d, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x30},
{0x00, 0x00, 0x00, 0x8d, 0x00, 0x00, 0x30, 0xbd, 0x00, 0x00, 0x00, 0x8d, 0x00, 0x00, 0x00, 0xbd},
{0x00, 0x00, 0x30, 0x8d, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x00, 0x00}
};

static int value_X[3][16] = {0};
static int value_Y[3][16] = {0};
static int value_Z[3][16] = {0};
static int value_W[3][16] = {0};
static int value_K[3][16] = {0};

static int value_X_1[3][16] = {0};
static int value_Y_1[3][16] = {0};
static int value_Z_1[3][16] = {0};
static int value_W_1[3][16] = {0};
static int value_K_1[3][16] = {0};

static int value_all_X[3][16][4] = {0}; 
static int value_all_Y[3][16][4] = {0};
static int value_all_K[3][16][4] = {0};

int diff_k115_num = 0;
int diff_k215_num = 0;
int diff_num[3][16] = {0};
static uint8_t test_state[16] = {0};
static uint8_t test_key128[16] = {0};
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

    return;

}

void get_diff_nums() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 16; j++) {
            diff_num[i][j] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].num;
        }
    }
    for(int i = 0; i < 3; i++) {
        printf("aes %d  round sbox diff num is \n", i);
        for(int j = 0; j < 16; j++)  {
            printf("%d  ", diff_num[i][j]);
        }printf("\n");
    }
}


void get_diff_values(){
    memset(value_all_X, -1, 3*16*4*sizeof(int));
    memset(value_all_Y, -1, 3*16*4*sizeof(int));
    memset(value_all_K, -1, 3*16*4*sizeof(int));
    for(int i = 0; i < 3; i++) {
        for (int j = 0; j < 16; j++) {
            for(int k = 0; k < diff_num[i][j]; k++){
                value_all_X[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][0];
                value_all_Y[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][1];
            }
        }
    }
}

void get_diff_k_nums() {
    diff_k115_num = sbox_ddt_term[Diff_K[1][15]][Diff_K[1][2] ^ Diff_K[2][2]].num;
    diff_k215_num = sbox_ddt_term[Diff_K[2][15]][Diff_K[2][2] ^ Diff_K[3][2]].num;
}

void get_diff_K_values(){
    memset(value_all_K, -1, 3*16*4*sizeof(int));
    for(int k = 0; k < diff_k115_num; k++){
        value_all_K[1][15][k] = sbox_ddt_term[Diff_K[1][15]][Diff_K[1][2] ^ Diff_K[2][2]].term[k][0]; 
    }
    for(int k = 0; k < diff_k215_num; k++){
        value_all_K[2][15][k] = sbox_ddt_term[Diff_K[2][15]][Diff_K[2][2] ^ Diff_K[3][2]].term[k][0]; 
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


void State2Matrix(uint8_t state[4][4], const uint8_t input[16]) { 
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            state[j][i] = *input++;  
        }
    }
}

void Matrix2State(uint8_t output[16], uint8_t state[4][4]) { 
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            *output++ = state[j][i];
        }
    }
}


void subBytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}
void invSubBytes(uint8_t state[4][4]) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = rsbox[state[i][j]];
        }
    }
}
void ShiftRows(uint8_t state[4][4]){ 
    int i,j;
    uint8_t temp[4][4];
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[i][j]=temp[i][(j+i)%4]; 
        }
    }
}
void InShiftRows(uint8_t state[4][4]){  
    int i,j;
    uint8_t temp[4][4];
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[i][j]=temp[i][(4+j-i)%4]; 
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

void MixColumns(uint8_t state[4][4]){
    int i,j;
    uint8_t temp[4][4];
    uint8_t M[4][4]={
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) { 
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void InMixColumns (uint8_t state[4][4]){
    int i,j;
    uint8_t temp[4][4];
    uint8_t M[4][4]={
        {0x0E, 0x0B, 0x0D, 0x09},
        {0x09, 0x0E, 0x0B, 0x0D},
        {0x0D, 0x09, 0x0E, 0x0B},
        {0x0B, 0x0D, 0x09, 0x0E}}; 
    for(i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void AddRoundKey(uint8_t state[4][4],uint8_t W[4][4]){  
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[j][i]^=W[j][i];
        }
    }
}


static void KeyExpansion16(const uint8_t *Key)
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

static void KeyExpansion(const uint8_t *key,uint8_t w[][4][4]){
    int i,j,r;
    KeyExpansion16(key);
    for(r = 0; r <= Nr; r++) {
        for(i=0;i<4;i++){
            for(j=0;j<4;j++){
                w[r][i][j]=RoundKey[r*16+i+j*4];
            }
        }
    } 
}


void subByte16(uint8_t *RoundText){
    for(int i=0;i<16;i++)
        RoundText[i]=sbox[RoundText[i]];
}
void InvSubByte16(uint8_t *RoundText){
    for(int i=0;i<16;i++)
        RoundText[i]=rsbox[RoundText[i]];
}
void ShiftRow16(uint8_t *RoundText) {
    uint8_t t;
    //row1
    t=RoundText[1]; RoundText[1]=RoundText[5]; RoundText[5]=RoundText[9]; RoundText[9]=RoundText[13]; RoundText[13]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[15];RoundText[15]=RoundText[11];RoundText[11]=RoundText[7]; RoundText[7]=RoundText[3]; RoundText[3]=t;
}
void ShiftRow16_int(int *RoundText) {
    int t;
    //row1
    t=RoundText[1]; RoundText[1]=RoundText[5]; RoundText[5]=RoundText[9]; RoundText[9]=RoundText[13]; RoundText[13]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[15];RoundText[15]=RoundText[11];RoundText[11]=RoundText[7]; RoundText[7]=RoundText[3]; RoundText[3]=t;
}
void InvShiftRow16(uint8_t *RoundText){
    uint8_t t;
    //row1
    t=RoundText[13]; RoundText[13]=RoundText[9];RoundText[9]=RoundText[5]; RoundText[5]=RoundText[1]; RoundText[1]=t;
    //row2
    t=RoundText[2],RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[3]; RoundText[3]=RoundText[7];RoundText[7]=RoundText[11]; RoundText[11]=RoundText[15]; RoundText[15]=t;
}
void MixColumn16(uint8_t* RoundText){
    uint8_t temp[4];
    for(int i=0;i<4;i++){
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

void InvMixColumn16(uint8_t* RoundText) {
    uint8_t temp[4];
    for(int i=0;i<4;i++){
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
    for(int i=0;i<16;i++)
        RoundText[i]^=RoundKey[round*16+i];
}

void AES128encrypt16(uint8_t* RoundText, uint8_t *key, int R){
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

void aes_128_enc(uint8_t output[16], const uint8_t key[16], int round, const uint8_t input[16]) {
    int i,j,k;
    uint8_t state[4][4];
    uint8_t w[11][4][4];
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


void test_aes_128(){
    uint8_t in[16]  = {0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51};
    uint8_t key[16] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    const uint8_t out[16] = {0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97};
    uint8_t state[16] = {0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51};
    //aes_128_enc(state, key, Nr, in);
    AES128encrypt16(state, key, Nr);
    printf("cipher\n");
    for(int i = 0; i < 16; i++) {
        printf("0x%02x, ", state[i]);
    }printf("\n");
    
}


int guess_K0_12_13_15(int m, int n, int p, int X[3][16], int Y[3][16], int Z[3][16], int W[3][16], int K[3][16], int P[16]){
    K[0][12] = m;
    X[0][12] = m ^ P[12];
    Y[0][12] = sbox[X[0][12]];
    K[0][13] = n;
    X[0][13] = n ^ P[13];
    Y[0][13] = sbox[X[0][13]];
    K[0][15] = p;
    X[0][15] = p ^ P[15];
    Y[0][15] = sbox[X[0][15]];
    int tstate[16];
    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[0][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[0][i] = tstate[i];
    }

    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[1][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[1][i] = tstate[i];
    }
    K[1][1] = K[0][1] ^ sbox[K[0][14]];
    K[1][2] = K[0][2] ^ sbox[K[0][15]];
    K[1][3] = K[0][3] ^ sbox[K[0][12]];
    K[1][7] = K[0][7] ^ K[1][3];
    K[1][6] = K[0][6] ^ K[1][2];
    K[1][11] = K[0][15] ^ K[1][15];
    K[0][11] = K[1][7] ^ K[1][11];
    K[1][12] = rsbox[K[2][15] ^ K[1][15] ^ K[1][11] ^ K[1][7] ^ K[1][3]];
    K[1][8] = K[0][12] ^ K[1][12];
    
    W[0][2] = X[1][2] ^ K[1][2];
    W[0][3] = X[1][3] ^ K[1][3];
    W[0][8] = X[1][8] ^ K[1][8];
    X[0][11] = K[0][11] ^ P[11];
    Y[0][11] = sbox[X[0][11]];
    Z[0][15] = Y[0][11];

    Z[0][8] = mul8D(W[0][8] ^ Z[0][11] ^ Z[0][10] ^ mul3(Z[0][9]));
    W[0][9] = Z[0][11] ^ Z[0][8] ^ mul2(Z[0][9]) ^ mul3(Z[0][10]);
    W[0][10] = Z[0][8] ^ Z[0][9] ^ mul2(Z[0][10]) ^ mul3(Z[0][11]);
    W[0][11] = Z[0][9] ^ Z[0][10] ^ mul2(Z[0][11]) ^ mul3(Z[0][8]);

    W[0][12] = Z[0][14] ^ Z[0][15] ^ mul2(Z[0][12]) ^ mul3(Z[0][13]);
    W[0][13] = Z[0][15] ^ Z[0][12] ^ mul2(Z[0][13]) ^ mul3(Z[0][14]);
    W[0][14] = Z[0][12] ^ Z[0][13] ^ mul2(Z[0][14]) ^ mul3(Z[0][15]);
    W[0][15] = Z[0][13] ^ Z[0][14] ^ mul2(Z[0][15]) ^ mul3(Z[0][12]);

    Y[0][8] = Z[0][8];
    X[0][8] = rsbox[Y[0][8]];
    K[0][8] = X[0][8] ^ P[8];
    K[1][13] = X[1][13] ^ W[0][13];
    K[1][14] = X[1][14] ^ W[0][14];

    X[1][11] = K[1][11] ^ W[0][11];
    X[1][12] = K[1][12] ^ W[0][12];
    X[1][15] = K[1][15] ^ W[0][15];
    Y[1][11] = sbox[X[1][11]];
    Y[1][12] = sbox[X[1][12]];
    Y[1][15] = sbox[X[1][15]];
    Z[1][12] = Y[1][12];
    Z[1][15] = Y[1][11];
    Z[1][3] = Y[1][15];

    
    K[1][9] = K[0][13] ^ K[1][13];
    K[1][10] = K[0][14] ^ K[1][14];
    K[0][10] = K[1][6] ^ K[1][10];
    K[1][4] = K[0][8] ^ K[1][8];
    W[0][4] = X[1][4] ^ K[1][4];
    
    X[1][9] = K[1][9] ^ W[0][9];
    X[1][10] = K[1][10] ^ W[0][10];
    X[0][10] = K[0][10] ^ P[10];
    Y[0][10] = sbox[X[0][10]];
    Z[0][2] = Y[0][10];

    Z[0][0] = mul8D(W[0][2] ^ W[0][3] ^ Z[0][3] ^ mul3(Z[0][2]));
    Z[0][1] = W[0][2] ^ Z[0][0] ^ mul2(Z[0][2]) ^ mul3(Z[0][3]);
    W[0][0] = Z[0][2] ^ Z[0][3] ^ mul2(Z[0][0]) ^ mul3(Z[0][1]);
    W[0][1] = Z[0][3] ^ Z[0][0] ^ mul2(Z[0][1]) ^ mul3(Z[0][2]);

    Y[0][0] = Z[0][0];
    X[0][0] = rsbox[Y[0][0]];
    K[0][0] = X[0][0] ^ P[0];
    Y[0][5] = Z[0][1];
    X[0][5] = rsbox[Y[0][5]];
    K[0][5] = X[0][5] ^ P[5];

    K[1][0] = K[0][0] ^ sbox[K[0][13]] ^ Rcon[1];
    K[0][4] = K[1][4] ^ K[1][0];
    K[1][5] = K[0][5] ^ K[1][1];
    K[0][9] = K[1][9] ^ K[1][5];

    X[0][4] = K[0][4] ^ P[4];
    X[0][9] = K[0][9] ^ P[9];
    Y[0][4] = sbox[X[0][4]];
    Y[0][9] = sbox[X[0][9]];
    Z[0][4] = Y[0][4];
    Z[0][5] = Y[0][9];

    int tmp = Z[0][6] ^ Z[0][7] ^ mul2(Z[0][4]) ^ mul3(Z[0][5]);
    W[0][5] = Z[0][7] ^ Z[0][4] ^ mul2(Z[0][5]) ^ mul3(Z[0][6]);
    W[0][6] = Z[0][4] ^ Z[0][5] ^ mul2(Z[0][6]) ^ mul3(Z[0][7]);
    W[0][7] = Z[0][5] ^ Z[0][6] ^ mul2(Z[0][7]) ^ mul3(Z[0][4]);

    if(tmp == W[0][4]) {
        return 1;
    } else {
        return 0;
    }   
}


void compute_to_start_point(int X[3][16], int Y[3][16], int Z[3][16], int W[3][16], int K[3][16]){
    uint8_t tstate1[16];
    for(int i = 0; i < 16; i++) {
        tstate1[i] = W[0][i] ^ K[1][i];
    }

    for(int i = 0; i < 16; i++) {
        X[1][i] = tstate1[i];
    }
    subByte16(tstate1);
    for(int i = 0; i < 16; i++) {
        Y[1][i] = tstate1[i];
    }
    ShiftRow16(tstate1);
    for(int i = 0; i < 16; i++) {
        tstate1[i] = Z[1][i];
    }
    MixColumn16(tstate1);
    for(int i = 0; i < 16; i++) {
        W[1][i] = tstate1[i];
    }

}


void collision_search_128(){
    int P[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02};
    memset(value_X, -1, 3*16*sizeof(int));
    memset(value_Y, -1, 3*16*sizeof(int));
    memset(value_Z, -1, 3*16*sizeof(int));
    memset(value_W, -1, 3*16*sizeof(int));
    memset(value_K, -1, 3*16*sizeof(int));
    memset(value_X_1, -1, 3*16*sizeof(int));
    memset(value_Y_1, -1, 3*16*sizeof(int));
    memset(value_Z_1, -1, 3*16*sizeof(int));
    memset(value_W_1, -1, 3*16*sizeof(int));
    memset(value_K_1, -1, 3*16*sizeof(int));
    uint64_t count = 0; 
    int count1 = 0;
    int count2 = 0;

    for(int j = 0; j < 16; j++){
        if ((diff_num[0][j]))
        {
            value_X[0][j] = value_all_X[0][j][1];
            value_Y[0][j] = value_all_Y[0][j][1];
            value_X_1[0][j] = Diff_X[0][j] ^ value_X[0][j];
            value_Y_1[0][j] = Diff_Y[0][j] ^ value_Y[0][j];
        }
    }

    for(int j = 0; j < 16; j++){
        if ((diff_num[1][j]))
        {
            value_X[1][j] = value_all_X[1][j][0];
            value_Y[1][j] = value_all_Y[1][j][0];
            value_X_1[1][j] = Diff_X[1][j] ^ value_X[1][j];
            value_Y_1[1][j] = Diff_Y[1][j] ^ value_Y[1][j];
        }
    }
        
    for(int j = 0; j < 16; j++){
        if ((diff_num[0][j])){
            value_K[0][j] = value_X[0][j] ^ P[j];
            value_K_1[0][j] = value_X_1[0][j] ^ P[j];
        }
    }
    value_K[1][15] = value_all_K[1][15][1];
    value_K_1[1][15] = value_K[1][15] ^ Diff_K[1][15];

    value_K[2][15] = value_all_K[2][15][1];
    value_K_1[2][15] = value_K[2][15] ^ Diff_K[2][15];


    for(int index0 = 0; index0 < 64; index0++){ 
        uint8_t mn0[16] = {0};
        int a0[6] = {1,2,3,6,7,14};
        int tt0 = index0;
        for(int s0 = 0; s0 < 6; s0++){
            mn0[a0[s0]] = tt0 % 2;
            tt0 = tt0 >> 1;
        }
        for(int s0 = 0; s0 < 16; s0++) {
            if(diff_num[0][s0]) {
                value_X[0][s0] = value_all_X[0][s0][mn0[s0]];
                value_Y[0][s0] = value_all_Y[0][s0][mn0[s0]];
                value_X_1[0][s0] = Diff_X[0][s0] ^ value_X[0][s0];
                value_Y_1[0][s0] = Diff_Y[0][s0] ^ value_Y[0][s0];
            }
        }
        for(int j = 0; j < 16; j++){
            if ((diff_num[0][j])){
                value_K[0][j] = value_X[0][j] ^ P[j];
                value_K_1[0][j] = value_X_1[0][j] ^ P[j];
            }
        }

    
        for(int index1 = 0; index1 < 64; index1++){ 
            uint8_t mn1[16] = {0};
            int a1[6] = {2,3,4,8,13,14};
            int tt1 = index1;
            for(int s1 = 0; s1 < 6; s1++){
                mn1[a1[s1]] = tt1 % 2;
                tt1 = tt1 >> 1;
            }
            for(int s1 = 0; s1 < 16; s1++) {
                if(diff_num[1][s1]) {
                    value_X[1][s1] = value_all_X[1][s1][mn1[s1]];
                    value_Y[1][s1] = value_all_Y[1][s1][mn1[s1]];
                    value_X_1[1][s1] = Diff_X[1][s1] ^ value_X[1][s1];
                    value_Y_1[1][s1] = Diff_Y[1][s1] ^ value_Y[1][s1];
                }
            }
        
            for(int index2 = 0; index2 < diff_k115_num; index2++) {
                value_K[1][15] = value_all_K[1][15][index2];
                value_K_1[1][15] = value_K[1][15] ^ Diff_K[1][15]; 
                for(int index3 = 0; index3 < diff_k215_num; index3++) {
                    value_K[2][15] = value_all_K[2][15][index3];
                    value_K_1[2][15] = value_K[2][15] ^ Diff_K[2][15];              

                    for(int m = 0; m < 256; m++){  
                    for(int n = 0; n < 256; n++){  
                        for(int p = 0; p < 256; p++){  
                            int tt1 = guess_K0_12_13_15(m, n, p, value_X, value_Y, value_Z, value_W, value_K, P);
                            int tt2 = guess_K0_12_13_15(m, n, p, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1, P);
                            count++;
                            if(tt1 && tt2) {
                                count1++;
                            } else {
                                continue;
                            }
                            //compute_to_start_point(value_X, value_Y, value_Z, value_W, value_K);
                            //compute_to_start_point(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                            uint8_t tmp[16];
                            for(int i = 0; i < 16; i++){
                                tmp[i] = value_K[i/16][i%16];
                            }
                            uint8_t ttemp[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02};
                            AES128encrypt16(ttemp, tmp, 3);
                            
                            uint8_t tmp1[16];
                            for(int i = 0; i < 16; i++){
                                tmp1[i] = value_K_1[i/16][i%16];
                            }
                            uint8_t ttemp1[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02};
                            AES128encrypt16(ttemp1, tmp1, 3);

                            bool test_collision = true;
                            for(int i = 0; i < 16; i++) {
                                if(ttemp1[i] ^ ttemp[i]) {
                                    test_collision = false;
                                }
                            }

                            if(test_collision) {
                                count2++;
                                printf("collison find:%d\n",count);
                                printf("index0 %d, index1 %d, index2 %d, index3 %d,k12 %d, k13 %d, k15 %d\n", index0, index1, index2, index3, m, n, p);
                                for(int i = 0; i < 16; i++) {
                                    printf("%x, ", tmp[i]);
                                }printf("\n");
                                for(int i = 0; i < 16; i++) {
                                    printf("%x, ", tmp1[i]);
                                }printf("\n");
                                if(count2 > 100) {
                                    exit(1);
                                }
                            }
                        }
                    }
                            
                }
                }

            }
        }
    }
    printf("count1 for filter: %x\n", count1);
    printf("count2 for collision: %d\n", count2);
    printf("total countt: %llx\n",count);

}

void verify_collision(){
    uint8_t K0[1][16] = {{0x64,0x1a,0xc4,0x62,0x46,0x4c,0xb9,0xc8,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xd5,0x1e}};
    uint8_t K1[1][16] = {{0x64,0xc0,0xc7,0xef,0x46,0x4c,0x8a,0x75,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xe5,0x1e}};

    for(int k = 0; k < 1; k++) {
        uint8_t ttemp[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02};
        uint8_t ttemp1[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02}; 
        uint8_t tmp[16];
        for(int i = 0; i < 16; i++){
            tmp[i] = K0[k][i];
        }
        AES128encrypt16(ttemp, tmp, 3);
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", tmp[i]);
        }printf("\n"); 

        uint8_t tmp1[16];
        for(int i = 0; i < 16; i++){
            tmp1[i] = K1[k][i];
        }
        AES128encrypt16(ttemp1, tmp1, 3);
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", tmp1[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%x, ", tmp1[i] ^ tmp[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%x, ", ttemp1[i] ^ ttemp[i]);
        }printf("\n\n\n"); 
    }

}

void galois_multiply(uint8_t X[16], uint8_t Y[16], uint8_t out[16])
{
    uint8_t V[16];
    uint8_t Z[16];

    for(int i=0;i<16;i++) 
    {
        Z[i]=0x00;
        V[i]=Y[i];
    }

    for (int i = 0; i < 16; i++) 
    {
		for (int j = 0; j < 8; j++) 
		{
			if (X[i] & 1<<(7 - j)) 
			{
				for(int k=0;k<16;k++)
					Z[k]=Z[k]^V[k];
			} 

			if (V[15] & 0x01) 
			{
				for(int k=0;k<16;k++)
				{ 
					if (k!=0)
						if (V[15-k] & 0x01)
							V[16-k] = V[16-k] | 0x80;
					V[15-k]=V[15-k]>>1;
				 }
			  	 V[0]=V[0]^0xe1;
			} 
			else 
			{
				for(int k=0;k<16;k++)
				{ 
					if (k!=0)
						if (V[15-k] & 0x01)
							V[16-k] = V[16-k] | 0x80;
					V[15-k]=V[15-k]>>1;
			       }	
			}
		}
	}
    for(int i=0;i<16;i++)
	{
	  out[i] = Z[i]; 
	}
}

void g_ctr(uint8_t* X, const uint8_t k[16], uint8_t ICB[16], long len_X, int round, uint8_t* out)
{
	uint8_t CB[16];
	uint8_t encrypt_CB[16];
	int inc = 0;
	unsigned int i32count = 1;

	if(len_X > 0)
	{
		for(int i=0;i<16;i++)
			CB[i] = ICB[i];
		for(int i=0;i<len_X;i++)
		{
			i32count++;
			if(i32count==0)
				i32count=1;
            CB[12]=i32count>>24;
            CB[13]=i32count>>16;
            CB[14]=i32count>>8;
            CB[15]=i32count;

            aes_128_enc(encrypt_CB, k, round, CB);

			for(int j =0;j<16;j++)
			{
				out[(i*16)+j] = encrypt_CB[j] ^ X[(i*16)+j];
			}
		}
	}
}

void g_hash(uint8_t H[16], uint8_t* X, unsigned int m, uint8_t out[16])
{
	uint8_t Y[16];

	for( int i=0;i<16;i++)
	{
		Y[i] = 0x00;
	}
	for(int i=0; i < m; i++)
	{
        uint8_t temp[16];
        for(int j =0;j<16;j++)
        {
            temp[j] = Y[j] ^ X[(i*16)+j];
        }
        galois_multiply(temp, H, Y);
    }
    for( int i=0;i<16;i++)
    {
		out[i] = Y[i];
    }
}

void aes128gcm(uint8_t *ciphertext, uint8_t tag[16], uint8_t k[16], const uint8_t IV[12], int round, const uint8_t *plaintext, const unsigned long len_p, const uint8_t* add_data, const unsigned long len_ad) 
{
 
	uint8_t J_0[16]; 
	uint8_t X[16*(len_p+len_ad+1)];
    for(int i = 0; i < 16*(len_p+len_ad+1); i++){
        X[i] = 0;
    }

	for(int i = 0;i<16;i++)
	{
		J_0[i] = 0x00;
		if(i<12)
			J_0[i] = IV[i];
	}
	J_0[15]=0x01;
	uint8_t H[16];
	uint8_t zero[16];
	for(int i = 0;i<16;i++)
	{
		zero[i]=0x00;
	}
	uint8_t P[16*len_p];
	for(int i=0; i<16*len_p; i++)
		P[i]= plaintext[i];

    aes_128_enc(H, k, round, zero);
    g_ctr(P,k,J_0,len_p,round, ciphertext);

	for(int i=0; i<16*len_ad; i++)
		X[i]= add_data[i];
	for(int i=16*len_ad; i<16*(len_ad+len_p); i++)
		X[i]= ciphertext[i-(16*len_ad)];
	for(int i=16*(len_ad+len_p);i< (16*(len_ad+len_p))+7;i++)
	{
		X[i] = ((uint64_t)(16*8*len_ad) >> (8*(7-(i-16*(len_ad+len_p))))) & 0xff ;
	}
	X[(16*(len_ad+len_p))+7] = (16*8*len_ad)  & 0xff;
	for(int i=(16*(len_ad+len_p))+8;i< (16*(len_ad+len_p))+15;i++)
	{
		X[i] = ((uint64_t)(16*8*len_p) >> (8*(7-(i-(16*(len_ad+len_p)))+8))) & 0xff;
	}
	X[(16*(len_ad+len_p))+15] = (16*8*len_p)  & 0xff;
	unsigned long len_x =(len_ad+len_p+1);
	uint8_t Y[16*3];

	g_hash(H, X,len_x, Y);
	uint8_t temp[16*3];
	uint8_t en_counter[16];
    aes_128_enc(en_counter, k, round, J_0);

	for(int i=0;i<16;i++)
	{
		tag[i] = en_counter[i]^Y[i];
	}

}

typedef struct {
    uint64_t hi;
    uint64_t lo;
} uint128_t;

void print_uint128(uint128_t value) {
    printf("0x%016llX%016llX", value.hi, value.lo);
}

int uint128_eq(uint128_t a, uint128_t b) {
    return (a.hi == b.hi) && (a.lo == b.lo);
}

uint128_t make_uint128(uint64_t hi, uint64_t lo) {
    uint128_t v = {hi, lo};
    return v;
}

uint128_t uint128_shl(uint128_t a) {
    uint128_t r;
    r.hi = (a.hi << 1) | (a.lo >> 63);
    r.lo = a.lo << 1;
    return r;
}

uint128_t uint128_shr(uint128_t a) {
    uint128_t r;
    r.lo = (a.lo >> 1) | (a.hi << 63);
    r.hi = a.hi >> 1;
    return r;
}

int uint128_get_bit127(uint128_t a) {
    return (a.hi >> 63) & 1;
}

uint128_t uint128_xor(uint128_t a, uint128_t b) {
    uint128_t r;
    r.hi = a.hi ^ b.hi;
    r.lo = a.lo ^ b.lo;
    return r;
}

uint128_t uint128_and(uint128_t a, uint128_t b) {
    uint128_t r;
    r.hi = a.hi & b.hi;
    r.lo = a.lo & b.lo;
    return r;
}

uint128_t gf128_mul(uint128_t a, uint128_t b) {
    uint128_t res = {0, 0};
    uint128_t tmp = a;
    const uint128_t MOD_POLY = {0, 0x87}; 

    for (int i = 0; i < 128; i++) {
        if (b.lo & 1) {
            res = uint128_xor(res, tmp);
        }
        
        int carry = uint128_get_bit127(tmp);
        tmp = uint128_shl(tmp);
        if (carry) {
            tmp = uint128_xor(tmp, MOD_POLY);
        }
        b = uint128_shr(b);
    }
    return res;
}

uint128_t gf128_inv(uint128_t a) {
    const uint128_t ZERO = {0, 0};
    const uint128_t ONE = {0, 1};
    
    if (uint128_eq(a, ZERO)) {
        return ZERO; 
    }
    
    const uint128_t exponent = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE};
    uint128_t base = a;
    uint128_t res = ONE;
    for (int i = 0; i < 128; i++) {
        uint128_t bit_mask;
        if (i < 64) {
            bit_mask = make_uint128(0, 1ULL << i);
        } else {
            bit_mask = make_uint128(1ULL << (i - 64), 0);
        }
        
        if (!uint128_eq(uint128_and(exponent, bit_mask), ZERO)) {
            res = gf128_mul(res, base);
        }
        base = gf128_mul(base, base);
    }
    return res;
}


int test_galois_inverse() {
    printf("AES-GCM Galois Field Inverse Calculator\n");
    printf("Irreducible polynomial: x^128 + x^7 + x^2 + x + 1\n\n");
    
    const uint128_t ZERO = {0, 0};
    const uint128_t ONE = {0, 1};
    
    uint128_t a1 = ONE;
    uint128_t inv1 = gf128_inv(a1);
    uint128_t product1 = gf128_mul(a1, inv1);
    
    printf("Test 1: Inverse of 1\n");
    printf("Input:  "); print_uint128(a1); printf("\n");
    printf("Inverse: "); print_uint128(inv1); printf("\n");
    printf("Product: "); print_uint128(product1); 
    printf(" (should be 1) - %s\n\n", 
           uint128_eq(product1, ONE) ? "SUCCESS" : "FAILURE");
    
    uint128_t a2 = {0x123456789ABCDEF0, 0x0FEDCBA987654321};
    uint128_t inv2 = gf128_inv(a2);
    uint128_t product2 = gf128_mul(a2, inv2);
    
    printf("Test 2: Random value\n");
    printf("Input:  "); print_uint128(a2); printf("\n");
    printf("Inverse: "); print_uint128(inv2); printf("\n");
    printf("Product: "); print_uint128(product2); 
    printf(" (should be 1) - %s\n\n", 
           uint128_eq(product2, ONE) ? "SUCCESS" : "FAILURE");
    
    uint128_t a3 = {0x55AA55AA55AA55AA, 0xAA55AA55AA55AA55};
    uint128_t inv3 = gf128_inv(a3);
    uint128_t product3 = gf128_mul(a3, inv3);
    
    printf("Test 3: Another random value\n");
    printf("Input:  "); print_uint128(a3); printf("\n");
    printf("Inverse: "); print_uint128(inv3); printf("\n");
    printf("Product: "); print_uint128(product3); 
    printf(" (should be 1) - %s\n\n", 
           uint128_eq(product3, ONE) ? "SUCCESS" : "FAILURE");
    
    uint128_t a4 = ZERO;
    uint128_t inv4 = gf128_inv(a4);
    
    printf("Test 4: Zero value\n");
    printf("Input:  "); print_uint128(a4); printf("\n");
    printf("Inverse: "); print_uint128(inv4); 
    printf(" (should be 0) - %s\n\n", 
           uint128_eq(inv4, ZERO) ? "SUCCESS" : "FAILURE");
    
    uint128_t a5 = {0, 1ULL << 63};
    uint128_t inv5 = gf128_inv(a5);
    uint128_t product5 = gf128_mul(a5, inv5);
    
    printf("Test 5: x^64\n");
    printf("Input:  "); print_uint128(a5); printf("\n");
    printf("Inverse: "); print_uint128(inv5); printf("\n");
    printf("Product: "); print_uint128(product5); 
    printf(" (should be 1) - %s\n", 
           uint128_eq(product5, ONE) ? "SUCCESS" : "FAILURE");

    uint128_t a6 = {0x123456789abcdef0, 0x123456789abcdef0};
    uint128_t inv6 = gf128_inv(a6);
    uint128_t product6 = gf128_mul(a6, inv6);
    
    printf("Test 6: x^64\n");
    printf("Input:  "); print_uint128(a6); printf("\n");
    printf("Inverse: "); print_uint128(inv6); printf("\n");
    printf("Product: "); print_uint128(product6); 
    printf(" (should be 1) - %s\n", 
           uint128_eq(product6, ONE) ? "SUCCESS" : "FAILURE");
    
    return 0;
}

uint128_t String2Struct(uint8_t input[16]) { 
    uint128_t output = {0,0}; 
    for (int i = 0; i < 8; i++) {
        output.hi |= (uint64_t)input[i]<<((7-i)*8);    
    }
    for (int i = 0; i < 8; i++) {
        output.lo |= (uint64_t)input[8+i]<<((7-i)*8);    
    }
    return output;
}

void Struct2String(uint8_t output[16], uint128_t input) { 
    for (int i = 0; i < 8; i++) {
        output[i] = (input.hi >> ((7-i)*8)) & 0xff;    
    }
    for (int i = 0; i < 8; i++) {
        output[i+8] = (input.lo >> ((7-i)*8)) & 0xff;    
    }
}

uint8_t reverse_byte(uint8_t b) {
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

uint128_t reverse_bits(uint128_t a) {
    uint128_t r = {0, 0};
    uint8_t bytes[16];
    
    for (int i = 0; i < 8; i++) {
        bytes[i] = (a.lo >> (i * 8)) & 0xFF;
        bytes[i + 8] = (a.hi >> (i * 8)) & 0xFF;
    }

    for (int i = 0; i < 16; i++) {
        bytes[i] = reverse_byte(bytes[i]);
    }
    uint8_t reversed_bytes[16];
    for (int i = 0; i < 16; i++) {
        reversed_bytes[i] = bytes[15 - i];
    }
    for (int i = 0; i < 8; i++) {
        r.lo |= (uint64_t)reversed_bytes[i] << (i * 8);
        r.hi |= (uint64_t)reversed_bytes[i + 8] << (i * 8);
    }
    
    return r;
}


void key_commit_attack_on_padding_fix(){
    uint8_t K0[16] = {0x64,0x1a,0xc4,0x62,0x46,0x4c,0xb9,0xc8,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xd5,0x1e};
    uint8_t K1[16] = {0x64,0xc0,0xc7,0xef,0x46,0x4c,0x8a,0x75,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xe5,0x1e};
    uint8_t IV[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    uint8_t add_data[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    uint8_t plaintext0[48] = {0};
    uint8_t plaintext1[48] = {0};
    uint8_t ciphertext0[48] = {0};
    uint8_t ciphertext1[48] = {0};
    uint8_t tag0[16] = {0};
    uint8_t tag1[16] = {0};
    uint8_t J_0[16]; 
    for(int i = 0;i<16;i++)
	{
		J_0[i] = 0x00;
		if(i < 12) {
			J_0[i] = IV[i];
        }
	}
	J_0[15] = 0x01;
	uint8_t H0[16];
    uint8_t H1[16];
	uint8_t zero[16];
	for(int i = 0;i<16;i++)
	{
		zero[i]=0x00;
	}
    aes_128_enc(H0, K0, 3, zero);
    aes_128_enc(H1, K1, 3, zero);
    uint8_t encJ0_0[16];
    uint8_t encJ0_1[16];
    aes_128_enc(encJ0_0, K0, 3, J_0);
    aes_128_enc(encJ0_1, K1, 3, J_0);

    uint8_t J_1[16]; 
    for(int i = 0;i < 15;i++)
	{
		J_1[i] = J_0[i];
	}
    J_1[15] = J_0[15] + 1;
    aes_128_enc(ciphertext0, K0, 3, J_1);
    aes_128_enc(ciphertext1, K1, 3, J_1);

    for(int i = 0; i < 16; i++) {
        if(ciphertext0[i] != ciphertext1[i]) {
            printf("error0\n");
            return;
        }
    }
    
    uint8_t encJ1_0[32];
    uint8_t encJ1_1[32];
    J_1[15] = J_1[15] + 1;
    aes_128_enc(encJ1_0, K0, 3, J_1);
    aes_128_enc(encJ1_1, K1, 3, J_1);
    J_1[15] = J_1[15] + 1;
    aes_128_enc(encJ1_0+16, K0, 3, J_1);
    aes_128_enc(encJ1_1+16, K1, 3, J_1);

    uint8_t H0_2[16];
    uint8_t H1_2[16];
    uint8_t H0_3[16];
    uint8_t H1_3[16];
    uint8_t H0_4[16];
    uint8_t H1_4[16];
    uint8_t H0_5[16];
    uint8_t H1_5[16];

    uint8_t lenAC[16] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x80};
    galois_multiply(H0, H0, H0_2);
    galois_multiply(H1, H1, H1_2);
    galois_multiply(H0, H0_2, H0_3);
    galois_multiply(H1, H1_2, H1_3);
    galois_multiply(H0, H0_3, H0_4);
    galois_multiply(H1, H1_3, H1_4);
    galois_multiply(H0, H0_4, H0_5);
    galois_multiply(H1, H1_4, H1_5);

    uint8_t tmpA0[16];
    uint8_t tmpA1[16];
    uint8_t tmpA[16];
    galois_multiply(ciphertext0, H0_4, tmpA0);
    galois_multiply(ciphertext1, H1_4, tmpA1);
    for(int i= 0; i < 16; i++){
        tmpA[i] = tmpA0[i] ^ tmpA1[i];
    }
    galois_multiply(ciphertext0+32, H0_2, tmpA0);
    galois_multiply(ciphertext1+32, H1_2, tmpA1);
    for(int i= 0; i < 16; i++){
        tmpA[i] ^= tmpA0[i] ^ tmpA1[i];
    }
    galois_multiply(add_data, H0_5, tmpA0);
    galois_multiply(add_data, H1_5, tmpA1);
    for(int i= 0; i < 16; i++){
        tmpA[i] ^= tmpA0[i] ^ tmpA1[i];
    }
    galois_multiply(lenAC, H0, tmpA0);
    galois_multiply(lenAC, H1, tmpA1);
    for(int i= 0; i < 16; i++){
        tmpA[i] ^= tmpA0[i] ^ tmpA1[i];
    }
    for(int i= 0; i < 16; i++){
        tmpA[i] ^= encJ0_0[i] ^ encJ0_1[i];
    }

    uint8_t tmpHi[16];
    for(int i= 0; i < 16; i++){
        tmpHi[i] = H0_3[i] ^ H1_3[i];
    }

    uint128_t tmpHi128 = {0,0};
    tmpHi128 = String2Struct(tmpHi);
    tmpHi128 = reverse_bits(tmpHi128);

    uint128_t tmpHiV128 = {0,0};  
    tmpHiV128 = gf128_inv(tmpHi128);
    uint128_t product5 = gf128_mul(tmpHi128, tmpHiV128);
    tmpHiV128 = reverse_bits(tmpHiV128);
    uint8_t tmpHiv[16] = {0};
    Struct2String(tmpHiv, tmpHiV128);

    uint8_t tmpproduct[16];
    galois_multiply(tmpHi, tmpHiv, tmpproduct);
    uint8_t tmpC[16];
    galois_multiply(tmpHiv, tmpA, tmpC);

    for(int i = 0; i < 16; i++){
        ciphertext0[i+16] = tmpC[i];
        ciphertext1[i+16] = tmpC[i];
    }
    for(int i = 0; i < 16; i++){
        plaintext0[i+16] = ciphertext0[i+16] ^ encJ1_0[i];
        plaintext1[i+16] = ciphertext1[i+16] ^ encJ1_1[i];
    }

    for(int i = 16; i < 32; i++){
        plaintext0[i+16] = encJ1_0[i];
        plaintext1[i+16] = encJ1_1[i];
    }
    printf("plaintext0\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",plaintext0[i]);
    }printf("\n");
    printf("plaintext1\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",plaintext1[i]);
    }printf("\n");

    printf("ciphertext0\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",ciphertext0[i]);
    }printf("\n");
    printf("ciphertext1\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",ciphertext1[i]);
    }printf("\n");

    uint8_t X0[16*5];
    uint8_t X1[16*5];
    for(int i = 0; i < 16; i++){
        X0[i] = add_data[i];
        X1[i] = add_data[i];
    }
    for(int i = 0; i < 48; i++){
        X0[i+16] = ciphertext0[i];
        X1[i+16] = ciphertext1[i];
    }
    for(int i = 0; i < 16; i++){
        X0[i+64] = lenAC[i];
        X1[i+64] = lenAC[i];
    }

    uint8_t tmptag0[16] = {0};
    uint8_t tmptag1[16] = {0};
    g_hash(H0, X0, 5, tmptag0);
    g_hash(H1, X1, 5, tmptag1);
    for(int i = 0; i < 16; i++){
        tag0[i] = tmptag0[i] ^ encJ0_0[i];
        tag1[i] = tmptag1[i] ^ encJ0_1[i];
    }
    printf("tag0\n");
    for(int i=0;i<16;i++)
	{
		printf("%02x ",tag0[i]);
	}printf("\n");
    printf("tag1\n");
    for(int i=0;i<16;i++)
	{
		printf("%02x ",tag1[i]);
	}printf("\n");

}

void key_commit_attack_verify(){
    uint8_t K0[16] = {0x64,0x1a,0xc4,0x62,0x46,0x4c,0xb9,0xc8,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xd5,0x1e};
    uint8_t K1[16] = {0x64,0xc0,0xc7,0xef,0x46,0x4c,0x8a,0x75,0x1f,0x4a,0x43,0xb,0x3a,0xb5,0xe5,0x1e};
    uint8_t IV[12] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    uint8_t add_data[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
    uint8_t plaintext0[48] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0xfe,0x47,0xf5,0x1d,0xa,0x8f,0xc,0xed,0xac,0xd4,0x39,0xad,0x62,0x3f,0xe8,0x33,0xaa,0x11,0xe0,0xa2,0x6b,0x98,0x4d,0x5d,0x58,0x8,0xb6,0x31,0x1,0xd,0x8f};
    uint8_t plaintext1[48] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x3f,0xa9,0x3c,0x15,0x8b,0x8f,0xbf,0x39,0xac,0xd4,0xb0,0xad,0x62,0xdf,0x6a,0x33,0x37,0xcd,0x72,0x33,0x7b,0x98,0x26,0x42,0x58,0x8,0x66,0x31,0x1,0x85,0x51};
    uint8_t ciphertext0[48] = {0};
    uint8_t ciphertext1[48] = {0};
    uint8_t tag0[16] = {0};
    uint8_t tag1[16] = {0};
    aes128gcm(ciphertext0, tag0, K0, IV, 3, plaintext0, 3, add_data, 1);
    aes128gcm(ciphertext1, tag1, K1, IV, 3, plaintext1, 3, add_data, 1);
    printf("IV\n");
    for(int i = 0; i < 12; i++){
        printf("%02x ",IV[i]);
    }printf("\n");

    printf("AD\n");
    for(int i = 0; i < 16; i++){
        printf("%02x ",add_data[i]);
    }printf("\n");

    printf("plaintext0\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",plaintext0[i]);
    }printf("\n");
    printf("plaintext1\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",plaintext1[i]);
    }printf("\n");

    printf("K0\n");
    for(int i = 0; i < 16; i++){
        printf("%02x ",K0[i]);
    }printf("\n");
    printf("K1\n");
    for(int i = 0; i < 16; i++){
        printf("%02x ",K1[i]);
    }printf("\n");

    printf("ciphertext0\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",ciphertext0[i]);
    }printf("\n");
    printf("ciphertext1\n");
    for(int i = 0; i < 48; i++){
        printf("%02x ",ciphertext1[i]);
    }printf("\n");

    printf("tag0\n");
    for(int i=0;i<16;i++)
	{
		printf("%02x ",tag0[i]);
	}printf("\n");
    printf("tag1\n");
    for(int i=0;i<16;i++)
	{
		printf("%02x ",tag1[i]);
	}printf("\n");

    for(int i = 0; i < 48; i++){
        printf("%x ", ciphertext0[i] ^ ciphertext1[i]);
    }printf("\n");

    for(int i=0;i<16;i++)
	{
		printf("%x ", tag0[i] ^ tag1[i]);
	}printf("\n");

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
    //collision_search_128();
    //verify_collision();
    key_commit_attack_on_padding_fix();
    key_commit_attack_verify();
    printf("end main\n");
}
