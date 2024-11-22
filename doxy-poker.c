/**
 * @file main.c
 * @brief Implementation of poker game simulation including deck creation, shuffling, dealing, hand evaluation, and Monte Carlo simulation.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

#define NUM_CARDS 52  /**< Total number of cards in a standard deck. */
#define NUM_RANKS 13  /**< Number of distinct card ranks. */
#define NUM_SUITS 4   /**< Number of distinct card suits. */
#define HAND_SIZE 2   /**< Number of cards in a player's hand. */
#define FLOP_SIZE 3   /**< Number of community cards in the flop. */
#define TURN_RIVER_SIZE 1  /**< Number of cards in the turn and river phases. */
#define MONTE_CARLO_SIMULATIONS 10000  /**< Number of simulations for Monte Carlo method. */

/**
 * @brief Array of card rank names.
 */
const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

/**
 * @brief Array of card suit symbols.
 */
const char *suits[] = {"\u2665", "\u2666", "\u2663", "\u2660"}; /**< ♥ ♦ ♣ ♠ */

/**
 * @struct Card
 * @brief Structure representing a card in the deck.
 */
typedef struct {
  int rank; /**< Rank of the card (0-12 corresponding to '2'-'A'). */
  int suit; /**< Suit of the card (0-3 corresponding to ♥, ♦, ♣, ♠). */
} Card;

/**
 * @struct HandEvaluation
 * @brief Structure representing the evaluation of a poker hand.
 */
typedef struct {
  int combination_level; /**< Level of the hand combination (e.g., straight, flush). */
  int highest_card; /**< Highest card rank in the combination. */
  int second_highest_card; /**< Second highest card rank, if applicable (e.g., for two pairs). */
} HandEvaluation;

/**
 * @brief Clears the input buffer by consuming all characters until a newline or EOF.
 * @return The last character read.
 */
int clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return c;
}

/**
 * @brief Shuffles the given deck of cards.
 * @param deck Pointer to the array representing the deck.
 */
void shuffle_deck(Card *deck) {
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1); /**< Choose a random element to swap with */
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

/**
 * @brief Prints a card to the console.
 * @param card The card to print.
 */
void print_card(Card card) {
    printf("[ %s%s ]", ranks[card.rank], suits[card.suit]);
}

/**
 * @brief Initializes a deck of 52 cards.
 * @param deck Pointer to the array representing the deck.
 */
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

/**
 * @brief Deals cards from the deck to a specified hand.
 * @param deck Pointer to the array representing the deck.
 * @param current_card Pointer to the index of the current card to be dealt.
 * @param hand Pointer to the array representing the player's hand.
 * @param size Number of cards to deal.
 */
void deal_cards(Card *deck, int *current_card, Card *hand, int size) {
    for (int i = 0; i < size; i++) {
        hand[i] = deck[*current_card];
        (*current_card)++;
    }
}

/**
 * @brief Compares two cards by rank for sorting from highest to lowest.
 * @param a Pointer to the first card.
 * @param b Pointer to the second card.
 * @return Positive if the rank of the second card is greater, negative if the first card is greater, zero if equal.
 */
int compare_cards(const void *a, const void *b) {
    Card *cardA = (Card *)a;
    Card *cardB = (Card *)b;
    return cardB->rank - cardA->rank;
}

/**
 * @brief Evaluates the strength of a player's hand combined with community cards.
 * @param hand Pointer to the player's hand.
 * @param community_cards Pointer to the community cards on the table.
 * @param community_size The number of community cards.
 * @return A HandEvaluation structure representing the hand's strength.
 */
