#ifndef NEURALNET_H
#define NEURALNET_H

#include "vector"

using namespace std;

struct Neuron{

   int m_NumInputs;

   vector<double> m_vecWeight;

   Neuron(int numInputs);

};

struct NeuronLayer{

  int m_NumNeurons;

  vector<Neuron> m_vecNeurons;

  NeuronLayer(int numNeurons, int numInputsPerNeuron);
};

class NeuralNet{
    public:
        NeuralNet();
        void createNet();
        vector<double> getWeights()const;
        int getNumberOfWeights()const;
        void putWeights(vector<double> &weights);
        vector<double> update(vector<double> &inputs);
        inline double sigmoid(double activation, double response);

    private:
        int m_NumInputs;
        int m_NumOutputs;
        int m_NumHiddenLayers;
        int m_NeuronsPerHiddenLayer;
        vector<NeuronLayer> m_vecLayers;
};

#endif // NEURALNET_H
