#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

#include "NeuralNet.h"
#include "GeneticAlg.h"

//Initalization +
struct TILE_TYPE{
	bool isPassable;
};

bool isPassable(float x, float y, float width, float height, float deltaX, float deltaY);
bool insideMap(float x, float y, float width, float height);
void drawMap();
void drawTile(float x, float y, int tileId);
string itos(int arg); //converts an integer to a std::string
string ftos (float arg); //converts an float to a std::string
void clamp(double &arg, double min, double max); //	clamps the first argument between the second two
bool loadConfiguration();
void newGeneration();

ALLEGRO_DISPLAY *display;

ALLEGRO_FONT *defaultFont;
ALLEGRO_FONT *smallFont;
ALLEGRO_FONT *bigFont;

ALLEGRO_BITMAP *cursorImage;
ALLEGRO_BITMAP *playerImage;
ALLEGRO_BITMAP *groundImage1;
ALLEGRO_BITMAP *groundImage2;
ALLEGRO_BITMAP *brokenWallImage;

ALLEGRO_KEYBOARD_STATE keyState;
ALLEGRO_MOUSE_STATE mouseState;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;

GeneticAlg*	geneticAlg;

Player *playerList[1];
vector<NeuralNet> networkList;
vector<Genome> populationList;

Engine engine;

ofstream fitnessFile("logs/NNGA.txt");

vector<vector<int> > mapArray;
vector<vector<int> > mapTileArray;

int mapArrayWidth, mapArrayHeight;

const char* versionNumber;
float fpsTimeNew, fpsCounter, fpsTimeOld;

bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, updateTick, inGame;
float mouseX, mouseY;
int lastKeyPress, mouseWheel = 0;

int score, updateTickTime, updateTickTimeHelper;

double logicSpeed, rockSpawnChance, activationResponse, neuronBias, crossoverRate, mutationRate, maxPerturbation, bestFitness, averageFitness;
int numHiddenLayers, neuronsPerHiddenLayer, numElite, numCopiesElite, populationSize, currentNetwork, currentGeneration;

TILE_TYPE tileIndex[] = {
	{true}, // (0) TILE_GROUND1
	{false}, // (1) TILE_WALL
};
//Initalization -

int main(){
    srand(time(0));

	Engine engine;

    fitnessFile << "Current Generation|Best Fitness|Average Fitness" << endl;

	//Initialize the Engine
	engine.init("Neural Network", screenWidth, screenHeight, false);

    if(!loadConfiguration()){
        al_show_native_message_box(display,
            "File not found",
            "File not found",
            "Check if the file config.ini exist inside the config folder",
            NULL,
        ALLEGRO_MESSAGEBOX_ERROR);
        engine.quit();
    }

	//Load the Menu
	engine.changeState(PlayState::instance());

    //Timestep Variables
    double t = 0.0;
    double dt = 1/logicSpeed;

    double currentTime = 0.0;
    double newTime = 0.0;
    double frameTime = 0.0;

    double accumulator = 0.0;

	//Main Loop
    while(engine.running()){
        ALLEGRO_EVENT events;
        al_wait_for_event(event_queue, &events);

        timerEvent = false;

        if(events.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            engine.quit();
        }

        if(events.type == ALLEGRO_EVENT_TIMER){
            timerEvent = true;
        }
        //Main Timer Event +
        if(timerEvent){
            al_get_mouse_state(&mouseState);

            al_get_keyboard_state(&keyState);

            //Update Mouse Variables +
            mouseX = al_get_mouse_state_axis(&mouseState,0);
            mouseY = al_get_mouse_state_axis(&mouseState,1);
            mouseWheel = al_get_mouse_state_axis(&mouseState, 2);

            if(al_mouse_button_down(&mouseState, 1)){
                mouseButtonLeft = true;
                mouseButtonLeftClick = false;
            }else if(mouseButtonLeft){
                mouseButtonLeftClick = true;
                mouseButtonLeft = false;
            }else{
                mouseButtonLeftClick = false;
            }

            if(al_mouse_button_down(&mouseState, 2)){
                mouseButtonRight = true;
                mouseButtonRightClick = false;
            }else if(mouseButtonRight){
                mouseButtonRightClick = true;
                mouseButtonRight = false;
            }else{
                mouseButtonRightClick = false;
            }
            //Update Mouse Variables -

            //Rest +
            if(!al_key_down(&keyState, lastKeyPress)){
                lastKeyPress = 0;
            }
            //Rest -

            if(events.timer.source == timer){
                newTime = al_get_time();
                frameTime = newTime - currentTime;
                if(frameTime > 0.25)
                    frameTime = 0.25;	  // note: max frame time to avoid spiral of death
                currentTime = newTime;

                accumulator += frameTime;

                while(accumulator >= dt){
                    engine.update(); //Call the gameState specfic update
                    dt = 1/logicSpeed;
                    t += dt;
                    accumulator -= dt;
                }

                engine.draw();

                al_draw_bitmap(cursorImage, mouseX-4, mouseY-4, 0);

                al_flip_display();
                al_clear_to_color(al_map_rgb(0, 0, 0));
            }
        }
        //Main Timer Event -
    }
	// cleanup the engine
	engine.cleanup();

    fitnessFile.close();
}

