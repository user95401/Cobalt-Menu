#include <shellapi.h>
#include <imgui-cocos.hpp>
#include <chrono>

#include <Geode/cocos/CCDirector.h>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCTransition.h>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;


bool isMenuShown = Mod::get()->getSettingValue<bool>("ShowOnStartup");
bool isDebugOn = Mod::get()->getSettingValue<bool>("isDebugOn");

// Speedhack
static float SpeedHackSpeed = 1.00f;
bool isSpeedhack = false;

// Cheats
bool isNoclip = false;

// Global

auto isCopyHack = Mod::get()->getSavedValue<bool>("isCopyHack");

auto isTransitionOffHack = Mod::get()->getSavedValue<bool>("isTransitionOffHack");

auto isEditHack = Mod::get()->getSavedValue<bool>("isEditHack");
auto isSliderHack = Mod::get()->getSavedValue<bool>("isSliderHack");

// Misc
int menuKey;
const char* keybindOptions[] = { "Tab", "F1", "F2", "F3", "F4", "F5"};
int selectedKeybindIndex = Mod::get()->getSavedValue<int>("menuKey");

cocos2d::enumKeyCodes PickedKey = KEY_Tab;

// Visual
auto isNotHide = Mod::get()->getSavedValue<bool>("isNotHide");
auto isNotDeathEffect = Mod::get()->getSavedValue<bool>("isNotDeathEffect");
auto isNotShake = Mod::get()->getSavedValue<bool>("isNotShake");


class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        if (down && (key == PickedKey)) {
            isMenuShown = !isMenuShown;
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};


class $modify(PlayLayerModification, PlayLayer) {


    void destroyPlayer(PlayerObject* p, GameObject* g) 
    {
        if (!isNoclip)
        {
            PlayLayer::destroyPlayer(p, g);
        }
    }


    void shakeCamera(float p0, float p1, float p2) {
        if (!isNotShake)
        {
            PlayLayer::shakeCamera(p0, p1, p2);
        }
    }
};

class $modify(PlayerObject) {
    void toggleVisibility(bool p0)
    {
        if (!isNotHide)
        {
            PlayerObject::toggleVisibility(p0);
        }
    }

    void playDeathEffect() 
    {
        if (!isNotDeathEffect)
        {
            PlayerObject::playDeathEffect();
        }
    }
};

// This is not the best way to do this
void caseMenuKeybind() {

    menuKey = Mod::get()->getSavedValue<int>("menuKey");

    switch(menuKey) {
    case 0:
        PickedKey = KEY_Tab;
        break;

    case 1:
        PickedKey = KEY_F1;
        break;

    case 2:
        PickedKey = KEY_F2;
        break;

    case 3:
        PickedKey = KEY_F3;
        break;

    case 4:
        PickedKey = KEY_F4;
        break;

    case 5:
        PickedKey = KEY_F5;
        break;

    // case 6: // FIX KEY_LeftShift and KEY_RightShift not working!
    //     PickedKey = KEY_LeftShift;
    //     break;

    // case 7:
    //     PickedKey = KEY_RightShift;
    //     break;

    default:
        // code block
        log::warn("menuKey not found, reverting to tab.");
        PickedKey = KEY_Tab;
        break;
    }
}

void PatchGame() {

    log::info("PatchGame Called");

    // Edit Hack. Patch from MHv5
    if (isEditHack) Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x1E4A32), {0x90, 0x90});
    else Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x1E4A32), {0x75, 0x6C});


    // Slider Bypass. Patch from MHv5
    if (isSliderHack) {
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x2E5CA), {0xEB});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x2E5F8), {0xEB});
    } else {
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x2E5CA), {0x76});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x2E5F8), {0x76});
    }

    // Copy Hack. Patch from MHv5
    if (isCopyHack) {
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x179B8E), {0x90, 0x90});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x176F5C), {0x8B, 0xCA, 0x90});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x176FE5), {0xB0, 0x01, 0x90});
    } else {
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x179B8E), {0x75, 0x0E});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x176F5C), {0x0F, 0x44, 0xCA});
        Mod::get()->patch(reinterpret_cast<void*>(base::get() + 0x176FE5), {0x0F, 0x95, 0xC0});
    }


}


