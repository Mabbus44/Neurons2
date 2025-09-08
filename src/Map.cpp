#include "Map.h"

Map::Map()
{
  logger->trace("Map::Map()");
  std::time_t t = std::time(0);   // get time now
  std::tm* now = std::localtime(&t);
  string mon = to_string(now->tm_mon + 1);
  string day = to_string(now->tm_mday);
  string hour = to_string(now->tm_hour);
  string min = to_string(now->tm_min);
  string sec = to_string(now->tm_sec);
  if(mon.length() == 1)
    mon = "0" + mon;
  if(day.length() == 1)
    day = "0" + day;
  if(hour.length() == 1)
    hour = "0" + hour;
  if(min.length() == 1)
    min = "0" + min;
  if(sec.length() == 1)
    sec = "0" + sec;
  _name = to_string(now->tm_year + 1900) + mon + day + " " + hour + ":" + min + ":" + sec;
  _plantPrototype = make_shared<Plant>();
  _waterPrototype = make_shared<Water>();
}

Map::~Map()
{
  logger->trace("Map::~Map()");
  free();
}

bool Map::resetMap(){
  logger->trace("Map::resetMap()");
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

  int bestCarnivoreId = 0;
  int bestHerbivoreId = 0;
  for(shared_ptr<EntitySpawner> spawner: _entitySpawners){
    spawner->generatePointCloud();
    if(spawner->entityType() == EntityType::WATER){
      spawner->addWater(_waterPrototype ,_map);
    }
  }
  for(shared_ptr<EntitySpawner> spawner: _entitySpawners){
    if(spawner->entityType() == EntityType::CARNIVORE){
      spawner->addCarnivores(_carnivores, _bestCarnivores, bestCarnivoreId, _map);
    }else if(spawner->entityType() == EntityType::HERBIVORE){
      spawner->addHerbivores(_herbivores, _bestHerbivores, bestHerbivoreId, _map);
    }else if(spawner->entityType() == EntityType::PLANT){
      spawner->addPlants(_plantPrototype ,_map);
    }
  }
  clearBestAnimals();
  _mapSetUp = true;
  _tickCount = 0;
  return true;
}

bool Map::loadMapSettings(string fileName){
  logger->trace("Map::loadMapSettings({})", fileName);
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
  logger->trace("Map::loadMapSettings(json)");
  vector<shared_ptr<Entity>> row = vector<shared_ptr<Entity>>(_sizeX);
  _map = vector<vector<shared_ptr<Entity>>>(_sizeY, row);
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
    }else if(itr.key() == "_name" && (*itr).isString()){
      _name = (*itr).asString();
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
        _map[_carnivores.back()->posY()][_carnivores.back()->posX()] = _carnivores.back();
      }
    }else if(itr.key() == "_herbivores" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _herbivores.push_back(make_shared<Herbivore>());
        _herbivores.back()->load((*itr)[entityId]);
        _map[_herbivores.back()->posY()][_herbivores.back()->posX()] = _herbivores.back();
      }
    }else if(itr.key() == "_plants" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _map[(*itr)[entityId]["y"].asInt()][(*itr)[entityId]["x"].asInt()] = _plantPrototype;
      }
    }else if(itr.key() == "_water" && (*itr).isArray()){
      for(Json::Value::ArrayIndex entityId = 0; entityId < (*itr).size(); entityId++){
        _map[(*itr)[entityId]["y"].asInt()][(*itr)[entityId]["x"].asInt()] = _waterPrototype;
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
    _defaultSpawner->loadSettings(json["defaultSpawner"], _sizeX-1, _sizeY-1);
  }
  if(json.isMember("entitySpawners") && json["entitySpawners"].isArray()){
    for(Json::Value::ArrayIndex spawnerId = 0; spawnerId < (json["entitySpawners"]).size(); spawnerId++){
      shared_ptr<EntitySpawner> e = make_shared<EntitySpawner>(*_defaultSpawner);
      if(e->loadSettings((json["entitySpawners"])[spawnerId], _sizeX-1, _sizeY-1))
        _entitySpawners.push_back(e);
    }
  }
  _validConfig = true;
  return _validConfig;
}

