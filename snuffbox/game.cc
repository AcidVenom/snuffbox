#include "../snuffbox/game.h"
#include "../snuffbox/environment.h"

#include <chrono>

#define SNUFF_VERSION_MAJOR 0
#define SNUFF_VERSION_MINOR 1

#ifdef _DEBUG
#define SNUFF_DEBUG_MODE "Debug"
#else
#define SNUFF_DEBUG_MODE "Release"
#endif

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
Game::Game(Win32Window* window) :
started_(true),
consoleEnabled_(false),
mouse_(environment::memory().ConstructShared<Mouse>()),
keyboard_(environment::memory().ConstructShared<Keyboard>()),
device_(environment::memory().ConstructShared<D3D11DisplayDevice>()),
path_(""),
gameTime_(0)
{
	environment::globalInstance = this;
	ParseCommandLine();
	window_ = window;
	InitialiseWindow();
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
	window_->Show();
	initialise_.Call(0);
}

//------------------------------------------------------------------------------------------------------
void Game::Update()
{
	++gameTime_;
	if (!started_)
		return;

	mouse_->Update();
	keyboard_->Update();
	high_resolution_clock::time_point startTime = high_resolution_clock::now();
	high_resolution_clock::time_point lastTime = startTime;

	JS_CREATE_SCOPE;
	Handle<Value> argv[1] = {
		Number::New(JS_ISOLATE, deltaTime_)
	};
	update_.Call(1,argv);
  double sleep = 16 - deltaTime_*1000;
  if (sleep < 0)
    sleep = 0;
  Sleep(static_cast<DWORD>(sleep));
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<double, std::milli> dtDuration = duration_cast<duration<double, std::milli>>(now - lastTime);
	deltaTime_ = dtDuration.count() * 1e-3f;
	lastTime = now;

	environment::render_device().IncrementTime();

	if (gameTime_ % 20 == 0)
	{
		environment::file_watcher().WatchFiles();
	}
}

//------------------------------------------------------------------------------------------------------
void Game::Draw()
{
	if (!started_)
		return;

	JS_CREATE_SCOPE;
	Handle<Value> argv[1] = {
		Number::New(JS_ISOLATE, deltaTime_)
	};

	environment::render_device().StartDraw();
	draw_.Call(1, argv);
	environment::render_device().Draw();
	environment::render_device().EndDraw();
}

//------------------------------------------------------------------------------------------------------
void Game::Shutdown()
{
	shutdown_.Call(0);

	JS_CREATE_SCOPE;
  Local<Context> ctx = JS_CONTEXT;
	Local<Object> global = ctx->Global();
	Local<Array> names = global->GetPropertyNames();
	Local<String> src;
	std::string c_src = "";

	for (unsigned int i = 0; i < names->Length(); ++i)
	{
		Local<Value> str = names->Get(i);
		c_src += *String::Utf8Value(str.As<String>());
		c_src += "=null;";
	}

	src = String::NewFromUtf8(JS_ISOLATE,c_src.c_str());
	Local<Script> script = Script::Compile(src, String::NewFromUtf8(JS_ISOLATE,"shutdown"));
	script->Run();
	
	SNUFF_LOG_INFO("Snuffbox shutdown..");
	started_ = false;
	device_->Destroy();
	window_->Destroy();
	SNUFF_LOG_INFO(".. Shutdown succesful");

	environment::globalInstance = nullptr;
}

//------------------------------------------------------------------------------------------------------
void Game::ParseCommandLine()
{
  std::string cmdLine = GetCommandLineA();

  SNUFF_XASSERT(CommandExists(cmdLine, "-source-directory="),"Could not find '-source-directory' argument in the command line!\nYou have to specify a directory to work with.");

  std::string path = GetCommand(cmdLine, "-source-directory=");
  environment::js_state_wrapper().path() = path;
	path_ = path;

	if (CommandExists(cmdLine, "-console"))
	{
		consoleEnabled_ = true;
	}
}

//------------------------------------------------------------------------------------------------------
std::string Game::GetCommand(const std::string& cmdLine, const char* option)
{
  size_t pos = cmdLine.find(option) + strlen(option);
	size_t len = strlen(cmdLine.c_str());
	size_t start = len - pos;


  std::string result = "";

  for (auto it = cmdLine.c_str()+pos; *it; it++)
  {
		if (*it != *"\0" && *it != *" ")
    {
      result += it[0];
    }
		else
		{
			break;
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
}

//------------------------------------------------------------------------------------------------------
void Game::NotifyEvent(GameEvents evt)
{
	switch (evt)
	{
	case GameEvents::kQuit:
		Shutdown();
		break;
	case GameEvents::kReload:
		Reload();
		break;
	}
}

//------------------------------------------------------------------------------------------------------
void Game::JSRender(JS_ARGS)
{
	JSWrapper wrapper(args);

	Camera* camera = wrapper.GetPointer<Camera>(0);

	environment::render_device().UpdateCamera(camera);
}

//------------------------------------------------------------------------------------------------------
void Game::RegisterJS(JS_TEMPLATE)
{
	JS_CREATE_SCOPE;

	JSFunctionRegister funcs[] = {
		JSFunctionRegister("render", JSRender)
	};

	JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
}

void Game::CreateCallbacks()
{
	JS_CREATE_SCOPE;

  Local<Context> ctx = JS_CONTEXT;

	Local<Object> global = ctx->Global();
	Local<Value> game = global->Get(String::NewFromUtf8(JS_ISOLATE,"Game"));

	JS_SETUP_CALLBACKS;

	SNUFF_XASSERT(game->IsObject(), "Could not find 'Game' object!");
	Local<Object> obj = game->ToObject();

	JS_OBJECT_CALLBACK("Initialise", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Initialise()' function! Please add it to your main.js");
	initialise_.SetFunction(cb);
	
	JS_OBJECT_CALLBACK("Update", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Update(dt)' function! Please add it to your main.js");
	update_.SetFunction(cb);

	JS_OBJECT_CALLBACK("Draw", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Draw(dt)' function! Please add it to your main.js");
	draw_.SetFunction(cb);

	JS_OBJECT_CALLBACK("Shutdown", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.Shutdown()' function! Please add it to your main.js");
	shutdown_.SetFunction(cb);

	JS_OBJECT_CALLBACK("OnReload", obj);
	SNUFF_XASSERT(cb->IsFunction(), "Could not find 'Game.OnReload()' function! Please add it to your main.js");
	onReload_.SetFunction(cb);
}

//------------------------------------------------------------------------------------------------------
void Game::Reload()
{
	onReload_.Call(0);
}

//------------------------------------------------------------------------------------------------------
int SNUFF_MAIN
{
	Connection connection;
	AllocatedMemory memory;
	JSStateWrapper js_state_wrapper;
	SharedPtr<FileWatcher> file_watcher = environment::memory().ConstructShared<FileWatcher>();
	SharedPtr<ContentManager> content_manager = environment::memory().ConstructShared<ContentManager>();

	std::string windowName(
		"Snuffbox_" + std::string(SNUFF_DEBUG_MODE) + "_V_" +
		std::to_string(SNUFF_VERSION_MAJOR) + "." +
		std::to_string(SNUFF_VERSION_MINOR)
		);
	SharedPtr<Game> game = environment::memory().ConstructShared<Game>(
		environment::memory().ConstructShared<Win32Window>(windowName.c_str(), 1280, 720));

	if (game->consoleEnabled())
	{
		connection.Initialise();
		Sleep(1000);
	}
	environment::render_device().Initialise();
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