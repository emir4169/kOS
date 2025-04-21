# follow firstprogram_S instructions for this too

# get the filename
mov    eax,0x3 # malloc syscall
mov    ebx,0x9 # 9 bytes
mov    ecx,ebx
int    0x80
# move to the address the data   "test(dot)txt"
mov    BYTE PTR [ebx+0x0],0x74
mov    BYTE PTR [ebx+0x1],0x65
mov    BYTE PTR [ebx+0x2],0x73
mov    BYTE PTR [ebx+0x3],0x74
mov    BYTE PTR [ebx+0x4],0x2e
mov    BYTE PTR [ebx+0x5],0x74
mov    BYTE PTR [ebx+0x6],0x78
mov    BYTE PTR [ebx+0x7],0x74

# actual file stuff
mov eax, 00 # open
# ebx contains the address to the filename
# ecx contains length of filename
int 0x80
# ebx is address to filedata
# ecx is size of the file
# edx is address to the file name
mov    BYTE PTR [edx+0x1],0x41
# write
mov eax, 01
     # arguments:
     # edx = filename
     # ecx = length of new file data
     # ebx = address to new file data
     # returns:
     # ecx = did we write? FF = yes, 00 = no
     # edx = the address to the file data

int 0x80



mov eax, 5
ret