require("loading");
require("statemanager");
require("menu");
require("level");

Game.timer = 0;

Game.Initialise = function()
{
	StateManager.switchState(MenuState);
}
Game.Update = function(dt)
{	
	Game.timer += dt;
	StateManager.updateState(dt);
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
	StateManager.switchState(StateManager.getState());
}
