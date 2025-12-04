#include "TestScene.h"


namespace Hana
{

	TestScene::TestScene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs)
	: Scene(_window, _numAgents, _numInputs)
	{
		m_track.AddNode({ {50.0f, 30.0f}, 20.0f });    //Start Line
		m_track.AddNode({ {150.0f, 30.0f}, 20.0f });   //Main Straight
		m_track.AddNode({ {250.0f, 30.0f}, 20.0f });   //Speed Trap
		m_track.AddNode({ {300.0f, 60.0f}, 22.0f });   //Wide Entry
		m_track.AddNode({ {320.0f, 120.0f}, 18.0f });  //Turn 1 (Sweeping Right)
		m_track.AddNode({ {300.0f, 180.0f}, 16.0f });  //Tightening Exit
		m_track.AddNode({ {200.0f, 200.0f}, 16.0f });  //Connector
		m_track.AddNode({ {150.0f, 250.0f}, 15.0f });  //Chicane Entry
		m_track.AddNode({ {180.0f, 290.0f}, 15.0f });  //Chicane Apex
		m_track.AddNode({ {140.0f, 330.0f}, 15.0f });  //Chicane Exit
		m_track.AddNode({ {100.0f, 350.0f}, 18.0f });  //Preparation
		m_track.AddNode({ {50.0f, 400.0f}, 25.0f });   //Back Straight Start (Wide)
		m_track.AddNode({ {-100.0f, 400.0f}, 25.0f }); //Long High Speed Section
		m_track.AddNode({ {-150.0f, 350.0f}, 22.0f }); //Banked Turn Entry
		m_track.AddNode({ {-150.0f, 200.0f}, 22.0f }); //Long Left Hander
		m_track.AddNode({ {-100.0f, 100.0f}, 20.0f }); //Return Leg
		m_track.AddNode({ {-20.0f, 50.0f}, 18.0f });   //Final Hairpin Entry
		m_track.AddNode({ {10.0f, 20.0f}, 18.0f });    //Final Hairpin Apex
		m_track.Construct(m_world);
	}
	
}