HandEvaluation evaluate_hand(Card *hand, Card *community_cards, int community_size) {
    Card all_cards[HAND_SIZE + community_size];
    for (int i = 0; i < HAND_SIZE; i++) {
        all_cards[i] = hand[i];
    }
    for (int i = 0; i < community_size; i++) {
        all_cards[HAND_SIZE + i] = community_cards[i];
    }

    qsort(all_cards, HAND_SIZE + community_size, sizeof(Card), compare_cards);

    int rank_count[NUM_RANKS] = {0};
    int suit_count[NUM_SUITS] = {0};
    for (int i = 0; i < HAND_SIZE + community_size; i++) {
        rank_count[all_cards[i].rank]++;
        suit_count[all_cards[i].suit]++;
    }

    HandEvaluation eval = {0, all_cards[0].rank, -1};

    for (int i = 0; i < NUM_SUITS; i++) {
        if (suit_count[i] >= 5) {
            eval.combination_level = 6;
            return eval;
        }
    }

    bool has_three = false;
    bool has_pair = false;
    for (int i = 0; i < NUM_RANKS; i++) {
        if (rank_count[i] == 4) {
            eval.combination_level = 8;
            eval.highest_card = i;
            return eval;
        }
        if (rank_count[i] == 3) {
            has_three = true;
            eval.highest_card = i;
        } else if (rank_count[i] == 2) {
            has_pair = true;
            if (eval.second_highest_card == -1 || i > eval.second_highest_card) {
                eval.second_highest_card = i;
            }
        }
    }
    if (has_three && has_pair) {
        eval.combination_level = 7;
        return eval;
    } else if (has_three) {
        eval.combination_level = 3;
        return eval;
    }

    int consecutive = 0;
    for (int i = NUM_RANKS - 1; i >= 0; i--) {
        if (rank_count[i] > 0) {
            consecutive++;
            if (consecutive >= 5) {
                eval.combination_level = 4;
                eval.highest_card = i + 4;
                return eval;
            }
        } else {
            consecutive = 0;
        }
    }

    int pairs = 0;
    for (int i = 0; i < NUM_RANKS; i++) {
        if (rank_count[i] == 2) {
            pairs++;
            if (eval.highest_card < i) {
                eval.second_highest_card = eval.highest_card;
                eval.highest_card = i;
            } else if (eval.second_highest_card < i) {
                eval.second_highest_card = i;
            }
        }
    }
    if (pairs >= 2) {
        eval.combination_level = 2;
        return eval;
    } else if (pairs == 1) {
        eval.combination_level = 1;
        return eval;
    }

    eval.combination_level = 0;
    return eval;
}

/**
 * @brief Determines if the first hand is better than the second.
 * @param hand1 Pointer to the first player's hand.
 * @param hand2 Pointer to the second player's hand.
 * @param community_cards Pointer to the community cards on the table.
 * @param community_size The number of community cards.
 * @return True if the first hand is better, false otherwise.
 */