$on_mod(Loaded) {

    PatchGame();

    caseMenuKeybind();

    ImGuiCocos::get().setup([&] {
       ImGuiStyle * style = &ImGui::GetStyle();

        // Style made with ImThemes
        style->Alpha = 1.0f;
        style->DisabledAlpha = 0.6000000238418579f;
        style->WindowPadding = ImVec2(15.0f, 15.0f);
        style->WindowRounding = 8.0f;
        style->WindowBorderSize = 0.0f;
        style->WindowMinSize = ImVec2(20.0f, 20.0f);
        style->WindowTitleAlign = ImVec2(0.0f, 0.5f);
        style->WindowMenuButtonPosition = ImGuiDir_None;
        style->ChildRounding = 10.19999980926514f;
        style->ChildBorderSize = 1.0f;
        style->PopupRounding = 7.5f;
        style->PopupBorderSize = 1.0f;
        style->FramePadding = ImVec2(11.10000038146973f, 3.0f);
        style->FrameRounding = 8.100000381469727f;
        style->FrameBorderSize = 0.0f;
        style->ItemSpacing = ImVec2(4.099999904632568f, 4.0f);
        style->ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style->CellPadding = ImVec2(4.0f, 2.0f);
        style->IndentSpacing = 21.0f;
        style->ColumnsMinSpacing = 6.0f;
        style->ScrollbarSize = 13.39999961853027f;
        style->ScrollbarRounding = 20.0f;
        style->GrabMinSize = 9.899999618530273f;
        style->GrabRounding = 20.0f;
        style->TabRounding = 8.699999809265137f;
        style->TabBorderSize = 0.0f;
        style->TabMinWidthForCloseButton = 0.0f;
        style->ColorButtonPosition = ImGuiDir_Right;
        style->ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style->SelectableTextAlign = ImVec2(0.0f, 0.0f);
        
        style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.5064366459846497f, 0.5064327120780945f, 0.5064377784729004f, 1.0f);
        style->Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
        style->Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style->Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
        style->Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
        style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style->Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.47843137383461f, 0.3568627536296844f, 0.5411764979362488f);
        style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1647479087114334f, 0.7381974458694458f, 0.7237408757209778f, 0.4000000059604645f);
        style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2119213938713074f, 0.7001098990440369f, 0.8369098901748657f, 0.6705882549285889f);
        style->Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
        style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1568627655506134f, 0.47843137383461f, 0.424606591463089f, 1.0f);
        style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
        style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2654313147068024f, 0.4721029996871948f, 0.3683236241340637f, 0.08154505491256714f);
        style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.999899930335232e-07f, 0.3476395010948181f);
        style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3393689095973969f, 0.8068669438362122f, 0.4989117085933685f, 0.54935622215271f);
        style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1377445012331009f, 0.4377682209014893f, 0.1240030005574226f, 1.0f);
        style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9999980330467224f, 1.0f, 0.9999899864196777f, 0.5665236115455627f);
        style->Colors[ImGuiCol_CheckMark] = ImVec4(0.9999991655349731f, 1.0f, 0.9999899864196777f, 1.0f);
        style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.9999983906745911f, 1.0f, 0.9999899864196777f, 1.0f);
        style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(9.999899930335232e-07f, 9.999999974752427e-07f, 9.999974963648128e-07f, 1.0f);
        style->Colors[ImGuiCol_Button] = ImVec4(0.1375416666269302f, 0.5622317790985107f, 0.4543187916278839f, 0.6392157077789307f);
        style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1647058874368668f, 0.7372549176216125f, 0.7254902124404907f, 0.6824034452438354f);
        style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.1773471683263779f, 0.7124463319778442f, 0.691777229309082f, 0.9656652212142944f);
        style->Colors[ImGuiCol_Header] = ImVec4(0.7081544995307922f, 0.8985635042190552f, 1.0f, 0.274678111076355f);
        style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6566523313522339f, 0.9248467087745667f, 1.0f, 0.3819742202758789f);
        style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.6094420552253723f, 1.0f, 0.803882896900177f, 0.5965665578842163f);
        style->Colors[ImGuiCol_Separator] = ImVec4(0.4385418891906738f, 0.4385488033294678f, 0.54935622215271f, 0.5f);
        style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4282635748386383f, 0.531344473361969f, 0.6437768340110779f, 0.7799999713897705f);
        style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4804288148880005f, 0.5780996084213257f, 0.6909871101379395f, 1.0f);
        style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.5312494039535522f, 0.6636775732040405f, 0.8197425007820129f, 0.2000000029802322f);
        style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
        style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
        style->Colors[ImGuiCol_Tab] = ImVec4(0.110519640147686f, 0.6437768340110779f, 0.3210758566856384f, 0.8619999885559082f);
        style->Colors[ImGuiCol_TabHovered] = ImVec4(0.2249074280261993f, 0.3175965547561646f, 0.2424695938825607f, 0.8111587762832642f);
        style->Colors[ImGuiCol_TabActive] = ImVec4(0.1953987032175064f, 0.4987000524997711f, 0.5836910009384155f, 1.0f);
        style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
        style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
        style->Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
        style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5763046741485596f, 1.0f, 0.545064389705658f, 1.0f);
        style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.5075061321258545f, 0.8263199925422668f, 0.9613733887672424f, 1.0f);
        style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 1.0f, 0.5236051082611084f, 1.0f);
        style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
        style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
        style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
        style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style->Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
        style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
        style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
        style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
        style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
        style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

    }).draw([&] {


        if (isMenuShown) {
            ImGui::Begin("Misc");
            
            if (ImGui::Button("AppData")) {
                ShellExecuteA(NULL, "open", CCFileUtils::get()->getWritablePath().c_str(), NULL, NULL, SW_SHOWDEFAULT);
            }

            if (ImGui::Button("Settings")) {
                OptionsLayer::addToCurrentScene(false);
            }


            if (ImGui::Combo("Menu Keybind", &selectedKeybindIndex, keybindOptions, IM_ARRAYSIZE(keybindOptions))) {
                Mod::get()->setSavedValue<int>("menuKey", selectedKeybindIndex); // Could Be a better way to do this but it sucks
                caseMenuKeybind();
            }
            // TODO: mod options

            ImGui::End();


            // Speedhack
            ImGui::Begin("Speedhack");

            ImGui::Checkbox("Enable Speedhack", &isSpeedhack);

            if (isSpeedhack) 
            {
                CCDirector::sharedDirector()->getScheduler()->setTimeScale(SpeedHackSpeed);
            }
            else 
            {
                CCDirector::sharedDirector()->getScheduler()->setTimeScale(1.0f);
            }


            ImGui::DragFloat("Speed", &SpeedHackSpeed, 0.1f, 0.0f, 5.0f);

            ImGui::End();


            // Cheats and more stuff
            ImGui::Begin("Cheats");
            
            ImGui::Checkbox("Noclip", &isNoclip);

            ImGui::End();


            // Global
            ImGui::Begin("Global");

            if (ImGui::Checkbox("Copy Hack", &isCopyHack)) {
                Mod::get()->setSavedValue<bool>("isCopyHack", isCopyHack);
                PatchGame();
            }

            // TODO: this            
            // if (ImGui::Checkbox("No Transitions", &isTransitionOffHack)) {
            //     Mod::get()->setSavedValue<bool>("isTransitionOffHack", isTransitionOffHack);
            // }

            if (ImGui::Checkbox("Edit Hack", &isEditHack)) {
                Mod::get()->setSavedValue<bool>("isEditHack", isEditHack);
                PatchGame();
            }

            if (ImGui::Checkbox("Slider Bypass", &isSliderHack)) {
                Mod::get()->setSavedValue<bool>("isSliderHack", isSliderHack);
                PatchGame();
            }


            ImGui::End();


            // Visual Stuff
            ImGui::Begin("Visual");
            
            if (ImGui::Checkbox("No Camera Shake", &isNotShake)) {
                Mod::get()->setSavedValue<bool>("isNotShake", isNotShake);
            }

            if (ImGui::Checkbox("No Death Effect", &isNotDeathEffect)) {
                Mod::get()->setSavedValue<bool>("isNotDeathEffect", isNotDeathEffect);
            }

            ImGui::Checkbox("No Hide Player", &isNotHide);

            ImGui::End();



            // DEBUG STUFF
            if (isDebugOn) {
                ImGui::Begin("DEBUG");
                
                ImGui::Text("Selected Keybind Index: %d", selectedKeybindIndex);

                ImGui::Separator();

                ImGui::Text("Saved Values:");

                ImGui::Text("menuKey: %d", Mod::get()->getSavedValue<int>("menuKey"));

                ImGui::Text("Speed: %f", SpeedHackSpeed);

                ImGui::Text("isTransitionOffHack: %s", isTransitionOffHack ? "True" : "False");

                ImGui::End();
            }

        }
    });
}



