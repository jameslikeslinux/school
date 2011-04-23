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
    int imask, omask;
    int count;
} count_t;

static count_t counts[256];
static uint8_t dec_sbox[16] = {9, 0xb, 6, 0x2, 7, 0xc, 0xd, 0xf, 5, 3, 0, 4, 0xa, 0xe, 8, 1};
static int numpcpairs = 0;

bool sort_counts(const count_t &a, const count_t &b) {
    if (a.count == 0)
        return false;

    if (b.count == 0)
        return true;

    return abs(a.count - numpcpairs / 2) > abs(b.count - numpcpairs / 2);
}

void print_binary(ostream &out, uint16_t num) {
    for (int j = 3; j >= 0; j--)
       out << (num >> j & 1);
}

int main(int argc, char *argv[]) {
    ifstream in("pc-pairs.txt");

    for (int imask = 0; imask < 16; imask++)
        for (int omask = 0; omask < 16; omask++) {
            int index = imask << 4 | omask;
            counts[index].imask = imask;
            counts[index].omask = omask;
            counts[index].count = 0;
        }

    while (!in.eof()) {
        numpcpairs++;

        uint32_t pt, ct;
        in >> pt >> ct;

        uint8_t ctbits = ct & 15;

        for (int imask = 1; imask < 16; imask++)
            for (int omask = 1; omask < 16; omask++) {
                uint8_t udec = dec_sbox[ctbits xor atoi(argv[1])];
    
                bitset<4> p(pt >> 12 & imask);
                bitset<4> u(udec & omask);

                if ((p[0] xor p[1] xor p[2] xor p[3] xor u[0] xor u[1] xor u[2] xor u[3]) == 0)
                    counts[imask << 4 | omask].count++;
            }
    }

    sort(counts, counts + 256, sort_counts);

    for (int i = 0; i < 10; i++) {
        print_binary(cout, counts[i].imask);
        cout << " ";
        print_binary(cout, counts[i].omask);
        cout << " " << counts[i].count << endl;
    }

    return 0;
}
