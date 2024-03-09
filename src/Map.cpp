#include "Map.h"

bool EntitySpawner::loadSettings(Json::Value& input, int mapSizeX, int mapSizeY){
  for(Json::Value::const_iterator itr = input.begin() ; itr != input.end() ; itr++){
    if(itr.key() == "spawnerType" && (*itr).isString()){
      if((*itr).asString() == "evenDistribution")
        _spawnerType = SpawnerType::EVEN_DISTROBUTION;
      else
        cout << "Warning: Invalid value {" << *itr << "} for key {" << itr.key() << "} in spawner settings" << endl;
    }else if(itr.key() == "entityType" && (*itr).isString()){
      if((*itr).asString() == "carnivore")
        _entityType = EntityType::CARNIVORE;
      else if((*itr).asString() == "herbivore")
        _entityType = EntityType::HERBIVORE;
      else if((*itr).asString() == "plant")
        _entityType = EntityType::PLANT;
      else
        cout << "Warning: Invalid value {" << *itr << "} for key {" << itr.key() << "} in spawner settings" << endl;
    }else if(itr.key() == "entityCount" && (*itr).isIntegral()){
      _entityCount = (*itr).asInt();
    }else if(itr.key() == "minX" && (*itr).isIntegral()){
      _minX = (*itr).asInt();
    }else if(itr.key() == "minY" && (*itr).isIntegral()){
      _minY = (*itr).asInt();
    }else if(itr.key() == "maxX" && (*itr).isIntegral()){
      _maxX = (*itr).asInt();
      if(_maxX == -1)
        _maxX = mapSizeX-1;
    }else if(itr.key() == "maxY" && (*itr).isIntegral()){
      _maxY = (*itr).asInt();
      if(_maxY == -1)
        _maxY = mapSizeY-1;
    }else if(itr.key() == "sensorRadius" && (*itr).isIntegral()){
      _sensorRadius = (*itr).asInt();
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
  return true;
}

void EntitySpawner::addCarnivores(vector<shared_ptr<Carnivore>>& entities, vector<shared_ptr<Carnivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map){
  int posX, posY;
  for(int eId=0; eId < _entityCount; eId++){
    int retries = 0;
    bool spotFree = false;
    while(!spotFree && retries < 3){
      posX = _minX + (rand() % sizeX());
      posY = _minY + (rand() % sizeY());
      if(!map[posX][posY])
        spotFree = true;
      else
        retries++;
    }
    if(spotFree){
      shared_ptr<Entity> entity;
      rawModelId++;
      if(rawModelId >= (int)rawModels.size())
        rawModelId = 0;
      shared_ptr<Carnivore> baseEntity;
      if((int)rawModels.size() > 0)
        baseEntity = rawModels[rawModelId];
      entity = make_shared<Carnivore>(posX, posY, baseEntity, _sensorRadius, _startEnergy, _maxEnergy, _energyCostMove, _energyCostEat,
                                      _energyCostNothing, _energyGainEat, _eatDist, _maxMutation);
      entities.push_back(dynamic_pointer_cast<Carnivore>(entity));
      map[posX][posY] = entity;
    }
  }
}

void EntitySpawner::addHerbivores(vector<shared_ptr<Herbivore>>& entities, vector<shared_ptr<Herbivore>>& rawModels, int& rawModelId, vector<vector<shared_ptr<Entity>>>& map){
  int posX, posY;
  for(int eId=0; eId < _entityCount; eId++){
    int retries = 0;
    bool spotFree = false;
    while(!spotFree && retries < 3){
      posX = _minX + (rand() % sizeX());
      posY = _minY + (rand() % sizeY());
      if(!map[posX][posY])
        spotFree = true;
      else
        retries++;
    }
    if(spotFree){
      shared_ptr<Entity> entity;
      rawModelId++;
      if(rawModelId >= (int)rawModels.size())
        rawModelId = 0;
      shared_ptr<Herbivore> baseEntity;
      if((int)rawModels.size() > 0)
        baseEntity = rawModels[rawModelId];
      entity = make_shared<Herbivore>(posX, posY, baseEntity, _sensorRadius, _startEnergy, _maxEnergy, _energyCostMove, _energyCostEat,
                                      _energyCostNothing, _energyGainEat, _eatDist, _maxMutation);
      entities.push_back(dynamic_pointer_cast<Herbivore>(entity));
      map[posX][posY] = entity;
    }
  }
}

void EntitySpawner::addPlants(vector<shared_ptr<Plant>>& entities, vector<vector<shared_ptr<Entity>>>& map){
  int posX, posY;
  for(int eId=0; eId < _entityCount; eId++){
    int retries = 0;
    bool spotFree = false;
    while(!spotFree && retries < 3){
      posX = _minX + (rand() % sizeX());
      posY = _minY + (rand() % sizeY());
      if(!map[posX][posY])
        spotFree = true;
      else
        retries++;
    }
    if(spotFree){
      shared_ptr<Entity> entity = make_shared<Plant>(posX, posY);
      entities.push_back(dynamic_pointer_cast<Plant>(entity));
      map[posX][posY] = entity;
    }
  }
}

Json::Value EntitySpawner::getJson(){
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
  }
  ret["entityCount"] = Json::Value(_entityCount);
  ret["minX"] = Json::Value(_minX);
  ret["minY"] = Json::Value(_minY);
  ret["maxX"] = Json::Value(_maxX);
  ret["maxY"] = Json::Value(_maxY);
  ret["sensorRadius"] = Json::Value(_sensorRadius);
  ret["maxEnergy"] = Json::Value(_maxEnergy);
  ret["startEnergy"] = Json::Value(_startEnergy);
  ret["energyCostMove"] = Json::Value(_energyCostMove);
  ret["energyCostEat"] = Json::Value(_energyCostEat);
  ret["energyCostNothing"] = Json::Value(_energyCostNothing);
  ret["energyGainEat"] = Json::Value(_energyGainEat);
  ret["eatDist"] = Json::Value(_eatDist);
  ret["maxMutation"] = Json::Value(_maxMutation);
  return ret;
}

shared_ptr<EntitySpawner> EntitySpawner::deepCopy(){
  shared_ptr<EntitySpawner> ret = make_shared<EntitySpawner>();
  ret->_entityType = _entityType;
  ret->_spawnerType = _spawnerType;
  ret->_entityCount = _entityCount;
  ret->_minX = _minX;
  ret->_minY = _minY;
  ret->_maxX = _maxX;
  ret->_maxY = _maxY;
  ret->_sensorRadius = _sensorRadius;
  ret->_maxEnergy = _maxEnergy;
  ret->_startEnergy = _startEnergy;
  ret->_energyCostMove = _energyCostMove;
  ret->_energyCostEat = _energyCostEat;
  ret->_energyCostNothing = _energyCostNothing;
  ret->_energyGainEat = _energyGainEat;
  ret->_eatDist = _eatDist;
  ret->_maxMutation = _maxMutation;
  return ret;
}

Map::Map()
{
  //ctor
}

Map::~Map()
{
  free();
}

bool Map::resetMap(){
  if(!_validConfig){
    cout << "Error: Cannot reset map, config not valid" << endl;
    return false;
  }
  clearMap();
  vector<shared_ptr<Entity>> row = vector<shared_ptr<Entity>>(_sizeX);
  _map = vector<vector<shared_ptr<Entity>>>(_sizeY, row);

  int carnivoreCount = 0;
  int herbivoreCount = 0;
  int plantCount = 0;

  for(shared_ptr<EntitySpawner> e: _entitySpawners){
    if(e->entityType() == EntityType::CARNIVORE)
      carnivoreCount += e->entityCount();
    else if(e->entityType() == EntityType::HERBIVORE)
      herbivoreCount += e->entityCount();
    else if(e->entityType() == EntityType::PLANT)
      plantCount += e->entityCount();
  }

  _carnivores = vector<shared_ptr<Carnivore>>();
  _carnivores.reserve(carnivoreCount);
  _herbivores = vector<shared_ptr<Herbivore>>();
  _herbivores.reserve(herbivoreCount);
  _plants = vector<shared_ptr<Plant>>();
  _plants.reserve(plantCount);

  int bestCarnivoreId = 0;
  int bestHerbivoreId = 0;
  for(shared_ptr<EntitySpawner> spawner: _entitySpawners){
    if(spawner->entityType() == EntityType::CARNIVORE){
      spawner->addCarnivores(_carnivores, _bestCarnivores, bestCarnivoreId, _map);
    }else if(spawner->entityType() == EntityType::HERBIVORE){
      spawner->addHerbivores(_herbivores, _bestHerbivores, bestHerbivoreId, _map);
    }else if(spawner->entityType() == EntityType::PLANT){
      spawner->addPlants(_plants, _map);
    }
  }
  clearBestAnimals();
  _mapSetUp = true;
  _tickCount = 0;
  return true;
}

void Map::populateMap(){
  vector<shared_ptr<Entity>> row = vector<shared_ptr<Entity>>(_sizeX);
  _map = vector<vector<shared_ptr<Entity>>>(_sizeY, row);
  for(shared_ptr<Carnivore> p: _carnivores)
    _map[p->posX()][p->posY()] = p;
  for(shared_ptr<Herbivore> p: _herbivores)
    _map[p->posX()][p->posY()] = p;
  for(shared_ptr<Plant> p: _plants)
    _map[p->posX()][p->posY()] = p;
}

bool Map::loadMapSettings(string fileName){
  ifstream file(fileName);
  if(file.fail()){
    cout << "Error: could not open file " << fileName << endl;
    return _validConfig;
  }
  Json::Value fileContents;
  try{
    file >> fileContents;
  }
  catch(exception& e){
    cout << "Error: Invalid json format in file " << fileName << endl;
    cout << e.what() << endl;
    return _validConfig;
  }
  free();
  return loadMapSettings(fileContents);
}

bool Map::loadMapSettings(Json::Value& json){
  for(Json::Value::const_iterator itr = json.begin() ; itr != json.end() ; itr++){
    if(itr.key() == "defaultSpawner" && (*itr).isObject()){
    }else if(itr.key() == "entitySpawners" && (*itr).isArray()){
    }else if(itr.key() == "sizeX" && (*itr).isIntegral()){
      _sizeX = (*itr).asInt();
    }else if(itr.key() == "sizeY" && (*itr).isIntegral()){
      _sizeY = (*itr).asInt();
    }else if(itr.key() == "resetHerbivoreCount" && (*itr).isIntegral()){
      _resetHerbivoreCount = (*itr).asInt();
    }else if(itr.key() == "resetCarnivoreCount" && (*itr).isIntegral()){
      _resetCarnivoreCount = (*itr).asInt();
    }else if(itr.key() == "_validConfig" && (*itr).isBool()){
      _validConfig = (*itr).asBool();
    }else if(itr.key() == "_mapSetUp" && (*itr).isBool()){
      _mapSetUp = (*itr).asBool();
    }else if(itr.key() == "_generationDone" && (*itr).isBool()){
      _generationDone = (*itr).asBool();
    }else if(itr.key() == "_tickCount" && (*itr).isIntegral()){
      _tickCount = (*itr).asInt();
    }else if(itr.key() == "_zoom" && (*itr).isIntegral()){
      _zoom = (*itr).asInt();
    }else if(itr.key() == "_generationCount" && (*itr).isIntegral()){
      _generationCount = (*itr).asInt();
    }else if(itr.key() == "_carnivores" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _carnivores.push_back(make_shared<Carnivore>());
        _carnivores.back()->load((*itr)[entityId]);
      }
    }else if(itr.key() == "_herbivores" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _herbivores.push_back(make_shared<Herbivore>());
        _herbivores.back()->load((*itr)[entityId]);
      }
    }else if(itr.key() == "_plants" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _plants.push_back(make_shared<Plant>());
        _plants.back()->load((*itr)[entityId]);
      }
    }else if(itr.key() == "_bestCarnivores" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _bestCarnivores.push_back(make_shared<Carnivore>());
        _bestCarnivores.back()->load((*itr)[entityId]);
      }
    }else if(itr.key() == "_bestHerbivores" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _bestHerbivores.push_back(make_shared<Herbivore>());
        _bestHerbivores.back()->load((*itr)[entityId]);
      }
    }else{
      cout << "Warning: Invalid key {" << itr.key() << "} in map settings" << endl;
    }
  }
  shared_ptr<EntitySpawner> _defaultSpawner = make_shared<EntitySpawner>();
  if(json.isMember("defaultSpawner") && json["defaultSpawner"].isObject()){
    _defaultSpawner->loadSettings(json["defaultSpawner"], _sizeX, _sizeY);
  }
  if(json.isMember("entitySpawners") && json["entitySpawners"].isArray()){
    for(Json::Value::ArrayIndex spawnerId = 0; spawnerId < (json["entitySpawners"]).size(); spawnerId++){
      shared_ptr<EntitySpawner> e = make_shared<EntitySpawner>(*_defaultSpawner);
      if(e->loadSettings((json["entitySpawners"])[spawnerId], _sizeX, _sizeY))
        _entitySpawners.push_back(e);
    }
  }
  populateMap();
  _validConfig = true;
  return _validConfig;
}

