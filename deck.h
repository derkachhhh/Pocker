
#ifndef DECK_H
#define DECK_H

#define NUM_CARDS 52
#define NUM_RANKS 13
#define NUM_SUITS 4
#define HAND_SIZE 2

typedef struct {
    int rank;
    int suit;
} Card;

void initialize_deck(Card *deck);
void shuffle_deck(Card *deck);
void deal_cards(Card *deck, int *current_card, Card *hand, int size);

#endif
