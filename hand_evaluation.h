
#ifndef HAND_EVALUATION_H
#define HAND_EVALUATION_H

#include "deck.h"

typedef struct {
    int combination_level;
    int highest_card;
    int second_highest_card;
} HandEvaluation;

HandEvaluation evaluate_hand(Card *hand, Card *community_cards, int community_size);

#endif
