require("test")

Game.Initialise = function()
{
	test = new Test();
}

Game.Update = function(dt)
{	
	test.update(dt);
}

Game.Draw = function(dt)
{

}

Game.Shutdown = function()
{

}