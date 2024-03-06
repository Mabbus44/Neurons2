#include "Entities.h"

Entity::Entity()
{
  //ctor
}

Entity::~Entity()
{
  //dtor
}

Animal::Animal(int x, int y, shared_ptr<Animal> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
           int energyCostNothing,int energyGainEat, int eatDist, int maxMutation) : Entity(x, y){
  _sensorRadius = sensorRadius;
  _energy = energy;
  _maxEnergy = maxEnergy;
  _energyCostMove = energyCostMove;
  _energyCostEat = energyCostEat;
  _energyCostNothing = energyCostNothing;
  _energyGainEat = energyGainEat;
  _eatDist = eatDist;
  _maxMutation = maxMutation;
  init(rawModel);
}

Animal::Animal(const Animal &other) : Entity(other){
  _sensorRadius = other._sensorRadius;
  _energy = other._energy;
  _maxEnergy = other._maxEnergy;
  _energyCostMove = other._energyCostMove;
  _energyCostEat = other._energyCostEat;
  _energyCostNothing = other._energyCostNothing;
  _energyGainEat = other._energyGainEat;
  _eatDist = other._eatDist;
  _maxMutation = other._maxMutation;
  initInputAndFactorSize();
  for(int i=0; i<_factorSize; i++)
    _factors[i] = other._factors[i];
}

void Animal::decideAction(shared_ptr<uint8_t[]> input){
  // As long as inputSize < 2^16 (65536) we wont get overflow if input and factor are 8 bit, and result 32 bit.
  int factorId = 0;
  int32_t highestActionWeight = -2147483648;
  _action = (AnimalAction)0;
  for(int actionId = 0; actionId < AnimalAction::COUNT; actionId++){
    int32_t actionWeight = 0;
    for(int inputId=0; inputId<_inputSize; inputId++)
      actionWeight += ((int32_t)(_factors[factorId++])-128) * (int32_t)(input[inputId]);
    if(actionWeight > highestActionWeight){
      highestActionWeight = actionWeight;
      _action = (AnimalAction)actionId;
    }
  }
}

void Animal::addEnergy(int newEnergy){
  _energy += newEnergy;
  if(_energy > _maxEnergy)
    _energy = _maxEnergy;
}

bool Animal::removeEnergy(int lostEnergy, vector<vector<shared_ptr<Entity>>>& map){
  _energy -= lostEnergy;
  if(_energy <= 0){
    _energy = 0;
    kill(map);
  }
  return _alive;
}

void Animal::init(shared_ptr<Animal> rawModel){
  initInputAndFactorSize();
  if(!rawModel){
    for(int i=0; i<_factorSize; i++)
      _factors[i] = rand() % 256;
  }else{
    for(int i=0; i<_factorSize; i++){
      int newFactor = rawModel->_factors[i] + rand()%(_maxMutation*2+1) - _maxMutation;
      if(newFactor>255)
        newFactor = 255;
      else if(newFactor<0)
        newFactor = 0;
      _factors[i] = newFactor;
    }
  }
}

void Animal::initInputAndFactorSize(){
  _inputSize = INPUTS_PER_SQUARE * (_sensorRadius * 2 + 1) * (_sensorRadius * 2 + 1);
  _factorSize = _inputSize * AnimalAction::COUNT;
  _factors = make_shared<uint8_t[]>(_factorSize);
}

void Animal::tryEat(vector<vector<shared_ptr<Entity>>>& map){
  shared_ptr<Entity> prey;
  if(_entityType == EntityType::CARNIVORE)
    prey = findEntity(map, EntityType::HERBIVORE);
  else if(_entityType == EntityType::HERBIVORE)
    prey = findEntity(map, EntityType::PLANT);
  if(!prey){
    removeEnergy(_energyCostEat, map);
    return;
  }
  prey->kill(map);
  addEnergy(_energyGainEat);
  removeEnergy(_energyCostEat, map);
}

