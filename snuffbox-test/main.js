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

	Game.box = Widget.new();
	Game.box.spawn();
	Game.box.setBlend(0.1,0.1,0.1);
	Game.box.setOffset(0.5,0.5);

	Game.rich = Text.new(Game.box);
	Game.rich.setFontSize(24);
	Game.rich.setAlignment(Text.Center);
	Game.rich.setText("[size=32][b]Oh hello[icon=textures/smiley_1.tif].[/b][/size]\n\n[colour=FFFF00][i]I'm just testing rich text![/i][/colour]\n____________________________\nIt's working [b][size=24]quite[/size][/b] well..\n[i][b][font=fonts/times.ttf]Have some funny characters ▲▼^&[/font][/b][/i]\n[b][colour=CCCCCC]Text metrics[/colour][/b] do their [i][colour=00FF00]job[/colour][/i] too\n\n[size=48](Icons also work[icon=textures/smiley_2.tif]yay!)[/size]");
	Game.rich.setTranslation(0,0,1);
	Game.rich.setShadowOffset(1,1);
	Game.rich.setShadowColour(0.5,0.5,0.2,1);
	Game.rich.setOffset(0,0.5);
	Game.rich.spawn();

	var metrics = Game.rich.metrics();

	Game.box.setSize(metrics.width + 80,metrics.height + 80);

	Game.FPS = Text.new();
	Game.FPS.setTranslation(-620,-340, 0);
	Game.FPS.setFontSize(24);
	Game.FPS.spawn();

	Game.deltas = [];

	Game.timer = 0;

	Game.polygon = Polygon.new();
	Game.polygon.addVertex(-640, -360, 0, 1,0,0,1, 0,0,1, 0,0);
	Game.polygon.addVertex(-640, 360, 0, 0,1,0,1, 0,0,1, 0,1);
	Game.polygon.addVertex(640, -360, 0, 0,0,1,1, 0,0,1, 1,0);
	Game.polygon.addVertex(640, 360, 0, 1,1,1,1, 0,0,1, 1,1);

	Game.polygon.addIndex(0);
	Game.polygon.addIndex(1);
	Game.polygon.addIndex(2);
	Game.polygon.addIndex(3);

	Game.polygon.addPass("shaders/pass.fx");

	Game.polygon.flush();

	Game.polygon.spawn();

	PostProcessing.addPass("shaders/gray.fx");
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
	var x = Math.cos(Game.timer)*20;
	var y = Math.sin(Game.timer)*20;
	Game.box.setTranslation(x,y,0);
	Game.box.setRotation(0,0,Math.sin(Game.timer)/7);
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
