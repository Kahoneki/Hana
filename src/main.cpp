#include "SimulationManager.h"
#include "TestScene.h"

#include <SFML/Graphics.hpp>




int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode({1280, 720}), "HanaAI", sf::Style::Default, sf::State::Windowed, sfmlContextSettings };
	
	Hana::SimulationManager<Hana::TestScene> simManager(window);
	simManager.Run();

	return 0;
}