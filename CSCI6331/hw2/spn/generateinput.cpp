#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <bitset>
#include <stdint.h>

using namespace std;

const int MESSAGE_LENGTH = 8;

int main() {
    // Seed random number generator
    srand(time(0));

    // Generate key
    for (int i = 0; i < 5; i++) {
        uint32_t key = (uint32_t) rand();
        cout << key;
        if (i < 4)
            cout << " ";
    }
    cout << endl;

    // Generate P-box
    vector<uint8_t> pbox;
    for (int i = 0; i < 32; i++)
        pbox.push_back(i);

    for (int i = 0; i < 32; i++) {
        int index = rand() % pbox.size();
        uint8_t bit = pbox[index];
        pbox.erase(pbox.begin() + index);
        cout << (int) bit;
        if (i < 31)
            cout << " ";
    }
    cout << endl;

    // Generate S-box
    vector<uint8_t> sbox;
    for (int i = 0; i < 256; i++)
        sbox.push_back(i);

    for (int i = 0; i < 256; i++) {
        int index = rand() % sbox.size();
        uint8_t subst = sbox[index];
        sbox.erase(sbox.begin() + index);
        cout << (int) subst;
        if (i < 255)
            cout << " ";
    }
    cout << endl;

    // Output message length
    cout << MESSAGE_LENGTH << endl;

    // Generate message
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
        cout << (uint32_t) rand();
        if (i < MESSAGE_LENGTH - 1)
            cout << " ";
    }
    cout << endl;

    // Perform encryption
    cout << 0 << endl;

    return 0;
}
