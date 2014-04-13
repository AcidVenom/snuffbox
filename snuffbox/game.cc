#include "../snuffbox/game.h"
#include "../snuffbox/win32/win32_window.h"
#include "../snuffbox/js/js_state_wrapper.h"
#include "../snuffbox/environment.h"

#include <string>

namespace snuffbox
{
	//---------------------------------------------------------------------------------------------------
	namespace environment {
		namespace 
    {
			Game* globalInstance = nullptr;
		}

		bool has_game() { return globalInstance != nullptr; }

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
	AllocatedMemory memory;
  JSStateWrapper js_state_wrapper;

	SharedPtr<Game> game = environment::memory().ConstructShared<Game>(
		environment::memory().ConstructShared<PlatformWindow>("Snuffbox Alpha (D3D11)",1024,600)
		);

	game->MakeGlobal();
	game->Initialise();

	while (game->started())
	{
		game->window()->ProcessMessages();
	}
	return EXIT_SUCCESS;
}