#ifndef ENTITIES_H
#define ENTITIES_H
#include "Constants.h"
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
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
    Animal(){if(DEBUG)cout << "Animal(): " << this << endl; init();}
    Animal(int x, int y, Animal* rawModel);
    Animal(const Animal &other);
    virtual ~Animal(){if(DEBUG)cout << "~Animal(): " << this << endl; if(DEBUG)cout << "delete [] _factors: " << (int*)_factors << endl; delete [] _factors;}
    AnimalAction action() {return _action;}
    void decideAction(uint8_t* input);
    void addEnergy(int newEnergy);
    bool removeEnergy(int lostEnergy);
    uint8_t* factors() {return _factors;}
    int energy() {return _energy;}

  protected:
    void init(Animal* rawModel = nullptr);
    AnimalAction _action;
    uint8_t* _factors = nullptr;
    int _energy = START_ENERGY;

  private:
};

class Carnivore: public Animal
{
  public:
    Carnivore(){_entityType = EntityType::CARNIVORE;if(DEBUG)cout << "Carnivore(): " << this << endl;}
    Carnivore(int x, int y, Carnivore* rawModel);
    virtual ~Carnivore(){if(DEBUG)cout << "~Carnivore(): " << this << endl;}

  protected:

  private:
};

class Herbivore: public Animal
{
  public:
    Herbivore(){_entityType = EntityType::HERBIVORE; if(DEBUG)cout << "Herbivore(): " << this << endl;}
    Herbivore(int x, int y, Herbivore* rawModel);
    virtual ~Herbivore(){if(DEBUG)cout << "~Herbivore(): " << this << endl;}

  protected:

  private:
};

class Plant: public Entity
{
  public:
    Plant(){_entityType = EntityType::PLANT; if(DEBUG)cout << "Plant(): " << this << endl;}
    Plant(int x, int y) : Entity(x, y){_entityType = EntityType::PLANT; if(DEBUG)cout << "Plant(int x, int y): " << this << endl;}
    virtual ~Plant(){if(DEBUG)cout << "~Plant(): " << this << endl;}

  protected:

  private:
};

#endif // ENTITIES_H
