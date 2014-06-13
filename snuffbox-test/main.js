var camera = camera || Camera.new("perspective");
camera.setFov(Math.PI/2);
camera.setTranslation(0,3,0);

var Cube = function(x,z)
{
	this.position = {x: x, z: z}
	this._color = [Math.random(),Math.random(),Math.random()];

	this._top = Quad.new();
	this._front = Quad.new();
	this._right = Quad.new();
	this._left = Quad.new();
	this._back = Quad.new();

	this._top.setUniform("float3",this._color[0],this._color[1],this._color[2]);
	this._front.setUniform("float3",this._color[0],this._color[1],this._color[2]);
	this._right.setUniform("float3",this._color[0],this._color[1],this._color[2]);
	this._left.setUniform("float3",this._color[0],this._color[1],this._color[2]);
	this._back.setUniform("float3",this._color[0],this._color[1],this._color[2]);

	this._top.setScale(0.57,1,0.9);
	this._front.setScale(0.6,1,1);
	this._right.setScale(0.6,1,1);
	this._left.setScale(0.6,1,1);
	this._back.setScale(0.6,1,1);

	this._front.setRotation(-Math.PI/2,0,0);
	this._right.setRotation(-Math.PI/2,-Math.PI/2,0);
	this._left.setRotation(-Math.PI/2,Math.PI/2,0);
	this._back.setRotation(-Math.PI/2,0,0);

	this._top.setOffset(0.5,0,0.5);
	this._front.setOffset(0.5,0,0.5);
	this._right.setOffset(0.5,0,0.5);
	this._left.setOffset(0.5,0,0.5);
	this._back.setOffset(0.5,0,0.5);

	this._top.spawn();
	this._front.spawn();
	this._right.spawn();
	this._left.spawn();
	this._back.spawn();

	this._top.setTranslation(this.position.x,0.5,this.position.z-0.3);
	this._front.setTranslation(this.position.x,0,this.position.z);
	this._right.setTranslation(this.position.x+0.29,0,this.position.z-0.29);
	this._left.setTranslation(this.position.x-0.29,0,this.position.z-0.29);
	this._back.setTranslation(this.position.x,0,this.position.z-0.6);
}

var cube = new Cube(0,-10);
var blocks = [];

Game.timer = 0;

Game.Initialise = function()
{
	Game.setName("Cubefield");
	RenderSettings.setResolution(1280,720);
	RenderSettings.setVsync(true);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	RenderSettings.setBackBufferColor(1,1,1,1);
}
Game.Update = function(dt)
{	
	if (Keyboard.isDown("Left"))
	{
		camera.translateBy(-6*dt,0,0);
	}
	if (Keyboard.isDown("Right"))
	{
		camera.translateBy(6*dt,0,0);
	}
	Game.timer += dt;
	if(Keyboard.isReleased("F9"))
	{
		Game.showConsole();
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
