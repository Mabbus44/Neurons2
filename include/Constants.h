#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

enum SimulationType {SIMULATION_STEP, SIMULATION_GENERATION};
enum SpawnerType {EVEN_DISTROBUTION};
enum EntityType {CARNIVORE, HERBIVORE, PLANT};
enum SelectableEntityType {SEL_CARNIVORE, SEL_HERBIVORE, SEL_PLANT, SEL_BEST_CARNIVORE, SEL_BEST_HERBIVORE, SEL_COUNT};
enum AnimalAction {NOTHING, MOVE_UPP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT, EAT, COUNT};
enum ConsoleAction {CREATE_MAP, INIT_MAP, DELETE_MAP, VIEW_MAP, RUN_SIMULATION_STEPS, RUN_SIMULATION_GENERATIONS, COPY_MAP, OUTPUT_SIMULATION_STATUS, OUTPUT_MAPS_ONELINE};
enum OutputLevel {OUTPUT_ONELINE, OUTPUT_OVERVIEW, OUTPUT_DEEP, OUTPUT_ALL};
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int INPUTS_PER_SQUARE = 4;    //Empty, Carnivore, Herbivore, Plant
const int SENSOR_RADIUS_SQUARES = 5;
const int MAX_ENERGY = 5000;
const int START_ENERGY = 500;
const int ENERGY_COST_MOVE = 20;
const int ENERGY_COST_EAT = 10;
const int ENERGY_COST_NOTHING = 5;
const int ENERGY_GAIN_EAT_PLANT = 1000;
const int ENERGY_GAIN_EAT_HERBI = 4000;
const int EAT_DIST_CARNI = 5;
const int EAT_DIST_HERBI = 3;
const int INPUT_SIZE = INPUTS_PER_SQUARE * (SENSOR_RADIUS_SQUARES * 2 + 1) * (SENSOR_RADIUS_SQUARES * 2 + 1);
const int FACTOR_SIZE = INPUT_SIZE * AnimalAction::COUNT;
const int MAX_MUTATION = 10;
const int RESET_HERBIVORE_COUNT = 10;
const int RESET_CARNIVORE_COUNT = 5;
const int MAX_ZOOM = 10;
const int DEFAULT_ZOOM = 3;
const bool DEBUG = false;

#endif // CONSTANTS_H_INCLUDED
