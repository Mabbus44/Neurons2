#ifndef SDLWINDOW_H
#define SDLWINDOW_H

#include "common.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include "Constants.h"
#include "Entities.h"
#include <memory>

class Map;

using namespace std;

class SDLWindow
{
  public:
    SDLWindow();
    virtual ~SDLWindow();
    virtual void open(int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED, int windowHeight=WINDOW_HEIGHT, int windowWidth=WINDOW_WIDTH);
    virtual void resizeWindow(int windowHeight, int windowWidth);
    virtual void prepareRender()=0;
    virtual void render()=0;
    virtual void close();
    bool isOpen(){return _open;}
    Uint32 windowId;
    bool keyFocus = false;
    int xOffset=0, yOffset=0;
  protected:
    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
    bool _open = false;
    int _windowHeight, _windowWidth;
};

class NeuronsWindow: public SDLWindow{
  public:
    NeuronsWindow(){};
    virtual ~NeuronsWindow(){};
    virtual void open(int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED, int windowHeight=WINDOW_HEIGHT, int windowWidth=WINDOW_WIDTH);
    virtual void resizeWindow(int windowHeight, int windowWidth);
    virtual void prepareRender();
    virtual void render();
    virtual void close();
    void drawClear();
    void setDrawColor(int r, int g, int b);
    void drawText(int x, int y, string text, SDL_Color c, int fontSize=12);
    void drawRect(int x, int y, int w, int h);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawFactorMap(shared_ptr<uint8_t[]> factors, int x, int y, int animalAction, int inputType, int sensorRadius);
    shared_ptr<Map> mapRef;
  protected:
    TTF_Font* _font = nullptr;
    TTF_Font* _smallFont = nullptr;
};

class MapWindow: public SDLWindow{
  public:
    MapWindow();
    virtual ~MapWindow();
    virtual void open(int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED, int windowHeight=WINDOW_HEIGHT, int windowWidth=WINDOW_WIDTH);
    virtual void resizeWindow(int windowHeight, int windowWidth);
    virtual void prepareRender();
    virtual void render();
    virtual void close();
    void drawPixel(int x, int y, int r, int g, int b, bool selected=false);
    shared_ptr<Map> mapRef;
  protected:
};
#endif // SDLWINDOW_H
