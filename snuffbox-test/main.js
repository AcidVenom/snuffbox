require("test")

Game.timer = 0;

Game.Initialise = function()
{
	StateManager.switchState(State1);
}

Game.Update = function(dt)
{	

	Game.timer++;

	if(Game.timer == 50)
	{
		StateManager.switchState(State2);
	}

	if(Game.timer == 100)
	{
		StateManager.switchState(State1);
	}
}

Game.Draw = function(dt)
{

}

Game.Shutdown = function()
{
		
}
