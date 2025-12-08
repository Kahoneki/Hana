#pragma once

#include "NeuralNetwork.h"
#include "Racecar.h"
#include "Track.h"


namespace Hana
{

	class Agent
	{
	public:
		explicit Agent() = default;
		explicit Agent(const std::size_t _numInputs, const b2WorldId _world);
		~Agent() = default;

		//Resets the racecar, current checkpoint, and fitness value
		//Leaves neural network untouched - this should be modified by a simulation manager
		void Reset(const Track& _track);
		
		void UpdateFitness(const Track& _track);
		
		Racecar m_racecar;
		NeuralNetwork m_neuralNetwork;
		float m_fitness;
		std::uint32_t m_currentCheckpoint;

		bool m_dead; //If true, this agent is skipped in the simulation
		float m_stuckTimeout; //The amount of time the car can be stuck for before m_dead getting set to true
		float m_timeSinceLastFitnessImprovement; //Tracks time since last fitness improvement, compared against m_stuckTimeout to set m_dead
	};
	
}