ContentManager.load("shader","shaders/custom.fx");
ContentManager.load("texture","textures/snuffbox.png");
ContentManager.load("model","models/axew.fbx");
var terrain = terrain || undefined;
var widget = widget || undefined; 
var widget2 = widget2 || undefined;
var mesh = mesh || Mesh.new("models/axew.fbx");

mesh.spawn();

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

	timer += dt;

	widget2.rotateBy(dt*2,dt*6,dt*4);
	widget2.setScale(64+Math.sin(timer)*40,64+Math.sin(timer)*40,64+Math.sin(timer)*40);
	widget2.setAlpha(0.5);
	widget2.setTranslation(66,71,0);
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
