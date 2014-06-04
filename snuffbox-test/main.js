require("loading")
require("npc")

var camera = camera || Camera.new("perspective");
var blocks = [];
var timer = 0;
var terrain = Terrain.new(256,256);
terrain.setShader("shaders/custom.fx");
terrain.setTexture("textures/sprBlock.png");
camera.setFov(120*Math.PI/180);

Game.Initialise = function()
{
	
}

for(var i = 0; i < 100; ++i)
{
	blocks.push(Billboard.new());
	blocks[i].setTranslation(i*10,-20,0);
	blocks[i].setOffset(0.5,0.5,0.5);
	blocks[i].setScale(30,30,30);
	blocks[i].setTexture("textures/sprAlphaTest.png");
	blocks[i].setRotation(-Math.PI/2,0,0);
	blocks[i].spawn();
}

terrain.spawn();

Game.Update = function(dt)
{	
	var mx = 0,
		mz = 0;

	var speed = 2;

	if(Keyboard.isDown("W")) mz = -speed;
	if(Keyboard.isDown("S")) mz = speed;
	if(Keyboard.isDown("A")) mx = -speed;
	if(Keyboard.isDown("D")) mx = speed;

	var movement = Mouse.movement();
	
	if(Mouse.isDown(0))
	{
		camera.rotateBy(-movement.y/200,-movement.x/200,0);
	}

	for(var i = 0; i < blocks.length; ++i)
	{
		var translation = blocks[i].translation();
		blocks[i].setTranslation(i*10,Math.sin(translation.x+timer)*10,Math.cos(translation.x+timer)*10);
	}

	camera.translateBy(mx,0,mz)
	timer+=dt;
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
