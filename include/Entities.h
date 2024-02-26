#ifndef ENTITIES_H
#define ENTITIES_H
#include "Constants.h"
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <memory>
using namespace std;

class Entity
{
  public:
    Entity();
    Entity(int x, int y){_posX = x; _posY = y;}
    virtual ~Entity();
    EntityType entityType() {return _entityType;}
    bool alive() {return _alive;}
    int posX() {return _posX;}
    int posY() {return _posY;}
    void posX(int posX) {_posX = posX;}
    void posY(int posY) {_posY = posY;}
    void kill() {_alive = false;}

  protected:

    int _posX, _posY;
    EntityType _entityType;
    bool _alive = true;
  private:
};

class Animal: public Entity
{
  public:
    Animal(){init();}
    Animal(int x, int y, shared_ptr<Animal> rawModel);
    Animal(const Animal &other);
    virtual ~Animal(){}
    AnimalAction action() {return _action;}
    void decideAction(shared_ptr<uint8_t[]> input);
    void addEnergy(int newEnergy);
    bool removeEnergy(int lostEnergy);
    shared_ptr<uint8_t[]> factors() {return _factors;}
    int energy() {return _energy;}

  protected:
    void init(shared_ptr<Animal> rawModel = nullptr);
    AnimalAction _action;
    shared_ptr<uint8_t[]> _factors;
    int _energy = START_ENERGY;

  private:
};

class Carnivore: public Animal
{
  public:
    Carnivore(){_entityType = EntityType::CARNIVORE;}
    Carnivore(int x, int y, shared_ptr<Carnivore> rawModel);
    virtual ~Carnivore(){}

  protected:

  private:
};

class Herbivore: public Animal
{
  public:
    Herbivore(){_entityType = EntityType::HERBIVORE;}
    Herbivore(int x, int y, shared_ptr<Herbivore> rawModel);
    virtual ~Herbivore(){}

  protected:

  private:
};

class Plant: public Entity
{
  public:
    Plant(){_entityType = EntityType::PLANT;}
    Plant(int x, int y) : Entity(x, y){_entityType = EntityType::PLANT;}
    virtual ~Plant(){}

  protected:

  private:
};

#endif // ENTITIES_H
