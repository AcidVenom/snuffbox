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
		/// Default constructor
		Camera();

		/// JavaScript constructor
		Camera(JS_ARGS);

		/// Defeault destructor
		~Camera();

		/// Returns the up vector
		XMVECTOR& up(){ return up_; }

		/// Returns the orientation vector
		XMVECTOR& orientation(){ return orientation_; }

		/// Returns the translation vector
		XMVECTOR& translation(){ return translation_; }

		/// Translates the camera by given values
		void TranslateBy(float x, float y, float z);
		
		/// Rotates the camera by given values
		void RotateBy(float x, float y, float z);

	private:
		XMVECTOR up_; ///< Up vector
		XMVECTOR orientation_; ///< Orientation vector
		XMVECTOR translation_; ///< Translation vector
	public:
		JS_NAME(Camera);
		static void RegisterJS(JS_TEMPLATE);
		static void JSTranslateBy(JS_ARGS);
		static void JSRotateBy(JS_ARGS);
	};
}