bool isPassable(float x, float y, float width, float height){
    if(!insideMap(x, y, width, height)){
        return false;
    }else{
        int tX = floor(x/tileSize), tY = floor(y/tileSize), tWX = floor((x+width-1)/tileSize), tHY = floor((y+height-1)/tileSize);

        if(!tileIndex[mapArray[tX][tY]].isPassable || !tileIndex[mapArray[tWX][tY]].isPassable || !tileIndex[mapArray[tX][tHY]].isPassable || !tileIndex[mapArray[tWX][tHY]].isPassable){
            return false;
        }
    }
    return true;
}

bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight){
    if(x + width - 1 < ex || x > ewidth + ex - 1 || y + height - 1 < ey || y > eheight + ey - 1){
        return false;
    }else{
        return true;
    }
}

bool insideMap(float x, float y, float width, float height){
    if(x < 0 || x + width-1 >= mapArrayWidth*tileSize || y < 0 || y + height-1 >= mapArrayHeight*tileSize){
        return false;
    }
    return true;
}

void drawMap(){
    for(int x = 0; x < mapArrayWidth; x++){
        for(int y = 0; y < mapArrayHeight; y++){
            drawTile(x, y, mapArray[x][y]);
        }
    }
}

void drawTile(float x, float y, int tileId){
    switch(tileId){
        case 0:
            //al_draw_filled_rectangle(x*tileSize, y*tileSize, x*tileSize+tileSize, y*tileSize+tileSize, al_map_rgb(150, 150, 150));
            al_draw_rotated_bitmap((mapTileArray[(int)x][(int)y] / 4 >= 1) ? groundImage1 : groundImage2, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, (mapTileArray[(int)x][(int)y]%4)*90*toRadians, NULL);
            break;

        case 1:
            //al_draw_filled_rectangle(x*tileSize, y*tileSize, x*tileSize+tileSize, y*tileSize+tileSize, al_map_rgb(100, 100, 100));
            al_draw_rotated_bitmap(brokenWallImage, tileSize/2, tileSize/2, x*tileSize+tileSize/2, y*tileSize+tileSize/2, mapTileArray[(int)x][(int)y]*90*toRadians, NULL);
            break;
    }
}

string dtos(int arg){
    ostringstream buffer;
    buffer << arg;
    return buffer.str();
}

string dtos(float arg){
    ostringstream buffer;
    buffer << arg;
    return buffer.str();
}

void clamp(double &arg, double min, double max){
	if(arg < min){
		arg = min;
	}
	if(arg > max){
		arg = max;
	}
}

bool loadConfiguration(){
    ifstream get("config/config.ini");

    if(!get){
        return false;
    }

    string description;

    get >> description >> mapArrayWidth;
    get >> description >> mapArrayHeight;

    mapArray.resize(mapArrayWidth);
    mapTileArray.resize(mapArrayWidth);
    for(int i = 0; i < mapArrayWidth; i++){
        mapArray[i].resize(mapArrayHeight);
        mapTileArray[i].resize(mapArrayHeight);
    }

    get >> description >> rockSpawnChance;
    get >> description >> activationResponse;
    get >> description >> neuronBias;
    get >> description >> crossoverRate;
    get >> description >> mutationRate;
    get >> description >> maxPerturbation;
    get >> description >> numHiddenLayers;
    get >> description >> neuronsPerHiddenLayer;
    get >> description >> numElite;
    get >> description >> numCopiesElite;
    get >> description >> populationSize;

    get.close();

    return true;
}
