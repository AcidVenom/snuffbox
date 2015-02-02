var Vector2D = function(x, y)
{
	this._translation = {
		x: x,
		y: y
	}

	this.translation = function()
	{
		return this._translation;
	}

	this.setTranslation = function(x, y)
	{
		this._translation = {
			x: x,
			y: y
		}
	}

	this.dot = function(other)
	{
		return (this._translation.x * other.translation().x) + (this._translation.y * other.translation().y);
	}

	this.length = function()
	{
		var x = this._translation.x
		var y = this._translation.y
		return Math.sqrt(x * x + y * y);
	}

	this.normalise = function()
	{
		var l = this.length();
		return new Vector2D(this._translation.x / l, this._translation.y / l);
	}

	this.project = function(other)
	{
		var d = this.dot(other);
		var x = other.translation().x
		var y = other.translation().y
		var v = x * x + y * y
		return new Vector2D(d / v * x, d / v * y)
	}

	this.rightHandNormal = function()
	{
		return new Vector2D(-this._translation.y, this._translation.x);
	}

	this.leftHandNormal = function()
	{
		return new Vector2D(this._translation.y, -this._translation.x);
	}
}