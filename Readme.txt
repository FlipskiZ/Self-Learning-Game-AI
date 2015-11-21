Controls:

A/D to change turn time (how long a turn takes)
,/. to change logic updates per second (how often the engine updates)

Both are used to speed/slow down the game, but they work a bit differently in the code. setting turn time to lowest and updates per second to highest yields the largest amount of ticks per second. the fastest the game will go, if your processor is fast enough for that it is. the larger the map the expontially more computation is required.

Logs:
The generation output is localized in logs/NNGA.txt

config.ini:

mapWidth - Width of the map
mapHeight - Height of the map
rockSpawnChance - Spawn chance of rocks where 1 is 100% and 0.01 is 1% (lower is accepted)
activationResponse - Steepness of the sigmoid function
neuronBias - The neuron bias
crossoverRate - The chance that two chromosomes will crossover 1 is 100% 0.01 is 1%
mutationRate - The chance that one value in a chromosome will change a little
maxPerturbation - The maximum amount that a value in a chromosome can change
numHiddenLayers - The amount of hidden layers, 1 is usually enough
neuronsPerHiddenLayer - The amount of neurons per hidden layer. 0 for a default value. This is usually good to tweak a bit
numElite - The amount of best chromosomes copied to the next generation
numCopiesElite - The amount of copies
populationSize - The population size
