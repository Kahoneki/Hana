#include "SimulationManager.h"
#include "TestScene.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>




int main()
{

	
	sf::ContextSettings sfmlContextSettings{};
	sfmlContextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window{ sf::VideoMode({1920, 1080}), "HanaAI", sf::Style::Default, sf::State::Windowed, sfmlContextSettings };
	ImGui::SFML::Init(window);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("Resource-Files/arial.ttf", 30.0f);
	ImGui::SFML::UpdateFontTexture();

	Hana::SimulationManager<Hana::TestScene> simManager(window);
	simManager.Initialise();

	sf::Color backgroundColour{ 0xecb0e8ff };
	float baseWindowWidthForFontScaling{ 3840 };
	float fontScaleFactor{ static_cast<float>(window.getSize().x / baseWindowWidthForFontScaling) };
	io.FontGlobalScale = fontScaleFactor;
	while (window.isOpen())
	{
		while (const std::optional<sf::Event> event{ window.pollEvent() })
		{
			ImGui::SFML::ProcessEvent(window, *event);
			if (event->is<sf::Event::Closed>())
			{
				window.close();
				ImGui::SFML::Shutdown();
				exit(0);
			}
			else if (event->is<sf::Event::Resized>())
			{
				float fontScaleFactor{ static_cast<float>(window.getSize().x / baseWindowWidthForFontScaling) };
				io.FontGlobalScale = fontScaleFactor;
			}
		}

		simManager.Update();
		ImGui::SFML::Update(window, sf::seconds(Hana::TimeManager::GetDeltaTime()));

		//uncomment for a good time
		//LEGAL NOTICE: If you believe yourself or those around you to be at risk of epileptic seizures, please consult a doctor or medical professional before using this software.
		//backgroundColour = sf::Color(Hana::Global::RandomIndex(0, 255), Hana::Global::RandomIndex(0, 255), Hana::Global::RandomIndex(0, 255), 255);

		window.clear(backgroundColour);

		simManager.Render();
		
		window.display();
	}

	return 0;
}