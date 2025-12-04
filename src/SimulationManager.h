#pragma once

#include "Scene.h"
#include "TimeManager.h"

#include <algorithm>
#include <iostream>


namespace Hana
{
	template<std::derived_from<Scene> SceneType>
	class SimulationManager final
	{
	public:
		explicit inline SimulationManager(sf::RenderWindow& _window)
		: m_window(_window)
		{
			m_numAgents = 100;
			m_numInputs = 10;
			m_scene = std::make_unique<SceneType>(_window, m_numAgents, m_numInputs);
			m_maxGenerations = 500;
			m_timePerGeneration = 60.0f;
			m_timeSpeedupFactor = 1.0f;
			m_elitismRatio = 0.05f;
			m_mutationRate = 0.15f;
			m_mutationStrength = 0.3f;
			m_tournamentSize = 5;
			m_timestepAccumulator = 0.0f;

			m_speedChangedLastFrame = false;
			if (!m_arialFont.openFromFile("Resource-Files/arial.ttf")) { throw std::runtime_error("Failed to load arial font."); }
		}
		~SimulationManager() = default;


		
		inline void Run()
		{
			//Start all agents with random weights and biases
			for (Agent& agent : m_scene->m_agents)
			{
				agent.m_neuralNetwork.RandomiseWeights();
				agent.m_neuralNetwork.RandomiseBiases();
			}

			const std::uint32_t physicsStepsPerGeneration{ static_cast<std::uint32_t>(m_timePerGeneration / Global::FIXED_UPDATE_TIMESTEP) };
			for (std::size_t generationCounter{ 0 }; generationCounter < m_maxGenerations; ++generationCounter)
			{
				for (Agent& agent : m_scene->m_agents)
				{
					agent.Reset(m_scene->m_track);
				}
				m_timestepAccumulator = 0.0f;
				
				std::size_t physicsStepCounter{ 0 };
				while (physicsStepCounter < physicsStepsPerGeneration)
				{
					TimeManager::Update();
					m_timestepAccumulator += TimeManager::GetDeltaTime() * m_timeSpeedupFactor;
					while (m_timestepAccumulator > Global::FIXED_UPDATE_TIMESTEP && physicsStepCounter < physicsStepsPerGeneration)
					{
						//Process the agents' neural networks
						for (Agent& agent : m_scene->m_agents)
						{
							const b2Vec2 pos{ agent.m_racecar.GetPosition() };
							const b2Rot angle{ agent.m_racecar.GetRotation() };
							const b2Vec2 linearVelocity{ agent.m_racecar.GetLocalLinearVelocity() };

							std::vector<float> inputs(m_numInputs);
							inputs[0] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -1.0f, 0.0f });
							inputs[1] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -0.7f, -0.7f });
							inputs[2] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -0.38f, -0.92f });
							inputs[3] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.0f, -1.0f });
							inputs[4] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.38f, -0.92f });
							inputs[5] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.7f, -0.7f });
							inputs[6] = agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 1.0f, 0.0f });
							inputs[7] = linearVelocity.x / agent.m_racecar.GetMaxLinearVelocityMagnitude();
							inputs[8] = linearVelocity.y / agent.m_racecar.GetMaxLinearVelocityMagnitude();
							inputs[9] = agent.m_racecar.GetLocalAngularVelocity();

							std::vector<float> outputs(2);
							agent.m_neuralNetwork.Process(inputs, outputs);
							
							agent.m_racecar.SetInput(RACECAR_INPUT::ACCELERATION, outputs[0]);
							agent.m_racecar.SetInput(RACECAR_INPUT::STEERING, outputs[1]);

							agent.m_racecar.FixedUpdate();
						}
						b2World_Step(m_scene->m_world, Global::FIXED_UPDATE_TIMESTEP, 4);

						//Update the fitness of all agents
						for (Agent& agent : m_scene->m_agents)
						{
							agent.UpdateFitness(m_scene->m_track);
						}
						
						++physicsStepCounter;
						m_timestepAccumulator -= Global::FIXED_UPDATE_TIMESTEP;
					}
					
					if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_SPEED_COUNTER) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_SPEED_COUNTER_10) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER_10))
					{
						if (!m_speedChangedLastFrame)
						{
							m_timeSpeedupFactor += (sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_SPEED_COUNTER) ? -1 : (sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER) ? 1 : (sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER_10) ? 10 : -10)));
							m_timeSpeedupFactor = std::clamp(m_timeSpeedupFactor, 1.0f, 1000.0f);

							m_speedChangedLastFrame = true;
						}
					}
					else
					{
						m_speedChangedLastFrame = false;
					}

					//Render
					constexpr sf::Color backgroundColour{ 0xecb0e8ff };
					m_window.clear(backgroundColour);
					m_scene->Draw();

					const sf::String speedString{ "SPEED: " + std::to_string(m_timeSpeedupFactor) };
					const sf::Text speedText{ m_arialFont, speedString };
					m_window.draw(speedText);

					m_window.display();
				}

				std::cout << "Generation " << generationCounter << " complete" << std::endl;

				//Sort all agents by fitness (highest to lowest)
				std::sort(m_scene->m_agents.begin(), m_scene->m_agents.end(), 
				[](const Agent& a, const Agent& b)
				{
					return a.m_fitness > b.m_fitness;
				});
				std::cout << "Best fitness: " << m_scene->m_agents[0].m_fitness << std::endl;
				std::cout << "Worst fitness: " << m_scene->m_agents[m_scene->m_agents.size() - 1].m_fitness << std::endl;
				std::cout << std::endl;

				
				//Pass the best agents (amount based on the elitism ratio) to the next generation, neural network unmodified
				const std::uint32_t numAgentsToPassThroughUnmodified{ static_cast<std::uint32_t>(m_numAgents * m_elitismRatio) };

				//Go through all (non-excluded agents) and mutate their neural networks
				for (std::size_t i{ numAgentsToPassThroughUnmodified }; i < m_numAgents; ++i)
				{
					Agent& agent{ m_scene->m_agents[i] };
					
					//Tournament select from all agents based on fitness to get base weights/biases for new agent
					float bestFitnessFound{ -1.0f };
					std::size_t bestAgentIndex{ SIZE_MAX };
					for (std::uint32_t tournamentAgent{ 0 }; tournamentAgent < m_tournamentSize; ++tournamentAgent)
					{
						const std::size_t agentIndex{ Global::RandomIndex(0, m_numAgents - 1) };
						const std::uint32_t agentFitness{ static_cast<std::uint32_t>(m_scene->m_agents[agentIndex].m_fitness) };
						if (agentFitness > bestFitnessFound)
						{
							bestFitnessFound = agentFitness;
							bestAgentIndex = agentIndex;
						}
					}

					agent.m_neuralNetwork = m_scene->m_agents[bestAgentIndex].m_neuralNetwork;
					agent.m_neuralNetwork.Mutate(m_mutationRate, m_mutationStrength);
				}
			}
		}
		

	private:
		sf::RenderWindow& m_window;
		std::unique_ptr<SceneType> m_scene;

		std::uint32_t m_numAgents;
		std::uint32_t m_numInputs;
		std::uint32_t m_maxGenerations;
		float m_timePerGeneration; //in virtual seconds
		float m_timeSpeedupFactor;
		float m_timestepAccumulator;
		
		float m_elitismRatio; //percent of best performing agents in a generation that automatically go on to the next generation unchanged (range 0 to 1)
		float m_mutationRate; //percent chance for any given weight/bias to mutate on a neural network between generations
		float m_mutationStrength; //when mutating, weights/biases will be multiplied by a random value in the range (-1 to 1) * m_mutationStrength
		std::uint32_t m_tournamentSize; //when mutating an agent, this many agents will be randomly selected from the set of all agents, the best one (determined by fitness) will be used as the basis for the agent's new weights/biases

		bool m_speedChangedLastFrame;
		sf::Font m_arialFont;
	};

}