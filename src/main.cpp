#include <filesystem>

#include <libvirt/libvirt-qemu.h>

int main(int argc, char **argv) {
    // this program needs 2 arguemnts:
    // the name of the vm and the filename it should dump the memory in.
    if (argc != 3) {
        std::fprintf(stderr, "Trufflepig Forensics libvirt memory dump tool\nusage: %s <libvirt_domain> <dump_filename>\n", argv[0]);
        exit(1);
    }

    // check if the dump file exists. If not, inform the user and get out.
    auto path = std::filesystem::absolute(argv[2]);
    if (std::filesystem::exists(path)) {
        std::fprintf(stderr, "file exists: %s\n", path.c_str());
        exit(2);
    }

    auto pConnection = virConnectOpen("qemu:///system");
    if (pConnection == nullptr) {
        std::fprintf(stderr, "could not open connection\n");
        exit(3);
    }

    auto pDomain = virDomainLookupByName(pConnection, argv[1]);
    if (pDomain == nullptr) {
        virConnectClose(pConnection);
        std::fprintf(stderr, "could not open domain\n");
        exit(4);
    }

    virDomainInfo info;
    auto err = virDomainGetInfo(pDomain, &info);
    if (err != 0) {
        std::fprintf(stderr, "could not get domain info\n");
        exit(5);
    }

    if (info.state == VIR_DOMAIN_RUNNING) {
        std::fprintf(stderr, "please pause the domain before dumping memory\n");
        exit(6);
    }

    if (info.state != VIR_DOMAIN_PAUSED) {
        std::fprintf(stderr, "vm is not in `paused` state. Please turn it on and pause it.\n");
        exit(7);
    }

    // virDomainInfo->maxMem is in KiB
    auto ramSize = info.maxMem * 1024;

    char *pOutputData;
    // working directory is `/` and path needs to be relative for some reason -> use an absolute path and prepend a .
    static constexpr const char *pCommandFormatString = "pmemsave 0 0x%016lx .%s";

    // get buffer len + one byte null termination
    auto len = static_cast<uint64_t>(std::snprintf(nullptr, 0, pCommandFormatString, ramSize, path.c_str())) + 1;
    auto pBuffer = new char[len];
    std::snprintf(pBuffer, len, pCommandFormatString, ramSize, path.c_str());
    // execute monitor command
    // NOTE: virDomainQemuMonitorCommand does not tell us if the file could not be written!
    auto r = virDomainQemuMonitorCommand(pDomain, pBuffer, &pOutputData, VIR_DOMAIN_QEMU_MONITOR_COMMAND_HMP);
    if (r != 0) {
        std::fprintf(stderr, "error: %i\n", r);
        exit(8);
    }

    // cleanup
    delete []pBuffer;
    virDomainFree(pDomain);
    virConnectClose(pConnection);

    if (!std::filesystem::exists(path)) {
        std::fprintf(stderr, "could not write file: unknown libvirt error\n");
        exit(9);
    }

    std::fprintf(stdout, "done, have a nice day\n");
    return 0;
}
