var LevelState = {
	name: "level",
	_camera: undefined,
	_background: undefined,

	initialise: function()
	{
		this._camera = Camera.new("orthographic"),
		this._background = Quad.new(),

		this._camera.setTranslation(0,0,1000);
		this._camera.setFov(Math.PI/2);

		this._background.setTranslation(0,0,0);
		this._background.setOffset(0.5,0.5,0.5);
		this._background.setScale(4,1.6,1.6);
		this._background.setRotation(-Math.PI/2,0,0);
		this._background.setTexture("textures/background.png");
		this._background.spawn();

		StateManager.start();
	},

	update: function(dt)
	{
		if(Keyboard.isReleased("Enter"))
		{
			StateManager.switchState("menu");
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
		this._started = false;
		Log.debug("Destroyed the level");
	}
}