#include "asm.h"
#include <string.h>
#include <stdio.h>

int64_t asm_add(int64_t a, int64_t b) {
  asm("add %[s],%[t];"
	:[t]"+r"(a)
	:[s]"r"(b)
	);
  return a;
	//return a + b;
}

int asm_popcnt(uint64_t x) {
  int s = 0;
  /*for (int i = 0; i < 64; i++) {
    if ((x >> i) & 1) s++;
  }*/
  asm("mov $0x0, %%eax;"//s
      "mov $0x0, %%ecx;"//i=0
      "L0:cmp $0x40, %%ecx;"
      "jge L1;"//if i>=64 leave the loop
      "movq %%rdx, %%rbx;"
      "test $0x1, %%rbx;"
      "je L2;"
      "inc %%eax;"
      "L2:inc %%ecx;"
      "shr $1, %%rdx;"
      "jmp L0;"
      "L1: ;"
	:"=a"(s)
	:"d"(x)
	:"%rbx","ecx","cc"
	);
  return s;
}

void *asm_memcpy(void *dest, const void *src, size_t n) {
  asm("mov $0x0, %%rsi;"//i
      "L4:cmp %%rsi, %[n];"
      "jle L5;"
      "movb (%[s], %%rsi), %%al;"
      "movb %%al, (%[d], %%rsi);"
      "inc %%rsi;"
      "jmp L4;"
      "L5: ;"
	:[d]"+r"(dest)
	:[s]"r"(src),[n]"r"(n)
	:"memory","rsi","al"
	);
  return dest;
	//return memcpy(dest, src, n);
}

int asm_setjmp(asm_jmp_buf env) {
  asm("mov %%rbp, (%%rdi);"
      "mov (%%rsp), %%rcx;"
      "mov %%rcx, 0x8(%%rdi);"
      "lea 0x8(%%rsp), %%rcx;"
      "mov %%rcx, 0x10(%%rdi);"
      "mov %%rbx, 0x18(%%rdi);"
      "mov %%r12, 0x20(%%rdi);"
      "mov %%r13, 0x28(%%rdi);"
      "mov %%r14, 0x30(%%rdi);"
      "mov %%r15, 0x38(%%rdi);"
	:
	:"rdi"(env)
	);
  return 0;
	//return setjmp(env);
}

void asm_longjmp(asm_jmp_buf env, int val) {
  asm("mov 0x38(%%rdi), %%r15;"
      "mov 0x30(%%rdi), %%r14;"
      "mov 0x28(%%rdi), %%r13;"
      "mov 0x20(%%rdi), %%r12;"
      "mov 0x18(%%rdi), %%rbx;"
      "mov (%%rdi), %%rbp;"
      "mov 0x10(%%rdi), %%rsp;"
      "mov 0x8(%%rdi), %%rdi;"
      "mov %%rsi, %%rax;"
      "jmp *%%rdi;"
      :
      :"rdi"(env),"rsi"(val)
	);
	//longjmp(env, val);
}
