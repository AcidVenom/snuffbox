Math.distance = function(x1,y1,x2,y2)
{
	var delta = {x: x2-x1, y: y2-y1}
	return Math.sqrt(delta.x*delta.x + delta.y*delta.y);
}

Math.smoothLerp = function(x1,y1,x2,y2,speed)
{
	var movement = {x: 0, y: 0}
	var distance = Math.distance(x1,y1,x2,y2);
	if (distance > speed*distance)
	{
		var ratio = speed/distance;
		movement.x = ratio*(x2-x1)*distance;
		movement.y = ratio*(y2-y1)*distance;
	}
	else
	{
		return true;
	}

	return movement;
}