#include "SimulationManager.h"
#include "TestScene.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>




int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode({1280, 720}), "HanaAI", sf::Style::Default, sf::State::Windowed, sfmlContextSettings };
	ImGui::SFML::Init(window);

	Hana::SimulationManager<Hana::TestScene> simManager(window);
	simManager.Initialise();

	sf::Color backgroundColour{ 0xecb0e8ff };
	while (window.isOpen())
	{
		simManager.Update();
		ImGui::SFML::Update(window, sf::seconds(Hana::TimeManager::GetDeltaTime()));

		//uncomment for a good time
		//LEGAL NOTICE: If you believe yourself or those around you to be at risk of epileptic seizures, please consult a doctor or medical professional before using this software.
		//backgroundColour = sf::Color(Hana::Global::RandomIndex(0, 255), Hana::Global::RandomIndex(0, 255), Hana::Global::RandomIndex(0, 255), 255);

		window.clear(backgroundColour);

		simManager.Render();
		
		window.display();

		while (const std::optional<sf::Event> event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
				ImGui::SFML::Shutdown();
			}
		}
	}

	return 0;
}