#ifndef MAP_H
#define MAP_H
#include "common.h"
#include <iostream>
#include "Graphics/SDLWindow.h"
#include "Entities.h"
#include <fstream>
#include <json/json.h>
#include <list>
#include <algorithm>
#include <memory>
#include <ctime>
#include "EntitySpawner.h"

class Map
{
  public:
    Map();
    virtual ~Map();
    bool resetMap();          //Clears map and sets it up with new entities. If bestAnimals exist they will be used when creating new ones
    bool loadMapSettings(string fileName);
    bool loadMapSettings(Json::Value& json);
    bool tick();              //If generation is done the map is reset. Then progress time one tick. Returns true when generation is done.
    bool validConfig() {return _validConfig;}
    bool generationDone() {return _generationDone;}
    void draw(MapWindow& window);
    void zoomIn() {_zoom++; if(_zoom > MAX_ZOOM) _zoom = MAX_ZOOM;}
    void zoomOut() {_zoom--; if(_zoom < 1) _zoom = 1;}
    Json::Value getJson();
    const vector<shared_ptr<Carnivore>>& carnivores(){return _carnivores;}
    const vector<shared_ptr<Herbivore>>& herbivores(){return _herbivores;}
    const vector<shared_ptr<Carnivore>>& bestCarnivore(){return _bestCarnivores;}
    const vector<shared_ptr<Herbivore>>& bestHerbivore(){return _bestHerbivores;}
    shared_ptr<Entity> getSelectedEntity();
    shared_ptr<Map> deepCopy();
    void output(string tab, OutputLevel level);
    void name(string n) {_name = n;}
    int tickCount() {return _tickCount;}
    int zoom() {return _zoom;}
    int sizeX() {return _sizeX;}
    int sizeY() {return _sizeY;}
    int generationCount() {return _generationCount;}
    int selectedEntityType = SelectableEntityType::SEL_CARNIVORE;
    int selectedEntityId = 0;
    bool pause=false;
    bool oneStep=false;

  protected:

  private:
    void free();                //Frees all memory
    void clearMap();            //Removes all entities on map, and the map (keeps best animals)
    void clearBestAnimals();    //Removes best animals
    shared_ptr<uint8_t[]> inputFromArea(int posX, int posY, int sensorRadius);
    void inputFromSquare(int posX, int posY, shared_ptr<uint8_t[]> input, int inputPos);
    void decideAcitons();
    void performActions();
    void removeDeadEntities();
    void saveBestCarnivores();
    void saveBestHerbivores();
    string _name;
    bool _validConfig = false;
    bool _mapSetUp = false;
    bool _generationDone = false;
    int _sizeX = 200;
    int _sizeY = 200;
    int _tickCount;
    int _zoom = DEFAULT_ZOOM;
    int _generationCount = 0;
    int _resetHerbivoreCount = 10;
    int _resetCarnivoreCount = 5;
    shared_ptr<Plant> _plantPrototype;
    shared_ptr<Water> _waterPrototype;
    vector<vector<shared_ptr<Entity>>> _map;
    vector<shared_ptr<Carnivore>> _carnivores;
    vector<shared_ptr<Herbivore>> _herbivores;
    vector<shared_ptr<Carnivore>> _bestCarnivores;
    vector<shared_ptr<Herbivore>> _bestHerbivores;
    vector<shared_ptr<EntitySpawner>> _entitySpawners;
};

#endif // MAP_H
