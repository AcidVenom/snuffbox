var Test = function()
{
	this.x = 0
	this.y = 0
	this.update = function(dt)
	{
		this.x = Mouse.position().x;
		this.y = Mouse.position().y;

		if(Mouse.IsPressed(1))
			Log.error("Pressed");

		if(Mouse.IsDoubleClicked(1))
			Log.warning("Double clicked");

		if(Mouse.IsDown(1))
			Log.debug("X,Y: " + this.x + " | " + this.y);

		if(Mouse.IsReleased(1))
			Log.success("Released");
	}
}