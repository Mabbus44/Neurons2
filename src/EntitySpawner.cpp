#include "EntitySpawner.h"

bool EntitySpawner::loadSettings(Json::Value& input, int maxX, int maxY){
  logger->trace("EntitySpawner::loadSettings(input, {}, {})", maxX, maxY);
  for(Json::Value::const_iterator itr = input.begin() ; itr != input.end() ; itr++){
    if(itr.key() == "spawnerType" && (*itr).isString()){
      if((*itr).asString() == "evenDistribution")
        _spawnerType = SpawnerType::EVEN_DISTROBUTION;
      else
        cout << "Warning: Invalid value {" << *itr << "} for key {" << itr.key() << "} in spawner settings" << endl;
    }else if(itr.key() == "entityType" && (*itr).isString()){
      if((*itr).asString() == "carnivore"){
        _entityType = EntityType::CARNIVORE;
      }else if((*itr).asString() == "herbivore"){
        _entityType = EntityType::HERBIVORE;
      }else if((*itr).asString() == "plant"){
        _entityType = EntityType::PLANT;
      }else if((*itr).asString() == "water"){
        _entityType = EntityType::WATER;
      }else{
        cout << "Warning: Invalid value {" << *itr << "} for key {" << itr.key() << "} in spawner settings" << endl;
      }
    }else if(itr.key() == "entityCount" && (*itr).isIntegral()){
      _entityCount = (*itr).asInt();
    }else if(itr.key() == "shapes" && (*itr).isArray()){
      _shapes.clear();
      Json::Value shapes = *itr;
      for(Json::Value::const_iterator shapeIt = shapes.begin() ; shapeIt != shapes.end() ; shapeIt++){
        Json::Value shape = *shapeIt;
        if(shape.isMember("type") && shape["type"].isString()){
          if(shape["type"] == "rectangle"){
            _shapes.push_back(make_shared<alg::Rectangle>());
            _shapes.back()->loadSettings(shape);
          }else if(shape["type"] == "circle"){
            _shapes.push_back(make_shared<alg::Circle>());
            _shapes.back()->loadSettings(shape);
          }else{
            cout << "Warning: shape with invalid type in spawner settings" << endl;
          }
        }else{
          cout << "Warning: shape missing type in spawner setting" << endl;
        }
      }
    }else if(itr.key() == "sensorRadius" && (*itr).isIntegral()){

      /*
      factors and inputs are 8bit (unsigned).
      actionWeight is 32 bit
      1 bit is lost to sign
      actionWeight += ((_factors[factorId])-128) * (input[inputId])
      each addition is maximum a 16 bit number (unsigned)

      factorCount can maximum be a 32-1-16 = 15 bits (unsigned), so 2^15 = 32768
      for INPUTS_PER_SQURE = 5 that means _sensorRadius can be maximum 39

      factorCount = INPUTS_PER_SQUARE * (_sensorRadius * 2 + 1) * (_sensorRadius * 2 + 1)

      Since we do _factors[]-128 that one is really just 7 bits, so 2^16 = 65536 and _sensorRadius = 56.
      But I havent checked edge cases, so 2^15 is still plenty
      */
      _sensorRadius = (*itr).asInt();
      uint32_t inputSize = INPUTS_PER_SQUARE * (_sensorRadius * 2 + 1) * (_sensorRadius * 2 + 1);
      uint32_t factorSize = inputSize * AnimalAction::COUNT;
      if(factorSize >= 32768){
        cout << "Warning: FactorCount is " << factorSize << ", which is larger then 2^15, possible overflow in actionWeight calculation" << endl;
      }
    }else if(itr.key() == "maxEnergy" && (*itr).isIntegral()){
      _maxEnergy = (*itr).asInt();
    }else if(itr.key() == "startEnergy" && (*itr).isIntegral()){
      _startEnergy = (*itr).asInt();
    }else if(itr.key() == "energyCostMove" && (*itr).isIntegral()){
      _energyCostMove = (*itr).asInt();
    }else if(itr.key() == "energyCostEat" && (*itr).isIntegral()){
      _energyCostEat = (*itr).asInt();
    }else if(itr.key() == "energyCostNothing" && (*itr).isIntegral()){
      _energyCostNothing = (*itr).asInt();
    }else if(itr.key() == "energyGainEat" && (*itr).isIntegral()){
      _energyGainEat = (*itr).asInt();
    }else if(itr.key() == "eatDist" && (*itr).isIntegral()){
      _eatDist = (*itr).asInt();
    }else if(itr.key() == "maxMutation" && (*itr).isIntegral()){
      _maxMutation = (*itr).asInt();
    }else{
      cout << "Warning: Invalid key {" << itr.key() << "} in spawner settings" << endl;
    }
  }
  _pointCloud.xOverflow(maxX+1);
  _pointCloud.yOverflow(maxY+1);
  generatePointCloud();
  return true;
}

void EntitySpawner::addCarnivores(vector<shared_ptr<Carnivore>>& entities, vector<shared_ptr<Carnivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map){
  logger->trace("EntitySpawner::addCarnivores(entities, rawModels, {}, map)", rawModelId);
  int addedCount = 0;
  while(addedCount < _entityCount && _pointCloud.size() > 0){
    alg::Point p = _pointCloud.pickRandomPoint();
    if(!map[p.x()][p.y()]){
      shared_ptr<Entity> entity;
      rawModelId++;
      if(rawModelId >= (int)rawModels.size())
        rawModelId = 0;
      shared_ptr<Carnivore> baseEntity;
      if((int)rawModels.size() > 0)
        baseEntity = rawModels[rawModelId];
      entity = make_shared<Carnivore>(p.x(), p.y(), baseEntity, _sensorRadius, _startEnergy, _maxEnergy, _energyCostMove, _energyCostEat,
                                      _energyCostNothing, _energyGainEat, _eatDist, _maxMutation);
      entities.push_back(dynamic_pointer_cast<Carnivore>(entity));
      map[p.x()][p.y()] = entity;
      addedCount++;
    }
  }
}

