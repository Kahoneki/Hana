#pragma once

#include "Scene.h"
#include "TimeManager.h"

#include <algorithm>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>


namespace Hana
{
	template<std::derived_from<Scene> SceneType>
	class SimulationManager final
	{
	public:
		explicit inline SimulationManager(sf::RenderWindow& _window)
		: m_window(_window)
		{
			m_numAgents = 500;
			m_numInputs = 12;
			m_timePerGeneration = 100.0f;
			m_scene = std::make_unique<SceneType>(_window, m_numAgents, m_numInputs, m_timePerGeneration);
			m_timeSpeedupFactor = 1.0f;
			m_elitismRatio = 0.10f;
			m_mutationRate = 0.2f;
			m_mutationStrength = 0.15f;
			m_tournamentSize = 5;
			m_timestepAccumulator = 0.0f;

			m_physicsStepsPerGeneration = static_cast<std::uint32_t>(m_timePerGeneration / Global::FIXED_UPDATE_TIMESTEP);

			m_speedChangedLastFrame = false;
			m_currentGeneration = 0;
			m_physicsStepCounter = 0;
			if (!m_arialFont.openFromFile("Resource-Files/arial.ttf")) { throw std::runtime_error("Failed to load arial font."); }
		}
		~SimulationManager() = default;



		inline void Initialise()
		{
			//Start all agents with random weights and biases
			for (Agent& agent : m_scene->m_agents)
			{
				agent.m_neuralNetwork.RandomiseWeights();
				agent.m_neuralNetwork.RandomiseBiases();
				
				agent.m_racecar.Reset(m_scene->m_track);
			}
		}



		inline void Update()
		{
			TimeManager::Update();
			HandleInput();
			m_timestepAccumulator += TimeManager::GetDeltaTime() * m_timeSpeedupFactor;
			while (m_timestepAccumulator > Global::FIXED_UPDATE_TIMESTEP)
			{
				if (m_physicsStepCounter == m_physicsStepsPerGeneration)
				{
					//End of generation, evolve
					std::cout << "Generation " << m_currentGeneration << " complete" << std::endl;
					EvolvePopulation();
					++m_currentGeneration;

					//Reset simulation for next generation
					m_physicsStepCounter = 0;
					for (Agent& agent : m_scene->m_agents)
					{
						agent.Reset(m_scene->m_track);
					}
				}

				FixedUpdate();
				++m_physicsStepCounter;
				m_timestepAccumulator -= Global::FIXED_UPDATE_TIMESTEP;
			}
		}



		inline void Render()
		{
			m_scene->Draw();

			//ImGui
			ImGui::Begin("Options");

			ImGui::SeparatorText("Simulation Speed");
			if (ImGui::DragFloat("##Speed", &m_timeSpeedupFactor, 0.1f, 1.0f, 1000.0f, "%.2fx"))
			{
				m_timeSpeedupFactor = std::clamp(m_timeSpeedupFactor, 1.0f, 1000.0f);
			}
			
			ImGui::SeparatorText("Evolution Settings");
			ImGui::SliderFloat("Mutation Rate", &m_mutationRate, 0.0f, 1.0f, "%.4f");
			ImGui::SliderFloat("Mutation Strength", &m_mutationStrength, 0.0f, 2.0f, "%.4f");
			ImGui::SliderFloat("Elitism Ratio", &m_elitismRatio, 0.0f, 1.0f, "%.2f");
			int tournamentSizeSigned{ static_cast<int>(m_tournamentSize) }; //shoutout imgui for this one
			ImGui::SliderInt("Tournament Size", &tournamentSizeSigned, 0, 20);
			m_tournamentSize = tournamentSizeSigned;

			ImGui::SeparatorText("Stats");
			ImGui::Text("Current Generation: %zu", m_currentGeneration);
			ImGui::Text("Best Fitness: %.4f", m_scene->m_agents[0].m_fitness);
			ImGui::Text("Best Time: %.4f", m_scene->m_agents[0].m_currentTime);
			ImGui::Text("Worst Fitness: %.4f", m_scene->m_agents[m_scene->m_agents.size() - 1].m_fitness);

			ImGui::SeparatorText("Camera");
			if (m_scene->m_followBestAgent)
			{
				if (ImGui::Button("View All Agents"))
				{
					m_scene->m_followBestAgent = false;
				}
				
			}
			else
			{
				//No concept of the "best agent" on generation 0
				if (m_currentGeneration != 0)
				{
					if (ImGui::Button("View Best Agent"))
					{
						m_scene->m_followBestAgent = true;
					}
				}
			}

			ImGui::End();
			ImGui::SFML::Render(m_window);
		}
		

