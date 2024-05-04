#ifndef ENTITYSPAWNER_H
#define ENTITYSPAWNER_H
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
#include "Algebra.h"

class EntitySpawner
{
  public:
    EntitySpawner(){}
    virtual ~EntitySpawner(){free();}
    bool loadSettings(Json::Value& input, int maxX, int maxY);
    void free(){}
    void addCarnivores(vector<shared_ptr<Carnivore>>& entities, vector<shared_ptr<Carnivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map);
    void addHerbivores(vector<shared_ptr<Herbivore>>& entities, vector<shared_ptr<Herbivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map);
    void addPlants(vector<shared_ptr<Plant>>& entities, vector<vector<shared_ptr<Entity>>>& map);
    void addWater(vector<shared_ptr<Water>>& entities, vector<vector<shared_ptr<Entity>>>& map);
    void generatePointCloud();
    shared_ptr<EntitySpawner> deepCopy();
    Json::Value getJson();
    int entityCount() {return _entityCount;}
    EntityType entityType() {return _entityType;};
    int size() {return _pointCloud.size();}

  protected:
  private:
    EntityType _entityType = EntityType::PLANT;
    SpawnerType _spawnerType = SpawnerType::EVEN_DISTROBUTION;
    vector<shared_ptr<alg::Shape2D>> _shapes;
    alg::PointCloud _pointCloud;
    int _entityCount = 10;
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

#endif // ENTITYSPAWNER_H