bool is_hand_better(Card *hand1, Card *hand2, Card *community_cards, int community_size) {
    HandEvaluation eval1 = evaluate_hand(hand1, community_cards, community_size);
    HandEvaluation eval2 = evaluate_hand(hand2, community_cards, community_size);

    if (eval1.combination_level > eval2.combination_level) {
        return true;
    } else if (eval1.combination_level < eval2.combination_level) {
        return false;
    } else {
        if (eval1.highest_card > eval2.highest_card) {
            return true;
        } else if (eval1.highest_card < eval2.highest_card) {
            return false;
        } else {
            if (eval1.second_highest_card != -1 && eval2.second_highest_card != -1) {
                if (eval1.second_highest_card > eval2.second_highest_card) {
                    return true;
                } else if (eval1.second_highest_card < eval2.second_highest_card) {
                    return false;
                }
            } else if (eval1.second_highest_card != -1) {
                return true;
            } else if (eval2.second_highest_card != -1) {
                return false;
            } else {
                for (int i = HAND_SIZE + community_size - 1; i >= 0; i--) {
                    if (community_cards[i].rank == -1) {
                        continue;
                    }
                    if (hand1[i].rank > hand2[i].rank) {
                        return true;
                    } else if (hand1[i].rank < hand2[i].rank) {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * @brief Finds the winning opponent among multiple hands.
 * @param player_hand Pointer to the player's hand.
 * @param opponent_hands Array of hands representing opponents.
 * @param num_opponents The number of opponents.
 * @param community_cards Pointer to the community cards on the table.
 * @param community_size The number of community cards.
 * @return The index of the winning opponent, or -1 if the player wins.
 */
int find_winning_opponent(Card *player_hand, Card opponent_hands[][HAND_SIZE], int num_opponents, Card *community_cards, int community_size) {
    HandEvaluation player_eval = evaluate_hand(player_hand, community_cards, community_size);
    int winning_opponent_index = -1;
    bool player_wins = true;

    for (int i = 0; i < num_opponents; i++) {
        HandEvaluation opponent_eval = evaluate_hand(opponent_hands[i], community_cards, community_size);
        if (opponent_eval.combination_level > player_eval.combination_level) {
            player_wins = false;
            winning_opponent_index = i;
        } else if (opponent_eval.combination_level == player_eval.combination_level) {
            if (opponent_eval.highest_card > player_eval.highest_card) {
                player_wins = false;
                winning_opponent_index = i;
            } else if (opponent_eval.highest_card == player_eval.highest_card && opponent_eval.second_highest_card > player_eval.second_highest_card) {
                player_wins = false;
                winning_opponent_index = i;
            }
        }
    }

    return player_wins ? -1 : winning_opponent_index;
}

/**
 * @brief Calculates the probability of winning using Monte Carlo simulations.
 * @param player_hand Pointer to the player's hand.
 * @param community_cards Pointer to the community cards on the table.
 * @param community_size The number of community cards.
 * @param num_players The total number of players in the game.
 * @return The probability of winning as a percentage.
 */
int calculate_win_probability(Card *player_hand, Card *community_cards, int community_size, int num_players) {
    int wins = 0, ties = 0, losses = 0;
    Card deck[NUM_CARDS];
    Card opponent_hands[num_players - 1][HAND_SIZE];
    int current_card;

    for (int i = 0; i < MONTE_CARLO_SIMULATIONS; i++) {
        initialize_deck(deck);
        shuffle_deck(deck);
        current_card = 0;

        for (int j = 0; j < HAND_SIZE; j++) {
            deck[player_hand[j].rank * NUM_SUITS + player_hand[j].suit].rank = -1;
        }
        for (int j = 0; j < community_size; j++) {
            deck[community_cards[j].rank * NUM_SUITS + community_cards[j].suit].rank = -1;
        }

        for (int k = 0; k < num_players - 1; k++) {
            for (int j = 0; j < HAND_SIZE; j++) {
                do {
                    opponent_hands[k][j] = deck[current_card];
                    current_card++;
                } while (opponent_hands[k][j].rank == -1);
            }
        }

        int winning_opponent_index = find_winning_opponent(player_hand, opponent_hands, num_players - 1, community_cards, community_size);
        if (winning_opponent_index == -1) {
            wins++;
        } else {
            losses++;
        }
    }

    return (wins * 100) / (wins + losses + ties);
}

/**
 * @brief Main function to simulate the poker game.
 * @return 0 if the program executes successfully, 1 otherwise.
 */
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

    printf("Enter your action (F to fold / C to continue): ");
    scanf(" %c", &action);
    if (action == 'F' || action == 'f') {
        printf("You have folded. The game ends here.\n");
        return 0;
    }
    clear_buffer();

    win_probability = calculate_win_probability(player_hands[0], community_cards, FLOP_SIZE, num_players);
    printf("** Probability of Winning After Flop **\n");
    printf("Your probability of winning is: %d%%\n", win_probability);

    printf("Enter your action (F to fold / C to continue): ");
    scanf(" %c", &action);
    if (action == 'F' || action == 'f') {
        printf("You have folded. The game ends here.\n");
        return 0;
    }
    clear_buffer();

    deal_cards(deck, &current_card, &community_cards[FLOP_SIZE], TURN_RIVER_SIZE);
    printf("** Community Cards (Turn) **\n");
    for (int i = 0; i < FLOP_SIZE + TURN_RIVER_SIZE; i++) {
        print_card(community_cards[i]);
        printf(" ");
    }
    printf("\n");

    win_probability = calculate_win_probability(player_hands[0], community_cards, FLOP_SIZE + TURN_RIVER_SIZE, num_players);
    printf("** Probability of Winning After Turn **\n");
    printf("Your probability of winning is: %d%%\n", win_probability);

    printf("Enter your action (F to fold / C to continue): ");
    scanf(" %c", &action);
    if (action == 'F' || action == 'f') {
        printf("You have folded. The game ends here.\n");
        return 0;
    }
    clear_buffer();

    deal_cards(deck, &current_card, &community_cards[FLOP_SIZE + TURN_RIVER_SIZE], TURN_RIVER_SIZE);
    printf("** Community Cards (River) **\n");
    for (int i = 0; i < FLOP_SIZE + TURN_RIVER_SIZE * 2; i++) {
        print_card(community_cards[i]);
        printf(" ");
    }
    printf("\n");

    win_probability = calculate_win_probability(player_hands[0], community_cards, FLOP_SIZE + TURN_RIVER_SIZE * 2, num_players);
    printf("** Final Probability of Winning After River **\n");
    printf("Your final probability of winning is: %d%%\n", win_probability);

    int winning_opponent_index = find_winning_opponent(player_hands[0], player_hands + 1, num_players - 1, community_cards, FLOP_SIZE + TURN_RIVER_SIZE * 2);
    if (winning_opponent_index == -1) {
        printf("** Player 1 wins **\n");
    } else {
        printf("** Bot %d wins **\n", winning_opponent_index + 2);
    }

    return 0;
}
