#include "Racecar.h"

#include <iostream>

#include "Global.h"


namespace Hana
{

	Racecar::Racecar(const b2WorldId _world)
	{
		m_mass = 1250.0f;

		//Not actually used, just left quite small for some tiny levels of resistance at small velocities so it can come to a stop
		m_linearDamping = 0.01f;
		m_angularDamping = 0.01f;

		m_engineForce = 34'000.0f;
		m_width = 2.0f;
		m_height = 4.0f;
		m_friction = 0.5f;
		m_airDragCoefficient = 4.2f;
		m_rollingResistanceCoefficient = 0.03f;
		m_engineInefficiencyCoefficient = 1.0f;
		m_steeringSpeed = 0.6f;
		m_maxSteeringAngle = 0.2f;
		m_tireGrip = 0.8f;
		m_currentSteeringAngle = 0.0f;
		
		m_geometry = b2MakeBox(m_width / 2.0f, m_height / 2.0f);
		
		b2BodyDef bodyDef{ b2DefaultBodyDef() };
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearDamping = m_linearDamping;
		bodyDef.angularDamping = m_angularDamping;
		m_physicsBody = b2CreateBody(_world, &bodyDef);

		b2ShapeDef materialDef{ b2DefaultShapeDef() };
		materialDef.density = m_mass / (m_width * m_height);
		materialDef.material.friction = m_friction;

		b2CreatePolygonShape(m_physicsBody, &materialDef, &m_geometry);
		
		m_wheels[0].localPosition = { -(m_width / 2.0f), -(m_height / 2.0f - m_height * 0.06f) }; //Front-left
		m_wheels[1].localPosition = {  (m_width / 2.0f), -(m_height / 2.0f - m_height * 0.06f) }; //Front-right
		m_wheels[2].localPosition = { -(m_width / 2.0f),  (m_height / 2.0f - m_height * 0.06f) }; //Back-left
		m_wheels[3].localPosition = {  (m_width / 2.0f),  (m_height / 2.0f - m_height * 0.06f) }; //Back-right

		//Front-wheel steering
		m_wheels[0].steerable = true;
		m_wheels[1].steerable = true;
		m_wheels[2].steerable = false;
		m_wheels[3].steerable = false;

		//Rear-wheel drive
		m_wheels[0].powered = false;
		m_wheels[1].powered = false;
		m_wheels[2].powered = true;
		m_wheels[3].powered = true;
		
	}



	void Racecar::FixedUpdate()
	{
		const b2Rot rot{ b2Body_GetRotation(m_physicsBody) };

		//Figure out how many wheels the engine force is split between
		std::uint32_t divider{ 0u };
		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			if (m_wheels[i].powered) { ++divider; }
		}
		const float forcePerWheel{ m_engineForce / divider };

