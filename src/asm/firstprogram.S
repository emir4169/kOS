#assemble with any online x86 assembler


mov    eax,0x3 # malloc syscall
mov    ebx,0x4 # 4 bytes
# ebx now contains address
int    0x80 # syscall
mov    BYTE PTR [ebx],0x41 # A
mov    BYTE PTR [ebx+0x1],0x42 # B
mov    BYTE PTR [ebx+0x2],0xa
mov    BYTE PTR [ebx+0x3],0x0
mov    eax,0x2 # print syscall
mov    ecx,0x3 # 3 bytes of text
int    0x80 # syscall
mov    eax,0x48 # set eax to 0x48
nop # do nothing
ret # exit