#include "../snuffbox/game.h"
#include "../snuffbox/win32/win32_window.h"
#include "../snuffbox/environment.h"

#include <chrono>

using namespace std::chrono;

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
  environment::globalInstance = this;
  CoInitialize(0);
}

//------------------------------------------------------------------------------------------------------
Game::~Game()
{
  CoUninitialize();
}

//------------------------------------------------------------------------------------------------------
void Game::Initialise()
{
	CreateCallbacks();
	InitialiseWindow();

	JS_CREATE_SCOPE;
	Handle<Function> cb = Local<Function>::New(environment::js_state_wrapper().isolate(), initialise_);
	Handle<Context> ctx = environment::js_state_wrapper().context();
	Handle<Value> argv[1] = { };
	ctx->Enter();
	cb->Call(ctx->Global(), 0, argv);
	ctx->Exit();
}

//------------------------------------------------------------------------------------------------------
void Game::Update()
{

	high_resolution_clock::time_point startTime = high_resolution_clock::now();
	high_resolution_clock::time_point lastTime = startTime;

	JS_CREATE_SCOPE;
	Handle<Function> cb = Local<Function>::New(environment::js_state_wrapper().isolate(), update_);
	Handle<Context> ctx = environment::js_state_wrapper().context();
	Handle<Value> argv[1] = { Number::New(environment::js_state_wrapper().isolate(), deltaTime_) };
	ctx->Enter();
	cb->Call(ctx->Global(), 1, argv);
	ctx->Exit();

	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float, std::milli> dtDuration = duration_cast<duration<float, std::milli>>(now - lastTime);
	deltaTime_ = dtDuration.count() * 1e-3f;

	lastTime = now;
}

//------------------------------------------------------------------------------------------------------
void Game::Draw()
{
	JS_CREATE_SCOPE;
	Handle<Function> cb = Local<Function>::New(environment::js_state_wrapper().isolate(), draw_);
	Handle<Context> ctx = environment::js_state_wrapper().context();
	Handle<Value> argv[1] = { Number::New(environment::js_state_wrapper().isolate(), deltaTime_) };
	ctx->Enter();
	cb->Call(ctx->Global(), 1, argv);
	ctx->Exit();
}

//------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	JS_CREATE_SCOPE;
	Handle<Function> cb = Local<Function>::New(environment::js_state_wrapper().isolate(), shutdown_);
	Handle<Context> ctx = environment::js_state_wrapper().context();
	Handle<Value> argv[1] = {};
	ctx->Enter();
	cb->Call(ctx->Global(), 0, argv);
	ctx->Exit();

	SNUFF_LOG_INFO("Snuffbox shutdown..");
	started_ = false;
	window_->Destroy();
	SNUFF_LOG_INFO(".. Shutdown succesful");
}

//------------------------------------------------------------------------------------------------------
void Game::ParseCommandLine()
{
  std::string cmdLine = GetCommandLineA();

  SNUFF_XASSERT(CommandExists(cmdLine, "-source-directory="),"Could not find '-source-directory' argument in the command line!\nYou have to specify a directory to work with.");

  std::string path = GetCommand(cmdLine, "-source-directory=");
  environment::js_state_wrapper().path() = path;
}

//------------------------------------------------------------------------------------------------------
std::string Game::GetCommand(const std::string& cmdLine, const char* option)
{
  unsigned int pos = cmdLine.find(option) + strlen(option);
  unsigned int len = strlen(cmdLine.c_str());
  unsigned int start = len - pos;


  std::string result = "";

  for (auto it = cmdLine.c_str()+pos; *it; it++)
  {
    if (strcmp(it, " ") != 0 || strcmp(it, "\0") == 0)
    {
      result += it[0];
    }
  }

  return result;
}

//------------------------------------------------------------------------------------------------------
bool Game::CommandExists(const std::string& cmdLine, const char* option)
{
  return cmdLine.find(option) != std::string::npos;
}

//------------------------------------------------------------------------------------------------------
void Game::InitialiseWindow()
{
  window_->Create();
  window_->Show();
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
void Game::RegisterJS(JS_TEMPLATE)
{
	
}

void Game::CreateCallbacks()
{
	JS_CREATE_SCOPE;

	Handle<Context> ctx = environment::js_state_wrapper().context();
	ctx->Enter();
	Handle<Object> global = ctx->Global();
	Handle<Value> game = global->Get(String::NewFromUtf8(environment::js_state_wrapper().isolate(),"Game"));

	JS_SETUP_CALLBACKS;

	SNUFF_XASSERT(game->IsFunction(), "Could not find 'Game' object!");
	Handle<Object> obj = game->ToObject();

	JS_OBJECT_CALLBACK("Initialise", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Initialise()' function! Please add it to your main.js");
	JS_CALLBACK_STORE(initialise_);
	
	JS_OBJECT_CALLBACK("Update", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Update(dt)' function! Please add it to your main.js");
	JS_CALLBACK_STORE(update_);

	JS_OBJECT_CALLBACK("Draw", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Draw(dt)' function! Please add it to your main.js");
	JS_CALLBACK_STORE(draw_);

	JS_OBJECT_CALLBACK("Shutdown", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Shutdown()' function! Please add it to your main.js");
	JS_CALLBACK_STORE(shutdown_);

	ctx->Exit();
}

//------------------------------------------------------------------------------------------------------
int SNUFF_MAIN
{
	AllocatedMemory memory;
  JSStateWrapper js_state_wrapper;

	SharedPtr<Game> game = environment::memory().ConstructShared<Game>(
		environment::memory().ConstructShared<PlatformWindow>("Snuffbox Alpha (D3D11)",1024,600)
		);

  game->ParseCommandLine();

  js_state_wrapper.Initialise();
	
	game->Initialise();

	while (game->started())
	{
  	game->window()->ProcessMessages();
		game->Update();
		game->Draw();
	}
	return EXIT_SUCCESS;
}