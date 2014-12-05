#pragma once

#include "../../snuffbox/d3d11/d3d11_display_device.h"
#include "../../snuffbox/js/js_state_wrapper.h"

namespace snuffbox
{
	/**
	* @class snuffbox::RenderTarget
	* @brief A wrapper class for a D3D11 render target
	* @author Daniël Konings
	*/ 
	class RenderTarget : public JSObject
	{
	public:
		/// Default constructor
		RenderTarget();

		/// JavaScript constructor
		RenderTarget(JS_ARGS);

		/// Default destructor
		virtual ~RenderTarget();


		/**
		* @return (std::vector<RenderElement*>&) The list of normal render elements
		*/
		std::vector<RenderElement*>& render_elements(){ return render_elements_; }

		/**
		* @return (std::vector<RenderElement*>&) The list of opaque render elements
		*/
		std::vector<RenderElement*>& opaque_elements(){ return opaque_elements_; }

		/**
		* @return (std::vector<RenderElement*>&) The list of ui render elements
		*/
		std::vector<RenderElement*>& ui_elements(){ return ui_elements_; }

		/**
		* @return (std::queue<RenderElement*>) Returns the render queue
		*/
		std::queue<RenderElement*>& render_queue(){ return queue_; }

		ID3D11RenderTargetView* view(){ return view_; }
		ID3D11ShaderResourceView* resource(){ return resource_; }

		/// Clears the render target for new use
		void Clear();

		/// Creates the target
		void Create();

		PostProcessing* post_processing(){ return post_processing_.get(); }

	private:
		std::queue<RenderElement*>		queue_;									//!< The render queue
		D3DTexture2D*									target_;								//!< The texture that will be rendered to
		ID3D11RenderTargetView*				view_;									//!< The render target view
		ID3D11ShaderResourceView*			resource_;							//!< The shader resource of the render target texture
		std::vector<RenderElement*>		render_elements_;				//!< The list of render elements
		std::vector<RenderElement*>		opaque_elements_;				//!< The list of opaque render elements
		std::vector<RenderElement*>		ui_elements_;						//!< The list of ui render elements
		SharedPtr<PostProcessing>			post_processing_;				//!< The post processing object

	public:
		JS_NAME(RenderTarget);
		static void RegisterJS(JS_TEMPLATE);
		static void JSClear(JS_ARGS);
		static void JSSetShader(JS_ARGS);
		static void JSAddPass(JS_ARGS);
		static void JSRemovePass(JS_ARGS);
		static void JSClearPasses(JS_ARGS);
		static void JSSetUniform(JS_ARGS);
	};
}