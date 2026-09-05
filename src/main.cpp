#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

class $modify(TestButtonMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        // Bikin sprite tombolnya pakai gambar custom Hoshino Ai.
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

        // Taruh di menu tombol yang sudah ada, biar otomatis kelola layout-nya
        if (auto bottomMenu = this->getChildByID("bottom-menu")) {
            myButton->setID("test-button"_spr);
            bottomMenu->addChild(myButton);
            bottomMenu->updateLayout();
        } else {
            // Fallback: taruh manual di pojok kanan atas kalau menu-nya nggak ketemu
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
        FLAlertLayer::create(
            "Tes Berhasil!",
            "Tombolnya jalan, mod-nya juga jalan. Cuma tes doang~",
            "OK"
        )->show();
    }
};
