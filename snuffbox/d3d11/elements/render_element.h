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
	/// A static unique user ID for Render Elements
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
  * @enum snuffbox::RenderMessage
  * @brief Render messages to interact with
  * @author Daniël Konings
  */
  enum RenderMessage
  {
    kAlpha,
    kBlend
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
      kQuad2D,
			kBillboard,
			kWidget,
			kMesh,
      kText,
			kPolygon
		};

		/**
		* @brief Constructs an element through a type
		* @param[in] type (snuffbox::ElementTypes) The type of this render element
		*/
		RenderElement(ElementTypes type);

		/// Default destructor
		~RenderElement(){}

		/// Removes this element from the display device
		void RemoveFromRenderer();

		/// Sets the buffers of the render element
		virtual void SetBuffers() = 0;

		/**
		* @return (std::vector<snuffbox::Vertex>&) The vertices of the render element
		*/
		std::vector<Vertex>& vertices(){ return vertices_; };

		/**
		* @return (std::vector<unsigned int>&) The indices of the render element
		*/
		std::vector<unsigned int>& indices(){ return indices_; };

		/**
		* @brief Calculates the world matrix, then returns it
		* @return (XMMATRIX&) The calculated world matrix
		*/
		virtual XMMATRIX& world_matrix(Camera* camera);

		/**
		* @return (XMMATRIX) The scaling matrix
		*/
		XMMATRIX scaling(){ return XMMatrixScaling(sx_*size_[0], sy_, sz_*size_[1]); }

		/**
		* @return (XMMATRIX) The 2D scaling matrix
		*/
		XMMATRIX scaling_2d(){ return XMMatrixScaling(sx_*size_[0], sy_*size_[1], sy_); }

		/**
		* @return (XMMATRIX) The 2D scaling matrix without size
		*/
		XMMATRIX scaling_2d_no_size(){ return XMMatrixScaling(sx_, sy_, sy_); }

		/**
		* @return (XMMATRIX) The offset matrix
		*/
		XMMATRIX offset(){ return XMMatrixTranslation(ox_*sx_*size_[0], oy_*sy_, oz_*sz_*size_[1]); }

		/**
		* @return (XMMATRIX) The 2D offset matrix
		*/
		virtual XMMATRIX offset_2d(){ return XMMatrixTranslation(ox_*sx_*size_[0], -oy_*sy_*size_[1], oy_*sz_); }

		/**
		* @return (XMVECTOR) The scaling vector
		*/
		XMVECTOR scale(){ return XMVectorSet(sx_, sy_, sz_, 0); }
		/**
		* @return (XMVECTOR) The size vector
		*/
		XMVECTOR size(){ return XMVectorSet(size_[0], 0, size_[1], 0); }
		/**
		* @return (XMMATRIX) The rotation matrix
		*/
		XMMATRIX rotation(){ return rotation_; }

		/**
		* @return (bool) Is this render element destroyed?
		*/
		bool destroyed();

		/**
		* @brief Sets the translation on the X, Y, Z plane
		* @param[in] x (float) The x value
		* @param[in] y (float) The y value
		* @param[in] z (float) The z value
		*/
		void SetTranslation(float x, float y, float z);

		/**
		* @brief Translates by on the X, Y, Z plane
		* @param[in] x (float) The x value
		* @param[in] y (float) The y value
		* @param[in] z (float) The z value
		*/
		void TranslateBy(float x, float y, float z);

		/**
		* @brief Sets the rotation on the X, Y, Z axes
		* @param[in] x (float) The x value
		* @param[in] y (float) The y value
		* @param[in] z (float) The z value
		*/
		void SetRotation(float x, float y, float z);

		/**
		* @brief Rotates by on the X, Y, Z axes
		* @param[in] x (float) The x value
		* @param[in] y (float) The y value
		* @param[in] z (float) The z value
		*/
		void RotateBy(float x, float y, float z);

		/**
		* @brief Sets the scale
		* @param[in] x (float) The x scaling
		* @param[in] y (float) The y scaling
		* @param[in] z (float) The z scaling
		*/
		void SetScale(float x, float y, float z);

		/**
		* @brief Sets the offset
		* @param[in] x (float) The x offset
		* @param[in] y (float) The y offset
		* @param[in] z (float) The z offset
		*/
		void SetOffset(float x, float y, float z);

		/**
		* @brief Sets the blend colour
		* @param[in] r (float) The r value (0.0 - 1.0)
		* @param[in] g (float) The g value (0.0 - 1.0)
		* @param[in] b (float) The b value (0.0 - 1.0)
		*/
		void SetBlend(float x, float y, float z);

		/**
		* @return (XMFLOAT3) The blend colour of this render element
		*/
		XMFLOAT3 blend(){ return blend_; }

		/**
		* @brief Sets the distance from the camera
		* @param[in] distance (float) The distance to be set
		*/
		void set_distance_from_camera(float distance){ distance_from_camera_ = distance; }

		/**
		* @return (float) The distance from the camera
		*/
		float distance_from_camera(){ return distance_from_camera_; }

		/// Destroys the render element
		void Destroy();

		/// Spawns the render element
		void Spawn();

		/**
		* @return (XMVECTOR) The translation as a vector
		*/
		XMVECTOR translation(){ return XMVectorSet(x_, y_, z_, 0.0f); }

		/**
		* @return (snuffbox::Texture*) The texture of this element
		*/
		Texture* texture(){ return texture_; }

		/**
		* @return (snuffbox::Texture*) The normal map of this element
		*/
		Texture* normal_map(){ return normal_map_; }

		/**
		* @return (snuffbox::Shader*) The shader of this element
		*/
		Shader* shader(){ return shader_; }

		/**
		* @return (snuffbox::VertexBufferType*) The vertex buffer type
		*/
		virtual VertexBufferType type() = 0;

		/**
		* @return (snuffbox::ElementTypes&) The element type
		*/
		ElementTypes& element_type(){ return element_type_; }

		/**
		* @brief Sets the alpha of this element
		* @param[in] alpha (float) The alpha to set
		*/
    void set_alpha(float alpha){ alpha_ = alpha; OnChange(RenderMessage::kAlpha); }

		/**
		* @return (float) The alpha of this element
		*/
		float alpha(){ return alpha_; }

		/**
		* @brief Sets a uniform for this element
		* @param[in] type (snuffbox::UniformType) The type of the uniform
		* @param[in] name (std::string) The underlying name of this uniform (only used for managing, not actual shader name)
		* @param[in] _1, _2, _3, _4 (float) Optional starting from _2, this depends on the uniform type that was given as 1st parameter
		*/
		void SetUniform(UniformType type, std::string name, float _1 = 0.0f, float _2 = 0.0f, float _3 = 0.0f, float _4 = 0.0f);

		/// Removes all uniforms for the next loop
		void ClearUniforms(){ uniforms_.clear(); }

		/**
		* @brief Converts a given uniform type name to a uniform type
		* @param[in] type (std::string) The uniform type name to convert
		* @return (UniformType) The converted type
		*/
		static UniformType TypeNameToUniformType(std::string type);

		/**
		* @return (std::vector<float>) A list of uniforms to assign to the constant buffer
		*/
		std::vector<float> uniforms();

		/**
		* @brief Sets the visibility of this render element
		* @param[in] visible (bool) The visibility to set
		*/
		void set_visible(bool visible){ visible_ = visible; }

		/**
		* @return (bool) Is this element visible or not?
		*/
		bool visible(){ return visible_; }

		/**
		* @return (float) The Z index of this element
		*/
		float z(){ return z_; }

		/**
		* @brief Sets if this element is destroyed or not
		* @param[in] value (bool) The destroyed state to set
		*/
		void set_destroyed(bool value);

		/**
		* @return (bool) Is this element spawned, but not drawn yet?
		*/
		bool spawned();

		/// Sets the render texture
		void set_texture(Texture* texture){ texture_ = texture; }

    /**
    * @brief When this render element changes, callback to this function
    * @param[in] msg (snuffbox::RenderMessage) The message type
    */
    virtual void OnChange(RenderMessage msg){}

		/**
		* @return (std::vector<snuffbox::Shader*>&) A list of shader passe
		*/
		std::vector<Shader*>& passes(){ return passes_; }

	protected:
		std::vector<Vertex>										vertices_; //!< The vertices
		std::vector<unsigned int>							indices_; //!< The indices
		XMMATRIX															world_matrix_; //!< The world matrix
		XMMATRIX															rotation_;	//!< Rotation

		float																	x_;	//!< The x translation
		float																	y_; //!< The y translation
		float																	z_; //!< The z translation

		float																	ox_; //!< The x offset
		float																	oy_; //!< The y offset
		float																	oz_; //!< The z offset

		float																	sx_; //!< The x scale
		float																	sy_; //!< The y scale
		float																	sz_; //!< The z scale

		float																	yaw_; //!< The yaw rotation
		float																	pitch_; //!< The pitch rotation
		float																	roll_; //!< The roll rotation

		float																	size_[2]; //!< The widget's size

		Texture*															texture_;	//!< The texture of this render element
		Texture*															normal_map_; //!< The normal map of this render element
		Shader*																shader_; //!< The current shader used by this element
		std::vector<Shader*>									passes_; //!< Shader passes used by this element
		ElementTypes													element_type_;	//!< The type of this render element
		bool																	destroyed_; //!< Is this element destroyed?
		float																	distance_from_camera_; //!< The distance from the camera
		float																	alpha_;	//!< The alpha value of this whole element
		std::map<std::string, ShaderUniform>	uniforms_;	//!< Uniforms for the constant buffer of the shader
		XMFLOAT3															blend_;	//!< The blend color of this render element
		bool																	visible_; //!< Is this element visible?
		std::string														name_;	 //!< The render element name
		bool																	spawned_; //!< Has this element already been spawned?

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
		static void JSSetSize(JS_ARGS);
		static void JSSize(JS_ARGS);
		static void JSDestroyed(JS_ARGS);
		static void JSSetToTexture(JS_ARGS);
		static void JSAddPass(JS_ARGS);
		static void JSClearPasses(JS_ARGS);
	};
}