Game.widget = Game.widget || Widget.new();
Game.widget.setBlend(1,0,0);
Game.widget.setSize(64,64);
Game.widget.setTranslation(0,0,50);
Game.widget.setOffset(0,0,0);
Game.widget.spawn();

Game.widget2 = Game.widget2 || Widget.new(Game.widget);
Game.widget2.setBlend(0,1,1);
Game.widget2.setSize(16,16);
Game.widget2.spawn();
Game.widget2.setTranslation(0,0,100);
Game.widget2.setOffset(0,0,0);

Game.quad2D = Quad2D.new();
Game.quad2D.setSize(32,32);
Game.quad2D.setOffset(0.5,0.5,0);
Game.quad2D.setBlend(0,1,0);
Game.quad2D.setTranslation(0,0,0);
Game.quad2D.spawn();

Game.mouseArea = MouseArea.new(Game.widget2);

Game.timer = 0;
Game.onEnter = function(callee)
{
	callee.setBlend(1,1,0);
}

Game.onLeave = function(callee)
{
	callee.setBlend(0,1,1);
}

Game.onPressed = function(callee,button)
{
	if (button == 0)
	{
		callee.setBlend(1,0,1);
	}
	else if (button == 1)
	{
		callee.setBlend(0,1,0);
	}
}

Game.onReleased = function(callee,button)
{
	callee.setBlend(0,1,1);
}

Game.mouseArea.setOnPressed(Game.onPressed, Game.widget2);
Game.mouseArea.setOnEnter(Game.onEnter, Game.widget2);
Game.mouseArea.setOnLeave(Game.onLeave, Game.widget2);
Game.mouseArea.setOnReleased(Game.onReleased, Game.widget2);

Game.Initialise = function()
{
	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(640,480);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullNone);
	RenderSettings.setBackBufferColour(0,0,0,1);
	RenderSettings.setWindowSize(640,480);
	RenderSettings.setYDown(true);
	Game.camera = Camera.new("orthographic");
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
	
	Game.timer += dt*3;
	Game.camera.translateBy(mx,0,mz);
	Game.widget2.setAnchorTop();
	Game.widget2.setAnchorRight();
	Game.widget.setAnchorTop();
	Game.widget.setAnchorLeft();
	Game.quad2D.rotateBy(0,0,dt);
	Game.quad2D.translateBy(0,dt*10,0);
	RenderSettings.setYDown(true);
	//Game.widget.setTranslation(Math.sin(Game.timer)*40,0,1);
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
