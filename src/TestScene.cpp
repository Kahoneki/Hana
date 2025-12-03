#include "TestScene.h"


namespace Hana
{

	TestScene::TestScene(sf::RenderWindow& _window)
	: Scene(_window)
	{
		m_racecar.SetPosition({ 50.0f, 30.0f });

		m_track.AddNode({ {50.0f, 30.0f}, 15.0f });
		m_track.AddNode({ {150.0f, 30.0f}, 15.0f });
		m_track.AddNode({ {180.0f, 60.0f}, 15.0f });
		m_track.AddNode({ {150.0f, 90.0f}, 15.0f });
		m_track.AddNode({ {50.0f, 90.0f}, 15.0f });
		m_track.AddNode({ {20.0f, 60.0f}, 15.0f });
		m_track.Construct(m_world);
	}
	
}