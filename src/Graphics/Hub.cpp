#include "Graphics/Hub.h"

shared_ptr<spdlog::logger> logger;

Hub::Hub()
{
  auto max_size = 1024*1024*50;
  auto max_files = 3;
  logger = spdlog::rotating_logger_st("logger", "logs/log.txt", max_size, max_files);
  logger->set_level(DEBUG_LEVEL);
  logger->debug("Logging started");
}

Hub::~Hub()
{
}

void Hub::run(){
  loadMapState({});
  auto _consoleRet = async(&ConsoleHandler::run, &_console);
  while(!_quit){
    parseConsoleInstructions();
    runSimulations();
    if(_neuronWindow.isOpen() || _mapWindow.isOpen()){
      handleSdlEvents();
    }
  }
}

void Hub::parseConsoleInstructions(){
  while(_console.instructions.size()>0){
    switch(_console.instructions[0].inst){
      case CREATE_MAP:
        createMap(_console.instructions[0].strArgs);
        break;
      case DELETE_MAP:
        deleteMap(_console.instructions[0].args);
        break;
      case VIEW_MAP:
        viewMap(_console.instructions[0].args);
        break;
      case RUN_SIMULATION_STEPS:
        runSimulationSteps(_console.instructions[0].args);
        break;
      case RUN_SIMULATION_GENERATIONS:
        runSimulationGenerations(_console.instructions[0].args);
        break;
      case COPY_MAP:
        copyMap(_console.instructions[0].args);
        break;
      case OUTPUT_SIMULATION_STATUS:
        outputSimulationStatus(_console.instructions[0].args);
        break;
      case OUTPUT_MAPS_ONELINE:
        outputMapsOneline(_console.instructions[0].args);
        break;
      case SAVE_MAP_STATE:
        saveMapState(_console.instructions[0].strArgs);
        break;
      case LOAD_MAP_STATE:
        loadMapState(_console.instructions[0].strArgs);
        break;
      case RENAME_MAP:
        renameMap(_console.instructions[0].strArgs);
        break;
      case QUIT:
        saveMapState({});
        _quit = true;
        break;
    }
    _console.instructions.erase(_console.instructions.begin());
  }
}

void Hub::handleSdlEvents(){
  if(_mapWindow.isOpen()){
    _mapWindow.prepareRender();
    _mapWindow.render();
  }
  if(_neuronWindow.isOpen()){
    _neuronWindow.prepareRender();
    _neuronWindow.render();
  }
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0){
    switch(e.type){
      case SDL_WINDOWEVENT:
        handleWindowEvents(e);
        break;
      case SDL_KEYDOWN:
        handleKeyEvents(e);
        break;
    }
  }
}

void Hub::handleWindowEvents(SDL_Event& e){
  switch((int)e.window.event)
  {
    case SDL_WINDOWEVENT_CLOSE:
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen())
        _mapWindow.close();
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen())
        _neuronWindow.close();
      break;
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen()){
        _mapWindow.resizeWindow(e.window.data2, e.window.data1);
        _mapWindow.prepareRender();
        _mapWindow.render();
      }
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen()){
        _neuronWindow.resizeWindow(e.window.data2, e.window.data1);
        _neuronWindow.prepareRender();
        _neuronWindow.render();
      }
      break;
    case SDL_WINDOWEVENT_EXPOSED:
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen())
        _mapWindow.render();
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen())
        _neuronWindow.render();
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen())
        _mapWindow.keyFocus = true;
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen())
        _neuronWindow.keyFocus = true;
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen())
        _mapWindow.keyFocus = false;
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen())
        _neuronWindow.keyFocus = false;
      break;
  }
}

