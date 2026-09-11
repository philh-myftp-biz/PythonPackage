#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <string>

namespace py = pybind11;

class MutInt { public:

    int value;

    MutInt(int val) {
        this->value = val;
    }

    // Python __bool__
    bool __bool__() const {
        return value > 0;
    }

    // Python __int__ and __index__
    int __int__() const {
        return value;
    }

    // Python __str__
    std::string __str__() const {
        return std::to_string(value);
    }

    // Python __iadd__ (inplace +=)
    MutInt& __iadd__(int other) {
        this->value += other;
        return *this;
    }

    // Python __isub__ (inplace -=)
    MutInt& __isub__(int other) {
        this->value -= other;
        return *this;
    }

    // Python __add__ (+)
    MutInt __add__(int other) const {
        return MutInt(this->value + other);
    }

    // Python __sub__ (-)
    MutInt __sub__(int other) const {
        return MutInt(this->value - other);
    }

};

class VERBOSE : public MutInt { public:

    int lvalue;

    VERBOSE(int val) : MutInt(value) {
        this->value = val;
        this->lvalue = val;
    }

    void pause() {
        this->lvalue = this->value;
        this->value = 0;
    }

    void resume() {
        this->value = this->lvalue;
    }

    void enable() {
        this->value = 1;
        this->lvalue = 1;
    }

    void disable() {
        this->value = 0;
        this->lvalue = 0;
    }
};

