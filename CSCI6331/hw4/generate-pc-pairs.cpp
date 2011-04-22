#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <bitset>
#include <stdint.h>

using namespace std;

// Hold permutation and substitution lookup tables
static uint16_t enc_keys[5];
static uint8_t enc_pbox[16] = {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15};
static uint8_t enc_sbox[16] = {0xe, 4, 0xd, 1, 2, 0xf, 0xb, 8, 3, 0xa, 6, 0xc, 5, 9, 0, 7};

static uint16_t dec_keys[5];
static uint8_t *dec_pbox = enc_pbox;
static uint8_t dec_sbox[16] = {0xe, 3, 4, 8, 1, 0xc, 0xa, 0xf, 7, 0xd, 9, 6, 0xb, 2, 0, 5};

void print_binary(ostream &out, uint16_t num) {
    for (int j = 15; j >= 0; j--) {
        out << (num >> j & 1);
        if (j % 4 == 0)
            out << " ";
    }
}

uint16_t permute(uint16_t input, uint8_t *pbox) {
    bitset<16> permInput(input);
    bitset<16> permOutput;

    // For each bit in the input, lookup in pbox where it should go in the output
    for (unsigned int i = 0; i < permInput.size(); i++)
        permOutput[pbox[i]] = permInput[i];

    return (uint16_t) permOutput.to_ulong();
}

uint16_t round(uint16_t block, uint16_t key, uint8_t *pbox, uint8_t *sbox, bool last) {
    // Pad with key 
    uint16_t pad = block xor key;

    // Perform substitution 
    uint16_t subst = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t subblock = (uint8_t) (pad >> (i * 4) & 15);
        subst |= ((uint16_t) sbox[subblock]) << (i * 4);
    }

    // If this is the last round, don't perform a permutation
    if (!last)
        return permute(subst, pbox);

    return subst;
}

uint16_t dorounds(uint16_t block, uint16_t *keys, uint8_t *pbox, uint8_t *sbox) {
    return round(round(round(round(block, keys[0], pbox, sbox, false), keys[1], pbox, sbox, false), keys[2], pbox, sbox, false), keys[3], pbox, sbox, true) xor keys[4];
}

uint16_t encrypt(uint16_t block) {
    return dorounds(block, enc_keys, enc_pbox, enc_sbox);
}

uint16_t decrypt(uint16_t block) {
    return dorounds(block, dec_keys, dec_pbox, dec_sbox);
}

int main() {
    srandom(time(NULL));
    ofstream out("pc-pairs.txt");

    for (int i = 0; i < 5; i++)
        enc_keys[i] = (uint16_t) random();

    dec_keys[0] = enc_keys[4];
    dec_keys[4] = enc_keys[0];
    for (int i = 1; i < 4; i++)
        dec_keys[i] = permute(enc_keys[4 - i], dec_pbox);

    for (int i = 0; i < 10000; i++) {
        uint16_t block = random();
        uint16_t encrypted = encrypt(block);
        uint16_t decrypted = decrypt(encrypted);
        if (block == decrypted)
            out << block << " " << encrypted << endl;
    }

    cout << "Try to guess key bits in: ";
    print_binary(cout, enc_keys[4]);
    cout << endl;

    return 0;
}
