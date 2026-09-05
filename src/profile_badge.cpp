#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

class $modify(HoshinoProfilePage, ProfilePage) {
    // GD sendiri sudah kasih tau lewat parameter `own` apakah profil yang
    // dibuka itu punya kita sendiri atau punya orang lain.
    bool init(int accountID, bool own) {
        if (!ProfilePage::init(accountID, own)) {
            return false;
        }

        // Cuma tampilin badge kalau ini profil kita sendiri.
        if (own) {
            auto winSize = CCDirector::sharedDirector()->getWinSize();

            auto badge = CCSprite::create("hoshino-button.png"_spr);
            if (badge) {
                badge->setScale(0.35f);
                badge->setID("hoshino-badge"_spr);
                badge->setPosition({winSize.width - 35.f, winSize.height - 35.f});
                this->addChild(badge, 100);
            }
        }

        return true;
    }
};
