/**
* @class StateManager
* @brief The state manager used for switching states
* @author Daniël Konings
*/
var StateManager = StateManager ||
{
	/// The current state
	_currentState: undefined,
	_pendingState: undefined,

	/// Switches the current state to a new state, destroying the current one
	switchState: function(state)
	{
		this._pendingState = state;
	},

	doSwitch: function()
	{
		Mouse.clearAreas();

		for (var i in RenderTargets)
		{
			RenderTargets[i].clear();
		}

		var newState = new this._pendingState();
		
		if (newState.name === undefined)
		{
			Log.error("[StateManager] State does not have a name!");
		}
		else if (newState.initialise === undefined)
		{
			assert("[StateManager] State does not have an initialise function!");
		}
		else if (newState.destroy === undefined)
		{
			assert("[StateManager] State does not have a destroy function!");
		}
		else
		{
			if (this._currentState !== undefined)
			{
				this._currentState.destroy();
				var name = this._currentState.name;
				for (var i in this._currentState)
				{
					this._currentState[i] = null;
				}

				Log.info("[StateManager] Destroyed the state '" + name + "'");
			}

			newState.initialise();
			Log.info("[StateManager] Initialised the state '" + newState.name + "'");

			this._currentState = newState;

			if (this._currentState.name !== undefined)
			{
				Log.debug("[StateManager] Switched to state '" + this._currentState.name + "'");
			}
			else
			{
				Log.debug("[StateManager] Switched to a state with no name");
			}
		}

		this._pendingState = undefined;
		Game.cleanUp();
	},

	/// Returns the current state
	getState: function()
	{
		return this._currentState;
	},

	update: function(dt)
	{
		if (this._currentState == undefined)
			return;

		if (this._currentState.update !== undefined)
		{
			this._currentState.update(dt);
		}
	},

	draw: function(dt)
	{
		if (this._pendingState !== undefined)
		{
			this.doSwitch();
		}
		
		if (this._currentState == undefined)
			return;

		if (this._currentState.draw !== undefined)
		{
			this._currentState.draw(dt);
		}
	},

	reload: function(path)
	{
		JSONManager.reload(path);
		if (this._currentState == undefined)
			return;
		
		if (this._currentState.reload !== undefined)
		{
			this._currentState.reload(path);
		}

		Game.cleanUp();

		Log.success("[StateManager] Succesfully reloaded");
	}
}