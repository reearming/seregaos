#include <stdint.h>
#include "IO.h"
#include "fbuffer.h"
#include "limine.h"
#include <stddef.h>
#include "acpi.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_ENABLE (1 << 11)
#define X2APIC_SVR_MSR 0x80F
#define IOAPIC_BASE 0xFEC00000
#define IOAPIC_REGSEL ((volatile uint32_t *)(IOAPIC_BASE + 0x00))
#define IOAPIC_WINDOW ((volatile uint32_t *)(IOAPIC_BASE + 0x10))

uint64_t hhdm_offset = 0;
volatile uint32_t *io_apic_regsel = NULL;
volatile uint32_t *io_apic_window = NULL;
uint32_t target_x2apic_id = 0;


__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};
__attribute__((used, section(".limine_requests")))
static volatile struct limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST_ID,
    .revision = 0
};


void ioapic_write(uint8_t reg, uint32_t val) {
    if (io_apic_regsel == NULL || io_apic_window == NULL) {
        printf("%RI/O APIC NOT INITIALIZED!");
        return;
    }
    *IOAPIC_REGSEL = reg;
    *IOAPIC_WINDOW = val;
}

void ioapic_keyboard(uint8_t vector) {
    ioapic_write(0x13, target_x2apic_id);

    uint32_t low = vector & 0xFF;
    ioapic_write(0x12, low);
}

static inline uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    asm("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    asm("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

void init_x2apic() {
    uint32_t eax, ebx, ecx, edx;

    asm volatile(
        "push %%rbx\n\t"
        "cpuid\n\t"
        "mov %%ebx, %1\n\t"
        "pop %%rbx"
        : "=a"(eax), "=r"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
        : "cc"
    );


    if (!(ecx & (1 << 21))) {
        print("%RERROR: x2APIC NOT SUPPORTED BY THE CPU!\n");
        return;
    }

    uint64_t apic_base_msr = read_msr(IA32_APIC_BASE_MSR);

    apic_base_msr |= (1ULL << 11);
    write_msr(IA32_APIC_BASE_MSR, apic_base_msr);

    apic_base_msr |= (1ULL << 10);
    write_msr(IA32_APIC_BASE_MSR, apic_base_msr);

    uint32_t svr = read_msr(X2APIC_SVR_MSR);
    svr |= (1ULL << 8);
    svr |= 0xFF;
    write_msr(X2APIC_SVR_MSR, svr);
}

void init_apic_acpi() {
    if (hhdm_request.response == NULL) {
        print("%RHHDM REQUEST FAILED\n");
        return;
    }
    hhdm_offset = hhdm_request.response->offset;
    printf("HHDM OFFSET: 0x%x\n", hhdm_offset);
    if (rsdp_request.response == NULL || rsdp_request.response->address == NULL) {
        print("%RACPI RSDP NOT FOUND\n");
        return;
    }

    struct xsdp *xsdp = (struct xsdp*)rsdp_request.response->address;
    if (xsdp->revision < 2) {
        print("%RXSDT IS NOT SUPPORTED\n");
        return;
    }
    //uint8_t *rsdp = (uint8_t *)rsdp_request.response->address;
    //printf("RSDP FOUND AT 0x%x\n", (uint64_t)rsdp);

    uint64_t xsdt_ptr = xsdp->xsdt_address;
    struct xsdt *xsdt = (struct xsdt *)(xsdt_ptr + hhdm_offset);
    printf("XSDT FOUND AT 0x%x\n", (uint64_t)xsdt_ptr);

    size_t entries_count = (xsdt->header.length - sizeof(struct acpi_sdt_header)) / 8;
    struct madt *madt = NULL;

    for (size_t i = 0; i < entries_count; i++) {
        uint64_t phys_table = xsdt->tables[i];

        struct acpi_sdt_header *table = (struct acpi_sdt_header *)(phys_table + hhdm_offset);
        if (table->signature[0] == 'A' && table->signature[1] == 'P' && table->signature[2] == 'I' && table->signature[3] == 'C') {
            madt = (struct madt*)table;
            break;
        }
    }

    if (madt == NULL) {
        print("%RMADT TABLE NOT FOUND\n");
        return;
    }
    
    printf("MADT FOUND AT 0x%x\n", (uint64_t)madt);

    uint8_t *ptr = madt->entries;
    uint8_t *end = (uint8_t *)madt + madt->header.length;

    int cpu_count = 0;

    while (ptr < end) {
        struct madt_entry_header *entry = (struct madt_entry_header *)ptr;
        if (entry->length == 0)
            break;

        switch (entry->type) {
        
            case 1:
                struct madt_io_apic *ioapic = (struct madt_io_apic *)ptr;
                uint32_t phys_io_apic = ioapic->io_apic_address;

                printf("FOUND I/0 APIC WITH ID %d AT 0x%x\n", ioapic->io_apic_id, phys_io_apic);
                uint64_t virt_io_apic = (uint64_t)phys_io_apic + hhdm_offset;
                io_apic_regsel = (volatile uint32_t *)(virt_io_apic + 0x00);
                io_apic_window = (volatile uint32_t *)(virt_io_apic + 0x10);

                printf("MAPPED I/O APIC TO VIRT 0x%x\n", virt_io_apic);
                break;

            case 2:
                struct madt_x2apic *x2apic = (struct madt_x2apic *)ptr;

                if (x2apic->flags & 1) {
                    cpu_count++;
                    printf("FOUND CORE %d WITH x2APIC ID %x\n", cpu_count, x2apic->x2apic_id);

                    if (target_x2apic_id == 0) {
                        target_x2apic_id = x2apic->x2apic_id;
                    }
                }
                break;

    
        }
        ptr += entry->length;
    }
    printf("ACTIVE CORES: %d\n", cpu_count);
}

