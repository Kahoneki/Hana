#include "TestScene.h"

#include <iostream>
#include <SFML/Graphics.hpp>


int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode({1280, 720}), "HanaAI", sf::Style::Default, sf::State::Windowed, sfmlContextSettings };

	Hana::TestScene scene{ window };

	while (window.isOpen())
	{
		while (const std::optional<sf::Event> event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}
		constexpr sf::Color backgroundColour{ 0xecb0e8ff };
		window.clear(backgroundColour);

		scene.Update();
		
		window.display();
	}

	return 0;
}