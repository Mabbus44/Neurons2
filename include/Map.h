#ifndef MAP_H
#define MAP_H
#include <iostream>
#include "Graphics/SDLWindow.h"
#include "Entities.h"
#include <fstream>
#include <json/json.h>
#include <list>
#include <algorithm>
#include <memory>

class EntitySpawner
{
  public:
    EntitySpawner(){}
    virtual ~EntitySpawner(){free();}
    bool loadSettings(Json::Value& input, int mapSizeX, int mapSizeY);
    void free(){}
    void addCarnivores(vector<shared_ptr<Carnivore>>& entities, vector<shared_ptr<Carnivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map);
    void addHerbivores(vector<shared_ptr<Herbivore>>& entities, vector<shared_ptr<Herbivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map);
    void addPlants(vector<shared_ptr<Plant>>& entities, vector<vector<shared_ptr<Entity>>>& map);
    shared_ptr<EntitySpawner> deepCopy();
    Json::Value getJson();
    int entityCount() {return _entityCount;}
    EntityType entityType() {return _entityType;};
    int minX() {return _minX;}
    int minY() {return _minY;}
    int maxX() {return _maxX;}
    int maxY() {return _maxY;}
    int sizeX() {return _maxX-_minX+1;}
    int sizeY() {return _maxY-_minY+1;}

  protected:
  private:
    EntityType _entityType = EntityType::PLANT;
    SpawnerType _spawnerType = SpawnerType::EVEN_DISTROBUTION;
    int _entityCount = 10;
    int _minX = 0;
    int _minY = 0;
    int _maxX = -1;
    int _maxY = -1;
    int _sensorRadius = 5;
    int _maxEnergy = 5000;
    int _startEnergy = 1000;
    int _energyCostMove = 20;
    int _energyCostEat = 10;
    int _energyCostNothing = 5;
    int _energyGainEat = 4000;
    int _eatDist = 5;
    int _maxMutation = 10;
};

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
    const vector<shared_ptr<Plant>>& plants(){return _plants;}
    const vector<shared_ptr<Carnivore>>& bestCarnivore(){return _bestCarnivores;}
    const vector<shared_ptr<Herbivore>>& bestHerbivore(){return _bestHerbivores;}
    shared_ptr<Entity> getSelectedEntity();
    shared_ptr<Map> deepCopy();
    int tickCount() {return _tickCount;}
    int zoom() {return _zoom;}
    int sizeX() {return _sizeX;}
    int sizeY() {return _sizeY;}
    int generationCount() {return _generationCount;}
    void output(string tab, OutputLevel level);
    int selectedEntityType = SelectableEntityType::SEL_CARNIVORE;
    int selectedEntityId = 0;
    bool pause=false;
    bool oneStep=false;

  protected:

  private:
    void free();                //Frees all memory
    void clearMap();            //Removes all entities on map, and the map (keeps best animals)
    void clearBestAnimals();    //Removes best animals
    void populateMap();         //Create map and populate it with entities from entity vectors
    shared_ptr<uint8_t[]> inputFromArea(int posX, int posY, int sensorRadius);
    void inputFromSquare(int posX, int posY, shared_ptr<uint8_t[]> input, int inputPos);
    void decideAcitons();
    void performActions();
    void removeDeadEntities();
    void saveBestCarnivores();
    void saveBestHerbivores();
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
    vector<vector<shared_ptr<Entity>>> _map;
    vector<shared_ptr<Carnivore>> _carnivores;
    vector<shared_ptr<Herbivore>> _herbivores;
    vector<shared_ptr<Plant>> _plants;
    vector<shared_ptr<Carnivore>> _bestCarnivores;
    vector<shared_ptr<Herbivore>> _bestHerbivores;
    vector<shared_ptr<EntitySpawner>> _entitySpawners;
};

#endif // MAP_H
