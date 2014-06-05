var World = function()
{
	this._mountains = Quad.new();
	this._mountains.setOffset(0.5,0,0.5);
	this._mountains.setRotation(-Math.PI/2,0,0);
	this._mountains.setTranslation(0,0,1);
	this._mountains.setTexture("textures/level/mountains.png");
	this._mountains.setScale(1.6,1.6,1.6);
	this._mountains.spawn();

	this._ground = Quad.new();
	this._ground.setOffset(0.5,0,0.5);
	this._ground.setRotation(-Math.PI/2,0,0);
	this._ground.setTranslation(0,0,2);
	this._ground.setScale(1.6,1.6,1.6);
	this._ground.setTexture("textures/level/ground.png");
	this._ground.spawn();

	this._treeline2 = Quad.new();
	this._treeline2.setOffset(0.5,0,0.5);
	this._treeline2.setRotation(-Math.PI/2,0,0);
	this._treeline2.setTranslation(0,0,3);
	this._treeline2.setTexture("textures/level/treeline2.png");
	this._treeline2.setScale(1.7,1.7,1.7);
	this._treeline2.spawn();

	this._world = Quad.new();
	this._world.setOffset(0.5,0,0.5);
	this._world.setRotation(-Math.PI/2,0,0);
	this._world.setTranslation(0,0,4);
	this._world.setTexture("textures/level/world.png");
	this._world.setScale(0.8,0.8,0.8);
	this._world.spawn();

	this._treeline1 = Quad.new();
	this._treeline1.setOffset(0.5,0,0.5);
	this._treeline1.setRotation(-Math.PI/2,0,0);
	this._treeline1.setTranslation(0,0,5);
	this._treeline1.setTexture("textures/level/treeline1.png");
	this._treeline1.setScale(1.6,1.6,1.6);
	this._treeline1.spawn();

	this.update = function(dt)
	{
		this._mountains.rotateBy(0,0,-0.002);
		this._treeline2.rotateBy(0,0,0.003);
	}
}