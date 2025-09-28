  .text
  .globl main
main:
  li t2, 2
  li t3, 3
  mul t0, t2, t3
  li t2, 1
  mv t3, t0
  add t1, t2, t3
  mv a0, t1
  ret
