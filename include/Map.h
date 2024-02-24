#ifndef MAP_H
#define MAP_H
#include <iostream>
#include "Graphics/SDLWindow.h"
#include "Entities.h"
#include <fstream>
#include <json/json.h>
#include <list>
#include <algorithm>

using namespace Json;

class EntitySpawner
{
  public:
    EntitySpawner(){cout << "New EntitySpawned: " << this << endl;}
    virtual ~EntitySpawner(){free(); cout << "Deleted EntitySpawned: " << this << endl;}
    bool loadSettings(Value input);
    void free(){}
    int entityCount() {return _entityCount;}
    EntityType entityType() {return _entityType;};
    int minX() {return _minX;}
    int minY() {return _minY;}
    int maxX(int mapMax);
    int maxY(int mapMax);
    int sizeX(int mapMax);
    int sizeY(int mapMax);

  protected:
  private:
    EntityType _entityType;
    SpawnerType _spawnerType;
    int _entityCount;
    int _minX, _minY, _maxX, _maxY;
};

class Map
{
  public:
    Map();
    virtual ~Map();
    bool resetMap();          //Clears map and sets it up with new entities. If bestAnimals exist they will be used when creating new ones
    bool loadMapSettings(string fileName);
    bool tick();              //If generation is done the map is reset. Then progress time one tick. Returns true when generation is done.
    void draw(MapWindow* window);
    bool validConfig() {return _validConfig;}
    bool generationDone() {return _generationDone;}
    const vector<Carnivore*>& carnivores(){return _carnivores;}
    const vector<Herbivore*>& herbivores(){return _herbivores;}
    const vector<Plant*>& plants(){return _plants;}
    const vector<Carnivore*>& bestCarnivore(){return _bestCarnivores;}
    const vector<Herbivore*>& bestHerbivore(){return _bestHerbivores;}
    Entity* getSelectedEntity();
    int tickCount() {return _tickCount;}
    bool pause=false;
    bool oneStep=false;
    void output(string tab, OutputLevel level);
    int selectedEntityType = SelectableEntityType::SEL_CARNIVORE;
    int selectedEntityId = 0;

  protected:

  private:
    void free();                //Frees all memory
    void clearMap();            //Removes all entities on map, and the map (keeps best animals)
    void clearBestAnimals();    //Removes best animals
    uint8_t* inputFromArea(int posX, int posY);
    void inputFromSquare(int posX, int posY, uint8_t* input, int inputPos);
    void decideAcitons();
    void performActions();
    void performAction(Animal* animal);
    void removeDeadEntities();
    void eat(Animal* animal);
    void kill(Entity* animal);
    void animalMove(Animal* animal, AnimalAction moveAction);
    void saveBestCarnivores();
    void saveBestHerbivores();
    Entity* findEntity(int posX, int posY, int radius, EntityType entityType);
    bool _validConfig = false;
    bool _mapSetUp = false;
    bool _generationDone = false;
    int _sizeX, _sizeY;
    int _tickCount;
    vector<vector<Entity*>> _map;
    vector<Carnivore*> _carnivores;
    vector<Herbivore*> _herbivores;
    vector<Plant*> _plants;
    vector<Carnivore*> _bestCarnivores;
    vector<Herbivore*> _bestHerbivores;
    list<EntitySpawner*> _entitySpawners;
};

#endif // MAP_H
