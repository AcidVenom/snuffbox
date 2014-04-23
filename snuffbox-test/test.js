var Test = function()
{
	this.x = 0
	this.y = 0
	this.update = function(dt)
	{
		this.x = Mouse.position().x;
		this.y = Mouse.position().y;

		if(Mouse.IsPressed(0))
			Log.error("Pressed");

		if(Mouse.IsDown(0))
			Log.debug("X,Y: " + this.x + " | " + this.y);

		if(Mouse.IsReleased(0))
			Log.success("Released");
	}
}