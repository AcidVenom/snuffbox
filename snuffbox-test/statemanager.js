var StateManager = StateManager || {
	_state: undefined,
	_newState: undefined,
	_loaded: false,

	switchState: function(state)
	{
		Log.debug("[StateManager] Switching to state: " + state.name);
		StateManager._newState = state;
		StateManager._loaded = false;

		if(state.loadContent)
		{
			if(state == StateManager._state)
			{
				StateManager._state.destroy();
			}
			state.loadContent();
			ContentManager.idleCallback(StateManager.initialiseState);
		}
		else
		{
			StateManager.initialiseState();
		}
	},

	updateState: function(dt)
	{
		if(StateManager._loaded)
			StateManager._state.update(dt);
	},

	drawState: function(dt)
	{
		if(StateManager._loaded)
			StateManager._state.draw(dt);
	},

	getState: function()
	{
		return StateManager._state;
	},

	initialiseState: function()
	{
		if(StateManager._state && StateManager._state != StateManager._newState)
				StateManager._state.destroy();
		
		Game.cleanUp();
		StateManager._loaded = false;
		StateManager._newState.initialise();
		StateManager._state = StateManager._newState;
		StateManager._loaded = true;
	}
}