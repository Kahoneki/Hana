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
		void Render(sf::RenderWindow& _window);

		inline void SetPosition(const b2Vec2& _pos) SYNTACTIC_CONST { b2Body_SetTransform(m_physicsBody, _pos, b2Body_GetRotation(m_physicsBody)); }
		inline void SetRotation(const b2Rot& _rot) SYNTACTIC_CONST { b2Body_SetTransform(m_physicsBody, b2Body_GetPosition(m_physicsBody), _rot); }
		[[nodiscard]] inline b2Vec2 GetPosition() const { return b2Body_GetPosition(m_physicsBody); }
		[[nodiscard]] inline b2Rot GetRotation() const { return b2Body_GetRotation(m_physicsBody); }


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
		float m_tireGrip;

		b2Polygon m_geometry;
		b2BodyId m_physicsBody;
		std::array<Wheel, 4> m_wheels; //Front-left, Front-right, Back-left, Back-right

		float m_currentSteeringAngle;

		//Skid marks
		sf::VertexArray m_skidMarks;
		std::vector<float> m_skidMarkTimeLeft; //For each group of 6 skid mark vertices in m_skidMarks, there is 1 entry in this vector for how long it has been alive. After m_skidMarkLifetime seconds, the skidmarks will be removed
		float m_skidMarkLifetime;
		std::uint8_t m_skidMarksStartingOpacity; //range 0-255
		std::array<b2Vec2, 4> m_prevWheelPositions;
	};
	
}