#include "../../../snuffbox/d3d11/elements/widget_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Widget::Widget() : RenderElement(RenderElement::ElementTypes::kWidget)
	{

	}

	//-------------------------------------------------------------------------------------------
	Widget::Widget(JS_ARGS) : RenderElement(RenderElement::ElementTypes::kWidget)
	{
		Create();
		SetScale(64, 0, 64);
	}

	//-------------------------------------------------------------------------------------------
	void Widget::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });

		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
		indices().push_back(3);

		vertexBuffer_ = environment::render_device().CreateVertexBuffer(vertices());
		indexBuffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Widget::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertexBuffer_);
		environment::render_device().SetIndexBuffer(indexBuffer_);
	}

	//-------------------------------------------------------------------------------
	void Widget::SetAnchor(Widget::WidgetAnchor anchor)
	{
		switch (anchor)
		{
		case WidgetAnchor::kLeft:
			anchors_[WidgetAnchor::kRight] = 0;
			break;
		case WidgetAnchor::kRight:
			anchors_[WidgetAnchor::kLeft] = 0;
			break;
		case WidgetAnchor::kTop:
			anchors_[WidgetAnchor::kBottom] = 0;
			break;
		case WidgetAnchor::kBottom:
			anchors_[WidgetAnchor::kTop] = 0;
			break;
		}

		anchors_[anchor] = 1;
	}

	//-------------------------------------------------------------------------------
	void Widget::RemoveAnchor(Widget::WidgetAnchor anchor)
	{
		anchors_[anchor] = 0;
	}

	//-------------------------------------------------------------------------------
	void Widget::RegisterExtraFunctions(JS_EXTRA)
	{
		JS_CREATE_SCOPE;
    obj->Set(String::NewFromUtf8(JS_ISOLATE, "setAnchorLeft"), Function::New(JS_ISOLATE, JSSetAnchorLeft));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "setAnchorRight"), Function::New(JS_ISOLATE, JSSetAnchorRight));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "setAnchorTop"), Function::New(JS_ISOLATE, JSSetAnchorTop));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "setAnchorBottom"), Function::New(JS_ISOLATE, JSSetAnchorBottom));
	}

	//-------------------------------------------------------------------------------
	Widget::~Widget()
	{
		SNUFF_SAFE_RELEASE(vertexBuffer_);
		SNUFF_SAFE_RELEASE(indexBuffer_);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetAnchorLeft(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->SetAnchor(Widget::WidgetAnchor::kLeft);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetAnchorRight(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->SetAnchor(Widget::WidgetAnchor::kRight);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetAnchorTop(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->SetAnchor(Widget::WidgetAnchor::kTop);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetAnchorBottom(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->SetAnchor(Widget::WidgetAnchor::kBottom);
	}

	//-------------------------------------------------------------------------------
	XMMATRIX Widget::anchor()
	{
		float w = static_cast<float>(environment::render_settings().settings().resolution.w);
		float h = static_cast<float>(environment::render_settings().settings().resolution.h);

		float anchorLeftRight = static_cast<float>(anchors_[WidgetAnchor::kRight] - anchors_[WidgetAnchor::kLeft]) * w;
		float anchorTopBottom = static_cast<float>(anchors_[WidgetAnchor::kBottom] - anchors_[WidgetAnchor::kTop]) * h;

		return XMMatrixTranslation(anchorLeftRight, anchorTopBottom, 0);
	}

	//-------------------------------------------------------------------------------
	XMMATRIX& Widget::World(Camera* camera)
	{
		world_ = scaling() * offset() * XMMatrixRotationX(-XM_PI / 2) * rotation() * XMMatrixTranslationFromVector(translation()) * anchor();
		return world_;
	}
}