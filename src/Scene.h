#pragma once

#include "Racecar.h"
#include "Track.h"

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>


namespace Hana
{

	class Scene
	{
	public:
		virtual ~Scene() = default;

		virtual void Update() final;


	protected:
		explicit Scene(sf::RenderWindow& _window);
		
		virtual void FixedUpdate() final;
		virtual void Draw() final;


		float m_timestepAccumulator;
		
		sf::RenderWindow& m_window;
		b2WorldId m_world;

		sf::View m_camera;
		
		Racecar m_racecar;
		Track m_track;
	};
	
}