#include "plugin.h"
#include "advancedPatternSearch.h"

static bool mempat(int argc, char* argv[]) {
	if (argc != 2)
	{
		dputs("usage: \"mempat line\"\n");
		return false;
	}
    std::vector<std::string> inputs;
    std::string str = argv[1];
    inputs.push_back(std::string(argv[1]));
    std::string result = createOutput(inputs);
    dprintf("%s\n", result.c_str());

	return true;

}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    _plugin_registercommand(pluginHandle, "mempat", mempat, false);
    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{

}
