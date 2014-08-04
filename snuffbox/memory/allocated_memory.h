#pragma once

#include "../../snuffbox/memory/shared_ptr.h"

namespace snuffbox
{
	/**
	* @struct snuffbox::AllocatedMemory
	* @brief Contains information about the number of allocations and allocated memory, also checks for leaks on destruction
	* @author Daniël Konings
	*/
	struct AllocatedMemory
	{
		/// Default constructor
		AllocatedMemory();

		/// Default destructor (Checks for leaks)
		~AllocatedMemory();

		unsigned int allocations; //!< The number of allocations of this allocator
		size_t allocatedMemory; //!< The total allocated memory in bytes of this allocator

		/// Constructs a shared pointer
		template<typename T, typename... Args>
		T* ConstructShared(Args&&... args);

		/// Destructs a pointer
		template<typename T>
		void Destruct(T* ptr);
	};

	//---------------------------------------------------------------------------------------------------------
	template<typename T, typename... Args>
	inline T* AllocatedMemory::ConstructShared(Args&&... args)
	{
		T* ptr = new T(std::forward<Args>(args)...);
		++allocations;
		allocatedMemory += sizeof(ptr);
		return ptr;
	}

	//---------------------------------------------------------------------------------------------------------
	template<typename T>
	inline void AllocatedMemory::Destruct(T* ptr)
	{
		--allocations;
		allocatedMemory -= sizeof(ptr);

		delete ptr;

		ptr = nullptr;
	}
}