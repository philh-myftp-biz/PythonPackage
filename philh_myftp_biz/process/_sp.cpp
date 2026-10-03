#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <memory>

#include "subprocess.h"

#include "remap.h"

using subproc = struct subprocess_s;

class _SubProcess { public:

    subproc proc;
    
    bool hide;

    str _stdout_cache;
    str _stderr_cache;

    _SubProcess(
        const vector<str>& args,
        const str& cwd,
        bool hide,
        bool wait
    ): hide(hide) {

        vector<const char*> c_args;
        for (const auto& arg : args) {
            c_args.push_back(arg.c_str());
        }

        int options = subprocess_option_inherit_environment | subprocess_option_enable_async;
        
        if (hide) options |= subprocess_option_no_window;

        _stdout_cache = "";
        _stderr_cache = "";

        subprocess_create_ex(
            c_args.data(),
            options,
            nullptr,
            cwd.c_str(),
            &proc
        );

        if (wait) this->wait();
        
    }

    ~_SubProcess() {
        subprocess_destroy(&proc);
    }

    void _drain_stream(
        unsigned int (*read_func)(subproc*, char*, unsigned int), 
        str& cache
    ) {
        char buf[4096]; 
        unsigned int bytes_read = 0;
        do {
            bytes_read = read_func(&proc, buf, sizeof(buf));
            if (bytes_read > 0)
                cache.append(buf, bytes_read);
        } while (bytes_read == sizeof(buf));
    }

    str get_stdout() {
        _drain_stream(subprocess_read_stdout, _stdout_cache);
        return _stdout_cache;
    }

    str get_stderr() {
        _drain_stream(subprocess_read_stderr, _stderr_cache);
        return _stderr_cache;
    }

    int wait() {
        int code;
        subprocess_join(&proc, &code);
        return code;
    }

    void stop() {
        if (running())
            subprocess_terminate(&proc);
    }

    bool running() {
        return subprocess_alive(&proc) != 0;
    }

    int pid() {
        return static_cast<int>(proc.alive);
    }

};

PYBIND11_MODULE(_sp, m) {

    py::class_<_SubProcess>(m, "_SubProcess")
        .def(py::init<
                vector<str>, str, 
                bool, bool
            >(),
            py::arg("args"),
            py::arg("cwd"),
            py::arg("hide"),
            py::arg("wait")
        )
        .def("wait", &_SubProcess::wait)
        .def("stop", &_SubProcess::stop)
        .def_property_readonly("stdout", &_SubProcess::get_stdout)
        .def_property_readonly("stderr", &_SubProcess::get_stderr)
        .def_property_readonly("pid", &_SubProcess::pid)
        .def_property_readonly("running", &_SubProcess::running);

}

