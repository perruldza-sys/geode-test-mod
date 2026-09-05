#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(TestButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        // Bikin sprite tombolnya. Kalau resources/button.png ada, dia bakal
        // dipakai. Kalau nggak, fallback ke sprite bawaan GD biar tetap jalan.
        auto sprite = CircleButtonSprite::create(
            CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
            CircleBaseColor::Green,
            CircleBaseSize::Medium
        );

        auto myButton = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(TestButtonMenuLayer::onTestButton)
        );

        // Taruh di menu tombol yang sudah ada, biar otomatis kelola layout-nya
        if (auto menu = this->getChildByID("bottom-menu")) {
            myButton->setID("test-button"_spr);
            menu->addChild(myButton);
            menu->updateLayout();
        } else {
            // Fallback: taruh manual di pojok kanan atas kalau menu-nya nggak ketemu
            auto winSize = CCDirector::sharedDirector()->getWinSize();
            auto menu = CCMenu::create();
            menu->setID("test-button-menu"_spr);
            menu->addChild(myButton);
            menu->setPosition({winSize.width - 30.f, winSize.height - 30.f});
            this->addChild(menu);
        }

        return true;
    }

    void onTestButton(CCObject* sender) {
        FLAlertLayer::create(
            "Tes Berhasil!",
            "Tombolnya jalan, mod-nya juga jalan. Cuma tes doang~",
            "OK"
        )->show();
    }
};