	private:
		inline void HandleInput()
		{
			const bool dec10{ sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_SPEED_COUNTER_10) };
			const bool dec{ sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_SPEED_COUNTER) };
			const bool inc{ sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER) };
			const bool inc10{ sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_SPEED_COUNTER_10) };
			if (dec10 XOR dec XOR inc XOR inc10)
			{
				if (!m_speedChangedLastFrame)
				{
					if (dec10)		{ m_timeSpeedupFactor -= 10.0f; }
					else if (dec)	{ m_timeSpeedupFactor -= 1.0f; }
					else if (inc)	{ m_timeSpeedupFactor += 1.0f; }
					else			{ m_timeSpeedupFactor += 10.0f; }

					m_timeSpeedupFactor = std::clamp(m_timeSpeedupFactor, 1.0f, 1000.0f);
					m_speedChangedLastFrame = true;
				}
			}
			else
			{
				m_speedChangedLastFrame = false;
			}
		}



		inline void FixedUpdate()
		{
			//Process the agents' neural networks
			for (Agent& agent : m_scene->m_agents)
			{
				const b2Vec2 pos{ agent.m_racecar.GetPosition() };
				const b2Rot angle{ agent.m_racecar.GetRotation() };
				const b2Vec2 linearVelocity{ agent.m_racecar.GetLocalLinearVelocity() };

				const std::uint32_t nextNodeIndex{ (agent.m_currentCheckpoint + 1) % m_scene->m_track.GetNumNodes() };
				const b2Vec2 nextCheckpointPos{ m_scene->m_track.GetNode(nextNodeIndex).centreWorldPosition };
				const b2Vec2 vecToNextCheckpoint{ nextCheckpointPos - pos };
				const float c = cos(-b2Rot_GetAngle(angle));
				const float s = sin(-b2Rot_GetAngle(angle));
				const b2Vec2 localVecToNextCheckpoint{
					vecToNextCheckpoint.x * c - vecToNextCheckpoint.y * s,
					vecToNextCheckpoint.x * s + vecToNextCheckpoint.y * c
				};
				const b2Vec2 directionToNextCheckpoint{ b2Normalize(localVecToNextCheckpoint) };
				
				std::vector<float> inputs(m_numInputs);
				inputs[0] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -1.0f, 0.0f });
				inputs[1] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -0.7f, -0.7f });
//				inputs[2] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { -0.38f, -0.92f });
				inputs[2] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.0f, -1.0f });
//				inputs[4] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.38f, -0.92f });
				inputs[3] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 0.7f, -0.7f });
				inputs[4] = 1.0f - agent.m_racecar.RaycastWithTrackForNeuralNetwork(m_scene->m_world, m_scene->m_track, { 1.0f, 0.0f });
				inputs[5] = linearVelocity.x / agent.m_racecar.GetMaxLinearVelocityMagnitude();
				inputs[6] = linearVelocity.y / agent.m_racecar.GetMaxLinearVelocityMagnitude();
				inputs[7] = agent.m_racecar.GetLocalAngularVelocity();
				inputs[8] = agent.m_racecar.GetInput(RACECAR_INPUT::ACCELERATION);
				inputs[9] = agent.m_racecar.GetInput(RACECAR_INPUT::STEERING);
				inputs[10] = directionToNextCheckpoint.x;
				inputs[11] = directionToNextCheckpoint.y;

				std::vector<float> outputs(2);
				agent.m_neuralNetwork.Process(inputs, outputs);

				agent.m_racecar.SetInput(RACECAR_INPUT::ACCELERATION, outputs[0]);
				agent.m_racecar.SetInput(RACECAR_INPUT::STEERING, outputs[1]);

				agent.m_racecar.FixedUpdate();
			}
			b2World_Step(m_scene->m_world, Global::FIXED_UPDATE_TIMESTEP, 4);

			//Update the fitness of all alive agents
			for (Agent& agent : m_scene->m_agents)
			{
//				if (agent.m_dead)
//				{
//					continue;
//				}

				const float oldFitness{ agent.m_fitness };
				agent.UpdateFitness(m_scene->m_track);

//				if (agent.m_fitness > oldFitness)
//				{
//					agent.m_timeSinceLastFitnessImprovement = 0.0f;
//				}
//				else
//				{
//					agent.m_timeSinceLastFitnessImprovement += Global::FIXED_UPDATE_TIMESTEP;
//				}
//				if (agent.m_timeSinceLastFitnessImprovement > agent.m_stuckTimeout)
//				{
//					agent.m_dead = true;
//				}
			}
		}



		inline void EvolvePopulation()
		{
			//Sort all agents by fitness (highest to lowest)
			std::sort(m_scene->m_agents.begin(), m_scene->m_agents.end(),
			[](const Agent& _a, const Agent& _b)
			{
				return _a.m_fitness > _b.m_fitness;
			});
			std::cout << "Best fitness: " << m_scene->m_agents[0].m_fitness << std::endl;
			std::cout << "Best time: " << m_scene->m_agents[0].m_currentTime << std::endl;
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
					const float agentFitness{ m_scene->m_agents[agentIndex].m_fitness };
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



		sf::RenderWindow& m_window;
		std::unique_ptr<SceneType> m_scene;

		std::uint32_t m_numAgents;
		std::uint32_t m_numInputs;
		float m_timePerGeneration; //in virtual seconds
		float m_timeSpeedupFactor;
		float m_timestepAccumulator;

		std::uint32_t m_physicsStepsPerGeneration;
		
		float m_elitismRatio; //percent of best performing agents in a generation that automatically go on to the next generation unchanged (range 0 to 1)
		float m_mutationRate; //percent chance for any given weight/bias to mutate on a neural network between generations
		float m_mutationStrength; //when mutating, weights/biases will be multiplied by a random value in the range (-1 to 1) * m_mutationStrength
		std::uint32_t m_tournamentSize; //when mutating an agent, this many agents will be randomly selected from the set of all agents, the best one (determined by fitness) will be used as the basis for the agent's new weights/biases

		bool m_speedChangedLastFrame;
		std::size_t m_currentGeneration;
		std::size_t m_physicsStepCounter;
		sf::Font m_arialFont;
	};

}