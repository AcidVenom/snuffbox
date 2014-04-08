#pragma once

namespace snuffbox
{
	class Game;

	namespace environment
	{
		/// Returns the global game object
		Game& game();
	}
}