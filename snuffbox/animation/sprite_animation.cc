#include "../../snuffbox/animation/sprite_animation.h"

#include "../../snuffbox/d3d11/elements/render_element.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"

namespace snuffbox
{
	//------------------------------------------------------------------------
	SpriteAnimation::SpriteAnimation(RenderElement* parent, std::vector<SpriteAnimationFrame> frames, std::string texture, std::string name) :
		texture_(nullptr),
		started_(false),
		current_frame_(0),
		parent_(parent),
		frames_(frames),
		name_(name)
	{
		texture_ = environment::content_manager().Get<Texture>(texture).get();
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Play()
	{
		started_ = true;
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Pause()
	{
		started_ = false;
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Update(float dt)
	{
		elapsed_time_ += dt;

		float time = 0.0f;
		float total_time = 0.0f;

		for (int i = 0; i < frames_.size(); ++i)
		{
			time = CalculateFrameTime(i);

			if (time <= elapsed_time_)
			{
				parent_->set_animation_coordinates(frames_.at(i));
				if (parent_->texture() != texture_)
				{
					parent_->set_texture(texture_);
				}
			}

			total_time += time;
		}

		if (elapsed_time_ >= total_time)
		{
			elapsed_time_ = 0.0f;
		}
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Stop()
	{
		started_ = false;
		current_frame_ = 0;
	}

	//------------------------------------------------------------------------
	float SpriteAnimation::CalculateFrameTime(int frame)
	{
		if (frames_.size() == 0)
		{
			return 0.0f;
		}

		float total = 0.0f;
		float speed = 1.0f / speed_;

		for (int i = 0; i < current_frame_; ++i)
		{
			const SpriteAnimationFrame& frame = frames_.at(i);
			
			total += speed;
			total += frame.wait * speed;
		}

		return total;
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::set_speed(float speed)
	{
		speed_ = speed;
	}

	//------------------------------------------------------------------------
	bool SpriteAnimation::started()
	{
		return started_;
	}

	//------------------------------------------------------------------------
	int SpriteAnimation::current_frame()
	{
		return current_frame_;
	}

	//------------------------------------------------------------------------
	Texture* SpriteAnimation::texture()
	{
		return texture_;
	}

	//------------------------------------------------------------------------
	std::string SpriteAnimation::name()
	{
		return name_;
	}

	//------------------------------------------------------------------------
	SpriteAnimation::~SpriteAnimation()
	{

	}
}