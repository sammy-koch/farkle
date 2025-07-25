#include <random>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <string>

class Dice {
  public:
    bool getInShaker() {
      return inShaker;
    }

    void setInShaker(bool set) {
      inShaker = set;
    }

    int getValue() {
      return value;
    }

    void roll() {
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::default_random_engine generator(seed);
      std::uniform_int_distribution<int> distribution(1, 6);
      value = distribution(generator);
    }

  private:
    bool inShaker = true;
    int value;
};

class DiceShaker {
  public:
  DiceShaker() : shaker(6) {}

  int getSize() {
    return 6;
  }

  void resetShaker() {
        shaker.clear(); // Removes all current Dice objects from the vector
        shaker.reserve(6); // Optional: Pre-allocate memory for 6 dice to avoid reallocations
        for (int i = 0; i < 6; ++i) {
            shaker.push_back(Dice()); // Add 6 new, default-constructed Dice (inShaker=true)
        }
    }

  std::vector<Dice> &shakeAndRoll() {
    for(int i = 0; i < shaker.size(); i++) {
      if(shaker[i].getInShaker()) {
        shaker[i].roll();
        // TODO: print value of die
      }
    }
  }
  private:
  std::vector<Dice> shaker;
};

class Game {
  public:
  struct Player {
    int points;
    bool winner;
    std::string name;
  };

  std::vector<Dice> rollDice() {
    diceShaker.shakeAndRoll();
  }

  int select(std::vector<Dice> &shake) {
    int pointTally = 0;
    // of a kinds
    std::unordered_map<int, std::vector<Dice>> diceGroups;
    for(auto &die : shake) {
      diceGroups[die.getValue()].push_back(die);
    }
    
    for(auto &p : diceGroups) {
      if(p.second.size() == 6) { // 6 of a kind
        pointTally += 3000;
        for(auto &d : p.second) {
          d.setInShaker(false);
        }
      } else if(p.second.size() == 5) { // 5 of a kind
        pointTally += 2000;
        for(auto &d : p.second) {
          d.setInShaker(false);
        }
      } else if(p.second.size() == 4) { // 4 of a kind AND 4 + pair
        pointTally += 1000;
        for(auto &d : p.second) {
          d.setInShaker(false);
        }
        if(diceGroups.size() == 2 && shake.size() == 6) {
          pointTally += 500;
          for(auto &d : shake) {
            d.setInShaker(false);
          }
        }
      }
    }
    // straight
    if(diceGroups.size() == 6) {
      pointTally += 1500;
      for(auto &d : shake) {
        d.setInShaker(false);
      }
    }
    // 3 pairs
    if(diceGroups.size() == 3 && shake.size() == 6) {
      pointTally += 1500;
      for(auto &d : shake) {
        d.setInShaker(false);
      }
    }
    // 2 triplets
    if(diceGroups.size() == 2 && shake.size() == 6) {
      pointTally += 2500;
      for(auto &d : shake) {
        d.setInShaker(false);
      }
    }

    // 3 of a kinds
    for(auto &p : diceGroups) {
      if(p.second.size() == 3) {
        bool overlap = false;
        for(auto &d : p.second) {
          if(!d.getInShaker()) {
            overlap = true;
          }
        }
        if(!overlap) {
          pointTally += p.first * 100;
          for(auto &d : p.second) {
            d.setInShaker(false);
          }
        }
      }
    }
    // 1s and 5s
    // TODO make sure no overlap with previous
    if(diceGroups[1].size() != 0) {
      for(auto &d : diceGroups[1]) {
        if(d.getInShaker()) {
          pointTally += 100;
        }
      }
    }
    if(diceGroups[5].size() != 0) {
      for(auto &d : diceGroups[5]) {
        if(d.getInShaker()) {
          pointTally += 50;
        }
      }
    }

    for(auto &d : diceGroups[1]) {
      d.setInShaker(false);
    }
    for(auto &d : diceGroups[5]) {
      d.setInShaker(false);
    }

    // update shaker
    shake.erase(std::remove_if(shake.begin(), shake.end(),
                          [](Dice& d) {
                              return !d.getInShaker(); // Remove if NOT in shaker
                          }),
            shake.end());
    
    return pointTally;
  }

  void turn(Player &player) {
    // does one turn for a single player
    // roll until no available points or player banks
    // make sure to reset shaker as needed

    // roll dice
    // select dice to keep
    // option to roll again or bank
    // if roll again, repeat process with remaining dice
    // if bank, store points tallied and return
    //
    // if all 6 dice rolled and selected, then reset shaker and repeat
    //   make sure to offer banking after 6 as well.
    bool rollAgain = true;
    std::vector<Dice> roll;
    int pointsTally = 0;
    while(rollAgain) {
      roll = rollDice();
      pointsTally += select(roll);
      if(select(roll) == 0) {
        // roll farkled
        pointsTally = 0;
        break;
      }
      char choice;
      std::cout << "Would you like to roll again or bank? (R/B): ";
      std::cin >> choice;
      choice = std::tolower(choice);
      while(choice != 'r' || choice != 'b') {
        std::cout << "Please enter \"R\" or \"B\": ";
        std::cin >> choice;
        choice = std::tolower(choice);
      }
      if(choice == 'b') {
        rollAgain = false;
      }
    }
    player.points += pointsTally;
  }

  void play() {
    while(!gameOver) {
      for(auto &p : players) {
        turn(p);
        if(p.points >= 10000) {
          gameOver = true;
          std::cout << p.name << " wins!" << std::endl;
          break;
        }
      }
    }

    // TODO print scoreboard
  }

  private:
  std::vector<Player> players;
  DiceShaker diceShaker;
  bool gameOver = false;
};