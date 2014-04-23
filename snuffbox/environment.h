#pragma once

namespace snuffbox
{
	class Game;
  class JSStateWrapper;
	struct AllocatedMemory;
	class Connection;
	class Mouse;

	namespace environment
	{
		/// Returns the global game object
		Game& game();

		/// Returns the allocated memory
		AllocatedMemory& memory();

    /// Returns the JavaScript state
    JSStateWrapper& js_state_wrapper();

		/// Returns the remote console
		Connection& console();

		/// Returns the mouse
		Mouse& mouse();

    /// Returns if the environment owns a game
    bool has_game();

    /// Returns if the environment owns a memory allocator
    bool has_memory();

    /// Returns if the environment owns a JavaScript state wrapper
    bool has_js_state_wrapper();

		/// Returns if there's a connection with the console
		bool has_console();

		/// Returns if there's a mouse or not
		bool has_mouse();
	}
}