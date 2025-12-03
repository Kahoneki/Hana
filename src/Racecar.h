#pragma once

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>

#include "Global.h"


namespace Hana
{

	struct Wheel
	{
		b2Vec2 localPosition; //Position relative to the car's centre
		bool powered; //Is it a drive wheel
		bool steerable; //Does the wheel turn
	};

	
	class Racecar final
	{
	public:
		explicit Racecar() = default;
		explicit Racecar(SYNTACTIC_CONST b2WorldId _world);
		~Racecar() = default;

		void FixedUpdate();
		void Render(sf::RenderWindow& _window) const;

		inline void SetPosition(const b2Vec2& _pos) SYNTACTIC_CONST { b2Body_SetTransform(m_physicsBody, _pos, b2Body_GetRotation(m_physicsBody)); }
		[[nodiscard]] inline b2Vec2 GetPosition() const { return b2Body_GetPosition(m_physicsBody); }


	private:
		float m_mass;
		float m_linearDamping;
		float m_angularDamping;
		float m_engineForce;
		float m_width;
		float m_height;
		float m_friction; //friction of car chassis, not wheels
		float m_airDragCoefficient;
		float m_rollingResistanceCoefficient;
		float m_engineBrakingForce;
		float m_engineInefficiencyCoefficient;
		float m_steeringSpeed; //rads per second
		float m_maxSteeringAngle; //rads
		float m_tireGrip; //0 (no grip) to 1 (full grip)

		b2Polygon m_geometry;
		b2BodyId m_physicsBody;
		std::array<Wheel, 4> m_wheels; //Front-left, Front-right, Back-left, Back-right

		float m_currentSteeringAngle;
	};
	
}