bool Map::tick(){
  logger->trace("Map::tick()");
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
  logger->trace("Map::draw(window)");
  shared_ptr<Entity> selected;
  if(selectedEntityId >= 0){
    if(selectedEntityType == SelectableEntityType::SEL_HERBIVORE){
      if(selectedEntityId < (int)_herbivores.size())
        selected = _herbivores[selectedEntityId];
    }
    if(selectedEntityType == SelectableEntityType::SEL_CARNIVORE){
      if(selectedEntityId < (int)_carnivores.size())
        selected = _carnivores[selectedEntityId];
    }
  }
  for(int y = 0; y < _sizeY; y++){
    for(int x = 0; x < _sizeX; x++){
      if(_map[y][x] != nullptr){
        if(_map[y][x]->entityType() == EntityType::PLANT)
          window.drawPixel(x, y, 0, 0x44, 0, _map[y][x] == selected);
        if(_map[y][x]->entityType() == EntityType::HERBIVORE)
          window.drawPixel(x, y, 0xFF, 0, 0xFF, _map[y][x] == selected);
        if(_map[y][x]->entityType() == EntityType::CARNIVORE)
          window.drawPixel(x, y, 0x88, 0, 0, _map[y][x] == selected);
        if(_map[y][x]->entityType() == EntityType::WATER)
          window.drawPixel(x, y, 0, 0, 0xFF, _map[y][x] == selected);
      }
    }
  }
}

Json::Value Map::getJson(){
  logger->trace("Map::getJson()");
  Json::Value ret;
  ret["_name"] = Json::Value(_name);
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
  ret["_water"] = Json::Value(Json::arrayValue);
  ret["_bestCarnivores"] = Json::Value(Json::arrayValue);
  ret["_bestHerbivores"] = Json::Value(Json::arrayValue);
  ret["entitySpawners"] = Json::Value(Json::arrayValue);
  for(shared_ptr<Carnivore> p: _carnivores)
    ret["_carnivores"].append(p->getJson());
  for(shared_ptr<Herbivore> p: _herbivores)
    ret["_herbivores"].append(p->getJson());
  for(int y = 0; y < _sizeY; y++){
    for(int x = 0; x < _sizeX; x++){
      if(_map[y][x] != nullptr && _map[y][x]->entityType() == EntityType::PLANT){
        Json::Value coord;
        coord["x"] = Json::Value(x);
        coord["y"] = Json::Value(y);
        ret["_plants"].append(coord);
      }
      if(_map[y][x] != nullptr && _map[y][x]->entityType() == EntityType::WATER){
        Json::Value coord;
        coord["x"] = Json::Value(x);
        coord["y"] = Json::Value(y);
        ret["_water"].append(coord);
      }
    }
  }
  for(shared_ptr<Carnivore> p: _bestCarnivores)
    ret["_bestCarnivores"].append(p->getJson());
  for(shared_ptr<Herbivore> p: _bestHerbivores)
    ret["_bestHerbivores"].append(p->getJson());
  for(shared_ptr<EntitySpawner> p: _entitySpawners)
    ret["entitySpawners"].append(p->getJson());
  return ret;
}

void Map::free(){
  logger->trace("Map::free()");
  _validConfig = false;
  clearMap();
  _entitySpawners.clear();
  clearBestAnimals();
}

void Map::clearMap(){
  logger->trace("Map::clearMap()");
  _mapSetUp = false;
  _generationDone = false;
  _map.clear();
  _carnivores.clear();
  _herbivores.clear();
}

void Map::clearBestAnimals(){
  logger->trace("Map::clearBestAnimals()");
  _bestCarnivores.clear();
  _bestHerbivores.clear();
}

shared_ptr<uint8_t[]> Map::inputFromArea(int posX, int posY, int sensorRadius){
  logger->trace("Map::inputFromArea({}, {}, {})", posX, posY, sensorRadius);
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
  logger->trace("Map::inputFromSquare({}, {}, input, {})", posX, posY, inputPos);
  if(!_map[posX][posY]){
    input[inputPos] = 255;
    input[inputPos+1] = 0;
    input[inputPos+2] = 0;
    input[inputPos+3] = 0;
    input[inputPos+4] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::CARNIVORE){
    input[inputPos] = 0;
    input[inputPos+1] = 255;
    input[inputPos+2] = 0;
    input[inputPos+3] = 0;
    input[inputPos+4] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::HERBIVORE){
    input[inputPos] = 0;
    input[inputPos+1] = 0;
    input[inputPos+2] = 255;
    input[inputPos+3] = 0;
    input[inputPos+4] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::PLANT){
    input[inputPos] = 0;
    input[inputPos+1] = 0;
    input[inputPos+2] = 0;
    input[inputPos+3] = 255;
    input[inputPos+4] = 0;
  }else if(_map[posX][posY]->entityType() == EntityType::WATER){
    input[inputPos] = 0;
    input[inputPos+1] = 0;
    input[inputPos+2] = 0;
    input[inputPos+3] = 0;
    input[inputPos+4] = 255;
  }
}

