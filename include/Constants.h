#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED
#include <string>
#include "spdlog/sinks/rotating_file_sink.h"
using namespace std;

//Enums
enum SimulationType {SIMULATION_STEP, SIMULATION_GENERATION};   //Types of simulations that can be started from console.
enum SpawnerType {EVEN_DISTROBUTION};                           //Distrobution patterns that a spawner in a map setting can have.
enum EntityType {CARNIVORE, HERBIVORE, PLANT, WATER};           //Types of entities that can exist on a map.
enum SelectableEntityType {SEL_CARNIVORE, SEL_HERBIVORE, SEL_BEST_CARNIVORE, SEL_BEST_HERBIVORE, SEL_COUNT};   //Selectable entity types in the neurons window.
enum AnimalAction {NOTHING, MOVE_UPP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, EAT, COUNT};  //Actions that entities can take on the map.
enum ConsoleAction {CREATE_MAP, DELETE_MAP, VIEW_MAP, RUN_SIMULATION_STEPS, RUN_SIMULATION_GENERATIONS, COPY_MAP, OUTPUT_SIMULATION_STATUS, OUTPUT_MAPS_ONELINE, SAVE_MAP_STATE, LOAD_MAP_STATE, RENAME_MAP, QUIT}; //Commands that can be given from console to main program.
enum OutputLevel {OUTPUT_ONELINE, OUTPUT_OVERVIEW, OUTPUT_DEEP, OUTPUT_ALL};          //Detail levels that can be passed to output funktions of for example maps and entities.

//Constants
const int WINDOW_WIDTH = 800;       //With of map window and neurons window when they are opened.
const int WINDOW_HEIGHT = 800;      //With of map window and neurons window when they are opened.
const int INPUTS_PER_SQUARE = 5;    //Number of inputs for each square when caltulating actions (Empty, Carnivore, Herbivore, Plant, Water).
const int MAX_ZOOM = 10;            //Max zoom when viewing map.
const int DEFAULT_ZOOM = 3;         //Zoom level when map window is opened.
const string DEFAULT_SETTINGS_FILENAME = "defaultMapSettings.mse";    //Name of map settings file used when none is provided in the console.
const string DEFAULT_MAP_STATE_FILENAME = "autosave.mst";             //Map state saved/loaded when program is exited/entered
const spdlog::level::level_enum DEBUG_LEVEL = spdlog::level::trace;
const int POINT_CLOUD_SIDE_OVERFLOW = 10000;                                            //Max value of x/y coordinate in point cloud (+1 since we have 0)
const int POINT_CLOUD_MAX_SIZE = POINT_CLOUD_SIDE_OVERFLOW * POINT_CLOUD_SIDE_OVERFLOW; //Max size of pointCloud

#endif // CONSTANTS_H_INCLUDED
