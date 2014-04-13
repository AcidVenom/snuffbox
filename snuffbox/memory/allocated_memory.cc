#include "../../snuffbox/memory/allocated_memory.h"
#include "../../snuffbox/logging.h"

namespace snuffbox
{
	//-------------------------------------------------------------------------------------------
	namespace environment
	{
		namespace 
    {
			AllocatedMemory* globalInstance;
		}

		bool has_memory() { return globalInstance != nullptr; }

		AllocatedMemory& memory()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}

	//-------------------------------------------------------------------------------------------
	AllocatedMemory::AllocatedMemory() : allocations(0), allocatedMemory(0)
	{
		environment::globalInstance = this;
	}

	//-------------------------------------------------------------------------------------------
	AllocatedMemory::~AllocatedMemory()
	{
		SNUFF_XASSERT(allocations == 0 && allocatedMemory == 0, "Detected a memory leak on the heap!");
		SNUFF_LOG_INFO("No memory leaks detected");
	}
}