void Hub::handleKeyEvents(SDL_Event& e){
  switch((int)e.key.keysym.sym)
  {
    case SDLK_q:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->selectedEntityType--;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->selectedEntityType--;
      break;
    case SDLK_w:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->selectedEntityType++;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->selectedEntityType++;
      break;
    case SDLK_a:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->selectedEntityId--;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->selectedEntityId--;
      break;
    case SDLK_s:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->selectedEntityId++;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->selectedEntityId++;
      break;
    case SDLK_z:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->oneStep = true;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->oneStep = true;
      break;
    case SDLK_p:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->pause = !_neuronWindow.mapRef->pause;
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->pause = !_mapWindow.mapRef->pause;
      break;
    case SDLK_PLUS:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->zoomIn();
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->zoomIn();
      break;
    case SDLK_MINUS:
      if(e.window.windowID == _neuronWindow.windowId && _neuronWindow.isOpen() && _neuronWindow.mapRef)
        _neuronWindow.mapRef->zoomOut();
      if(e.window.windowID == _mapWindow.windowId && _mapWindow.isOpen() && _mapWindow.mapRef)
        _mapWindow.mapRef->zoomOut();
  }
}

void Hub::runSimulations(){
  for(SimulationJob& job:_jobs){
    if(!job.m->pause || job.m->oneStep){
      bool generationDone = job.m->tick();
      if(job.simulationType == SIMULATION_GENERATION && generationDone)
        job.stepsLeft--;
      if(job.simulationType == SIMULATION_STEP)
        job.stepsLeft--;
    }
  }
  for(int i=_jobs.size()-1; i>=0; i--){
    if(_jobs[i].stepsLeft<=0)
      _jobs.erase(_jobs.begin()+i);
  }
}

void Hub::createMap(vector<string> args){
  cout << "Creating map " << _maps.size() << "..." << endl;
  string fileName = DEFAULT_SETTINGS_FILENAME;
  if(args.size() > 0 && args[0].size() > 0)
    fileName = args[0];
  if(fileName[0] == '/')
    fileName = fileName.substr(1, fileName.size() - 1);
  shared_ptr<Map> m = make_shared<Map>();
  if(m->loadMapSettings(fileName)){
    _maps.push_back(m);
    cout << "Map " << _maps.size()-1 << " created" << endl;
    m->resetMap();
  }else{
    cout << "Error: Could not create map" << endl;
  }
}

void Hub::renameMap(vector<string> args){
  if(args.size()<2){
    cout << "Error: Missing arguments" << endl;
    return;
  }
  int i = -1;
  try{
    i = stoi(args[0]);
  }catch(...){
    i = -1;
  }
  if(i==-1){
    cout << "Error: Invalid map id" << endl;
    return;
  }

  shared_ptr<Map> m = map(i);
  if(!m){
    cout << "Error: Map id does not exist" << endl;
    return;
  }
  m->name(args[1]);
  cout << "Map name changed" << endl;
}

void Hub::deleteMap(vector<int> args){
  shared_ptr<Map> m;
  int i=-1;
  if(args.size()>0)
    i=args[0];
  if(!(m=map(i)))
    return;
  cout << "Deleting map " << i << "..." << endl;
  _maps.erase(_maps.begin()+i);
  cout << "Map " << i << " deleted" << endl;
}

void Hub::copyMap(vector<int> args){
  shared_ptr<Map> m;
  int i=-1;
  if(args.size()>0)
    i=args[0];
  if(!(m=map(i)))
    return;
  cout << "Copying map " << i << "..." << endl;
  _maps.push_back(m->deepCopy());
  cout << "Done" << endl;
}

void Hub::saveMapState(vector<string> args){
  int i=-1;
  string fileName = DEFAULT_MAP_STATE_FILENAME;
  if(args.size()>1){
    try{
      i = stoi(args[0]);
    }catch(...){
      i = -1;
    }
    fileName = args[1];
  }
  if(args.size() == 1)
    fileName = args[0];
  if(_maps.size() == 0){
    cout << "No maps to save" << endl;
    return;
  }
  ofstream file(fileName);
  if(file.fail()){
    cout << "Error: could not create file " << fileName << endl;
    return;
  }
  Json::Value fileContents = Json::Value(Json::arrayValue);
  if(i==-1){
    cout << "Saving map state for all maps to file " << fileName << "..." << endl;
    for(shared_ptr<Map>& m: _maps)
      fileContents.append(m->getJson());
  }else{
    shared_ptr<Map> m;
    if(!(m=map(i)))
      return;
    cout << "Saving map state " << i << " to filename " << fileName << "..." << endl;
    fileContents.append(m->getJson());
  }
  try{
    file << fileContents;
  }
  catch(exception& e){
    cout << "Error: Could not write to file " << fileName << endl;
    cout << e.what() << endl;
    file.close();
    return;
  }
  file.close();
  cout << "Save done" << endl;
}

