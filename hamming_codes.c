#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int sender() {
    return 0;
}

int receiver() {
    return 0;
}

int main() {

    // Hamming Codes Algorithm for data self correction on receivers end
    // Error Control Protocol
    
    // total bits in data
    int bits;
    printf("Enter the number of bits: ");
    scanf("%d", &bits);

    // inputting data
    int data[bits];
    for (int i = 0; i < bits; i++) {
        printf("Enter bit [%d]: ", i);
        scanf("%d", &data[i]);
    }

    // number of parity bits
    int j = 0;  // j = number of parity bits
    while (1) {
        if ((pow(2, j)) >= (j + bits + 1)) {
            break;
        }
        j++;
    }
    printf("Parity Bits: %d", j);

    // total code bits
    int total_bits = bits + j;
    int sender_code[total_bits];

    // storing data bits in sender code
    /*int data_bit_count = 0; 
    for (int i = 0; i < bits; i++) {
        sender_code[i] = data[data_bit_count];
        data_bit_count++;
    }*/ // faulty code

    // calculating and storing parity bits in sender code


    return 0;
}