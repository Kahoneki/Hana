#include "NeuralNetwork.h"

#include "Global.h"


namespace Hana
{

	NeuralNetwork::NeuralNetwork(const std::size_t _numInputs, const std::vector<std::size_t> _hiddenLayerSizes, const std::size_t _numOutputs)
	{
		m_layerSizes.push_back(_numInputs);
		for (std::size_t hiddenLayerSize : _hiddenLayerSizes)
		{
			m_layerSizes.push_back(hiddenLayerSize);
		}
		m_layerSizes.push_back(_numOutputs);

		m_neurons.resize(m_layerSizes.size());
		m_biases.resize(m_layerSizes.size());
		m_weights.resize(m_layerSizes.size()); //index 0 is unused because input layer doesn't have any weights
		for (std::size_t layer{ 0 }; layer < m_layerSizes.size(); ++layer)
		{
			m_neurons[layer].resize(m_layerSizes[layer]);
			m_biases[layer].resize(m_layerSizes[layer]);
			
			m_weights[layer].resize(m_layerSizes[layer]);
			if (layer != 0)
			{
				for (std::size_t neuron{ 0 }; neuron < m_weights[layer].size(); ++neuron)
				{
					m_weights[layer][neuron].resize(m_layerSizes[layer - 1]);
				}
			}
		}
	}



	void NeuralNetwork::Process(const std::vector<float>& _inputs, std::vector<float>& _outputs)
	{
		m_neurons[0] = _inputs;

		//Start at 1 because the first layer (index 0) doesn't have any weights
		for (std::size_t layerCounter{ 1 }; layerCounter < m_layerSizes.size(); ++layerCounter)
		{
			for (std::size_t neuronCounter{ 0 }; neuronCounter < m_layerSizes[layerCounter]; ++neuronCounter)
			{
				m_neurons[layerCounter][neuronCounter] = 0.0f;

				//Loop through all neurons in the previous layer
				for (std::size_t prevLayerNeuronCounter{ 0 }; prevLayerNeuronCounter < m_layerSizes[layerCounter - 1]; ++prevLayerNeuronCounter)
				{
					m_neurons[layerCounter][neuronCounter] += m_neurons[layerCounter - 1][prevLayerNeuronCounter] * m_weights[layerCounter][neuronCounter][prevLayerNeuronCounter];
				}
				m_neurons[layerCounter][neuronCounter] += m_biases[layerCounter][neuronCounter];
				m_neurons[layerCounter][neuronCounter] = std::tanh(m_neurons[layerCounter][neuronCounter]); //activation function
			}
		}

		_outputs = m_neurons[m_neurons.size() - 1];
	}



	void NeuralNetwork::Mutate(const float _mutationRate, const float _mutationStrength)
	{
		//Weight mutation
		//Start at 1 because the first layer (index 0) doesn't have any weights
		for (std::size_t layerCounter{ 1 }; layerCounter < m_layerSizes.size(); ++layerCounter)
		{
			for (std::size_t neuronCounter{ 0 }; neuronCounter < m_layerSizes[layerCounter]; ++neuronCounter)
			{
				//Loop through all neurons in the previous layer
				for (std::size_t prevLayerNeuronCounter{ 0 }; prevLayerNeuronCounter < m_layerSizes[layerCounter - 1]; ++prevLayerNeuronCounter)
				{
					if (Global::RandomFloat(0.0f, 1.0f) < _mutationRate)
					{
						const float mutationAmount{ Global::RandomFloat(-1, 1) * _mutationStrength };
						m_weights[layerCounter][neuronCounter][prevLayerNeuronCounter] += mutationAmount;
					}
				}
			}
		}

		//Bias mutation
		for (std::size_t layerCounter{ 1 }; layerCounter < m_layerSizes.size(); ++layerCounter)
		{
			for (std::size_t neuronCounter{ 0 }; neuronCounter < m_layerSizes[layerCounter]; ++neuronCounter)
			{
				if (Global::RandomFloat(0.0f, 1.0f) < _mutationRate)
				{
					const float mutationAmount{ Global::RandomFloat(-1, 1) * _mutationStrength };
					m_biases[layerCounter][neuronCounter] += mutationAmount;
				}
			}
		}
	}



	void NeuralNetwork::RandomiseWeights()
	{
		//Weights
		//Start at 1 because the first layer (index 0) doesn't have any weights
		for (std::size_t layerCounter{ 1 }; layerCounter < m_layerSizes.size(); ++layerCounter)
		{
			for (std::size_t neuronCounter{ 0 }; neuronCounter < m_layerSizes[layerCounter]; ++neuronCounter)
			{
				//Loop through all neurons in the previous layer
				for (std::size_t prevLayerNeuronCounter{ 0 }; prevLayerNeuronCounter < m_layerSizes[layerCounter - 1]; ++prevLayerNeuronCounter)
				{
					m_weights[layerCounter][neuronCounter][prevLayerNeuronCounter] = Global::RandomFloat(-1, 1);
				}
			}
		}
	}


	
	void NeuralNetwork::RandomiseBiases()
	{
		//Biases
		for (std::size_t layerCounter{ 1 }; layerCounter < m_layerSizes.size(); ++layerCounter)
		{
			for (std::size_t neuronCounter{ 0 }; neuronCounter < m_layerSizes[layerCounter]; ++neuronCounter)
			{
				m_biases[layerCounter][neuronCounter] = Global::RandomFloat(-1, 1);
			}
		}
	}

}