void Hub::loadMapState(vector<string> args){
  string fileName = DEFAULT_MAP_STATE_FILENAME;
  if(args.size()>0)
    fileName = args[0];
  ifstream file(fileName);
  if(file.fail()){
    cout << "Error: could not open file " << fileName << endl;
    return;
  }
  Json::Value fileContents;
  cout << "Loading map states from file " << fileName << "..." << endl;
  try{
    file >> fileContents;
  }
  catch(exception& e){
    cout << "Error: Could not read from file " << fileName << endl;
    cout << e.what() << endl;
    file.close();
    return;
  }
  file.close();

  if(fileContents.isArray()){
    for(Json::Value::ArrayIndex mapId = 0; mapId < fileContents.size(); mapId++){
      _maps.push_back(make_shared<Map>());
      _maps.back()->loadMapSettings(fileContents[mapId]);
    }
  }else{
    cout << "File format invalid, does not contain array" << endl;
    return;
  }
  cout << fileContents.size() << " map states loaded" << endl;
}

void Hub::viewMap(vector<int> args){
  shared_ptr<Map> m;
  int i=-1;
  if(args.size()>0)
    i=args[0];
  if(!(m=map(i)))
    return;
  _mapWindow.mapRef = m;
  if(!_mapWindow.isOpen())
    _mapWindow.open(0, 35);
  _neuronWindow.mapRef = m;
  if(!_neuronWindow.isOpen())
    _neuronWindow.open(WINDOW_WIDTH, 35);
  cout << "window for map " << i << " opened" << endl;
}

void Hub::runSimulationSteps(vector<int> args){
  shared_ptr<Map> m;
  int i=-1;
  if(args.size()>0)
    i=args[0];
  if(!(m=map(i)))
    return;
  int steps = 1;
  if(args.size()>1 && args[1]>0)
    steps=args[1];
  cout << "Starting simulation on map " << i << " for " << steps << " step(s)..." << endl;
  _jobs.push_back(SimulationJob(SIMULATION_STEP, steps, m));
}

void Hub::runSimulationGenerations(vector<int> args){
  shared_ptr<Map> m;
  int i=-1;
  if(args.size()>0)
    i=args[0];
  if(!(m=map(i)))
    return;
  int steps = 1;
  if(args.size()>1 && args[1]>0)
    steps=args[1];
  cout << "Starting simulation on map " << i << " for " << steps << " generation(s)..." << endl;
  _jobs.push_back(SimulationJob(SIMULATION_GENERATION, steps, m));
}

void Hub::outputSimulationStatus(vector<int> args){
  if(_jobs.size()==0)
    cout << "No simulations running" << endl;
  for(SimulationJob& job: _jobs){
    cout << "Simulation have " << job.stepsLeft;
    if(job.simulationType == SIMULATION_GENERATION)
      cout << " generation";
    if(job.simulationType == SIMULATION_STEP)
      cout << " step";
    if(job.stepsLeft>1)
      cout << "s";
    cout << " left" << endl;
  }
}

shared_ptr<Map> Hub::map(int& i){
  if(_maps.size() == 0){
    cout << "No maps available " << endl;
    return shared_ptr<Map>();
  }
  if(i<-1 || i>=(int)_maps.size()){
    cout << "Map " << i << " does not exist" << endl;
    return shared_ptr<Map>();
  }
  if(i==-1)
    i=_maps.size()-1;
  return _maps[i];
}

void Hub::outputMapsOneline(vector<int> args){
  for(int i=0 ; i<(int)_maps.size(); i++){
    cout << "Map " << i << ": ";
    _maps[i]->output("", OUTPUT_ONELINE);
    cout << endl;
  }
}
