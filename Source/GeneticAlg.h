#ifndef GENETICALG_H
#define GENETICALG_H

#include "vector"

using namespace std;

struct Genome{
    vector <double>  vecWeights;
    double fitness;

    Genome():fitness(0){}

    Genome(vector <double> w, double f): vecWeights(w), fitness(f){}

    friend bool operator<(const Genome& lhs, const Genome& rhs){
		return (lhs.fitness < rhs.fitness);
	}
};

class GeneticAlg{
    public:
        GeneticAlg(int popSize, double mutationRate, double crossoverRate, int numWeights);
        vector<Genome> epoch(vector<Genome> &old_pop); //this runs the GA for one generation.
        vector<Genome> getChromosones()const{return m_vecPop;}
        double averageFitness()const{return m_TotalFitness/m_PopSize;}
        double bestFitness()const{return m_BestFitness;}

    private:
        vector <Genome> m_vecPop; //this holds the entire population of chromosomes
        int m_PopSize; //size of population
        int m_ChromosoneLength; //amount of weights per chromo
        double m_TotalFitness; //total fitness of population
        double m_BestFitness; //best fitness this population
        double m_AverageFitness; //average fitness
        double m_WorstFitness; //worst
        int m_FittestGenome; //keeps track of the best genome
        double m_MutationRate; //probability that a chromosomes bits will mutate. Try figures around 0.05 to 0.3 ish
        double m_CrossoverRate; //probability of chromosomes crossing over bits, 0.7 is pretty good
        int m_Generation; //generation counter

        void crossover(const vector<double> &mum, const vector<double> &dad, vector<double> &baby1, vector<double> &baby2);
        void mutate(vector<double> &chromosone);
        Genome getChromoRoulette();
        void grabNBest(int NBest, const int NumCopies, vector<Genome> &vecPop);
        void calculateBestWorstAvTot();
        void reset();
};

#endif // GENETICALGORITHM_H
