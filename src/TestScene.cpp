#include "TestScene.h"


namespace Hana
{


	TestScene::TestScene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs, const float _timePerGeneration)
	: Scene(_window, _numAgents, _numInputs, _timePerGeneration)
	{
		m_track.AddNode({ {50.0f, 30.0f}, 22.0f });
		m_track.AddNode({ {100.0f, 30.0f}, 22.0f });
		m_track.AddNode({ {130.0f, 10.0f}, 20.0f });
		m_track.AddNode({ {160.0f, 50.0f}, 20.0f });
		m_track.AddNode({ {200.0f, 30.0f}, 22.0f });
		m_track.AddNode({ {250.0f, 30.0f}, 22.0f });

		m_track.AddNode({ {290.0f, 50.0f}, 20.0f });
		m_track.AddNode({ {320.0f, 30.0f}, 18.0f });
		m_track.AddNode({ {350.0f, 80.0f}, 18.0f });
		m_track.AddNode({ {330.0f, 130.0f}, 18.0f });
		m_track.AddNode({ {350.0f, 170.0f}, 18.0f });
		m_track.AddNode({ {300.0f, 200.0f}, 16.0f });
		m_track.AddNode({ {270.0f, 160.0f}, 16.0f });
		m_track.AddNode({ {240.0f, 140.0f}, 16.0f });
		m_track.AddNode({ {210.0f, 170.0f}, 16.0f });
		m_track.AddNode({ {230.0f, 210.0f}, 16.0f });
		m_track.AddNode({ {260.0f, 240.0f}, 18.0f });

		m_track.AddNode({ {280.0f, 280.0f}, 18.0f });
		m_track.AddNode({ {250.0f, 310.0f}, 18.0f });
		m_track.AddNode({ {270.0f, 350.0f}, 18.0f });
		m_track.AddNode({ {230.0f, 380.0f}, 20.0f });
		m_track.AddNode({ {180.0f, 400.0f}, 22.0f });
		m_track.AddNode({ {140.0f, 370.0f}, 20.0f });
		m_track.AddNode({ {100.0f, 410.0f}, 20.0f });
		m_track.AddNode({ {50.0f, 380.0f}, 22.0f });
		m_track.AddNode({ {0.0f, 400.0f}, 22.0f });
		m_track.AddNode({ {-40.0f, 370.0f}, 20.0f });

		m_track.AddNode({ {-80.0f, 380.0f}, 18.0f });
		m_track.AddNode({ {-120.0f, 350.0f}, 18.0f });
		m_track.AddNode({ {-90.0f, 310.0f}, 16.0f });
		m_track.AddNode({ {-130.0f, 280.0f}, 16.0f });
		m_track.AddNode({ {-160.0f, 300.0f}, 16.0f });
		m_track.AddNode({ {-180.0f, 260.0f}, 16.0f });
		m_track.AddNode({ {-140.0f, 230.0f}, 16.0f });
		m_track.AddNode({ {-170.0f, 200.0f}, 16.0f });
		m_track.AddNode({ {-190.0f, 160.0f}, 18.0f });

		m_track.AddNode({ {-150.0f, 130.0f}, 16.0f });
		m_track.AddNode({ {-110.0f, 150.0f}, 15.0f });
//		m_track.AddNode({ {-80.0f, 110.0f}, 15.0f });
		m_track.AddNode({ {-50.0f, 130.0f}, 15.0f });
//		m_track.AddNode({ {-20.0f, 90.0f}, 15.0f });
		m_track.AddNode({ {10.0f, 110.0f}, 16.0f });
//		m_track.AddNode({ {40.0f, 70.0f}, 18.0f });
		
		m_track.Construct(m_world);
	}
	
}