void EntitySpawner::addHerbivores(vector<shared_ptr<Herbivore>>& entities, vector<shared_ptr<Herbivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map){
  logger->trace("EntitySpawner::addHerbivores(entities, rawModels, {}, map)", rawModelId);
  int addedCount = 0;
  while(addedCount < _entityCount && _pointCloud.size() > 0){
    alg::Point p = _pointCloud.pickRandomPoint();
    if(!map[p.x()][p.y()]){
      shared_ptr<Entity> entity;
      rawModelId++;
      if(rawModelId >= (int)rawModels.size())
        rawModelId = 0;
      shared_ptr<Herbivore> baseEntity;
      if((int)rawModels.size() > 0)
        baseEntity = rawModels[rawModelId];
      entity = make_shared<Herbivore>(p.x(), p.y(), baseEntity, _sensorRadius, _startEnergy, _maxEnergy, _energyCostMove, _energyCostEat,
                                      _energyCostNothing, _energyGainEat, _eatDist, _maxMutation);
      entities.push_back(dynamic_pointer_cast<Herbivore>(entity));
      map[p.x()][p.y()] = entity;
      addedCount++;
    }
  }
}

void EntitySpawner::addPlants(vector<shared_ptr<Plant>>& entities, vector<vector<shared_ptr<Entity>>>& map){
  logger->trace("EntitySpawner::addPlants(entities, map)");
  int addedCount = 0;
  while(addedCount < _entityCount && _pointCloud.size() > 0){
    alg::Point p = _pointCloud.pickRandomPoint();
    if(!map[p.x()][p.y()]){
      shared_ptr<Entity> entity = make_shared<Plant>(p.x(), p.y());
      entities.push_back(dynamic_pointer_cast<Plant>(entity));
      map[p.x()][p.y()] = entity;
      addedCount++;
    }
  }
}

void EntitySpawner::addWater(vector<shared_ptr<Water>>& entities, vector<vector<shared_ptr<Entity>>>& map){
  logger->trace("EntitySpawner::addWater(entities, map)");
  while(_pointCloud.size() > 0){
    alg::Point p = _pointCloud.pickFirstPoint();
    if(!map[p.x()][p.y()]){
      shared_ptr<Entity> entity = make_shared<Water>(p.x(), p.y());
      entities.push_back(dynamic_pointer_cast<Water>(entity));
      map[p.x()][p.y()] = entity;
    }
  }
}

Json::Value EntitySpawner::getJson(){
  logger->trace("EntitySpawner::getJson()");
  Json::Value ret;
  switch(_spawnerType){
    case SpawnerType::EVEN_DISTROBUTION:
      ret["spawnerType"] = Json::Value("evenDistribution");
      break;
  }
  switch(_entityType){
    case EntityType::CARNIVORE:
      ret["entityType"] = Json::Value("carnivore");
      break;
    case EntityType::HERBIVORE:
      ret["entityType"] = Json::Value("herbivore");
      break;
    case EntityType::PLANT:
      ret["entityType"] = Json::Value("plant");
      break;
    case EntityType::WATER:
      ret["entityType"] = Json::Value("water");
      break;
  }
  ret["entityCount"] = Json::Value(_entityCount);
  ret["sensorRadius"] = Json::Value(_sensorRadius);
  ret["maxEnergy"] = Json::Value(_maxEnergy);
  ret["startEnergy"] = Json::Value(_startEnergy);
  ret["energyCostMove"] = Json::Value(_energyCostMove);
  ret["energyCostEat"] = Json::Value(_energyCostEat);
  ret["energyCostNothing"] = Json::Value(_energyCostNothing);
  ret["energyGainEat"] = Json::Value(_energyGainEat);
  ret["eatDist"] = Json::Value(_eatDist);
  ret["maxMutation"] = Json::Value(_maxMutation);
  ret["shapes"] = Json::Value(Json::arrayValue);
  for(shared_ptr<alg::Shape2D> s: _shapes)
    ret["shapes"].append(s->getJson());
  return ret;
}

shared_ptr<EntitySpawner> EntitySpawner::deepCopy(){
  logger->trace("EntitySpawner::deepCopy()");
  shared_ptr<EntitySpawner> ret = make_shared<EntitySpawner>();
  ret->_entityType = _entityType;
  ret->_spawnerType = _spawnerType;
  ret->_entityCount = _entityCount;
  ret->_sensorRadius = _sensorRadius;
  ret->_maxEnergy = _maxEnergy;
  ret->_startEnergy = _startEnergy;
  ret->_energyCostMove = _energyCostMove;
  ret->_energyCostEat = _energyCostEat;
  ret->_energyCostNothing = _energyCostNothing;
  ret->_energyGainEat = _energyGainEat;
  ret->_eatDist = _eatDist;
  ret->_maxMutation = _maxMutation;
  // Copy shape pointers is fine as long as we dont change the values.
  ret->_shapes = _shapes;
  // Pointcloud is not copied since it is calculated from shapes.
  ret->generatePointCloud();
  return ret;
}

void EntitySpawner::generatePointCloud(){
  _pointCloud.clear();
  for(shared_ptr<alg::Shape2D>& shape: _shapes){
    shape->appendPointCloud(_pointCloud);
  }
}
