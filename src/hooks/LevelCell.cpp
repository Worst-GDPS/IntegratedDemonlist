#include "../IntegratedDemonlist.hpp"
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/utils/StringBuffer.hpp>
#include <jasmine/hook.hpp>
#include <jasmine/setting.hpp>

using namespace geode::prelude;

class $modify(IDLevelCell, LevelCell) {

    static void onModify(ModifyBase<ModifyDerive<IDLevelCell, LevelCell>>& self) {
        (void)self.setHookPriorityAfterPost("LevelCell::loadFromLevel", "hiimjustin000.level_size");
        jasmine::hook::modify(self.m_hooks, "LevelCell::loadFromLevel", "enable-rank");
    }

    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        auto platformer = level->isPlatformer();
        auto difficulty = level->m_demonDifficulty;
        if (level->m_levelType == GJLevelType::Editor || level->m_demon.value() <= 0 ||
            (!platformer && difficulty < 6) || (platformer && difficulty != 0 && difficulty < 5)) return;

        auto levelID = level->m_levelID.value();
        std::vector<int> positions;
        for (auto& demon : IntegratedDemonlist::demonlist) {
            if (demon.id == levelID) positions.push_back(demon.position);
        }
        if (!positions.empty()) return addRank(positions);
    }

    void addRank(const std::vector<int>& positions) {
        if (m_mainLayer->getChildByID("level-rank-label"_spr)) return;

        auto dailyLevel = m_level->m_dailyID.value() > 0;
        auto isWhite = dailyLevel || jasmine::setting::getValue<bool>("white-rank");

        StringBuffer positionsStr;
        for (auto it = positions.begin(); it != positions.end(); ++it) {
            if (it != positions.begin()) positionsStr.append('/');
            positionsStr.append("#{}", *it);
        }
        positionsStr.append(" Demonlist");

        auto rankTextNode = CCLabelBMFont::create(positionsStr.c_str(), "chatFont.fnt");
        rankTextNode->setPosition({ 346.0f, dailyLevel ? 6.0f : 1.0f });
        rankTextNode->setAnchorPoint({ 1.0f, 0.0f });
        rankTextNode->setScale(m_compactView ? 0.45f : 0.6f);
        auto rlc = Loader::get()->getLoadedMod("raydeeux.revisedlevelcells");
        if (rlc && rlc->getSettingValue<bool>("enabled") && rlc->getSettingValue<bool>("blendingText")) {
            rankTextNode->setBlendFunc({ GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA });
        }
        else if (isWhite) {
            rankTextNode->setOpacity(152);
        }
        else {
            rankTextNode->setColor({ 51, 51, 51 });
            rankTextNode->setOpacity(200);
        }
        rankTextNode->setID("level-rank-label"_spr);
        m_mainLayer->addChild(rankTextNode);

        if (auto levelSizeLabel = m_mainLayer->getChildByID("hiimjustin000.level_size/size-label")) {
            levelSizeLabel->setPosition({
                m_compactView ? 343.0f - rankTextNode->getScaledContentWidth() : 346.0f,
                m_compactView ? 1.0f : 12.0f
            });
        }
    }
};
