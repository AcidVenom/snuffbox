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
	Game.rich.setText("[size=32][b]Oh hello.[/b][/size]\n\n[colour=FFFF00][i]I'm just testing rich text![/i][/colour]\n__________________\nIt's working [b][size=24]quite[/size][/b] well..\n[i][b][font=fonts/times.ttf]Have some funny characters ▲▼^&@[/font][/b][/i]");
	Game.rich.setTranslation(0,0,1);
	Game.rich.spawn();

	var metrics = Game.rich.metrics();

	Game.box = Widget.new();
	Game.box.setSize(metrics.width,metrics.height);
	Game.box.spawn();
	Game.box.setBlend(0.3,0.3,0.3);
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
