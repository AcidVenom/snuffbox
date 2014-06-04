require("loading");
require("statemanager");

Game.timer = 0;

Game.Initialise = function()
{
	StateManager.addState({
		_camera: Camera.new("orthographic"),
		_background: Quad.new(),
		_logo: Quad.new(),

		name: "menu",
		initialise: function()
		{
			this._camera.setTranslation(0,0,0);
			this._camera.setFov(Math.PI);

			this._background.setTranslation(0,0,-10);
			this._background.setOffset(0.5,0.5,0.5);
			this._background.setScale(4,1.6,1.6);
			this._background.setRotation(-Math.PI/2,0,0);
			this._background.setTexture("textures/background.png");
			this._background.spawn();

			this._logo.setTranslation(0,0,-1);
			this._logo.setOffset(0.5,0.5,0.5);
			this._logo.setScale(0,0,0);
			this._logo.setTexture("textures/menu/logo.png");
			this._logo.setRotation(-Math.PI/2,0,0);
			this._logo.spawn();
			this._logo.setAlpha(0);
		},

		update: function(dt)
		{
			var scale = this._logo.scale();
			if (scale.x < 1.5)
			{
				this._logo.setScale(scale.x+0.015,scale.y+0.005,scale.z+0.01);
			}

			if (this._logo.alpha() < 1)
			{
				this._logo.setAlpha(this._logo.alpha() + 0.01);
			}
			this._logo.setRotation(-Math.PI/2,0,Math.sin(Game.timer*2)*0.1);
		},

		draw: function(dt)
		{
			Game.render(this._camera);
		},

		destroy: function()
		{

		}
	});

	StateManager.switchState("menu");
}
Game.Update = function(dt)
{	
	Game.timer += dt;
	StateManager.updateState(dt);
}

Game.Draw = function(dt)
{
	StateManager.drawState(dt);
}

Game.Shutdown = function()
{
		
}

Game.OnReload = function()
{
	StateManager.switchState("menu");
}
