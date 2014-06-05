var MenuState = {
	_camera: undefined,
	_background: undefined,
	_logo: undefined,
	_logoBackground: undefined,
	_splitter: undefined,

	name: "menu",
	loadContent: function()
	{	
		ContentManager.load("texture","textures/menu/logo.png");
		ContentManager.load("texture","textures/menu/logo_background.png");
		ContentManager.load("texture","textures/menu/splitter.png");
	},

	initialise: function()
	{
		Log.info("Initialising menu");

		MenuState._camera = Camera.new("orthographic"),
		MenuState._background = Quad.new(),
		MenuState._logo = Quad.new(),
		MenuState._logoBackground = Quad.new(),
		MenuState._splitter = Quad.new(),

		MenuState._camera.setTranslation(0,0,1000);
		MenuState._camera.setFov(Math.PI/2);

		MenuState._background.setTranslation(0,0,0);
		MenuState._background.setOffset(0.5,0.5,0.5);
		MenuState._background.setScale(4,1.6,1.6);
		MenuState._background.setRotation(-Math.PI/2,0,0);
		MenuState._background.setTexture("textures/background.png");
		MenuState._background.spawn();

		MenuState._logo.setTranslation(0,0,2);
		MenuState._logo.setOffset(0.5,0.5,0.5);
		MenuState._logo.setScale(0,0,0);
		MenuState._logo.setTexture("textures/menu/logo.png");
		MenuState._logo.setRotation(-Math.PI/2,0,0);
		MenuState._logo.spawn();
		MenuState._logo.setAlpha(0);

		MenuState._logoBackground.setTranslation(0,-0.2,1);
		MenuState._logoBackground.setOffset(0.5,0.5,0.5);
		MenuState._logoBackground.setScale(1,1,1);
		MenuState._logoBackground.setTexture("textures/menu/logo_background.png");
		MenuState._logoBackground.setRotation(-Math.PI/2,0,0);
		MenuState._logoBackground.spawn();
		MenuState._logoBackground.setAlpha(0);

		MenuState._splitter.setTranslation(0,0,3);
		MenuState._splitter.setScale(0,0,0);
		MenuState._splitter.setAlpha(0);
		MenuState._splitter.setOffset(0.5,0,0.5);
		MenuState._splitter.setRotation(-Math.PI/2,0,0);
		MenuState._splitter.spawn();
		MenuState._splitter.setTexture("textures/menu/splitter.png");

		Log.info("Initialised menu");
		StateManager.start();
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

		this._logo.setRotation(-Math.PI/2,0,Math.sin(Game.timer/1.5)*0.3);
		this._logoBackground.setRotation(-Math.PI/2,0,Math.sin(Game.timer/1.5)*0.3);
	
		if (this._logoBackground.translation().y < 0.1)
		{
			this._logoBackground.translateBy(0,0.001,0);
		}

		if(Keyboard.isReleased("Enter"))
		{
			StateManager.switchState("level");
		}
	},

	draw: function(dt)
	{
		Game.render(this._camera);
	},

	destroy: function()
	{
		this._camera = null;
		this._background = null;
		this._logo = null;
		this._logoBackground = null;
		this._splitter = null;

		ContentManager.unload("texture","textures/menu/logo.png");
		ContentManager.unload("texture","textures/menu/logo_background.png");
		ContentManager.unload("texture","textures/menu/splitter.png");
		this._started = false;
		Log.debug("Destroyed the menu");
	}
}