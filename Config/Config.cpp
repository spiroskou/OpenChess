#include "Config.h"

static Config config;

static void init_config()
{
	config.AI_OPPONENT = true;
	config.depth = 4;
}

const Config & getConfigurations()
{
	init_config();

	return config;
}