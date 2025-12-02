#pragma once

#include "Scene.h"


namespace Hana
{

	class TestScene final : public Scene
	{
	public:
		explicit TestScene(sf::RenderWindow& _window);
		virtual ~TestScene() override = default;
	};
	
}