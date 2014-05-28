#pragma once

#include "../../snuffbox/content/content_manager.h"

namespace snuffbox
{
	/**
	* @class snuffbox::Content
	* @brief A content holder for every kind of content
	* @author Daniël Konings
	*/
	template<typename T>
	class Content
	{
	public:
		/// Default constructor
		Content(ContentManager::ContentTypes type, SharedPtr<T>& ptr);
		/// Default destructor
		~Content(){} 

		/// Returns the content type
		ContentManager::ContentTypes type(){ return contentType_; }

		/// Returns the content
		SharedPtr<T>& Get(){ return ptr_; }

	private:
		ContentManager::ContentTypes contentType_; ///< The type of this content
		SharedPtr<T> ptr_;	///< The pointer to the actual content
	};

	template<typename T>
	inline Content<T>::Content(ContentManager::ContentTypes type, SharedPtr<T>& ptr) :
		contentType_(type),
		ptr_(ptr)
	{

	}
}