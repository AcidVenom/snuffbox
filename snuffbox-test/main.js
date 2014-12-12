Game.Initialise = function()
{
	ContentManager.load("box", "test.box");

	Game.setName("Snuffbox Test Project");
	RenderSettings.setResolution(1280,720);
	RenderSettings.setVsync(true);
	RenderSettings.setFullscreen(false);
	RenderSettings.setCullMode(RenderSettings.CullFront);
	RenderSettings.setBackBufferColour(0,0,0.4,1);
	RenderSettings.setWindowSize(1280,720);
	RenderSettings.setYDown(true);
	Game.camera = Camera.new("orthographic");
	Game.camera.setTranslation(0,0,0);

	Game.renderTarget = RenderTarget.new("Default");

	Game.FPS = Text.new();
	Game.FPS.setTranslation(-620,-340, 0);
	Game.FPS.setFontSize(24);
	Game.FPS.spawn("Default");

	Game.deltas = [];

	Game.timer = 0;
}

Game.Update = function(dt)
{	
	if(Keyboard.isReleased("F9"))
	{
		Game.showConsole();
	}
	
	Game.timer += dt;

	Game.deltas.push(dt);

	if (Game.deltas.length > 10)
	{
		Game.deltas.shift();
	}

	var sum = 0;

	for (var i = 0; i < Game.deltas.length; ++i)
	{
		sum += Game.deltas[i];
	}

	var average = sum / Game.deltas.length;
	var fps = Math.floor(1/average + 0.5);

	if (fps > 60)
	{
		fps = 60;
	}
	Game.FPS.setText("FPS: " + fps + "/60");
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
