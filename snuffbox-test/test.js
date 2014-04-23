var Test = function()
{
	this.x = 0
	this.y = 0
	this.update = function(dt)
	{
		this.x = Mouse.position().x;
		this.y = Mouse.position().y;
		Log.debug("Delta time: " + dt);
	}
}