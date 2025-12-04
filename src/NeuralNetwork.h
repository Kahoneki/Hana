#pragma once

#include <vector>


namespace Hana
{

	class NeuralNetwork final
	{
	public:
		explicit NeuralNetwork() = default;
		explicit NeuralNetwork(const std::size_t _numInputs, const std::vector<std::size_t> _hiddenLayerSizes, const std::size_t _numOutputs);
		~NeuralNetwork() = default;

		void Process(const std::vector<float>& _inputs, std::vector<float>& _outputs);
		void Mutate(const float _mutationRate, const float _mutationStrength);
		void RandomiseWeights();
		void RandomiseBiases();
		

	private:
		std::vector<std::size_t> m_layerSizes;
		
		//1 vector per layer
		//1 float per neuron
		std::vector<std::vector<float>> m_neurons;
		std::vector<std::vector<float>> m_biases;

		//For any given neuron, the number of weights that contribute to that neuron is the number of neurons in the previous layer
		//Hence, for any given neuron, there is a vector of weights (std::vector<float>) for it
		//Hence, in any given layer, the weights for all neurons can be stored as an std::vector<std::vector<float>>
		//Hence, for the whole neural network, the weights of all neurons in all layers can be stored as an std::vector<std::vector<std::vector<float>>>
		std::vector<std::vector<std::vector<float>>> m_weights;
	};
	
}