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

	Game.text = Text.new();
	Game.text.setText("This is a test text\nWith very proper alignment, hooray\n(Yes really)");
	Game.text.setFontSize(28);
	Game.text.spawn();
	Game.text.setBlend(0,0,0);
	Game.text.setAlignment(Text.Center);

	var metrics = Game.text.metrics();

	Game.text.setTranslation(0,-metrics.height/2,0);

	Game.textStupid = Text.new();
	Game.textStupid.setText("Couldn't leave this ugly one out, could I?");
	Game.textStupid.setFontFamily("comicz.ttf");
	Game.textStupid.setFontSize(24);
	Game.textStupid.setBlend(1,0,0);
	Game.textStupid.setTranslation(-400,-300, 1);
	Game.textStupid.spawn();
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
