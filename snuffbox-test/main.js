Game.Initialise = function()
{
	ContentManager.load("box", "test.box");

	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(800,600);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullFront);
	RenderSettings.setBackBufferColour(0.3,0.3,0.3,1);
	RenderSettings.setWindowSize(800,600);
	RenderSettings.setYDown(false);
	Game.camera = Camera.new("orthographic");
	Game.camera.setTranslation(0,0,0);
	Game.widget = Widget.new(undefined);
	Game.widget.setSize(64,64);
	Game.widget.spawn();

	Game.widget2 = Widget.new(Game.widget);
	Game.widget2.setSize(32,32);
	Game.widget2.setTranslation(0,0,1);
	Game.widget2.spawn();

	Game.widget3 = Widget.new(Game.widget2);
	Game.widget3.setSize(16,16);
	Game.widget3.setTranslation(0,0,2);
	Game.widget3.spawn();

	Game.widget4 = Widget.new(Game.widget3);
	Game.widget4.setSize(8,8);
	Game.widget4.setTranslation(0,0,3);
	Game.widget4.setBlend(1,1,1);
	Game.widget4.spawn();

	Game.widget.setTexture("tex.png");
	Game.widget2.setTexture("tex2.png");
	Game.timer = 0;

	Game.mouseArea = MouseArea.new(Game.widget);
	Game.mouseArea.setOnEnter(Game.onEnter,Game.widget);
	Game.mouseArea.setOnLeave(Game.onLeave,Game.widget);

	Game.text = Text.new(Game.widget);
	Game.text.setText("This is a test text\nWith align..\nNO REALLY IT HAS ALIGNMENT HOLY SHIT\nhaha..")
	Game.text.setFontSize(28);
	Game.text.setTranslation(0,0,4);
	Game.text.spawn();
	Game.text.setBlend(0,0,0);
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
}

Game.Draw = function(dt)
{
	Game.render(Game.camera);
}

Game.Shutdown = function()
{
		
}

Game.OnReload = function(file)
{

}
