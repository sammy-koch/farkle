#include <random>
#include <chrono>
#include <vector>

class Dice {
  public:
    bool getInShaker() {
      return inShaker;
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

  std::vector<Dice> shakeAndRoll() {
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

class Player {
  public:

  private:
    int points;
    bool winner = false;
};

class Game {
  public:

  void turn() {
    // does one turn for a single player
    // roll until no available points or player banks
    //
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
  DiceShaker *diceShaker;
};