#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

#include "NeuralNet.h"
#include "GeneticAlg.h"

void Engine::init(const char* title, int width, int height, bool fullscreen){

	// initialize ALLEGRO
	al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_acodec_addon();
    al_install_keyboard();
    al_install_mouse();
    al_install_audio();

    if(fullscreen){
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	}else{
        al_set_new_display_flags(ALLEGRO_WINDOWED);
	}
    display = al_create_display(screenWidth, screenHeight);
    al_set_window_title(display, title);

    bigFont = al_load_font("fonts/pixelFont.ttf", 48, 0);
    defaultFont = al_load_font("fonts/pixelFont.ttf", 24, 0);
    smallFont = al_load_font("fonts/pixelFont.ttf", 16, 0);

    cursorImage = al_load_bitmap("graphics/cursorImage.png");
    playerImage = al_load_bitmap("graphics/playerImage.png");
    groundImage1 = al_load_bitmap("graphics/groundImage1.png");
    groundImage2 = al_load_bitmap("graphics/groundImage2.png");
    brokenWallImage = al_load_bitmap("graphics/brokenWallImage.png");

    al_reserve_samples(0);

    event_queue = al_create_event_queue();
    timer = al_create_timer(1/FPS);

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_hide_mouse_cursor(display);

	m_fullscreen = fullscreen;
    m_running = true;

    //Pointer Lists +
    networkList.clear();
    populationList.clear();
    //Pointer Lists -

    //Variables +
    versionNumber = "v1.0";

    fpsTimeNew = 0, fpsCounter = 0, fpsTimeOld = 0;

    drawScreen = false, timerEvent = false, done = false, mouseButtonLeft = false, mouseButtonLeftClick = false, mouseButtonRight = false, mouseButtonRightClick = false, updateTick = false, inGame = false;
    mouseX = 0, mouseY = 0;
    lastKeyPress = 0, mouseWheel = 0;

    score = 0, updateTickTime = 64, updateTickTimeHelper = 0;

    logicSpeed = 64, rockSpawnChance = 0, activationResponse = 0, neuronBias = 0, crossoverRate = 0, mutationRate = 0, maxPerturbation = 0;
    numHiddenLayers = 0, neuronsPerHiddenLayer = 0, numElite = 0, numCopiesElite = 0, populationSize = 0, currentNetwork = 0, currentGeneration = 0;

    for(int x = 0; x < mapArrayWidth; x++){
        for(int y = 0; y < mapArrayHeight; y++){
            mapArray[x][y] = 0;
            mapTileArray[x][y] = rand() % 8;
        }
    }

    al_start_timer(timer);
}

void Engine::cleanup(){
	// cleanup the all states

	while(!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// switch back to windowed mode so other
	// programs won't get accidentally resized
	if(m_fullscreen){
		al_set_new_display_flags(ALLEGRO_WINDOWED);
	}

    al_destroy_font(defaultFont);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
}

void Engine::changeState(GameState* state) {
	// cleanup the current state
	if(!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->cleanup();
	states.back()->init();
}

void Engine::pushState(GameState* state){
	// pause current state
	if (!states.empty()){
		states.back()->pause();
	}

	// store and init the new state
	states.push_back(state);
	states.back()->init();
}

void Engine::popState(){
	// cleanup the current state
	if (!states.empty()){
		states.back()->cleanup();
		states.pop_back();
	}

	// resume previous state
	if (!states.empty()){
		states.back()->resume();
	}
}

void Engine::update(){
	// let the state update the game
	states.back()->update(this);
}

void Engine::draw(){
	// let the state update the game
	drawScreen = true;
	states.back()->draw(this);
}
