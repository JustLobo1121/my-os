#include "screen.h"
#include "ports.h"

extern void hex_to_string(unsigned int val, char* dest);

unsigned int e1000_mmio_base = 0;
unsigned char mac_address[6];

unsigned int pci_read_dword(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset) {
    unsigned int address = (unsigned int)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    port_dword_out(0xCF8, address);
    return port_dword_in(0xCFC);
}

void pci_write_dword(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned int data) {
    unsigned int address = (unsigned int)((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000);
    port_dword_out(0xCF8, address);
    port_dword_out(0xCFC, data);
}

unsigned int e1000_read_reg(unsigned short offset) {
    volatile unsigned int* reg = (volatile unsigned int*)(e1000_mmio_base + offset);
    return *reg;
}

void e1000_write_reg(unsigned short offset, unsigned int value) {
    volatile unsigned int* reg = (volatile unsigned int*)(e1000_mmio_base + offset);
    *reg = value;
}

void print_mac_byte(unsigned char byte) {
    unsigned int mac_low = e1000_read_reg(0x5400);
    unsigned int mac_high = e1000_read_reg(0x5404);

    char str_low[11];
    char str_high[11];

    hex_to_string(mac_low, str_low);
    hex_to_string(mac_high, str_high);

    print("MAC RAL0 (Low) : ");
    print(str_low);
    print("\n");

    print("MAC RAH0 (High): ");
    print(str_high);
    print("\n");
}

void read_mac_address() {
    unsigned int mac_low = e1000_read_reg(0x5400);
    unsigned int mac_high = e1000_read_reg(0x5404);

    mac_address[0] = mac_low & 0xFF;
    mac_address[1] = (mac_low >> 8) & 0xFF;
    mac_address[2] = (mac_low >> 16) & 0xFF;
    mac_address[3] = (mac_low >> 24) & 0xFF;
    mac_address[4] = mac_high & 0xFF;
    mac_address[5] = (mac_high >> 8) & 0xFF;

    print("mac address: \n");
    print_mac_byte(1);
    // print("\n");
}

void init_E1000() {
    print("--- starting network ---\n");
    print("searching card intel pro/1000 (E1000)... \n");

    int found = 0;
    unsigned char target_bus, target_slot;

    for (int bus = 0; bus < 256; bus++) {
        for (int slot = 0; slot < 32; slot++) {
            unsigned int vendor_device = pci_read_dword(bus, slot, 0, 0);
            if (vendor_device == 0x100E8086) {
                target_bus = bus;
                target_slot = slot;
                found = 1;
                break;
            }
        }
        if (found) break;
    }

    if (!found) {
        print("Critical error: hardware compatible not found\n");
        return;
    }

     unsigned int bar0 = pci_read_dword(target_bus, target_slot, 0, 0x10);
     unsigned int mmio_address = bar0 & 0xFFFFFFF0;

     unsigned int pci_command = pci_read_dword(target_bus, target_slot, 0, 0x04);
     pci_command |= (1 << 2) | (1 << 1) | (1 << 10); 
     pci_write_dword(target_bus, target_slot, 0, 0x04, pci_command);
 
    e1000_mmio_base = mmio_address;
 
    e1000_write_reg(0x00D8, 0xFFFFFFFF); 
 
    volatile unsigned int clear_interrupt = e1000_read_reg(0x00C0);
 
    print("DMA activated. the card have access to the ram\n");
    
    char hex_address[11];
    hex_to_string(mmio_address, hex_address);
    print("MMIO Base: "); print(hex_address); print("\n");

    read_mac_address();
}
