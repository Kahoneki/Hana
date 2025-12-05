#include "SimulationManager.h"
#include "TestScene.h"

#include <SFML/Graphics.hpp>




int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode({1280, 720}), "HanaAI", sf::Style::Default, sf::State::Windowed, sfmlContextSettings };
	
	Hana::SimulationManager<Hana::TestScene> simManager(window);
	simManager.Initialise();

	sf::Color backgroundColour{ 0xecb0e8ff };
	while (window.isOpen())
	{
		while (const std::optional<sf::Event> event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		simManager.Update();

		//uncomment for a good time
		//backgroundColour = sf::Color(backgroundColour.r + 255, backgroundColour.g + 255, backgroundColour.b + 255, 255);

		window.clear(backgroundColour);

		simManager.Render();
		
		window.display();
	}

	return 0;
}