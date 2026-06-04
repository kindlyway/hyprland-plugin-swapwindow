#include <SharedDefs.hpp>
#include <helpers/Color.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/layout/LayoutManager.hpp>
#include <hyprland/src/managers/EventManager.hpp>
#include <hyprland/src/config/lua/LuaBindings.hpp>
#include <hyprland/src/config/lua/bindings/LuaBindingsInternal.hpp>
#include <hyprutils/string/String.hpp>
#include <hyprutils/string/VarList.hpp>
#include <lauxlib.h>
#include <lua.h>
#include <sstream>
#include <stdexcept>
#include <string>
inline HANDLE PHANDLE = nullptr;


APICALL EXPORT std::string PLUGIN_API_VERSION(){
        return HYPRLAND_API_VERSION;
}

static void luaCheckArgCount(lua_State* L, const char* fn, int min, int max){
        const int count = lua_gettop(L);
        if (count < min || count > max){
                if (min == max){
                        luaL_error(L, "%s: expected %d argument(s), got %d", fn, min, count);
                } else{
                        luaL_error(L, "%s: expected %d-%d argument(s), got %d", fn, min, max, count);
                }
        }
}
static std::string luaStringArg(lua_State* L, int idx, const char* fn, const char* name) {
	if (lua_isnoneornil(L, idx)) {
		luaL_error(L, "%s: missing required argument '%s'", fn, name);
		return {};
	}

	if (!lua_isstring(L, idx)) {
		luaL_error(L, "%s: argument '%s' must be a string", fn, name);
		return {};
	}

	return lua_tostring(L, idx);
}

static SDispatchResult swapWindows(const std::string& first, const std::string& second){
        const auto FIRSTWINDOW = g_pCompositor->getWindowByRegex(first);
        if (!FIRSTWINDOW)
                return {.success = false, .error = "First window not found"};
        if (FIRSTWINDOW->isFullscreen())
                return {.success = false, .error = "Cant swap fullscreen window"};
        const auto SECONDWINDOW = g_pCompositor->getWindowByRegex(second);
        if (!SECONDWINDOW || SECONDWINDOW == FIRSTWINDOW)
                return {.success = false, .error = "Invalid"};
        g_layoutManager->switchTargets(FIRSTWINDOW->layoutTarget(), SECONDWINDOW->layoutTarget(), true);
        g_pEventManager->postEvent(SHyprIPCEvent{"swapwindowv2"});
        return SDispatchResult{};
}

static SDispatchResult dispatch_swapwindowv2(std::string value){
        auto args = Hyprutils::String::CVarList(value);
        if (args.size() < 2)
                return{.success = false, .error = "Expected two window selectors"};
        return swapWindows(args[0], args[1]);
}


static int luaSwapWindow(lua_State* L){
        static constexpr const char* FN = "hl.plugin.swapwindow.swap";
        luaCheckArgCount(L, FN, 2, 2);
        auto first = luaStringArg(L, 1, FN, "first_window");
        auto second = luaStringArg(L, 2, FN, "second_window");
        if (first.empty() || second.empty())
                return luaL_error(L, "%s window selectors must not be empty", FN);
        auto dspSwapWindow = [](lua_State* L)->int{
                std::string first = lua_tostring(L, lua_upvalueindex(1));
                std::string second = lua_tostring(L, lua_upvalueindex(2));
                swapWindows(first, second);
                return 0;
        };
        lua_pushstring(L, first.c_str());
        lua_pushstring(L, second.c_str());

        lua_pushcclosure(L, dspSwapWindow, 2);
        return 1;
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
        success = success && HyprlandAPI::addDispatcherV2(PHANDLE, "swapwindowv2", ::dispatch_swapwindowv2);
        success = success && HyprlandAPI::addLuaFunction(PHANDLE, "swapwindow", "swap", luaSwapWindow);

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
