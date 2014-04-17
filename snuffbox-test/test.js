function Test()
{
	Log.info("Created a test object, nothing to see here")
	this.x = 0;
	this.y = 0;
	this.timer = 0;
	this.reached = false;
	this.dead = false;

	this.update = function(dt)
	{
		this.timer += 1*dt;

		if(this.x < 2)
		{
			this.x+=1*dt;
		}
		else
		{
			if(!this.reached)
			{
				Log.debug("Reached my destination");
				this.reached = true;
			}
		}

		if(this.timer > 4)
		{
			this.Destroy();
		}
	}

	this.Destroy = function()
	{
		Log.debug("I was destroyed ):");
		this.dead = true;
	}
}