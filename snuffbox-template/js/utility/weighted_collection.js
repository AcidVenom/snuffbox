var WeightedCollection = WeightedCollection || {
	retrieve: function(tupleArray)
	{
		var final = [];

		for (var i = 0; i < tupleArray.length; ++i)
		{
			var tuple = tupleArray[i];
			for (var j = 0; j < tuple[1]; ++j)
			{
				final.push(tuple[0]);
			}
		}

		return final[Math.floor(Math.random()*final.length)];
	},

	retrieveAsTuple: function(tupleArray)
	{
		var final = [];

		for (var i = 0; i < tupleArray.length; ++i)
		{
			var tuple = tupleArray[i];
			for (var j = 0; j < tuple[1]; ++j)
			{
				final.push(tuple);
			}
		}

		return final[Math.floor(Math.random()*final.length)];
	}
}