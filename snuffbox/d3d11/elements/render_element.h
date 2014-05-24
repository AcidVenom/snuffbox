#pragma once

#include "../../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../../snuffbox/logging.h"
#include "../../../snuffbox/js/js_state_wrapper.h"
#include "../../../snuffbox/game.h"
#include "../../../snuffbox/d3d11/d3d11_texture.h"

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
			x_(0.0f), y_(0.0f), z_(0.0f),
			ox_(0.0f), oy_(0.0f), oz_(0.0f),
			sx_(1.0f), sy_(1.0f), sz_(1.0f),
			yaw_(0.0f), pitch_(0.0f), roll_(0.0f),
			texture_(nullptr)
		{}

    /// Default destructor
    ~RenderElement()
    {
      unsigned int index = 0;
      if (environment::has_game())
      {
        for (auto& it : environment::render_device().renderElements())
        {
          if (it == this)
          {
            environment::render_device().renderElements().erase(environment::render_device().renderElements().begin() + index);
            break;
          }
          ++index;
        }
      }
    }

		/// Sets the buffers of the render element
		virtual void SetBuffers() = 0;
		
		/// Returns the vertices of the render element
		std::vector<Vertex>& vertices(){ return vertices_; };
		/// Returns the indices of the render element
		std::vector<unsigned int>& indices(){ return indices_; };

		/// Returns the world matrix
    XMMATRIX& World(){ 
      worldMatrix_ = XMMatrixTranslation(ox_, oy_, oz_) *  
      XMMatrixRotationRollPitchYaw(roll_, pitch_, yaw_) *
      XMMatrixScaling(sx_, sy_, sz_) *
      XMMatrixTranslation(x_, y_, z_);
      return worldMatrix_; 
    }

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

		/// Returns the texture
		Texture* texture(){ return texture_; }

    /// Returns the vertex buffer type
    virtual VertexBufferType type() = 0;

	private:
		std::vector<Vertex>						vertices_; ///< The vertices
		std::vector<unsigned int>			indices_; ///< The indices
		XMMATRIX											worldMatrix_; ///< The world matrix
		float													yaw_, pitch_, roll_; ///< Rotation floats
    float                         x_, y_, z_; ///< Translation floats
    float                         ox_, oy_, oz_; ///< Offset floats
    float                         sx_, sy_, sz_; ///< Scaling floats
		Texture*											texture_;	///< The texture of this render element
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
		static void JSSetTexture(JS_ARGS);
	};

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetOffset(float x, float y, float z)
	{
    ox_ = x;
    oy_ = y;
    oz_ = z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::TranslateBy(float x, float y, float z)
	{
    x_ += x;
    y_ += y;
    z_ += z;
	}
	
	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetTranslation(float x, float y, float z)
	{
    x_ = x;
    y_ = y;
    z_ = z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::RotateBy(float x, float y, float z)
	{
    yaw_ += x;
    pitch_ += y;
    roll_ += z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetRotation(float x, float y, float z)
	{
    yaw_ = x;
    pitch_ = y;
    roll_ = z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetScale(float x, float y, float z)
	{
    sx_ = x;
    sy_ = y;
    sz_ = z;
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

    wrapper.ReturnTriple<float>(-self->ox_, -self->oy_, -self->oz_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSScale(JS_ARGS)
	{
		JS_SETUP(RenderElement);

    wrapper.ReturnTriple<float>(self->sx_, self->sy_, self->sz_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

    wrapper.ReturnTriple<float>(self->x_, self->y_, self->z_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnTriple<float>(self->yaw_, self->pitch_, self->roll_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		self->texture_ = wrapper.GetPointer<Texture>(0);
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
			JSFunctionRegister("setOffset", JSSetOffset),
			JSFunctionRegister("setTexture", JSSetTexture)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}