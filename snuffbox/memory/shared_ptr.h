#pragma once

#include "../../snuffbox/logging.h"
#include "../../snuffbox/memory/allocated_memory.h"

namespace snuffbox
{
	/**
	* @class snuffbox::RefCount
	* @brief Used for snuffbox::SharedPtr<T> to keep its references
	* @author Daniël Konings
	*/
	template<typename T>
	class RefCount
	{
	public:

		/// Default constructor
		RefCount() : refCount_(1){}

		/// Construction through pointer
		RefCount(T* ptr) : refCount_(1)
		{
			SNUFF_ASSERT_NOTNULL(ptr);
			ptr_ = ptr;
		}

		/// Default destructor
		~RefCount(){}

		/// Destroys the associated pointer
		inline void Destroy()
		{
			
			SNUFF_ASSERT_NOTNULL(ptr_);
			environment::memory().Destruct(ptr_);
		}

		/// Increases the reference count by one
		inline void IncreaseRef(){ 
			++refCount_; 
		}

		/// Decreases the reference count by one
		inline void DecreaseRef(){ 
			if (--refCount_ == 0)
			{
				Destroy();
			}
		}

	private:
		unsigned int refCount_;	///< The amount of references of this object
		T* ptr_;
	};

	/**
	* @class snuffbox::SharedPtr
	* @brief A garbage collecting shared pointer, so memory management won't become a worry
	* @author Daniël Konings
	*/
	template<typename T>
	class SharedPtr
	{
	public:
		/// Default constructor
		SharedPtr();

		/// Initialisation constructor
		SharedPtr(T* ptr);

		// Copy constructors
		SharedPtr(const SharedPtr<T>& other);
		SharedPtr(SharedPtr<T>&& other);

		/// Default destructor
		~SharedPtr();

		// Operator overloading
		SharedPtr& operator=(const SharedPtr& other);
		SharedPtr& operator=(SharedPtr&& other);

		T* operator->(void);
		T& operator*(void);

		bool operator==(const SharedPtr<T>& other) const;
		bool operator==(const T* other) const;
		bool operator==(std::nullptr_t) const;

		bool operator!=(const SharedPtr<T>& other) const;
		bool operator!=(const T* other) const;
		bool operator!=(std::nullptr_t) const;

		/// Swaps two SharedPtrs
		void Swap(SharedPtr<T>& right);

		/// Resets the shared pointer
		void Reset();

		/// Resets to other
		void Reset(T* ptr, RefCount<T>* ref);

		/// Resets the shared pointer raw
		void ResetRaw(T* ptr, RefCount<T>* ref);

		/// Returns the pointer of this shared pointer
		T* get();

	private:
		T* ptr_; ///< The pointer held by this shared pointer
		RefCount<T>* ref_; ///< The references held by this pointer
	};

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr() : ref_(nullptr), ptr_(nullptr)
	{

	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(const SharedPtr<T>& other) : ref_(nullptr), ptr_(nullptr)
	{
		Reset(other.ptr_, other.ref_);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(SharedPtr<T>&& other) : ptr_(static_cast<T*>(other.ptr_)), ref_(other.ref_)
	{
		other.ptr_ = nullptr;
		other.ref_ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::SharedPtr(T* ptr)
	{
		SNUFF_ASSERT_NOTNULL(ptr);
		ptr_ = ptr;
		ref_ = new RefCount<T>(ptr);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>::~SharedPtr()
	{
		if (ref_ != nullptr)
			ref_->DecreaseRef();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other)
	{
		SharedPtr<T>(other).Swap(*this);
		return this;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other)
	{
		SharedPtr<T>(std::move(other)).Swap(*this);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T* SharedPtr<T>::operator->(void)
	{
		SNUFF_ASSERT_NOTNULL(ptr_);
		return ptr_;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T& SharedPtr<T>::operator*(void)
	{
		SNUFF_ASSERT_NOTNULL(ptr_);
		return *ptr_;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(const SharedPtr<T>& other) const
	{
		return other.get() == get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(const T* other) const
	{
		SNUFF_ASSERT_NOTNULL(other);
		return other == get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator==(std::nullptr_t) const
	{
		return ptr_ == nullptr
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(const SharedPtr<T>& other) const
	{
		return other.get() != get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(const T* other) const
	{
		SNUFF_ASSERT_NOTNULL(other);
		return other != get();
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	bool SharedPtr<T>::operator!=(std::nullptr_t) const
	{
		return ptr_ != nullptr
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Swap(SharedPtr<T>& right)
	{
		std::swap(ref_, right.ref_);
		std::swap(ptr_, right.ptr_);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Reset()
	{
		if (ref_ != nullptr)
			ref_->IncreaseRev();
		reset_raw(ptr_, ref_);
	}

	//-------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::Reset(T* ptr, RefCount<T>* ref)
	{
		if (ref != nullptr)
			ref->IncreaseRef();
		ResetRaw(ptr, ref);
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	void SharedPtr<T>::ResetRaw(T* ptr, RefCount<T>* ref)
	{
		if (ref_ != nullptr)
			ref_->DecreaseRef();
		ptr_ = ptr;
		ref_ = ref;
	}

	//------------------------------------------------------------------------------------------------------
	template<typename T>
	T* SharedPtr<T>::get()
	{
		SNUFF_ASSERT_NOTNULL(ptr_);
		return ptr_;
	}

}