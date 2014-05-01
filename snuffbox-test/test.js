var Bluh = function()
{
	this.testObj = TestObject.new();
}

var State1 = {
	arr: new Array(),

	Initialise: function()
	{
		for(var i = 0; i < 10; ++i)
		{
			this.arr.push(new Bluh());
		}
		Log.debug("Switched to state 1");
	},

	Destroy: function()
	{
		this.arr = [];
		Log.debug("Destroyed state1");
	}
}

var State2 = {

	Initialise: function()
	{
		Log.debug("Switched to state 2");
	},

	Destroy: function()
	{
		Log.debug("Destroyed state2");
	}
}

var StateManager = {
	_currentState: undefined,
	switchState: function(state)
	{
		if (!this._currentState)
		{
			this._currentState = state;
			this._currentState.Initialise();
		}
		else
		{
			this._currentState.Destroy();
			this._currentState = state;
			this._currentState.Initialise();
		}
	}
}