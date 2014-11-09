#include "../../../snuffbox/d3d11/elements/widget_element.h"
#include "../../../snuffbox/freetype/freetype_wrapper.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Widget::Widget() : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr),
		is_text_(false),
		current_font_(environment::free_type_wrapper().default_font())
	{

	}

	//-------------------------------------------------------------------------------------------
	Widget::Widget(JS_ARGS) : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr),
		is_text_(false),
		current_font_(environment::free_type_wrapper().default_font())
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
		vertices().push_back({ 1.0f, 1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
		vertices().push_back({ 0.0f, 1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) });
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
	void Widget::SetText(std::string text)
	{
		is_text_ = true;
		text_ = text;

	}

	//-------------------------------------------------------------------------------
	bool Widget::is_text()
	{
		return is_text_;
	}

	//-------------------------------------------------------------------------------
	void Widget::RegisterExtraFunctions(JS_EXTRA)
	{
		JS_CREATE_SCOPE;
		obj->Set(String::NewFromUtf8(JS_ISOLATE, "setText"), Function::New(JS_ISOLATE, JSSetText));
	}

	//-------------------------------------------------------------------------------
	Widget::~Widget()
	{
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);
	}

	//-------------------------------------------------------------------------------
	void Widget::JSSetText(JS_ARGS)
	{
		JS_SETUP(Widget, "S");
		self->SetText(wrapper.GetString(0));
	}

	//-------------------------------------------------------------------------------
	XMMATRIX& Widget::world_matrix(Camera* camera)
	{
		XMMATRIX trans = XMMatrixTranslationFromVector(translation());
		XMMATRIX off = offset_2d();

		if (environment::render_settings().y_down() == false)
		{
			trans._42 = -trans._42;
		}
		else
		{
			off._42 = -off._42;
		}

		world_ = scaling_2d() * off * rotation();
		if (parent_ != nullptr)
		{
			Widget* parent = parent_;
      XMVECTOR t = XMVectorSet(0,0,0,0);
      XMMATRIX rot = XMMatrixIdentity();
      XMMATRIX s = XMMatrixIdentity();

      while (parent->parent() != nullptr)
      {
        t += parent->translation();
        rot *= parent->rotation();
        s *= parent->scaling_2d_no_size();
        parent = parent->parent();
      }

			
      XMMATRIX parentTrans = XMMatrixTranslationFromVector(t);
      XMMATRIX rootTrans = XMMatrixTranslationFromVector(parent->translation());

      world_ *= trans * s * rot * parentTrans * parent->scaling_2d_no_size() * parent->rotation() * rootTrans;

		}
		else
		{
			world_ *= trans;
		}
		
		return world_;
	}
}