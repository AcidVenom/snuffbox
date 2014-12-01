#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Polygon
	* @brief A polygon of vertices, created with an index and vertex buffer
	* @author Daniël Konings
	*/
	class Polygon : public RenderElement
	{
	public:
		/// Default constructor
		Polygon();

		/// JavaScript constructor
		Polygon(JS_ARGS);

		/// Default destructor
		virtual ~Polygon();

		/**
		* @brief Adds a vertex to this polygon
		* @param[in] x (float) The x position of the vertex
		* @param[in] y (float) The y position of the vertex
		* @param[in] z (float) The z position of the vertex
		* @param[in] r (float) The red value of the vertex
		* @param[in] g (float) The green value of the vertex
		* @param[in] b (float) The blue value of the vertex
		* @param[in] a (float) The alpha value of the vertex
		* @param[in] nx (float) The x normal of the vertex
		* @param[in] ny (float) The y normal of the vertex
		* @param[in] nz (float) The z normal of the vertex
		* @param[in] tx (float) The x texture coordinate of the vertex
		* @param[in] ty (float) The y texture coordinate of the vertex
		*/
		void AddVertex(float x, float y, float z, float r, float g, float b, float a, float nx, float ny, float nz, float tx, float ty);

		/// Creates the widget
		virtual void Create();

		/// Sets the buffers for the render device to use
		virtual void SetBuffers();

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
		virtual VertexBufferType type(){ return VertexBufferType::kPolygon; }

		/**
		* @return (D3D11_PRIMITIVE_TOPOLOGY) The topology of this polygon
		*/
		D3D11_PRIMITIVE_TOPOLOGY topology();

	private:
		D3D11_PRIMITIVE_TOPOLOGY topology_; //!< The current primitive topology for this polygon
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer of this polygon
		ID3D11Buffer* index_buffer_; //!< The index buffer of this polygon

	public:
		static void RegisterExtraFunctions(JS_EXTRA);
		static void JSAddVertex(JS_ARGS);
	};
}