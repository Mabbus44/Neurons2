#include "Entities.h"

Entity::Entity()
{
  //ctor
}

Entity::~Entity()
{
  //dtor
}

Animal::Animal(int x, int y, shared_ptr<Animal> rawModel) : Entity(x, y){
  init(rawModel);
}

void Animal::decideAction(shared_ptr<uint8_t[]> input){
  // As long as inputSize < 2^16 (65536) we wont get overflow if input and factor are 8 bit, and result 32 bit.
  int factorId = 0;
  int32_t highestActionWeight = -2147483648;
  _action = (AnimalAction)0;
  for(int actionId = 0; actionId < AnimalAction::COUNT; actionId++){
    int32_t actionWeight = 0;
    for(int inputId=0; inputId<INPUT_SIZE; inputId++)
      actionWeight += ((int32_t)(_factors[factorId++])-128) * (int32_t)(input[inputId]);
    if(actionWeight > highestActionWeight){
      highestActionWeight = actionWeight;
      _action = (AnimalAction)actionId;
    }
  }
}

void Animal::addEnergy(int newEnergy){
  _energy += newEnergy;
  if(_energy > MAX_ENERGY)
    _energy = MAX_ENERGY;
}

bool Animal::removeEnergy(int lostEnergy){
  _energy -= lostEnergy;
  if(_energy <= 0){
    _energy = 0;
    _alive = false;
  }
  return _alive;
}

void Animal::init(shared_ptr<Animal> rawModel){
  _factors = make_shared<uint8_t[]>(FACTOR_SIZE);
  if(!rawModel){
    for(int i=0; i<FACTOR_SIZE; i++)
      _factors[i] = rand() % 256;
  }else{
    for(int i=0; i<FACTOR_SIZE; i++){
      int newFactor = rawModel->_factors[i] + rand()%(MAX_MUTATION*2+1) - MAX_MUTATION;
      if(newFactor>255)
        newFactor = 255;
      else if(newFactor<0)
        newFactor = 0;
      _factors[i] = newFactor;
    }
  }
}

Animal::Animal(const Animal &other) : Entity(other){
  _factors = make_shared<uint8_t[]>(FACTOR_SIZE);
  for(int i=0; i<FACTOR_SIZE; i++)
    _factors[i] = other._factors[i];
}

Carnivore::Carnivore(int x, int y, shared_ptr<Carnivore> rawModel) : Animal(x, y, rawModel){
  _entityType = EntityType::CARNIVORE;
}

Herbivore::Herbivore(int x, int y, shared_ptr<Herbivore> rawModel) : Animal(x, y, rawModel){
  _entityType = EntityType::HERBIVORE;
}
