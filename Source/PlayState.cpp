#include "Engine.h"
#include "GameState.h"
#include "PlayState.h"

#include "NeuralNet.h"
#include "GeneticAlg.h"

PlayState PlayState::m_PlayState;

void PlayState::init(){

    //Create Player
    int width = 32, height = 32;
    playerList[0] = new Player();
    playerList[0]->setPos(mapArrayWidth/2, mapArrayHeight-1);
    playerList[0]->setDimensions(width, height);
    playerList[0]->setBitmap(playerImage);

    //Set some vital variables
    neuronsPerHiddenLayer = mapArrayWidth*mapArrayHeight;

    //Pointer Lists +
    networkList.clear();
    networkList.resize(populationSize);
    //Pointer Lists -

    m_outputs.assign(2, 0);

	geneticAlg = new GeneticAlg(populationSize, mutationRate, crossoverRate, networkList[0].getNumberOfWeights()); //Initialize the Genetic Algorithm class
	populationList = geneticAlg->getChromosones(); //Get the weights from the GA and insert into the sweepers brains


    al_stop_samples();
}

void PlayState::cleanup(){
}

void PlayState::pause(){
    inGame = true;
}

void PlayState::resume(){
}

void PlayState::update(Engine* engine){

    al_get_mouse_state(&mouseState);

    al_get_keyboard_state(&keyState);

    if(updateTickTimeHelper >= updateTickTime){
        updateTick = true;
        updateTickTimeHelper = 0;

        for(int x = 0; x < mapArrayWidth; x++){
            mapArray[x][mapArrayHeight-1] = 0;
        }

        //Neural net
        vector<double> m_inputs;
        for(int x = 0; x < mapArrayWidth; x++){
            for(int y = 0; y < mapArrayHeight; y++){
                if(y < mapArrayHeight-1){
                    m_inputs.push_back(mapArray[x][y] == 0 ? -10 : 10);
                }else{
                    m_inputs.push_back(playerList[0]->posX == x ? 10 : -10);
                }
            }
        }
        m_outputs = networkList[currentNetwork].update(m_inputs);

        if(m_outputs[0] >= 0.75){
            playerList[0]->deltaX += 1;
        }if(m_outputs[0] <= 0.25){
            playerList[0]->deltaX -= 1;
        }
    }

    //Player Input +
    if(al_key_down(&keyState, ALLEGRO_KEY_A)){
        if(lastKeyPress != ALLEGRO_KEY_A){
            updateTickTime /= 2;
            if(updateTickTime <= 0){
                updateTickTime = 1;
            }

            lastKeyPress = ALLEGRO_KEY_A;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_D)){
        if(lastKeyPress != ALLEGRO_KEY_D){
            if(updateTickTime < 256){
                updateTickTime *= 2;
            }

            lastKeyPress = ALLEGRO_KEY_D;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_COMMA)){
        if(lastKeyPress != ALLEGRO_KEY_COMMA){
            if(logicSpeed > 8){
                logicSpeed /= 2;
            }

            if(logicSpeed <= 0){
                logicSpeed = 1;
            }

            lastKeyPress = ALLEGRO_KEY_COMMA;
        }
    }else if(al_key_down(&keyState, ALLEGRO_KEY_FULLSTOP)){
        if(lastKeyPress != ALLEGRO_KEY_FULLSTOP){
            if(logicSpeed < 65536){
                logicSpeed *= 2;
            }

            lastKeyPress = ALLEGRO_KEY_FULLSTOP;
        }
    }
    //Player Input -

    //Update Entities +
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(playerList[i] != NULL && playerList[i]->checkActive()){
            playerList[i]->update();
        }
    }

    if(updateTick){
        int temp[mapArrayWidth][mapArrayHeight];
        int tempTile[mapArrayWidth][mapArrayHeight];
        for(int x = 0; x < mapArrayWidth; x++){
            for(int y = 1; y < mapArrayHeight; y++){
                temp[x][y] = mapArray[x][y-1];
                tempTile[x][y] = mapTileArray[x][y-1];
            }
        }
        for(int x = 0; x < mapArrayWidth; x++){
            for(int y = 0; y < mapArrayHeight; y++){
                mapArray[x][y] = temp[x][y];
                mapTileArray[x][y] = tempTile[x][y];
            }
        }
        for(int x = 0; x < mapArrayWidth; x++){
            if(RANDOM_NUM < rockSpawnChance){
                mapArray[x][0] = 1;
            }else{
                mapArray[x][0] = 0;
            }
            mapTileArray[x][0] = rand() % 8;
        }

        if(isPassable(playerList[0]->posX*tileSize, playerList[0]->posY*tileSize, playerList[0]->width, playerList[0]->height)){
            score++;
        }else{
            //Game over
            populationList[currentNetwork].fitness = score;
            currentNetwork++;
            if(currentNetwork >= populationSize){
                currentNetwork = 0;

                bestFitness = geneticAlg->bestFitness();
                averageFitness = geneticAlg->averageFitness();

                fitnessFile << currentGeneration << "|" << bestFitness << "|" << averageFitness << endl;

                currentGeneration++; //Increment the generation counter

                populationList = geneticAlg->epoch(populationList); //run the GA to create a new population

                for(int i = 0; i < populationSize; i++){
                    networkList[i].putWeights(populationList[i].vecWeights); //insert the new (hopefully)improved brains back into the sweepers and reset their positions etc

                    populationList[i].fitness = 0;
                }
            }

            playerList[0]->setPos(mapArrayWidth/2, mapArrayHeight-1);

            for(int x = 0; x < mapArrayWidth; x++){
                for(int y = 0; y < mapArrayHeight; y++){
                    mapArray[x][y] = 0;
                    mapTileArray[x][y] = rand() % 8;
                }
            }
            score = 0;
        }

        updateTick = false;
    }
    //Update Entities -

    //Rest +
    updateTickTimeHelper++;
    //Rest -

}

void PlayState::draw(Engine* engine){
    //Draw Map+
    drawMap();
    //Draw Map-

    //Draw Entities +
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(playerList[i] != NULL && playerList[i]->checkActive()){
            playerList[i]->draw();
        }
    }
    //Draw Entities -

    //Draw GUI +
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 0, NULL, "Score: %d", score);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 20, NULL, "Turn Update Time: %fs", updateTickTime/logicSpeed);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 40, NULL, "Updates per second: %fu/s", logicSpeed);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 60, NULL, "Movement: %s", m_outputs[0] < 0.75 && m_outputs[0] > 0.25 ? "Wait" : m_outputs[0] >= 0.75 ? "Right" : "Left");
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 80, NULL, "Current Network: %d", currentNetwork);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 100, NULL, "Current Generation: %d", currentGeneration);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 120, NULL, "Best Fitness Last Generation: %d", (int)bestFitness);
    al_draw_textf(defaultFont, al_map_rgb(50, 150, 50), mapArrayWidth*tileSize, 140, NULL, "Average Fitness Last Generation: %f", averageFitness);

    fpsTimeNew = al_get_time();
    fpsCounter = 1/(fpsTimeNew - fpsTimeOld);
    fpsTimeOld = fpsTimeNew;
    al_draw_textf(defaultFont, (fpsCounter > 55) ? al_map_rgb(50, 150, 50) : (fpsCounter <= 55 && fpsCounter > 30) ? al_map_rgb(150, 150, 50) : al_map_rgb(150, 50, 50), screenWidth-95, 0, NULL, "FPS: %d", (int)round(fpsCounter));
}
