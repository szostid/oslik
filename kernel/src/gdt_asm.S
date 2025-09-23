.intel_syntax noprefix

.global set_gdt
.global reload_segments

gdtr:
    .word 0   # limit (16 bits)
    .long 0   # base (32 bits)

set_gdt:
   mov   AX, [esp + 4]
   mov   [gdtr], AX
   mov   EAX, [ESP + 8]
   mov   [gdtr + 2], EAX
   lgdt  [gdtr]
   ret

reload_segments:
   # Reload CS register containing code selector:
   jmp   0x08:.reload_CS # 0x08 is a stand-in for your code segment

.reload_CS:
   # Reload data segment registers:
   mov   AX, 0x10 # 0x10 is a stand-in for your data segment
   mov   DS, AX
   mov   ES, AX
   mov   FS, AX
   mov   GS, AX
   mov   SS, AX
   ret