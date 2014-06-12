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
	this.speed = 0;
	this.maxSpeed = 2.5;

	this.update = function(dt,camera)
	{
		if(Keyboard.isDown("Left"))
		{
			if(this.speed > -this.maxSpeed)
			{
				this.speed -= 0.05;
			}

			this.setScale(0.075,0.075,0.075);
		}

		if(Keyboard.isDown("Right"))
		{
			if(this.speed < this.maxSpeed)
			{
				this.speed += 0.05;
			}

			this.setScale(-0.075,0.075,0.075);
		}

		if(!Keyboard.isDown("Right") && !Keyboard.isDown("Left"))
		{
			if(this.speed > -0.5 && this.speed < 0.5)
			{
				this.speed = 0;
			}

			if(this.speed > 0)
			{
				this.speed -=0.05;
			}

			if(this.speed < 0)
			{
				this.speed +=0.05;
			}
		}

		this.angle += this.speed*dt;
		this.setRotation(-Math.PI/2,0,-this.angle);

		var wobble = 0;
		if(Keyboard.isDown("Right") || Keyboard.isDown("Left"))
		{
			wobble = Math.abs(Math.sin(this.angle*10));
		}
		this.position.x = Math.sin(this.angle)*(this.radius+wobble/100);
		this.position.y = Math.cos(this.angle)*(this.radius+wobble/100);

		this.setTranslation(this.position.x,this.position.y,6);
		this.setAnimationSpeed(Math.abs(this.speed*(50/2.5)));
		this.updateAnimation(dt);
		this.updateCamera(camera);

		this.speedPreviousTick = this.speed;
	}

	this.updateCamera = function(camera)
	{
		var position = {x: this.position.x-Math.sin(this.angle)*0.2, y: this.position.y-Math.cos(this.angle)*0.2}
		var camTranslation = camera.translation();
		var movement = Math.smoothLerp(camTranslation.x,camTranslation.y,position.x-Math.sin(this.angle)*0.1,position.y,0.01);
		
		if (movement != true)
		{
			camera.translateBy(movement.x,movement.y,0);
		}
		else
		{
			camera.setTranslation(position.x,position.y,1000);
		}
	}
}