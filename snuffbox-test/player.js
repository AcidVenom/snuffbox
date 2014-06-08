var playerIdle = new Animation("idle");

for (var x = 0; x < 16; ++x)
{
	playerIdle.addFrame({
		x: x,
		y: 0,
		w: 1/16,
		h: 1
	})
}

playerIdle.setSpeed(60);

var Player = function()
{
	this.__gameObject = new GameObject("textures/characters/character_walk.png");
	extend(this,this.__gameObject);

	this.setTranslation(0,0,6);
	this.setScale(0.075,0.075,0.075);
	this.setOffset(0.5,0,0.5);
	this.offset = 0;

	this.addAnimation(playerIdle);
	this.setAnimation("idle");
	this.startAnimation();

	this.position = {x: 0, y: 0}
	this.angle = 0;
	this.radius = 0.45;
	this.speed = 2.5;

	this.update = function(dt)
	{
		if(Keyboard.isDown("Left"))
		{
			this.angle -= dt*this.speed;
		}

		if(Keyboard.isDown("Right"))
		{
			this.angle += dt*this.speed;
		}

		this.setRotation(-Math.PI/2,0,-this.angle);

		var wobble = 0;
		if(Keyboard.isDown("Right") || Keyboard.isDown("Left"))
		{
			wobble = Math.abs(Math.sin(this.angle*200));
		}
		this.position.x = Math.sin(this.angle)*(this.radius+wobble/70);
		this.position.y = Math.cos(this.angle)*(this.radius+wobble/70);

		this.setTranslation(this.position.x,this.position.y,6);
		this.updateAnimation(dt);
	}
}