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
		translation_ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		target_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		yaw_ = 0.0f;
		pitch_ = 0.0f;
		camForward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		camRight_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		viewMatrix_ = XMMatrixIdentity();
	}
	
	//------------------------------------------------------------------------------
	Camera::~Camera()
	{

	}

	//------------------------------------------------------------------------------
	void Camera::TranslateBy(float x, float y, float z)
	{
		moveLeftRight_ = x;
		moveBackForward_ = z;
	}

	//------------------------------------------------------------------------------
	void Camera::RotateBy(float x, float y, float z)
	{
		yaw_ += x;

		if (yaw_ > XM_PI*2.0f)
			yaw_ = 0.0f;
		if (yaw_ < 0.0f)
			yaw_ = XM_PI*2.0f;

		pitch_ += y;

		if (pitch_ > XM_PI*2.0f)
			pitch_ = 0.0f;
		if (pitch_ < 0.0f)
			pitch_ = XM_PI*2.0f;
	}

	XMMATRIX& Camera::view()
	{
		rotation_ = XMMatrixRotationRollPitchYaw(pitch_, yaw_, 0);
		target_ = XMVector3TransformCoord(forward_, rotation_);
		target_ = XMVector3Normalize(target_);

		camRight_ = XMVector3TransformCoord(right_, rotation_);
		camForward_ = XMVector3TransformCoord(forward_, rotation_);
		up_ = XMVector3Cross(camForward_, camRight_);

		translation_ += moveLeftRight_*camRight_;
		translation_ += moveBackForward_*camForward_;

		target_ = translation_ + target_;

		viewMatrix_ = XMMatrixLookAtLH(translation_, target_, up_);
		return viewMatrix_;
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