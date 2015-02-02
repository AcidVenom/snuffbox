/// The global object, should not be accessed from anything other than helper functions
var _GLOBAL_ = this;

/// Basic class inheritance
function extend(classA, classB)
{
	for (var b in classB)
	{
		if (b == "__quad")
		{
			continue;
		}
		classA[b] = classB[b];
	}
}

/**
* @class Enum
* @brief An enumerator class
* @author Daniël Konings
*/
var Enum = function(name, enumerations)
{
	this._name = name;
	for (var i = 0; i < enumerations.length; ++i)
	{
		this[enumerations[i]] = i;
	}
}

/// Changes the toString of the Enum class
Enum.prototype.toString = function()
{
	return (this._name);
}

/// Creates an enumerator
function enumerator(name, enumerations)
{
	_GLOBAL_[name] = new Enum(name,enumerations);
}

/// Draws a rectangle
function DrawRectangle(x1, y1, x2, y2, z, r, g, b, rr, gg, bb)
{
	if (x1 === undefined || y1 === undefined || x2 === undefined || y2 === undefined)
	{
		Log.error("Cannot draw a rectangle, not all required parameters were set! (x1, y1, x2, y2)");
		return;
	}

	var zz = z === undefined ? 1 : z;

	var r1 = r === undefined ? 1 : r;
	var g1 = g === undefined ? 1 : g;
	var b1 = b === undefined ? 1 : b;

	var r2 = rr === undefined ? r1 : rr;
	var g2 = gg === undefined ? g1 : gg;
	var b2 = bb === undefined ? b1 : bb;

	Line.draw(x1, y1, zz, r1, g1, b1, x2, y1, zz, r2, g2, b2);
	Line.draw(x2, y1, zz, r1, g1, b1, x2, y2, zz, r2, g2, b2);
	Line.draw(x1, y2, zz, r1, g1, b1, x2, y2, zz, r2, g2, b2);
	Line.draw(x1, y1, zz, r1, g1, b1, x1, y2, zz, r2, g2, b2);
}