require("js/utility/broadcaster");
require("js/utility/state_manager");
require("js/utility/math_extension");
require("js/utility/vector");
require("js/utility/weighted_collection");
require("js/utility/helper");
require("js/utility/json");

var RenderTargets = RenderTargets || {
	default: RenderTarget.new("Default")
}

Game.Initialise = function()
{
	Game.setName("Snuffbox Template");

	RenderSettings.setVsync(true);
	RenderSettings.setResolution(1280,720);
	RenderSettings.setYDown(true);
	RenderSettings.setWindowSize(1280,720);

	Game.debug = true;
	Game.speed = 1;
}

Game.FixedUpdate = function(timeSteps, fixedSteps)
{

}

Game.Update = function(dt)
{
	if (Game.debug == true)
	{
		var oldSpeed = Game.speed;
		if (Keyboard.isReleased("OEM4"))
		{
			Game.speed /= 1.5;
		}
		else if (Keyboard.isReleased("OEM6"))
		{
			Game.speed *= 1.5;
		}

		if (oldSpeed != Game.speed)
		{
			Log.rgb("Game speed changed to: " + Game.speed, 255, 255, 255, 127, 127, 127);
		}

		dt *= Game.speed;
	}
	StateManager.update(dt);
}

Game.Draw = function(dt)
{
	StateManager.draw(dt);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	StateManager.reload(path);
}