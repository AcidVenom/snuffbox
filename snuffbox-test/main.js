require("test")

var camera = camera || Camera.new();
var quads = [];
var timer = timer || 0;
var terrain = terrain || Terrain.new(256,256);
ContentManager.load("texture","textures/test.png");
ContentManager.load("shader","shaders/custom.fx");

terrain.setTexture("textures/test.png");
function CreateQuads()
{
	quads = [];
	for(var i = 0; i < 10; ++i)
	{
		var quad = Billboard.new();
		quad.setTranslation(0+i*3,0,0);
		quad.setRotation(-90*Math.PI/180,0,0);
		quad.setOffset(0.5,0,0.5);
		quad.setTexture("textures/test.png");
		quads.push(quad);
	}
}

Game.Initialise = function()
{
	CreateQuads();
}

Game.Update = function(dt)
{	
	var mx = 0,
		mz = 0,
		rx = 0,
		rz = 0,
		speed = 2;

	if(Keyboard.isDown("W")) mz = 1;
	if(Keyboard.isDown("S")) mz = -1;
	if(Keyboard.isDown("A")) mx = -1;
	if(Keyboard.isDown("D")) mx = 1;

	if (Mouse.isDown(0))
	{
		rx = Mouse.movement().x;
		rz = Mouse.movement().y;
	}

	timer+=dt;
	camera.translateBy(mx*speed*dt,0,mz*speed*dt);
	camera.rotateBy(rx/300,rz/300,0);

	var multiplier = 1;
	for(var i = 0; i < quads.length; ++i)
	{
		if(i % 2 == 0)
		{
			//quads[i].rotateBy(0,0,Math.sin(timer)*0.5);
			quads[i].setTranslation(0+i*3,1,0);
		}
		else 
		{
			//quads[i].rotateBy(0,0,Math.sin(timer)*0.5);
			quads[i].setTranslation(0+i*3,1,0);
		}

		//quads[i].setScale(0.5+Math.abs(Math.sin(timer)),0.5+Math.abs(Math.sin(timer)),0.5+Math.abs(Math.sin(timer)));
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
	CreateQuads();
}
