require("world");
require("player");

var LevelState = LevelState || {
	name: "level",
	_camera: undefined,
	_background: undefined,
	_world: undefined,
	_player: undefined
}

LevelState.loadContent = function()
{
	ContentManager.load("texture","textures/level/world.png");
	ContentManager.load("texture","textures/level/ground.png");
	ContentManager.load("texture","textures/level/clouds.png");
	ContentManager.load("texture","textures/level/mountains.png");
	ContentManager.load("texture","textures/level/treeline1.png");
	ContentManager.load("texture","textures/level/treeline2.png");
	ContentManager.load("texture","textures/characters/character_walk.png");
	ContentManager.load("texture","textures/effects/dust.png");
}

LevelState.initialise = function()
{
	this._camera = Camera.new("orthographic");
	this._background = new GameObject("textures/background.png");

	this._camera.setTranslation(0,0,1000);
	this._camera.setFov(Math.PI/2);

	this._background.setTranslation(0,0,0);
	this._background.setOffset(0.5,0.5,0.5);
	this._background.setScale(4,2,2);

	this._world = new World();
	this._player = new Player();
}

LevelState.update = function(dt)
{
	this._world.update(dt);
	this._player.update(dt,this._camera);
}

LevelState.draw = function(dt)
{
	Game.render(this._camera);
}

LevelState.destroy = function()
{
	ContentManager.unload("texture","textures/level/world.png");
	ContentManager.unload("texture","textures/level/ground.png");
	ContentManager.unload("texture","textures/level/clouds.png");
	ContentManager.unload("texture","textures/level/mountains.png");
	ContentManager.unload("texture","textures/level/treeline1.png");
	ContentManager.unload("texture","textures/level/treeline2.png");
	ContentManager.unload("texture","textures/characters/character_walk.png");
	ContentManager.unload("texture","textures/effects/dust.png");

	this._world = null;
	this._camera = null;
	this._background = null;
	this._player = null;
}