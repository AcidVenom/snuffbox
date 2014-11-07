Game.Initialise = function()
{
	ContentManager.load("box", "test.box");
	ContentManager.load("texture", "tex.png");
	ContentManager.load("texture", "tex2.png");

	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(640,480);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullFront);
	RenderSettings.setBackBufferColour(0,0,0.8,1);
	RenderSettings.setWindowSize(640,480);
	RenderSettings.setYDown(false);
	Game.camera = Camera.new("orthographic");
	Game.camera.setTranslation(0,0,0);
	Game.widget = Widget.new();
	Game.widget.setSize(32,32);
	Game.widget.spawn();

	Game.widget2 = Widget.new(Game.widget);
	Game.widget2.setSize(16,16);
	Game.widget2.setTranslation(0,0,1);
	Game.widget2.spawn();

	Game.widget3 = Widget.new(Game.widget2);
	Game.widget3.setSize(16,16);
	Game.widget3.setTranslation(0,0,2);
	Game.widget3.spawn();

	Game.widget.setTexture("tex.png");
	Game.widget2.setTexture("tex2.png");
	Game.timer = 0;

	Game.mouseArea = MouseArea.new(Game.widget);
	Game.mouseArea.setOnEnter(Game.onEnter,Game.widget);
	Game.mouseArea.setOnLeave(Game.onLeave,Game.widget);
}

Game.onEnter = function(callee)
{
	callee.setAlpha(0.5);
}

Game.onLeave = function(callee)
{
	callee.setAlpha(1);
}

Game.Update = function(dt)
{	
	if(Keyboard.isReleased("F9"))
	{
		Game.showConsole();
	}
	
	Game.timer += dt;
	Game.widget.setTranslation(0,0,0);
	Game.widget.setOffset(0.5,0.5);
	Game.widget2.setTranslation(0,0,1);
	Game.widget2.setOffset(0.5,0.5);
	Game.widget3.setTranslation(0,0,2);
	Game.widget3.setOffset(0.5,0.5);
	Game.widget3.setScale(0.5,0.5);
	Game.widget3.setRotation(0,0,0);
	Game.widget3.setOffset(0.5,0.5);

	Game.widget2.setTranslation(16,-16, 2);
	Game.widget3.setTranslation(8,-8, 3);
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
