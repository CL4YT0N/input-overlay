/**
 * Created by univrsal on 23.05.2018.
 * This file is part of input-overlay which is licensed
 * under the MOZILLA PUBLIC LICENSE 2.0 - mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#pragma once

#include "util/SDL_helper.hpp"
#include "dialog/DialogSetup.hpp"
#include "dialog/DialogElementSettings.hpp"
#include "dialog/DialogHelp.hpp"
#include "dialog/DialogNewElement.hpp"
#include "util/Texture.hpp"
#include "util/Notifier.hpp"
#include "Config.hpp"
#include <SDL.h>

class SDL_helper;
class Element;
class Config;
class DialogElementSettings;
class DialogHelp;
class DialogNewElement;
class Texture;

enum ToolState
{
	IN_SETUP,
	IN_BUILD,
	IN_HELP,
	IN_NEW_ELEMENT
};

#define TOOL_ACTION_HELP_OPEN 0
#define TOOL_ACTION_HELP_EXIT 1
#define TOOL_ACTION_NEW_ELEMENT_OPEN 2
#define TOOL_ACTION_NEW_ELEMENT_EXIT 3
#define TOOL_ACTION_NEW_ELEMENT_ADD 4
#define TOOL_ACTION_MOD_ELEMENT_OPEN 5
#define TOOL_ACTION_MOD_ELEMENT_APPLY 6
#define TOOL_ACTION_SAVE_CONFIG 7
#define TOOL_ACTION_SETUP_EXIT 8

class Tool
{
public:
	Tool() { m_helper = nullptr; }
	Tool(SDL_helper * helper);
	~Tool();

	void program_loop();

	Element * get_selected(void);

	uint16_t get_selected_id(void);

	void action_performed(uint8_t type);

	Texture * get_atlas(void);

	void delete_element(uint16_t id);
private:
	void add_element(Element * e);

	void close_toplevel(void);
	void handle_input();
	bool m_run_flag = true;
	bool m_queue_close = false; /* True when top level dialog should be closed */

	SDL_Event m_event;
	SDL_helper * m_helper;

	Config * m_config = nullptr;

	DialogElementSettings * m_element_settings = nullptr;
	Dialog * m_toplevel = nullptr;
	Notifier * m_notify = nullptr;
	ToolState m_state;
};
