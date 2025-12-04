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
		explicit Scene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs);
		
		virtual void FixedUpdate() final;
		virtual void Draw() final;


		float m_timestepAccumulator;
		
		sf::RenderWindow& m_window;
		b2WorldId m_world;

		sf::View m_camera;
		
		std::vector<Agent> m_agents;
		Track m_track;
	};
	
}