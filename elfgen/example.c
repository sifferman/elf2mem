
#define SIZE 0x100

const int const_array[SIZE] = { 0xdeadbeef };
int out[SIZE];

void __start(void) {
    for (int i = 0; i != SIZE; i++) {
        out[i] = i;
    }

    while (1);
}