void Map::decideAcitons(){
  logger->trace("Map::decideAcitons()");
  for(shared_ptr<Carnivore> c: _carnivores)
    c->decideAction(inputFromArea(c->posX(), c->posY(), c->sensorRadius()));
  for(shared_ptr<Herbivore> h: _herbivores)
    h->decideAction(inputFromArea(h->posX(), h->posY(), h->sensorRadius()));
}

void Map::performActions(){
  logger->trace("Map::performActions()");
  for(shared_ptr<Carnivore> c: _carnivores)
    if(c->alive())
      c->performAction(_map);
  for(shared_ptr<Herbivore> h: _herbivores){
    if(h->alive())
      h->performAction(_map);
  }
}

void Map::removeDeadEntities(){
  logger->trace("Map::removeDeadEntities()");
  for(int i=0; i<(int)_herbivores.size(); i++)
    if(!_herbivores[i]->alive())
      _herbivores.erase(_herbivores.begin()+i);
  for(int i=0; i<(int)_carnivores.size(); i++)
    if(!_carnivores[i]->alive())
      _carnivores.erase(_carnivores.begin()+i);
}

void Map::saveBestCarnivores(){
  logger->trace("Map::saveBestCarnivores()");
  _bestCarnivores.clear();
  for(shared_ptr<Carnivore> c: _carnivores)
    _bestCarnivores.push_back(make_shared<Carnivore>(*c));
}

void Map::saveBestHerbivores(){
  logger->trace("Map::saveBestHerbivores()");
  _bestHerbivores.clear();
  for(shared_ptr<Herbivore> h: _herbivores)
    _bestHerbivores.push_back(make_shared<Herbivore>(*h));
}

void Map::output(string tab, OutputLevel level){
  logger->trace("Map::output({}, {})", tab, (int)level);
  switch(level){
    case OutputLevel::OUTPUT_ONELINE:
    case OutputLevel::OUTPUT_ALL:
    case OutputLevel::OUTPUT_DEEP:
    case OutputLevel::OUTPUT_OVERVIEW:
      cout << tab << "Name: " << _name << " Generation: " << _generationCount << " Tick: " << _tickCount;
      break;
  }
}

shared_ptr<Entity> Map::getSelectedEntity(){
  logger->trace("Map::getSelectedEntity()");
  shared_ptr<Entity> ret;
  if(_carnivores.size() + _herbivores.size() + _bestCarnivores.size() + _bestHerbivores.size() == 0)
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
  logger->trace("Map::deepCopy()");
  shared_ptr<Map> ret = make_shared<Map>();
  vector<shared_ptr<Entity>> row = vector<shared_ptr<Entity>>(_sizeX);
  ret->_map = vector<vector<shared_ptr<Entity>>>(_sizeY, row);
  ret->_name = _name;
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

  for(int y=0; y<_sizeY; y++){
    for(int x=0; x<_sizeX; x++){
      if(_map[y][x] != nullptr){
        if(_map[y][x]->entityType() == EntityType::CARNIVORE){
          ret->_map[y][x] = _map[y][x]->deepCopy();
          ret->_carnivores.push_back(dynamic_pointer_cast<Carnivore>(ret->_map[y][x]));
        }
        if(_map[y][x]->entityType() == EntityType::HERBIVORE){
          ret->_map[y][x] = _map[y][x]->deepCopy();
          ret->_herbivores.push_back(dynamic_pointer_cast<Herbivore>(ret->_map[y][x]));
        }
        if(_map[y][x]->entityType() == EntityType::WATER)
          ret->_map[y][x] = ret->_waterPrototype;
        if(_map[y][x]->entityType() == EntityType::PLANT)
          ret->_map[y][x] = ret->_plantPrototype;
      }
    }
  }

  for(shared_ptr<Carnivore> c: _bestCarnivores)
    ret->_bestCarnivores.push_back(c->deepCopy());
  for(shared_ptr<Herbivore> h: _bestHerbivores)
    ret->_bestHerbivores.push_back(h->deepCopy());
  for(shared_ptr<EntitySpawner> e: _entitySpawners)
    ret->_entitySpawners.push_back(e->deepCopy());
  return ret;
}
