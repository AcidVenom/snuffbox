require("animation");

function extend(a,b)
{
	for (var key in b)
	{
		a[key] = b[key];
	}
}

var GameObject = function(texture)
{
	this.__quad = Quad.new();
	extend(this,this.__quad);

	this.setTexture(texture);
	this.setRotation(-Math.PI/2,0,0);
	this.spawn();

	this.setUniform("float2","Frame",1,1);
	this.setUniform("float2","FrameOffset",0,0);

	this._animations = {};
	this._currentAnimation = undefined;

	this.addAnimation = function(animation)
	{
		this._animations[animation.name()] = animation;
	}

	this.setAnimation = function(name)
	{
		this._currentAnimation = this._animations[name];
	}

	this.startAnimation = function()
	{
		if (this._currentAnimation)
		{
			this._currentAnimation.start();
		}
	}

	this.pauseAnimation = function()
	{
		if (this._currentAnimation)
		{
			this._currentAnimation.pause();
		}
	}

	this.stopAnimation = function()
	{
		if (this._currentAnimation)
		{
			this._currentAnimation.stop();
		}
	}

	this.setAnimationSpeed = function(speed)
	{
		if (this._currentAnimation)
		{
			this._currentAnimation.setSpeed(speed);
		}
	}

	this.updateAnimation = function(dt)
	{
		if (this._currentAnimation)
		{
			this._currentAnimation.update(dt);
			this.setUniform("float2","Frame",this._currentAnimation.frame().w,this._currentAnimation.frame().h);
			this.setUniform("float2","FrameOffset",this._currentAnimation.frame().x,this._currentAnimation.frame().y);
		}
	}
}