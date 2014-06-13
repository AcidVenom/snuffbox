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

var DustParticle = function(x,y,z,speed,angle,parent)
{
	this.__gameObject = new GameObject("textures/effects/dust.png");
	extend(this,this.__gameObject);

	this._speed = speed/3 || 0.1;
	this._angle = angle || 0;
	this._parent = parent;
	this._radius = 0.42+Math.random()*0.02;
	this._increaseRatio = 0.1+Math.random()*0.3;
	this._z = z;
	this.setTranslation(x,y,z);
	this.setScale(0.01,0.01,0.01);
	this.setOffset(0.5,0,0.5);
	this.setRotation(-Math.PI/2,0,Math.random()*Math.PI);

	this._position = {x: Math.sin(this._angle)*this._radius, y: Math.cos(this._angle)*this._radius}

	this.update = function(dt)
	{
		this._angle -= this._speed*dt;
		this._radius += this._increaseRatio*dt;
		this._position.x = Math.sin(this._angle)*this._radius;
		this._position.y = Math.cos(this._angle)*this._radius;

		if(this.alpha() > 0)
		{
			this.setAlpha(this.alpha()-dt*2);
		}

		if(this.alpha() < 0)
		{
			this.setAlpha(0);
			this.destroy();
			this._parent._particles.splice(this._parent._particles.indexOf(this),1);
		}

		var scale = this.scale();
		this.setScale(scale.x+dt*0.2,scale.y+dt*0.2,scale.z+dt*0.2);

		this.rotateBy(0,0,0.1*dt);

		this.setTranslation(this._position.x,this._position.y,this._z);
	}
}

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

	this._particles = [];
	this._particleZ = 6.1;

	this.update = function(dt,camera)
	{
		if(Keyboard.isDown("Left"))
		{
			if(this.speed > -this.maxSpeed)
			{
				this.speed -= 5*dt;
			}

			this.setScale(0.075,0.075,0.075);
		}

		if(Keyboard.isDown("Right"))
		{
			if(this.speed < this.maxSpeed)
			{
				this.speed += 5*dt;
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
				this.speed -=10*dt;
			}

			if(this.speed < 0)
			{
				this.speed +=10*dt;
			}
		}

		if(Math.abs(this.speed) > 1.3)
		{
			this._particles.push(new DustParticle(this.position.x,this.position.y,this._particleZ,this.speed,this.angle,this));
			
			this._particleZ += 0.01;

			if(this._particleZ > 10)
			{
				this._particleZ = 6.1;
			}

			Log.watch("Particle Length",this._particles.length);
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
		this.updateCamera(dt,camera);

		for(var i = 0; i < this._particles.length; ++i)
		{
			this._particles[i].update(dt);
		}

		this.speedPreviousTick = this.speed;
	}

	this.updateCamera = function(dt,camera)
	{
		var position = {x: this.position.x-Math.sin(this.angle)*0.2, y: this.position.y-Math.cos(this.angle)*0.2}
		var camTranslation = camera.translation();
		var movement = Math.smoothLerp(camTranslation.x,camTranslation.y,position.x-Math.sin(this.angle)*0.1,position.y,dt*2);
		
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