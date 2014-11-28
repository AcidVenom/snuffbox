Game.Initialise = function()
{
	ContentManager.load("box", "test.box");

	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(800,600);
	RenderSettings.setVsync(false);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullFront);
	RenderSettings.setBackBufferColour(0,0,0,1);
	RenderSettings.setWindowSize(800,600);
	RenderSettings.setYDown(false);
	Game.camera = Camera.new("orthographic");
	Game.camera.setTranslation(0,0,0);

	Game.rich = Text.new();
	Game.rich.setFontSize(24);
	Game.rich.setAlignment(Text.Center);
	Game.rich.setText("[size=32][b]Oh hello.[/b][/size]\n
		\n
		[colour=FFFF00][i]I'm just testing rich text![/i][/colour]\n
		____________________________\n
		It's working [b][size=24]quite[/size][/b] well..\n
		[i][b][font=fonts/times.ttf]Have some funny characters ▲▼^&@[/font][/b][/i]\n
		[b][colour=CCCCCC]Text metrics[/colour][/b] do their [i][colour=00FF00]job[/colour][/i] too\n
		\n
		(This line is the default font, by the way)[colour=FFFF00]\n
		\n
		▲\n
		▲ ▲[/colour]");
	Game.rich.setTranslation(0,0,1);
	Game.rich.setShadowOffset(1,1);
	Game.rich.setShadowColour(0.5,0.5,0.2,1);
	Game.rich.spawn();
	Game.rich.alignVertical(true);

	var metrics = Game.rich.metrics();

	Game.box = Widget.new();
	Game.box.setSize(metrics.width + 80,metrics.height + 80);
	Game.box.spawn();
	Game.box.setBlend(0.1,0.1,0.1);
	Game.box.setOffset(0.5,0.5);
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