bool Map::tick(){
  if(!_validConfig || (pause && !oneStep))
    return _generationDone;
  if(!_mapSetUp || _generationDone)
    resetMap();
  decideAcitons();
  performActions();
  removeDeadEntities();
  if((int)_herbivores.size() > 0 && (int)_herbivores.size() <= _resetHerbivoreCount && (int)_bestHerbivores.size() == 0)
    saveBestHerbivores();
  if((int)_carnivores.size() > 0 && (int)_carnivores.size() <= _resetCarnivoreCount && (int)_bestCarnivores.size() == 0)
    saveBestCarnivores();
  if(!_generationDone && (int)_herbivores.size() <= _resetHerbivoreCount && (int)_carnivores.size() <= _resetCarnivoreCount){
    _generationDone = true;
    _generationCount++;
  }
  oneStep = false;
  _tickCount++;
  return _generationDone;
}

void Map::draw(MapWindow& window){
  shared_ptr<Entity> selected;
  if(selectedEntityId >= 0){
    if(selectedEntityType == SelectableEntityType::SEL_PLANT){
      if(selectedEntityId < (int)_plants.size())
        selected = _plants[selectedEntityId];
    }
    if(selectedEntityType == SelectableEntityType::SEL_HERBIVORE){
      if(selectedEntityId < (int)_herbivores.size())
        selected = _herbivores[selectedEntityId];
    }
    if(selectedEntityType == SelectableEntityType::SEL_CARNIVORE){
      if(selectedEntityId < (int)_carnivores.size())
        selected = _carnivores[selectedEntityId];
    }
  }
  for(shared_ptr<Entity> e : _plants)
    window.drawPixel(e->posX(), e->posY(), 0, 0x44, 0, e == selected);
  for(shared_ptr<Entity> e : _herbivores)
    window.drawPixel(e->posX(), e->posY(), 0, 0, 0xFF, e == selected);
  for(shared_ptr<Entity> e : _carnivores)
    window.drawPixel(e->posX(), e->posY(), 0x88, 0, 0, e == selected);
}

