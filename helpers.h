#ifndef HELPERS_H
#define HELPERS_H

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include "globals.h"
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <cctype>
#include <regex>
#include <set>
#include <thread>
#include <algorithm>
#include <random>
#include <unordered_map>

using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

// structs
struct Card {
    char suit;
    std::string card; 
    int value; 
};

struct pOrB {
    std::vector<Card> cards;
    int value; 
    std::string name; 
};

// extern
extern bool uninitialized;
extern std::vector<Card> sortedDeck;
extern std::vector<Card> shuffledDeck;
extern std::vector<Card> curDeck; 
extern std::unordered_map<std::string, int> cardToValue; 
extern bool betP;
extern bool betB; 
extern bool betT;
extern pOrB player;
extern pOrB banker; 
extern bool simulating; 
extern bool userWin; 
// extern std::string cardArray[13];
// extern char suitArray[4]; 
// helper functions 
void initializeDeck(); //Initializes 8 decks, in a sorted order
void shuffleDeck();
void initializeCardToValue();
void initializeEverything();
void reset(); 
void addCard(pOrB &pB); 
int dealGame(); 
void print(bool print, pOrB pB);

bool checkPlayer(); // checks if player hits 3rd card
bool checkBanker(); // checks if banker hits 3rd card
bool checkNatural(); // checks if there has been a natural
int checkWinner(); // determines winner player or banker
bool checkUserWin(int win); // checks if user wins
bool checkShoe();
void updateBalance(int win, double &balance, double bet, bool userWin); 
#endif
