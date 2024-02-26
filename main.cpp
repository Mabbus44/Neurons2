#include <SDL.h>
#include "Graphics/SDLWindow.h"
#include <time.h>
#include "Map.h"
#include "Graphics/ConsoleHandler.h"
#include "Graphics/Hub.h"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>

using namespace std;
const std::string b = "cds";

int main( int argc, char* args2[] )
{
  srand (time(NULL));
  Hub h;
  h.run();
  std::string a = "abs";

  return 0;
}

