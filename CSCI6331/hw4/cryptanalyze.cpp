#include <iostream>
#include <iomanip>
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

static count_t counts1[16], counts2[16];
static uint8_t dec_sbox[16] = {9, 0xb, 6, 0x2, 7, 0xc, 0xd, 0xf, 5, 3, 0, 4, 0xa, 0xe, 8, 1};
static int numpcpairs = 0;

bool sort_counts(const count_t &a, const count_t &b) {
    return a.count > b.count;
}

void print_binary(ostream &out, uint16_t num) {
    for (int j = 3; j >= 0; j--)
       out << (num >> j & 1);
}

int main() {
    ifstream in("pc-pairs.txt");

    for (int key = 0; key < 16; key++) {
        counts1[key].key = key;
        counts1[key].count = 0;
        counts2[key].key = key;
        counts2[key].count = 0;
    }

    while (!in.eof()) {
        numpcpairs++;

        uint32_t pt32, ct32;
        in >> pt32 >> ct32;

        int pt4 = (pt32 >> 12) & 15;
        int ct4 = ct32 & 15;

        for (int key = 0; key < 16; key++) {
            bitset<4> ptbits(pt4 & 0x1);
            bitset<4> ctbits(dec_sbox[ct4 xor key] & 0xe);

            if ((ptbits[0] xor ptbits[1] xor ptbits[2] xor ptbits[3] xor
                 ctbits[0] xor ctbits[1] xor ctbits[2] xor ctbits[3]) == 0)
                    counts1[key].count++;
        }

        pt4 = (pt32 >> 16) & 15;
        ct4 = (ct32 >> 4) & 15;

        for (int key = 0; key < 16; key++) {
            bitset<4> ptbits(pt4 & 0x1);
            bitset<4> ctbits(dec_sbox[ct4 xor key] & 0xe);

            if ((ptbits[0] xor ptbits[1] xor ptbits[2] xor ptbits[3] xor
                 ctbits[0] xor ctbits[1] xor ctbits[2] xor ctbits[3]) == 0)
                    counts2[key].count++;
        }
    }

    for (int i = 0; i < 16; i++) {
        counts1[i].count = abs(counts1[i].count - numpcpairs / 2);
        counts2[i].count = abs(counts2[i].count - numpcpairs / 2);
    }

    sort(counts1, counts1 + 16, sort_counts);
    sort(counts2, counts2 + 16, sort_counts);

    cout << "Top 16 keys:\n";
    cout << "Key1  Count1  Key2  Count2\n";
    cout << "--------------------------\n";
    for (int i = 0; i < 16; i++) {
        print_binary(cout, counts2[i].key);
        cout << "  " << setw(6) << counts1[i].count << "  ";
        print_binary(cout, counts1[i].key);
        cout << "  " << setw(6) << counts1[i].count << endl;
    }

    return 0;
}
