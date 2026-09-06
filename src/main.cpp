#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

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

    if (isNoclipEnabled) {
        Notification::create("Noclip ON", NotificationIcon::Success)->show();
    } else {
        Notification::create("Noclip OFF", NotificationIcon::Error)->show();
    }

    log::info("Noclip: {}", isNoclipEnabled ? "ON" : "OFF");
}

// =========================================================
// HELPER: bikin tombol Hoshino (dipakai ulang di MenuLayer & PauseLayer)
// =========================================================

CCMenuItemSpriteExtra* createHoshinoButton(CCObject* target, SEL_MenuHandler selector) {
    auto sprite = CircleButtonSprite::create(
        CCSprite::create("hoshino-button.png"_spr),
        CircleBaseColor::Pink,
        CircleBaseSize::Medium
    );

    return CCMenuItemSpriteExtra::create(sprite, target, selector);
}

// =========================================================
// HOOK MENU LAYER - TOMBOL HOSHINO DI MENU UTAMA
// =========================================================

class $modify(TestButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto myButton = createHoshinoButton(
            this,
            menu_selector(TestButtonMenuLayer::onTestButton)
        );

        if (auto bottomMenu = this->getChildByID("bottom-menu")) {
            myButton->setID("test-button"_spr);
            bottomMenu->addChild(myButton);
            bottomMenu->updateLayout();
        } else {
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto fallbackMenu = CCMenu::create();
            fallbackMenu->setID("test-button-menu"_spr);
            fallbackMenu->addChild(myButton);
            fallbackMenu->setPosition({winSize.width - 30.f, winSize.height - 30.f});
            this->addChild(fallbackMenu);
        }

        return true;
    }

    void onTestButton(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// HOOK PAUSE LAYER - TOMBOL HOSHINO YANG SAMA DI MENU PAUSE
// =========================================================

class $modify(TestButtonPauseLayer, PauseLayer) {
    bool init(bool p0) {
        if (!PauseLayer::init(p0)) {
            return false;
        }

        auto myButton = createHoshinoButton(
            this,
            menu_selector(TestButtonPauseLayer::onTestButton)
        );

        if (auto rightMenu = this->getChildByID("right-button-menu")) {
            myButton->setID("test-button"_spr);
            rightMenu->addChild(myButton);
            rightMenu->updateLayout();
        } else {
            // Fallback: taruh manual kalau ID container-nya beda di versi ini
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto fallbackMenu = CCMenu::create();
            fallbackMenu->setID("test-button-menu"_spr);
            fallbackMenu->addChild(myButton);
            fallbackMenu->setPosition({winSize.width - 30.f, winSize.height - 30.f});
            this->addChild(fallbackMenu);
        }

        return true;
    }

    void onTestButton(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// HOOK PLAY LAYER - KEYBIND & DEATH PREVENTION
// =========================================================

class $modify(NoclipPlayLayer, PlayLayer) {
    void onEnter() {
        PlayLayer::onEnter();

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
