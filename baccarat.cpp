// blackjack game under the assumption of multiple decks
#include <chrono>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
// #include "globals.h"
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

#include "globals.h"
#include "helpers.h"

// helper functions
void simulate();
void play(double &balance); 

// global variables 
std::vector<Card> sortedDeck;
std::vector<Card> shuffledDeck;
std::vector<Card> curDeck; 

std::unordered_map<std::string, int> cardToValue; 
bool uninitialized = true;

// to be reset in reset() 
bool betP = false;
bool betB = false; 
pOrB player;
pOrB banker; 
bool simulating = false; 
bool userWin = false; 
bool betT = false; 
// main
int main() {
  // Get user input to decide whether or not to play, cash out, or simulate 
  initializeEverything();
  double startingBal = 0;
  double balance = 0;
  std::string line;
  // Get starting balance
  std::cout << "Welcome to Baccarat." << std::endl;
  // Prompt for input
  while (startingBal == 0 || std::cin.fail()) {
    std::cout << "Enter the balance to start with: " << std::endl;
    std::cout << "> ";
    std::cin >> startingBal;
    if (std::cin.fail() || startingBal <= 0) {
      std::cout << "Error: Invalid Value." << std::endl;
      startingBal = 0;
      std::cin.clear();
    }
  }
  balance = startingBal;
  bool cashedOut = false;
  char command;
  getline(std::cin, line); // Get a line from standard input
  std::cout << "\n\n"
      << "Your balance: " << balance << std::endl;
  std::cout << "Enter a command:" << std::endl;
  std::cout << "Play or Cash out or Simulate (P/C/S)" << std::endl;
  std::cout << "> ";
  while (!std::cin.eof() && (cashedOut == false) && balance != 0) {
    std::cin >> command;
    // If command is deal: play game
    if ((command == 'p') || (command == 'P')) {
      play(balance);
    } else if (command == 'c' || command == 'C') {
      cashedOut = true;
    } else if (command == 's' || command == 'S') {
      simulate();
    } else {
      std::cout << "Invalid command. Re-enter." << std::endl;
    }
    sleep_for(250ms);
    getline(std::cin, line);

    if (!cashedOut && balance != 0) {
      std::cout << "\n\n"
          << "Your balance: " << balance << std::endl;
      std::cout << "Enter a command:" << std::endl;
      std::cout << "Play or Cash out or Simulate (P/C/S)" << std::endl;
      std::cout << "> "; // Prompt for input
    }
  }

  if (balance == 0) {
    std::cout << "\n\n"
        << "Game Over! You lost!" << std::endl;
    sleep_for(1000ms);
  } else if (cashedOut == true) {
    double profit = balance - startingBal;
    std::cout << "You cashed out " << balance << " dollars. You profited: " << profit
        << " dollars"
        << "." << std::endl;
  }

  return 0;
}

// helper functions:
void play(double &balance) {
  // Starting hand has a top and bottom card for both player and dealer.
  if (checkShoe()) {
    shuffleDeck(); 
  }

  std::cout << "\n";
  double bet = 0;
  std::cout << "Please enter the amount you would like to bet" << std::endl;
  std::cout << "> ";
  while (bet == 0 || bet > balance || std::cin.fail()) {
    std::cin.clear();
    std::cin >> bet;
    if (bet > balance) {
      std::cout << "Error: not enough chips" << std::endl;
      std::cout << "You have " << balance << " chips." << std::endl;
    }
    if (std::cin.fail()) {
      std::cout << "Error: Invalid input." << std::endl;
    }
  }
  // If bet goes through, we play
  if (bet > 0 && bet <= balance) {
    std::cout << "Your bet: " << bet << " chips." << std::endl;
    balance = balance - bet; // update balance
    std::cout << "Your balance: " << balance << " chips." << std::endl;
    char command = '0'; 
    while (command != 'p'  && command != 'P' && command != 'b' && command != 'B' && command != 't' && command != 'T') {
      std::cin.clear();
      std::cout << "Bet on Player or Banker or Tie? (P/B/T)" << std::endl; 
      std::cin >> command; 
      // for now don't consider tie 
      if ((command == 'p') || (command == 'P')) {
        betP = true;
      } else if (command == 'b' || command == 'B') {
        betB = true;
      } else if (command == 't' || command == 'T') {
        betT = true;
      } else {
        std::cout << "Invalid command. Re-enter." << std::endl;
      }
    }

    // deal
    int outcome = dealGame(); 
    userWin = checkUserWin(outcome); 
    if (outcome == BANKER_WIN) {
      std::cout << "Banker Wins!" << std::endl;
    } else if (outcome == PLAYER_WIN) {
      std::cout << "Player Wins!" << std::endl;
    } else {
      std::cout << "Tie!" << std::endl;
    }
    updateBalance(outcome, balance, bet, userWin);
    std::cout << "Your balance: " << balance << " chips." << std::endl;
    reset();
  }
}

void simulate() {
  // the strategy is to bet 2 on player first. If loss, bet 2 on banker. If win, bet 1 on player again.
  // same goes if wins on banker. And the cycle continues until the shoe is over. 
  simulating = true; 
  double startingBal = 0;
  while (startingBal == 0 || std::cin.fail()) {
    // get starting value 
    std::cin.clear();
    std::cout << "Enter the balance to start with: " << std::endl;
    std::cout << "> ";
    std::cin >> startingBal;
    if (std::cin.fail() || startingBal <= 0) {
      std::cout << "Error: Invalid Value." << std::endl;
      std::cin.clear();
    }
  }
  std::cout << "Simulating with starting balance: " << startingBal << std::endl; 
  int numWins = 0;
  int numTies = 0;
  int numLoss = 0; 

  // int numSims; // number of simulations 
  // int curSim = 0; 
  double bal = startingBal; 
  double bet = 2; 
  bool betPlayer = true; // we first bet player
  while (!checkShoe())  {
    bal -= bet;
    if (betPlayer)  {
        betP = true;
    } else 
      betB = true; 

    int outcome = dealGame(); 
    userWin = checkUserWin(outcome); 
    updateBalance(outcome, bal, bet, userWin);
    if (outcome == TIE_WIN) {
      numTies++;
    } else if (userWin) {
      numWins++;
      bet = 1;
    } else {
      numLoss++;
      bet = 2; 
      betPlayer = !betPlayer; 
    }
    reset();
  }
  std::cout << "At end of simulation:" << std::endl; 
  std::cout << "Wins: " << numWins << ". Ties: " << numTies << ". Loss: " << numLoss << std::endl;
  std::cout << "Balance at end : " << bal << " . Profit: " << bal - startingBal; 
  
  // reshuffle deck and return 
  shuffleDeck(); 
  simulating = false; 
}
