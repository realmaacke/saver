#include "Project/Describe.hpp"

#include <sstream>
 
std::string Describe::serialize() const {
    std::ostringstream oss;
    oss << tree_hash << '\n';
    oss << (parent_hash ? *parent_hash : "-") << '\n';
    oss << std::chrono::duration_cast<std::chrono::seconds>(timestamp.time_since_epoch()).count() << '\n';
    oss << '\n';
    oss << message << '\n';
    return oss.str();
}
 
