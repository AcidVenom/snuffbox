#include "../../../snuffbox/d3d11/elements/render_element.h"
#include "../../../snuffbox/d3d11/d3d11_render_target.h"

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
		element_type_(type),
		shader_(environment::content_manager().Get<Shader>("shaders/base.fx").get()),
		destroyed_(true),
		spawned_(false),
		distance_from_camera_(0.0f),
		alpha_(1.0f),
		blend_(1.0f, 1.0f, 1.0f),
		visible_(true),
		yaw_(0.0f),
		pitch_(0.0f),
		roll_(0.0f),
		animation_(nullptr),
		anim_coords_(0.0f, 0.0f, 1.0f, 1.0f),
    target_(nullptr),
		sampler_type_(SamplerState::kLinear)
	{
		size_[0] = 1.0f;
		size_[1] = 1.0f;
		name_ = "RenderElement_" + std::to_string(++element_uuid);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderElement::RemoveFromRenderer()
	{
		if (!environment::has_render_device() || target_ == nullptr)
		{
			return;
		}
		std::vector<RenderElement*>* vec = &target_->opaque_elements();
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
		else if (element_type() == ElementTypes::kWidget || element_type() == ElementTypes::kText)
		{
			vec = &target_->ui_elements();

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
			vec = &target_->render_elements();

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
		float sx = sx_*size_[0];
		float sz = sz_*size_[1];

		world_matrix_ = XMMatrixScaling(sx, sy_, sz) *
			XMMatrixTranslation(ox_*sx, oy_*sy_, oz_*sz) *
			rotation_ *
			XMMatrixTranslation(x_, y_, z_);
		return world_matrix_;
	}

	//-------------------------------------------------------------------------------------------
	bool RenderElement::spawned()
	{
		return spawned_;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::Spawn(std::string target)
	{
		target_ = environment::render_device().get_target(target);
		if (destroyed_ == true)
		{
			target_->render_queue().push(this);
			spawned_ = true;
		}
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::set_destroyed(bool value)
	{
		destroyed_ = value;
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
		spawned_ = false;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetOffset(float x, float y, float z)
	{
		if (environment::render_settings().y_down() == false)
		{
			z = -z;
		}
		ox_ = -x;
		oy_ = -y;
		oz_ = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::TranslateBy(float x, float y, float z)
	{
		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		x_ += x;
		y_ += y;
		z_ += z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetTranslation(float x, float y, float z)
	{
		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		x_ = x;
		y_ = y;
		z_ = z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::RotateBy(float x, float y, float z)
	{
		rotation_ *= XMMatrixRotationRollPitchYaw(-x, -y, -z);

		yaw_ += x;
		pitch_ += y;
		roll_ += z;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetRotation(float x, float y, float z)
	{
		rotation_ = XMMatrixRotationRollPitchYaw(-x, 0, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, -y, 0);
		rotation_ *= XMMatrixRotationRollPitchYaw(0, 0, -z);

		yaw_ = x;
		pitch_ = y;
		roll_ = z;
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

    OnChange(RenderMessage::kBlend);
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
	void RenderElement::set_animation_coordinates(const SpriteAnimationFrame& frame)
	{
		float width = static_cast<float>(frame.w) / animation_->texture()->width();
		float height = static_cast<float>(frame.h) / animation_->texture()->height();

		float offsetX = static_cast<float>(frame.x) / animation_->texture()->width();
		float offsetY = static_cast<float>(frame.y) / animation_->texture()->height();

		anim_coords_.x = offsetX;
		anim_coords_.y = offsetY;
		anim_coords_.z = width;
		anim_coords_.w = height;
	}

	//-------------------------------------------------------------------------------------------
	XMFLOAT4 RenderElement::animation_coordinates()
	{
		return anim_coords_;
	}

	//-------------------------------------------------------------------------------------------
	SpriteAnimation* RenderElement::animation()
	{
		return animation_;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::UpdateAnimation(float dt)
	{
		if (animation_ != nullptr)
		{
			animation_->Update(dt);
		}
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::AddAnimation(std::string name, std::string texture, std::vector<SpriteAnimationFrame> frames)
	{
		if (AnimationExists(name) == true)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' already exists").c_str());
			return;
		}
		
		animations_.emplace(name, SpriteAnimation(this, frames, texture, name));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetAnimation(std::string name)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return;
		}

		animation_ = &animations_.find(name)->second;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::TriggerAnimationEvent(std::string name, SpriteAnimation::AnimationEvents evt)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return;
		}

		SpriteAnimation* ptr = (&animations_.find(name)->second);

		switch (evt)
		{
		case SpriteAnimation::AnimationEvents::kPause:
			ptr->Pause();
			break;
		case SpriteAnimation::AnimationEvents::kPlay:
			ptr->Play();
			break;
		case SpriteAnimation::AnimationEvents::kStop:
			ptr->Stop();
			break;
		}
	}

	//-------------------------------------------------------------------------------------------
	std::vector<SpriteAnimationFrame> RenderElement::GetFramesFromArray(Local<Array> arr)
	{
		std::vector<SpriteAnimationFrame> temp;

		for (unsigned int i = 0; i < arr->Length(); ++i)
		{
			Local<Value> val = arr->Get(i);

			if (val->IsObject())
			{
				Local<Object> frame = val->ToObject();
				Local<Value> x = frame->Get(String::NewFromUtf8(JS_ISOLATE, "x"));
				Local<Value> y = frame->Get(String::NewFromUtf8(JS_ISOLATE, "y"));
				Local<Value> width = frame->Get(String::NewFromUtf8(JS_ISOLATE, "width"));
				Local<Value> height = frame->Get(String::NewFromUtf8(JS_ISOLATE, "height"));

				if (x.IsEmpty() || y.IsEmpty() || width.IsEmpty() || height.IsEmpty())
				{
					SNUFF_LOG_ERROR("Not all required fields have been entered for a sprite animation frame");
					return temp;
				}

				if (x->IsNumber() && y->IsNumber() && width->IsNumber() && height->IsNumber())
				{
					Local<Value> wait = frame->Get(String::NewFromUtf8(JS_ISOLATE, "wait"));
					float w = 0.0f;
					if (!wait.IsEmpty() && wait->IsNumber())
					{
						w = static_cast<float>(wait->ToNumber()->NumberValue());
					}
					SpriteAnimationFrame frame(
						x->ToNumber()->Int32Value(),
						y->ToNumber()->Int32Value(),
						width->ToNumber()->Int32Value(),
						height->ToNumber()->Int32Value(),
						w
						);

					temp.push_back(frame);
				}
				else
				{
					SNUFF_LOG_ERROR("x, y, width or height is not a number in sprite animation!");
					return temp;
				}
			}
			else
			{
				SNUFF_LOG_ERROR("Wrong formatting for sprite animation!");
				return temp;
			}
		}

		return temp;
	}

	//-------------------------------------------------------------------------------------------
	bool RenderElement::AnimationExists(std::string name)
	{
		if (animations_.find(name) != animations_.end())
		{
			return true;
		}
		return false;
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetAnimationSpeed(std::string name, float speed)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return;
		}

		(&animations_.find(name)->second)->set_speed(speed);
	}

	//-------------------------------------------------------------------------------------------
	bool RenderElement::IsAnimationPlaying(std::string name)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return false;
		}

		return (&animations_.find(name)->second)->started();
	}

	//-------------------------------------------------------------------------------------------
	int RenderElement::CurrentAnimationFrame(std::string name)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return false;
		}

		return (&animations_.find(name)->second)->current_frame();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::SetFrame(std::string name, int frame)
	{
		if (AnimationExists(name) == false)
		{
			SNUFF_LOG_ERROR(std::string("Animation with name '" + name + "' does not exist").c_str());
			return;
		}

		(&animations_.find(name)->second)->set_frame(frame);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSpawn(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->Spawn(wrapper.GetString(0));
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
		JS_SETUP(RenderElement, "NN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = y;

		if (wrapper.argLength() > 2)
		{
			z = wrapper.GetNumber<float>(2);
		}

		self->SetScale(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);

		float z = y;

		if (wrapper.argLength() > 2)
		{
			z = wrapper.GetNumber<float>(2);
		}

		self->SetOffset(x, y, z);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSOffset(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		float z = -self->oz_;

		if (environment::render_settings().y_down() == true)
		{
			z = -z;
		}

		wrapper.ReturnTriple<float>(-self->ox_, -self->oy_, z);
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

		float y = self->y_;

		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		wrapper.ReturnTriple<float>(self->x_, y, self->z_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSRotation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnTriple<float>(self->yaw_, self->pitch_, self->roll_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

    if (wrapper.GetValue(0).IsEmpty() || wrapper.GetValue(0)->IsUndefined())
    {
      self->texture_ = nullptr;
    }
    else
    {
      self->texture_ = environment::content_manager().Get<Texture>(wrapper.GetString(0)).get();
    }
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
	void RenderElement::JSVisible(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnBool(self->visible());
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
	void RenderElement::JSSetSize(JS_ARGS)
	{
		JS_SETUP(RenderElement, "NN");
		self->size_[0] = wrapper.GetNumber<float>(0);
		self->size_[1] = wrapper.GetNumber<float>(1);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSize(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnTuple<float>(self->size_[0], self->size_[1], "w", "h");
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSDestroyed(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");
		wrapper.ReturnBool(self->destroyed_);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetToTexture(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		if (self->texture_ == nullptr)
		{
			return;
		}

		self->size_[0] = self->texture_->width();
		self->size_[1] = self->texture_->height();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSAddPass(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		Shader* pass = environment::content_manager().Get<Shader>(wrapper.GetString(0)).get();
		self->passes().push_back(pass);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSClearPasses(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		self->passes().clear();
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSTextureMetrics(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnTuple<int>(self->texture()->width(), self->texture()->height(), "width", "height");
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSAddAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "SSA");

		Local<Array> arr = wrapper.GetValue(2).As<Array>();
		std::vector<SpriteAnimationFrame> frames = self->GetFramesFromArray(arr);

		self->AddAnimation(wrapper.GetString(0), wrapper.GetString(1), frames);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->SetAnimation(wrapper.GetString(0));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSStopAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->TriggerAnimationEvent(wrapper.GetString(0), SpriteAnimation::AnimationEvents::kStop);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSPauseAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->TriggerAnimationEvent(wrapper.GetString(0), SpriteAnimation::AnimationEvents::kPause);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSPlayAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		self->TriggerAnimationEvent(wrapper.GetString(0), SpriteAnimation::AnimationEvents::kPlay);
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetAnimationSpeed(JS_ARGS)
	{
		JS_SETUP(RenderElement, "SN");

		self->SetAnimationSpeed(wrapper.GetString(0), wrapper.GetNumber<float>(1));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSAnimationPlaying(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		wrapper.ReturnBool(self->IsAnimationPlaying(wrapper.GetString(0)));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSCurrentAnimation(JS_ARGS)
	{
		JS_SETUP(RenderElement, "V");

		wrapper.ReturnString(self->animation_->name().c_str());
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSCurrentFrame(JS_ARGS)
	{
		JS_SETUP(RenderElement, "S");

		wrapper.ReturnNumber<int>(self->CurrentAnimationFrame(wrapper.GetString(0)));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetFrame(JS_ARGS)
	{
		JS_SETUP(RenderElement, "SN");

		self->SetFrame(wrapper.GetString(0), wrapper.GetNumber<int>(1));
	}

	//-------------------------------------------------------------------------------------------
	void RenderElement::JSSetSampling(JS_ARGS)
	{
		JS_SETUP(RenderElement, "N");

		self->sampler_type_ = static_cast<SamplerState>(wrapper.GetNumber<int>(0));
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
			JSFunctionRegister("setShader", JSSetShader),
			JSFunctionRegister("destroy", JSDestroy),
			JSFunctionRegister("spawn", JSSpawn),
			JSFunctionRegister("alpha", JSAlpha),
			JSFunctionRegister("setAlpha", JSSetAlpha),
			JSFunctionRegister("setUniform", JSSetUniform),
			JSFunctionRegister("setBlend", JSSetBlend),
			JSFunctionRegister("blend", JSBlend),
			JSFunctionRegister("setName", JSSetName),
			JSFunctionRegister("name", JSName),
			JSFunctionRegister("setSize", JSSetSize),
			JSFunctionRegister("size", JSSize),
			JSFunctionRegister("destroyed", JSDestroyed),
			JSFunctionRegister("setToTexture", JSSetToTexture),
			JSFunctionRegister("addPass", JSAddPass),
			JSFunctionRegister("clearPasses", JSClearPasses),
			JSFunctionRegister("textureMetrics", JSTextureMetrics),
			JSFunctionRegister("setVisible", JSSetVisible),
			JSFunctionRegister("visible", JSVisible),
			JSFunctionRegister("addAnimation", JSAddAnimation),
			JSFunctionRegister("setAnimation", JSSetAnimation),
			JSFunctionRegister("playAnimation", JSPlayAnimation),
			JSFunctionRegister("stopAnimation", JSStopAnimation),
			JSFunctionRegister("pauseAnimation", JSPauseAnimation),
			JSFunctionRegister("setAnimationSpeed", JSSetAnimationSpeed),
			JSFunctionRegister("animationPlaying", JSAnimationPlaying),
			JSFunctionRegister("currentAnimation", JSCurrentAnimation),
			JSFunctionRegister("currentFrame", JSCurrentFrame),
			JSFunctionRegister("setFrame", JSSetFrame),
			JSFunctionRegister("setSampling", JSSetSampling)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}
}