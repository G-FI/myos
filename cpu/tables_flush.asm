[global gdt_flush]

gdt_flush:
  mov eax, [esp + 4] ;address of gdt_ptr
  lgdt [eax]

  mov ax, 0x10    ; 0-8, 8-16-> null and code segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax
  ; flush the pipeline?
  ;  by the very definition of a far jump, it will automatically cause the CPU to update our cs register to the target segment
  jmp 0x08:.flush ; jmp <segment>:<offset>
.flush:
    ret
    
    
[global idt_flush]

idt_flush:
  mov eax, [esp + 4] ;address of gdt_ptr
  lidt [eax]
  ret