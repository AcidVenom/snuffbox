require("loading");
require("gameobject");
require("statemanager");
require("menu");
require("level");

Game.timer = 0;

Game.Initialise = function()
{
	Game.setName("Dwarves & Goblins");
	RenderSettings.setResolution(1366,768);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	StateManager.switchState(MenuState);
}
Game.Update = function(dt)
{	
	Game.timer += dt;
	StateManager.updateState(dt);

	if(Keyboard.isReleased("F9"))
	{
		Game.showConsole();
	}
}

Game.Draw = function(dt)
{
	StateManager.drawState(dt);
}

Game.Shutdown = function()
{
		
}

Game.OnReload = function()
{
	RenderSettings.setVsync(false);
	StateManager.switchState(StateManager.getState());
}
