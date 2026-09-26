#ifndef ACPI_H
#define ACPI_H
#include <stdint.h>

struct acpi_sdt_header {
    char signature[4]; 
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct xsdt {
    struct acpi_sdt_header header;
    uint64_t tables[]; 
} __attribute__((packed));

struct xsdp {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed));

struct madt {
    struct acpi_sdt_header header;
    uint32_t local_apic_address; 
    uint32_t flags;
    uint8_t entries[];
} __attribute__((packed));

struct madt_entry_header {
    uint8_t type;
    uint8_t length;
} __attribute__((packed));

struct madt_io_apic {
    struct madt_entry_header header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_address; 
    uint32_t global_system_interrupt_base;
} __attribute__((packed));

struct madt_x2apic {
    struct madt_entry_header header;
    uint16_t reserved;
    uint32_t x2apic_id;
    uint32_t flags;
    uint32_t acpi_processor_uid;
} __attribute__((packed));
#endif
