#pragma once

#include "../../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../../snuffbox/logging.h"
#include "../../../snuffbox/js/js_state_wrapper.h"
#include "../../../snuffbox/js/js_wrapper.h"
#include "../../../snuffbox/game.h"
#include "../../../snuffbox/d3d11/d3d11_texture.h"
#include "../../../snuffbox/content/content_manager.h"
#include "../../../snuffbox/d3d11/d3d11_settings.h"

namespace snuffbox
{
	static int element_uuid = 0;

	class D3D11DisplayDevice;
	
	/**
	* @enum snuffbox::UniformType
	* @brief An enumerator for shader uniform types
	* @author Daniël Konings
	*/
	enum UniformType
	{
		kFloat,
		kFloat2,
		kFloat3,
		kFloat4
	};

	/**
	* @struct snuffbox::ShaderUniform
	* @brief Holds data about a uniform for use with a shader
	* @author Daniël Konings
	*/
	struct ShaderUniform
	{
		std::string name;
		UniformType type;
		float value[4];
	};

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
			kWidget,
			kMesh
		};

		/// Default constructor
		RenderElement(ElementTypes type) :
			worldMatrix_(XMMatrixIdentity()),
			x_(0.0f), y_(0.0f), z_(0.0f),
			ox_(0.0f), oy_(0.0f), oz_(0.0f),
			sx_(1.0f), sy_(1.0f), sz_(1.0f),
			rotation_(XMMatrixIdentity()),
			texture_(nullptr),
			normalMap_(nullptr),
			elementType_(type),
			shader_(environment::content_manager().Get<Shader>("shaders/base.fx").get()),
			destroyed_(true),
			distanceFromCamera_(0.0f),
			alpha_(1.0f),
			blend_(1.0f,1.0f,1.0f),
			visible_(true)
		{
			name_ = "RenderElement_" + std::to_string(++element_uuid);
		}

    /// Default destructor
    ~RenderElement()
    {
			
    }

		static void RemoveFromRenderer(RenderElement* ptr)
		{
			if (!environment::has_render_device())
			{
				return;
			}
			std::vector<RenderElement*>& vec = environment::render_device().opaqueElements();
			ptr->Destroy();
			if (ptr->element_type() == ElementTypes::kTerrain)
			{
				for (unsigned int i = 0; i < vec.size(); ++i)
				{
					RenderElement* it = vec[i];

					if (it == ptr)
					{
						vec.erase(vec.begin() + i);
						break;
					}
				}
			}
			else if (ptr->element_type() == ElementTypes::kWidget)
			{
				vec = environment::render_device().uiElements();

				for (unsigned int i = 0; i < vec.size(); ++i)
				{
					RenderElement* it = vec[i];

					if (it == ptr)
					{
						vec.erase(vec.begin() + i);
						break;
					}
				}
			}
			else
			{
				vec = environment::render_device().renderElements();

				for (unsigned int i = 0; i < vec.size(); ++i)
				{
					RenderElement* it = vec[i];

					if (it == ptr)
					{
						vec.erase(vec.begin() + i);
						break;
					}
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
    virtual XMMATRIX& World(Camera* camera){
			worldMatrix_ = XMMatrixScaling(sx_, sy_, sz_) *
			XMMatrixTranslation(ox_*sx_, oy_*sy_, oz_*sz_) *
      rotation_ *
      XMMatrixTranslation(x_, y_, z_);
      return worldMatrix_; 
    }

		XMMATRIX scaling(){ return XMMatrixScaling(sx_, sy_, sz_); }
		XMMATRIX offset(){ return XMMatrixTranslation(ox_*sx_, oy_*sy_, oz_*sz_); }
		XMVECTOR scale(){ return XMVectorSet(sx_, sy_, sz_,0); }
		XMMATRIX rotation(){ return rotation_; }

		bool destroyed();

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
		/// Sets the blend color of this render element
		void SetBlend(float x, float y, float z);
		/// Returns the blend color of this render element
		XMFLOAT3 blend(){ return blend_; }

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

		/// Returns the normal map
		Texture* normalMap(){ return normalMap_; }

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
		void SetUniform(UniformType type, std::string name, float _1 = 0.0f, float _2 = 0.0f, float _3 = 0.0f, float _4 = 0.0f);

		/// Removes all uniforms for the next loop
		void ClearUniforms(){ uniforms_.clear(); }

		/// Converts a given uniform type name to a uniform type
		static UniformType TypeNameToUniformType(std::string type);

		/// Returns a list of uniforms to assign to the constant buffer
		std::vector<float> uniforms();

		/// Sets the visibility of this render element
		void SetVisible(bool visible){ visible_ = visible; }

		/// Returns the visibility of this render element
		bool visible(){ return visible_; }

		/// Returns the z index
		float z(){ return z_; }

	private:
		std::vector<Vertex>										vertices_; ///< The vertices
		std::vector<unsigned int>							indices_; ///< The indices
		XMMATRIX															worldMatrix_; ///< The world matrix
		XMMATRIX															rotation_;	///< Rotation
    float																	x_, y_, z_; ///< Translation floats
    float																	ox_, oy_, oz_; ///< Offset floats
    float																	sx_, sy_, sz_; ///< Scaling floats
		Texture*															texture_;	///< The texture of this render element
		Texture*															normalMap_; ///< The normal map of this render element
		Shader*																shader_; ///< The current shader used by this element
		ElementTypes													elementType_;	///< The type of this render element
    bool																	destroyed_; ///< Is this element destroyed?
		float																	distanceFromCamera_; ///< The distance from the camera
		float																	alpha_;	///< The alpha value of this whole element
		std::map<std::string, ShaderUniform>	uniforms_;	///< Uniforms for the constant buffer of the shader
		XMFLOAT3															blend_;	///< The blend color of this render element
		bool																	visible_; ///< Is this element visible?
		std::string														name_;	 ///< The render element name
		
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
		static void JSSetNormalMap(JS_ARGS);
		static void JSSetShader(JS_ARGS);
    static void JSDestroy(JS_ARGS);
    static void JSSpawn(JS_ARGS);
		static void JSSetAlpha(JS_ARGS);
		static void JSAlpha(JS_ARGS);
		static void JSSetUniform(JS_ARGS);
		static void JSSetBlend(JS_ARGS);
		static void JSBlend(JS_ARGS);
		static void JSSetVisible(JS_ARGS);
		static void JSSetName(JS_ARGS);
		static void JSName(JS_ARGS);
	};

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::Spawn()
  {
    if (destroyed_)
    {
      destroyed_ = false;
			if (element_type() == ElementTypes::kTerrain)
			{
				environment::render_device().opaqueElements().push_back(this);
			}
			else if (element_type() == ElementTypes::kWidget)
			{
				environment::render_device().uiElements().push_back(this);
			}
			else
			{
				environment::render_device().renderElements().push_back(this);
			}
    }
  }

	//-------------------------------------------------------------------------------------------
	inline bool RenderElement::destroyed()
	{
		return destroyed_;
	}

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::Destroy()
  {
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
	inline void RenderElement::SetBlend(float x, float y, float z)
	{
		blend_.x = x;
		blend_.y = y;
		blend_.z = z;
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::SetUniform(UniformType type, std::string name, float _1, float _2, float _3, float _4)
	{
		ShaderUniform uniform;

		uniform.name = name;
		uniform.type = type;
		uniform.value[0] = _1;
		uniform.value[1] = _2;
		uniform.value[2] = _3;
		uniform.value[3] = _4;

		uniforms_.emplace(name, uniform);

		auto& it = uniforms_.find(name);
		if (it != uniforms_.end())
		{
			it->second = uniform;
		}
		else
		{
			uniforms_.emplace(name,uniform);
		}
	}

	//-------------------------------------------------------------------------------------------
	inline UniformType RenderElement::TypeNameToUniformType(std::string type)
	{
		if (strcmp(type.c_str(), "float") == 0) return UniformType::kFloat;
		if (strcmp(type.c_str(), "float2") == 0) return UniformType::kFloat2;
		if (strcmp(type.c_str(), "float3") == 0) return UniformType::kFloat3;
		if (strcmp(type.c_str(), "float4") == 0) return UniformType::kFloat4;

		SNUFF_ASSERT("Tried to set an invalid uniform type!");
		return UniformType::kFloat;
	}

	//-------------------------------------------------------------------------------------------
	inline std::vector<float> RenderElement::uniforms()
	{
		std::vector<float> temp;
		for (std::map<std::string, ShaderUniform>::iterator it = uniforms_.begin(); it != uniforms_.end(); ++it) {
			switch(it->second.type)
			{
			case UniformType::kFloat:
				temp.push_back(it->second.value[0]);
				break;
			case UniformType::kFloat2:
				temp.push_back(it->second.value[0]);
				temp.push_back(it->second.value[1]);
				break;
			case UniformType::kFloat3:
				temp.push_back(it->second.value[0]);
				temp.push_back(it->second.value[1]);
				temp.push_back(it->second.value[2]);
				break;
			case UniformType::kFloat4:
				temp.push_back(it->second.value[0]);
				temp.push_back(it->second.value[1]);
				temp.push_back(it->second.value[2]);
				temp.push_back(it->second.value[3]);
				break;
			}
			
		}
		return temp;
	}

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::JSSpawn(JS_ARGS)
  {
    JS_SETUP(RenderElement,"V");

    self->Spawn();
  }

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetTranslation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSTranslateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->TranslateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetRotation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->RotateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetScale(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetScale(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement,"NNN");

		float x = -wrapper.GetNumber<float>(0);
		float y = -wrapper.GetNumber<float>(1);
		float z = -wrapper.GetNumber<float>(2);

		self->SetOffset(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement,"V");

    wrapper.ReturnTriple<float>(-self->ox_, -self->oy_, -self->oz_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSScale(JS_ARGS)
	{
		JS_SETUP(RenderElement,"V");

    wrapper.ReturnTriple<float>(self->sx_, self->sy_, self->sz_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement,"V");

    wrapper.ReturnTriple<float>(self->x_, self->y_, self->z_);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement,"V");
		float roll = atan2(self->rotation_._31, self->rotation_._32);
		float pitch = acos(self->rotation_._33);
		float yaw = -atan2(self->rotation_._13, self->rotation_._23);
		wrapper.ReturnTriple<float>(yaw, pitch, roll);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement,"S");

		self->texture_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetNormalMap(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->normalMap_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetShader(JS_ARGS)
	{
		JS_SETUP(RenderElement,"S");

		self->shader_ = environment::content_manager().Get<Shader>(wrapper.GetString(0)).get();
	}

  //-------------------------------------------------------------------------------------------
  inline void RenderElement::JSDestroy(JS_ARGS)
  {
    JS_SETUP(RenderElement,"V");

    self->Destroy();
  }

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement,"N");
		
		self->SetAlpha(wrapper.GetNumber<float>(0));
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement,"V");

		wrapper.ReturnNumber<float>(self->alpha());
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetUniform(JS_ARGS)
	{
		JS_SETUP(RenderElement,"SSN");
		float* value = 0;
		std::string type = wrapper.GetString(0);
		std::string name = wrapper.GetString(1);
		
		UniformType uniformType = RenderElement::TypeNameToUniformType(type);

		switch (uniformType)
		{
		case UniformType::kFloat:
			self->SetUniform(uniformType, name, 
				wrapper.GetNumber<float>(2));
			break;
		case UniformType::kFloat2:
			self->SetUniform(uniformType, name, 
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3));
			break;
		case UniformType::kFloat3:
			self->SetUniform(uniformType, name, 
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3),
				wrapper.GetNumber<float>(4));
			break;
		case UniformType::kFloat4:
			self->SetUniform(uniformType, name, 
				wrapper.GetNumber<float>(2),
				wrapper.GetNumber<float>(3),
				wrapper.GetNumber<float>(4),
				wrapper.GetNumber<float>(5));
			break;
		}
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetBlend(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		self->SetBlend(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2));
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSBlend(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		XMFLOAT3 blend = self->blend();
		wrapper.ReturnTriple<float>(blend.x,blend.y,blend.z,"r","g","b");
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetVisible(JS_ARGS)
	{
		JS_SETUP(RenderElement, "B");
		self->SetVisible(wrapper.GetBool(0));
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSSetName(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");
		self->name_ = wrapper.GetString(0);
	}

	//-------------------------------------------------------------------------------------------
	inline void RenderElement::JSName(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnString(self->name_.c_str());
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
			JSFunctionRegister("setNormalMap", JSSetNormalMap),
			JSFunctionRegister("setShader", JSSetShader),
      JSFunctionRegister("destroy", JSDestroy),
      JSFunctionRegister("spawn", JSSpawn),
			JSFunctionRegister("alpha", JSAlpha),
			JSFunctionRegister("setAlpha", JSSetAlpha),
			JSFunctionRegister("setUniform", JSSetUniform),
			JSFunctionRegister("setBlend", JSSetBlend),
			JSFunctionRegister("blend", JSBlend),
			JSFunctionRegister("setName", JSSetName),
			JSFunctionRegister("name", JSName)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}