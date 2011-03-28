#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    // Seed random number generator
    srand(time(0));

    // Half-block length from 16 to 128 bits
    int n = rand() % 113 + 16;

    cout << n << endl;

    // Generate key
    for (int i = 0; i < n; i++)
        cout << rand() % 2;
    cout << endl;

    // Generate message
    for (int i = 0; i < n * 2; i++)
        cout << rand() % 2;
    cout << endl;

    cout << 0 << endl;

    return 0;
}
