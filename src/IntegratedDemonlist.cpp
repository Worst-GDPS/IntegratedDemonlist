#include "IntegratedDemonlist.hpp"
#include <charconv>
#include <jasmine/web.hpp>

using namespace geode::prelude;

std::vector<IDListDemon> IntegratedDemonlist::demonlist;
bool IntegratedDemonlist::demonlistLoaded = false;

void IntegratedDemonlist::loadDemonlist(TaskHolder<web::WebResponse>& listener, Function<void()> success, CopyableFunction<void(int)> failure) {
    listener.spawn(
        web::WebRequest().get("https://146.59.93.5/demonlist/levelapi.php"),
        [failure = std::move(failure), success = std::move(success)](web::WebResponse res) mutable {
            if (!res.ok()) return failure(res.code());

            demonlistLoaded = true;
            demonlist.clear();

            for (auto& item : jasmine::web::getArray(res)) {
                int levelID = 0;
                auto worstID = item.get<int>("worstid");
                if (worstID.isOk()) {
                    levelID = worstID.unwrap();
                }
                else if (auto worstIDStr = item.get<std::string>("worstid"); worstIDStr.isOk()) {
                    const auto& value = worstIDStr.unwrap();
                    auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), levelID);
                    if (ec != std::errc()) continue;
                }
                else {
                    continue;
                }

                auto position = item.get<int>("position");
                if (!position.isOk()) continue;

                auto name = item.get<std::string>("lvlname");
                if (!name.isOk()) continue;

                auto creator = item.get<std::string>("creator");
                auto ytlink = item.get<std::string>("ytlink");
                auto verifier = item.get<std::string>("verifier");
                auto mainid = item.get<std::string>("mainid");
                auto legacy = item.get<bool>("legacy");

                IDListDemon demon(levelID, position.unwrap(), std::move(name).unwrap());
                demon.creator = creator.isOk() ? std::move(creator).unwrap() : std::string();
                demon.ytlink = ytlink.isOk() ? std::move(ytlink).unwrap() : std::string();
                demon.verifier = verifier.isOk() ? std::move(verifier).unwrap() : std::string();
                demon.mainid = mainid.isOk() ? std::move(mainid).unwrap() : std::string();
                demon.legacy = legacy.isOk() && legacy.unwrap();

                if (demon.legacy) continue;

                demonlist.insert(std::ranges::upper_bound(demonlist, demon, [](const IDListDemon& a, const IDListDemon& b) {
                    return a.position < b.position;
                }), std::move(demon));
            }

            success();
        }
    );
}
