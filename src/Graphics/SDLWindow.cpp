#include "Graphics/SDLWindow.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "Constants.h"
#include "Map.h"

using namespace std;

SDLWindow::SDLWindow(){
}

SDLWindow::~SDLWindow(){
	if(_open)
    close();
}

void SDLWindow::open(int x, int y, int windowHeight, int windowWidth){
  if(_open)
    return;
  this->_windowHeight = windowHeight;
  this->_windowWidth = windowWidth;
	_open = true;
  _window = SDL_CreateWindow( "SDL Graphics", x, y, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if(!_window)
    cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
  windowId = SDL_GetWindowID(_window);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!_renderer)
    cout << "Unable to create renderer: " << SDL_GetError() << endl;
  SDL_RenderSetLogicalSize(_renderer, windowWidth, windowHeight);
}

void SDLWindow::resizeWindow(int windowHeight, int windowWidth){
  this->_windowHeight = windowHeight;
  this->_windowWidth = windowWidth;
  SDL_RenderSetLogicalSize(_renderer, windowWidth, windowHeight);
}

void SDLWindow::close(){
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  keyFocus = false;
  _open = false;
}

void NeuronsWindow::open(int x, int y, int windowHeight, int windowWidth){
  SDLWindow::open(x, y, windowHeight, windowWidth);
  _font = TTF_OpenFont( "fonts/OpenSans-Regular.ttf", 20 );
  if(!_font)
    cout << "Warning: Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << endl;
  _smallFont = TTF_OpenFont( "fonts/OpenSans-Regular.ttf", 15 );
  if(!_smallFont)
    cout << "Warning: Failed to load small lazy font! SDL_ttf Error: " << TTF_GetError() << endl;
}

void NeuronsWindow::close(){
  if(_open){
    TTF_CloseFont(_font);
    TTF_CloseFont(_smallFont);
    SDLWindow::close();
  }
}

void NeuronsWindow::resizeWindow(int windowHeight, int windowWidth){
  SDLWindow::resizeWindow(windowHeight, windowWidth);
}

void NeuronsWindow::setDrawColor(int r, int g, int b){
  SDL_SetRenderDrawColor( _renderer, r, g, b, 0xFF );
}

void NeuronsWindow::drawRect(int x, int y, int w, int h){
  SDL_Rect fillRect = {x,y,w,h};
  SDL_RenderFillRect( _renderer, &fillRect );
}

void NeuronsWindow::drawLine(int x1, int y1, int x2, int y2){
  SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
}

void NeuronsWindow::drawFactorMap(shared_ptr<uint8_t[]> factors, int x, int y, int animalAction, int inputType, int sensorRadius){
  int inputSize = INPUTS_PER_SQUARE * (sensorRadius * 2 + 1) * (sensorRadius * 2 + 1);
  int factorId = animalAction * inputSize + inputType;
  for(int dy=0; dy < sensorRadius * 2 + 1; dy++){
    for(int dx=0; dx < sensorRadius * 2 + 1; dx++){
      int r = factors[factorId];
      int b = 255-r;
      r -= 128;
      b -= 128;
      if(r<0)
        r=0;
      if(b<0)
        b=0;
      setDrawColor(r*2, 0, b*2);
      drawRect(x+dx*4, y+dy*4, 4, 4);
      factorId += INPUTS_PER_SQUARE;
    }
  }
}

void NeuronsWindow::drawText(int x, int y, string text, SDL_Color c, int fontSize){
  SDL_Surface* textSurface;
  if(fontSize >= 20)
    textSurface=TTF_RenderText_Solid( _font, text.c_str(),  c);
  else
    textSurface=TTF_RenderText_Solid( _smallFont, text.c_str(),  c);
  if(!textSurface){
    cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
    return;
  }
  SDL_Texture* mTexture = SDL_CreateTextureFromSurface( _renderer, textSurface );
  if(!mTexture){
    cout <<"Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
    return;
  }
  int mWidth = textSurface->w;
  int mHeight = textSurface->h;
  SDL_Rect rect = {x, y, mWidth, mHeight};
  SDL_RenderCopy(_renderer, mTexture, NULL, &rect);
  SDL_FreeSurface( textSurface );
  SDL_DestroyTexture( mTexture );
}

