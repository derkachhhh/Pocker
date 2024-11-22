
#include "deck.h"
#include <stdlib.h>

void initialize_deck(Card *deck) {
    int index = 0;
    for (int i = 0; i < NUM_RANKS; i++) {
        for (int j = 0; j < NUM_SUITS; j++) {
            deck[index].rank = i;
            deck[index].suit = j;
            index++;
        }
    }
}

void shuffle_deck(Card *deck) {
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void deal_cards(Card *deck, int *current_card, Card *hand, int size) {
    for (int i = 0; i < size; i++) {
        hand[i] = deck[*current_card];
        (*current_card)++;
    }
}
