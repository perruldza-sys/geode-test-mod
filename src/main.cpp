#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

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
        Notification::create("🟢 Noclip ON", NotificationIcon::Success)->show();
    } else {
        Notification::create("🔴 Noclip OFF", NotificationIcon::Error)->show();
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
        
        // Auto enable (opsional)
        if (Mod::get()->getSettingValue<bool>("noclip-on-start")) {
            if (!isNoclipEnabled) toggleNoclip();
        }
    }

    void keyDown(cocos2d::enumKeyCodes key) {
        if (key == cocos2d::KEY_N) {
            toggleNoclip();
            return;
        }
        PlayLayer::keyDown(key);
    }

    void playDeathEffect() {
        if (isNoclipEnabled) {
            log::info("Noclip prevented death!");
            return;
        }
        PlayLayer::playDeathEffect();
    }

    void onQuit() {
        if (isNoclipEnabled) {
            isNoclipEnabled = false;
        }
        PlayLayer::onQuit();
    }
};

// =========================================================
// HOOK PLAYER OBJECT - BYPASS COLLISION
// =========================================================

class $modify(NoclipPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        if (isNoclipEnabled) return;
        PlayerObject::pushButton(button);
    }

    void releaseButton(PlayerButton button) {
        if (isNoclipEnabled) return;
        PlayerObject::releaseButton(button);
    }

    bool canBeCollidedWith(PlayerObject* other) {
        if (isNoclipEnabled) return false;
        return PlayerObject::canBeCollidedWith(other);
    }
};
