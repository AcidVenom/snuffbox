#include "../snuffbox/game.h"

#include "../snuffbox/win32/win32_window.h"
#include "../snuffbox/win32/win32_file_watch.h"

#include "../snuffbox/input/mouse.h"
#include "../snuffbox/input/keyboard.h"

#include "../snuffbox/d3d11/d3d11_display_device.h"
#include "../snuffbox/d3d11/d3d11_camera.h"
#include "../snuffbox/d3d11/d3d11_settings.h"

#include "../snuffbox/content/content_manager.h"

#include "../snuffbox/environment.h"
#include "../snuffbox/console/console.h"

#include "../snuffbox/fbx/fbx_loader.h"

#include "../snuffbox/io/io_manager.h"

#include "../snuffbox/freetype/freetype_font_manager.h"


#include <QtCore>
#include <stdio.h>
#include <fstream>

#define SNUFF_VERSION_MAJOR 0
#define SNUFF_VERSION_MINOR 610

#ifdef _DEBUG
#define SNUFF_DEBUG_MODE "Debug"
#define SNUFF_DEBUG
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
Game::Game(Win32Window* window, QApplication& app) :
started_(false),
consoleEnabled_(false),
doReload_(false),
shouldReload_(false),
mouse_(environment::memory().ConstructShared<Mouse>()),
keyboard_(environment::memory().ConstructShared<Keyboard>()),
device_(environment::memory().ConstructShared<D3D11DisplayDevice>()),
path_(""),
gameTime_(0),
qtApp_(app),
shouldQuit_(false),
deltaTime_(0.0)
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
	started_ = true;
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
	lastTime_ = high_resolution_clock::now();

	JS_CREATE_SCOPE;
	Handle<Value> argv[1] = {
		Number::New(JS_ISOLATE, deltaTime_)
	};
	
	update_.Call(1,argv);

	if (environment::render_device().camera())
	{
		environment::render_device().UpdateCamera(environment::render_device().camera());
	}


	if (gameTime_ % 20 == 0 && doReload_)
	{
		environment::file_watcher().WatchFiles();
	}

	if (shouldQuit_)
	{
		Shutdown();
	}

	if (shouldReload_)
	{
    environment::mouse().ClearAreas();

    JS_CREATE_SCOPE;
    Local<Value> argv[1] = {
      String::NewFromUtf8(JS_ISOLATE, environment::file_watcher().last_reloaded().c_str())
    };

		onReload_.Call(1, argv);
		shouldReload_ = false;
	}
}

//------------------------------------------------------------------------------------------------------
void Game::Draw()
{
	if (!started_)
		return;

	JS_CREATE_SCOPE;
	Local<Value> argv[1] = {
		Number::New(JS_ISOLATE, deltaTime_)
	};

	environment::render_device().StartDraw();
	environment::render_device().Draw();
	environment::render_device().EndDraw();
	draw_.Call(1, argv);

	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<double, std::milli> dtDuration = duration_cast<duration<double, std::milli>>(now - lastTime_);
	deltaTime_ = dtDuration.count() * 1e-3f;
	lastTime_ = now;

	environment::render_device().IncrementTime(deltaTime_);
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
	std::string game_member_src = "";

	Local<Array> gameMembers = global->Get(String::NewFromUtf8(JS_ISOLATE,"Game"))->ToObject()->GetPropertyNames();
	for (unsigned int i = 0; i < gameMembers->Length(); ++i)
	{
		game_member_src += "Game.";
		game_member_src += *String::Utf8Value(gameMembers->Get(i).As<String>());
		game_member_src += "=null;";
	}
	for (unsigned int i = 0; i < names->Length(); ++i)
	{
		Local<Value> str = names->Get(i);
		c_src += *String::Utf8Value(str.As<String>());
		c_src += "=null;";
	}

	src = String::NewFromUtf8(JS_ISOLATE, game_member_src.c_str());
	Local<Script> script = Script::Compile(src, String::NewFromUtf8(JS_ISOLATE, "shutdown"));
	script->Run();

	src = String::NewFromUtf8(JS_ISOLATE,c_src.c_str());
	script = Script::Compile(src, String::NewFromUtf8(JS_ISOLATE,"shutdown"));
	script->Run();
	
	SNUFF_LOG_INFO("Snuffbox shutdown..");
	started_ = false;
	device_->Destroy();
	window_->Destroy();

	environment::globalInstance = nullptr;
}

