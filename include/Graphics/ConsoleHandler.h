#ifndef CONSOLEHANDLER_H
#define CONSOLEHANDLER_H

#include <iostream>
#include <vector>
#include "Map.h"

using namespace std;

class Instruction{
  public:
    Instruction(){};
    Instruction(const Instruction& other){inst=other.inst; args=other.args; strArgs=other.strArgs;}
    Instruction(int inst, vector<int> args={}){this->inst=inst; this->args=args;}
    Instruction(int inst, vector<string> args){this->inst=inst; this->args={}; this->strArgs = args;}
    ~Instruction(){};
    int inst;
    vector<int> args;
    vector<string> strArgs;
};

class ConsoleHandler
{
  public:
    ConsoleHandler();
    virtual ~ConsoleHandler();
    void run();
    vector<Instruction> instructions;
  private:
    void outputMapsOneline();
    vector<int> argsToInt(string args);
    vector<string> argsTostr(string args);
};

#endif // CONSOLEHANDLER_H