Json::Value Map::getJson(){
  Json::Value ret;
  ret["_validConfig"] = Json::Value(_validConfig);
  ret["_mapSetUp"] = Json::Value(_mapSetUp);
  ret["_generationDone"] = Json::Value(_generationDone);
  ret["sizeX"] = Json::Value(_sizeX);
  ret["sizeY"] = Json::Value(_sizeY);
  ret["_tickCount"] = Json::Value(_tickCount);
  ret["_zoom"] = Json::Value(_zoom);
  ret["_generationCount"] = Json::Value(_generationCount);
  ret["resetCarnivoreCount"] = Json::Value(_resetHerbivoreCount);
  ret["resetHerbivoreCount"] = Json::Value(_resetCarnivoreCount);
  ret["_carnivores"] = Json::Value(Json::arrayValue);
  ret["_herbivores"] = Json::Value(Json::arrayValue);
  ret["_plants"] = Json::Value(Json::arrayValue);
  ret["_bestCarnivores"] = Json::Value(Json::arrayValue);
  ret["_bestHerbivores"] = Json::Value(Json::arrayValue);
  ret["entitySpawners"] = Json::Value(Json::arrayValue);
  for(shared_ptr<Carnivore> p: _carnivores)
    ret["_carnivores"].append(p->getJson());
  for(shared_ptr<Herbivore> p: _herbivores)
    ret["_herbivores"].append(p->getJson());
  for(shared_ptr<Plant> p: _plants)
    ret["_plants"].append(p->getJson());
  for(shared_ptr<Carnivore> p: _bestCarnivores)
    ret["_bestCarnivores"].append(p->getJson());
  for(shared_ptr<Herbivore> p: _bestHerbivores)
    ret["_bestHerbivores"].append(p->getJson());
  for(shared_ptr<EntitySpawner> p: _entitySpawners)
    ret["entitySpawners"].append(p->getJson());
  return ret;
}

