var NPC = function(x,y)
{
	this.x = x || 0;
	this.y = y || 0;

	this.obj = Quad.new();
	this.obj.setTranslation(x,y,-0.1);
	this.obj.setRotation(-Math.PI/2,-Math.PI,0);
	this.obj.setScale(0.05,0.05,0.05);

	this.obj.spawn();
}