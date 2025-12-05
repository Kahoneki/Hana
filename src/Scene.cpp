#include "Scene.h"
#include "TimeManager.h"

#include <iostream>


namespace Hana
{

	Scene::Scene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs)
	: m_window(_window)
	{
		b2WorldDef worldDef{ b2DefaultWorldDef() };
		worldDef.gravity = { 0, 0 }; //top-down so no gravity !!

		m_world = b2CreateWorld(&worldDef);

		m_agents.reserve(_numAgents);
		for (std::size_t i{ 0 }; i < _numAgents; ++i)
		{
			m_agents.emplace_back(_numInputs, m_world);
		}
		
		m_camera.setSize({ 4800, 2700 }); //16:9
		m_camera.setCenter({ 600, 1200 });
		m_followBestAgentBaseCameraSize = { 1280, 720 };
		_window.setView(m_camera);

		m_followBestAgent = false;
		m_followBestAgentZoomLevel = 1.0f;
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
		for (Agent& agent : m_agents)
		{
			agent.m_racecar.FixedUpdate();
		}
		m_track.FixedUpdate(m_world);
	}



	void Scene::Draw()
	{
		if (m_followBestAgent && m_agents.size() != 0)
		{
			m_camera.setSize({ m_followBestAgentBaseCameraSize.x * m_followBestAgentZoomLevel, m_followBestAgentBaseCameraSize.y * m_followBestAgentZoomLevel });
			m_camera.setCenter({ m_agents[0].m_racecar.GetPosition().x * Global::PIXELS_PER_METRE, m_agents[0].m_racecar.GetPosition().y * Global::PIXELS_PER_METRE });
			m_camera.setRotation(sf::radians(b2Rot_GetAngle(m_agents[0].m_racecar.GetRotation())));
		}
		else
		{
			m_camera.setSize({ 4800, 2700 }); //16:9
			m_camera.setCenter({ 600, 1200 });
			m_camera.setRotation(sf::Angle::Zero);
		}
		m_window.setView(m_camera);
		m_track.Render(m_window);
		for (Agent& agent : m_agents)
		{
			agent.m_racecar.Render(m_window);
		}
	}

}