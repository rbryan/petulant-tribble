#include <stdlib.h>
#include <stdio.h>
#include <sge.h>

int keyFunc(SGEGAMESTATE *state, SGEEVENT *event)
{
	sgeGameStateManagerQuit(state->manager);
	return EVENT_HANDLED;
}

int run(int argc, char **argv)
{
	SGEGAMESTATEMANAGER *manager;
	SGEGAMESTATE *game_state;

	sgeInit(NOAUDIO, NOJOYSTICK);
	sgeOpenScreen("Terraria ... in ... SPAAAAAAACE!!!", 500, 500, 32, NOFULLSCREEN);

	game_state = sgeGameStateNew();
	game_state->onKeyDown = keyFunc;

	manager = sgeGameStateManagerNew();
	sgeGameStateManagerChange(manager, game_state);
	sgeGameStateManagerRun(manager, 30);

	sgeCloseScreen();
	return 0;
}
