#include "../snuffbox/game.h"
#include "../snuffbox/win32/win32_window.h"
#include <string>

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------
	namespace environment {
		namespace {
			Game* globalInstance = nullptr;
		}
		bool has_display_device() { return globalInstance != nullptr; }
		Game& game()
		{
			SNUFF_ASSERT_NOTNULL(globalInstance);
			return *globalInstance;
		}
	}
}

using namespace snuffbox;

//------------------------------------------------------------------------------------------------------
Game::Game(PlatformWindow* window) : started_(true)
{
	window_ = window;
}

//------------------------------------------------------------------------------------------------------
Game::~Game()
{
	
}

//------------------------------------------------------------------------------------------------------
void Game::Initialise()
{
	window_->Create();
	window_->Show();
}

//------------------------------------------------------------------------------------------------------
void Game::Update()
{

}

//------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	SNUFF_LOG_INFO("Snuffbox shutdown..");
	started_ = false;
	window_->Destroy();
	SNUFF_LOG_INFO(".. Shutdown succesful");
}

//------------------------------------------------------------------------------------------------------
void Game::MakeGlobal()
{
	environment::globalInstance = this;
}

//------------------------------------------------------------------------------------------------------
void Game::NotifyEvent(GameEvents evt)
{
	switch (evt)
	{
	case GameEvents::kQuit:
		Shutdown();
		break;
	}
}

//------------------------------------------------------------------------------------------------------
int SNUFF_MAIN
{
	SharedPtr<Game> game = new Game(new PlatformWindow("Snuffbox D3D11 (Alpha)",1024,600));
	game->MakeGlobal();
	game->Initialise();

	std::string result;

	while (game->started())
	{
		game->window()->ProcessMessages();
	}
	return EXIT_SUCCESS;
}