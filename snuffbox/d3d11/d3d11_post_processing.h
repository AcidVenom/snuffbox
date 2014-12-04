#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	class Shader;
	/**
	* @class snuffbox::PostProcessing
	* @brief Handles all post processing shader management, including passes
	* @author Daniël Konings
	*/
	class PostProcessing : public JSObject
	{
	public:
		/// Default constructor
		PostProcessing();

		/// Default destructor
		~PostProcessing();

		/// Sets the current post processing shader
		void SetPostProcessingShader(std::string path);

		/// Adds a post processing pass
		void AddPostProcessingPass(std::string path);

		/// Removes a post processing pass by index
		void RemovePostProcessingPass(int idx);

		/// Clears all post processing passes
		void ClearPostProcessingPasses();

		/// Returns the current shader
		Shader* shader();

		/// Returns the vector of passes
		std::vector<Shader*>& passes();

	private:
		Shader*												shader_; //!< The current post processing shader
		std::vector<Shader*>					passes_; //!< The post processing passes
		std::vector<float>						uniforms_; //!< The uniforms for post processing

	public:
		JS_NAME(PostProcessing);

		static void RegisterJS(JS_TEMPLATE);

	private:
		static void JSSetShader(JS_ARGS);
		static void JSAddPass(JS_ARGS);
		static void JSRemovePass(JS_ARGS);
		static void JSClearPasses(JS_ARGS);
	};
}