ContentManager.load("shader","shaders/custom.fx");
var terrain = terrain || undefined;
var quad = quad || Quad.new();
var quad2 = quad2 || Quad.new();
var widget = widget || Widget.new();

quad2.spawn();
quad2.setRotation(-Math.PI/2,0,0);
quad2.setScale(80,0,80);

var timer = timer || 0;

quad.spawn();
quad.setBlend(1,0,0);
quad.setRotation(-Math.PI/2,0,0);
quad.setScale(100,0,100);
quad.setOffset(0.5,0,0.5);
quad.setTranslation(-400,0,0);

if(terrain != undefined)
{
	terrain.setBlend(1,0.8,0.5);
}

Game.Initialise = function()
{
	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(1280,720);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	RenderSettings.setBackBufferColor(0,0,0,1);
	Game.camera = Camera.new("perspective");
	Game.camera.setTranslation(0,0,0);
	terrain = Terrain.new(128,128);
	terrain.spawn();
	terrain.setShader("shaders/custom.fx");
}
Game.Update = function(dt)
{	
	if(Keyboard.isReleased("F9"))
	{
		Game.showConsole();
	}

	var mx = 0,
		mz = 0,
		rx = 0,
		ry = 0;

	var delta = Mouse.movement();

	ry = -delta.x;
	rx = -delta.y;

	if (Keyboard.isDown("W")) mz = -1;
	if (Keyboard.isDown("S")) mz = 1;
	if (Keyboard.isDown("A")) mx = -1;
	if (Keyboard.isDown("D")) mx = 1;

	if (Mouse.isDown(0)) Game.camera.rotateBy(rx/100,ry/100,0);
	
	Game.camera.translateBy(mx,0,mz);

	++timer;


}

Game.Draw = function(dt)
{
	Game.render(Game.camera);
}

Game.Shutdown = function()
{
		
}

Game.OnReload = function()
{
	
}
