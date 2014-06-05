#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/js/js_wrapper.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------
	Camera::Camera()
	{

	}
	
	//------------------------------------------------------------------------------
	Camera::Camera(JS_ARGS)
	{
		JSWrapper wrapper(args);
		up_ = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		translation_ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		target_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		yaw_ = 0.0f;
		pitch_ = 0.0f;
		roll_ = 0.0f;
		camForward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		camRight_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		viewMatrix_ = XMMatrixIdentity();

		if (strcmp(wrapper.GetString(0).c_str(), "orthographic") == 0)
		{
			type_ = CameraType::kOrthographic;
		}

		if (strcmp(wrapper.GetString(0).c_str(), "perspective") == 0)
		{
			type_ = CameraType::kPerspective;
		}

		fov_ = 80 * XM_PI / 180;
	}
	
	//------------------------------------------------------------------------------
	Camera::~Camera()
	{

	}

	//------------------------------------------------------------------------------
	void Camera::TranslateBy(float x, float y, float z)
	{
		moveLeftRight_ = x;
		moveUpDown_ = y;
		moveBackForward_ = z;
	}

	//------------------------------------------------------------------------------
	void Camera::SetTranslation(float x, float y, float z)
	{
		translation_ = XMVectorSet(x, y, z, XMVectorGetW(translation_));
	}

	//------------------------------------------------------------------------------
	void Camera::SetRotation(float x, float y, float z)
	{
		yaw_ = x;
		pitch_ = y;
		roll_ = z;
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

		roll_ += z;

		if (roll_ > XM_PI*2.0f)
			roll_ = 0.0f;
		if (roll_ < 0.0f)
			roll_ = XM_PI*2.0f;
	}

	XMMATRIX& Camera::view()
	{
		rotation_ = XMMatrixRotationRollPitchYaw(yaw_, pitch_, roll_);
		target_ = XMVector3TransformCoord(forward_, rotation_);
		target_ = XMVector3Normalize(target_);

		camRight_ = XMVector3TransformCoord(right_, rotation_);
		camForward_ = XMVector3TransformCoord(forward_, rotation_);
		up_ = XMVector3Cross(camForward_, camRight_);

		translation_ += moveLeftRight_ * camRight_;
		translation_ += moveBackForward_ * camForward_;
		translation_ += moveUpDown_ * up_;

		target_ = translation_ + target_;

		viewMatrix_ = XMMatrixLookAtLH(translation_, target_, up_);
		moveLeftRight_ = 0.0f;
		moveBackForward_ = 0.0f;
		moveUpDown_ = 0.0f;
		return viewMatrix_;
	}

	//------------------------------------------------------------------------------
	void Camera::RegisterJS(JS_TEMPLATE)
	{
		JS_CREATE_SCOPE;

		JSFunctionRegister funcs[] = {
			JSFunctionRegister("translateBy", JSTranslateBy),
			JSFunctionRegister("setTranslation", JSSetTranslation),
			JSFunctionRegister("rotateBy", JSRotateBy),
			JSFunctionRegister("setRotation", JSSetRotation),
			JSFunctionRegister("rotation", JSRotation),
			JSFunctionRegister("translation", JSTranslation),
			JSFunctionRegister("setFov", JSSetFov),
			JSFunctionRegister("setType", JSSetType)
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

	//------------------------------------------------------------------------------
	void Camera::JSSetTranslation(JS_ARGS)
	{
		JS_SETUP(Camera);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetTranslation(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetRotation(JS_ARGS)
	{
		JS_SETUP(Camera);

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetRotation(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSRotation(JS_ARGS)
	{
		JS_SETUP(Camera);
		std::vector<float> rotation = self->rotation();
		float x = rotation[0];
		float y = rotation[1];
		float z = rotation[2];

		wrapper.ReturnTriple(x, y, z, "yaw", "pitch", "roll");
	}

	//------------------------------------------------------------------------------
	void Camera::JSTranslation(JS_ARGS)
	{
		JS_SETUP(Camera);
		XMVECTOR translation = self->translation();

		float x = XMVectorGetX(translation);
		float y = XMVectorGetY(translation);
		float z = XMVectorGetZ(translation);

		wrapper.ReturnTriple(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetFov(JS_ARGS)
	{
		JS_SETUP(Camera);

		self->fov_ = wrapper.GetNumber<float>(0);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetType(JS_ARGS)
	{
		JS_SETUP(Camera);
		std::string typing = wrapper.GetString(0);
		if (strcmp(typing.c_str(), "perspective") == 0)
		{
			self->type_ = Camera::CameraType::kPerspective;
		}
		if (strcmp(typing.c_str(), "orthographic") == 0)
		{
			self->type_ = Camera::CameraType::kOrthographic;
		}
	}
}