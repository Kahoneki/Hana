#pragma once

#include "Global.h"

#include <vector>
#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>


namespace Hana
{

	struct TrackNode
	{
	public:
		b2Vec2 centreWorldPosition;
		float width;
	};


	class Track final
	{
	public:
		explicit Track();
		~Track() = default;

		void Construct(SYNTACTIC_CONST b2WorldId& _world);
		
		inline void AddNode(const TrackNode& _node) { m_nodes.push_back(_node); }
		[[nodiscard]] inline const TrackNode& GetNode(const std::size_t _index) const { return m_nodes[_index]; }
		[[nodiscard]] inline std::size_t GetNumNodes() const { return m_nodes.size(); }

		void FixedUpdate(SYNTACTIC_CONST b2WorldId& _world);
		void Render(sf::RenderWindow& _window) const;
		

	private:
		//Get position on catmull-rom spline (between p1 and p2)
		[[nodiscard]] static b2Vec2 GetSplinePoint(const b2Vec2& _p0, const b2Vec2& _p1, const b2Vec2& _p2, const b2Vec2& _p3, const float _t);
		
		std::vector<TrackNode> m_nodes;
		sf::VertexArray m_triangleStripVertices; //For rendering

		b2BodyId m_chainBody;
		b2ChainId m_innerChain;
		b2ChainId m_outerChain;
		std::vector<b2Vec2> m_innerChainVertices; //For physics
		std::vector<b2Vec2> m_outerChainVertices; //For physics

		float m_wallFriction;

		std::int32_t m_subdivisions; //signed to avoid weird clamping issues
		sf::Font m_arialFont;
		bool m_subdivisionCountChangedLastFrame;
	};

}