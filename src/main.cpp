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

    if (isNoclipEnabled) {
        Notification::create("Noclip ON", NotificationIcon::Success)->show();
    } else {
        Notification::create("Noclip OFF", NotificationIcon::Error)->show();
    }

    log::info("Noclip: {}", isNoclipEnabled ? "ON" : "OFF");
}

// =========================================================
// PANEL YANG BISA DI-DRAG, DIBUKA, DITUTUP
// =========================================================

class HoshinoPanel : public CCLayer {
protected:
    bool m_dragging = false;
    CCPoint m_touchOffset;
    CCPoint m_panelSize = {220.f, 160.f};

public:
    static HoshinoPanel* create() {
        auto ret = new HoshinoPanel();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Background panel
        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize(m_panelSize);
        bg->setPosition({m_panelSize.x / 2, m_panelSize.y / 2});
        bg->setID("hoshino-panel-bg"_spr);
        this->addChild(bg, 0);

        // Judul
        auto title = CCLabelBMFont::create("Hoshino Panel", "bigFont.fnt");
        title->setScale(0.5f);
        title->setPosition({m_panelSize.x / 2, m_panelSize.y - 15.f});
        this->addChild(title, 1);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        this->addChild(menu, 2);

        // Tombol close (X) di pojok kanan atas
        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeSprite, this, menu_selector(HoshinoPanel::onClose)
        );
        closeBtn->setPosition({m_panelSize.x - 15.f, m_panelSize.y - 15.f});
        menu->addChild(closeBtn);

        // Tombol toggle noclip di tengah panel
        auto noclipSprite = CircleButtonSprite::create(
            CCSprite::create("hoshino-button.png"_spr),
            CircleBaseColor::Pink,
            CircleBaseSize::Medium
        );
        auto noclipBtn = CCMenuItemSpriteExtra::create(
            noclipSprite, this, menu_selector(HoshinoPanel::onToggleNoclip)
        );
        noclipBtn->setPosition({m_panelSize.x / 2, m_panelSize.y / 2 - 10.f});
        menu->addChild(noclipBtn);

        this->setContentSize(m_panelSize);
        this->ignoreAnchorPointForPosition(false);
        this->setPosition({
            winSize.width / 2 - m_panelSize.x / 2,
            winSize.height / 2 - m_panelSize.y / 2
        });

        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);

        return true;
    }

    // Cuma bagian atas (header, 30px teratas) yang bisa dipakai buat drag,
    // biar nggak nabrak sama tombol-tombol di dalam panel.
    bool isInHeader(CCPoint local) {
        return local.x >= 0 && local.x <= m_panelSize.x
            && local.y >= m_panelSize.y - 30.f && local.y <= m_panelSize.y;
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        auto local = this->convertTouchToNodeSpace(touch);
        if (isInHeader(local)) {
            m_dragging = true;
            m_touchOffset = ccpSub(this->getPosition(), touch->getLocation());
            return true;
        }
        return false;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (m_dragging) {
            this->setPosition(ccpAdd(touch->getLocation(), m_touchOffset));
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        m_dragging = false;
    }

    void ccTouchCancelled(CCTouch* touch, CCEvent* event) {
        m_dragging = false;
    }

    void onClose(CCObject* sender) {
        this->removeFromParentAndCleanup(true);
    }

    void onToggleNoclip(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// HOOK MENU LAYER - TOMBOL HOSHINO BUKA PANEL
// =========================================================

class $modify(TestButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto sprite = CircleButtonSprite::create(
            CCSprite::create("hoshino-button.png"_spr),
            CircleBaseColor::Pink,
            CircleBaseSize::Medium
        );

        auto myButton = CCMenuItemSpriteExtra::create(
            sprite,
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
        // Jangan bikin panel baru kalau udah ada yang kebuka
        if (this->getChildByID("hoshino-panel"_spr)) {
            return;
        }
        auto panel = HoshinoPanel::create();
        panel->setID("hoshino-panel"_spr);
        this->addChild(panel, 1000);
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
