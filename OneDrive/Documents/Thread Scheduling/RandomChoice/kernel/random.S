# help from  https://wiki.osdev.org/Random_Number_Generator#Types_of_random_number_generators
    .global random
random:
    mov %ecx, 100   # number of retries
.retry:
    rdrand %eax
    jc .done     # carry flag is set on success
    loop .retry
.fail:
.done:
    ret