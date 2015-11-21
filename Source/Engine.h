#ifndef ENGINE_H
#define ENGINE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

#define PI 3.14159265359
#define toRadians 0.0174532925
#define RANDOM_NUM  ((double)rand()/(RAND_MAX+1))

#define screenWidth 1280
#define screenHeight 720

#define tileSize 32

#define MAX_PLAYERS 1

#define FPS 60.0

using namespace std;

#include "Player.h"
#include "NeuralNet.h"
#include "GeneticAlg.h"

bool isPassable(float x, float y, float width, float height);
bool checkCollision(float x, float y, float ex, float ey, float width, float height, float ewidth, float eheight);
bool insideMap(float x, float y, float width, float height);
void drawMap();
void drawTile(float x, float y, int tileId);
string dtos(int arg); //converts an integer to a std::string
string dtos (float arg); //converts an float to a std::string
void clamp(double &arg, double min, double max); //	clamps the first argument between the second two

inline int randInt(int x,int y){return rand()%(y-x+1)+x;} //returns a random integer between x and y

inline double randFloat() {return (rand())/(RAND_MAX+1.0);} //returns a random float between zero and 1

inline bool RandBool(){ //returns a random bool
	if(randInt(0,1))return true;
	else return false;
}

inline double randomClamped(){return randFloat() - randFloat();} //returns a random float in the range -1 < n < 1

struct sPoint{
	float x, y;

	sPoint(){}
	sPoint(float a, float b):x(a),y(b){}
};


extern ALLEGRO_DISPLAY *display;

extern ALLEGRO_FONT *defaultFont;
extern ALLEGRO_FONT *smallFont;
extern ALLEGRO_FONT *bigFont;

extern ALLEGRO_BITMAP *cursorImage;
extern ALLEGRO_BITMAP *playerImage;
extern ALLEGRO_BITMAP *groundImage1;
extern ALLEGRO_BITMAP *groundImage2;
extern ALLEGRO_BITMAP *brokenWallImage;

extern ALLEGRO_KEYBOARD_STATE keyState;
extern ALLEGRO_MOUSE_STATE mouseState;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;

extern GeneticAlg* geneticAlg;

extern Player *playerList[1];
extern vector<NeuralNet> networkList;
extern vector<Genome> populationList;

extern ofstream fitnessFile;

extern vector<vector<int> > mapArray;
extern vector<vector<int> > mapTileArray;

extern int mapArrayWidth, mapArrayHeight;

extern const char* versionNumber;
extern float fpsTimeNew, fpsCounter, fpsTimeOld;

extern bool drawScreen, timerEvent, done, mouseButtonLeft, mouseButtonLeftClick, mouseButtonRight, mouseButtonRightClick, updateTick, inGame;
extern float mouseX, mouseY;
extern int lastKeyPress, mouseWheel;

extern int score, updateTickTime, updateTickTimeHelper;

extern double logicSpeed, rockSpawnChance, activationResponse, neuronBias, crossoverRate, mutationRate, maxPerturbation, bestFitness, averageFitness;
extern int numHiddenLayers, neuronsPerHiddenLayer, numElite, numCopiesElite, populationSize, currentNetwork, currentGeneration;

class GameState;

class Engine{
    public:
        void init(const char* title, int width, int height, bool fullscreen);
        void cleanup();

        void changeState(GameState* state);
        void pushState(GameState* state);
        void popState();

        void update();
        void draw();

        bool running(){ return m_running; }
        void quit(){m_running = false;}

    private:
        // the stack of states
        vector<GameState*> states;

        bool m_running;
        bool m_fullscreen;
};

#endif // ENGINE_H
