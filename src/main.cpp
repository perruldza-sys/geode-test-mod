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
static bool showIndicator = true;

// =========================================================
// INDICATOR LABEL
// =========================================================

class NoclipIndicator : public CCLabelBMFont {
public:
    static NoclipIndicator* create() {
        auto label = CCLabelBMFont::create("NOCLIP: OFF", "bigFont.fnt");
        if (label) {
            label->setColor(ccc3(255, 0, 0));
            label->setScale(0.7f);
            label->setZOrder(999);
            label->setVisible(false);
        }
        return static_cast<NoclipIndicator*>(label);
    }
};

// =========================================================
// TOGGLE FUNCTION
// =========================================================

void toggleNoclip() {
    isNoclipEnabled = !isNoclipEnabled;
    
    // Cari indicator di PlayLayer
    auto playLayer = PlayLayer::get();
    if (playLayer) {
        auto indicator = playLayer->getChildByID("noclip-indicator"_spr);
        if (indicator) {
            auto label = static_cast<CCLabelBMFont*>(indicator);
            label->setVisible(isNoclipEnabled && showIndicator);
            label->setString(isNoclipEnabled ? "NOCLIP: ON" : "NOCLIP: OFF");
            label->setColor(isNoclipEnabled ? ccc3(0, 255, 0) : ccc3(255, 0, 0));
        }
    }

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

        // ===== TOMBOL NOCLIP =====
        auto sprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName("GJ_noclipBtn_001.png"),
            CircleBaseColor::Cyan,
            CircleBaseSize::Medium
        );

        // Kalo sprite gak ada, pake teks
        if (!sprite) {
            sprite = CircleButtonSprite::create(
                CCLabelBMFont::create("NC", "bigFont.fnt"),
                CircleBaseColor::Cyan,
                CircleBaseSize::Medium
            );
        }

        auto myButton = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(NoclipMenuLayer::onNoclipButton)
        );

        // Taruh di bottom menu
        if (auto bottomMenu = this->getChildByID("bottom-menu")) {
            myButton->setID("noclip-button"_spr);
            bottomMenu->addChild(myButton);
            bottomMenu->updateLayout();
        } else {
            // Fallback
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto fallbackMenu = CCMenu::create();
            fallbackMenu->setID("noclip-menu"_spr);
            fallbackMenu->addChild(myButton);
            fallbackMenu->setPosition({winSize.width - 30.f, winSize.height - 30.f});
            this->addChild(fallbackMenu);
        }

        return true;
    }

    void onNoclipButton(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// HOOK PLAY LAYER - INDICATOR & KEYBIND
// =========================================================

class $modify(NoclipPlayLayer, PlayLayer) {
    NoclipIndicator* m_indicator = nullptr;

    void onEnter() {
        PlayLayer::onEnter();

        // Buat indicator
        if (!m_indicator) {
            m_indicator = NoclipIndicator::create();
            if (m_indicator) {
                auto winSize = CCDirector::sharedDirector()->getWinSize();
                m_indicator->setPosition(ccp(winSize.width / 2, winSize.height - 50));
                m_indicator->setID("noclip-indicator"_spr);
                m_indicator->setVisible(false);
                this->addChild(m_indicator);
            }
        }

        // Cek setting on-start
        bool onStart = Mod::get()->getSettingValue<bool>("noclip-on-start");
        if (onStart && !isNoclipEnabled) {
            toggleNoclip();
        }
    }

    // Hook tombol keyboard
    void keyDown(cocos2d::enumKeyCodes key) {
        // N = toggle
        if (key == cocos2d::KEY_N) {
            toggleNoclip();
            return;
        }
        
        // Bisa juga pake key dari setting
        PlayLayer::keyDown(key);
    }

    // Hook death prevention
    void playDeathEffect() {
        if (isNoclipEnabled) {
            log::info("Noclip prevented death!");
            // Skip death
            return;
        }
        PlayLayer::playDeathEffect();
    }

    void onQuit() {
        // Matikan noclip saat keluar level
        if (isNoclipEnabled) {
            isNoclipEnabled = false;
            if (m_indicator) {
                m_indicator->setVisible(false);
            }
        }
        PlayLayer::onQuit();
    }
};

// =========================================================
// HOOK PLAYER OBJECT - BYPASS COLLISION
// =========================================================

class $modify(NoclipPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        if (isNoclipEnabled) {
            // Skip collision
            return;
        }
        PlayerObject::pushButton(button);
    }

    void releaseButton(PlayerButton button) {
        if (isNoclipEnabled) {
            return;
        }
        PlayerObject::releaseButton(button);
    }

    bool canBeCollidedWith(PlayerObject* other) {
        if (isNoclipEnabled) {
            return false;
        }
        return PlayerObject::canBeCollidedWith(other);
    }
};
