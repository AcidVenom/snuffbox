#pragma once

#include <vector>

namespace snuffbox
{
	class Texture;
	class RenderElement;

	/**
	* @struct snuffbox::SpriteAnimationFrame
	* @brief Contains animation frame data for use with sprite animations
	* @author Daniël Konings
	*/
	struct SpriteAnimationFrame
	{
		SpriteAnimationFrame(int x_, int y_, int w_, int h_, float wait_ = 0) :
			x(x_),
			y(y_),
			w(w_),
			h(h_),
			wait(wait_)
		{}

		int x, y, w, h;
		float wait;
	};

	/**
	* @class snuffbox::SpriteAnimation
	* @brief Handles all animations for a render element
	* @author Daniël Konings
	*/
	class SpriteAnimation
	{
	public:
		/**
		* @enum snuffbox::AnimationEvents
		* @brief An enumerator to trigger events on animations
		* @author Daniël Konings
		*/
		enum AnimationEvents
		{
			kStop,
			kPlay,
			kPause
		};

		/// Default constructor
		SpriteAnimation(RenderElement* parent, std::vector<SpriteAnimationFrame> frames, std::string texture, std::string name);

		/// Default destructor
		~SpriteAnimation();

		/**
		* @brief Updates all animations
		* @param[in] dt (float) The delta timing
		*/
		void Update(float dt);

		/// Starts/resumes the current animation
		void Play();

		/// Stops the animation, resets the current frame to 0
		void Stop();

		/// Pauses the animation, resumable from the current frame
		void Pause();

		/**
		* @return (bool) Is this animation started?
		*/
		bool started();

		/**
		* @return (int) The current frame of this animation
		*/
		int current_frame();

		/**
		* @brief Sets the speed of this animation
		* @param[in] The new speed
		*/
		void set_speed(float speed);

		/**
		* @return (snuffbox::Texture*) The texture this animation is using
		*/
		Texture* texture();

		/**
		* @param[in] frame (int) The frame to check
		* @return (float) The time the frame is triggered at
		*/
		float CalculateFrameTime(int frame);

		/**
		* @return (std::string) The name of this animation
		*/
		std::string name();

	private:
		std::string name_; //!< The name of this animation
		std::vector<SpriteAnimationFrame> frames_; //!< The frames of this animation
		int current_frame_; //!< The current frame of this animation
		Texture* texture_; //!< The texture associated with this animation
		RenderElement* parent_; //!< The parent of this animation
		bool started_; //!< Is this animation started?
		float elapsed_time_; //!< The current elapsed time
		float speed_; //!< The speed of this animation
	};
}