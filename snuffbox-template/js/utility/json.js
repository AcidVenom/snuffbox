Log.JSON = function(str)
{
	Log.rgb("[JSONManager] " + str, 100, 250, 220, 50, 100, 100);
}

var JSONManager = JSONManager || {
	_cache: {},
	_hotReload: {},

	load: function(path, key, reloading)
	{
		var result = IO.open(path);
		if (result === "")
		{
			return;
		}
		this._cache[key] = {
			result: JSON.parse(result),
			path: path
		}

		if (this._cache[key].result === undefined)
		{
			Log.error("Could not parse JSON file '" + path + "'");
			return;
		}
		this._hotReload[path] = true;

		if (reloading == false || reloading === undefined)
		{
			ContentManager.watch(path);
			Log.JSON("Added " + path + " to the file watch");
			Log.JSON("Parsed JSON from " + path + " stored in '" + key + "'");
		}
	},

	get: function(key)
	{
		var toReturn = this._cache[key];
		if (toReturn === undefined)
		{
			Log.error("Could not find loaded JSON with key '" + key + "'");
		}

		return toReturn.result;
	},

	reload: function(path)
	{
		if (this._hotReload[path] == true)
		{
			var cached = undefined;
			var key = undefined;

			for (var i in this._cache)
			{
				cached = this._cache[i];

				if (cached.path === path)
				{
					key = i;
					break;
				}
			}

			if (key !== undefined)
			{
				this.load(path, key, true);
				Log.JSON("Reloaded JSON file " + path);
			}
		}
	}
}