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

		/// Translates the camera by given values
		void TranslateBy(float x, float y, float z);

		/// Sets the translation of the camera
		void SetTranslation(float x, float y, float z);
		
		/// Rotates the camera by given values
		void RotateBy(float x, float y, float z);

		/// Sets the rotation of the camera
		void SetRotation(float x, float y, float z);

		XMVECTOR translation(){ return translation_; }
		XMVECTOR target(){ return target_; }
		std::vector<float> rotation(){ std::vector<float> temp; temp.push_back(yaw_); temp.push_back(pitch_); temp.push_back(roll_); return temp; }

		XMMATRIX& view();
		float fov(){ return fov_; }
		CameraType type(){ return type_; }

	private:
		XMVECTOR up_; //!< Up vector
		XMVECTOR target_; //!< Orientation vector
		XMVECTOR translation_; //!< Translation vector
		XMMATRIX viewMatrix_; //!< The resulting view matrix

		XMMATRIX rotation_;
		const XMVECTOR forward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		const XMVECTOR right_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR camForward_;
		XMVECTOR camRight_;

		float yaw_, pitch_, roll_;
		float moveLeftRight_, moveBackForward_, moveUpDown_;
		float fov_;
		float ox_, oy_, oz_;
		CameraType type_;
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
	};
}