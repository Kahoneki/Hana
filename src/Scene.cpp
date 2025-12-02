#include "Scene.h"
#include "TimeManager.h"

#include <iostream>


namespace Hana
{

	Scene::Scene(sf::RenderWindow& _window)
	: m_window(_window)
	{
		b2WorldDef worldDef{ b2DefaultWorldDef() };
		worldDef.gravity = { 0, 0 }; //top-down so no gravity !!
		m_world = b2CreateWorld(&worldDef);
		m_racecar = std::move(Racecar(m_world));
		m_racecar.SetPosition({ _window.getSize().x / 2 * Global::METRES_PER_PIXELS, _window.getSize().y / 2 * Global::METRES_PER_PIXELS });
	}


	void Scene::Update()
	{
		TimeManager::Update();

		m_timestepAccumulator += TimeManager::GetDeltaTime();
		while (m_timestepAccumulator > Global::FIXED_UPDATE_TIMESTEP)
		{
			FixedUpdate();
			b2World_Step(m_world, Global::FIXED_UPDATE_TIMESTEP, 4);
			m_timestepAccumulator -= Global::FIXED_UPDATE_TIMESTEP;
		}
		
		Draw();
	}
	

	void Scene::FixedUpdate()
	{
		m_racecar.FixedUpdate();
	}



	void Scene::Draw()
	{
		m_racecar.Render(m_window);
	}

}