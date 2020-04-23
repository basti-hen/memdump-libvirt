# Trufflepig Forensics libvirt memory dump tool

This tool allows you to dump memory of libvirt hosted VMs in a file.

## Dependencies:

```
libvirt
```

## How to get it:

```
git clone https://github.com/trufflepig-forensics/memdump-libvirt && cd memdump-libvirt
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Usage:

```
./memdump-libvirt test-vm memory_dump.bin
```
