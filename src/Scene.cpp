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
		
		m_camera.setCenter({ m_racecar.GetPosition().x * Global::PIXELS_PER_METRE, m_racecar.GetPosition().y * Global::PIXELS_PER_METRE });
		m_camera.setSize({ 1280, 720 });
		m_camera.setRotation(sf::radians(b2Rot_GetAngle(m_racecar.GetRotation())));
		_window.setView(m_camera);
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
		m_track.FixedUpdate(m_world);
	}



	void Scene::Draw()
	{
		m_camera.setCenter({ m_racecar.GetPosition().x * Global::PIXELS_PER_METRE, m_racecar.GetPosition().y * Global::PIXELS_PER_METRE });
		m_camera.setRotation(sf::radians(b2Rot_GetAngle(m_racecar.GetRotation())));
		m_window.setView(m_camera);
		m_track.Render(m_window);
		m_racecar.Render(m_window);
	}

}