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
  vector<string> argSplit;
  vector<int> args;
  vector<string> strArgs;
  while(ans2!="y"){
    while(ans!="q"){
      cout << "\n\n\n\n----------Maps----------" << endl;
      outputMapsOneline();
      cout << "\n----------Main menu----------" << endl;
      cout << "create \t\tceates new map" << endl;
      cout << "delete i\tdeletes map with id i" << endl;
      cout << "init i \t\tinitializes map with id i" << endl;
      cout << "viewmap i \tshows graphics for map i" << endl;
      cout << "runstep i j \truns map with id i for j simulation steps" << endl;
      cout << "rungen i j \truns map with id i for j simulation generations" << endl;
      cout << "copy i \t\tcopy map with id i" << endl;
      cout << "status \t\toutputs simulation status" << endl;
      cout << "q \t\tquits any menu" << endl;
      cout << "run: ";
      getline(cin, ans);
      cout << endl;
      argSplit = argsTostr(ans);
      args.clear();
      strArgs.clear();
      if(argSplit.size()>0){
        args = argsToInt(argSplit[1]);
        strArgs.push_back(argSplit[1]);
      }
      if(argSplit[0] == "create") instructions.push_back(Instruction(CREATE_MAP, strArgs));
      if(argSplit[0] == "delete") instructions.push_back(Instruction(DELETE_MAP, args));
      if(argSplit[0] == "init") instructions.push_back(Instruction(INIT_MAP, args));
      if(argSplit[0] == "viewmap") instructions.push_back(Instruction(VIEW_MAP, args));
      if(argSplit[0] == "runstep") instructions.push_back(Instruction(RUN_SIMULATION_STEPS, args));
      if(argSplit[0] == "rungen") instructions.push_back(Instruction(RUN_SIMULATION_GENERATIONS, args));
      if(argSplit[0] == "copy") instructions.push_back(Instruction(COPY_MAP, args));
      if(argSplit[0] == "status") instructions.push_back(Instruction(OUTPUT_SIMULATION_STATUS, args));
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

vector<string> ConsoleHandler::argsTostr(string args){
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

void ConsoleHandler::outputMapsOneline(){
  instructions.push_back(Instruction(OUTPUT_MAPS_ONELINE));
  while(instructions.size()>0){};
}
