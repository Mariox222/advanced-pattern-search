#include <iostream>
#include <fstream>
#include <string>
#include "plugin.h"
#include "advancedPatternSearch.h"
#include "../mempatWindow.h"

enum {
	MEMORY_PATTERN
};

// menuentry callback
PLUG_EXPORT void CBMENUENTRY(CBTYPE, PLUG_CB_MENUENTRY* info) {
	switch (info->hEntry)
	{
	case MEMORY_PATTERN: {
		// window here
		OpenMemPatWindow();
		
	}
	break;
	}
}

// comand callback
static bool mempat(int argc, char* argv[]) {
	if (argc != 2)
	{
		dputs("usage: mempat \"path\\to\\file.txt\"\n");
		return false;
	}
    std::vector<std::string> inputs;

	std::fstream fileStream;

	fileStream.open(argv[1], std::ios::in);
	if (fileStream.is_open()) {
		std::string tp;
		while (std::getline(fileStream, tp)) {
			inputs.push_back(tp);
		}

		fileStream.close();
	}
	else {
		dputs("error opening the file");
		return true;
	}

	std::string result;
	try{
		result = createOutput(inputs); 
	}
	catch (const std::invalid_argument& e) {
		dprintf("%s\n", e.what());
		return true;
	}
    dprintf("%s\n", result.c_str());

	return true; // C:\Users\Mario\Desktop\input.txt
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
	_plugin_menuaddentry(hMenu, MEMORY_PATTERN, "Open window");

}
