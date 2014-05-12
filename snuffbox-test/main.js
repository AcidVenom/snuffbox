require("test")

camera = Camera.new();
terrain = Terrain.new(128,128);
timer = 0;

Game.Initialise = function()
{

}

Game.Update = function(dt)
{	
	var mx = 0,
		mz = 0,
		rx = 0,
		rz = 0,
		speed = 2;

	if(Keyboard.isDown("W")) mz = 1;
	if(Keyboard.isDown("S")) mz = -1;
	if(Keyboard.isDown("A")) mx = -1;
	if(Keyboard.isDown("D")) mx = 1;

	if (Mouse.isDown(0))
	{
		rx = Mouse.movement().x;
		rz = Mouse.movement().y;
	}

	timer+=1*dt;
	camera.translateBy(mx*speed,0,mz*speed);
	camera.rotateBy(rx/300,rz/300,0);
}

Game.Draw = function(dt)
{
	Game.render(camera);
}

Game.Shutdown = function()
{
		
}
