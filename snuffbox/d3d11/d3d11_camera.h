#pragma once

#include "../../snuffbox/js/js_state_wrapper.h"
#include <xnamath.h>

namespace snuffbox
{
	/**
	* @class snuffbox::D3D11Camera
	* @brief Contains all matrices for vertex and pixel transformation to simulate a camera
	* @author Daniël Konings
	*/
	class Camera : public JSObject
	{
	public:
		/**
		* @enum snuffbox::Camera::CameraType
		* @brief An enumerator for different camera types
		* @author Daniël Konings
		*/
		enum CameraType
		{
			kOrthographic,
			kPerspective
		};

		/// Default constructor
		Camera();

		/// JavaScript constructor
		Camera(JS_ARGS);

		/// Defeault destructor
		~Camera();

		/**
		* @brief Sets the zoom of the camera
		* @param[in] z (float) The actual zoom
		*/
		void SetZoom(float z){ zoom_ = z; }

		/**
		* @brief Translates the camera by given values
		* @param[in] x (float) Translate by on x
		* @param[in] y (float) Translate by on y
		* @param[in] z (float) Translate by on z
		*/
		void TranslateBy(float x, float y, float z);

		/**
		* @brief Sets the camera translation by given values
		* @param[in] x (float) Translation on x
		* @param[in] y (float) Translation on y
		* @param[in] z (float) Translation on z
		*/
		void SetTranslation(float x, float y, float z);
		
		/**
		* @brief Rotates the camera by given values
		* @param[in] x (float) Rotate by on x axis
		* @param[in] y (float) Rotate by on y axis
		* @param[in] z (float) Rotate by on z axis
		*/
		void RotateBy(float x, float y, float z);

		/**
		* @brief Sets the camera rotation by given values
		* @param[in] x (float) Rotation on x axis
		* @param[in] y (float) Rotation on y axis
		* @param[in] z (float) Rotation on z axis
		*/
		void SetRotation(float x, float y, float z);

		/**
		* @return (XMVECTOR) The vector of translation
		*/
		XMVECTOR translation(){ return translation_; }

		/**
		* @return (XMVECTOR) The target of the camera as a vector relative from the camera
		*/
		XMVECTOR target(){ return target_; }

		/**
		* @return (std::vector<float>) The yaw, pitch and roll of the camera stored in a vector under [0], [1] and [2]
		*/
		std::vector<float> rotation(){ std::vector<float> temp; temp.push_back(yaw_); temp.push_back(pitch_); temp.push_back(roll_); return temp; }

		/**
		* @return (XMMATRIX&) The view matrix of the camera
		*/
		XMMATRIX& view_matrix();

		/**
		* @return (float) The field of view of the camera
		*/
		float fov(){ return fov_; }

		/**
		* @return (snuffbox::CameraType) The camera type the camera is using
		*/
		CameraType type(){ return type_; }

		/**
		*	@return (float) The zoom of the camera
		*/
		float zoom(){ return zoom_; }

	private:
		XMVECTOR up_; //!< Up vector
		XMVECTOR target_; //!< Orientation vector
		XMVECTOR translation_; //!< Translation vector
		XMMATRIX view_matrix_; //!< The resulting view matrix

		XMMATRIX rotation_; //!< The rotation matrix
		const XMVECTOR forward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); //!< The constant forward vector
		const XMVECTOR right_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); //!< The constant right vector
		XMVECTOR cam_forward_; //!< The current relative forward vector of the camera
		XMVECTOR cam_right_; //!< The current relative right vector of the camera

		float yaw_; //!< The yaw of the camera
		float pitch_; //!< The pitch of the camera
		float roll_; //!< The roll of the camera
		float move_left_right_; //!< The move left/right modifier
		float move_back_forward_; //!< The move back/forward modifier
		float move_up_down_; //!< The move up/down modifier
		float fov_; //!< The field of view of the camera
		float zoom_; //!< The zoom of the camera
		CameraType type_; //!< The camera type
	public:
		JS_NAME(Camera);
		static void RegisterJS(JS_TEMPLATE);
		static void JSTranslateBy(JS_ARGS);
		static void JSRotateBy(JS_ARGS);
		static void JSSetTranslation(JS_ARGS);
		static void JSSetRotation(JS_ARGS);
		static void JSTranslation(JS_ARGS);
		static void JSRotation(JS_ARGS);
		static void JSSetFov(JS_ARGS);
		static void JSSetType(JS_ARGS);
		static void JSSetZoom(JS_ARGS);
	};
}