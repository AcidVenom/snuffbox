require("loading")
require("npc")

var camera = camera || Camera.new("perspective");
var blocks = [];
var timer = 0;
var terrain = Terrain.new(256,256);
terrain.setShader("shaders/custom.fx");
terrain.setTexture("textures/sprBlock.png");
camera.setFov(120*Math.PI/180);
terrain.spawn();
Game.Initialise = function()
{
	
}

for(var i = 0; i < 100; ++i)
{
	blocks.push(Billboard.new());
	blocks[i].setTranslation(i*2,0,0);
	blocks[i].setOffset(0.5,0.5,0.5);
	blocks[i].setScale(2,2,2);
	blocks[i].setTexture("textures/sprAlphaTest.png");
	blocks[i].setRotation(-Math.PI/2,0,0);
	blocks[i].spawn();
}

Game.Update = function(dt)
{	
	var mx = 0,
		mz = 0;

	var speed = 0.1;

	if(Keyboard.isDown("W")) mz = -speed;
	if(Keyboard.isDown("S")) mz = speed;
	if(Keyboard.isDown("A")) mx = -speed;
	if(Keyboard.isDown("D")) mx = speed;

	var movement = Mouse.movement();
	
	if(Mouse.isDown(0))
	{
		camera.rotateBy(-movement.y/200,-movement.x/200,0);
	}

	camera.translateBy(mx,0,mz)
}

Game.Draw = function(dt)
{
	Game.render(camera);
}

Game.Shutdown = function()
{
		
}

Game.OnReload = function()
{

}
