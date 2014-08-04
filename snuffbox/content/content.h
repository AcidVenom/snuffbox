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
		/**
		* @brief Construct content through a type and a pointer to the actual content
		* @param[in] type (snuffbox::ContentManager::ContentTypes) The content type
		* @param[in] ptr (snuffbox::SharedPtr<T>&) The shared pointer to the actual content
		*/
		Content(ContentManager::ContentTypes type, SharedPtr<T>& ptr);

		/// Default destructor
		~Content(){}

		/*
		* @return (snuffbox::ContentManager::ContentTypes) The content type of this content
		*/
		ContentManager::ContentTypes type(){ return content_type_; }

		/*
		* @return (snuffbox::SharedPtr<T>&) The content pointer
		*/
		SharedPtr<T>& Get(){ return ptr_; }

	private:
		ContentManager::ContentTypes content_type_; //!< The type of this content
		SharedPtr<T> ptr_;	//!< The pointer to the actual content
	};

	//-----------------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Content<T>::Content(ContentManager::ContentTypes type, SharedPtr<T>& ptr) :
		content_type_(type),
		ptr_(ptr)
	{

	}
}