void Map::free(){
  _validConfig = false;
  clearMap();
  _entitySpawners.clear();
  clearBestAnimals();
}

void Map::clearMap(){
  _mapSetUp = false;
  _generationDone = false;
  _map.clear();
  _carnivores.clear();
  _herbivores.clear();
  _plants.clear();
}

void Map::clearBestAnimals(){
  _bestCarnivores.clear();
  _bestHerbivores.clear();
}

shared_ptr<uint8_t[]> Map::inputFromArea(int posX, int posY, int sensorRadius){
  int inputSize = INPUTS_PER_SQUARE * (sensorRadius * 2 + 1) * (sensorRadius * 2 + 1);
  shared_ptr<uint8_t[]> input = make_shared<uint8_t[]>(inputSize);
  int inputPos = 0;
  posX -= sensorRadius;
  if(posX < 0)
    posX += _sizeX;
  posY -= sensorRadius;
  if(posY < 0)
    posY += _sizeY;
  const int startX = posX;
  int row = 0;
  int rowAndColCount = sensorRadius * 2 + 1;
  while(row < rowAndColCount){
    int col = 0;
    posX = startX;
    while(col < rowAndColCount){
      inputFromSquare(posX, posY, input, inputPos);
      inputPos += INPUTS_PER_SQUARE;
      posX++;
      col++;
      if(posX == _sizeX)
        posX = 0;
    }
    row++;
    posY++;
    if(posY == _sizeY)
      posY = 0;
  }
  return input;
}

