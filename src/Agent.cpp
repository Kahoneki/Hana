#include "Agent.h"


namespace Hana
{

	Agent::Agent(const std::size_t _numInputs, const b2WorldId _world)
		: m_neuralNetwork(_numInputs, { 16 }, 2)
	{
		m_racecar = std::move(Racecar(_world));
		m_fitness = 0.0f;
	}


	
	void Agent::Reset(const Track& _track)
	{
		m_racecar.Reset(_track);
		m_currentCheckpoint = 0;
		m_fitness = 0.0f;
	}

	

	void Agent::UpdateFitness(const Track& _track)
	{
		float newFitness{ m_fitness };
		
		const std::size_t nextCheckpoint{ (m_currentCheckpoint + 1) % _track.GetNumNodes() };

		//Check if the next checkpoint has been passed (distance to it is under a threshold)
		const float distanceToNextCheckpointThreshold{ _track.GetNode(nextCheckpoint).width };
		const float distanceToNextCheckpoint{ b2Distance(m_racecar.GetPosition(), _track.GetNode(nextCheckpoint).centreWorldPosition) };
		if (distanceToNextCheckpoint <= distanceToNextCheckpointThreshold)
		{
			//Next checkpoint has been passed
			m_currentCheckpoint = nextCheckpoint;
			newFitness = static_cast<std::uint32_t>(newFitness + 1);
		}
		else
		{
			//Get the amount the ai has managed to get to the next checkpoint and use it as the fractional part of the fitness
			const float distanceBetweenCurrentAndNextCheckpoint{ b2Distance(_track.GetNode(m_currentCheckpoint).centreWorldPosition, _track.GetNode(nextCheckpoint).centreWorldPosition) };
			const float fitnessFrac{ 1 - (distanceToNextCheckpoint / distanceBetweenCurrentAndNextCheckpoint) };
			newFitness = static_cast<std::uint32_t>(newFitness) + fitnessFrac;
		}

		//Only update the fitness if it's greater than the current one, i.e.: m_fitness stores the best fitness the agent managed to achieve this generation
		if (newFitness > m_fitness)
		{
			m_fitness = newFitness;
		}

		//Note: in the current way the fitness function has been set up, more fitness is inherently awarded to parts of the track with more curves (checkpoints / nodes) than straight sections
		//^This is actually fine as we want to award more fitness to harder sections of the map which will naturally be the more curvy sections
	}

}