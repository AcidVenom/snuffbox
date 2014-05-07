require("test")

camera = Camera.new();
timer = 0;

Game.Initialise = function()
{

}

Game.Update = function(dt)
{	
	var mx = 0,
		mz = 0,
		speed = 0.1;

	if(Keyboard.isDown("W")) mz = 1;
	if(Keyboard.isDown("S")) mz = -1;
	if(Keyboard.isDown("A")) mx = -1;
	if(Keyboard.isDown("D")) mx = 1;

	timer++;
	camera.translateBy(mx*speed,mz*speed,0);
}

Game.Draw = function(dt)
{
	Game.render(camera);
}

Game.Shutdown = function()
{
		
}
