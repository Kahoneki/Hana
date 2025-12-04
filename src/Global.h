#pragma once

#include <cstdint>
#include <random>
#include <SFML/Graphics.hpp>


#define SYNTACTIC_CONST const
//https://stackoverflow.com/a/1596681
#define XOR !=

namespace Hana
{
	struct Global
	{
	public:
		enum class PHYSICS_COLLISION_LAYER
		{
			TRACK	= 1 << 0,
			CAR		= 1 << 1,
		};
		
		
		static constexpr std::uint32_t PIXELS_PER_METRE{ 6u }; //How many SFML pixels there are for every 1 metre in Box2D
		static constexpr float METRES_PER_PIXELS{ 1.0f / PIXELS_PER_METRE }; //How many Box2D metres there are for every 1 SFML pixel

		static constexpr float FIXED_UPDATE_TIMESTEP{ 1 / 60.0f };

		
		static constexpr sf::Keyboard::Key SFML_KEY_ACCELERATE{ sf::Keyboard::Key::Up };
		static constexpr sf::Keyboard::Key SFML_KEY_BRAKE{ sf::Keyboard::Key::Down };
		static constexpr sf::Keyboard::Key SFML_KEY_TURN_LEFT{ sf::Keyboard::Key::Left };
		static constexpr sf::Keyboard::Key SFML_KEY_TURN_RIGHT{ sf::Keyboard::Key::Right };

		static constexpr sf::Keyboard::Key SFML_KEY_DECREMENT_TRACK_SUBDIVISIONS{ sf::Keyboard::Key::Num1 };
		static constexpr sf::Keyboard::Key SFML_KEY_INCREMENT_TRACK_SUBDIVISIONS{ sf::Keyboard::Key::Num2 };

		

		[[nodiscard]] static inline float RandomFloat(const float _min, const float _max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dist(_min, _max);
			return dist(gen);
		}



		[[nodiscard]] static inline std::size_t RandomIndex(const std::size_t _min, const std::size_t _max)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<std::size_t> dist(_min, _max);
			return dist(gen);
		}



		[[nodiscard]] static inline float InverseLerp(const float _a, const float _b, const float _v)
		{
			//Don't divide by zero
			if (std::abs(_b - _a) < 1e-5)
			{
				return 0.0f;
			}
			return (_v - _a) / (_b - _a);
		}
	};
}