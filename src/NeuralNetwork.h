#pragma once

#include <vector>


namespace Hana
{

	class NeuralNetwork final
	{
	public:
		explicit NeuralNetwork() = default;
		explicit NeuralNetwork(const std::size_t _numInputs, const std::size_t _numOutputs);
		~NeuralNetwork() = default;

		void Process(const std::vector<float>& _inputs, std::vector<float>& _outputs) const;
		void Mutate(const float _mutationRate, const float _mutationStrength);
		void RandomiseWeights();
		void RandomiseBiases();
		

	private:
		std::vector<std::vector<float>> m_weights; //std::vector<float> contains all the weights for the inputs, each output has its own set of weights for all the inputs, hence each std::vector<float> corresponds to an output - std::vector<std::vector<float>>
		std::vector<float> m_biases; //added to the final output
	};
	
}