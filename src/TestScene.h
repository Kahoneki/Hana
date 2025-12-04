#pragma once

#include "Scene.h"


namespace Hana
{

	class TestScene final : public Scene
	{
	public:
		explicit TestScene(sf::RenderWindow& _window, const std::size_t _numAgents, const std::size_t _numInputs);
		virtual ~TestScene() override = default;
	};
	
}