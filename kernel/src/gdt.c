#include <panic.h>
#include <stddef.h>
#include <stdint.h>

//
// clang-format off
//


// -------------------------------------------------------
// ACCESS BYTES (HIGHEST TO LOWEST)

/// Segment's "present" bit.
///
/// Must be set (`1`) for any valid segment.
#define SEG_PRESENT 0b10000000

/// Encodes the segment's descriptor privilege level.
///
/// - `0`: Ring 0 - kernel (highest privilege)
/// - `1`: Ring 1 - device drivers
/// - `2`: Ring 2 - device drivers
/// - `3`: Ring 3 - user mode (applications)
#define SEG_PRIVILEGE(x) (((x) << 5) & 0b01100000)

/// Encodes a segment's descriptor type.
/// - `0`: system
/// - `1`: code / data
#define SEG_TYPE(x) ((x) << 0b00000100)

// Segment desciptor types.
// Look at intel manual table 3-1, which shows them all

//                                // | Expand-down | Writeable | Accessed? |
#define SEG_DATA_RO        0b0000 // |      x      |     x     |     x     |
#define SEG_DATA_RO_A      0b0001 // |      x      |     x     |     ✓     |
#define SEG_DATA_RW        0b0010 // |      x      |     ✓     |     x     |
#define SEG_DATA_RW_A      0b0011 // |      x      |     ✓     |     ✓     |
#define SEG_DATA_RO_EXPD   0b0100 // |      ✓      |     x     |     x     |
#define SEG_DATA_RO_EXPD_A 0b0101 // |      ✓      |     x     |     ✓     |
#define SEG_DATA_RW_EXPD   0b0110 // |      ✓      |     ✓     |     x     |
#define SEG_DATA_RW_EXPD_A 0b0111 // |      ✓      |     ✓     |     ✓     |

//                                // | Conforming? | Readable? | Accessed? |
#define SEG_CODE_XO        0b1000 // |      x      |     x     |     x     |
#define SEG_CODE_XO_A      0b1001 // |      x      |     x     |     ✓     |
#define SEG_CODE_XR        0b1010 // |      x      |     ✓     |     x     |
#define SEG_CODE_XR_A      0b1011 // |      x      |     ✓     |     ✓     |
#define SEG_CODE_XO_C      0b1100 // |      ✓      |     x     |     x     |
#define SEG_CODE_XO_C_A    0b1101 // |      ✓      |     x     |     ✓     |
#define SEG_CODE_XR_C      0b1110 // |      ✓      |     ✓     |     x     |
#define SEG_CODE_XR_C_A    0b1111 // |      ✓      |     ✓     |     ✓     |

// END OF ACCESS BYTES
// -------------------------------------------------------

// -------------------------------------------------------
// SEGMENT FLAGS (HIGHEST TO LOWEST)

/// Sets the segment's granularity to KBs
///
/// This way, the segment's limit will be interpreted as pages (4KiB
/// increments), not bytes.
#define SEG_GRANULARITY_PAGES 1 << 3

/// Sets the segment's size flag to indicate a 32-bit protected mode segment
///
/// If not present, the descriptor will define a 16-bit protected mode segment.
/// A GDT can have both 16-bit and 32-bit selectors at once.
#define SEG_SIZE_32BIT 1 << 2

// NOTE: Long mode flag is unsupported

// NOTE: Byte 0 is reserved

// END OF SEGMENT FLAGS
// -------------------------------------------------------

//
// clang-format on
//

#define SEGMENT_COUNT 4

uint8_t gdt_table[(SEGMENT_COUNT + 1) * 8];

extern void set_gdt(uint16_t limit, size_t base);
extern void reload_segments(void);

typedef struct
{
    /// @brief Defines the location of byte 0 of the segment within the 4GB
    /// linear address space.
    uint32_t base;
    /// @brief Specified the size of the segment.
    ///
    /// If the granularity flag is set, the segment size can range from 4KB to
    /// 4GB, in 4KB increments. Otherwise, the segment size can range from 1B to
    /// 1MB, in byte increments.
    ///
    /// @attention The limit should be a 20-bit value. Using higher values will
    /// result in a kernel panic.
    uint32_t limit;
    /// @brief Segment descriptor access flags.
    ///
    /// This should include a present flag, `SEG_PRESENT`, a privilege flag,
    /// `SEG_PRIVILEGE(...)`, a descriptor type `SEG_DESCTYPE(...)`, and a
    /// segment type, `SEG_CODE_*` for code segments or `SEG_DATA_*` for data
    /// segments
    uint8_t access;
    /// @brief Segment descriptor flags.
    ///
    /// These should only optionally include `SEG_GRANULARITY_PAGES` and
    /// `SEG_SIZE_32BIT`
    uint8_t flags;
} gdt_segment_descriptor_t;

/// @brief Writes a GDT segment entry into the provided target pointer
/// @param target Target pointer to write the GDT segment descriptor
/// @param source Segment descriptor to be written
void write_gdt_segment_entry(uint8_t *target, gdt_segment_descriptor_t source)
{
    if (source.limit > 0xFFFFF)
    {
        kpanic("GDT cannot encode limits larger than 0xFFFFF");
    }

    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[5] = source.access;
    target[6] = (source.flags << 4) | ((source.limit >> 16) & 0x0F);
    target[7] = (source.base >> 24) & 0xFF;
}

typedef struct
{
    uint8_t request_privilege_level : 2;
    uint8_t table_indicator : 1;
    uint16_t index : 13;
} __attribute__((__packed__)) gdt_segment_selector_t;

void write_gdts(gdt_segment_descriptor_t segments[SEGMENT_COUNT])
{
    for (size_t i = 0; i < 8; i++)
    {
        gdt_table[i] = 0;
    }

    for (size_t i = 0; i < SEGMENT_COUNT; i++)
    {
        write_gdt_segment_entry(&gdt_table[(i + 1) * 8], segments[i]);
    }

    set_gdt((uint16_t)(sizeof(gdt_table) - 1), (size_t)(&gdt_table));
    reload_segments();
}

void setup_gdt(void)
{
    gdt_segment_descriptor_t code_kernel = {
        .base = 0x0,
        .limit = 0x000FFFFF,
        .access = SEG_PRESENT | SEG_PRIVILEGE(0) | SEG_TYPE(1) | SEG_CODE_XR,
        .flags = SEG_SIZE_32BIT | SEG_GRANULARITY_PAGES,
    };

    gdt_segment_descriptor_t data_kernel = {
        .base = 0x0,
        .limit = 0x000FFFFF,
        .access = SEG_PRESENT | SEG_PRIVILEGE(0) | SEG_TYPE(1) | SEG_DATA_RW,
        .flags = SEG_SIZE_32BIT | SEG_GRANULARITY_PAGES,
    };

    gdt_segment_descriptor_t code_user = {
        .base = 0x0,
        .limit = 0x000FFFFF,
        .access = SEG_PRESENT | SEG_PRIVILEGE(3) | SEG_TYPE(1) | SEG_CODE_XR,
        .flags = SEG_SIZE_32BIT | SEG_GRANULARITY_PAGES,
    };

    gdt_segment_descriptor_t data_user = {
        .base = 0x0,
        .limit = 0x000FFFFF,
        .access = SEG_PRESENT | SEG_PRIVILEGE(3) | SEG_TYPE(1) | SEG_DATA_RW,
        .flags = SEG_SIZE_32BIT | SEG_GRANULARITY_PAGES,
    };

    gdt_segment_descriptor_t segments[SEGMENT_COUNT] = {
        code_kernel,
        data_kernel,
        code_user,
        data_user,
    };

    write_gdts(segments);
}