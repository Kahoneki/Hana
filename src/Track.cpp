#include "Track.h"


namespace Hana
{

	Track::Track()
	{
		m_wallFriction = 0.5f;
		m_subdivisions = 8;
		if (!m_arialFont.openFromFile("Resource-Files/arial.ttf")) { throw std::runtime_error("Failed to load arial font."); }
		m_subdivisionCountChangedLastFrame = false;
		m_chainBody = b2_nullBodyId;
	}



	void Track::Construct(SYNTACTIC_CONST b2WorldId& _world)
	{
		if (B2_IS_NON_NULL(m_chainBody))
		{
			b2DestroyBody(m_chainBody);
			m_chainBody = b2_nullBodyId;
		}
		
		//Populate vertices
		m_triangleStripVertices.clear();
		m_innerChainVertices.clear();
		m_outerChainVertices.clear();
		m_triangleStripVertices = sf::VertexArray(sf::PrimitiveType::TriangleStrip);
		
		for (std::size_t i{ 0 }; i < m_nodes.size(); ++i)
		{
			//Get 4 points required for catmull rom spline sample
			//p0 = prev, p1 = current, p2 = next, p3 = next-next
			const b2Vec2& p0{ m_nodes[(i + m_nodes.size() - 1) % m_nodes.size()].centreWorldPosition };
			const b2Vec2& p1{ m_nodes[i].centreWorldPosition };
			const b2Vec2& p2{ m_nodes[(i + 1) % m_nodes.size()].centreWorldPosition };
			const b2Vec2& p3{ m_nodes[(i + 2) % m_nodes.size()].centreWorldPosition };

			//Also interpolate widths across the control points
			const float w1{ m_nodes[i].width };
			const float w2{ m_nodes[(i + 1) % m_nodes.size()].width };

			for (std::uint32_t step{ 0 }; step < m_subdivisions + 1; ++step)
			{
				const float t{ static_cast<float>(step) / (m_subdivisions + 1) };
				const b2Vec2 splinePos{ GetSplinePoint(p0, p1, p2, p3, t) };

				//Calculate position slightly ahead to get the normal
				const b2Vec2 nextSplinePos{ GetSplinePoint(p0, p1, p2, p3, t + 0.001f) };
				const b2Vec2 dir{ b2Normalize(nextSplinePos - splinePos) };
				const b2Vec2 perp{ -dir.y, dir.x }; //Points perpendicularly (inwardly) to the way the track is facing at this node

				const float w{ lerp(w1, w2, t) };
				
				m_innerChainVertices.push_back(splinePos + perp * (w / 2));
				m_outerChainVertices.push_back(splinePos - perp * (w / 2));
				m_triangleStripVertices.append(sf::Vertex({ m_innerChainVertices.back().x * Global::PIXELS_PER_METRE, m_innerChainVertices.back().y * Global::PIXELS_PER_METRE }, sf::Color::Blue));
				m_triangleStripVertices.append(sf::Vertex({ m_outerChainVertices.back().x * Global::PIXELS_PER_METRE, m_outerChainVertices.back().y * Global::PIXELS_PER_METRE }, sf::Color::Blue));
			}
		}
		//Close the strip visually
		m_triangleStripVertices.append(m_triangleStripVertices[0]);
		m_triangleStripVertices.append(m_triangleStripVertices[1]);

		//Reverse winding order of outer chain
		std::reverse(m_outerChainVertices.begin(), m_outerChainVertices.end());
		

		//Create chains
		const b2BodyDef chainBodyDef{ b2DefaultBodyDef() };
		m_chainBody = b2CreateBody(_world, &chainBodyDef);

		b2SurfaceMaterial chainMaterial{};
		chainMaterial.friction = m_wallFriction;
		
		b2ChainDef chainDef{ b2DefaultChainDef() };
		chainDef.points = m_innerChainVertices.data();
		chainDef.count = m_innerChainVertices.size();
		chainDef.isLoop = true;
		chainDef.materialCount = 1;
		chainDef.materials = &chainMaterial;
		chainDef.filter.categoryBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::TRACK);
		chainDef.filter.maskBits = std::to_underlying(Global::PHYSICS_COLLISION_LAYER::CAR);
		m_innerChain = b2CreateChain(m_chainBody, &chainDef);

		chainDef.points = m_outerChainVertices.data();
		chainDef.count = m_innerChainVertices.size();
		m_outerChain = b2CreateChain(m_chainBody, &chainDef);
	}



	void Track::FixedUpdate(SYNTACTIC_CONST b2WorldId& _world)
	{
		if (sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_TRACK_SUBDIVISIONS) XOR sf::Keyboard::isKeyPressed(Global::SFML_KEY_INCREMENT_TRACK_SUBDIVISIONS))
		{
			if (!m_subdivisionCountChangedLastFrame)
			{
				m_subdivisions += (sf::Keyboard::isKeyPressed(Global::SFML_KEY_DECREMENT_TRACK_SUBDIVISIONS) ? -1 : 1);
				m_subdivisions = std::clamp(m_subdivisions, 0, 100);
				
				Construct(_world);
				m_subdivisionCountChangedLastFrame = true;
			}
		}
		else
		{
			m_subdivisionCountChangedLastFrame = false;
		}
	}



	void Track::Render(sf::RenderWindow& _window) const
	{
		_window.draw(m_triangleStripVertices);

		for (const TrackNode& node : m_nodes)
		{
			constexpr float radius{ 10.0f };
			sf::CircleShape circle(radius);
			circle.setFillColor(sf::Color::Green);
			circle.setOrigin({ radius, radius });
			circle.setPosition({ node.centreWorldPosition.x * Global::PIXELS_PER_METRE, node.centreWorldPosition.y * Global::PIXELS_PER_METRE });
			_window.draw(circle);
		}

		if (m_subdivisions > 0)
		{
			const sf::String subdivisionsString{ "SUBDIVISIONS: " + std::to_string(m_subdivisions) };
			const sf::Text subdivisionsText{ m_arialFont, subdivisionsString };
			_window.draw(subdivisionsText);
		}
	}



	b2Vec2 Track::GetSplinePoint(const b2Vec2& _p0, const b2Vec2& _p1, const b2Vec2& _p2, const b2Vec2& _p3, const float _t)
	{
		//The maths behind these coefficients seems pretty complicated, so shoutout https://www.mvps.org/directx/articles/catmull/
		const b2Vec2 a{ 2*_p1 };
		const b2Vec2 b{ -_p0 + _p2 };
		const b2Vec2 c{ 2*_p0 - 5*_p1 + 4*_p2 - _p3 };
		const b2Vec2 d{ -_p0 + 3*_p1 - 3*_p2 + _p3 };
		return 0.5f * (a + (b * _t) + (c * _t * _t) + (d * _t * _t * _t));
	}

}