#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <bitset>
#include <stdint.h>

using namespace std;

// Hold permutation and substitution lookup tables
static uint32_t enc_keys[5];
static uint8_t enc_pbox[32] = {28, 29, 30, 31, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
static uint8_t enc_sbox[16] = {0xa, 0xf, 3, 9, 0xb, 8, 2, 4, 0xe, 0, 0xc, 1, 5, 6, 0xd, 7};

static uint32_t dec_keys[5];
static uint8_t dec_pbox[32] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 0, 1, 2, 3};
static uint8_t dec_sbox[16] = {9, 0xb, 6, 0x2, 7, 0xc, 0xd, 0xf, 5, 3, 0, 4, 0xa, 0xe, 8, 1};

void print_binary(ostream &out, uint32_t num) {
    for (int j = 3; j >= 0; j--) {
        out << (num >> j & 1);
        if (j % 4 == 0)
            out << " ";
    }
}

uint32_t permute(uint32_t input, uint8_t *pbox) {
    bitset<32> permInput(input);
    bitset<32> permOutput;

    // For each bit in the input, lookup in pbox where it should go in the output
    for (unsigned int i = 0; i < permInput.size(); i++)
        permOutput[pbox[i]] = permInput[i];

    return (uint32_t) permOutput.to_ulong();
}

uint32_t round(uint32_t block, uint32_t key, uint8_t *pbox, uint8_t *sbox, bool last) {
    // Pad with key 
    uint32_t pad = block xor key;

    // Perform substitution 
    uint32_t subst = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t subblock = (uint8_t) (pad >> (i * 4) & 15);
        subst |= ((uint32_t) sbox[subblock]) << (i * 4);
    }

    // If this is the last round, don't perform a permutation
    if (!last)
        return permute(subst, pbox);

    return subst;
}

uint32_t dorounds(uint32_t block, uint32_t *keys, uint8_t *pbox, uint8_t *sbox) {
    return round(round(round(round(block, keys[0], pbox, sbox, false), keys[1], pbox, sbox, false), keys[2], pbox, sbox, false), keys[3], pbox, sbox, true) xor keys[4];
}

uint32_t encrypt(uint32_t block) {
    return dorounds(block, enc_keys, enc_pbox, enc_sbox);
}

uint32_t decrypt(uint32_t block) {
    return dorounds(block, dec_keys, dec_pbox, dec_sbox);
}

int main() {
    srandom(time(NULL));
    ofstream out("pc-pairs.txt");

    for (int i = 0; i < 5; i++)
        enc_keys[i] = 0;
    enc_keys[4] = (uint32_t) random();

    dec_keys[0] = enc_keys[4];
    dec_keys[4] = enc_keys[0];
    for (int i = 1; i < 4; i++)
        dec_keys[i] = permute(enc_keys[4 - i], dec_pbox);

    for (int i = 0; i < 5000; i++) {
        uint32_t block = random();
        uint32_t encrypted = encrypt(block);
        uint32_t decrypted = decrypt(encrypted);
        if (block == decrypted)
            out << block << " " << encrypted << endl;
    }

    cout << "Try to guess these key bits: ";
    print_binary(cout, enc_keys[4] >> 4 & 15);
    print_binary(cout, enc_keys[4] & 15);
    cout << endl;

    return 0;
}
