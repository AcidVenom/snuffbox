require("loading");
require("statemanager");
require("states");

Game.timer = 0;

Game.Initialise = function()
{
	LoadStates();
	StateManager.switchState("menu");
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
	LoadStates();
	StateManager.switchState("menu");
}