void NeuronsWindow::prepareRender(){
  SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear(_renderer);
  // Legend
  if(!mapRef)
    drawText(10,10, "No map selected", {0xFF, 0x00, 0x00}, 20);
  else{
    int x = 10;
    int y = 10;
    drawText(x,y, "Shortcuts", {0xFF, 0xFF, 0xFF},20);
    drawText(x,y+21, "q: Prev entity type", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+37, "w: Next entity type", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+53, "a: Prev entity", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+69, "s: Next entity", {0xFF, 0xFF, 0xFF},15);
    if(mapRef->pause)
      drawText(x,y+85, "p: Pause", {0xFF, 0x00, 0x00},15);
    else
      drawText(x,y+85, "p: Pause", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+101, "z: Move one tick", {0xFF, 0xFF, 0xFF},15);

    //Map stats
    x = 10;
    y = 150;
    drawText(x,y, "Map", {0xFF, 0xFF, 0xFF},20);
    drawText(x,y+21, "Generation:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+21, to_string(mapRef->generationCount() + 1), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+37, "Tick:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+37, to_string(mapRef->tickCount()), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+53, "Gen done:", {0xFF, 0xFF, 0xFF},15);
    if(mapRef->generationDone())
      drawText(x+96,y+53, "True", {0xFF, 0xFF, 0xFF},15);
    else
      drawText(x+96,y+53, "False", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+69, "Carni count:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+69, to_string(mapRef->carnivores().size()), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+85, "Herbi count:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+85, to_string(mapRef->herbivores().size()), {0xFF, 0xFF, 0xFF},15);

    //Entity stats
    x = 170;
    y = 10;
    shared_ptr<Entity> entity = mapRef->getSelectedEntity();
    if(!entity)
      return;
    bool isAnimal = true;
    switch(mapRef->selectedEntityType){
      case SelectableEntityType::SEL_CARNIVORE:
        drawText(x,y, "Carnivore", {0xFF, 0x00, 0x00}, 20);
        break;
      case SelectableEntityType::SEL_HERBIVORE:
        drawText(x,y, "Herbivore", {0x00, 0xFF, 0x00}, 20);
        break;
      case SelectableEntityType::SEL_BEST_CARNIVORE:
        drawText(x,y, "Best carnivore", {0xFF, 0x00, 0x00}, 20);
        break;
      case SelectableEntityType::SEL_BEST_HERBIVORE:
        drawText(x,y, "Best herbivore", {0x00, 0xFF, 0x00}, 20);
        break;
    }
    drawText(x,y+21, "ID:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+66,y+21, to_string(mapRef->selectedEntityId), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+37, "Pos:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+66,y+37, "(" + to_string(entity->posX()) + ", " + to_string(entity->posY()) + ")", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+53, "Alive:", {0xFF, 0xFF, 0xFF},15);
    if(entity->alive())
      drawText(x+66,y+53, "True", {0xFF, 0xFF, 0xFF},15);
    else
      drawText(x+66,y+53, "False", {0xFF, 0xFF, 0xFF},15);
    if(isAnimal){
      shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(entity);
      drawText(x,y+69, "Action:", {0xFF, 0xFF, 0xFF},15);
      switch(animal->action()){
        case AnimalAction::NOTHING:
          drawText(x+66,y+69, "Nothing", {0xFF, 0xFF, 0xFF},15);
          break;
        case AnimalAction::MOVE_UPP:
          drawText(x+66,y+69, "Up", {0xFF, 0xFF, 0xFF},15);
          break;
        case AnimalAction::MOVE_DOWN:
          drawText(x+66,y+69, "Down", {0xFF, 0xFF, 0xFF},15);
          break;
        case AnimalAction::MOVE_LEFT:
          drawText(x+66,y+69, "Left", {0xFF, 0xFF, 0xFF},15);
          break;
        case AnimalAction::MOVE_RIGHT:
          drawText(x+66,y+69, "Right", {0xFF, 0xFF, 0xFF},15);
          break;
        case AnimalAction::EAT:
          drawText(x+66,y+69, "Eat", {0xFF, 0xFF, 0xFF},15);
          break;
        default:
          drawText(x+66,y+69, "Unknown", {0xFF, 0xFF, 0xFF},15);
          break;
      }
      drawText(x,y+85, "Energy:", {0xFF, 0xFF, 0xFF},15);
      drawText(x+66,y+85, to_string(animal->energy()), {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+101, "Factors:  Empty      Carni      Herbi       Plant        Water", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+133, "Nothing", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+197, "Up", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+261, "Down", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+325, "Left", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+389, "Right", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+453, "Eat", {0xFF, 0xFF, 0xFF},15);
      for(int animalAction=0; animalAction < AnimalAction::COUNT; animalAction++){
        for(int inputType=0; inputType < INPUTS_PER_SQUARE; inputType++){
          drawFactorMap(animal->factors(), x+66 + inputType * 64, y+128 + animalAction*64, animalAction, inputType, animal->sensorRadius());
        }
      }
    }
  }
}

void NeuronsWindow::render(){
  SDL_RenderPresent(_renderer);
}

MapWindow::MapWindow(){
}

MapWindow::~MapWindow(){
}

void MapWindow::open(int x, int y, int windowHeight, int windowWidth){
	if(_open)
    return;
	SDLWindow::open(x, y, windowHeight, windowWidth);
}

void MapWindow::close(){
  if(_open){
    if(mapRef)
      mapRef->pause = false;
    SDLWindow::close();
  }
}

void MapWindow::resizeWindow(int windowHeight, int windowWidth){
	SDLWindow::resizeWindow(windowHeight, windowWidth);
}

void MapWindow::prepareRender(){
  if(!mapRef){
    cout << "No mapref" << endl;
    return;
  }
  SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear(_renderer);
  int zoom = mapRef->zoom();
  SDL_SetRenderDrawColor( _renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_Rect rect = {0,0,mapRef->sizeX()*zoom+2,mapRef->sizeY()*zoom+2};
  SDL_RenderDrawRect( _renderer, &rect);
  mapRef->draw(*this);
}

void MapWindow::render(){
  SDL_RenderPresent(_renderer);
}

void MapWindow::drawPixel(int x, int y, int r, int g, int b, bool selected){
  if(x<0 || x>=_windowWidth/2 || y<0 || y>=_windowHeight/2)
    return;

  int zoom = mapRef->zoom();
  int zoomX2 = mapRef->zoom()*2;
  int zoomD2 = mapRef->zoom()/2;
  if(zoomD2 == 0)
    zoomD2 = 1;
  SDL_SetRenderDrawColor( _renderer, r, g, b, 0xFF );
  SDL_Rect fillRect = {x*zoom+1,y*zoom+1,zoom,zoom};
  if(selected)
    fillRect = {x*zoom-zoomD2, y*zoom-zoomD2, zoomX2, zoomX2};
  SDL_RenderFillRect( _renderer, &fillRect);
}

