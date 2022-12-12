#include <queue>

class Monkey {
  private:
    Monkey* m_falseMonkey;
    Monkey* m_trueMonkey;

  public:
    std::queue<uint64_t> items;
    char operation;
    int operationRhs, testValue;
    int inspectCount = 0;
    int commonModulo;

    Monkey();
    Monkey(std::queue<uint64_t> item, char operation, int operationRhs, int testValue, int commonModulo);

  void addItem (uint64_t item);
  void runRound (int partNum);
  void setMonkeys(Monkey *trueMonkey, Monkey *falseMonkey);
  const void print ();
};