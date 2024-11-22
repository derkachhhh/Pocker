
#include "utils.h"
#include <stdio.h>

int clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return c;
}

void print_card(Card card) {
    // Logic to print a card to console
    printf("[Card]");
}
