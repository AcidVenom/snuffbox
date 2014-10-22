Game.viewport = Game.viewport || Widget.new();
Game.viewport.setBlend(0,0,0.2);
Game.viewport.setSize(640,480);
Game.viewport.spawn();
Game.viewport.setTranslation(0,0,10);
Game.viewport.setOffset(0.5,0,0.5);

Game.widget = Game.widget || Widget.new();
Game.widget.setBlend(1,0,0);
Game.widget.setSize(64,64);
Game.widget.setTranslation(0,0,50);
Game.widget.setOffset(0.5,0,0.5);
Game.widget.spawn();

Game.widget2 = Game.widget2 || Widget.new(Game.widget);
Game.widget2.setBlend(0,1,1);
Game.widget2.setSize(16,16);
Game.widget2.spawn();
Game.widget2.setTranslation(0,0,100);
Game.widget2.setOffset(0.5,0,0.5);

Game.mouseArea = MouseArea.new(Game.widget2);

Game.Initialise = function()
{
	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(640,480);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	RenderSettings.setBackBufferColour(0,0,0,1);
	RenderSettings.setWindowSize(640,480);
	Game.camera = Camera.new("perspective");
	Game.camera.setTranslation(0,0,0);
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

	var speed = 100;

	if (Keyboard.isDown("W")) mz = -dt*speed;
	if (Keyboard.isDown("S")) mz = dt*speed;
	if (Keyboard.isDown("A")) mx = -dt*speed;
	if (Keyboard.isDown("D")) mx = dt*speed;

	if (Mouse.isDown(0)) Game.camera.rotateBy(rx/100,ry/100,0);
	
	Game.camera.translateBy(mx,0,mz);
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
