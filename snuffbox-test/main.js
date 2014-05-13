require("test")

camera = Camera.new();
quads = new Array();
timer = 0;

Game.Initialise = function()
{
	for(var i = 0; i < 1000; ++i)
	{
		var quad = Quad.new();
		quad.setTranslation(0+i*3,0,0);
		quad.setRotation(90*Math.PI/180,90*Math.PI/180,0);
		quad.setOffset(0.5,0,0.5);
		quads.push(quad);
	}
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

	for(var i = 0; i < quads.length; ++i)
	{
		if(i % 2 == 0)
		{
			quads[i].rotateBy(0,0,Math.sin(timer)*0.5);
			quads[i].setTranslation(0+i*3,Math.sin(timer)*3,0);
		}
		else 
		{
			quads[i].rotateBy(0,0,-Math.sin(timer)*0.5);
			quads[i].setTranslation(0+i*3,-Math.sin(timer)*3,0);
		}

		quads[i].setScale(0.5+Math.abs(Math.sin(timer)),0.5+Math.abs(Math.sin(timer)),0.5+Math.abs(Math.sin(timer)));

	}
}

Game.Draw = function(dt)
{
	Game.render(camera);
}

Game.Shutdown = function()
{
		
}
