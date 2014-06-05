require("menu");
require("level");

function LoadStates()
{
	StateManager.addState(MenuState);
	StateManager.addState(LevelState);
}