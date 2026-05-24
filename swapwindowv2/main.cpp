#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/layout/LayoutManager.hpp>
#include <hyprland/src/managers/EventManager.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
inline HANDLE PHANDLE = nullptr;


APICALL EXPORT std::string PLUGIN_API_VERSION(){
        return HYPRLAND_API_VERSION;
}

static SDispatchResult swapwindowv2(std::string in){
        std::stringstream ss(in);
        std::string first, second;
        if (std::getline(ss, first, ',') && std::getline(ss, second, ',')){
                const auto FIRSTWINDOW = g_pCompositor->getWindowByRegex(first);
                if (!FIRSTWINDOW){
                        return {.success = false, .error = "First window not found"};
                }
                if (FIRSTWINDOW->isFullscreen()){
                        return {.success = false, .error = "Can't swap fullscreen window"};
                }
                const auto SECONDWINDOW = g_pCompositor->getWindowByRegex(second);
                if (!SECONDWINDOW || SECONDWINDOW == FIRSTWINDOW){
                        return {.success = false, .error = "Invalid"};
                }
                g_layoutManager->switchTargets(FIRSTWINDOW->layoutTarget(), SECONDWINDOW->layoutTarget(), true);
                g_pEventManager->postEvent(SHyprIPCEvent{"swapwindowv2"});
        }
        else{
                return {.success = false, .error = "Invalid dispatcher: Comma is needed to seperate windows"};
        }
        return SDispatchResult{};
}


APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle){
        PHANDLE = handle;

        const std::string COMPOSITOR_HASH = __hyprland_api_get_hash();
        const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

        if (COMPOSITOR_HASH != CLIENT_HASH){
                HyprlandAPI::addNotification(PHANDLE, "[swapwindowv2] mismatched headers! Can't proceed", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
                throw std::runtime_error("[swapwindowv2] version mismatch");
        }

        bool success = true;
        success = success && HyprlandAPI::addDispatcherV2(PHANDLE, "plugin:swapwindowv2:swapwindowv2", ::swapwindowv2);

        if (success){
                HyprlandAPI::addNotification(PHANDLE, "[swapwindowv2] initialized successfully!", CHyprColor{0.2,1.0,0.2,1.0}, 5000);
        }
        else{
                HyprlandAPI::addNotification(PHANDLE, "[swapwindowv2] failure to register dispatchers", CHyprColor{1.0,0.2,0.2,1.0}, 5000);
                throw std::runtime_error("[swapwindowv2] dispatchers failed");
        }

        return {"swapwindowv2", "adds an extra dispatcher to swap windows", "kindlyway", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT(){
        ;
}
