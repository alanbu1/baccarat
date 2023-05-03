#ifndef globals_h
#define globals_h
#include <string>

#define NUM_DECKS 8 // 8 deck baccarat

#define CARDS_SHOE 250 // the size of the card array before we shuffle again 

// baccarat definitions
#define WIN_MULTIPLIER_PLAYER 2
#define WIN_MULTIPLIER_BANKER 1.95
#define TIE_MULTIPLIER 8

#define MAX_NUM 10 // max number is 9, but this is the value we are checking
#define BANKER_WIN 0 // essentially a bool  
#define PLAYER_WIN 1
#define TIE_WIN 2 

#define NATURAL 8 // natural 8 or 9
#endif /* globals_h */
