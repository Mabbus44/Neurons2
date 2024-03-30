#include "Entities.h"

Entity::Entity()
{
  //ctor
}

Entity::~Entity()
{
  //dtor
}

void Entity::load(const Json::Value& json){
  if(json["_posX"].isInt())
    _posX = (AnimalAction)json["_posX"].asInt();
  if(json["_posY"].isInt())
    _posY = json["_posY"].asInt();
  if(json["_entityType"].isInt())
    _entityType = EntityType(json["_entityType"].asInt());
  if(json["_alive"].isInt())
    _alive = json["_alive"].asBool();
}

Json::Value Entity::getJson(Json::Value ret){
  ret["_posX"] = Json::Value(_posX);
  ret["_posY"] = Json::Value(_posY);
  ret["_entityType"] = Json::Value(_entityType);
  ret["_alive"] = Json::Value(_alive);
  return ret;
}

shared_ptr<Entity> Entity::deepCopy(shared_ptr<Entity> ret){
  ret->_posX = _posX;
  ret->_posY = _posY;
  ret->_entityType = _entityType;
  ret->_alive = _alive;
  return ret;
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

void Animal::load(const Json::Value& json){
  Entity::load(json);
  if(json["_action"].isInt())
    _action = (AnimalAction)json["_action"].asInt();
  if(json["_sensorRadius"].isInt())
    _sensorRadius = json["_sensorRadius"].asInt();
  if(json["_energy"].isInt())
    _energy = json["_energy"].asInt();
  if(json["_maxEnergy"].isInt())
    _maxEnergy = json["_maxEnergy"].asInt();
  if(json["_energyCostMove"].isInt())
    _energyCostMove = json["_energyCostMove"].asInt();
  if(json["_energyCostEat"].isInt())
    _energyCostEat = json["_energyCostEat"].asInt();
  if(json["_energyCostNothing"].isInt())
    _energyCostNothing = json["_energyCostNothing"].asInt();
  if(json["_energyGainEat"].isInt())
    _energyGainEat = json["_energyGainEat"].asInt();
  if(json["_eatDist"].isInt())
    _eatDist = json["_eatDist"].asInt();
  if(json["_maxMutation"].isInt())
    _maxMutation = json["_maxMutation"].asInt();
  if(json["_inputSize"].isInt())
    _inputSize = json["_inputSize"].asInt();
  if(json["_factorSize"].isInt())
    _factorSize = json["_factorSize"].asInt();
  initInputAndFactorSize();                   //_inputSize and _factorSize will be overwritten, but perhaps the values from the file can be used in the future.
  int addedFactors = 0;
  if(json["_factors"].isArray()){
    for(Json::Value::ArrayIndex factorId = 0; factorId < (json["_factors"]).size(); factorId++){
      if(addedFactors >= _factorSize){
        cout << "Warning: Json for Entity has more factors than _factorSize" << endl;
      }else{
        _factors[addedFactors++] = json["_factors"][factorId].asInt();
      }
    }
    if(addedFactors < _factorSize)
      cout << "Warning: Json for entity has fewer factors than _factorSize" << endl;
  }
}

Json::Value Animal::getJson(Json::Value ret){
  ret = Entity::getJson(ret);
  ret["_action"] = Json::Value(_action);
  ret["_sensorRadius"] = Json::Value(_sensorRadius);
  ret["_energy"] = Json::Value(_energy);
  ret["_maxEnergy"] = Json::Value(_maxEnergy);
  ret["_energyCostMove"] = Json::Value(_energyCostMove);
  ret["_energyCostEat"] = Json::Value(_energyCostEat);
  ret["_energyCostNothing"] = Json::Value(_energyCostNothing);
  ret["_energyGainEat"] = Json::Value(_energyGainEat);
  ret["_eatDist"] = Json::Value(_eatDist);
  ret["_maxMutation"] = Json::Value(_maxMutation);
  ret["_inputSize"] = Json::Value(_inputSize);
  ret["_factorSize"] = Json::Value(_factorSize);
  ret["_factors"] = Json::Value(Json::arrayValue);
  for(int i=0; i<_factorSize; i++)
    ret["_factors"].append(Json::Value(_factors[i]));
  return ret;
}

void Animal::decideAction(shared_ptr<uint8_t[]> input){
  /*
  Ideally I would like to calculate actionWeightSum = sum(actionWeigt for all actions)
  and then the ods for each action would be actionWeight/actionWeightSum.
  But since that would require 64bit int to not overflow, and saving all actionweights in a vector I figured it might be slow.
  (But on a second thougth it might be negitable, but I have worked out this other method, and sunken cost fallacy is pretty cool,
   I´ll do it this other way)
  So each actionWeight is always competing against the current best actionWeight.
  And everytime a action wins its chanses are doubled for the next competition.
  The math will end up to almost the same as proper method, 32 bit int is enough, and no vector required, hopefully making it faster.

  A positive actionWeight always beat a negative one.
  */
  int factorId = 0;
  uint32_t highestActionWeight = 0;               // Weight of leading action
  uint32_t highestActionMultiplier = 1;           // Multiplier of leading actions weight (2^how many times it has won)
  bool highestActionWeightPositiveSign = false;   // If leading actions weight is positive
  _action = (AnimalAction)0;                      // ID of leading action
  for(int actionId = 0; actionId < AnimalAction::COUNT; actionId++){
    int32_t actionWeight = 0;
    for(int inputId=0; inputId<_inputSize; inputId++)
      actionWeight += ((int32_t)(_factors[factorId++])-128) * (int32_t)(input[inputId]);
    bool win = false;
    bool positive = true;

    if(actionWeight < 0){
      actionWeight += 2147483648; // Convert to positive number
      positive = false;
    }
    if(highestActionWeightPositiveSign == positive){
      uint32_t actionWeightCopy = (uint32_t)actionWeight / highestActionMultiplier;
      uint32_t prodSum = highestActionWeight + actionWeightCopy;
      while(prodSum > RAND_MAX){
        // Scale to fit random number generator
        prodSum /= 2;
        actionWeightCopy /= 2;
      }
      if(rand()%prodSum < actionWeightCopy)
        win = true;
    }
    if(!highestActionWeightPositiveSign && positive)
      win = true;
    if(win){
      highestActionWeight = actionWeight;
      highestActionMultiplier = 2;
      highestActionWeightPositiveSign = positive;
      _action = (AnimalAction)actionId;
    }else
      highestActionMultiplier *= 2;
  }
}

void Animal::decideActionDeterministic(shared_ptr<uint8_t[]> input){
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

shared_ptr<Animal> Animal::deepCopy(shared_ptr<Animal> ret){
  Entity::deepCopy(dynamic_pointer_cast<Entity>(ret));
  ret->_action = _action;
  ret->_sensorRadius = _sensorRadius;
  ret->_energy = _energy;
  ret->_maxEnergy = _maxEnergy;
  ret->_energyCostMove = _energyCostMove;
  ret->_energyCostEat = _energyCostEat;
  ret->_energyCostNothing = _energyCostNothing;
  ret->_energyGainEat = _energyGainEat;
  ret->_eatDist = _eatDist;
  ret->_maxMutation = _maxMutation;
  ret->_inputSize = _inputSize;
  ret->_factorSize = _factorSize;
  ret->_factors = make_shared<uint8_t[]>(_factorSize);
  for(int i=0; i<_factorSize; i++)
    ret->_factors[i] = _factors[i];
  return ret;
}

Carnivore::Carnivore(int x, int y, shared_ptr<Carnivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
            int energyCostNothing,int energyGainEat, int eatDist, int maxMutation) : Animal(x, y, rawModel, sensorRadius, energy, maxEnergy,
            energyCostMove, energyCostEat, energyCostNothing, energyGainEat, eatDist, maxMutation){
  _entityType = EntityType::CARNIVORE;
}

shared_ptr<Carnivore> Carnivore::deepCopy(){
  shared_ptr<Carnivore> ret = make_shared<Carnivore>();
  Animal::deepCopy(dynamic_pointer_cast<Animal>(ret));
  return ret;
}

Herbivore::Herbivore(int x, int y, shared_ptr<Herbivore> rawModel, int sensorRadius, int energy, int maxEnergy, int energyCostMove, int energyCostEat,
            int energyCostNothing,int energyGainEat, int eatDist, int maxMutation) : Animal(x, y, rawModel, sensorRadius, energy, maxEnergy,
            energyCostMove, energyCostEat, energyCostNothing, energyGainEat, eatDist, maxMutation){
  _entityType = EntityType::HERBIVORE;
}

shared_ptr<Herbivore> Herbivore::deepCopy(){
  shared_ptr<Herbivore> ret = make_shared<Herbivore>();
  Animal::deepCopy(dynamic_pointer_cast<Animal>(ret));
  return ret;
}

shared_ptr<Plant> Plant::deepCopy(){
  shared_ptr<Plant> ret = make_shared<Plant>();
  Entity::deepCopy(dynamic_pointer_cast<Entity>(ret));
  return ret;
}
