#pragma once

namespace snuffbox
{
	class Game;
  class JSStateWrapper;
	struct AllocatedMemory;
	class Console;
	class Mouse;
	class Keyboard;
	class D3D11DisplayDevice;
	class FileWatcher;
	class ContentManager;
	class D3D11Settings;
	class FBXLoader;
	class FreeTypeWrapper;

	namespace environment
	{
		/// Returns the global game object
		Game& game();

		/// Returns the allocated memory
		AllocatedMemory& memory();

    /// Returns the JavaScript state
    JSStateWrapper& js_state_wrapper();

		/// Returns the remote console
		Console& console();

		/// Returns the mouse
		Mouse& mouse();

		/// Returns the keyboard
		Keyboard& keyboard();

		/// Returns the render device
		D3D11DisplayDevice& render_device();

		/// Returns the file watcher
		FileWatcher& file_watcher();

		/// Returns the content manager
		ContentManager& content_manager();

		/// Returns the render settings
		D3D11Settings& render_settings();

		/// Returns the FBX loader
		FBXLoader& fbx_loader();

		/// Returns the FreeType wrapper
		FreeTypeWrapper& free_type_wrapper();

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

		/// Returns if there's a content manager or not
		bool has_content_manager();

		/// Returns if there are render settings or not
		bool has_render_settings();

		/// Returns if there is an FBX loader or not
		bool has_fbx_loader();

		/// Returns if there is a FreeType wrapper or not
		bool has_free_type_wrapper();
	}
}