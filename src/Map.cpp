#include "Map.h"

bool EntitySpawner::loadSettings(Value input){
  if(input["spawnerType"] == "evenDistribution")
    _spawnerType = SpawnerType::EVEN_DISTROBUTION;
  else{
    cout << "Error: Invalid spawnertype in entitySpawner " << input << endl;
    return false;
  }
  if(input["entityType"] == "carnivore")
    _entityType = EntityType::CARNIVORE;
  else if(input["entityType"] == "herbivore")
    _entityType = EntityType::HERBIVORE;
  else if(input["entityType"] == "plant")
    _entityType = EntityType::PLANT;
  else{
    cout << "Error: Invalid entityType in entitySpawner " << input << endl;
    return false;
  }
  if(input["entityCount"].isIntegral() && input["minX"].isIntegral() && input["maxX"].isIntegral() && input["minY"].isIntegral() &&
        input["maxY"].isIntegral()){
    _entityCount = input["entityCount"].asInt();
    _minX = input["minX"].asInt();
    _maxX = input["maxX"].asInt();
    _minY = input["minY"].asInt();
    _maxY = input["maxY"].asInt();
  }else{
    cout << "Error: Missing required value (entityCount, minX, maxX, minY or maxY) in entitySpawner " << input << endl;
    return false;
  }
  return true;
}

int EntitySpawner::maxX(int mapMax){
  if(_maxX == -1 || _maxX > mapMax)
    return mapMax;
  else
    return _maxX;
}

int EntitySpawner::maxY(int mapMax){
  if(_maxY == -1 || _maxY > mapMax)
    return mapMax;
  else
    return _maxY;
}

int EntitySpawner::sizeX(int mapMax){
  if(_maxX == -1 || _maxX > mapMax)
    return mapMax-_minX+1;
  else
    return _maxX-_minX+1;
}

int EntitySpawner::sizeY(int mapMax){
  if(_maxY == -1 || _maxY > mapMax)
    return mapMax-_minY+1;
  else
    return _maxY-_minY+1;
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

  _carnivores = vector<shared_ptr<Carnivore>>(carnivoreCount);
  _herbivores = vector<shared_ptr<Herbivore>>(herbivoreCount);
  _plants = vector<shared_ptr<Plant>>(plantCount);

  int carnivoreId = 0;
  int herbivoreId = 0;
  int plantId = 0;
  int bestCarnivoreId = 0;
  int bestHerbivoreId = 0;
  for(shared_ptr<EntitySpawner> spawner: _entitySpawners){
    int entityCount = spawner->entityCount();
    for(int eId=0; eId < entityCount; eId++){
      int retries = 0;
      bool entityCreated = false;
      while(!entityCreated && retries < 3){
        int posX = spawner->minX() + (rand() % spawner->sizeX(_sizeX-1));
        int posY = spawner->minY() + (rand() % spawner->sizeY(_sizeY-1));
        if(!_map[posX][posY]){
          shared_ptr<Entity> entity;
          if(spawner->entityType() == EntityType::CARNIVORE){
            bestCarnivoreId++;
            if(bestCarnivoreId >= (int)_bestCarnivores.size())
              bestCarnivoreId = 0;
            shared_ptr<Carnivore> baseCarnivore;
            if((int)_bestCarnivores.size() > 0)
              baseCarnivore = _bestCarnivores[bestCarnivoreId];
            entity = make_shared<Carnivore>(posX, posY, baseCarnivore);
            _carnivores[carnivoreId++] = dynamic_pointer_cast<Carnivore>(entity);
          }else if(spawner->entityType() == EntityType::HERBIVORE){
            bestHerbivoreId++;
            if(bestHerbivoreId >= (int)_bestHerbivores.size())
              bestHerbivoreId = 0;
            shared_ptr<Herbivore> baseHerbivore;
            if((int)_bestHerbivores.size() > 0)
              baseHerbivore = _bestHerbivores[bestHerbivoreId];
            entity = make_shared<Herbivore>(posX, posY, baseHerbivore);
            _herbivores[herbivoreId++] = dynamic_pointer_cast<Herbivore>(entity);
          }else if(spawner->entityType() == EntityType::PLANT){
            entity = make_shared<Plant>(posX, posY);
            _plants[plantId++] = dynamic_pointer_cast<Plant>(entity);
          }
          _map[posX][posY] = entity;
          entityCreated = true;
        }else{
          retries++;
        }
      }
    }
  }
  clearBestAnimals();
  _mapSetUp = true;
  _tickCount = 0;
  return true;
}

