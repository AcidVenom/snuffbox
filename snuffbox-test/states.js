function LoadStates()
{
	StateManager.addState({
		_camera: Camera.new("orthographic"),
		_background: Quad.new(),
		_logo: Quad.new(),
		_logoBackground: Quad.new(),
		_splitter: Quad.new(),

		name: "menu",
		initialise: function()
		{
			this._camera.setTranslation(0,0,1000);
			this._camera.setFov(Math.PI/2);

			this._background.setTranslation(0,0,0);
			this._background.setOffset(0.5,0.5,0.5);
			this._background.setScale(4,1.6,1.6);
			this._background.setRotation(-Math.PI/2,0,0);
			this._background.setTexture("textures/background.png");
			this._background.spawn();

			this._logo.setTranslation(0,0,2);
			this._logo.setOffset(0.5,0.5,0.5);
			this._logo.setScale(0,0,0);
			this._logo.setTexture("textures/menu/logo.png");
			this._logo.setRotation(-Math.PI/2,0,0);
			this._logo.spawn();
			this._logo.setAlpha(0);

			this._logoBackground.setTranslation(0,-0.2,1);
			this._logoBackground.setOffset(0.5,0.5,0.5);
			this._logoBackground.setScale(1,1,1);
			this._logoBackground.setTexture("textures/menu/logo_background.png");
			this._logoBackground.setRotation(-Math.PI/2,0,0);
			this._logoBackground.spawn();
			this._logoBackground.setAlpha(0);

			this._splitter.setTranslation(0,0,3);
			this._splitter.setScale(0,0,0);
			this._splitter.setAlpha(0);
			this._splitter.setOffset(0.5,0,0.5);
			this._splitter.setRotation(-Math.PI/2,0,0);
			this._splitter.spawn();
			this._splitter.setTexture("textures/menu/splitter.png");
		},

		update: function(dt)
		{
			var scale = this._logo.scale();
			var splitterScale = this._splitter.scale();
			if (scale.x < 0.8)
			{
				this._logo.setScale(scale.x+0.01,scale.y,scale.z+0.006);
				this._splitter.setScale(splitterScale.x+0.004,splitterScale.y,splitterScale.z+0.001);
			}

			if (this._logo.alpha() < 1)
			{
				this._logo.setAlpha(this._logo.alpha() + 0.01);
				this._splitter.setAlpha(this._splitter.alpha() + 0.01);
				this._logoBackground.setAlpha(this._logoBackground.alpha() + 0.01)
			}

			this._logo.setRotation(-Math.PI/2,0,Math.sin(Game.timer/1.5)*0.2);
			this._logoBackground.setRotation(-Math.PI/2,0,Math.sin(Game.timer/1.5)*0.2);
		
			if (this._logoBackground.translation().y < 0.1)
			{
				this._logoBackground.translateBy(0,0.001,0);
			}
		},

		draw: function(dt)
		{
			Game.render(this._camera);
		},

		destroy: function()
		{

		}
	});
}