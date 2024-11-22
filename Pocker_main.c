
#include "deck.h"
#include "hand_evaluation.h"
#include "montecarlo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));

    int num_players = 2;
    printf("Enter the number of players (2-6): ");
    if (scanf("%d", &num_players) != 1 || num_players < 2 || num_players > 6) {
        printf("Invalid number of players. The number must be between 2 and 6.\n");
        return 1;
    }

    Card deck[NUM_CARDS];
    initialize_deck(deck);
    shuffle_deck(deck);

    int current_card = 0;
    Card player_hands[num_players][HAND_SIZE];
    Card community_cards[FLOP_SIZE + TURN_RIVER_SIZE * 2];

    for (int i = 0; i < num_players; i++) {
        deal_cards(deck, &current_card, player_hands[i], HAND_SIZE);
    }

    printf("** Your Hand **\n");
    for (int i = 0; i < HAND_SIZE; i++) {
        print_card(player_hands[0][i]);
        printf(" ");
    }
    printf("\n");

    int win_probability = calculate_win_probability(player_hands[0], NULL, 0, num_players);
    printf("** Preflop Probability of Winning **\n");
    printf("Your preflop probability of winning against %d players is: %d%%\n", num_players - 1, win_probability);

    char action;
    printf("Enter your action (F to fold / C to continue): ");
    scanf(" %c", &action);
    if (action == 'F' || action == 'f') {
        printf("You have folded. The game ends here.\n");
        return 0;
    }
    clear_buffer();

    deal_cards(deck, &current_card, community_cards, FLOP_SIZE);
    printf("** Community Cards (Flop) **\n");
    for (int i = 0; i < FLOP_SIZE; i++) {
        print_card(community_cards[i]);
        printf(" ");
    }
    printf("\n");

    // Additional phases (Turn, River) omitted for brevity

    return 0;
}
