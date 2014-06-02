require("loading")

var camera = camera || Camera.new("orthographic");
var blocks = blocks || [];
var timer = 0;

Game.Initialise = function()
{
	camera.setFov(120*Math.PI/180);
	
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
	if(Keyboard.isDown("A"))
	{
		camera.translateBy(-2*dt,0,0);
	}

	if(Keyboard.isDown("D"))
	{
		camera.translateBy(2*dt,0,0);
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
