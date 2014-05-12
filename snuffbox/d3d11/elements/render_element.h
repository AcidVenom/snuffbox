#pragma once

#include "../../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../../snuffbox/logging.h"
#include "../../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	class D3D11DisplayDevice;
	/**
	* @class snuffbox::RenderElement
	* @brief Used as base class for all visual game objects
	* @author Daniël Konings
	*/
	class RenderElement
	{
	public:
		/// Default constructor
		RenderElement() : 
			worldMatrix_(XMMatrixIdentity()),
			translation_(XMMatrixIdentity()),
			rotation_(XMMatrixIdentity()),
			scaling_(XMMatrixIdentity())
		{}

		/// Draws the render element
		virtual void Draw() = 0;
		
		/// Returns the vertices of the render element
		std::vector<Vertex>& vertices(){ return vertices_; };
		/// Returns the indices of the render element
		std::vector<unsigned int>& indices(){ return indices_; };

		/// Returns the world matrix
		XMMATRIX& World(){ worldMatrix_ = rotation_ * scaling_ * translation_; return worldMatrix_; }

		/// Sets the translation on the X, Y, Z plane
		void SetTranslation(float x, float y, float z);
		/// Translates on the X, Y, Z plane
		void TranslateBy(float x, float y, float z);
		/// Sets the rotation on the X, Y, Z axes
		void SetRotation(float x, float y, float z);
		/// Rotates on the X, Y, Z axes
		void RotateBy(float x, float y, float z);
		/// Sets the X, Y and Z scale
		void SetScale(float x, float y, float z);

	private:
		std::vector<Vertex>						vertices_; ///< The vertices
		std::vector<unsigned int>			indices_; ///< The indices
		XMMATRIX											worldMatrix_; ///< The world matrix
		XMMATRIX											translation_;	///< The translation matrix
		XMMATRIX											rotation_; ///< The rotation matrix
		XMMATRIX											scaling_; ///< The scaling matrix

	public:
		static void JSTranslateBy(JS_ARGS);
		static void JSSetTranslation(JS_ARGS);
		static void JSRotateBy(JS_ARGS);
		static void JSSetRotation(JS_ARGS);
		static void JSSetScale(JS_ARGS);
	};

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::TranslateBy(float x, float y, float z)
	{
		translation_ *= XMMatrixTranslation(x, y, z);
	}
	
	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetTranslation(float x, float y, float z)
	{
		translation_ = XMMatrixTranslation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::RotateBy(float x, float y, float z)
	{
		rotation_ *= XMMatrixRotationRollPitchYaw(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetRotation(float x, float y, float z)
	{
		rotation_ = XMMatrixRotationRollPitchYaw(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetScale(float x, float y, float z)
	{
		scaling_ = XMMatrixScaling(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetTranslation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSTranslateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->TranslateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetRotation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->RotateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetScale(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetScale(x, y, z);
	}
}