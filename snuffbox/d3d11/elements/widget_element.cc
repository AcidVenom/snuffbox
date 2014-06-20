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
		SetAspect(true);
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
		anchors_[static_cast<unsigned int>(anchor)] = true;
	}

	//-------------------------------------------------------------------------------
	void Widget::RemoveAnchor(Widget::WidgetAnchor anchor)
	{
		anchors_[static_cast<unsigned int>(anchor)] = false;
	}

	//-------------------------------------------------------------------------------
	void Widget::RegisterExtraFunctions(JS_EXTRA)
	{
		JS_CREATE_SCOPE;
    obj->Set(String::NewFromUtf8(JS_ISOLATE, "setAnchor"), Function::New(JS_ISOLATE, JSSetAnchor));
    obj->Set(String::NewFromUtf8(JS_ISOLATE, "removeAnchor"), Function::New(JS_ISOLATE, JSRemoveAnchor));
	}

	//-------------------------------------------------------------------------------
	Widget::~Widget()
	{
		SNUFF_SAFE_RELEASE(vertexBuffer_);
		SNUFF_SAFE_RELEASE(indexBuffer_);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetAnchor(JS_ARGS)
	{
		JS_SETUP(Widget, "N");
		self->SetAnchor(static_cast<Widget::WidgetAnchor>(wrapper.GetNumber<unsigned int>(0)));
	}

	//-------------------------------------------------------------------------------
	void Widget::JSRemoveAnchor(JS_ARGS)
	{
		JS_SETUP(Widget, "N");
		self->RemoveAnchor(static_cast<Widget::WidgetAnchor>(wrapper.GetNumber<unsigned int>(0)));
	}
}