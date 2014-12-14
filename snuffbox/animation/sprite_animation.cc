#include "../../snuffbox/animation/sprite_animation.h"

#include "../../snuffbox/d3d11/elements/render_element.h"
#include "../../snuffbox/d3d11/d3d11_texture.h"

namespace snuffbox
{
	//------------------------------------------------------------------------
	SpriteAnimation::SpriteAnimation(RenderElement* parent, std::vector<SpriteAnimationFrame> frames, std::string texture, std::string name) :
		texture_(nullptr),
		started_(false),
		current_frame_(-1),
		parent_(parent),
		frames_(frames),
		name_(name),
		elapsed_time_(0)
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
	void SpriteAnimation::set_frame(int frame)
	{
		if (frame >= frames_.size() || frame < 0)
		{
			SNUFF_LOG_WARNING(std::string("Frame index " + std::to_string(frame) + " is out of bounds, frame size is " + std::to_string(frames_.size())).c_str());
			frame = static_cast<int>(frames_.size() - 1);
		}

		float total_time = 0.0f;
		float time = 0.0f;

		for (int i = 0; i < frame; ++i)
		{
			time = CalculateFrameTime(i);
			total_time += (1.0f / speed_);
			total_time += frames_.at(i).wait * (1.0f / speed_);
		}
		
		elapsed_time_ = total_time;
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Update(float dt)
	{
		if (started_ == false)
		{
			return;
		}

		float time = 0.0f;
		float total_time = 0.0f;

		current_frame_ = -1;

		for (int i = 0; i < frames_.size(); ++i)
		{
			time = CalculateFrameTime(i);
			total_time += (1.0f / speed_);
			total_time += frames_.at(i).wait * (1.0f / speed_);

			if (time <= elapsed_time_)
			{
				++current_frame_;
			}
		}

		parent_->set_animation_coordinates(frames_.at(current_frame_));

		elapsed_time_ += dt * speed_;

		while (elapsed_time_ >= total_time)
		{
			elapsed_time_ = elapsed_time_ - total_time;
		}

		if (parent_->texture() != texture_)
		{
			parent_->set_texture(texture_);
		}
	}

	//------------------------------------------------------------------------
	void SpriteAnimation::Stop()
	{
		started_ = false;
		elapsed_time_ = 0.0f;
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

		for (int i = 0; i < current_frame_ + 1; ++i)
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
		return current_frame_ + 1;
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