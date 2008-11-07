#include "player.hpp"
#include "serialization.hpp"

namespace battlemints {

thing * player::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);

    return new player(center);
}

}
