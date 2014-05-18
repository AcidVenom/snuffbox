#pragma once

namespace snuffbox
{
	class Game;
  class JSStateWrapper;
	struct AllocatedMemory;
	class Connection;
	class Mouse;
	class Keyboard;
	class D3D11DisplayDevice;
	class FileWatcher;

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

		/// Returns the keyboard
		Keyboard& keyboard();

		/// Returns the render device
		D3D11DisplayDevice& render_device();

		/// Returns the file watcher
		FileWatcher& file_watcher();

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

		/// Returns if there's a keyboard or not
		bool has_keyboard();

		/// Returns if there's a render device or not
		bool has_render_device();

		/// Returns if there's a file watcher or not
		bool has_file_watcher();
	}
}