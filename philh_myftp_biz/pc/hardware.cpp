#include <pybind11/pybind11.h>
#include <iostream>
#include <string>

#include <_hw/hdd.hpp>
#include <_hw/pcie.hpp>
#include <_hw/vdisk.hpp>

namespace py = pybind11;

// Create the Python bindings
PYBIND11_MODULE(hardware, m) {

    py::class_<HardDrive>(m, "HardDrive")
        .def(py::init<
            std::string, std::string, 
            int, std::string
        >(),
            py::arg("Tower"), 
            py::arg("Conn"), 
            py::arg("ID"), 
            py::arg("SN")
        )
        .def_readonly("Connected", &HardDrive::Connected)
        .def_readonly("Tower", &HardDrive::Tower)
        .def_readonly("Conn", &HardDrive::Conn)
        .def_readonly("ID", &HardDrive::ID)
        .def_readonly("SN", &HardDrive::SN)
        .def_readonly("Index", &HardDrive::Index)
        .def_readonly("Name", &HardDrive::Name)       
        .def_property("FriendlyName", &HardDrive::FriendlyName, &HardDrive::setFriendlyName)
        .def_property("Usage", &HardDrive::Usage, &HardDrive::setUsage);

    py::class_<PCIeCard>(m, "PCIeCard")
        .def(py::init<std::string, int, std::string>(),
            py::arg("Slot"), 
            py::arg("Lanes"), 
            py::arg("DeviceId")
        )
        .def_readonly("Slot", &PCIeCard::Slot)
        .def_readonly("Lanes", &PCIeCard::Lanes)
        .def_readonly("DeviceId", &PCIeCard::DeviceId)
        .def_readonly("Name", &PCIeCard::Name)
        .def_property_readonly("Connected", &PCIeCard::Connected);

    py::class_<VirtualDisk>(m, "VirtualDisk")
        .def(py::init<std::string, std::string>(),
            py::arg("Name"), 
            py::arg("Mount")
        )
        .def_readonly("Name", &VirtualDisk::Name)
        .def_readonly("Mount", &VirtualDisk::Mount)
        .def_property("Connected", &VirtualDisk::Connected, &VirtualDisk::setConnected)
        .def("Repair", &VirtualDisk::Repair);

}

