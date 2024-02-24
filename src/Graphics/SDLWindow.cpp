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
  if( _window == NULL )
    cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
  windowId = SDL_GetWindowID(_window);
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (_renderer == NULL)
    cout << "Unable to create renderer: " << SDL_GetError() << endl;
  SDL_RenderSetLogicalSize(_renderer, windowWidth, windowHeight);
  _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
  if (_texture == NULL)
    cout << "Unable to create texture: " << SDL_GetError() << endl;
}

void SDLWindow::resizeWindow(int windowHeight, int windowWidth){
  this->_windowHeight = windowHeight;
  this->_windowWidth = windowWidth;
  SDL_DestroyTexture(_texture);
  SDL_RenderSetLogicalSize(_renderer, windowWidth, windowHeight);
  _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
  if (_texture == NULL)
    cout << "Unable to create texture: " << SDL_GetError() << endl;
}

void SDLWindow::close(){
  SDL_DestroyTexture(_texture);
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  keyFocus = false;
  _open = false;
}

void NeuronsWindow::open(int x, int y, int windowHeight, int windowWidth){
  SDLWindow::open(x, y, windowHeight, windowWidth);
  _font = TTF_OpenFont( "fonts/OpenSans-Regular.ttf", 20 );
  if( _font == NULL )
    cout << "Warning: Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << endl;
  _smallFont = TTF_OpenFont( "fonts/OpenSans-Regular.ttf", 15 );
  if( _smallFont == NULL )
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

void NeuronsWindow::drawFactorMap(uint8_t* factors, int x, int y, int animalAction, int inputType){
  int factorId = animalAction * INPUT_SIZE + inputType;
  for(int dy=0; dy < SENSOR_RADIUS_SQUARES * 2 + 1; dy++){
    for(int dx=0; dx < SENSOR_RADIUS_SQUARES * 2 + 1; dx++){
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
  if( textSurface == NULL ){
    cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
    return;
  }
  SDL_Texture* mTexture = SDL_CreateTextureFromSurface( _renderer, textSurface );
  if( mTexture == NULL ){
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
  if(mapRef == nullptr)
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
    drawText(x,y+21, "Tick:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+21, to_string(mapRef->tickCount()), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+37, "Gen done:", {0xFF, 0xFF, 0xFF},15);
    if(mapRef->generationDone())
      drawText(x+96,y+37, "True", {0xFF, 0xFF, 0xFF},15);
    else
      drawText(x+96,y+37, "False", {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+53, "Carni count:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+53, to_string(mapRef->carnivores().size()), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+69, "Herbi count:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+69, to_string(mapRef->herbivores().size()), {0xFF, 0xFF, 0xFF},15);
    drawText(x,y+85, "Plant count:", {0xFF, 0xFF, 0xFF},15);
    drawText(x+96,y+85, to_string(mapRef->plants().size()), {0xFF, 0xFF, 0xFF},15);

    //Entity stats
    x = 170;
    y = 10;
    Entity* entity = mapRef->getSelectedEntity();
    if(entity == nullptr)
      return;
    bool isAnimal = true;
    switch(mapRef->selectedEntityType){
      case SelectableEntityType::SEL_CARNIVORE:
        drawText(x,y, "Carnivore", {0xFF, 0x00, 0x00}, 20);
        break;
      case SelectableEntityType::SEL_HERBIVORE:
        drawText(x,y, "Herbivore", {0x00, 0xFF, 0x00}, 20);
        break;
      case SelectableEntityType::SEL_PLANT:
        drawText(x,y, "Plant", {0x00, 0x00, 0xFF}, 20);
        isAnimal = false;
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
      Animal* animal = (Animal*)entity;
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
      drawText(x,y+101, "Factors:  Empty      Carni      Herbi       Plant", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+133, "Nothing", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+197, "Up", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+261, "Down", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+325, "Left", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+389, "Right", {0xFF, 0xFF, 0xFF},15);
      drawText(x,y+453, "Eat", {0xFF, 0xFF, 0xFF},15);
      for(int animalAction=0; animalAction < AnimalAction::COUNT; animalAction++){
        for(int inputType=0; inputType < INPUTS_PER_SQUARE; inputType++){
          drawFactorMap(animal->factors(), x+66 + inputType * 64, y+128 + animalAction*64, animalAction, inputType);
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
//  if(_open)
//    delete[] _pixels;
}

void MapWindow::open(int x, int y, int windowHeight, int windowWidth){
	if(_open)
    return;
	SDLWindow::open(x, y, windowHeight, windowWidth);
/*  _pixels = new uint8_t[windowWidth * windowHeight * 4];
  for(int i=0; i<windowWidth*windowHeight; i++){
    _pixels[i*4] = 0xFF;
    _pixels[i*4+1] = 0xFF;
    _pixels[i*4+2] = 0xFF;
    _pixels[i*4+3] = 0xFF;
  }*/
}

void MapWindow::close(){
  if(_open){
    if(mapRef != nullptr)
      mapRef->pause = false;
//    delete[] _pixels;
    SDLWindow::close();
  }
}

void MapWindow::resizeWindow(int windowHeight, int windowWidth){
	SDLWindow::resizeWindow(windowHeight, windowWidth);
/*  delete [] _pixels;
  _pixels = new uint8_t[windowWidth * windowHeight * 4];
  for(int i=0; i<windowWidth*windowHeight; i++){
    _pixels[i*4] = 0x00;
    _pixels[i*4+1] = 0x00;
    _pixels[i*4+2] = 0xFF;
    _pixels[i*4+3] = 0xFF;
  }*/
}

void MapWindow::prepareRender(){
  if(mapRef == nullptr){
    cout << "No mapref" << endl;
    return;
  }
  for(int i=0; i<_windowWidth*_windowHeight; i++){
    _pixels[i*4] = 0xFF;
    _pixels[i*4+1] = 0xFF;
    _pixels[i*4+2] = 0xFF;
    _pixels[i*4+3] = 0xFF;
  }
  mapRef->draw(this);
  int texture_pitch = 0;
  void* texture_pixels = NULL;
  if (SDL_LockTexture(_texture, NULL, &texture_pixels, &texture_pitch) != 0) {
    cout << "Unable to lock texture: " << SDL_GetError() << endl;
  }
  else {
    memcpy(texture_pixels, _pixels, texture_pitch * _windowHeight);
  }
  SDL_UnlockTexture(_texture);
  SDL_SetRenderDrawColor(_renderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear(_renderer);
  SDL_RenderCopy(_renderer, _texture, NULL, NULL);
}

void MapWindow::render(){
  SDL_RenderPresent(_renderer);
}

void MapWindow::drawPixel(int x, int y, int r, int g, int b, bool selected){
  if(x<0 || x>=_windowWidth/2 || y<0 || y>=_windowHeight/2)
    return;

  _pixels[(2*x+2*y*_windowWidth)*4] = r;
  _pixels[(2*x+1+2*y*_windowWidth)*4] = r;
  _pixels[(2*x+(2*y+1)*_windowWidth)*4] = r;
  _pixels[(2*x+1+(2*y+1)*_windowWidth)*4] = r;
  _pixels[(2*x+2*y*_windowWidth)*4+1] = g;
  _pixels[(2*x+1+2*y*_windowWidth)*4+1] = g;
  _pixels[(2*x+(2*y+1)*_windowWidth)*4+1] = g;
  _pixels[(2*x+1+(2*y+1)*_windowWidth)*4+1] = g;
  _pixels[(2*x+2*y*_windowWidth)*4+2] = b;
  _pixels[(2*x+1+2*y*_windowWidth)*4+2] = b;
  _pixels[(2*x+(2*y+1)*_windowWidth)*4+2] = b;
  _pixels[(2*x+1+(2*y+1)*_windowWidth)*4+2] = b;
  if(selected){
    if(x>0){
      if(y>0){
        _pixels[(2*x-1+(2*y-1)*_windowWidth)*4] = r;
        _pixels[(2*x-1+(2*y-1)*_windowWidth)*4+1] = g;
        _pixels[(2*x-1+(2*y-1)*_windowWidth)*4+2] = b;
      }
      _pixels[(2*x-1+(2*y)*_windowWidth)*4] = r;
      _pixels[(2*x-1+(2*y)*_windowWidth)*4+1] = g;
      _pixels[(2*x-1+(2*y)*_windowWidth)*4+2] = b;
      _pixels[(2*x-1+(2*y+1)*_windowWidth)*4] = r;
      _pixels[(2*x-1+(2*y+1)*_windowWidth)*4+1] = g;
      _pixels[(2*x-1+(2*y+1)*_windowWidth)*4+2] = b;
      if(y<(_windowHeight/2-1)){
        _pixels[(2*x-1+(2*y+2)*_windowWidth)*4] = r;
        _pixels[(2*x-1+(2*y+2)*_windowWidth)*4+1] = g;
        _pixels[(2*x-1+(2*y+2)*_windowWidth)*4+2] = b;
      }
    }
    if(y>0){
      _pixels[(2*x+(2*y-1)*_windowWidth)*4] = r;
      _pixels[(2*x+(2*y-1)*_windowWidth)*4+1] = g;
      _pixels[(2*x+(2*y-1)*_windowWidth)*4+2] = b;
      _pixels[(2*x+1+(2*y-1)*_windowWidth)*4] = r;
      _pixels[(2*x+1+(2*y-1)*_windowWidth)*4+1] = g;
      _pixels[(2*x+1+(2*y-1)*_windowWidth)*4+2] = b;
    }
    if(x<(_windowWidth/2-1)){
      if(y>0){
        _pixels[(2*x+2+(2*y-1)*_windowWidth)*4] = r;
        _pixels[(2*x+2+(2*y-1)*_windowWidth)*4+1] = g;
        _pixels[(2*x+2+(2*y-1)*_windowWidth)*4+2] = b;
      }
      _pixels[(2*x+2+(2*y)*_windowWidth)*4] = r;
      _pixels[(2*x+2+(2*y)*_windowWidth)*4+1] = g;
      _pixels[(2*x+2+(2*y)*_windowWidth)*4+2] = b;
      _pixels[(2*x+2+(2*y+1)*_windowWidth)*4] = r;
      _pixels[(2*x+2+(2*y+1)*_windowWidth)*4+1] = g;
      _pixels[(2*x+2+(2*y+1)*_windowWidth)*4+2] = b;
      if(y<(_windowHeight/2-1)){
        _pixels[(2*x+2+(2*y+2)*_windowWidth)*4] = r;
        _pixels[(2*x+2+(2*y+2)*_windowWidth)*4+1] = g;
        _pixels[(2*x+2+(2*y+2)*_windowWidth)*4+2] = b;
      }
    }
    if(y<(_windowHeight/2-1)){
      _pixels[(2*x+(2*y+2)*_windowWidth)*4] = r;
      _pixels[(2*x+(2*y+2)*_windowWidth)*4+1] = g;
      _pixels[(2*x+(2*y+2)*_windowWidth)*4+2] = b;
      _pixels[(2*x+1+(2*y+2)*_windowWidth)*4] = r;
      _pixels[(2*x+1+(2*y+2)*_windowWidth)*4+1] = g;
      _pixels[(2*x+1+(2*y+2)*_windowWidth)*4+2] = b;
    }
  }
}

