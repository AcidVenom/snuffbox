#include "../../snuffbox/d3d11/d3d11_camera.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------
	Camera::Camera()
	{

	}
	
	//------------------------------------------------------------------------------
	Camera::Camera(JS_ARGS)
	{
		up_ = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		translation_ = XMVectorSet(0.0f, 0.0f, -2.0f, 0.0f);
		orientation_ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	}
	
	//------------------------------------------------------------------------------
	Camera::~Camera()
	{

	}

	//------------------------------------------------------------------------------
	void Camera::TranslateBy(float x, float y, float z)
	{
		translation_ += XMVectorSet(x, y, z, 0.0f);
	}

	//------------------------------------------------------------------------------
	void Camera::RotateBy(float x, float y, float z)
	{
		orientation_ += XMVectorSet(x, y, z, 0.0f);
	}

	//------------------------------------------------------------------------------
	void Camera::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("translateBy", JSTranslateBy),
			JSFunctionRegister("rotateBy", JSRotateBy)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}

	//------------------------------------------------------------------------------
	void Camera::JSTranslateBy(JS_ARGS)
	{
		JS_SETUP(Camera);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->TranslateBy(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(Camera);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->RotateBy(x, y, z);
	}
}