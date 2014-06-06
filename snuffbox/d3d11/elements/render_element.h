#pragma once

#include "../../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../../snuffbox/logging.h"
#include "../../../snuffbox/js/js_state_wrapper.h"
#include "../../../snuffbox/js/js_wrapper.h"
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

		/**
		* @enum snuffbox::ElementTypes
		* @brief An enumerator to hold typing information on render elements
		* @author Daniël Konings
		*/
		enum ElementTypes
		{
			kTerrain,
			kQuad,
			kBillboard,
		};

		/// Default constructor
		RenderElement(ElementTypes type) :
			worldMatrix_(XMMatrixIdentity()),
			x_(0.0f), y_(0.0f), z_(0.0f),
			ox_(0.0f), oy_(0.0f), oz_(0.0f),
			sx_(1.0f), sy_(1.0f), sz_(1.0f),
			rotation_(XMMatrixIdentity()),
			texture_(nullptr),
			elementType_(type),
			shader_(environment::content_manager().Get<Shader>("shaders/base.fx").get()),
			destroyed_(true),
			distanceFromCamera_(0.0f),
			alpha_(1.0f)
		{}

    /// Default destructor
    ~RenderElement()
    {
      Destroy();
    }

		/// Sets the buffers of the render element
		virtual void SetBuffers() = 0;
		
		/// Returns the vertices of the render element
		std::vector<Vertex>& vertices(){ return vertices_; };
		/// Returns the indices of the render element
		std::vector<unsigned int>& indices(){ return indices_; };

		/// Returns the world matrix
    XMMATRIX& World(){ 
			worldMatrix_ = XMMatrixScaling(sx_, sy_, sz_) *
			XMMatrixTranslation(ox_*sx_, oy_*sy_, oz_*sz_) *
      rotation_ *
      XMMatrixTranslation(x_, y_, z_);
      return worldMatrix_; 
    }

		XMMATRIX scaling(){ return XMMatrixScaling(sx_, sy_, sz_); }
		XMMATRIX offset(){ return XMMatrixTranslation(ox_, oy_, oz_); }
		XMVECTOR scale(){ return XMVectorSet(sx_, sy_, sz_,0); }
		XMMATRIX rotation(){ return rotation_; }

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

		/// Sets the distance from the camera
		void SetDistanceFromCamera(float distance){ distanceFromCamera_ = distance; }
		/// Returns the distance from the camera
		float distanceFromCamera(){ return distanceFromCamera_; }

    /// Destroys the render element
    void Destroy();

    /// Spawns the render element
    void Spawn();

		/// Returns the translation as a vector
		XMVECTOR translation(){ return XMVectorSet(x_, y_, z_, 0.0f); }

		/// Returns the texture
		Texture* texture(){ return texture_; }

		/// Returns the shader
		Shader* shader(){ return shader_; }

    /// Returns the vertex buffer type
    virtual VertexBufferType type() = 0;
		
		/// Returns the element type
		ElementTypes& element_type(){ return elementType_; }

		/// Sets the alpha of this element
		void SetAlpha(float alpha){ alpha_ = alpha; }

		/// Returns the alpha of this element
		float alpha(){ return alpha_; }

		/// Sets a uniform for this element
		void SetUniform(std::string name, float value);

		/// Returns a list of uniforms to assign to the constant buffer
		std::vector<float> uniforms();

	private:
		std::vector<Vertex>						vertices_; ///< The vertices
		std::vector<unsigned int>			indices_; ///< The indices
		XMMATRIX											worldMatrix_; ///< The world matrix
		XMMATRIX											rotation_;	///< Rotation
    float                         x_, y_, z_; ///< Translation floats
    float                         ox_, oy_, oz_; ///< Offset floats
    float                         sx_, sy_, sz_; ///< Scaling floats
		Texture*											texture_;	///< The texture of this render element
		Shader*												shader_; ///< The current shader used by this element
		ElementTypes									elementType_;	///< The type of this render element
    bool                          destroyed_; ///< Is this element destroyed?
		float													distanceFromCamera_; ///< The distance from the camera
		float													alpha_;	///< The alpha value of this whole element
		std::map<std::string, float>	uniforms_;	///< Uniforms for the constant buffer of the shader
		std::vector<std::string>			uniformNames_; ///< A list of uniform names
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
		static void JSSetShader(JS_ARGS);
    static void JSDestroy(JS_ARGS);
    static void JSSpawn(JS_ARGS);
		static void JSSetAlpha(JS_ARGS);
		static void JSAlpha(JS_ARGS);
	};

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::Spawn()
  {
    if (destroyed_)
    {
      destroyed_ = false;
			if (element_type() != ElementTypes::kTerrain)
			{
				environment::render_device().renderElements().push_back(this);
			}
			else
			{
				environment::render_device().opaqueElements().push_back(this);
			}
    }
  }

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::Destroy()
  {
    if (!destroyed_)
    {
      unsigned int index = 0;
      if (environment::has_game())
      {
				if (element_type() != ElementTypes::kTerrain)
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
				else
				{
					for (auto& it : environment::render_device().opaqueElements())
					{
						if (it == this)
						{
							environment::render_device().opaqueElements().erase(environment::render_device().opaqueElements().begin() + index);
							break;
						}
						++index;
					}
				}
      }
    }

    destroyed_ = true;
  }

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
		rotation_ *= XMMatrixRotationRollPitchYaw(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetRotation(float x, float y, float z)
	{
		rotation_ = XMMatrixRotationRollPitchYaw(x, 0, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, y, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, 0, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetScale(float x, float y, float z)
	{
    sx_ = x;
    sy_ = y;
    sz_ = z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetUniform(std::string name, float value)
	{
		auto it = uniforms_.find(name);
		if (it == uniforms_.end())
		{
			uniforms_.emplace(name, value);
		}
		else
		{
			it->second = value;
		}
	}

	//-------------------------------------------------------------------------------------------
	inline std::vector<float> RenderElement::uniforms()
	{
		std::vector<float> temp;

		return temp;
	}

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::JSSpawn(JS_ARGS)
  {
    JS_SETUP(RenderElement);

    self->Spawn();
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
		float roll = atan2(self->rotation_._31, self->rotation_._32);
		float pitch = acos(self->rotation_._33);
		float yaw = -atan2(self->rotation_._13, self->rotation_._23);
		wrapper.ReturnTriple<float>(yaw, pitch, roll);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		self->texture_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetShader(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		self->shader_ = environment::content_manager().Get<Shader>(wrapper.GetString(0)).get();
	}

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::JSDestroy(JS_ARGS)
  {
    JS_SETUP(RenderElement);

    self->Destroy();
  }

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement);
		
		self->SetAlpha(wrapper.GetNumber<float>(0));
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement);

		wrapper.ReturnNumber<float>(self->alpha());
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
			JSFunctionRegister("setTexture", JSSetTexture),
			JSFunctionRegister("setShader", JSSetShader),
      JSFunctionRegister("destroy", JSDestroy),
      JSFunctionRegister("spawn", JSSpawn),
			JSFunctionRegister("alpha", JSAlpha),
			JSFunctionRegister("setAlpha", JSSetAlpha)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}