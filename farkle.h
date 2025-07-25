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

    void roll(std::default_random_engine& gen, std::uniform_int_distribution<int>& dist) {
      value = dist(gen);
    }

  private:
    bool inShaker = true;
    int value;
};

class DiceShaker {
  public:
  DiceShaker() : shaker(6) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed); // Initialize the member generator
  }

  int getSize() {
    return 6;
  }

  bool checkIfEmpty() {
    for(auto &d : shaker) {
      if(d.getInShaker()) {
        return false;
      }
    }
    return true;
  }

  void resetShaker() {
        shaker.clear(); // Removes all current Dice objects from the vector
        shaker.reserve(6); // Optional: Pre-allocate memory for 6 dice to avoid reallocations
        for (int i = 0; i < 6; ++i) {
            shaker.push_back(Dice()); // Add 6 new, default-constructed Dice (inShaker=true)
        }
  }

  void updateShaker() {
    shaker.erase(std::remove_if(shaker.begin(), shaker.end(),
                          [](Dice& d) {
                              return !d.getInShaker(); // Remove if NOT in shaker
                          }),
            shaker.end());
  }

  std::vector<Dice> &shakeAndRoll() {
    std::cout << "\nRoll: \n";
    for(size_t i = 0; i < shaker.size(); i++) {
      if(shaker[i].getInShaker()) {
        shaker[i].roll(generator, distribution);
        // print value of die
        std::cout << shaker[i].getValue() << " ";
      }
    }
    std::cout << "\n\n";
    return shaker;
  }
  private:
  std::vector<Dice> shaker;

  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution{1, 6};
};

class Game {
  public:
  struct Player {
    int points;
    bool winner;
    std::string name;
  };

  std::vector<Dice> &rollDice() {
    if(diceShaker.checkIfEmpty()) {
      diceShaker.resetShaker();
    }
    return diceShaker.shakeAndRoll();
  }

  int select(std::vector<Dice> &shake) {
    int pointTally = 0;
    // of a kinds
    std::unordered_map<int, std::vector<Dice*>> diceGroups;
    for(auto &die : shake) {
      diceGroups[die.getValue()].push_back(&die);
    }
    
    for(auto &p : diceGroups) {
      if(p.second.size() == 6) { // 6 of a kind
        pointTally += 3000;
        for(auto &d : p.second) {
          d->setInShaker(false);
        }
      } else if(p.second.size() == 5) { // 5 of a kind
        pointTally += 2000;
        for(auto &d : p.second) {
          d->setInShaker(false);
        }
      } else if(p.second.size() == 4) { // 4 of a kind AND 4 + pair
        pointTally += 1000;
        for(auto &d : p.second) {
          d->setInShaker(false);
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
    //TODO fix this because it counts for 333441
    if(diceGroups.size() == 3 && shake.size() == 6) {
      bool falseAlarm  = false;
      for(auto &p : diceGroups) {
        if(p.second.size() != 2) {
          falseAlarm = true;
        }
      }
      if(!falseAlarm) {
        pointTally += 1500;
        for(auto &d : shake) {
          d.setInShaker(false);
        }
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
          if(!d->getInShaker()) {
            overlap = true;
          }
        }
        if(!overlap) {
          pointTally += p.first * 100;
          for(auto &d : p.second) {
            d->setInShaker(false);
          }
        }
      }
    }
    // 1s and 5s
    // make sure no overlap with previous
    if(diceGroups[1].size() != 0) {
      for(auto &d : diceGroups[1]) {
        if(d->getInShaker()) {
          pointTally += 100;
        }
      }
    }
    if(diceGroups[5].size() != 0) {
      for(auto &d : diceGroups[5]) {
        if(d->getInShaker()) {
          pointTally += 50;
        }
      }
    }

    for(auto &d : diceGroups[1]) {
      d->setInShaker(false);
    }
    for(auto &d : diceGroups[5]) {
      d->setInShaker(false);
    }

    // update shaker
    diceShaker.updateShaker();
    
    return pointTally;
  }

  void turn(Player &player) {
    std::cout << player.name << "\'s turn!\n";
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
    int pointsTally = 0;
    while(rollAgain) {
      std::vector<Dice> &roll = rollDice();
      int selection = select(roll);
      pointsTally += selection;
      if(selection == 0) {
        // roll farkled
        std::cout << "* * * * FARKLE! * * * *\n";
        std::cout << "      FARKLE!\n";
        std::cout << "* * * * FARKLE! * * * *\n\n";
        std::cout << "Better luck next time!\n\n";
        pointsTally = 0;
        break;
      }
      char choice;
      std::cout << selection << " points have been added to your tally (for now..)\n";
      std::cout << "Your total for this turn so far is: " << pointsTally << "\n";
      std::cout << "Would you like to roll again or bank those points? (R/B): ";
      std::cin >> choice;
      choice = std::tolower(choice);
      while(choice != 'r' && choice != 'b') {
        std::cout << "Please enter \"R\" or \"B\": ";
        std::cin >> choice;
        choice = std::tolower(choice);
      }
      if(choice == 'b') {
        rollAgain = false;
        std::cout << "\nWell done " << player.name << " , " << pointsTally << " points have been added to your score!\n\n";
      }
    }
    player.points += pointsTally;
    diceShaker.resetShaker();
    std::cout << "--------------------\n";
    std::cout << "CURRENT SCOREBOARD: \n";
    for(auto &p : players) {
      std::cout << p.name << ": " << p.points << "\n";
    }
    std::cout << "--------------------\n\n";
  }

  void getPlayers() {
    std::cout << "Welcome to FARKLE!" << "\n" << std::endl;
    std::string name = "q";
    std::vector<std::string> playerNames;
    while(name == "Q" || name == "q") {
      std::cout << "Please enter the first player's name (name unfortunately can not be \"Q\"): ";
      std::cin >> name;
    }
    playerNames.push_back(name);

    while(name != "q" && name != "Q") {
      std::cout << "Please enter a player name (enter \"Q\" if all players have been entered): ";
      std::cin >> name;
      playerNames.push_back(name);
    }

    for(size_t i = 0; i < playerNames.size() - 1; i++) {
      Player p = {0, false, playerNames[i]};
      players.push_back(p);
    }
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
    std::cout << "FINAL SCOREBOARD:\n";
    for(auto &p : players) {
      std::cout << p.name << ": " << p.points << "\n";
    }
  }

  private:
  std::vector<Player> players;
  DiceShaker diceShaker;
  bool gameOver = false;
};