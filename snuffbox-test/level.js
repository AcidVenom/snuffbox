require("world");

var LevelState = LevelState || {
	name: "level",
	_camera: undefined,
	_background: undefined,
	_world: undefined,
}

LevelState.loadContent = function()
{
	ContentManager.load("texture","textures/level/world.png");
	ContentManager.load("texture","textures/level/ground.png");
	ContentManager.load("texture","textures/level/clouds.png");
	ContentManager.load("texture","textures/level/mountains.png");
	ContentManager.load("texture","textures/level/treeline1.png");
	ContentManager.load("texture","textures/level/treeline2.png");
}

LevelState.initialise = function()
{
	this._camera = Camera.new("orthographic"),
	this._background = Quad.new(),

	this._camera.setTranslation(0,0,1000);
	this._camera.setFov(Math.PI/2);

	this._background.setTranslation(0,0,0);
	this._background.setOffset(0.5,0.5,0.5);
	this._background.setScale(4,1.6,1.6);
	this._background.setRotation(-Math.PI/2,0,0);
	this._background.setTexture("textures/background.png");
	this._background.spawn();
	this._world = new World();
}

LevelState.update = function(dt)
{
	this._world.update(dt);
}

LevelState.draw = function(dt)
{
	Game.render(this._camera);
}

LevelState.destroy = function()
{
	this._world = null;
	this._camera = null;
	this._background = null;
}