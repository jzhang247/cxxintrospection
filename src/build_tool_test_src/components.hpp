#pragma once
#pragma introspection_enabled

#include "common/_index.hpp"

namespace reflection_test_namespace {

struct base {
    int mb;
};
struct s0 {
    std::string m;
    int m0;

    struct s1 {
        int m;
        int m1;
        void method(const base& arg);
    };

    struct s2;
};

namespace nested_namespace {
struct sn {
    int mn;
};
}
namespace nested_namespace_2::nested_namespace_3 {
struct s23 {
    int m23;
};
}

}

void reflection_test_namespace::s0::s1::method(const base& arg) {

}

struct reflection_test_namespace::s0::s2 : public reflection_test_namespace::base {
    int m;
    int m2;
};


namespace {
struct local_type {
    int m;
    int m2;
};
}









