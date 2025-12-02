#include "TimeManager.h"

#include <SFML/Window.hpp>


namespace Hana
{

	sf::Clock TimeManager::m_clock;
	
	void TimeManager::Update()
	{
		m_totalTime = m_clock.getElapsedTime().asSeconds();
		m_dt = m_totalTime - m_lastTime;
		m_lastTime = m_totalTime;
	}
	
}