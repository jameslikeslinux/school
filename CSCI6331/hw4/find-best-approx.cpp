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
} count_for_key_t;

typedef struct {
    int imask, omask;
    count_for_key_t forkey[16];
} count_t;

static count_t counts[225];
static uint8_t dec_sbox[16] = {9, 0xb, 6, 0x2, 7, 0xc, 0xd, 0xf, 5, 3, 0, 4, 0xa, 0xe, 8, 1};
static int numpcpairs = 0;

bool sort_key_counts(const count_for_key_t &a, const count_for_key_t &b) {
    return a.count > b.count;
}

void print_binary(ostream &out, uint16_t num) {
    for (int j = 3; j >= 0; j--)
       out << (num >> j & 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <keybits>\n";
        return 1;
    }

    int keybits = strtol(argv[1], NULL, 2);

    for (int i = 0, imask = 1; imask < 16; imask++)
        for (int omask = 1; omask < 16; omask++, i++) {
            counts[i].imask = imask;
            counts[i].omask = omask;

            for (int key = 0; key < 16; key++) {
                counts[i].forkey[key].key = key;
                counts[i].forkey[key].count = 0;
            }
    
            ifstream in("pc-pairs.txt");
            numpcpairs = 0;

            while (!in.eof()) {
                uint32_t pt32, ct32;
                in >> pt32 >> ct32;
                numpcpairs++;

                int pt4 = (pt32 >> 12) & 15;
                int ct4 = ct32 & 15;

                for (int key = 0; key < 16; key++) {
                    bitset<4> ptbits(pt4 & imask);
                    bitset<4> ctbits(dec_sbox[ct4 xor key] & omask);

                    if ((ptbits[0] xor ptbits[1] xor ptbits[2] xor ptbits[3] xor
                         ctbits[0] xor ctbits[1] xor ctbits[2] xor ctbits[3]) == 0)
                            counts[i].forkey[key].count++;
                }
            }

            in.close();
        }

    for (int i = 0; i < 225; i++)
        for (int key = 0; key < 16; key++)
            counts[i].forkey[key].count = abs(counts[i].forkey[key].count - numpcpairs / 2);

    for (int i = 0; i < 225; i++)
        sort(counts[i].forkey, counts[i].forkey + 16, sort_key_counts);
    
    int highestindex = -1, highestcount = 0;
    for (int i = 0; i < 225; i++) {
        int maxcount = counts[i].forkey[0].count;
        for (int j = 0; counts[i].forkey[j].count == maxcount && j < 16; j++)
            if (counts[i].forkey[j].key == keybits && maxcount > highestcount) {
                highestindex = i;
                highestcount = maxcount;
            }
    }
    
    cout << "Found key " << keybits << " with approx (" << hex << counts[highestindex].imask << ", " << counts[highestindex].omask << ") and bias " << dec << highestcount << endl;

    return 0;
}
