#include <pybind11/pybind11.h>
#include <iostream>
#include <string>
#include <remap.h>

#include <_hw/device.h>

#ifdef WINDOWS
    #include <_hw/hdd.hpp>
    #include <_hw/pcie.hpp>
    #include <_hw/vdisk.hpp>
#endif

PYBIND11_MODULE(hardware, m) {

    // Bind the strict superclass using the PyDevice trampoline
    py::class_<Device, PyDevice>(m, "Device")
        .def(py::init<>())
        .def_property_readonly("Name", &Device::GetName)
        .def_property_readonly("Connected", &Device::GetConnected);

    #ifdef WINDOWS
    
        // Bind HardDrive as a child of Device
        py::class_<HardDrive, Device>(m, "HardDrive")
            .def(py::init<str, str, int, str>(),
                py::arg("Tower"), 
                py::arg("Conn"), 
                py::arg("ID"), 
                py::arg("SN")
            )
            .def_readonly("Tower", &HardDrive::Tower)
            .def_readonly("Conn", &HardDrive::Conn)
            .def_readonly("ID", &HardDrive::ID)
            .def_readonly("SN", &HardDrive::SN)
            .def_property_readonly("Index", &HardDrive::Index)
            .def_property("FriendlyName", &HardDrive::FriendlyName, &HardDrive::setFriendlyName)
            .def_property("Usage", &HardDrive::Usage, &HardDrive::setUsage);

        // Bind PCIeCard as a child of Device
        py::class_<PCIeCard, Device>(m, "PCIeCard")
            .def(py::init<str, int, str>(),
                py::arg("Slot"), py::arg("Lanes"), py::arg("DeviceId"))
            .def_readonly("Slot", &PCIeCard::Slot)
            .def_readonly("Lanes", &PCIeCard::Lanes)
            .def_readonly("DeviceId", &PCIeCard::DeviceId);

        // Bind VirtualDisk as a child of Device
        py::class_<VirtualDisk, Device>(m, "VirtualDisk")
            .def(py::init<str, str>(),
                py::arg("Name"), 
                py::arg("Mount")
            )
            .def_readonly("Mount", &VirtualDisk::Mount)
            .def_property("Connected", &VirtualDisk::GetConnected, &VirtualDisk::setConnected)
            .def("Repair", &VirtualDisk::Repair);

    #endif
}

