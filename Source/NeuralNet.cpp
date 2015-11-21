#include "NeuralNet.h"
#include "Engine.h"

Neuron::Neuron(int numInputs): m_NumInputs(numInputs+1){
    for(int i = 0; i < numInputs+1; i++){
        m_vecWeight.push_back(randomClamped());
    }
}

NeuronLayer::NeuronLayer(int numNeurons, int numInputsPerNeuron): m_NumNeurons(numNeurons){
    for(int i = 0; i < numNeurons; i++){
        m_vecNeurons.push_back(Neuron(numInputsPerNeuron));
    }
}

NeuralNet::NeuralNet(){
    m_NumInputs = mapArrayWidth*mapArrayHeight;
    m_NumOutputs = 2;
    m_NumHiddenLayers = numHiddenLayers;
    m_NeuronsPerHiddenLayer = neuronsPerHiddenLayer;

    createNet();
}

void NeuralNet::createNet(){
    if(m_NumHiddenLayers > 0){
        m_vecLayers.push_back(NeuronLayer(m_NeuronsPerHiddenLayer, m_NumInputs)); //Create the layers of the network

        for(int i = 0; i < m_NumHiddenLayers-1; i++){
            m_vecLayers.push_back(NeuronLayer(m_NeuronsPerHiddenLayer, m_NeuronsPerHiddenLayer)); //Create first hidden layer
        }

        m_vecLayers.push_back(NeuronLayer(m_NumOutputs, m_NeuronsPerHiddenLayer)); //Create output layer
    }else{
        m_vecLayers.push_back(NeuronLayer(m_NumOutputs, m_NumInputs)); //Create output layer
    }
}

vector<double> NeuralNet::getWeights()const{
	vector<double> weights; //this will hold the weights

	for(int i = 0; i < m_NumHiddenLayers+1; i++){
		for(int j = 0; j < m_vecLayers[i].m_NumNeurons; j++){
			for(int k = 0; k < m_vecLayers[i].m_vecNeurons[j].m_NumInputs; k++){
				weights.push_back(m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k]);
			}
		}
	}

	return weights;
}

int NeuralNet::getNumberOfWeights()const{
	int weights = 0;

	for(int i = 0; i < m_NumHiddenLayers+1; i++){
		for(int j = 0; j < m_vecLayers[i].m_NumNeurons; j++){
			for(int k = 0; k < m_vecLayers[i].m_vecNeurons[j].m_NumInputs; k++){
				weights++;
			}
		}
	}

	return weights;
}

void NeuralNet::putWeights(vector<double> &weights){
    int cWeight = 0;
	for(int i = 0; i < m_NumHiddenLayers+1; i++){
		for(int j = 0; j < m_vecLayers[i].m_NumNeurons; j++){
			for(int k = 0; k < m_vecLayers[i].m_vecNeurons[j].m_NumInputs; k++){
				m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] = weights[cWeight++];
			}
		}
	}
}

vector<double> NeuralNet::update(vector<double> &inputs){
    vector<double> outputs;
    int weight = 0;

    if(inputs.size() != m_NumInputs){
        return outputs;
    }

    for(int i = 0; i < m_NumHiddenLayers+1; i++){
        if(i > 0){
            inputs = outputs;
        }

        outputs.clear();
        weight = 0;

        for(int j = 0; j < m_vecLayers[i].m_NumNeurons; j++){
            double netInput = 0;
            int numInputs =  m_vecLayers[i].m_vecNeurons[j].m_NumInputs ;

            for(int k = 0; k < numInputs - 1; k++){
                netInput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[k] * inputs[weight++];
            }

            netInput += m_vecLayers[i].m_vecNeurons[j].m_vecWeight[numInputs-1] * neuronBias;
            if(i < m_NumHiddenLayers){
                outputs.push_back(sigmoid(netInput, activationResponse));
            }else{
                outputs.push_back(netInput >= 0 ? 1 : 0);
            }
            weight = 0;
        }
    }

    return outputs;
}

double NeuralNet::sigmoid(double netinput, double response){
    return (1/(1+exp(-netinput/response)));
}
