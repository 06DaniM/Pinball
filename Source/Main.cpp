#include "Application.h"
#include "Globals.h"

#include "raylib.h"

#include <stdlib.h>

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char ** argv)
{
	printf("Starting game '%s'...\n", TITLE);

	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	Application* App = NULL;

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			printf("-------------- Application Creation --------------\n");
			App = new Application();
			state = MAIN_START;
			break;

		case MAIN_START:

			printf("-------------- Application Init --------------\n");
			if (App->Init() == false)
			{
				printf("Application Init exits with ERROR");
				state = MAIN_EXIT;
			}
			else
			{
				state = MAIN_UPDATE;
				printf("-------------- Application Update --------------\n");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR)
			{
				printf("Application Update exits with ERROR\n");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			printf("-------------- Application CleanUp --------------\n");
			if (App->CleanUp() == false)
			{
				printf("Application CleanUp exits with ERROR");
			}
			else
				main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	printf("Exiting game '%s'...\n", TITLE);
	return main_return;
}