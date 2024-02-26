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

using namespace Json;

class EntitySpawner
{
  public:
    EntitySpawner(){}
    virtual ~EntitySpawner(){free();}
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
    void draw(MapWindow& window);
    void zoomIn() {_zoom++; if(_zoom > MAX_ZOOM) _zoom = MAX_ZOOM;}
    void zoomOut() {_zoom--; if(_zoom < 1) _zoom = 1;}
    bool validConfig() {return _validConfig;}
    bool generationDone() {return _generationDone;}
    const vector<shared_ptr<Carnivore>>& carnivores(){return _carnivores;}
    const vector<shared_ptr<Herbivore>>& herbivores(){return _herbivores;}
    const vector<shared_ptr<Plant>>& plants(){return _plants;}
    const vector<shared_ptr<Carnivore>>& bestCarnivore(){return _bestCarnivores;}
    const vector<shared_ptr<Herbivore>>& bestHerbivore(){return _bestHerbivores;}
    shared_ptr<Entity> getSelectedEntity();
    int tickCount() {return _tickCount;}
    int zoom() {return _zoom;}
    int sizeX() {return _sizeX;}
    int sizeY() {return _sizeY;}
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
    shared_ptr<uint8_t[]> inputFromArea(int posX, int posY);
    void inputFromSquare(int posX, int posY, shared_ptr<uint8_t[]> input, int inputPos);
    void decideAcitons();
    void performActions();
    void performAction(shared_ptr<Animal> animal);
    void removeDeadEntities();
    void eat(shared_ptr<Animal> animal);
    void kill(shared_ptr<Entity> animal);
    void animalMove(shared_ptr<Animal> animal, AnimalAction moveAction);
    void saveBestCarnivores();
    void saveBestHerbivores();
    shared_ptr<Entity> findEntity(int posX, int posY, int radius, EntityType entityType);
    bool _validConfig = false;
    bool _mapSetUp = false;
    bool _generationDone = false;
    int _sizeX, _sizeY;
    int _tickCount;
    int _zoom = DEFAULT_ZOOM;
    vector<vector<shared_ptr<Entity>>> _map;
    vector<shared_ptr<Carnivore>> _carnivores;
    vector<shared_ptr<Herbivore>> _herbivores;
    vector<shared_ptr<Plant>> _plants;
    vector<shared_ptr<Carnivore>> _bestCarnivores;
    vector<shared_ptr<Herbivore>> _bestHerbivores;
    list<shared_ptr<EntitySpawner>> _entitySpawners;
};

#endif // MAP_H
