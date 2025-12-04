#include "NeuralNetwork.h"

#include "Global.h"


namespace Hana
{

	NeuralNetwork::NeuralNetwork(const std::size_t _numInputs, const std::size_t _numOutputs)
	{
		m_weights.resize(_numOutputs);
		for (std::vector<float>& v : m_weights)
		{
			v.resize(_numInputs);
		}

		m_biases.resize(_numOutputs);
	}



	void NeuralNetwork::Process(const std::vector<float>& _inputs, std::vector<float>& _outputs) const
	{
		for (std::size_t i{ 0 }; i < _outputs.size(); ++i)
		{
			for (std::size_t j{ 0 }; j < _inputs.size(); ++j)
			{
				_outputs[i] += _inputs[j] * m_weights[i][j];
			}
			_outputs[i] += m_biases[i];
			_outputs[i] = std::tanh(_outputs[i]);
		}
	}



	void NeuralNetwork::Mutate(const float _mutationRate, const float _mutationStrength)
	{
		for (std::vector<float>& inputWeights : m_weights)
		{
			for (float& weight : inputWeights)
			{
				if (Global::RandomFloat(0, 1) < _mutationRate)
				{
					//Mutate this weight
					const float mutationAmount{ Global::RandomFloat(-1, 1) * _mutationStrength };
					weight += mutationAmount;
				}
			}
		}

		for (float& bias : m_biases)
		{
			if (Global::RandomFloat(0, 1) < _mutationRate)
			{
				//Mutate this bias
				const float mutationAmount{ Global::RandomFloat(-1, 1) * _mutationStrength };
				bias += mutationAmount;
			}
		}
	}



	void NeuralNetwork::RandomiseWeights()
	{
		for (std::vector<float>& inputWeights : m_weights)
		{
			for (float& weight : inputWeights)
			{
				weight = Global::RandomFloat(-1, 1);
			}
		}
	}


	
	void NeuralNetwork::RandomiseBiases()
	{
		for (float& bias : m_biases)
		{
			bias = Global::RandomFloat(-1, 1);
		}
	}

}