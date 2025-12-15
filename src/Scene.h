#pragma once

#include "Agent.h"
#include "Track.h"

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>


namespace Hana
{

	class Scene
	{
		template<std::derived_from<Scene> SceneType>
		friend class SimulationManager;
		
		
	public:
		virtual ~Scene() = default;

		virtual void Update() final;


	protected:
		explicit Scene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs, const float _timePerGeneration);
		
		virtual void FixedUpdate() final;
		virtual void Draw() final;


		float m_timestepAccumulator;
		
		sf::RenderWindow& m_window;
		b2WorldId m_world;

		sf::View m_camera;
		
		std::vector<Agent> m_agents;
		Track m_track;

		bool m_followBestAgent; //Assumes agents vector is sorted (gets sorted by simulation manager) - set by simulation manager
		float m_followBestAgentZoomLevel; //When following best agent, this changes the camera's zoom - set by simulation manager
		sf::Vector2u m_followBestAgentBaseCameraSize; //When following best agent, this is the base camera size that m_followBestAgentZoomLevel will modify
	};
	
}