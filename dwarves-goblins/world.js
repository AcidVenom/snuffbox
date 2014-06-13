var World = function()
{
	this._mountains = new GameObject("textures/level/mountains.png");
	this._mountains.setOffset(0.5,0,0.5);
	this._mountains.setTranslation(0,0,1.5);
	this._mountains.setScale(1.6,1.6,1.6);

	this._ground = new GameObject("textures/level/ground.png");
	this._ground.setOffset(0.5,0,0.5);
	this._ground.setTranslation(0,0,2);
	this._ground.setScale(1.6,1.6,1.6);

	this._treeline2 = new GameObject("textures/level/treeline2.png");
	this._treeline2.setOffset(0.5,0,0.5);
	this._treeline2.setTranslation(0,0,3);
	this._treeline2.setScale(1.7,1.7,1.7);

	this._world = new GameObject("textures/level/world.png");
	this._world.setOffset(0.5,0,0.5);
	this._world.setTranslation(0,0,4);
	this._world.setScale(0.8,0.8,0.8);

	this._treeline1 = new GameObject("textures/level/treeline1.png");
	this._treeline1.setOffset(0.5,0,0.5);
	this._treeline1.setTranslation(0,0,5);
	this._treeline1.setScale(1.6,1.6,1.6);

	this.update = function(dt)
	{
		this._mountains.rotateBy(0,0,-0.2*dt);
		this._treeline2.rotateBy(0,0,0.3*dt);
	}
}