#include <stdint.h>
const uint64_t M=0xffffffffffffffff;
uint64_t A[64];
uint64_t MOD(uint64_t x,uint64_t m){
  uint64_t qm=m;
  while(x>=m){
    while(qm<x&&qm<((uint64_t)1<<63)){
      qm=qm<<1;
    }
    while(qm>x&&qm>m){
      qm=qm>>1;
    }
    x=x-qm;
  }
  return x;
}
uint64_t ADD(uint64_t x,uint64_t y,uint64_t m){
  if(x>M-y){
    uint64_t t=x+y;
    return ADD(MOD(t,m),MOD((MOD(M,m)+MOD(1,m)),m),m);
  }
  else{
    return MOD((MOD(x,m)+MOD(y,m)),m);
  }
}
uint64_t multimod(uint64_t a, uint64_t b, uint64_t m) {
  uint64_t c=0;
  for(int i=0;i<64;i++){
    A[i]=(a>>i)&1;
  }
  for(int i=0;i<64;i++){
    if(A[i]==0){
      continue;
    }
    else{
	    uint64_t qb=b;
      for(int j=0;j<i;j++){
        if(qb>M-qb){
	  qb=ADD(qb,qb,m);
	}
	else qb=qb<<1;
      }
      c=ADD(c,qb,m);
    }
  }
  return MOD(c,m); // buggy
}
