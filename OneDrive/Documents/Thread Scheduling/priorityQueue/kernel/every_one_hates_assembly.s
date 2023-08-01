    .global everyone_hates_assembly
everyone_hates_assembly:
    # get the 2 arguments from the stack
    mov 4(%esp),%eax      # from
    mov 8(%esp),%ecx      # to

    # don't save context unless we're really a thread
    cmp $0,%eax
    jz restore

    # save context
    push %ebx
    push %esi
    push %edi
    push %ebp
    mov %esp,(%eax)

restore:
    # restore context
    mov (%ecx),%esp
    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
    
    ret
