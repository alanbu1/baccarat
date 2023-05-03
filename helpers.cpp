#include "helpers.h"
#include "globals.h"
#include <tuple>

#define NUM_CARDS 13
std::string cardArray[13] = {"A", "2", "3",  "4", "5", "6", "7",
                        "8", "9", "10", "J", "Q", "K"};
#define NUM_SUITS 4
char suitArray[4] = {'D', 'C', 'H', 'S'};


void initializeDeck()  {
    // initialize 8 decks
    for (int deck = 0; deck < NUM_DECKS; deck++) {
        for (int card = 0; card < NUM_CARDS; card++) {
            for (int suit = 0; suit < NUM_SUITS; suit++) {
                Card cardInsert;
                cardInsert.suit = suitArray[suit];
                cardInsert.card = cardArray[card];
                cardInsert.value = cardToValue[cardArray[card]];  
                sortedDeck.push_back(cardInsert); 
            }
        }
    }
}
void shuffleDeck() {
    // makes shuffledDeck a shuffledDeck
    if (!simulating && !uninitialized)  {
        std::cout << "Looks like it's the shoe! *Shuffling*" << std::endl; 
    }

    shuffledDeck = sortedDeck; 
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    std::shuffle(std::begin(shuffledDeck), std::end(shuffledDeck), e);
}
void initializeCardToValue() {
    for (int card = 0; card < NUM_CARDS; card++) {
        if (card < 9) {
            cardToValue[cardArray[card]] = (card + 1);
        } else {
            cardToValue[cardArray[card]] = 0; 
        }
    }
}
void initializeEverything() {
    initializeCardToValue(); 
    initializeDeck();
    shuffleDeck(); 
    player.name = "Player";
    player.value = 0;

    banker.name = "Banker";
    banker.value = 0; 
    uninitialized = false; 
}
void reset() {
    betP = false;
    betB = false; 
    betT = false;
    player.cards.clear();
    banker.cards.clear();
    player.value = 0;
    banker.value = 0;  
    userWin = false; 
}
void addCard(pOrB &pB)  {
    // get the first card of shuffled deck then pop it 
    pB.cards.push_back(shuffledDeck[0]); 
    pB.value += shuffledDeck[0].value;
    if (pB.value >= MAX_NUM) {
        pB.value -= MAX_NUM; 
    }
    shuffledDeck.erase(shuffledDeck.begin()); // pop front
}
int dealGame()  {
    // deals the cards, prints them (if print), and returns winner, defined in globals

    bool printIt = !simulating; // don't print if we are simulating
    // first card
    addCard(player);
    print(printIt, player);
    addCard(banker);
    print(printIt, banker);

    // second card
    addCard(player);
    print(printIt, player);
    addCard(banker); 
    print(printIt, banker);

    if (checkNatural()) { // if there's a natural game ends
        return checkWinner(); 
    }

    if (checkPlayer()) {
        addCard(player);
    }
    if (checkBanker())  {
        addCard(banker);
    }

    if (checkPlayer() || checkBanker())  {
        print(printIt, player);
        print(printIt, banker); 
    }
    return checkWinner();
}

void print(bool print, pOrB pB)  {
    if (!print)
        return;

    std::cout << pB.name << ":";
    for (unsigned i = 0; i < pB.cards.size(); i++) {
        std::cout << " " << pB.cards[i].card << pB.cards[i].suit; 
    }
    std::cout << std::endl;
    std::cout << pB.name << " total: " << pB.value << std::endl << std::endl; 
}

bool checkPlayer()  {
    // stand on 6+
    if (player.value < 6) {
        return true; 
    }
    return false; 
} // checks if player hits 3rd card

bool checkBanker()  {
    // When Banker’s first two cards total:
    // 0 – 1 – 2 : Bank draws unless Player shows a natural 8 or 9.
    // 3 : Bank draws if Player’s third card is 0 – 1 – 2 – 3 – 4 – 5 – 6 – 7 or 9.
    // Bank stands when Player’s third card is 8.
    // 4 : Bank draws if Player’s third card is 2 – 3 – 4 – 5 – 6 – 7.
    // Bank stands when Player’s third card is 0 – 1 – 8 – 9.
    // 5 : Bank draws if Player’s third card is 4 – 5 – 6 – 7.
    // Bank stands when Player’s third card is 0 – 1 – 2 – 3 – 8 – 9.
    // 6 : Bank draws if Player’s third card is 6 – 7.
    // Bank stands when Player’s third card is 0 – 1 – 2 – 3 – 4 – 5 – 8 – 9.
    // 7 : Stands.
    // 8 – 9 : Natural Numbers – neither hand draws a third card.
    if (banker.value >= 7) {
        return false;
    } else if (banker.value <= 2) {
        return true;
    } else if (banker.value == 3 && player.value != 8) {
        return true;
    } else if (banker.value == 4 && (player.value >= 2 && player.value <= 7)) {
        return true;
    } else if (banker.value == 5 && (player.value >= 4 && player.value <= 7)) {
        return true; 
    } else if (banker.value == 6 && (player.value == 6 || player.value == 7)) {
        return true; 
    } else {
        return false;
    } 
} // checks if banker hits 3rd card

bool checkNatural() {
    if (player.value >= NATURAL || banker.value >= NATURAL)
        return true; 
    return false; 
}

int checkWinner()  {
    // check winner and also reset everything 
    if (player.value > banker.value)  {
        return PLAYER_WIN;
    }
    else if (banker.value > player.value) {
        return BANKER_WIN;
    } else  {
        return TIE_WIN;
    }
}

bool checkShoe()  {
    // checks if we need to shuffle 
    if (shuffledDeck.size() <= CARDS_SHOE)
        return true;
    return false; 
}

bool checkUserWin(int win)  {
    if (betB && (win == BANKER_WIN)) {
        return true;
    } else if (betP && (win == PLAYER_WIN))  {
        return true;
    } else  {
        return false; 
    }
}

void updateBalance(int win, double &balance, double bet, bool user)  {
    if (win == BANKER_WIN && user)  {
        if (!simulating) 
            std::cout << "You Win!" << std::endl;
        balance += bet * WIN_MULTIPLIER_BANKER; 
    } else if (win == TIE_WIN && betT)  {
        if (!simulating) 
            std::cout << "You Win!" << std::endl;
        balance += bet * TIE_MULTIPLIER; 
    } else if (win == TIE_WIN)  {
        if (!simulating) 
            std::cout << "Tie! Push!" << std::endl;
        balance += bet; 
    } else if (win == PLAYER_WIN && user) {
        balance += bet * WIN_MULTIPLIER_PLAYER;
        if (!simulating) 
            std::cout << "You Win!" << std::endl;
    } else {
        if (!simulating) 
            std::cout << "You Lose!" << std::endl; 
    }
}
