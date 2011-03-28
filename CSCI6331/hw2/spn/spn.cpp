#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <bitset>
#include <stdint.h>

using namespace std;

// Hold permutation and substitution lookup tables
static uint8_t pbox[32];
static uint8_t sbox[256];

inline uint32_t permute(uint32_t input) {
    bitset<32> permInput(input);
    bitset<32> permOutput;

    // For each bit in the input, lookup in pbox where it should go in the output
    for (unsigned int i = 0; i < permInput.size(); i++) {
        permOutput[pbox[i]] = permInput[i];
    }

    return (uint32_t) permOutput.to_ulong();
}

uint32_t round(uint32_t block, uint32_t key, bool last) {
    // Pad with key 
    uint32_t pad = block xor key;

    // Perform substitution 
    uint32_t subst = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t subblock = (uint8_t) (pad >> (i * 8));
        subst |= ((uint32_t) sbox[subblock]) << (i * 8);
    }

    // If this is the last round, don't perform a substitution
    if (!last)
        return permute(subst);

    return subst;
}

int main() {
    ifstream input("input_spn.txt");
    ofstream out("output_spn.txt");

    /*
     * Read input file
     */
    uint32_t inputKeys[5];
    for (int i = 0; i < 5; i++) {
        unsigned long key;  // long enough to hold uint32_t and be read using >>
        input >> key;
        inputKeys[i] = (uint32_t) key;
    }

    uint8_t inputPbox[32];
    for (int i = 0; i < 32; i++) {
        unsigned short bit; // long enough to hold uint8_t and be read using >>
        input >> bit;
        inputPbox[i] = (uint8_t) bit;
    }

    uint8_t inputSbox[256];
    for (int i = 0; i < 256; i++) {
        unsigned short subst;
        input >> subst;
        inputSbox[i] = (uint8_t) subst;
    }

    int numBlocks;
    input >> numBlocks;

    vector<uint32_t> blocks;
    for (int i = 0; i < numBlocks; i++) {
        unsigned long block;
        input >> block;
        blocks.push_back((uint32_t) block);
    }

    bool decrypt;
    input >> decrypt;

    uint32_t keys[5];

    /*
     * Set the global lookup tables based on whether we are performing
     * an encryption or decryption
     */
    if (decrypt) {
        for (int i = 0; i < 256; i++)
            sbox[inputSbox[i]] = i;

        for (int i = 0; i < 32; i++)
            pbox[inputPbox[i]] = i;

        keys[0] = inputKeys[4];
        keys[4] = inputKeys[0];
        for (int i = 1; i < 4; i++)
            keys[i] = permute(inputKeys[4 - i]);
    } else {
        for (int i = 0; i < 5; i++)
            keys[i] = inputKeys[i];

        for (int i = 0; i < 32; i++)
            pbox[i] = inputPbox[i];

        for (int i = 0; i < 256; i++)
            sbox[i] = inputSbox[i];
    }


    // Output keys 
    for (int i = 0; i < 5; i++) {
        out << inputKeys[i];
        if (i < 4)
            out << " ";
    }
    out << endl;

    // Output pbox 
    for (int i = 0; i < 32; i++) {
        out << (int) inputPbox[i];
        if (i < 31)
            out << " ";
    }
    out << endl;

    // Output sbox 
    for (int i = 0; i < 256; i++) {
        out << (int) inputSbox[i];
        if (i < 255)
            out << " ";
    }
    out << endl;

    out << numBlocks << endl;

    // Output crypted message
    for (unsigned int i = 0; i < blocks.size(); i++) {
        /*
         * Do the encryption...
         * Perform four rounds and then pad with the last key.
         */ 
        uint32_t newBlock = round(round(round(round(blocks[i], keys[0], false), keys[1], false), keys[2], false), keys[3], true) xor keys[4];
        out << newBlock;
        if (i < blocks.size() - 1)
            out << " ";
    }
    out << endl;

    out << !decrypt << endl;

    return 0;
}
