require("loading")

var camera = camera || Camera.new("orthographic");
var blocks = [];
for(var i = 0; i < 20; ++i)
{
	blocks.push(Quad.new());
	blocks[i].setTranslation(i*0.05,0,0);
	blocks[i].setScale(0.05,0.05,0.05);
	blocks[i].setTexture("textures/sprBlock.png");
	blocks[i].setRotation(-Math.PI/2,0,0);
}
Game.Initialise = function()
{

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