shared_ptr<Entity> Animal::findEntity(vector<vector<shared_ptr<Entity>>>& map, EntityType entityType){
  int posX = _posX;
  int posY = _posY;
  int sizeX = map.size();
  int sizeY = map[0].size();
  int sideLen = 2; // Side len is actually 3, but the corners is shared between two sides, so we just look at 2 squares per side
  for(int r = 0; r < _eatDist; r++){
    posX++;
    if(posX == sizeX)
      posX = 0;
    posY--;
    if(posY < 0)
      posY = sizeY - 1;
    for(int dy = 0; dy < sideLen; dy++){
      if(map[posX][posY] && map[posX][posY]->entityType() == entityType)
        return map[posX][posY];
      posY++;
      if(posY == sizeY)
        posY = 0;
    }
    for(int dx = 0; dx < sideLen; dx++){
      if(map[posX][posY] && map[posX][posY]->entityType() == entityType)
        return map[posX][posY];
      posX--;
      if(posX < 0)
        posX = sizeX - 1;
    }
    for(int dy = 0; dy < sideLen; dy++){
      if(map[posX][posY] && map[posX][posY]->entityType() == entityType)
        return map[posX][posY];
      posY--;
      if(posY < 0)
        posY = sizeY - 1;
    }
    for(int dx = 0; dx < sideLen; dx++){
      if(map[posX][posY] && map[posX][posY]->entityType() == entityType)
        return map[posX][posY];
      posX++;
      if(posX == sizeX)
        posX = 0;
    }
    sideLen += 2;
  }
  switch(entityType){
    case EntityType::CARNIVORE:{
      return shared_ptr<Carnivore>();
      break;}
    case EntityType::HERBIVORE:{
      return shared_ptr<Herbivore>();
      break;}
    case EntityType::PLANT:{
      return shared_ptr<Plant>();
      break;}
  }
  return shared_ptr<Entity>();
}

void Animal::performAction(vector<vector<shared_ptr<Entity>>>& map){
  switch(_action){
    case AnimalAction::EAT:
      tryEat(map);
      break;
    case AnimalAction::MOVE_DOWN:
    case AnimalAction::MOVE_LEFT:
    case AnimalAction::MOVE_RIGHT:
    case AnimalAction::MOVE_UPP:
      movePos(map, _action);
      break;
    case AnimalAction::NOTHING:
      removeEnergy(_energyCostNothing, map);
      break;
    default:
      cout << "Error: Invalid action " << (int)_action << endl;
      break;
  }
}

void Animal::movePos(vector<vector<shared_ptr<Entity>>>& map, AnimalAction moveAction){
  int newPosX = _posX;
  int newPosY = _posY;
  shared_ptr<Entity> thisEntity = map[_posX][_posY];
  switch(moveAction){
    case AnimalAction::MOVE_DOWN:
      newPosY++;
      if(newPosY == (int)map[0].size())
        newPosY = 0;
      break;
    case AnimalAction::MOVE_LEFT:
      newPosX--;
      if(newPosX < 0)
        newPosX = map.size()-1;
      break;
    case AnimalAction::MOVE_RIGHT:
      newPosX++;
      if(newPosX == (int)map.size())
        newPosX = 0;
      break;
    case AnimalAction::MOVE_UPP:
      newPosY--;
      if(newPosY < 0)
        newPosY = map[0].size()-1;
      break;
    default:
      cout << "Error: non move action sent to move function" << endl;
      break;
  }
  if(!map[newPosX][newPosY]){
    map[newPosX][newPosY] = thisEntity;
    map[_posX][_posY].reset();
    _posX = newPosX;
    _posY = newPosY;
    removeEnergy(_energyCostMove, map);
    return;
  }
  removeEnergy(_energyCostNothing, map);
}

Carnivore::Carnivore(int x, int y, shared_ptr<Carnivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
            int energyCostNothing,int energyGainEat, int eatDist, int maxMutation) : Animal(x, y, rawModel, sensorRadius, energy, maxEnergy,
            energyCostMove, energyCostEat, energyCostNothing, energyGainEat, eatDist, maxMutation){
  _entityType = EntityType::CARNIVORE;
}

Herbivore::Herbivore(int x, int y, shared_ptr<Herbivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
            int energyCostNothing,int energyGainEat, int eatDist, int maxMutation) : Animal(x, y, rawModel, sensorRadius, energy, maxEnergy,
            energyCostMove, energyCostEat, energyCostNothing, energyGainEat, eatDist, maxMutation){
  _entityType = EntityType::HERBIVORE;
}
