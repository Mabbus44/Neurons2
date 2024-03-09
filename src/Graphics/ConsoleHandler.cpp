#include "Graphics/ConsoleHandler.h"

using namespace std;

ConsoleHandler::ConsoleHandler(){
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
  if( TTF_Init() == -1 )
    cout << "Warning: SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
}

ConsoleHandler::~ConsoleHandler(){
	SDL_Quit();
}

void ConsoleHandler::run(){
  string ans;
  string ans2;
  vector<string> commandAndArgs;
  vector<int> args;
  vector<string> strArgs;
  while(ans2!="y"){
    while(ans!="q"){
      cout << "\n\n\n\n----------Maps----------" << endl;
      outputMapsOneline();
      cout << "\n----------Main menu----------" << endl;
      cout << "create s\tceates new map from file s" << endl;
      cout << "copy i \t\tcopy map with id i" << endl;
      cout << "delete i\tdeletes map with id i" << endl;
      cout << "save i s\tsaves state of map with id i to file s" << endl;
      cout << "load s\t\tload state from file s" << endl;
      cout << "viewmap i\tshows graphics for map i" << endl;
      cout << "runstep i j\truns map with id i for j simulation steps" << endl;
      cout << "rungen i j\truns map with id i for j simulation generations" << endl;
      cout << "status\t\toutputs simulation status" << endl;
      cout << "q \t\tquits any menu" << endl;
      cout << "run: ";
      getline(cin, ans);
      cout << endl;
      commandAndArgs = splitCommandAndArguments(ans);
      args.clear();
      strArgs.clear();
      if(commandAndArgs.size()>0){
        args = argsToInt(commandAndArgs[1]);
        strArgs = argsTostr(commandAndArgs[1]);
      }
      if(commandAndArgs[0] == "create") instructions.push_back(Instruction(CREATE_MAP, strArgs));
      if(commandAndArgs[0] == "copy") instructions.push_back(Instruction(COPY_MAP, args));
      if(commandAndArgs[0] == "delete") instructions.push_back(Instruction(DELETE_MAP, args));
      if(commandAndArgs[0] == "save") instructions.push_back(Instruction(SAVE_MAP_STATE, strArgs));
      if(commandAndArgs[0] == "load") instructions.push_back(Instruction(LOAD_MAP_STATE, strArgs));
      if(commandAndArgs[0] == "viewmap") instructions.push_back(Instruction(VIEW_MAP, args));
      if(commandAndArgs[0] == "runstep") instructions.push_back(Instruction(RUN_SIMULATION_STEPS, args));
      if(commandAndArgs[0] == "rungen") instructions.push_back(Instruction(RUN_SIMULATION_GENERATIONS, args));
      if(commandAndArgs[0] == "status") instructions.push_back(Instruction(OUTPUT_SIMULATION_STATUS, args));
      int aa2;
      while(instructions.size()>0){
        int aa=0;
        aa++;
        if(aa==100){
          aa2++;
          cout << "Main menu locked " << aa2 << endl;
          aa = 0;
        }
      };
    }
    cout << "Are you sure? (y/n)";
    getline(cin, ans2);
    ans="";
  }
}

vector<int> ConsoleHandler::argsToInt(string args){
  vector<int> ret;
  int pos;
  while ((pos = args.find(" ")) != (int)string::npos) {
    try{
      ret.push_back(stoi(args.substr(0, pos)));
    }catch(...){
      ret.push_back(0);
    }
    args.erase(0, pos + 1);
  }
  if(args.length()>0){
    try{
      ret.push_back(stoi(args));
    }catch(...){
      ret.push_back(0);
    }
  }
  return ret;
}

vector<string> ConsoleHandler::splitCommandAndArguments(string args){
  vector<string> ret;
  int pos;
  if((pos = args.find(" ")) == (int)string::npos) {
    ret.push_back(args);
    ret.push_back("");
  }else{
    ret.push_back(args.substr(0, pos));
    ret.push_back(args.substr(pos+1, args.length()-pos-1));
  }
  return ret;
}

vector<string> ConsoleHandler::argsTostr(string args){
  vector<string> ret;
  int pos;
  while((pos = args.find(" ")) != (int)string::npos) {
    ret.push_back(args.substr(0, pos));
    args.erase(0, pos + 1);
  }
  if(args.length() > 0)
    ret.push_back(args);
  return ret;
}

void ConsoleHandler::outputMapsOneline(){
  instructions.push_back(Instruction(OUTPUT_MAPS_ONELINE));
  while(instructions.size()>0){};
}
