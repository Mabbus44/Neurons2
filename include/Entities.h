#ifndef ENTITIES_H
#define ENTITIES_H
#include "Constants.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <memory>
#include "json/json.h"
using namespace std;

class Entity
{
  public:
    Entity();
    Entity(int x, int y){_posX = x; _posY = y;}
    virtual ~Entity();
    EntityType entityType() {return _entityType;}
    bool alive() {return _alive;}
    void load(const Json::Value& json);
    virtual Json::Value getJson(Json::Value ret = Json::Value());
    int posX() {return _posX;}
    int posY() {return _posY;}
    void posX(int posX) {_posX = posX;}
    void posY(int posY) {_posY = posY;}
    void kill(vector<vector<shared_ptr<Entity>>>& map) {_alive = false; map[_posX][_posY].reset();}
    shared_ptr<Entity> deepCopy(shared_ptr<Entity> ret = make_shared<Entity>());

  protected:

    int _posX, _posY;
    EntityType _entityType;
    bool _alive = true;
  private:
};

class Animal: public Entity
{
  public:
    Animal(){}
    Animal(int x, int y, shared_ptr<Animal> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
           int energyCostNothing,int energyGainEat, int eatDist, int maxMutation);
    Animal(const Animal &other);
    virtual ~Animal(){}
    AnimalAction action() {return _action;}
    void decideAction(shared_ptr<uint8_t[]> input);
    void addEnergy(int newEnergy);
    void performAction(vector<vector<shared_ptr<Entity>>>& map);
    bool removeEnergy(int lostEnergy, vector<vector<shared_ptr<Entity>>>& map);
    void load(const Json::Value& json);
    virtual Json::Value getJson(Json::Value ret = Json::Value());
    shared_ptr<uint8_t[]> factors() {return _factors;}
    int energy() {return _energy;}
    int sensorRadius() {return _sensorRadius;}
    shared_ptr<Animal> deepCopy(shared_ptr<Animal> ret = make_shared<Animal>());

  protected:
    void init(shared_ptr<Animal> rawModel = nullptr);
    void initInputAndFactorSize();
    void tryEat(vector<vector<shared_ptr<Entity>>>& map);
    void movePos(vector<vector<shared_ptr<Entity>>>& map, AnimalAction moveAction);
    shared_ptr<Entity> findEntity(vector<vector<shared_ptr<Entity>>>& map, EntityType entityType);
    AnimalAction _action;
    shared_ptr<uint8_t[]> _factors;
    int _sensorRadius;
    int _energy;
    int _maxEnergy;
    int _energyCostMove;
    int _energyCostEat;
    int _energyCostNothing;
    int _energyGainEat;
    int _eatDist;
    int _maxMutation;
    int _inputSize;
    int _factorSize;

  private:
};

class Carnivore: public Animal
{
  public:
    Carnivore(){_entityType = EntityType::CARNIVORE;}
    Carnivore(int x, int y, shared_ptr<Carnivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
           int energyCostNothing,int energyGainEat, int eatDist, int maxMutation);
    virtual ~Carnivore(){}
    shared_ptr<Carnivore> deepCopy();

  protected:

  private:
};

class Herbivore: public Animal
{
  public:
    Herbivore(){_entityType = EntityType::HERBIVORE;}
    Herbivore(int x, int y, shared_ptr<Herbivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
           int energyCostNothing,int energyGainEat, int eatDist, int maxMutation);
    virtual ~Herbivore(){}
    shared_ptr<Herbivore> deepCopy();

  protected:

  private:
};

class Plant: public Entity
{
  public:
    Plant(){_entityType = EntityType::PLANT;}
    Plant(int x, int y) : Entity(x, y){_entityType = EntityType::PLANT;}
    virtual ~Plant(){}
    shared_ptr<Plant> deepCopy();

  protected:

  private:
};

#endif // ENTITIES_H
