#include <Geode/utils/web.hpp>

struct IDListDemon {
    int id = 0;
    int position = 0;
    bool legacy = false;
    std::string name;
    std::string creator;
    std::string verifier;
    std::string ytlink;
    std::string mainid;

    bool operator==(const IDListDemon& other) const {
        return id == other.id && position == other.position;
    }
};

namespace IntegratedDemonlist {
    extern std::vector<IDListDemon> demonlist;
    extern bool demonlistLoaded;

    void loadDemonlist(geode::async::TaskHolder<geode::utils::web::WebResponse>&, geode::Function<void()>, geode::CopyableFunction<void(int)>);
}