void Map::inputFromSquare(int posX, int posY, shared_ptr<uint8_t[]> input, int inputPos){
  if(!_map[posX][posY]){
    input[inputPos] = 255;
    input[inputPos+1] = 0;
    input[inputPos+2] = 0;
    input[inputPos+3] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::CARNIVORE){
    input[inputPos] = 0;
    input[inputPos+1] = 255;
    input[inputPos+2] = 0;
    input[inputPos+3] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::HERBIVORE){
    input[inputPos] = 0;
    input[inputPos+1] = 0;
    input[inputPos+2] = 255;
    input[inputPos+3] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::PLANT){
    input[inputPos] = 0;
    input[inputPos+1] = 0;
    input[inputPos+2] = 0;
    input[inputPos+3] = 255;
  }
}

void Map::decideAcitons(){
  for(shared_ptr<Carnivore> c: _carnivores)
    c->decideAction(inputFromArea(c->posX(), c->posY(), c->sensorRadius()));
  for(shared_ptr<Herbivore> h: _herbivores)
    h->decideAction(inputFromArea(h->posX(), h->posY(), h->sensorRadius()));
}

void Map::performActions(){
  for(shared_ptr<Carnivore> c: _carnivores)
    if(c->alive())
      c->performAction(_map);
  for(shared_ptr<Herbivore> h: _herbivores){
    if(h->alive())
      h->performAction(_map);
  }
}

void Map::removeDeadEntities(){
  for(int i=0; i<(int)_plants.size(); i++)
    if(!_plants[i]->alive())
      _plants.erase(_plants.begin()+i);
  for(int i=0; i<(int)_herbivores.size(); i++)
    if(!_herbivores[i]->alive())
      _herbivores.erase(_herbivores.begin()+i);
  for(int i=0; i<(int)_carnivores.size(); i++)
    if(!_carnivores[i]->alive())
      _carnivores.erase(_carnivores.begin()+i);
}

void Map::saveBestCarnivores(){
  _bestCarnivores.clear();
  for(shared_ptr<Carnivore> c: _carnivores)
    _bestCarnivores.push_back(make_shared<Carnivore>(*c));
}

void Map::saveBestHerbivores(){
  _bestHerbivores.clear();
  for(shared_ptr<Herbivore> h: _herbivores)
    _bestHerbivores.push_back(make_shared<Herbivore>(*h));
}

void Map::output(string tab, OutputLevel level){
  switch(level){
    case OutputLevel::OUTPUT_ONELINE:
    case OutputLevel::OUTPUT_ALL:
    case OutputLevel::OUTPUT_DEEP:
    case OutputLevel::OUTPUT_OVERVIEW:
      cout << tab << "Address: " << this << " Carnivores: " << _carnivores.size() << " Herbivores: " << _herbivores.size() << " Plants: " << _plants.size();
      break;
  }
}

