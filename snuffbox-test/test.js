var Test = function()
{
	this.x = 0;
	this.y = 0;

	this.update = function()
	{
		if(Keyboard.IsDown("W"))
		{
			Log.debug("Key W held down");
		}

		if(Mouse.IsDown(0))
		{
			Log.debug("Left mouse button held down");
		}
	}
}