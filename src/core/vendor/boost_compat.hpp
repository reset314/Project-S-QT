// boost_compat.hpp — C++17 replacements for Boost types used by aqt-stylesheets.
// Include this BEFORE any aqt-stylesheets headers via CMake's -include flag.
#pragma once

#include <variant>
#include <optional>
#include <algorithm>
#include <functional>
#include <string>
#include <cassert>
#include <QDebug>

namespace boost {

// boost::variant → std::variant
template <typename... Ts>
using variant = std::variant<Ts...>;

// boost::optional → std::optional
template <typename T>
using optional = std::optional<T>;

// boost::lexical_cast → thin wrapper
template <typename T, typename U>
T lexical_cast(const U &val);

template <>
inline float lexical_cast<float>(const std::string &s) {
    return QString::fromStdString(s).toFloat();
}

// boost::algorithm::clamp → std::clamp
namespace algorithm {
    using std::clamp;

    inline std::string to_lower_copy(const std::string &s) {
        return QString::fromStdString(s).toLower().toStdString();
    }
}

// boost::variant helpers
namespace detail {
    template <typename Visitor, typename Variant>
    decltype(auto) apply_visitor_impl(Visitor &&visitor, Variant &&var) {
        return std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(var));
    }
}

// boost::variant::apply_visitor → std::visit
struct apply_visitor_t {
    template <typename Visitor>
    struct delayed {
        Visitor &&visitor;
        template <typename Variant>
        decltype(auto) operator()(Variant &&var) {
            return std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(var));
        }
    };
};
// For usage: boost::apply_visitor(visitor, variant) — replace inline below

// boost::variant::get → std::get
using std::get;

// boost::variant::static_visitor — empty base, not needed with std::visit + generic lambda

// boost::functional::hash → std::hash
namespace functional {
    template <typename T>
    using hash = std::hash<T>;
}

// boost::adaptors::reverse → manual
namespace adaptors {
    struct reversed_t {};
    static constexpr reversed_t reversed{};
}
}

// boost::log::trivial → Qt
#define BOOST_LOG_TRIVIAL(severity) qDebug()

// boost::assert → assert
#define BOOST_ASSERT(expr) assert(expr)
#define BOOST_ASSERT_MSG(expr, msg) assert((expr) && (msg))

// boost::apply_visitor
namespace boost {
    template <typename Visitor, typename Variant>
    decltype(auto) apply_visitor(Visitor &&v, Variant &&var) {
        return std::visit(std::forward<Visitor>(v), std::forward<Variant>(var));
    }
}
