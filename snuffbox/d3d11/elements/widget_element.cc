#include "../../../snuffbox/d3d11/elements/widget_element.h"
#include "../../../snuffbox/input/mouse_area.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	Widget::Widget() : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr),
    blend_changed_(false),
    alpha_changed_(false),
    is_text_(false)
	{

	}

  //-------------------------------------------------------------------------------------------
  Widget::Widget(RenderElement::ElementTypes type) :
    RenderElement(type),
    parent_(nullptr),
    blend_changed_(false),
    alpha_changed_(false),
    is_text_(true)
  {

  }

	//-------------------------------------------------------------------------------------------
	Widget::Widget(JS_ARGS) : 
		RenderElement(RenderElement::ElementTypes::kWidget),
		parent_(nullptr),
    blend_changed_(false),
    alpha_changed_(false),
    is_text_(false)
	{
		JSWrapper wrapper(args);
		Create();

		if (args.Length() > 0 && !wrapper.GetValue(0)->IsUndefined())
		{
			parent_ = wrapper.GetPointer<Widget>(0);
      parent_->AddChild(this);
		}
	}

  //-------------------------------------------------------------------------------------------
  void Widget::AddChild(Widget* child)
  {
    children_.push_back(child);
  }

	//-------------------------------------------------------------------------------------------
	void Widget::Create()
	{
		vertices().push_back({ 0.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 0.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, -1.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });
		vertices().push_back({ 1.0f, 0.0f, 0.0f, 1.0f, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) });

		indices().push_back(1);
		indices().push_back(0);
		indices().push_back(3);
		indices().push_back(2);

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
  void Widget::AddMouseArea(MouseArea* area)
  {
    mouse_area_children_.push_back(area);
  }

	//-------------------------------------------------------------------------------
	Widget::~Widget()
	{
    if (is_text_ == true)
    {
      return;
    }
		SNUFF_SAFE_RELEASE(vertex_buffer_);
		SNUFF_SAFE_RELEASE(index_buffer_);

    for (auto it : mouse_area_children_)
    {
      it->SetParent(nullptr);
    }

    if (parent_ != nullptr)
    {
      parent_->RemoveChild(this);
    }

    for (auto it : children_)
    {
      it->SetParent(nullptr);
    }
	}

  //-------------------------------------------------------------------------------
  void Widget::RemoveChild(Widget* child)
  {
    for (int i = static_cast<int>(children_.size()) - 1; i >= 0; --i)
    {
      if (children_.at(i) == child)
      {
        children_.erase(children_.begin() + i);
      }
    }
  }

  //-------------------------------------------------------------------------------
  void Widget::Propagate()
  {
    if (blend_changed_ == false && alpha_changed_ == false)
    {
      return;
    }

    for (auto it : children_)
    {
      if (alpha_changed_ == true)
      {
         it->set_alpha(alpha());
      }

      if (blend_changed_ == true)
      {
        XMFLOAT3 b = blend();
        it->SetBlend(b.x, b.y, b.z);
      }
    }

    blend_changed_ = false;
    alpha_changed_ = false;
  }

  //-------------------------------------------------------------------------------
  void Widget::OnChange(RenderMessage msg)
  {
    switch (msg)
    {
    case RenderMessage::kAlpha:
      alpha_changed_ = true;
      break;

    case RenderMessage::kBlend:
      blend_changed_ = true;
      break;

    default:
      break;
    }

    Propagate();
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

      if (environment::render_settings().y_down() == false)
      {
        parentTrans._42 = -parentTrans._42;
        rootTrans._42 = -rootTrans._42;
      }

      world_ *= trans * s * rot * parentTrans * parent->scaling_2d_no_size() * parent->rotation() * rootTrans;

		}
		else
		{
			world_ *= trans;
		}
		
		return world_;
	}
}