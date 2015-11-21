#include "GeneticAlg.h"
#include "Engine.h"

//-----------------------------------constructor-------------------------
//	sets up the population with random floats
//-----------------------------------------------------------------------
GeneticAlg::GeneticAlg(int popSize, double mutationRateRat, double crossoverRateRat, int numWeights) :
        m_PopSize(popSize),m_MutationRate(mutationRate), m_CrossoverRate(crossoverRate), m_ChromosoneLength(numWeights), m_TotalFitness(0),
        m_Generation(0), m_FittestGenome(0), m_BestFitness(0), m_WorstFitness(99999999), m_AverageFitness(0)
{
	for(int i = 0; i < m_PopSize; i++){ //initialise population with chromosomes consisting of random weights and all fitnesses set to zero
		m_vecPop.push_back(Genome());

		for(int j = 0; j < m_ChromosoneLength; j++){
			m_vecPop[i].vecWeights.push_back(randomClamped());
		}
	}
}


//---------------------------------Mutate--------------------------------
//	mutates a chromosome by perturbing its weights by an amount not greater than maxPerturbation
//-----------------------------------------------------------------------
void GeneticAlg::mutate(vector<double> &chromosone){
	//traverse the chromosome and mutate each weight dependent
	//on the mutation rate
	for (int i = 0; i < chromosone.size(); i++){
		if(randFloat() < m_MutationRate){
			chromosone[i] += (randomClamped() * maxPerturbation);
		}
	}
}

//----------------------------------GetChromoRoulette()------------------
//	returns a chromo based on roulette wheel sampling
//-----------------------------------------------------------------------
Genome GeneticAlg::getChromoRoulette(){
	double slice = (double)(randFloat() * m_TotalFitness); //generate a random number between 0 & total fitness count

	Genome theChosenOne; //this will be set to the chosen chromosome

	double fitnessSoFar = 0; //go through the chromosones adding up the fitness so far

	for(int i = 0; i < m_PopSize; i++){
		fitnessSoFar += m_vecPop[i].fitness;

		if(fitnessSoFar >= slice){ //if the fitness so far > random number return the chromo at this point
			theChosenOne = m_vecPop[i];
            break;
		}

	}
	return theChosenOne;
}

//-------------------------------------Crossover()-----------------------
//  given parents and storage for the offspring this method performs crossover according to the GAs crossover rate
//-----------------------------------------------------------------------
void GeneticAlg::crossover(const vector<double> &mum, const vector<double> &dad, vector<double> &baby1, vector<double> &baby2){
	if((randFloat() > m_CrossoverRate) || (mum == dad)){ //just return parents as offspring dependent on the rate or if parents are the same
        baby1 = mum;
		baby2 = dad;
		return;
	}

	int cp = randInt(0, m_ChromosoneLength - 1); //determine a crossover point

	for(int i = 0; i < cp; i++){ //create the offspring
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}

	for(int i = cp; i < mum.size(); i++){
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}
	return;
}

//-----------------------------------Epoch()-----------------------------
//	takes a population of chromosones and runs the algorithm through onem cycle.
//	Returns a new population of chromosones.
//-----------------------------------------------------------------------
vector<Genome> GeneticAlg::epoch(vector<Genome> &old_pop){

    m_vecPop = old_pop; //assign the given population to the classes population

    reset(); //reset the appropriate variables
    sort(m_vecPop.begin(), m_vecPop.end()); //sort the population (for scaling and elitism)
    calculateBestWorstAvTot(); //calculate best, worst, average and total fitness

    vector <Genome> vecNewPop; //create a temporary vector to store new chromosones

    //Now to add a little elitism we shall add in some copies of the fittest genomes. Make sure we add an EVEN number or the roulette wheel sampling will crash
    if(!(numCopiesElite * numElite % 2)){
        grabNBest(numElite, numCopiesElite, vecNewPop);
    }


    //now we enter the GA loopwhich repeats until a new population is generated
    while(vecNewPop.size() < m_PopSize){
        Genome mum = getChromoRoulette(); //grab two chromosones
        Genome dad = getChromoRoulette();

        vector<double> baby1, baby2; //create some offspring via crossover

        crossover(mum.vecWeights, dad.vecWeights, baby1, baby2);

        mutate(baby1); //now we mutate
        mutate(baby2);

        vecNewPop.push_back(Genome(baby1, 0)); //now copy into vecNewPop population
        vecNewPop.push_back(Genome(baby2, 0));
    }
    m_vecPop = vecNewPop;	 //finished so assign new pop back into m_vecPop

    return m_vecPop;
}


//-------------------------GrabNBest----------------------------------
//	This works like an advanced form of elitism by inserting NumCopies
//  copies of the NBest most fittest genomes into a population vector
//--------------------------------------------------------------------
void GeneticAlg::grabNBest(int NBest, const int	NumCopies, vector<Genome> &Pop){
    //add the required amount of copies of the n most fittest to the supplied vector
	while(NBest--){
		for(int i = 0; i < NumCopies; i++){
			Pop.push_back(m_vecPop[(m_PopSize - 1) - NBest]);
	  }
	}
}

//-----------------------CalculateBestWorstAvTot-----------------------
//	calculates the fittest and weakest genome and the average/total fitness scores
//---------------------------------------------------------------------
void GeneticAlg::calculateBestWorstAvTot(){
	m_TotalFitness = 0;

	double highestSoFar = 0;
	double lowestSoFar  = 9999999;

	for(int i = 0; i < m_PopSize; i++){
		if (m_vecPop[i].fitness > highestSoFar){ //update fittest if necessary
			highestSoFar = m_vecPop[i].fitness;

			m_FittestGenome = i;

			m_BestFitness = highestSoFar;
		}

		if (m_vecPop[i].fitness < lowestSoFar){ //update worst if necessary
			lowestSoFar = m_vecPop[i].fitness;
			m_WorstFitness = lowestSoFar;
		}

		m_TotalFitness += m_vecPop[i].fitness;
	}
	m_AverageFitness = m_TotalFitness/m_PopSize;
}

//-------------------------Reset()------------------------------
//	resets all the relevant variables ready for a new generation
//--------------------------------------------------------------
void GeneticAlg::reset(){
	m_TotalFitness		= 0;
	m_BestFitness		= 0;
	m_WorstFitness		= 9999999;
	m_AverageFitness	= 0;
}

