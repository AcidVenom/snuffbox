var StateManager = StateManager || {
	__state: null,
	__states: {},
	__started: false,

	getState: function()
	{
		return this.__state;
	},

	switchState: function(name)
	{
		this.__started = false;
		if(this.__states[name] == undefined)
		{
			Log.error("[StateManager] State '" + name + "' doesn't exist!");
			return;
		}

		if (this.__state != null)
		{
			if(this.__state.destroy == undefined)
			{
				Log.error("[StateManager] Current state has no destroy function!");
				return;
			}
			this.__state.destroy();
			Game.cleanUp();
		}

		this.__state = this.__states[name];
		
		Log.debug("[StateManager] Switched to state: " + name);

		if(this.__state.initialise == undefined)
		{
			Log.error("[StateManager] Current state has no initiliase function!");
			return;
		}

		if(this.__state.loadContent != undefined)
		{
			this.__state.loadContent();
			ContentManager.idleCallback(this.__state.initialise);
		}
		else
		{
			this.__state.initialise();
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
	}
};