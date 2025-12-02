#include "TestScene.h"

#include <iostream>
#include <SFML/Graphics.hpp>


int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antialiasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode(800, 600), "HanaAI", sf::Style::Default, sfmlContextSettings };

	Hana::TestScene scene{ window };

	while (window.isOpen())
	{
		sf::Event event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		sf::Color backgroundColour{ 0xecb0e8ff };
		window.clear(backgroundColour);

		scene.Update();
		
		window.display();
	}

	return 0;
}