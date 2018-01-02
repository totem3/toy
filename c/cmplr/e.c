#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

typedef unsigned (*asmFunc)(void);

static uint8_t * mem;
int main()
{
   // probably needs to be page aligned...

   unsigned int codeBytes = 4096;

   mem = mmap(
       NULL,
       codeBytes,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_ANONYMOUS | MAP_PRIVATE,
       0,
       0);

   // write some code in
   /* unsigned char * mem = (unsigned char *) (virtualCodeAddress); */
   // 元々のコードは↓だけど、RCX, RDXではないはず? うまく動かなかった。 RDI, RSI が正しいはずなので、0xf8, 0xf0。
   // https://qiita.com/FAMASoon/items/a93c1361f80bb28f895c
   /* mem[0] = 0x48; mem[1] = 0x89; mem[2] = 0xc8; // MOV RAX, RCX */
   /* mem[3] = 0x48; mem[4] = 0x01; mem[5] = 0xd0; // ADD RAX, RDX */
   /* mem[6] = 0xc3;                               // RET */
   mem[0] = 0x48; mem[1] = 0x89; mem[2] = 0xf8; // MOV RAX, RDI
   mem[3] = 0x48; mem[4] = 0x01; mem[5] = 0xf0; // ADD RAX, RDX
   mem[6] = 0xc3;                               // RET

   uint64_t ret = ((uint64_t(*)(uint64_t, uint64_t))(mem))(11, 32);

   /* unsigned out = myFunc(); */

   printf("out is %llu\n", ret);

   return 0;
}