//------------------------------------------------------------------------------------------------------
void Game::ParseCommandLine()
{
  std::string cmdLine = GetCommandLineA();
	char currentPath[FILENAME_MAX];
	std::string path = "";
	std::string temp = "";

	if (CommandExists(cmdLine, "-source-directory="))
	{
		path = GetCommand(cmdLine, "-source-directory=");
	}
	else
	{
		GetCurrentDirectoryA(sizeof(currentPath), currentPath);
		temp = currentPath;

		for (auto it = temp.begin(); it != temp.end(); ++it)
		{
			auto ch = it[0];
			if (ch == *"\\")
			{
				path += "/";
			}
			else
			{
				path += ch;
			}
		}
	}
  
  environment::js_state_wrapper().path() = path;
	path_ = path;

	consoleEnabled_ = CommandExists(cmdLine, "-console");
	doReload_ = CommandExists(cmdLine, "-reload");

	std::fstream fin(path + "/main.js");

	if (!fin)
	{
		MessageBoxA(GetDesktopWindow(), "Snuffbox cannot be ran from this directory.\nMissing 'main.js'", "Snuffbox Error", MB_ICONERROR);
		exit(1);
	}

	fin.close();
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
		shouldQuit_ = true;
		break;
	case GameEvents::kReload:
		shouldReload_ = true;
		break;
	}
}

//------------------------------------------------------------------------------------------------------
void Game::JSRender(JS_ARGS)
{
	JS_CHECK_PARAMS("O");

	Camera* camera = wrapper.GetPointer<Camera>(0);

	environment::render_device().SetCamera(camera);
}

//------------------------------------------------------------------------------------------------------
void Game::JSCleanUp(JS_ARGS)
{
  V8::LowMemoryNotification();
}

//------------------------------------------------------------------------------------------------------
void Game::RegisterJS(JS_TEMPLATE)
{
	JS_CREATE_SCOPE;

	JSFunctionRegister funcs[] = {
		JSFunctionRegister("render", JSRender),
		JSFunctionRegister("clearRenderer", JSClearRenderer),
    JSFunctionRegister("cleanUp", JSCleanUp),
		JSFunctionRegister("setName", JSSetName),
		JSFunctionRegister("showConsole", JSShowConsole),
    JSFunctionRegister("quit", JSQuit)
	};

	JS_REGISTER_OBJECT_FUNCTIONS(obj, funcs, false);
}

//------------------------------------------------------------------------------------------------------
void Game::JSClearRenderer(JS_ARGS)
{
	environment::render_device().Clear();
}

//------------------------------------------------------------------------------------------------------
void Game::JSShowConsole(JS_ARGS)
{
	if (environment::game().consoleEnabled())
	{
		environment::console().Show();
	}
}

//------------------------------------------------------------------------------------------------------
void Game::JSSetName(JS_ARGS)
{
	JS_CHECK_PARAMS("S");
	SetWindowTextA(environment::game().window()->handle(), std::string(environment::game().window()->params().name + " " + wrapper.GetString(0)).c_str());
}

//------------------------------------------------------------------------------------------------------
void Game::JSQuit(JS_ARGS)
{
  environment::game().NotifyEvent(GameEvents::kQuit);
}

//------------------------------------------------------------------------------------------------------
void Game::CreateCallbacks()
{
	JS_CREATE_SCOPE;

  Local<Context> ctx = JS_CONTEXT;

	Local<Object> global = ctx->Global();
	Local<Value> game = global->Get(String::NewFromUtf8(JS_ISOLATE,"Game"));

	JS_SETUP_CALLBACKS;

	SNUFF_XASSERT(game->IsObject(), "Could not find 'Game' object!");
	Handle<Object> obj = game->ToObject();

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
void Game::Run()
{
  Update();
	Draw();
}

//------------------------------------------------------------------------------------------------------
int SNUFF_MAIN
{
	QApplication app(__argc, __argv);
	QWidget console_window;
	ConsoleWidget console_widget(console_window);
	Console console(console_window, console_widget);
	
	AllocatedMemory memory;
	
	JSStateWrapper js_state_wrapper;
	SharedPtr<FileWatcher> file_watcher = environment::memory().ConstructShared<FileWatcher>();
	SharedPtr<FBXLoader> fbx_loader = environment::memory().ConstructShared<FBXLoader>();
	SharedPtr<IOManager> io_manager = environment::memory().ConstructShared<IOManager>();
	SharedPtr<ContentManager> content_manager = environment::memory().ConstructShared<ContentManager>();
	SharedPtr<D3D11Settings> render_settings = environment::memory().ConstructShared<D3D11Settings>();

	std::string windowName(
		"Snuffbox_" + std::string(SNUFF_DEBUG_MODE) + "_V_" +
		std::to_string(SNUFF_VERSION_MAJOR) + "." +
		std::to_string(SNUFF_VERSION_MINOR)
		);
	SharedPtr<Game> game = environment::memory().ConstructShared<Game>(
		environment::memory().ConstructShared<Win32Window>(windowName.c_str(), 640, 480),
		app);
	
	if (game->consoleEnabled())
	{
		console.Initialise(app);
		console_window.show();
	}
	
	environment::render_device().Initialise();
	js_state_wrapper.Initialise();

  SharedPtr<FontManager> font_manager = environment::memory().ConstructShared<FontManager>();

	game->Initialise();

	while (game->started())
	{
		if (game->consoleEnabled())
		{
			app.processEvents();
		}
		game->window()->ProcessMessages();
		game->Run();
	}
	return EXIT_SUCCESS;
}