#include "Racecar.h"

#include "TimeManager.h"

#include <iostream>


namespace Hana
{

	Racecar::Racecar(SYNTACTIC_CONST b2WorldId _world)
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
		m_tireGrip = 2.4f;
		m_currentSteeringAngle = 0.0f;
		m_skidMarkLifetime = 5.0f;
		m_skidMarksStartingOpacity = 100u;

		m_rollingResistanceDragMagnitude = m_rollingResistanceCoefficient * m_mass * 9.81f; //much more impactful at lower speeds
		m_maxLinearVelocityMagnitude = sqrt((m_engineForce - m_rollingResistanceDragMagnitude) / m_airDragCoefficient);

		m_skidMarks = sf::VertexArray(sf::PrimitiveType::Triangles);
		
		m_geometry = b2MakeBox(m_width / 2.0f, m_height / 2.0f);
		
		b2BodyDef bodyDef{ b2DefaultBodyDef() };
		bodyDef.type = b2_dynamicBody;
		bodyDef.linearDamping = m_linearDamping;
		bodyDef.angularDamping = m_angularDamping;
		m_physicsBody = b2CreateBody(_world, &bodyDef);

		b2ShapeDef materialDef{ b2DefaultShapeDef() };
		materialDef.density = m_mass / (m_width * m_height);
		materialDef.material.friction = m_friction;
		materialDef.filter.categoryBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::CAR);
		materialDef.filter.maskBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::TRACK);

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
		
		m_inputs[RACECAR_INPUT::ACCELERATION] = 0.0f;
		m_inputs[RACECAR_INPUT::STEERING] = 0.0f;
	}



	void Racecar::Reset(const Track& _track)
	{
		b2Body_SetTransform(m_physicsBody, _track.GetNode(0).centreWorldPosition, b2MakeRot(B2_PI / 2.0f));
		b2Body_SetLinearVelocity(m_physicsBody, { 0.0f, 0.0f });
		b2Body_SetAngularVelocity(m_physicsBody, 0.0f);
		b2Body_SetAwake(m_physicsBody, true);
		m_currentSteeringAngle = 0.0f;
		m_inputs[RACECAR_INPUT::ACCELERATION] = 0.0f;
		m_inputs[RACECAR_INPUT::STEERING] = 0.0f;
//		m_skidMarks.resize(0);
//		m_skidMarkTimeLeft.clear();
//		for (b2Vec2& pos : m_prevWheelPositions)
//		{
//			pos = { 0.0f, 0.0f };
//		}
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

//		if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_TURN_LEFT))
		//if (m_inputs[RACECAR_INPUT::STEERING] < -0.25f)
		//{
		//	m_currentSteeringAngle -= m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP;
		//}
//		else if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_TURN_RIGHT))
		//else if (m_inputs[RACECAR_INPUT::STEERING] > 0.25f)
		//{
			//m_currentSteeringAngle += m_inputs[RACECAR_INPUT::STEERING] * m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP;
		//}
		//else
		//{
		//	//Centre steering
		//	if (m_currentSteeringAngle > 0) { m_currentSteeringAngle -= m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP; }
		//	if (m_currentSteeringAngle < 0) { m_currentSteeringAngle += m_steeringSpeed * Global::FIXED_UPDATE_TIMESTEP; }

		//	//Snap to 0 if close
		//	if (std::abs(m_currentSteeringAngle) < 0.05f) { m_currentSteeringAngle = 0.0f; }
		//}
		//m_currentSteeringAngle = std::clamp(m_currentSteeringAngle, -m_maxSteeringAngle, m_maxSteeringAngle);

		m_currentSteeringAngle = m_inputs[RACECAR_INPUT::STEERING] * m_maxSteeringAngle;
		
//		if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_ACCELERATE) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_BRAKE))
		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			if (m_wheels[i].powered)
			{
				float wheelAngle = b2Rot_GetAngle(rot);
				if (m_wheels[i].steerable) { wheelAngle += m_currentSteeringAngle; }

				const b2Vec2 wheelForward{ sin(wheelAngle), -cos(wheelAngle) };
				const b2Vec2 wheelWorldPos{ b2Body_GetWorldPoint(m_physicsBody, m_wheels[i].localPosition) };
				const b2Vec2 force{ wheelForward * forcePerWheel };
//					b2Body_ApplyForce(m_physicsBody, sf::Keyboard::isKeyPressed(Global::SFML_KEY_ACCELERATE) ? force : -force, wheelWorldPos, true);
				b2Body_ApplyForce(m_physicsBody, m_inputs[RACECAR_INPUT::ACCELERATION] * force, wheelWorldPos, true);
			}
		}


		const b2Vec2 vel{ b2Body_GetLinearVelocity(m_physicsBody) };
		const float speed{ b2Length(vel) };
		if (speed > 0.1f)
		{
			//Apply drag
			const b2Vec2 dragDirection{ b2Normalize({ -vel.x, -vel.y }) };
			const float airDragMagnitude{ m_airDragCoefficient * speed * speed }; //proportional to v^2 for high speeds
			
			//Estimate the drag from engine gear back-force (since the car realistically isn't in neutral)
			//This will depend on the engine's max force, its efficiency, and how fast it's going relative to its max speed
			//Need max speed:
			//=> Max speed occurs when forces are balanced
			//=> F_engine = F_airResistance + F_rollingResistance
			//=> m_engineForce = (m_airDragCoefficient * vMax^2) + rollingResistanceDragMagnitude
			//=> vMax^2 = (m_engineForce - rollingResistanceDragMagnitude) / m_airDragCoefficient
			//=> vMax = sqrt((m_engineForce - rollingResistanceDragMagnitude) / m_airDragCoefficient)
			const float engineBackForce{ m_engineForce * m_engineInefficiencyCoefficient * (speed / m_maxLinearVelocityMagnitude) };
			
			const float totalDragMagnitude{ airDragMagnitude + m_rollingResistanceDragMagnitude + engineBackForce };
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

			//The tire can only support a certain amount of frictional force
			//If the friction received as a result of the current lateral speed exceeds this threshold, the tire will slide and the car will drift
			const float lateralSpeed{ b2Dot(wheelVel, wheelRightNorm) }; //component of velocity pointing in rightwards direction
			const float impulseMagnitudeForInfiniteGrip{ -lateralSpeed * massPerWheel }; //the impulse that would be required to remove all lateral velocity (infinite grip)
			const float maxImpulse{ massPerWheel * 50.0f * m_tireGrip * Global::FIXED_UPDATE_TIMESTEP }; //technically 30.0f is a bit of a hacky workaround to mimic downforce
			const float impulseMagnitude{ std::clamp(impulseMagnitudeForInfiniteGrip, -maxImpulse, maxImpulse) };
			
//			if (std::abs(impulseMagnitudeForInfiniteGrip) > maxImpulse)
//			{
//				//Car is drifting, add some skid marks
//				const sf::Color skidColour{ 20, 20, 20, m_skidMarksStartingOpacity };
//				constexpr float skidWidth{ 0.15f };
//				const b2Vec2 dir{ b2Normalize(wheelWorldPos - m_prevWheelPositions[i]) };
//				const b2Vec2 perp{ -dir.y, dir.x };
//				m_skidMarks.append(sf::Vertex({ (m_prevWheelPositions[i].x - perp.x * skidWidth) * Global::PIXELS_PER_METRE, (m_prevWheelPositions[i].y - perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//				m_skidMarks.append(sf::Vertex({ (wheelWorldPos.x - perp.x * skidWidth) * Global::PIXELS_PER_METRE, (wheelWorldPos.y - perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//				m_skidMarks.append(sf::Vertex({ (m_prevWheelPositions[i].x + perp.x * skidWidth) * Global::PIXELS_PER_METRE, (m_prevWheelPositions[i].y + perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//				
//				m_skidMarks.append(sf::Vertex({ (m_prevWheelPositions[i].x + perp.x * skidWidth) * Global::PIXELS_PER_METRE, (m_prevWheelPositions[i].y + perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//				m_skidMarks.append(sf::Vertex({ (wheelWorldPos.x - perp.x * skidWidth) * Global::PIXELS_PER_METRE, (wheelWorldPos.y - perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//				m_skidMarks.append(sf::Vertex({ (wheelWorldPos.x + perp.x * skidWidth) * Global::PIXELS_PER_METRE, (wheelWorldPos.y + perp.y * skidWidth) * Global::PIXELS_PER_METRE }, skidColour));
//
//				m_skidMarkTimeLeft.push_back(m_skidMarkLifetime);
//			}
//			m_prevWheelPositions[i] = wheelWorldPos;

			
			const b2Vec2 impulse{ wheelRightNorm * impulseMagnitude };
			b2Body_ApplyLinearImpulse(m_physicsBody, impulse, wheelWorldPos, true);
		}
	}



	void Racecar::Render(sf::RenderWindow& _window)
	{
//		//Update skid mark times
//		std::size_t skidMarksToRemoveFromArrayStart{ 0 };
//		for (std::size_t i{ 0 }; i < m_skidMarkTimeLeft.size(); ++i)
//		{
//			m_skidMarkTimeLeft[i] -= static_cast<float>(TimeManager::GetDeltaTime());
//			if (m_skidMarkTimeLeft[i] <= 0)
//			{
//				//This group of vertices needs to be removed
//				//Because vertices are added in order of when they are created, all vertices up to this point also need to be removed, so just updating the amount to remove from the start is fine
//				skidMarksToRemoveFromArrayStart = i + 1;
//			}
//		}
//		if (skidMarksToRemoveFromArrayStart != 0)
//		{
//			m_skidMarkTimeLeft.erase(m_skidMarkTimeLeft.begin(), m_skidMarkTimeLeft.begin() + skidMarksToRemoveFromArrayStart);
//
//			//Remove old vertices from vertex array by shifting all other vertices down and resizing array
//			const std::size_t verticesToRemoveFromArrayStart{ skidMarksToRemoveFromArrayStart * 6 };
//			for (std::size_t i{ 0 }; i < m_skidMarks.getVertexCount() - verticesToRemoveFromArrayStart; ++i)
//			{
//				m_skidMarks[i] = m_skidMarks[i + verticesToRemoveFromArrayStart];
//			}
//			m_skidMarks.resize(m_skidMarks.getVertexCount() - verticesToRemoveFromArrayStart);
//		}
//
//
//		//Update remaining skid mark opacities
//		for (std::size_t i{ 0 }; i < m_skidMarkTimeLeft.size(); ++i)
//		{
//			const float percentTimeLeftForCurrentGroupOfVertices{ m_skidMarkTimeLeft[i] / m_skidMarkLifetime }; //range 0-1
//			const std::uint8_t opacity{ static_cast<std::uint8_t>(std::lerp(0.0f, static_cast<float>(m_skidMarksStartingOpacity), percentTimeLeftForCurrentGroupOfVertices)) };
//			const std::size_t startingVertexIndex{ i * 6 };
//			for (std::size_t j{ startingVertexIndex }; j < startingVertexIndex + 6; ++j)
//			{
//				m_skidMarks[j].color.a = opacity;
//			}
//		}
//		
//		//Draw skid marks
//		_window.draw(m_skidMarks);
		
		
		//Draw chassis
		const b2Vec2 chassisPos{ b2Body_GetPosition(m_physicsBody) };
		const b2Rot chassisRot{ b2Body_GetRotation(m_physicsBody) };
		const float chassisRotAngleDeg{ b2Rot_GetAngle(chassisRot) * (180.0f / B2_PI) };

		sf::RectangleShape chassis{ sf::Vector2f(m_width * Global::PIXELS_PER_METRE, m_height * Global::PIXELS_PER_METRE) };
		chassis.setOrigin(chassis.getSize() / 2.0f); //Centre origin
		chassis.setPosition({ chassisPos.x * Global::PIXELS_PER_METRE, chassisPos.y * Global::PIXELS_PER_METRE });
		chassis.setRotation(sf::degrees(chassisRotAngleDeg));
		chassis.setFillColor(sf::Color::Red);

		_window.draw(chassis);


		//Draw wheels (20% of chassis width, 30% of chassis height)
		sf::RectangleShape wheel{ sf::Vector2f(0.2f * m_width * Global::PIXELS_PER_METRE, 0.3f * m_height * Global::PIXELS_PER_METRE) };
		wheel.setOrigin(wheel.getSize() / 2.0f); //Centre origin
		wheel.setFillColor(sf::Color::Black);
		
		for (std::size_t i{ 0 }; i < 4; ++i)
		{
			const b2Vec2 wheelWorldPos = b2Body_GetWorldPoint(m_physicsBody, m_wheels[i].localPosition);
			wheel.setPosition({ wheelWorldPos.x * Global::PIXELS_PER_METRE, wheelWorldPos.y * Global::PIXELS_PER_METRE });
			float rot = chassisRotAngleDeg;
			if (m_wheels[i].steerable)
			{
				rot += m_currentSteeringAngle * (180.0f / B2_PI);
			}
			wheel.setRotation(sf::degrees(rot));

			_window.draw(wheel);
		}
	}



	float Racecar::RaycastWithTrackForNeuralNetwork(const b2WorldId& _world, const Track& _track, const b2Vec2 _dir) const
	{
		constexpr float maxRayLength{ 50.0f };

		const b2Vec2 start{ GetPosition() };
		const b2Vec2 translation{ b2Normalize(b2Body_GetWorldVector(m_physicsBody, _dir)) * maxRayLength };
		b2QueryFilter filter{ b2DefaultQueryFilter() };
		filter.categoryBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::CAR);
		filter.maskBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::TRACK);

		const b2RayResult result{ b2World_CastRayClosest(_world, start, translation, filter) };

		return (result.hit) ? (1.0f - result.fraction) : (0.0f);
	}

}