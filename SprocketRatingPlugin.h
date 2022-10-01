#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class SprocketRatingPlugin: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow//, public BakkesMod::Plugin::PluginWindow
{
public:
	virtual void onLoad();
	virtual void onUnload();
	void openScoreboard(std::string eventName);
	void closeScoreboard(std::string eventName);
	void gameStart(std::string eventName);
	void gameEnd(std::string eventName);
	void unregister(std::string eventName);
	void render(CanvasWrapper canvas);
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
};

