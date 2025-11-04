#include "HardcodedDescAdapter.h"
#include <sstream>

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Description {

                std::string HardcodedDescAdapter::generateDescription(const TuiRogGame::Domain::Model::Position& player_position) {
                    std::stringstream ss;
                    ss << "You are at (" << player_position.x << ", " << player_position.y << ").";

                    if (player_position.x == 0 && player_position.y == 0) {
                        ss << " This is the starting point.";
                    } else if (player_position.x == 5 && player_position.y == 5) {
                        ss << " A mysterious aura emanates from here.";
                    } else if (player_position.x > 0 && player_position.y > 0) {
                        ss << " The air is fresh here.";
                    } else {
                        ss << " The path ahead is unknown.";
                    }
                    return ss.str();
                }

            } // namespace Description
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame
