#include "pch.h"
#include "SprocketRatingPlugin.h"


// Plugin Settings Window code here
std::string SprocketRatingPlugin::GetPluginName() {
	return "Sprocket Rating";
}

void SprocketRatingPlugin::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> SprocketRatingPlugin
void SprocketRatingPlugin::RenderSettings() {

	// Enabled Checkbox
	CVarWrapper enableCvar = cvarManager->getCvar("sr_enableBool");
	if (!enableCvar) { return; }

	bool enabled = enableCvar.getBoolValue();

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle plugin");
	}

	// Always On Checkbox
	CVarWrapper alwaysOnCvar = cvarManager->getCvar("sr_alwaysOn");
	if (!alwaysOnCvar) { return; }

	bool alwaysOn = alwaysOnCvar.getBoolValue();

	if (ImGui::Checkbox("Always on", &alwaysOn)) {
		alwaysOnCvar.setValue(alwaysOn);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle display always on");
	}

	// Background Opacity Slider
	CVarWrapper bgOpacityCvar = cvarManager->getCvar("sr_bgOpacity");
	if (!bgOpacityCvar) { return; }

	int bgOpacity = bgOpacityCvar.getIntValue();

	if (ImGui::SliderInt("Overlay background opacity", &bgOpacity, 0, 255, "%d")) {
		bgOpacityCvar.setValue(bgOpacity);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Opacity of the in-game overlay");
	}

	// bgWidth 0-500 int
	// bgHeight 0-500 int
	// x pos % float
	// y pos % float

	// Background Width Slider
	CVarWrapper bgWidthCvar = cvarManager->getCvar("sr_bgWidth");
	if (!bgWidthCvar) { return; }

	int bgWidth = bgWidthCvar.getIntValue();

	if (ImGui::SliderInt("Overlay background width", &bgWidth, 0, 500, "%d")) {
		bgWidthCvar.setValue(bgWidth);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Width of the in-game overlay");
	}

	// Background height Slider
	CVarWrapper bgHeightCvar = cvarManager->getCvar("sr_bgHeight");
	if (!bgHeightCvar) { return; }

	int bgHeight = bgHeightCvar.getIntValue();

	if (ImGui::SliderInt("Overlay background height", &bgHeight, 0, 500, "%d")) {
		bgHeightCvar.setValue(bgHeight);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Height of the in-game overlay");
	}

	// X Position Percentage
	CVarWrapper bgXCvar = cvarManager->getCvar("sr_bgX");
	if (!bgXCvar) { return; }

	int bgX = bgXCvar.getFloatValue();

	if (ImGui::SliderInt("Overlay X location", &bgX, 0, 100, "%d")) {
		bgXCvar.setValue(bgX);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("X location of the in-game overlay");
	}

	// X Position Percentage
	CVarWrapper bgYCvar = cvarManager->getCvar("sr_bgY");
	if (!bgYCvar) { return; }

	int bgY = bgYCvar.getFloatValue();

	if (ImGui::SliderInt("Overlay Y location", &bgY, 0, 100, "%d")) {
		bgYCvar.setValue(bgY);
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Y location of the in-game overlay");
	}

	ImGui::Separator();
	ImGui::LabelText("", "Plugin made by AJBinky");
	ImGui::LabelText("", "Original MVPR plugin by TeaBoneJones");
}