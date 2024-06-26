#ifndef HUB_H
#define HUB_H

#include "common.h"
#include "Graphics/ConsoleHandler.h"
#include <thread>
#include <future>
#include <memory>

class SimulationJob{
  public:
    SimulationJob(){};
    SimulationJob(const SimulationJob& other){simulationType=other.simulationType; stepsLeft=other.stepsLeft, m=other.m;}
    SimulationJob(int simulationType, int stepsLeft, shared_ptr<Map> m){this->simulationType=simulationType; this->stepsLeft=stepsLeft; this->m=m;}
    ~SimulationJob(){};
    int simulationType;
    int stepsLeft;
    shared_ptr<Map> m;
};

class Hub
{
  public:
    Hub();
    virtual ~Hub();
    void run();

  protected:

  private:
    void createMap(vector<string> args);
    void copyMap(vector<int> args);
    void deleteMap(vector<int> args);
    void saveMapState(vector<string> args);
    void loadMapState(vector<string> args);
    void viewMap(vector<int> args);
    void runSimulationSteps(vector<int> args);
    void runSimulationGenerations(vector<int> args);
    void outputSimulationStatus(vector<int> args);
    void outputMapsOneline(vector<int> args);
    void renameMap(vector<string> args);
    void parseConsoleInstructions();
    void handleSdlEvents();
    void handleWindowEvents(SDL_Event& e);
    void handleKeyEvents(SDL_Event& e);
    void runSimulations();
    shared_ptr<Map> map(int& i);
    MapWindow _mapWindow;
    NeuronsWindow _neuronWindow;
    ConsoleHandler _console;
    vector<shared_ptr<Map>> _maps;
    vector<SimulationJob> _jobs;
    bool _quit = false;
};

#endif // HUB_H
