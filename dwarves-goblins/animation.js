var AnimationFrame = function(x,y,w,h)
{
	this.x = x;
	this.y = y;
	this.w = w;
	this.h = h;
}

var Animation = function(name)
{
	this._name = name;
	this._frames = [];
	this._started = false;
	this._currentFrame = 0;
	this._frame = {};
	this._speed = 1;
	this._ticks = 0;

	this.name = function()
	{
		return this._name;
	}

	this.start = function()
	{
		this._callbacks["started"]();
		this._started = true;
	}

	this.pause = function()
	{
		this._callbacks["paused"]();
		this._started = false;
	}

	this.stop = function()
	{
		this._callbacks["ended"]();
		this._ticks = 0;
		this._currentFrame = 0;
		this._started = false;
	}

	this.setCallback = function(evt,cb)
	{
		this._callbacks[evt] = cb;
	}

	this.addFrame = function(params)
	{
		this._frames.push(new AnimationFrame(params.x,params.y,params.w,params.h));
	}

	this.update = function(dt)
	{
		this._ticks += dt*this._speed;
		if(this._ticks > 1)
		{
			this._ticks = 0;
			++this._currentFrame;
			this._callbacks["perFrame"]();
		}

		if(this._currentFrame >= this._frames.length)
		{
			this._currentFrame = 0;
			this._callbacks["ended"]();
		} 
	}

	this.setSpeed = function(speed)
	{
		this._speed = speed;
	}

	this.frame = function()
	{
		return this._frames[this._currentFrame];
	}

	this._callbacks = {ended: function(){}, started: function(){}, paused: function(){}, perFrame: function(){}}
}