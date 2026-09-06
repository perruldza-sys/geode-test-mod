#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// =========================================================
// STATE
// =========================================================

static bool isNoclipEnabled = false;

// =========================================================
// TOGGLE FUNCTION
// =========================================================

void toggleNoclip() {
    isNoclipEnabled = !isNoclipEnabled;

    // Notifikasi
    if (isNoclipEnabled) {
        Notification::create("Noclip ON", NotificationIcon::Success)->show();
    } else {
        Notification::create("Noclip OFF", NotificationIcon::Error)->show();
    }

    log::info("Noclip: {}", isNoclipEnabled ? "ON" : "OFF");
}

// =========================================================
// HOOK MENU LAYER - TOMBOL NOCLIP
// =========================================================

class $modify(NoclipMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        // Tombol Noclip
        auto sprite = CircleButtonSprite::create(
            CCLabelBMFont::create("NC", "bigFont.fnt"),
            CircleBaseColor::Cyan,
            CircleBaseSize::Medium
        );

        auto myButton = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(NoclipMenuLayer::onNoclipButton)
        );

        if (auto bottomMenu = this->getChildByID("bottom-menu")) {
            myButton->setID("noclip-button"_spr);
            bottomMenu->addChild(myButton);
            bottomMenu->updateLayout();
        }

        return true;
    }

    void onNoclipButton(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// HOOK PLAY LAYER - KEYBIND & DEATH PREVENTION
// =========================================================

class $modify(NoclipPlayLayer, PlayLayer) {
    void onEnter() {
        PlayLayer::onEnter();

        // Auto enable (opsional, butuh setting "noclip-on-start" di mod.json)
        if (Mod::get()->getSettingValue<bool>("noclip-on-start")) {
            if (!isNoclipEnabled) toggleNoclip();
        }
    }

    void keyDown(cocos2d::enumKeyCodes key, double dt) {
    if (key == cocos2d::KEY_N) {
        toggleNoclip();
        return;
    }
    PlayLayer::keyDown(key, dt);
    }

    // Ini fungsi yang beneran dipanggil pas player nabrak hazard.
    // Kalau noclip aktif, kita skip pemanggilan aslinya -> nggak jadi mati.
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (isNoclipEnabled) {
            log::info("Noclip prevented death!");
            return;
        }
        PlayLayer::destroyPlayer(player, object);
    }

    void onQuit() {
        if (isNoclipEnabled) {
            isNoclipEnabled = false;
        }
        PlayLayer::onQuit();
    }
};
