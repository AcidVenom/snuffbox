#pragma once

namespace snuffbox
{
	class Game;
	struct AllocatedMemory;

	namespace environment
	{
		/// Returns the global game object
		Game& game();

		/// Returns the allocated memory
		AllocatedMemory& memory();
	}
}