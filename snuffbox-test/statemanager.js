var StateManager = StateManager || {
	__state: null,
	__states: {},
	__started: false,
	__newState: null,

	getState: function()
	{
		return this.__state;
	},

	switchState: function(name)
	{
		this.__newState = this.__states[name];
		if(this.__newState == undefined)
		{
			Log.error("[StateManager] State '" + name + "' doesn't exist!");
			return;
		}
		
		Log.debug("[StateManager] Switched to state: " + name);

		if(this.__newState.initialise == undefined)
		{
			Log.error("[StateManager] Current state has no initiliase function!");
			return;
		}

		if(this.__newState.loadContent != undefined)
		{
			this.__newState.loadContent();
			ContentManager.idleCallback(this.initialise);
		}
		else
		{
			this.initialise()
		}
	},

	addState: function(obj)
	{
		this.__states[obj.name] = obj;
	},

	updateState: function(dt)
	{
		if(this.__state == undefined) return;
		if(this.__started)
		{
			if(this.__state.update == undefined)
			{
				Log.error("[StateManager] Current state has no update function!");
				return;
			}
			this.__state.update(dt);
		}
	},

	drawState: function(dt)
	{
		if(this.__state == undefined) return;
		if(this.__started)
		{
			if(this.__state.draw == undefined)
			{
				Log.error("[StateManager] Current state has no draw function!");
				return;
			}
			this.__state.draw(dt);
		}
	},

	start: function()
	{
		this.__started = true;
	},

	stop: function()
	{
		this.__started = false;
	},

	initialise: function()
	{
		if (StateManager.__state != null)
		{
			if(StateManager.__state.destroy == undefined)
			{
				Log.error("[StateManager] Current state has no destroy function!");
				return;
			}
			StateManager.__state.destroy();
			Game.cleanUp();
		}
		
		StateManager.stop();
		StateManager.__newState.initialise();
		StateManager.__state = StateManager.__newState;
		StateManager.start();
	}
};