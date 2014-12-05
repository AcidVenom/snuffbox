#pragma once

#include <map>
#include <vector>

namespace snuffbox
{
	class Shader;
  enum UniformType;
  struct ShaderUniform;

	/**
	* @class snuffbox::PostProcessing
	* @brief Handles all post processing shader management, including passes
	* @author Daniël Konings
	*/
	class PostProcessing
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

    /// Returns the uniforms of the post processing effects
    std::vector<float> uniforms();

    /**
    * @brief Sets a uniform for this element
    * @param[in] type (snuffbox::UniformType) The type of the uniform
    * @param[in] name (std::string) The underlying name of this uniform (only used for managing, not actual shader name)
    * @param[in] _1, _2, _3, _4 (float) Optional starting from _2, this depends on the uniform type that was given as 1st parameter
    */
    void SetUniform(UniformType type, std::string name, float _1 = 0.0f, float _2 = 0.0f, float _3 = 0.0f, float _4 = 0.0f);

	private:
		Shader*												        shader_; //!< The current post processing shader
		std::vector<Shader*>					        passes_; //!< The post processing passes
    std::map<std::string, ShaderUniform>	uniforms_;	//!< Uniforms for the constant buffer of the shader
	};
}