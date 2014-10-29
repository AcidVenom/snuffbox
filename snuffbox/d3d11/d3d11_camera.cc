#include "../../snuffbox/d3d11/d3d11_camera.h"
#include "../../snuffbox/js/js_wrapper.h"
#include "../../snuffbox/d3d11/d3d11_settings.h"

namespace snuffbox
{
	//------------------------------------------------------------------------------
	Camera::Camera()
	{

	}
	
	//------------------------------------------------------------------------------
	Camera::Camera(JS_ARGS)
	{
		JS_CHECK_PARAMS("S");
		up_ = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		translation_ = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		target_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		yaw_ = 0.0f;
		pitch_ = 0.0f;
		roll_ = 0.0f;
		cam_forward_ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		cam_right_ = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		view_matrix_ = XMMatrixIdentity();
		zoom_ = 1;

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
		move_left_right_ = x;
		move_up_down_ = y;
		move_back_forward_ = z;
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

	//------------------------------------------------------------------------------
	XMMATRIX& Camera::view_matrix()
	{
		rotation_ = XMMatrixRotationRollPitchYaw(yaw_, pitch_, roll_);
		target_ = XMVector3TransformCoord(forward_, rotation_);
		target_ = XMVector3Normalize(target_);

		cam_right_ = XMVector3TransformCoord(right_, rotation_);
		cam_forward_ = XMVector3TransformCoord(forward_, rotation_);
		up_ = XMVector3Cross(cam_forward_, cam_right_);

		translation_ += move_left_right_ * cam_right_;
		translation_ += move_back_forward_ * cam_forward_;
		translation_ += move_up_down_ * up_;

		target_ = translation_ + target_;

		view_matrix_ = XMMatrixLookAtLH(translation_, target_, up_);
		move_left_right_ = 0.0f;
		move_back_forward_ = 0.0f;
		move_up_down_ = 0.0f;
		return view_matrix_;
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
			JSFunctionRegister("setType", JSSetType),
			JSFunctionRegister("setZoom", JSSetZoom)
		};

		JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, true);
	}

	//------------------------------------------------------------------------------
	void Camera::JSTranslateBy(JS_ARGS)
	{
		JS_SETUP(Camera,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->TranslateBy(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSRotateBy(JS_ARGS)
	{
		JS_SETUP(Camera,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->RotateBy(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetTranslation(JS_ARGS)
	{
		JS_SETUP(Camera,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);
		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		self->SetTranslation(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetRotation(JS_ARGS)
	{
		JS_SETUP(Camera,"NNN");

		float x = wrapper.GetNumber<float>(0);
		float y = wrapper.GetNumber<float>(1);
		float z = wrapper.GetNumber<float>(2);

		self->SetRotation(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSRotation(JS_ARGS)
	{
		JS_SETUP(Camera,"V");
		std::vector<float> rotation = self->rotation();
		float x = rotation[0];
		float y = rotation[1];
		float z = rotation[2];

		wrapper.ReturnTriple(x, y, z, "yaw", "pitch", "roll");
	}

	//------------------------------------------------------------------------------
	void Camera::JSTranslation(JS_ARGS)
	{
		JS_SETUP(Camera,"V");
		XMVECTOR translation = self->translation();

		float x = XMVectorGetX(translation);
		float y = XMVectorGetY(translation);
		float z = XMVectorGetZ(translation);

		if (environment::render_settings().y_down() == true)
		{
			y = -y;
		}
		wrapper.ReturnTriple(x, y, z);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetFov(JS_ARGS)
	{
		JS_SETUP(Camera,"N");

		self->fov_ = wrapper.GetNumber<float>(0);
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetType(JS_ARGS)
	{
		JS_SETUP(Camera,"S");
		std::string typing = wrapper.GetString(0);
		if (strcmp(typing.c_str(), "perspective") == 0)
		{
			self->type_ = Camera::CameraType::kPerspective;
		}
		else if (strcmp(typing.c_str(), "orthographic") == 0)
		{
			self->type_ = Camera::CameraType::kOrthographic;
		}
		else
		{
			SNUFF_LOG_ERROR(std::string("Unknown camera type '" + typing).c_str());
		}
	}

	//------------------------------------------------------------------------------
	void Camera::JSSetZoom(JS_ARGS)
	{
		JS_SETUP(Camera, "N");

		self->SetZoom(wrapper.GetNumber<float>(0));
	}
}