		if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_TURN_LEFT))
		{
			m_currentSteeringAngle -= m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP;
		}
		else if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_TURN_RIGHT))
		{
			m_currentSteeringAngle += m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP;
		}
		else
		{
			//Centre steering
			if (m_currentSteeringAngle > 0) { m_currentSteeringAngle -= m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP; }
			if (m_currentSteeringAngle < 0) { m_currentSteeringAngle += m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP; }

			//Snap to 0 if close
			if (std::abs(m_currentSteeringAngle) < 0.05f) { m_currentSteeringAngle = 0.0f; }
		}

		m_currentSteeringAngle = std::clamp(m_currentSteeringAngle, -m_maxSteeringAngle, m_maxSteeringAngle);
		
		if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_ACCELERATE) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_BRAKE))
		{
			for (std::size_t i{ 0 }; i < 4; ++i)
			{
				if (m_wheels[i].powered)
				{
					float wheelAngle = b2Rot_GetAngle(rot);
					if (m_wheels[i].steerable) { wheelAngle += m_currentSteeringAngle; }

					const b2Vec2 wheelForward{ sin(wheelAngle), -cos(wheelAngle) };
					const b2Vec2 wheelWorldPos{ b2Body_GetWorldPoint(m_physicsBody, m_wheels[i].localPosition) };
					const b2Vec2 force{ wheelForward * forcePerWheel };
					b2Body_ApplyForce(m_physicsBody, sf::Keyboard::isKeyPressed(Global::SFML_KEY_ACCELERATE) ? force : -force, wheelWorldPos, true);
				}
			}
		}


		const b2Vec2 vel{ b2Body_GetLinearVelocity(m_physicsBody) };
		const float speed{ b2Length(vel) };
		if (speed > 0.1f)
		{
			//Apply drag
			const b2Vec2 dragDirection{ b2Normalize({ -vel.x, -vel.y }) };
			const float airDragMagnitude{ m_airDragCoefficient * speed * speed }; //proportional to v^2 for high speeds
			const float rollingResistanceDragMagnitude{ m_rollingResistanceCoefficient * m_mass * 9.81f }; //much more impactful at lower speeds

			//Estimate the drag from engine gear back-force (since the car realistically isn't in neutral)
			//This will depend on the engine's max force, its efficiency, and how fast it's going relative to its max speed
			//Need max speed:
			//=> Max speed occurs when forces are balanced
			//=> F_engine = F_airResistance + F_rollingResistance
			//=> m_engineForce = (m_airDragCoefficient * vMax^2) + rollingResistanceDragMagnitude
			//=> vMax^2 = (m_engineForce - rollingResistanceDragMagnitude) / m_airDragCoefficient
			//=> vMax = sqrt((m_engineForce - rollingResistanceDragMagnitude) / m_airDragCoefficient)
			const float vMax{ sqrt((m_engineForce - rollingResistanceDragMagnitude) / m_airDragCoefficient) };
			const float engineBackForce{ m_engineForce * m_engineInefficiencyCoefficient * (speed / vMax) };

			std::cout << "ENGINE BACK FORCE: " << engineBackForce << std::endl;
			
			const float totalDragMagnitude{ airDragMagnitude + rollingResistanceDragMagnitude + engineBackForce };
			b2Body_ApplyForceToCenter(m_physicsBody, totalDragMagnitude * dragDirection, true);
		}

		
		//Apply lateral velocity damping for the wheels
		
		//Distribute the mass over each wheel appropriately
		const float massPerWheel{ m_mass / 4.0f };

		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			float wheelAngle{ b2Rot_GetAngle(rot) };
			if (m_wheels[i].steerable) { wheelAngle += m_currentSteeringAngle; }

			const b2Vec2 wheelRightNorm{ cos(wheelAngle), sin(wheelAngle) };
			const b2Vec2 wheelWorldPos{ b2Body_GetWorldPoint(m_physicsBody, m_wheels[i].localPosition) };
			const b2Vec2 wheelVel{ b2Body_GetWorldPointVelocity(m_physicsBody, wheelWorldPos) };
			const float lateralSpeed{ b2Dot(wheelVel, wheelRightNorm) }; //component of velocity pointing in rightwards direction
			const float impulseMagnitude{ -lateralSpeed * massPerWheel * m_tireGrip }; //multiply by grip to allow some sick drifts
			const b2Vec2 impulse{ wheelRightNorm * impulseMagnitude };
			b2Body_ApplyLinearImpulse(m_physicsBody, impulse, wheelWorldPos, true);
		}
	}



	void Racecar::Render(sf::RenderWindow& _window) const
	{
		//Draw chassis
		const b2Vec2 chassisPos{ b2Body_GetPosition(m_physicsBody) };
		const b2Rot chassisRot{ b2Body_GetRotation(m_physicsBody) };
		const float chassisRotAngleDeg{ b2Rot_GetAngle(chassisRot) * (180.0f / B2_PI) };

		sf::RectangleShape chassis{ sf::Vector2f(m_width * Global::PIXELS_PER_METRE, m_height * Global::PIXELS_PER_METRE) };
		chassis.setOrigin(chassis.getSize() / 2.0f); //Centre origin
		chassis.setPosition(chassisPos.x * Global::PIXELS_PER_METRE, chassisPos.y * Global::PIXELS_PER_METRE);
		chassis.setRotation(chassisRotAngleDeg);
		chassis.setFillColor(sf::Color::Red);

		_window.draw(chassis);


		//Draw wheels (20% of chassis width, 30% of chassis height)
		sf::RectangleShape wheel{ sf::Vector2f(0.2f * m_width * Global::PIXELS_PER_METRE, 0.3f * m_height * Global::PIXELS_PER_METRE) };
		wheel.setOrigin(wheel.getSize() / 2.0f); //Centre origin
		wheel.setFillColor(sf::Color::Black);
		
		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			b2Vec2 wheelWorldPos = b2Body_GetWorldPoint(m_physicsBody, m_wheels[i].localPosition);
			wheel.setPosition(wheelWorldPos.x * Global::PIXELS_PER_METRE, wheelWorldPos.y * Global::PIXELS_PER_METRE);
			float rot = chassisRotAngleDeg;
			if (m_wheels[i].steerable)
			{
				rot += m_currentSteeringAngle * (180.0f / B2_PI);
			}
			wheel.setRotation(rot);

			_window.draw(wheel);
		}
	}
}