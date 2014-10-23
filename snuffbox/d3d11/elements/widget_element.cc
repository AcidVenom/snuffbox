#include "../../../snuffbox/d3d11/elements/widget_element.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Widget::Widget() : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr)
	{

	}

	//-------------------------------------------------------------------------------------------
	Widget::Widget(JS_ARGS) : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr)
	{
		JSWrapper wrapper(args);
		Create();

		if (args.Length() > 0)
		{
			parent_ = wrapper.GetPointer<Widget>(0);
		}
	}

	//-------------------------------------------------------------------------------------------
	void Widget::Create()
	{
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });

		indices().push_back(2);
		indices().push_back(0);
		indices().push_back(1);
		indices().push_back(3);

		vertex_buffer_ = environment::render_device().CreateVertexBuffer(vertices());
		index_buffer_ = environment::render_device().CreateIndexBuffer(indices());
	}

	//-------------------------------------------------------------------------------------------
	void Widget::SetBuffers()
	{
		environment::render_device().SetVertexBuffer(vertex_buffer_);
		environment::render_device().SetIndexBuffer(index_buffer_);
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
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "removeAnchorTop"), Function::New(JS_ISOLATE, JSRemoveAnchorTop));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "removeAnchorBottom"), Function::New(JS_ISOLATE, JSRemoveAnchorBottom));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "removeAnchorLeft"), Function::New(JS_ISOLATE, JSRemoveAnchorLeft));
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "removeAnchorRight"), Function::New(JS_ISOLATE, JSRemoveAnchorRight));
	}

	//-------------------------------------------------------------------------------
	Widget::~Widget()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
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
	void Widget::JSRemoveAnchorTop(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->RemoveAnchor(Widget::WidgetAnchor::kTop);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSRemoveAnchorBottom(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->RemoveAnchor(Widget::WidgetAnchor::kBottom);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSRemoveAnchorLeft(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->RemoveAnchor(Widget::WidgetAnchor::kLeft);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSRemoveAnchorRight(JS_ARGS)
	{
		JS_SETUP(Widget, "V");
		self->RemoveAnchor(Widget::WidgetAnchor::kRight);
	}

	//-------------------------------------------------------------------------------
	XMMATRIX Widget::anchor()
	{
		float w = static_cast<float>(environment::render_settings().settings().resolution.w);
		float h = static_cast<float>(environment::render_settings().settings().resolution.h);

		float anchorLeftRight = 0, anchorTopBottom = 0;

		if (parent_ != nullptr)
		{
			XMVECTOR size = parent_->scale() * parent_->size();
			w = XMVectorGetX(size);
			h = XMVectorGetZ(size);

			anchorLeftRight = w / 2;
			anchorTopBottom = h / 2;
		}

		if (anchors_[WidgetAnchor::kLeft] == 1)
		{
			anchorLeftRight = parent_ == nullptr ? -(w / 2) : 0;
		}

		if (anchors_[WidgetAnchor::kRight] == 1)
		{
			XMVECTOR vec = scale() * size();

			float offset = parent_ == nullptr ? w / 2 : w;
			anchorLeftRight = offset - XMVectorGetX(vec);
		}

		if (anchors_[WidgetAnchor::kTop] == 1)
		{
			XMVECTOR vec = scale() * size();

			float offset = parent_ == nullptr ? h / 2 : h;
			anchorTopBottom = offset - XMVectorGetZ(vec);
		}

		if (anchors_[WidgetAnchor::kBottom] == 1)
		{
			anchorTopBottom = parent_ == nullptr ? -(h / 2) : 0;
		}

		if (parent_ != nullptr)
		{
			XMMATRIX& m = parent_->offset();
			anchorLeftRight += m._41;
			anchorTopBottom += m._43;

			XMVECTOR translation = parent_->translation();
			anchorLeftRight += XMVectorGetX(translation);
			anchorTopBottom += XMVectorGetY(translation);

			XMMATRIX& anchor = parent_->anchor();
			anchorLeftRight += anchor._41;
			anchorTopBottom += anchor._42;
		}

		return XMMatrixTranslation(anchorLeftRight, anchorTopBottom, 0);
	}

	//-------------------------------------------------------------------------------
	XMMATRIX& Widget::world_matrix(Camera* camera)
	{
		XMMATRIX trans = XMMatrixTranslationFromVector(translation());
		trans._42 = -trans._42;

		world_ = scaling() * offset() * XMMatrixRotationX(-XM_PI / 2) * rotation() * trans * anchor();
		return world_;
	}
}