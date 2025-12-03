#pragma once

#include <cstdint>
#include <SFML/Graphics.hpp>


#define SYNTACTIC_CONST const
//https://stackoverflow.com/a/1596681
#define XOR !=

namespace Hana
{
	struct Global
	{
	public:
		static constexpr std::uint32_t PIXELS_PER_METRE{ 6u }; //How many SFML pixels there are for every 1 metre in Box2D
		static constexpr float METRES_PER_PIXELS{ 1.0f / PIXELS_PER_METRE }; //How many Box2D metres there are for every 1 SFML pixel

		static constexpr float FIXED_UPDATE_TIMESTEP{ 1 / 60.0f };

		
		static constexpr sf::Keyboard::Key SFML_KEY_ACCELERATE{ sf::Keyboard::Key::W };
		static constexpr sf::Keyboard::Key SFML_KEY_BRAKE{ sf::Keyboard::Key::S };
		static constexpr sf::Keyboard::Key SFML_KEY_TURN_LEFT{ sf::Keyboard::Key::A };
		static constexpr sf::Keyboard::Key SFML_KEY_TURN_RIGHT{ sf::Keyboard::Key::D };

		static constexpr sf::Keyboard::Key SFML_KEY_DECREMENT_TRACK_SUBDIVISIONS{ sf::Keyboard::Key::Num1 };
		static constexpr sf::Keyboard::Key SFML_KEY_INCREMENT_TRACK_SUBDIVISIONS{ sf::Keyboard::Key::Num2 };
	};
}