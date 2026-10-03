#include <pybind11/pybind11.h>
#include <algorithm>
#include <string>
#include <iostream>
#include <variant>
#include <memory>

#include "remap.h"

class UnconsumingIO { public:

    str _buffer;

    pyobj _pystr;

    std::iostream* _iostr = nullptr;

    UnconsumingIO(str stream) {
        pyobj cls = import("io").attr("StringIO");
        _pystr = cls(stream);
        _buffer = "";
    }
    
    UnconsumingIO(pyobj stream) {
        _pystr = stream;
        _buffer = "";
    }

    UnconsumingIO(std::iostream& stream) {
        _iostr = &stream;
        _buffer = "";
    }

    UnconsumingIO(std::ostream& stream) {
        _iostr = dynamic_cast<std::iostream*>(&stream);
        _buffer = "";
    }

    void write(const str& data) {
        int lpos = pos();
        if (_iostr == nullptr) {
            _pystr.attr("write")(data);
        } else {
            *_iostr << data;
        }
        seek(lpos);
    }

    void flush() {
        if (_iostr != nullptr) {
            _iostr->flush();
        } else if (py::hasattr(_pystr, "flush")) {
            _pystr.attr("flush")();
        }
    }

    void write(const py::bytes& data) {
        py::str sdata = data.attr("decode")("utf-8", "ignore");
        write(sdata.cast<str>());
    }

    str read(int size = -1) {

        if (_iostr == nullptr) {
            _buffer += _pystr.attr("read")().cast<str>();
        } else {
            _iostr->clear();
            char ch;
            while (_iostr->get(ch)) {
                _buffer += ch;
            }
        }

        int _len = static_cast<int>(_buffer.length());
        int sz = (size == -1) ? _len : std::min(size, _len);

        return _buffer.substr(0, sz);
    }

    void seek(int pos) {
        if (_iostr == nullptr) {
            _pystr.attr("seek")(pos);
        } else {
            _iostr->clear();
            _iostr->seekg(pos, std::ios::beg); // Sync read pointer
            _iostr->seekp(pos, std::ios::beg); // Sync write pointer
        }
    }

    int pos() {
        if (_iostr == nullptr) {
            return _pystr.attr("tell")().cast<int>();
        } else {
            return static_cast<int>(_iostr->tellp());
        }
    }

};

PYBIND11_MODULE(uio, m) {

    py::class_<UnconsumingIO>(m, "UnconsumingIO")
        .def(py::init<str>(), py::arg("stream"))
        .def(py::init<pyobj>(), py::arg("stream"))
        .def("write", py::overload_cast<const str&>(&UnconsumingIO::write))
        .def("write", py::overload_cast<const py::bytes&>(&UnconsumingIO::write))
        .def("read", &UnconsumingIO::read, py::arg("size") = -1)
        .def("seek", &UnconsumingIO::seek)
        .def("flush", &UnconsumingIO::flush)
        .def_property_readonly("pos", &UnconsumingIO::pos);

}
