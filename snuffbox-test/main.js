ContentManager.load("shader","shaders/custom.fx");
ContentManager.load("shader","shaders/lighting.fx");
ContentManager.load("texture","textures/snuffbox.png");
ContentManager.load("model","models/axew.fbx");
ContentManager.load("texture","textures/axew.png");
ContentManager.load("model","models/totodile.fbx");
ContentManager.load("texture","textures/totodile.png");
ContentManager.load("model","models/gible.fbx");
ContentManager.load("texture","textures/gible.png");
ContentManager.load("model","models/scraggy.fbx");
ContentManager.load("texture","textures/scraggy.png");
var terrain = terrain || undefined;
var widget = widget || undefined; 
var widget2 = widget2 || undefined;
var mesh = mesh || Mesh.new("models/axew.fbx");
var mesh2 = mesh2 || Mesh.new("models/totodile.fbx");
var mesh3 = mesh3 || Mesh.new("models/gible.fbx");
var mesh4 = mesh4 || Mesh.new("models/scraggy.fbx");

mesh.spawn();
mesh.setTexture("textures/axew.png");
mesh.setShader("shaders/lighting.fx");
mesh.setScale(10,10,10);
mesh.setTranslation(-30,0,0);
mesh.setRotation(0,0,0);

mesh2.spawn();
mesh2.setTexture("textures/totodile.png");
mesh2.setShader("shaders/lighting.fx");
mesh2.setScale(10,10,10);
mesh2.setTranslation(-90,0,0);

mesh3.spawn();
mesh3.setTexture("textures/gible.png");
mesh3.setShader("shaders/lighting.fx");
mesh3.setScale(10,10,10);
mesh3.setTranslation(-150,0,0);

mesh4.spawn();
mesh4.setTexture("textures/scraggy.png");
mesh4.setShader("shaders/lighting.fx");
mesh4.setScale(10,10,10);
mesh4.setTranslation(-210,0,0);

var timer = timer || 0;

Game.Initialise = function()
{
	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(1280,720);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	RenderSettings.setBackBufferColor(0,0,0.5,1);
	Game.camera = Camera.new("perspective");
	Game.camera.setTranslation(0,0,0);
	terrain = Terrain.new(128,128);
	terrain.spawn();
	terrain.setShader("shaders/custom.fx");
	widget = Widget.new();
	widget.setScale(256,0,256);
	widget.setTexture("textures/snuffbox.png");
	widget.setAnchorTop();
	widget.setAnchorLeft();
	widget.spawn();

	widget2 = Widget.new(widget);
	widget2.setScale(64,0,64);
	widget2.setBlend(1,0,0);
	widget2.spawn();
	widget2.setOffset(0.5,0,0.5);
}
Game.Update = function(dt)
{	
	Log.watch("dt",dt);
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

	var speed = 100;

	if (Keyboard.isDown("W")) mz = -dt*speed;
	if (Keyboard.isDown("S")) mz = dt*speed;
	if (Keyboard.isDown("A")) mx = -dt*speed;
	if (Keyboard.isDown("D")) mx = dt*speed;

	if (Mouse.isDown(0)) Game.camera.rotateBy(rx/100,ry/100,0);
	
	Game.camera.translateBy(mx,0,mz);

	timer += dt;

	widget2.rotateBy(dt*2,dt*6,dt*4);
	widget2.setScale(64+Math.sin(timer)*40,64+Math.sin(timer)*40,64+Math.sin(timer)*40);
	widget2.setAlpha(0.5);
	widget2.setTranslation(66,71,0);

	if (Mouse.isDown(1))
	{
		mesh.rotateBy(-rx/100,-ry/100,0);
		mesh2.rotateBy(-rx/100,-ry/100,0);
		mesh3.rotateBy(-rx/100,-ry/100,0);
		mesh4.rotateBy(-rx/100,-ry/100,0);
	}
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
