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
	class RenderElement : public JSObject
	{
	public:
		/// Default constructor
		RenderElement() : 
			worldMatrix_(XMMatrixIdentity()),
			translation_(XMMatrixIdentity()),
			rotation_(XMMatrixIdentity()),
			scaling_(XMMatrixIdentity()),
			offset_(XMMatrixIdentity()),
			yaw_(0.0f), pitch_(0.0f), roll_(0.0f)
		{}

		/// Draws the render element
		virtual void Draw() = 0;
		
		/// Returns the vertices of the render element
		std::vector<Vertex>& vertices(){ return vertices_; };
		/// Returns the indices of the render element
		std::vector<unsigned int>& indices(){ return indices_; };

		/// Returns the world matrix
		XMMATRIX& World(){ worldMatrix_ = offset_ * rotation_ * scaling_ * translation_; return worldMatrix_; }

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
		/// Sets the X, Y and Z offset
		void SetOffset(float x, float y, float z);

	private:
		std::vector<Vertex>						vertices_; ///< The vertices
		std::vector<unsigned int>			indices_; ///< The indices
		XMMATRIX											worldMatrix_; ///< The world matrix
		XMMATRIX											translation_;	///< The translation matrix
		XMMATRIX											rotation_; ///< The rotation matrix
		XMMATRIX											scaling_; ///< The scaling matrix
		XMMATRIX											offset_;	///< The offset matrix
		float													yaw_, pitch_, roll_; ///< Rotation floats

	public:
		static void RegisterJS(JS_TEMPLATE);
		static void JSTranslateBy(JS_ARGS);
		static void JSSetTranslation(JS_ARGS);
		static void JSRotateBy(JS_ARGS);
		static void JSSetRotation(JS_ARGS);
		static void JSSetScale(JS_ARGS);
		static void JSSetOffset(JS_ARGS);
		static void JSOffset(JS_ARGS);
		static void JSScale(JS_ARGS);
		static void JSRotation(JS_ARGS);
		static void JSTranslation(JS_ARGS);
	};

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetOffset(float x, float y, float z)
	{
		offset_ = XMMatrixTranslation(x, y, z);
	}

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

		self->yaw_ = x;
		self->pitch_ = y;
		self->roll_ = z;

		self->SetRotation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->yaw_ += x;
		self->pitch_ += y;
		self->roll_ += z;

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

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		float x = -wrapper.GetNumber<float>(0);
		float y = -wrapper.GetNumber<float>(1);
		float z = -wrapper.GetNumber<float>(2);

		self->SetOffset(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnTriple<float>(-self->offset_._41, -self->offset_._42, -self->offset_._43);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSScale(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnTriple<float>(self->scaling_._41, self->scaling_._42, self->scaling_._43);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnTriple<float>(self->translation_._41, self->translation_._42, self->translation_._43);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnTriple<float>(self->yaw_, self->pitch_, self->roll_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("translation", JSTranslation),
			JSFunctionRegister("rotation", JSRotation),
			JSFunctionRegister("offset", JSOffset),
			JSFunctionRegister("scale", JSScale),
			JSFunctionRegister("setTranslation", JSSetTranslation),
			JSFunctionRegister("translateBy", JSTranslateBy),
			JSFunctionRegister("rotateBy", JSRotateBy),
			JSFunctionRegister("setRotation", JSSetRotation),
			JSFunctionRegister("setScale", JSSetScale),
			JSFunctionRegister("setOffset", JSSetOffset)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}