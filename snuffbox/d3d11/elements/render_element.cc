#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
	//--------------------------------------------------------------------------------------------------
	RenderElement::RenderElement(ElementTypes type) :
		world_matrix_(XMMatrixIdentity()),
		x_(0.0f), y_(0.0f), z_(0.0f),
		ox_(0.0f), oy_(0.0f), oz_(0.0f),
		sx_(1.0f), sy_(1.0f), sz_(1.0f),
		rotation_(XMMatrixIdentity()),
		texture_(nullptr),
		normal_map_(nullptr),
		element_type_(type),
		shader_(environment::content_manager().Get<Shader>("shaders/base.fx").get()),
		destroyed_(true),
		distance_from_camera_(0.0f),
		alpha_(1.0f),
		blend_(1.0f, 1.0f, 1.0f),
		visible_(true)
	{
		name_ = "RenderElement_" + std::to_string(++element_uuid);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderElement::RemoveFromRenderer()
	{
		if (!environment::has_render_device())
		{
			return;
		}
		std::vector<RenderElement*>* vec = &environment::render_device().opaque_elements();
		Destroy();
		if (element_type() == ElementTypes::kTerrain)
		{
			for (unsigned int i = 0; i < vec->size(); ++i)
			{
				RenderElement* it = vec->at(i);

				if (it == this)
				{
					vec->erase(vec->begin() + i);
					break;
				}
			}
		}
		else if (element_type() == ElementTypes::kWidget)
		{
			vec = &environment::render_device().ui_elements();

			for (unsigned int i = 0; i < vec->size(); ++i)
			{
				RenderElement* it = vec->at(i);

				if (it == this)
				{
					vec->erase(vec->begin() + i);
					break;
				}
			}
		}
		else
		{
			vec = &environment::render_device().render_elements();

			for (unsigned int i = 0; i < vec->size(); ++i)
			{
				RenderElement* it = vec->at(i);

				if (it == this)
				{
					vec->erase(vec->begin() + i);
					break;
				}
			}
		}
	}

	//--------------------------------------------------------------------------------------------------
	XMMATRIX& RenderElement::world_matrix(Camera* camera)
	{
		world_matrix_ = XMMatrixScaling(sx_, sy_, sz_) *
			XMMatrixTranslation(ox_*sx_, oy_*sy_, oz_*sz_) *
			rotation_ *
			XMMatrixTranslation(x_, y_, z_);
		return world_matrix_;
	}


	//-------------------------------------------------------------------------------------------
	void RenderElement::Spawn()
	{
		if (destroyed_ == true)
		{
			if (element_type() == ElementTypes::kTerrain)
			{
				environment::render_device().opaque_elements().push_back(this);
			}
			else if (element_type() == ElementTypes::kWidget)
			{
				environment::render_device().ui_elements().push_back(this);
			}
			else
			{
				environment::render_device().render_elements().push_back(this);
			}

			destroyed_ = false;
		}
	}

	//-------------------------------------------------------------------------------------------
	bool RenderElement::destroyed()
	{
		return destroyed_;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::Destroy()
	{
		destroyed_ = true;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetOffset(float x, float y, float z)
	{
		ox_ = x;
		oy_ = y;
		oz_ = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::TranslateBy(float x, float y, float z)
	{
		x_ += x;
		y_ += y;
		z_ += z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetTranslation(float x, float y, float z)
	{
		x_ = x;
		y_ = y;
		z_ = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::RotateBy(float x, float y, float z)
	{
		rotation_ *= XMMatrixRotationRollPitchYaw(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetRotation(float x, float y, float z)
	{
		rotation_ = XMMatrixRotationRollPitchYaw(x, 0, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, y, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, 0, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetScale(float x, float y, float z)
	{
		sx_ = x;
		sy_ = y;
		sz_ = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetBlend(float x, float y, float z)
	{
		blend_.x = x;
		blend_.y = y;
		blend_.z = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetUniform(UniformType type, std::string name, float _1, float _2, float _3, float _4)
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
			uniforms_.emplace(name, uniform);
		}
	}

	//-------------------------------------------------------------------------------------------
	UniformType RenderElement::TypeNameToUniformType(std::string type)
	{
		if (strcmp(type.c_str(), "float") == 0) return UniformType::kFloat;
		if (strcmp(type.c_str(), "float2") == 0) return UniformType::kFloat2;
		if (strcmp(type.c_str(), "float3") == 0) return UniformType::kFloat3;
		if (strcmp(type.c_str(), "float4") == 0) return UniformType::kFloat4;

		SNUFF_ASSERT("Tried to set an invalid uniform type!");
		return UniformType::kFloat;
	}

	//-------------------------------------------------------------------------------------------
	std::vector<float> RenderElement::uniforms()
	{
		std::vector<float> temp;
		for (std::map<std::string, ShaderUniform>::iterator it = uniforms_.begin(); it != uniforms_.end(); ++it) {
			switch (it->second.type)
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
	void RenderElement::JSSpawn(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		self->Spawn();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetTranslation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSTranslateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->TranslateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetRotation(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->RotateBy(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetScale(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetScale(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		float x = -wrapper.GetNumber<float>(0);
		float y = -wrapper.GetNumber<float>(1);
		float z = -wrapper.GetNumber<float>(2);

		self->SetOffset(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnTriple<float>(-self->ox_, -self->oy_, -self->oz_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSScale(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnTriple<float>(self->sx_, self->sy_, self->sz_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSTranslation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnTriple<float>(self->x_, self->y_, self->z_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		float roll = atan2(self->rotation_._31, self->rotation_._32);
		float pitch = acos(self->rotation_._33);
		float yaw = -atan2(self->rotation_._13, self->rotation_._23);
		wrapper.ReturnTriple<float>(yaw, pitch, roll);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->texture_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetNormalMap(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->normal_map_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetShader(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->shader_ = environment::content_manager().Get<Shader>(wrapper.GetString(0)).get();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSDestroy(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		self->Destroy();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement, "N");

		self->set_alpha(wrapper.GetNumber<float>(0));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSAlpha(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnNumber<float>(self->alpha());
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetUniform(JS_ARGS)
	{
		JS_SETUP(RenderElement, "SSN");
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
	void RenderElement::JSSetBlend(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NNN");

		self->SetBlend(wrapper.GetNumber<float>(0), wrapper.GetNumber<float>(1), wrapper.GetNumber<float>(2));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSBlend(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		XMFLOAT3 blend = self->blend();
		wrapper.ReturnTriple<float>(blend.x, blend.y, blend.z, "r", "g", "b");
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetVisible(JS_ARGS)
	{
		JS_SETUP(RenderElement, "B");
		self->set_visible(wrapper.GetBool(0));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetName(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");
		self->name_ = wrapper.GetString(0);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSName(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnString(self->name_.c_str());
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::RegisterJS(JS_TEMPLATE)
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