shared_ptr<Entity> Map::getSelectedEntity(){
  shared_ptr<Entity> ret;
  if(_carnivores.size() + _herbivores.size() + _plants.size() + _bestCarnivores.size() + _bestHerbivores.size() == 0)
    return ret;
  bool shiftTypeDown = false;
  if(selectedEntityType < 0){
    selectedEntityType = SelectableEntityType::SEL_COUNT - 1;
    shiftTypeDown = true;
  }

  while(!ret){
    switch(selectedEntityType){
      case SelectableEntityType::SEL_CARNIVORE:
        if(_carnivores.size() > 0){
          if(selectedEntityId >= (int)_carnivores.size())
            selectedEntityId = 0;
          if(selectedEntityId < 0)
            selectedEntityId = _carnivores.size() - 1;
          ret = _carnivores[selectedEntityId];
        }
        break;
      case SelectableEntityType::SEL_HERBIVORE:
        if(_herbivores.size() > 0){
          if(selectedEntityId >= (int)_herbivores.size())
            selectedEntityId = 0;
          if(selectedEntityId < 0)
            selectedEntityId = _herbivores.size() - 1;
          ret = _herbivores[selectedEntityId];
        }
        break;
      case SelectableEntityType::SEL_PLANT:
        if(_plants.size() > 0){
          if(selectedEntityId >= (int)_plants.size())
            selectedEntityId = 0;
          if(selectedEntityId < 0)
            selectedEntityId = _plants.size() - 1;
          ret = _plants[selectedEntityId];
        }
        break;
      case SelectableEntityType::SEL_BEST_CARNIVORE:
        if(_bestCarnivores.size() > 0){
          if(selectedEntityId >= (int)_bestCarnivores.size())
            selectedEntityId = 0;
          if(selectedEntityId < 0)
            selectedEntityId = _bestCarnivores.size() - 1;
          ret = _bestCarnivores[selectedEntityId];
        }
        break;
      case SelectableEntityType::SEL_BEST_HERBIVORE:
        if(_bestHerbivores.size() > 0){
          if(selectedEntityId >= (int)_bestHerbivores.size())
            selectedEntityId = 0;
          if(selectedEntityId < 0)
            selectedEntityId = _bestHerbivores.size() - 1;
          ret = _bestHerbivores[selectedEntityId];
        }
        break;
    }
    if(!ret){
      if(shiftTypeDown)
        selectedEntityType--;
      else
        selectedEntityType++;
      if(selectedEntityType >= SelectableEntityType::SEL_COUNT)
        selectedEntityType = 0;
      if(selectedEntityType < 0 )
        selectedEntityType = SelectableEntityType::SEL_COUNT - 1;
    }
  }
  return ret;
}

shared_ptr<Map> Map::deepCopy(){
  shared_ptr<Map> ret = make_shared<Map>();
  ret->selectedEntityType = selectedEntityType;
  ret->selectedEntityId = selectedEntityId;
  ret->pause = pause;
  ret->oneStep = oneStep;
  ret->_validConfig = _validConfig;
  ret->_mapSetUp = _mapSetUp;
  ret->_generationDone = _generationDone;
  ret->_sizeX = _sizeX;
  ret->_sizeY = _sizeY;
  ret->_tickCount = _tickCount;
  ret->_zoom = _zoom;
  ret->_generationCount = _generationCount;
  ret->_resetHerbivoreCount = _resetHerbivoreCount;
  ret->_resetCarnivoreCount = _resetCarnivoreCount;
  for(shared_ptr<Carnivore> c: _carnivores)
    ret->_carnivores.push_back(c->deepCopy());
  for(shared_ptr<Herbivore> h: _herbivores)
    ret->_herbivores.push_back(h->deepCopy());
  for(shared_ptr<Plant> p: _plants)
    ret->_plants.push_back(p->deepCopy());
  for(shared_ptr<Carnivore> c: _bestCarnivores)
    ret->_bestCarnivores.push_back(c->deepCopy());
  for(shared_ptr<Herbivore> h: _bestHerbivores)
    ret->_bestHerbivores.push_back(h->deepCopy());
  for(shared_ptr<EntitySpawner> e: _entitySpawners)
    ret->_entitySpawners.push_back(e->deepCopy());
  ret->populateMap();
  return ret;
}
