#include <random>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <algorithm>

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
  };

  void select(std::vector<Dice> &shake) {
    int pointTally = 0;
    // of a kinds
    std::unordered_map<int, std::vector<Dice>> diceGroups;
    for(auto &die : shake) {
      diceGroups[die.getValue()].push_back(die);
    }
    
    for(auto &p : diceGroups) {
      if(p.second.size() == 6) {
        pointTally += 3000;

      } else if(p.second.size() == 5) {
        pointTally += 2000;
        for(auto &d : p.second) {
          d.setInShaker(false);
        }
      } else if(p.second.size() == 4) {
        pointTally += 1000;
        for(auto &d : p.second) {
          d.setInShaker(false);
        }
      }
    }
    // straight
    if(diceGroups.size() == 6) {
      pointTally += 1500;
    }
    // 3 pairs

    // 4 + pair

    // 2 triplets

    // 3 of a kinds

    // 1s and 5s

    // update shaker
    shake.erase(std::remove_if(shake.begin(), shake.end(),
                          [](Dice& d) {
                              return !d.getInShaker(); // Remove if NOT in shaker
                          }),
            shake.end());
  }

  void turn() {
    // does one turn for a single player
    // roll until no available points or player banks
    // make sure to reset shaker as needed
  }

  void play() {
    // plays a full game
    // while true
    // for all players in players, do a turn (turn will exit if player reaches 10k)
    // print results
    // return
  }

  private:
  std::vector<Player> players;
  DiceShaker diceShaker;
};