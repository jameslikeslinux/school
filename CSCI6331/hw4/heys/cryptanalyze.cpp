#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <bitset>
#include <algorithm>
#include <stdint.h>

using namespace std;

typedef struct {
    int kl, kr;
    int count;
} count_t;

static count_t counts[256];
static uint8_t dec_sbox[16] = {0xe, 3, 4, 8, 1, 0xc, 0xa, 0xf, 7, 0xd, 9, 6, 0xb, 2, 0, 5};
static int numpcpairs = 0;

bool sort_counts(const count_t &a, const count_t &b) {
    return abs(a.count - numpcpairs / 2) > abs(b.count - numpcpairs / 2);
}

void print_binary(ostream &out, uint16_t num) {
    for (int j = 3; j >= 0; j--)
       out << (num >> j & 1);
}

int main() {
    ifstream in("pc-pairs.txt");

    for (int kl = 0; kl < 16; kl++)
        for (int kr = 0; kr < 16; kr++) {
            int index = kl << 4 | kr;
            counts[index].kl = kl;
            counts[index].kr = kr;
            counts[index].count = 0;
        }

    while (!in.eof()) {
        numpcpairs++;

        uint16_t pt, ct;
        in >> pt >> ct;
        bitset<16> ptbits(pt);

        uint8_t ctl, ctr;
        ctl = ct >> 8 & 15;
        ctr = ct & 15;

        for (int kl = 0; kl < 16; kl++)
            for (int kr = 0; kr < 16; kr++) {
                bitset<4> ul(dec_sbox[ctl xor kl]);
                bitset<4> ur(dec_sbox[ctr xor kr]);

                if ((ptbits[8] xor ptbits[9] xor ptbits[11] xor ul[0] xor ul[2] xor ur[0] xor ur[2]) == 0)
                    counts[(kl << 4 | kr)].count++;
            }
    }

    sort(counts, counts + 256, sort_counts);

    cout << "Guessing: ";
    print_binary(cout, counts[0].kl);
    cout << " ";
    print_binary(cout, counts[0].kr);
    cout << endl;

    return 0;
}
