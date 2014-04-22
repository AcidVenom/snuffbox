require("test")

Game.Initialise = function()
{
	test = new Test();
	// test = 0; // Checking if instanceof works already, seemingly it does. Perfect.
}

Game.Update = function(dt)
{	

	if (typeof(test) != "undefined")
	{
		if(test instanceof Test)
		{
			test.update(dt);
			if (test.dead)
			{
				delete test;
			}
		}
		else
		{
			assert("type of 'test' is not of Test()");
		}
	}

	Log.error("asdasdasdsad");
}

Game.Draw = function(dt)
{

}

Game.Shutdown = function()
{

}