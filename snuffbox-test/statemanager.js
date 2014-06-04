var StateManager = StateManager || {
	__state: null,
	__states: {},

	getState: function()
	{
		return this.__state;
	},

	switchState: function(name)
	{
		if (this.__state != null)
		{
			if(typeof(this.__state.destroy) == "undefined")
			{
				Log.error("[StateManager] Current state has no destroy function!");
				return;
			}
			this.__state.destroy();
		}

		this.__state = this.__states[name];
		if(typeof(this.__state) == "undefined")
		{
			Log.error("[StateManager] State '" + name + "' doesn't exist!");
			return;
		}

		Log.debug("[StateManager] Switched to state: " + name);

		if(typeof(this.__state.initialise) == "undefined")
		{
			Log.error("[StateManager] Current state has no initiliase function!");
			return;
		}
		this.__state.initialise();
	},

	addState: function(obj)
	{
		this.__states[obj.name] = obj;
	},

	updateState: function(dt)
	{
		if(typeof(this.__state.update) == "undefined")
		{
			Log.error("[StateManager] Current state has no update function!");
			return;
		}
		this.__state.update(dt);
	},

	drawState: function(dt)
	{
		if(typeof(this.__state.draw) == "undefined")
		{
			Log.error("[StateManager] Current state has no draw function!");
			return;
		}
		this.__state.draw(dt);
	}
};