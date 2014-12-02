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

		/**
		* @brief Sets the colour of a given vertex index
		* @param[in] idx (int) The index of the vertex
		* @param[in] r (float) The red value
		* @param[in] g (float) The green value
		* @param[in] b (float) The blue value
		* @param[in] a (float) The alpha value
		*/
		void SetColour(int idx, float r, float g, float b, float a);

		/**
		* @brief Sets the position of a given vertex index
		* @param[in] idx (int) The index of the vertex
		* @param[in] x (float) The X value
		* @param[in] y (float) The Y value
		* @param[in] z (float) The Z value
		*/
		void SetPosition(int idx, float x, float y, float z);

		/**
		* @brief Sets the normal of a given vertex index
		* @param[in] idx (int) The index of the vertex
		* @param[in] x (float) The X value
		* @param[in] y (float) The Y value
		* @param[in] z (float) The Z value
		*/
		void SetNormal(int idx, float x, float y, float z);

		/**
		* @brief Sets the texture coordinates of a given vertex index
		* @param[in] idx (int) The index of the vertex
		* @param[in] x (float) The X value
		* @param[in] y (float) The Y value
		*/
		void SetTexCoords(int idx, float x, float y);

		/**
		* @brief Remove a vertex by index
		* @param[in] idx (int) The index to remove
		*/
		void RemoveVertex(int idx);

		/**
		* @brief Adds an index to the index buffer
		* @param[in] idx (int) The index to add
		*/
		void AddIndex(int idx);

		/**
		* @brief Removes an index by index
		* @param[in] idx (int) The index of the index to remove
		*/
		void RemoveIndex(int idx);

		/// Flushes the vertex vector and index vector to the corresponding buffers
		void Flush();

		/// Creates the polygon
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

		/**
		* @brief Sets the primitive topology of this polygon
		* @param[in] type (int) The topology type
		*/
		void SetTopology(int type);

	private:
		D3D11_PRIMITIVE_TOPOLOGY topology_; //!< The current primitive topology for this polygon
		ID3D11Buffer* vertex_buffer_; //!< The vertex buffer of this polygon
		ID3D11Buffer* index_buffer_; //!< The index buffer of this polygon

	public:
		JS_NAME(Polygon);
		void RegisterExtraFunctions(JS_EXTRA);
		static void JSAddVertex(JS_ARGS);
		static void JSRemoveVertex(JS_ARGS);
		static void JSClearVertices(JS_ARGS);
		static void JSClearIndices(JS_ARGS);
		static void JSClear(JS_ARGS);
		static void JSAddIndex(JS_ARGS);
		static void JSRemoveIndex(JS_ARGS);
		static void JSFlush(JS_ARGS);
		static void JSSetColour(JS_ARGS);
		static void JSSetPosition(JS_ARGS);
		static void JSSetTexCoords(JS_ARGS);
		static void JSSetNormal(JS_ARGS);
		static void JSSetTopology(JS_ARGS);
	};
}