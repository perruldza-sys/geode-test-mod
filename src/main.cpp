#include <Geode/Geode.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// =========================================================
// STATE
// =========================================================

static bool isNoclipEnabled = false;

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
// PANEL (dibuka/tutup lewat icon floating)
// =========================================================

class HoshinoPanel : public CCLayer {
protected:
    CCPoint m_panelSize = {220.f, 160.f};
    bool m_dragging = false;
    CCPoint m_touchOffset;

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

        auto bg = CCScale9Sprite::create("GJ_square01.png");
        bg->setContentSize(m_panelSize);
        bg->setPosition({m_panelSize.x / 2, m_panelSize.y / 2});
        this->addChild(bg, 0);

        auto title = CCLabelBMFont::create("Hoshino Panel", "bigFont.fnt");
        title->setScale(0.5f);
        title->setPosition({m_panelSize.x / 2, m_panelSize.y - 15.f});
        this->addChild(title, 1);

        auto menu = CCMenu::create();
        menu->setPosition({0.f, 0.f});
        this->addChild(menu, 2);

        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeSprite, this, menu_selector(HoshinoPanel::onClose)
        );
        closeBtn->setPosition({m_panelSize.x - 15.f, m_panelSize.y - 15.f});
        menu->addChild(closeBtn);

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

    // Panel juga bisa di-drag lewat header-nya (30px teratas)
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

    void ccTouchEnded(CCTouch* touch, CCEvent* event) { m_dragging = false; }
    void ccTouchCancelled(CCTouch* touch, CCEvent* event) { m_dragging = false; }

    void onClose(CCObject* sender) {
        this->removeFromParentAndCleanup(true);
    }

    void onToggleNoclip(CCObject* sender) {
        toggleNoclip();
    }
};

// =========================================================
// FLOATING OVERLAY - ICON HOSHINO YANG GLOBAL & BISA DI-DRAG
// =========================================================

class HoshinoOverlay : public CCLayer {
protected:
    CCSprite* m_icon = nullptr;
    HoshinoPanel* m_panel = nullptr;

    bool m_dragging = false;
    CCPoint m_touchStartLocal;
    CCPoint m_iconStartPos;

    static constexpr float kDragThreshold = 10.f;

public:
    static HoshinoOverlay* create() {
        auto ret = new HoshinoOverlay();
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

        // Posisi default: pojok kanan bawah. Kalau ada posisi tersimpan, pakai itu.
        float defaultX = winSize.width - 40.f;
        float defaultY = 40.f;
        float posX = Mod::get()->getSavedValue<float>("hoshino-pos-x", defaultX);
        float posY = Mod::get()->getSavedValue<float>("hoshino-pos-y", defaultY);

        m_icon = CCSprite::create("hoshino-button.png"_spr);
        m_icon->setScale(0.5f);
        m_icon->setPosition({posX, posY});
        this->addChild(m_icon, 100);

        this->setTouchEnabled(true);
        this->setTouchMode(kCCTouchesOneByOne);

        return true;
    }

    // Prioritas tinggi biar icon selalu duluan nangkep sentuhan
    void registerWithTouchDispatcher() {
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1000, true);
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) {
        auto local = this->convertTouchToNodeSpace(touch);
        auto box = m_icon->boundingBox();

        if (box.containsPoint(local)) {
            m_dragging = true;
            m_touchStartLocal = local;
            m_iconStartPos = m_icon->getPosition();
            return true; // tangkep sentuhannya, jangan diteruskan ke tombol di belakang
        }
        return false; // biarin lewat kalau nggak kena icon
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) {
        if (!m_dragging) return;

        auto local = this->convertTouchToNodeSpace(touch);
        auto delta = ccpSub(local, m_touchStartLocal);
        auto newPos = ccpAdd(m_iconStartPos, delta);

        // Batasin biar nggak keluar layar
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto contentSize = m_icon->getContentSize();
        float halfW = (contentSize.width * m_icon->getScale()) / 2.f;
        float halfH = (contentSize.height * m_icon->getScale()) / 2.f;

        newPos.x = clampf(newPos.x, halfW, winSize.width - halfW);
        newPos.y = clampf(newPos.y, halfH, winSize.height - halfH);

        m_icon->setPosition(newPos);

        if (m_panel) {
            m_panel->setPosition(newPos);
        }
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) {
        if (!m_dragging) return;
        m_dragging = false;

        auto local = this->convertTouchToNodeSpace(touch);
        float dist = ccpDistance(local, m_touchStartLocal);

        if (dist < kDragThreshold) {
            // Ini tap, bukan drag -> buka/tutup panel
            togglePanel();
        } else {
            // Ini drag -> simpan posisi baru
            auto pos = m_icon->getPosition();
            Mod::get()->setSavedValue("hoshino-pos-x", pos.x);
            Mod::get()->setSavedValue("hoshino-pos-y", pos.y);
        }
    }

    void ccTouchCancelled(CCTouch* touch, CCEvent* event) {
        m_dragging = false;
    }

    void togglePanel() {
        if (m_panel) {
            m_panel->removeFromParentAndCleanup(true);
            m_panel = nullptr;
            return;
        }

        m_panel = HoshinoPanel::create();
        this->addChild(m_panel, 50);
    }
};

// =========================================================
// PASANG OVERLAY SEKALI PAS GAME START (GLOBAL, SATU INSTANCE)
// =========================================================

class $modify(HoshinoAppDelegate, AppDelegate) {
    bool applicationDidFinishLaunching() {
        auto ret = AppDelegate::applicationDidFinishLaunching();

        auto overlay = HoshinoOverlay::create();
        overlay->retain();

        // Ini yang bikin overlay-nya tetap dirender di atas scene apapun,
        // dan nggak ikut kehapus pas game pindah scene (Home <-> PlayLayer <-> dst).
        CCDirector::sharedDirector()->setNotificationNode(overlay);

        return ret;
    }
};

// =========================================================
// HOOK PLAY LAYER - KEYBIND & DEATH PREVENTION (tidak berubah)
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
