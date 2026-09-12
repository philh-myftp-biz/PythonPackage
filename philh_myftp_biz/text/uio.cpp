#include <pybind11/pybind11.h>
#include <string>

namespace py = pybind11;

using str = std::string;

class UnconsumingIO { public:

    py::object _stream;
    str _buffer;

    UnconsumingIO(py::object stream) {
        
        if (py::isinstance<py::str>(stream)) {
            py::object cls = py::module_::import("io").attr("StringIO");
            stream = cls(stream);
        }
        
        this->_stream = stream;
        this->_buffer = "";
    
    }

    void write(const str& data) {
        _stream.attr("write")(data);
    }

    str read(py::object size = py::none()) {
        
        _buffer += _stream.attr("read")().cast<str>();

        if (size.is_none())
            return _buffer;

        size_t sz = size.cast<size_t>();
        if (sz >= _buffer.length()) {
            return _buffer;
        } else {
            return _buffer.substr(0, sz);
        }

    }

};

PYBIND11_MODULE(uio, m) {

    py::class_<UnconsumingIO>(m, "UnconsumingIO")
        .def(py::init<py::object>(), py::arg("stream"))
        .def("write", &UnconsumingIO::write)
        .def("read", &UnconsumingIO::read, py::arg("size") = py::none());

}
