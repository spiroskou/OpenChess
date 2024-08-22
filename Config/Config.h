#pragma once

struct Config
{
	bool AI_OPPONENT;
	int depth;
};

const Config & getConfigurations();