bool Map::loadMapSettings(string fileName){
  ifstream file(fileName);
  if(file.fail()){
    cout << "Error: could not open file " << fileName << endl;
    return _validConfig;
  }
  Value fileContents;
  try{
    file >> fileContents;
  }
  catch(exception& e){
    cout << "Error: Invalid json format in file " << fileName << endl;
    cout << e.what() << endl;
    return _validConfig;
  }
  free();
  if(fileContents.isMember("entitySpawners") && fileContents["entitySpawners"].isArray()){
    Value entitySpawnersInput = fileContents["entitySpawners"];
    for(Value::ArrayIndex spawnerId = 0; spawnerId < entitySpawnersInput.size(); spawnerId++){
      shared_ptr<EntitySpawner> e = make_shared<EntitySpawner>();
      if(e->loadSettings(entitySpawnersInput[spawnerId]))
        _entitySpawners.push_back(e);
    }
  }
  if(fileContents["sizeX"].isIntegral() && fileContents["sizeY"].isIntegral() && fileContents["sizeX"].asInt() > 0 &&
        fileContents["sizeY"].asInt() > 0){
    _sizeX = fileContents["sizeX"].asInt();
    _sizeY = fileContents["sizeY"].asInt();
  }else{
    cout << "Error: Missing or invalid required value (sizeX, sizeY) in map settings " << fileContents << endl;
    return _validConfig;
  }
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
  if((int)_herbivores.size() > 0 && (int)_herbivores.size() <= RESET_HERBIVORE_COUNT && (int)_bestHerbivores.size() == 0)
    saveBestHerbivores();
  if((int)_carnivores.size() > 0 && (int)_carnivores.size() <= RESET_CARNIVORE_COUNT && (int)_bestCarnivores.size() == 0)
    saveBestCarnivores();
  if(!_generationDone && (int)_herbivores.size() <= RESET_HERBIVORE_COUNT && (int)_carnivores.size() <= RESET_CARNIVORE_COUNT){
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

shared_ptr<uint8_t[]> Map::inputFromArea(int posX, int posY){
  shared_ptr<uint8_t[]> input = make_shared<uint8_t[]>(INPUT_SIZE);
  int inputPos = 0;
  posX -= SENSOR_RADIUS_SQUARES;
  if(posX < 0)
    posX += _sizeX;
  posY -= SENSOR_RADIUS_SQUARES;
  if(posY < 0)
    posY += _sizeY;
  const int startX = posX;
  int row = 0;
  int rowAndColCount = SENSOR_RADIUS_SQUARES * 2 + 1;
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
    c->decideAction(inputFromArea(c->posX(), c->posY()));
  for(shared_ptr<Herbivore> h: _herbivores)
    h->decideAction(inputFromArea(h->posX(), h->posY()));
}

void Map::performActions(){
  for(shared_ptr<Carnivore> c: _carnivores)
    if(c->alive())
      performAction(c);
  for(shared_ptr<Herbivore> h: _herbivores){
    if(h->alive())
      performAction(h);
  }
}

void Map::performAction(shared_ptr<Animal> animal){
  AnimalAction action = animal->action();
  switch(action){
    case AnimalAction::EAT:
      eat(animal);
      animal->removeEnergy(ENERGY_COST_EAT);
      break;
    case AnimalAction::MOVE_DOWN:
    case AnimalAction::MOVE_LEFT:
    case AnimalAction::MOVE_RIGHT:
    case AnimalAction::MOVE_UPP:
      animalMove(animal, action);
      break;
    case AnimalAction::NOTHING:
      animal->removeEnergy(ENERGY_COST_NOTHING);
      break;
    default:
      cout << "Error: Invalid action " << (int)action << endl;
      break;
  }
  if(!animal->alive()){
    kill(animal);
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

void Map::eat(shared_ptr<Animal> animal){
  EntityType entityType = animal->entityType();
  EntityType preyType;
  shared_ptr<Entity> prey;
  int eatDist = 0;
  if(entityType == EntityType::CARNIVORE){
    preyType = EntityType::HERBIVORE;
    eatDist = EAT_DIST_CARNI;
  }
  else if(entityType == EntityType::HERBIVORE){
    preyType = EntityType::PLANT;
    eatDist = EAT_DIST_HERBI;
  }
  prey = findEntity(animal->posX(), animal->posY(), eatDist, preyType);
  if(!prey)
    return;
  kill(prey);
  if(entityType == EntityType::CARNIVORE)
    animal->addEnergy(ENERGY_GAIN_EAT_HERBI);
  else if(entityType == EntityType::HERBIVORE)
    animal->addEnergy(ENERGY_GAIN_EAT_PLANT);
}

void Map::kill(shared_ptr<Entity> entity){
  _map[entity->posX()][entity->posY()].reset();
  entity->kill();
}

void Map::animalMove(shared_ptr<Animal> animal, AnimalAction moveAction){
  int oldPosX = animal->posX();
  int oldPosY = animal->posY();
  int newPosX, newPosY;
  switch(moveAction){
    case AnimalAction::MOVE_DOWN:
      newPosX = oldPosX;
      newPosY = oldPosY + 1;
      if(newPosY == _sizeY)
        newPosY = 0;
      break;
    case AnimalAction::MOVE_LEFT:
      newPosX = oldPosX - 1;
      newPosY = oldPosY;
      if(newPosX < 0)
        newPosX = _sizeX-1;
      break;
    case AnimalAction::MOVE_RIGHT:
      newPosX = oldPosX + 1;
      newPosY = oldPosY;
      if(newPosX == _sizeX)
        newPosX = 0;
      break;
    case AnimalAction::MOVE_UPP:
      newPosX = oldPosX;
      newPosY = oldPosY - 1;
      if(newPosY < 0)
        newPosY = _sizeY-1;
      break;
    default:
      cout << "Error: non move action sent to move function" << endl;
      break;
  }
  if(!_map[newPosX][newPosY]){
    _map[newPosX][newPosY] = animal;
    _map[oldPosX][oldPosY].reset();
    animal->posY(newPosY);
    animal->posX(newPosX);
    animal->removeEnergy(ENERGY_COST_MOVE);
    return;
  }
  animal->removeEnergy(ENERGY_COST_NOTHING);
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

shared_ptr<Entity> Map::findEntity(int posX, int posY, int radius, EntityType entityType){
  int sideLen = 2; // Side len is actually 3, but the corners is shared between two sides, so we just look at 2 squares per side
  for(int r = 0; r < radius; r++){
    posX++;
    if(posX == _sizeX)
      posX = 0;
    posY--;
    if(posY < 0)
      posY = _sizeY - 1;
    for(int dy = 0; dy < sideLen; dy++){
      if(_map[posX][posY] && _map[posX][posY]->entityType() == entityType)
        return _map[posX][posY];
      posY++;
      if(posY == _sizeY)
        posY = 0;
    }
    for(int dx = 0; dx < sideLen; dx++){
      if(_map[posX][posY] && _map[posX][posY]->entityType() == entityType)
        return _map[posX][posY];
      posX--;
      if(posX < 0)
        posX = _sizeX - 1;
    }
    for(int dy = 0; dy < sideLen; dy++){
      if(_map[posX][posY] && _map[posX][posY]->entityType() == entityType)
        return _map[posX][posY];
      posY--;
      if(posY < 0)
        posY = _sizeY - 1;
    }
    for(int dx = 0; dx < sideLen; dx++){
      if(_map[posX][posY] && _map[posX][posY]->entityType() == entityType)
        return _map[posX][posY];
      posX++;
      if(posX == _sizeX)
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
