require("loading")
require("npc")

var camera = camera || Camera.new("orthographic");
var blocks = blocks || [];
var timer = 0;
var npc = new NPC(0,0);
camera.setFov(90*Math.PI/180);

Game.Initialise = function()
{
	
}

for(var i = 0; i < 10; ++i)
{
	blocks.push(Quad.new());
	blocks[i].setTranslation(i*0.05,0,0);
	blocks[i].setOffset(0.5,0.5,0.5);
	blocks[i].setScale(0.05,0.05,0.05);
	blocks[i].setTexture("textures/sprBlock.png");
	blocks[i].setRotation(-Math.PI/2,0,0);
	blocks[i].spawn();
}

Game.Update = function(dt)
{	

	var movement = Mouse.movement();
	
	if(Mouse.isDown(0))
	{
		camera.translateBy(-movement.x/500,0,0);
	}
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
