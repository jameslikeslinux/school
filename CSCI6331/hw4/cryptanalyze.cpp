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
    int key;
    int count;
} count_t;

static count_t counts[16];
static uint8_t dec_sbox[16] = {9, 0xb, 6, 0x2, 7, 0xc, 0xd, 0xf, 5, 3, 0, 4, 0xa, 0xe, 8, 1};
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

    for (int key = 0; key < 16; key++) {
        counts[key].key = key;
        counts[key].count = 0;
    }

    while (!in.eof()) {
        numpcpairs++;

        uint32_t pt, ct;
        in >> pt >> ct;
        bitset<32> ptbits(pt);

        uint8_t ctbits = ct & 15;

        for (int key = 0; key < 16; key++) {
            bitset<4> u(dec_sbox[ctbits xor key]);

            if ((ptbits[15] xor ptbits[13] xor ptbits[12] xor u[3]) == 0)
                counts[key].count++;
        }
    }

    sort(counts, counts + 16, sort_counts);

    for (int i = 0; i < 10; i++) {
        print_binary(cout, counts[i].key);
        cout << " " << counts[i].count << endl;
    }

    return 0;
}
