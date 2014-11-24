#pragma once

#include "../../../snuffbox/d3d11/elements/render_element.h"

namespace snuffbox
{
  class MouseArea;

	/**
	* @class snuffbox::Widget
	* @brief A UI widget element as a container or a visual representation
	* @author Daniël Konings
	*/ 
	class Widget : public RenderElement
	{
	public:

		/// Default constructor
		Widget();

    /// Default constructor for text
    Widget(RenderElement::ElementTypes type);

		/// JavaScript constructor
		Widget(JS_ARGS);

		/// Default destructor
		virtual ~Widget();

		/// Creates the widget
		virtual void Create();

		/// Sets the buffers for the render device to use
		virtual void SetBuffers();

		/**
		* @return (snuffbox::VertexBufferType) The vertex buffer type
		*/
    virtual VertexBufferType type(){ return VertexBufferType::kQuad2D; }

		/**
		* @brief Override the world matrix to use our own ui world matrix
		* @param[in] camera (snuffbox::Camera*) The currently in-use camera
		* @return (XMMATRIX&) The calculated world matrix
		*/
    XMMATRIX& world_matrix(Camera* camera);

		/**
		* @return (snuffbox::Widget*) The widget's parent
		*/
		Widget* parent(){ return parent_; }

    /**
    * @brief Adds a mouse area as a child
    * @param[in] area (snuffbox::MouseArea*) The mouse area to add
    */
    void AddMouseArea(MouseArea* area);

    /**
    * @brief Adds a child to this widget
    * @param[in] child (snuffbox::Widget*) The child widget
    */
    void AddChild(Widget* child);

    /// Propagates all children widgets
    void Propagate();

    /// Override the on change method
    void OnChange(RenderMessage msg);

    /**
    * @brief Removes a child from this widget
    * @param[in] child (snuffbox::Widget*) The child to remove
    */
    void RemoveChild(Widget* child);

    /**
    * @brief Sets the parent of this widget
    * @param[in] parent (snuffbox::Widget*) The parent to set
    */
    void SetParent(Widget* parent){ parent_ = parent; }

	private:
		ID3D11Buffer*							vertex_buffer_; //!< The vertex buffer of this element
		ID3D11Buffer*							index_buffer_; //!< The index buffer of this element
		XMMATRIX									world_; //!< The world matrix
		Widget*										parent_; //!< The widget's parent
    std::vector<MouseArea*>   mouse_area_children_; //!< The mouse area children
    std::vector<Widget*>      children_; //!< The children of this widget
    bool                      alpha_changed_; //!< Has the alpha changed since last frame?
    bool                      blend_changed_; //!< Has the blend changed since last frame?
    bool                      is_text_; //!< Is this widget text?
	public:
		JS_NAME(Widget);
	};
}