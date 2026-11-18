/*
 * actimage.annotated.c  --  annotated Ghidra decompile of root/bin/actimage
 * ============================================================================
 * The A/B dual-image selector. Reads (and with -w sets) the "current_active"
 * variable in the U-Boot NAND environment (partition /dev/mtd3) to decide which
 * application image /etc/init.d/rcS mounts as /disk (mtdblock8 vs mtdblock9).
 * See docs/boot-process.md section 6.4.
 *
 * actimage is STATICALLY linked; Ghidra resolved the libc calls from the symbol
 * table, so most appear under their real names already. This file keeps ONLY
 * actimage's own routines annotated; the bundled glibc functions are present in
 * actimage.decompiled.c but are standard library code, not OEM logic. A few
 * internal helpers Ghidra could not name remain as FUN_<addr>. Not produced by
 * running the binary.
 */

/* ---- _init @ 004000cc ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/* ---- __start @ 00400120 ----
 * (library/support function; see decompiled body)
 */

void processEntry __start(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined1 auStack_20 [16];
  code *local_10;
  undefined4 local_c;
  undefined1 *local_8;
  
  local_8 = auStack_20;
  local_10 = _fini;
  local_c = param_1;
  __uClibc_main(main,param_2,&param_3,_init);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* ---- __do_global_dtors_aux @ 00400170 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x004001c4) */
/* WARNING: Removing unreachable block (ram,0x004001c8) */
/* WARNING: Removing unreachable block (ram,0x004001ec) */
/* WARNING: Removing unreachable block (ram,0x004001fc) */

void __do_global_dtors_aux(void)

{
  if (completed_4749 == '\0') {
    completed_4749 = '\x01';
  }
  return;
}



/* ---- frame_dummy @ 0040022c ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400240) */
/* WARNING: Removing unreachable block (ram,0x00400270) */

void frame_dummy(void)

{
  return;
}



/* ---- main @ 00400290 ----
 * Entry. nandenv_open() the U-Boot environment on NAND, then:
 *  - no args: nand_read_env("current_active") -> atoi -> return 0 or 1 (default
 *    0 if invalid/missing). rcS uses this EXIT CODE to pick the active image:
 *    0 => mount /dev/mtdblock8 as /disk, 1 => /dev/mtdblock9.
 *  - '-w <0|1>': nand_write_env("current_active", <n>) to switch the active
 *    image bank (used by an upgrade to flip to the freshly written image).
 *  nandenv_close() at the end. This is the A/B dual-image selector.
 */

int main(int param_1,int param_2)

{
  int iVar1;
  int local_18;
  char *local_14;
  int local_10;
  
  local_18 = 0;
  local_14 = (char *)0x0;
  iVar1 = nandenv_open();
  if (iVar1 == 0) {
    if (param_1 == 1) {
      iVar1 = nand_read_env("current_active",&local_14);
      if (iVar1 != 0) {
        puts("Can\'t get current active image.");
        return 0;
      }
      local_18 = atoi(local_14);
      if ((local_18 != 0) && (local_18 != 1)) {
        local_18 = 0;
      }
    }
    else if (param_1 == 3) {
      iVar1 = strcmp(*(char **)(param_2 + 4),"-w");
      if (iVar1 == 0) {
        local_18 = atoi(*(char **)(param_2 + 8));
        if ((local_18 == 0) || (local_18 == 1)) {
          iVar1 = nand_write_env("current_active",*(undefined4 *)(param_2 + 8));
          if (iVar1 != 0) {
            puts("Can\'t set current active image.");
          }
        }
        else {
          puts("Incorrect parameter.");
          local_18 = 0;
        }
      }
    }
    else {
      puts("Incorrect parameter.");
      local_18 = 0;
    }
    nandenv_close();
    local_10 = local_18;
  }
  else {
    puts("nandenv_open() failed.");
    local_10 = 0;
  }
  return local_10;
}



/* ---- crc32 @ 004004e0 ----
 * Standard CRC32 (U-Boot env checksum).
 */

uint crc32(uint param_1,byte *param_2,uint param_3)

{
  uint uVar1;
  uint local_res0;
  byte *local_res4;
  uint local_res8;
  
  local_res0 = ~param_1;
  local_res4 = param_2;
  for (local_res8 = param_3; 7 < local_res8; local_res8 = local_res8 - 8) {
    uVar1 = *(uint *)(&crc_table + ((*local_res4 ^ local_res0) & 0xff) * 4) ^ local_res0 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[1] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[2] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[3] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[4] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[5] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    uVar1 = *(uint *)(&crc_table + ((local_res4[6] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    local_res0 = *(uint *)(&crc_table + ((local_res4[7] ^ uVar1) & 0xff) * 4) ^ uVar1 >> 8;
    local_res4 = local_res4 + 8;
  }
  for (; local_res8 != 0; local_res8 = local_res8 - 1) {
    local_res0 = *(uint *)(&crc_table + ((*local_res4 ^ local_res0) & 0xff) * 4) ^ local_res0 >> 8;
    local_res4 = local_res4 + 1;
  }
  return ~local_res0;
}



/* ---- env_crc_update @ 004007f0 ----
 * Recompute the env CRC32 header after a change.
 */

void env_crc_update(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  puVar1 = penv;
  uVar2 = crc32(0,penv + 1,0x7fc);
  *puVar1 = uVar2;
  return;
}



/* ---- envmatch @ 0040084c ----
 * Match 'name=' at the start of an env entry.
 */

char * envmatch(char *param_1,char *param_2)

{
  char *local_res0;
  char *local_res4;
  char *local_10;
  
  local_res4 = param_2;
  for (local_res0 = param_1; (*local_res0 == *local_res4 && (*local_res0 != '\0'));
      local_res0 = local_res0 + 1) {
    if (*local_res0 == '=') {
      return local_res4 + 1;
    }
    local_res4 = local_res4 + 1;
  }
  if ((*local_res0 == '\0') && (*local_res4 == '=')) {
    local_10 = local_res4 + 1;
  }
  else {
    local_10 = (char *)0x0;
  }
  return local_10;
}



/* ---- _getenv @ 00400934 ----
 * (library/support function; see decompiled body)
 */

char * _getenv(char *__name)

{
  char *pcVar1;
  char *pcVar2;
  char *local_18;
  char *local_14;
  
  pcVar1 = (char *)(penv + 0x800);
  local_14 = (char *)(penv + 4);
  while( true ) {
    if (*local_14 == '\0') {
      return (char *)0x0;
    }
    for (local_18 = local_14; *local_18 != '\0'; local_18 = local_18 + 1) {
      if (pcVar1 <= local_18) {
        return (char *)0x0;
      }
    }
    pcVar2 = (char *)envmatch(__name,local_14);
    if (pcVar2 != (char *)0x0) break;
    local_14 = local_18 + 1;
  }
  return pcVar2;
}



/* ---- _getenv2 @ 00400a2c ----
 * (library/support function; see decompiled body)
 */

char * _getenv2(undefined4 param_1)

{
  char *pcVar1;
  int iVar2;
  char *local_18;
  char *local_14;
  
  pcVar1 = (char *)(penv + 0x800);
  local_14 = (char *)(penv + 4);
  while( true ) {
    if (*local_14 == '\0') {
      return (char *)0x0;
    }
    for (local_18 = local_14; *local_18 != '\0'; local_18 = local_18 + 1) {
      if (pcVar1 <= local_18) {
        return (char *)0x0;
      }
    }
    iVar2 = envmatch(param_1,local_14);
    if (iVar2 != 0) break;
    local_14 = local_18 + 1;
  }
  return local_14;
}



/* ---- do_setenv @ 00400b24 ----
 * In-memory U-Boot setenv: replace or append 'name=value' in the env buffer
 * (reports 'environment overflow' if it won't fit).
 */

undefined4 do_setenv(char *param_1,char *param_2)

{
  char *pcVar1;
  char *pcVar2;
  size_t sVar3;
  size_t sVar4;
  char *local_res0;
  char *local_res4;
  char local_34;
  char *local_30;
  char *local_28;
  char *local_24;
  char *local_20;
  undefined4 local_18;
  
  local_24 = (char *)_getenv2(param_1);
  if (local_24 != (char *)0x0) {
    for (local_20 = local_24; *local_20 != '\0'; local_20 = local_20 + 1) {
    }
    pcVar1 = local_20 + 1;
    if (*pcVar1 == '\0') {
      *local_24 = '\0';
      local_24[1] = '\0';
    }
    else {
      pcVar2 = (char *)(penv + 0x800);
      local_34 = *pcVar1;
      for (local_30 = local_20 + 2;
          ((local_20 = pcVar1, local_30 < pcVar2 && (local_34 != '\0')) && (*local_30 != '\0'));
          local_30 = local_30 + 1) {
        local_34 = *local_30;
      }
      for (; local_20 <= pcVar2; local_20 = local_20 + 1) {
        *local_24 = *local_20;
        local_24 = local_24 + 1;
      }
    }
  }
  if (param_2 == (char *)0x0) {
    local_18 = 0;
  }
  else if (*param_2 == '\0') {
    local_18 = 0;
  }
  else {
    for (local_28 = (char *)(penv + 4); (*local_28 != '\0' || (local_28[1] != '\0'));
        local_28 = local_28 + 1) {
    }
    if ((char *)(penv + 4) < local_28) {
      local_28 = local_28 + 1;
    }
    sVar3 = strlen(param_1);
    sVar4 = strlen(param_2);
    local_res0 = param_1;
    if ((penv - (int)local_28) + 0x800U < sVar3 + sVar4 + 3) {
      fprintf((FILE *)stderr,"## Error:environment overflow, \"%s\" deleted\n",param_1);
      local_18 = 0xffffffff;
    }
    else {
      for (; *local_res0 != '\0'; local_res0 = local_res0 + 1) {
        *local_28 = *local_res0;
        local_28 = local_28 + 1;
      }
      *local_28 = '=';
      pcVar1 = local_28;
      for (local_res4 = param_2; local_28 = pcVar1 + 1, *local_res4 != '\0';
          local_res4 = local_res4 + 1) {
        *local_28 = *local_res4;
        pcVar1 = local_28;
      }
      *local_28 = '\0';
      pcVar1[2] = '\0';
      local_18 = 0;
    }
  }
  return local_18;
}



/* ---- getfirstgoodblock @ 00400ea0 ----
 * Find the first non-bad NAND block for the env (bad-block aware).
 */

uint getfirstgoodblock(void)

{
  int iVar1;
  uint local_20;
  
  local_20 = 0;
  while( true ) {
    if (available_blocks <= local_20) {
      return 0xffffffff;
    }
    iVar1 = ioctl(envfd,0x80084d0b);
    if (iVar1 == 0) break;
    local_20 = local_20 + 1;
  }
  return local_20;
}



/* ---- do_nand_write @ 00400f68 ----
 * Low-level NAND write of the env block (ioctl MEMERASE + page writes).
 */

undefined4 do_nand_write(void)

{
  uint param2;
  int iVar1;
  uint uVar2;
  size_t sVar3;
  void *local_28;
  uint local_1c;
  undefined4 local_10;
  
  env_crc_update();
  iVar1 = getfirstgoodblock();
  if (iVar1 < 0) {
    fwrite("## Error:no good blocks in this device.\n",1,0x28,(FILE *)stderr);
    local_10 = 0xffffffff;
  }
  else {
    param2 = iVar1 * blocksize;
    iVar1 = ioctl(envfd,0x80084d02);
    if (iVar1 < 0) {
      fprintf((FILE *)stderr,"## Error:can\'t erase nand device block start from %u.\n",param2);
      local_10 = 0xffffffff;
    }
    else {
      uVar2 = lseek(envfd,param2,0);
      if (uVar2 == param2) {
        local_28 = penv;
        for (local_1c = 0; local_1c < env_pages; local_1c = local_1c + 1) {
          sVar3 = write(envfd,local_28,pagesize);
          if (sVar3 != pagesize) {
            fwrite("## Error:Can\'t Write env to NAND Flash.\n",1,0x28,(FILE *)stderr);
            return 0xffffffff;
          }
          local_28 = (void *)((int)local_28 + pagesize);
        }
        local_10 = 0;
      }
      else {
        fwrite("## Error:can\'t find a good block in this device.\n",1,0x31,(FILE *)stderr);
        local_10 = 0xffffffff;
      }
    }
  }
  return local_10;
}



/* ---- nandenv_open @ 004011c4 ----
 * Open the U-Boot environment partition (/dev/mtd3), read the env block into
 * memory (nand_read_env over good blocks), and verify its CRC
 * (CRC of boot env is incorrect -> error).
 */

undefined4 nandenv_open(void)

{
  int iVar1;
  __off_t _Var2;
  size_t sVar3;
  int iVar4;
  int *local_50;
  uint local_4c;
  uint local_38;
  uint local_34;
  uint local_30;
  undefined4 local_2c;
  undefined4 local_20;
  
  envfd = open("/dev/mtd3",2);
  if (envfd == -1) {
    local_20 = 0xffffffff;
  }
  else {
    iVar1 = ioctl(envfd,0x40204d01);
    if (iVar1 == 0) {
      blocksize = local_34;
      pagesize = local_30;
      mtdsize = local_38;
      ooblen = local_2c;
      available_blocks = local_38 / local_34;
      if (local_34 == 0) {
        trap(7);
      }
      env_pages = (local_30 + 0x800) / local_30;
      if (local_30 == 0) {
        trap(7);
      }
      penv = calloc(1,env_pages * local_30);
      if (penv == (int *)0x0) {
        fwrite("## Error:can\'t allocate memory\n",1,0x1f,(FILE *)stderr);
        local_20 = 0xffffffff;
      }
      else {
        iVar1 = getfirstgoodblock();
        if (iVar1 < 0) {
          fwrite("## Error:can\'t find a good block in this device.\n",1,0x31,(FILE *)stderr);
          local_20 = 0xffffffff;
        }
        else {
          iVar1 = iVar1 * blocksize;
          _Var2 = lseek(envfd,iVar1,0);
          if (_Var2 == iVar1) {
            local_50 = penv;
            for (local_4c = 0; local_4c < env_pages; local_4c = local_4c + 1) {
              sVar3 = read(envfd,local_50,pagesize);
              if (sVar3 != pagesize) {
                fwrite("## Error:Can\'t read env from NAND Flash.\n",1,0x29,(FILE *)stderr);
                return 0xffffffff;
              }
              local_50 = (int *)((int)local_50 + pagesize);
            }
            iVar4 = *penv;
            iVar1 = crc32(0,penv + 1,0x7fc);
            if (iVar4 != iVar1) {
              puts("CRC of boot env is incorrect.");
            }
            local_20 = 0;
          }
          else {
            fwrite("## Error:Can\'t find a good block in this device.\n",1,0x31,(FILE *)stderr);
            local_20 = 0xffffffff;
          }
        }
      }
    }
    else {
      local_20 = 0xffffffff;
    }
  }
  return local_20;
}



/* ---- nandenv_close @ 00401584 ----
 * Flush/close the env partition.
 */

void nandenv_close(void)

{
  if (envfd != -1) {
    close(envfd);
  }
  if (penv != (void *)0x0) {
    free(penv);
  }
  return;
}



/* ---- nand_read_env @ 00401608 ----
 * Read the env block(s) from NAND into the working buffer.
 */

undefined4 nand_read_env(char *param_1,int *param_2)

{
  char *pcVar1;
  undefined4 local_10;
  
  if (((param_1 == (char *)0x0) || (param_2 == (int *)0x0)) || (*param_1 == '\0')) {
    fwrite("## Error:invalid arguments\n",1,0x1b,(FILE *)stderr);
    local_10 = 0xffffffff;
  }
  else {
    pcVar1 = _getenv(param_1);
    *param_2 = (int)pcVar1;
    if (*param_2 == 0) {
      local_10 = 0xffffffff;
    }
    else {
      local_10 = 0;
    }
  }
  return local_10;
}



/* ---- nand_write_env @ 004016dc ----
 * Set a variable in the env image (do_setenv), recompute the CRC
 * (env_crc_update), and write the env block back to NAND (do_nand_write),
 * erasing the target blocks first and skipping bad blocks.
 */

undefined4 nand_write_env(char *param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 local_10;
  
  if ((param_1 == (char *)0x0) || (*param_1 == '\0')) {
    fwrite("## Error:invalid arguments\n",1,0x1b,(FILE *)stderr);
    local_10 = 0xffffffff;
  }
  else {
    iVar1 = do_setenv(param_1,param_2);
    if (iVar1 == 0) {
      local_10 = do_nand_write();
    }
    else {
      local_10 = 0xffffffff;
    }
  }
  return local_10;
}



/* ---- nand_clean_env @ 0040179c ----
 * Erase/clear the environment region.
 */

void nand_clean_env(void)

{
  memset(penv,0,0x800);
  do_nand_write();
  return;
}



/* ---- nandenvdump @ 004017f0 ----
 * Debug: dump the parsed env ('------->Nand env dump:').
 */

void nandenvdump(void)

{
  uint *puVar1;
  uint local_18;
  uint *local_10;
  uint *local_c;
  
  puVar1 = penv + 0x200;
  puts("------->Nand env dump:");
  puts("Nand device information:");
  printf("blocksize =%u\n",blocksize);
  printf("pagesize =%u\n",pagesize);
  printf("mtdsize =%u\n",mtdsize);
  printf("ooblen =%u\n",ooblen);
  printf("available_blocks=%u\n",available_blocks);
  puts("Env data:");
  printf("Env head addr 0x%08x\n",(uint)penv);
  printf("Env addr 0x%08x\n",(uint)(penv + 1));
  printf("crc:0x%08x\n",*penv);
  local_c = penv + 1;
  while ((char)*local_c != '\0') {
    for (local_10 = local_c; ((char)*local_10 != '\0' && (local_10 < puVar1));
        local_10 = (uint *)((int)local_10 + 1)) {
    }
    if (local_c != local_10) {
      printf("\"%s\"\n",(char *)local_c);
    }
    local_c = (uint *)((int)local_10 + 1);
  }
  for (local_18 = 0;
      ((int)local_18 < 0x7fb &&
      ((*(char *)((int)penv + local_18 + 4) != '\0' || (*(char *)((int)penv + local_18 + 5) != '\0')
       ))); local_18 = local_18 + 1) {
  }
  if ((*(char *)((int)penv + local_18 + 4) == '\0') && (*(char *)((int)penv + local_18 + 5) == '\0')
     ) {
    printf("Left space:%u\n",~local_18 + 0x7fb);
  }
  else {
    puts("##Error:No end flag.");
  }
  return;
}



/* ---- close @ 00401b30 ----
 * (library/support function; see decompiled body)
 */

int close(int __fd)

{
  int iVar1;
  undefined4 *puVar2;
  int in_a3;
  
  iVar1 = 0xfa6;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfa6;
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- ioctl @ 00401b90 ----
 * (library/support function; see decompiled body)
 */

int ioctl(int __fd,ulong __request,...)

{
  undefined4 *puVar1;
  int in_a3;
  int iVar2;
  
  syscall(0);
  iVar2 = 0xfd6;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0xfd6;
    iVar2 = -1;
  }
  return iVar2;
}



/* ---- lseek @ 00401c00 ----
 * (library/support function; see decompiled body)
 */

__off_t lseek(int __fd,__off_t __offset,int __whence)

{
  __off_t _Var1;
  undefined4 *puVar2;
  int in_a3;
  
  _Var1 = 0xfb3;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfb3;
    _Var1 = -1;
  }
  return _Var1;
}



/* ---- open @ 00401c60 ----
 * (library/support function; see decompiled body)
 */

int open(char *__file,int __oflag,...)

{
  undefined4 *puVar1;
  undefined4 in_a2;
  undefined4 uVar2;
  int in_a3;
  int iVar3;
  
  uVar2 = 0;
  if ((__oflag & 0x100U) != 0) {
    uVar2 = in_a2;
  }
  syscall(0);
  iVar3 = 0xfa5;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)(__file,__oflag,uVar2);
    *puVar1 = 0xfa5;
    iVar3 = -1;
  }
  return iVar3;
}



/* ---- creat @ 00401cd8 ----
 * (library/support function; see decompiled body)
 */

int creat(char *__file,__mode_t __mode)

{
  int iVar1;
  
                    /* WARNING: Could not recover jumptable at 0x00401cec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  iVar1 = (*(code *)PTR_open_0045abb8)(__file,0x301,__mode);
  return iVar1;
}



/* ---- read @ 00401d00 ----
 * (library/support function; see decompiled body)
 */

ssize_t read(int __fd,void *__buf,size_t __nbytes)

{
  ssize_t sVar1;
  undefined4 *puVar2;
  int in_a3;
  
  sVar1 = 0xfa3;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfa3;
    sVar1 = -1;
  }
  return sVar1;
}



/* ---- write @ 00401d60 ----
 * (library/support function; see decompiled body)
 */

ssize_t write(int __fd,void *__buf,size_t __n)

{
  ssize_t sVar1;
  undefined4 *puVar2;
  int in_a3;
  
  sVar1 = 0xfa4;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfa4;
    sVar1 = -1;
  }
  return sVar1;
}



/* ---- __GI___errno_location @ 00401dc0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int * __GI___errno_location(void)

{
  return (int *)PTR_errno_0045acb0;
}



/* ---- puts @ 00401de0 ----
 * (library/support function; see decompiled body)
 */

int puts(char *__s)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined1 auStack_28 [20];
  
  iVar3 = *(int *)PTR_stdout_0045add4;
  iVar4 = *(int *)(iVar3 + 0x4c);
  if (iVar4 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
              (auStack_28,PTR___pthread_mutex_trylock_0045ad64,iVar3 + 0x50);
    (*(code *)PTR___pthread_mutex_trylock_0045adcc)(iVar3 + 0x50);
  }
  iVar1 = (*(code *)PTR_fputs_unlocked_0045ada8)(__s,iVar3);
  iVar2 = iVar1;
  if (iVar1 != -1) {
    iVar3 = (*(code *)PTR_fputc_unlocked_0045ad1c)(10,iVar3);
    iVar2 = -1;
    if (iVar3 != -1) {
      iVar2 = iVar1 + 1;
    }
  }
  if (iVar4 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_28,1);
  }
  return iVar2;
}



/* ---- printf @ 00401ed0 ----
 * (library/support function; see decompiled body)
 */

int printf(char *__format,...)

{
  int iVar1;
  undefined4 in_a1;
  undefined4 in_a2;
  undefined4 in_a3;
  undefined4 local_res4;
  undefined4 local_res8;
  undefined4 local_resc;
  
  local_res4 = in_a1;
  local_res8 = in_a2;
  local_resc = in_a3;
  iVar1 = (*(code *)PTR_vfprintf_0045ace8)(*(undefined4 *)PTR_stdout_0045add4,__format,&local_res4);
  return iVar1;
}



/* ---- fprintf @ 00401f30 ----
 * (library/support function; see decompiled body)
 */

int fprintf(FILE *__stream,char *__format,...)

{
  int iVar1;
  undefined4 in_a2;
  undefined4 in_a3;
  undefined4 local_res8;
  undefined4 local_resc;
  
  local_res8 = in_a2;
  local_resc = in_a3;
  iVar1 = (*(code *)PTR_vfprintf_0045ace8)(__stream,__format,&local_res8);
  return iVar1;
}



/* ---- _stdio_init @ 00401f80 ----
 * (library/support function; see decompiled body)
 */

void _stdio_init(void)

{
  ushort uVar1;
  undefined *puVar2;
  undefined *puVar3;
  short sVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  
  puVar3 = PTR_isatty_0045ad40;
  puVar2 = PTR_0045abbc;
  puVar5 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
  uVar6 = *puVar5;
  uVar1 = *(ushort *)(puVar2 + -0x5658);
  sVar4 = (*(code *)puVar3)(0);
  *(ushort *)(puVar2 + -0x5658) = uVar1 ^ (1 - sVar4) * 0x100;
  uVar1 = *(ushort *)(puVar2 + -22000);
  sVar4 = (*(code *)puVar3)(1);
  *(ushort *)(puVar2 + -22000) = uVar1 ^ (1 - sVar4) * 0x100;
  *puVar5 = uVar6;
  return;
}



/* ---- __stdio_init_mutex @ 00402044 ----
 * (library/support function; see decompiled body)
 */

void __stdio_init_mutex(undefined4 param_1)

{
                    /* WARNING: Could not recover jumptable at 0x0040205c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memcpy_0045ad34)(param_1,PTR_s__a__d__b__Y__T_0045abc0 + -0x6450,0x18);
  return;
}



/* ---- _stdio_term @ 00402064 ----
 * (library/support function; see decompiled body)
 */

void _stdio_term(void)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *puVar3;
  int iVar4;
  undefined4 uVar5;
  ushort *puVar6;
  ushort *puVar7;
  undefined2 *puVar8;
  
  puVar3 = PTR_memcpy_0045ad34;
  puVar1 = PTR_s__a__d__b__Y__T_0045abc0;
  (*(code *)PTR_memcpy_0045ad34)
            (PTR__stdio_openlist_add_lock_0045acd4,PTR_s__a__d__b__Y__T_0045abc0 + -0x6450,0x18);
  (*(code *)puVar3)(PTR__stdio_openlist_del_lock_0045acc0,puVar1 + -0x6450,0x18);
  puVar8 = *(undefined2 **)PTR__stdio_openlist_0045ad54;
  while( true ) {
    puVar2 = PTR___stdio_wcommit_0045abc4;
    if (puVar8 == (undefined2 *)0x0) break;
    iVar4 = (*(code *)PTR___pthread_mutex_trylock_0045ad5c)(puVar8 + 0x28);
    if (iVar4 != 0) {
      uVar5 = *(undefined4 *)(puVar8 + 4);
      *puVar8 = 0x30;
      *(undefined4 *)(puVar8 + 10) = uVar5;
      *(undefined4 *)(puVar8 + 0xc) = uVar5;
      *(undefined4 *)(puVar8 + 0xe) = uVar5;
      *(undefined4 *)(puVar8 + 8) = uVar5;
    }
    *(undefined4 *)(puVar8 + 0x26) = 1;
    (*(code *)puVar3)(puVar8 + 0x28,puVar1 + -0x6450,0x18);
    puVar8 = *(undefined2 **)(puVar8 + 0x10);
  }
  puVar7 = *(ushort **)PTR__stdio_openlist_0045ad54;
LAB_00402194:
  do {
    if (puVar7 == (ushort *)0x0) {
      return;
    }
    if ((*puVar7 & 0x40) == 0) {
      puVar6 = *(ushort **)(puVar7 + 0x12);
    }
    else {
      (*(code *)puVar2)(puVar7);
      puVar6 = *(ushort **)(puVar7 + 0x12);
    }
    if (puVar6 != puVar7 + 2) {
      if (*(code **)(puVar7 + 0x1a) == (code *)0x0) {
        puVar7 = *(ushort **)(puVar7 + 0x10);
        goto LAB_00402194;
      }
      (**(code **)(puVar7 + 0x1a))(puVar6);
    }
    puVar7 = *(ushort **)(puVar7 + 0x10);
  } while( true );
}



/* ---- __stdio_wcommit @ 004021c0 ----
 * (library/support function; see decompiled body)
 */

int __stdio_wcommit(int param_1)

{
  undefined *puVar1;
  
  puVar1 = PTR___stdio_WRITE_0045abc8;
  if (*(int *)(param_1 + 0x10) != *(int *)(param_1 + 8)) {
    *(int *)(param_1 + 0x10) = *(int *)(param_1 + 8);
    (*(code *)puVar1)();
  }
  return *(int *)(param_1 + 0x10) - *(int *)(param_1 + 8);
}



/* ---- _cs_close @ 00402220 ----
 * (library/support function; see decompiled body)
 */

void _cs_close(undefined4 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x00402230. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_0045abcc)(*param_1);
  return;
}



/* ---- _cs_seek @ 00402238 ----
 * (library/support function; see decompiled body)
 */

undefined4 _cs_seek(undefined4 *param_1,longlong *param_2,undefined4 param_3)

{
  undefined4 uVar1;
  longlong lVar2;
  
  lVar2 = (*(code *)PTR_lseek64_0045adb0)
                    (*param_1,param_2,(int)*param_2,*(undefined4 *)((int)param_2 + 4),param_3);
  uVar1 = (undefined4)lVar2;
  if (-1 < lVar2) {
    *param_2 = lVar2;
    uVar1 = 0;
  }
  return uVar1;
}



/* ---- _cs_write @ 0040229c ----
 * (library/support function; see decompiled body)
 */

void _cs_write(undefined4 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x004022ac. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_write_0045abd0)(*param_1);
  return;
}



/* ---- _cs_read @ 004022b4 ----
 * (library/support function; see decompiled body)
 */

void _cs_read(undefined4 *param_1)

{
                    /* WARNING: Could not recover jumptable at 0x004022c4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_read_0045abd4)(*param_1);
  return;
}



/* ---- vfprintf @ 004022d0 ----
 * (library/support function; see decompiled body)
 */

int vfprintf(FILE *__s,char *__format,__gnuc_va_list __arg)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined1 auStack_28 [16];
  
  iVar3 = *(int *)&__s->field_0x4c;
  if (iVar3 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
              (auStack_28,PTR___pthread_mutex_trylock_0045ad64,&__s->_offset);
    (*(code *)PTR___pthread_mutex_trylock_0045adcc)(&__s->_offset);
  }
  if ((__s->_flags & 0xc0) != 0xc0) {
    iVar1 = (*(code *)PTR___stdio_trans2w_o_0045abdc)(__s,0x80);
    iVar2 = -1;
    if (iVar1 != 0) goto LAB_00402388;
  }
  iVar2 = (*(code *)PTR__vfprintf_internal_0045abd8)(__s,__format,__arg);
LAB_00402388:
  if (iVar3 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_28,1);
  }
  return iVar2;
}



/* ---- FUN_004023d0 @ 004023d0 ----
 * (library/support function; see decompiled body)
 */

int FUN_004023d0(undefined4 param_1,byte param_2,int param_3,undefined4 param_4)

{
  bool bVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  byte local_28 [8];
  
  iVar6 = 0;
  iVar3 = param_3;
  if ((param_2 & 0x80) != 0) {
    iVar3 = (*(code *)PTR_strlen_0045aca0)(param_4);
    puVar2 = PTR___stdio_fwrite_0045abe0;
    iVar6 = param_3 - iVar3;
    if (iVar6 < 1) {
      iVar6 = 0;
    }
    else {
      local_28[0] = param_2 & 0x7f;
      iVar5 = iVar6;
      do {
        iVar4 = (*(code *)puVar2)(local_28,1,param_1);
        if (iVar4 != 1) {
          iVar5 = iVar6 - iVar5;
          bVar1 = iVar5 != iVar6;
          iVar6 = iVar5;
          if (bVar1) {
            return iVar5;
          }
          break;
        }
        iVar5 = iVar5 + -1;
      } while (iVar5 != 0);
    }
  }
  iVar5 = 0;
  if (0 < iVar3) {
    iVar5 = (*(code *)PTR___stdio_fwrite_0045abe0)(param_4,iVar3,param_1);
  }
  return iVar6 + iVar5;
}



/* ---- _vfprintf_internal @ 004024f4 ----
 * (library/support function; see decompiled body)
 */

undefined1 * _vfprintf_internal(undefined4 param_1,char *param_2,undefined4 param_3)

{
  bool bVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  int iVar6;
  int iVar7;
  char cVar8;
  undefined1 *puVar9;
  uint uVar10;
  char *pcVar11;
  char *pcVar12;
  undefined1 *puVar13;
  uint uVar14;
  char cVar15;
  undefined1 *puVar16;
  undefined1 local_180 [4];
  int local_17c [3];
  int *local_170 [5];
  char local_15c [127];
  undefined1 auStack_dd [5];
  char *local_d8;
  undefined1 *local_d4;
  undefined1 *local_d0;
  int local_cc;
  uint local_c8;
  char local_c4;
  int local_c0;
  int local_bc;
  uint local_b8;
  byte local_b2;
  undefined1 auStack_88 [72];
  char **local_40;
  undefined1 *local_3c;
  char *local_38;
  int *local_34;
  int *local_30;
  
  iVar3 = (*(code *)PTR__ppfs_init_0045abe4)(&local_d8);
  if (iVar3 < 0) {
    puVar16 = (undefined1 *)0xffffffff;
    iVar3 = (*(code *)PTR_strlen_0045aca0)(local_d8);
    if (iVar3 != 0) {
      (*(code *)PTR___stdio_fwrite_0045abe0)(local_d8,iVar3,param_1);
LAB_00402cc8:
      puVar16 = (undefined1 *)0xffffffff;
    }
  }
  else {
    (*(code *)PTR__ppfs_prepargs_0045abe8)(&local_d8,param_3);
    local_3c = local_180;
    local_38 = local_15c;
    local_34 = local_17c;
    local_30 = local_17c + 1;
    puVar16 = (undefined1 *)0x0;
    pcVar12 = param_2;
    local_40 = &local_d8;
    while( true ) {
      for (; (*pcVar12 != '\0' && (*pcVar12 != '%')); pcVar12 = pcVar12 + 1) {
      }
      iVar3 = (int)pcVar12 - (int)param_2;
      if (pcVar12 != param_2) {
        iVar4 = 0;
        if (0 < iVar3) {
          iVar4 = (*(code *)PTR___stdio_fwrite_0045abe0)(param_2,iVar3,param_1);
        }
        puVar16 = puVar16 + iVar4;
        if (iVar4 != iVar3) goto LAB_00402cc8;
      }
      if (*pcVar12 == '\0') break;
      pcVar11 = pcVar12 + 1;
      param_2 = pcVar12 + 1;
      pcVar12 = pcVar12 + 2;
      if (*pcVar11 != '%') {
        local_17c[0] = 0;
        local_d8 = param_2;
        (*(code *)PTR__ppfs_parsespec_0045abec)(local_40);
        (*(code *)PTR__ppfs_setargs_0045abf0)(local_40);
        puVar9 = auStack_88;
        if (0 < local_c0) {
          puVar9 = puVar9 + (1 - (uint)local_b2) * -8;
        }
        iVar3 = 0;
        while( true ) {
          iVar4 = iVar3 * 4;
          bVar1 = local_bc <= iVar3;
          iVar3 = iVar3 + 1;
          if (bVar1) break;
          *(undefined1 **)(local_3c + iVar4 + 0x10) = puVar9;
          puVar9 = puVar9 + 8;
        }
        if (local_b8 == 0) {
          (*(code *)PTR__store_inttype_0045abf4)
                    (*local_170[0],local_c8 & 0xf00,puVar16,(int)puVar16 >> 0x1f);
        }
        else {
          if (local_b8 < 8) {
            iVar3 = (int)(char)PTR_s__a__d__b__Y__T_0045abc0[local_b8 - 0x6415];
            if (local_b8 < 6) {
              uVar14 = 0xb;
              if (local_b8 == 1) {
                uVar14 = 6;
              }
            }
            else {
              iVar3 = -iVar3;
              uVar14 = 0xb;
            }
            cVar15 = ' ';
            if ((int)local_d4 < 0) {
              cVar15 = local_c4;
            }
            uVar10 = 0x400;
            if (local_b8 != 1) {
              uVar10 = (uint)local_40[10 - (1 - (uint)local_b2)] & 0xf00;
            }
            (*(code *)PTR__load_inttype_0045abf8)(uVar10,local_170[0],iVar3);
            pcVar12 = (char *)(*(code *)PTR__uintmaxtostr_0045abfc)(auStack_dd);
            if (5 < local_b8) {
              if (*pcVar12 == '-') {
                local_c8 = local_c8 | 2;
                pcVar12 = pcVar12 + 1;
                uVar14 = 2;
              }
              else if ((local_c8 & 2) == 0) {
                if ((local_c8 & 1) != 0) {
                  uVar14 = 4;
                }
              }
              else {
                uVar14 = 0;
              }
            }
            puVar9 = local_d4;
            if ((int)local_d4 < 0) {
              puVar9 = (undefined1 *)0x1;
            }
            puVar13 = auStack_dd + -(int)pcVar12;
            if ((local_c8 & 0x10) == 0) {
LAB_00402874:
              cVar8 = *pcVar12;
            }
            else {
              if (local_b8 < 3) {
                uVar14 = 6;
              }
              if (local_b8 == 3) {
                uVar14 = 9;
              }
              if (local_b8 == 4) {
                if (puVar9 <= puVar13) {
                  puVar9 = (undefined1 *)0x1;
                  if (*pcVar12 != '0') {
                    puVar9 = auStack_dd + -(int)pcVar12 + 1;
                  }
                  goto LAB_00402874;
                }
                cVar8 = *pcVar12;
              }
              else {
                cVar8 = *pcVar12;
              }
            }
            if (cVar8 == '0') {
              if (5 < uVar14) {
                uVar14 = 0xb;
              }
              if (local_b8 == 1) {
                puVar13 = (undefined1 *)0x5;
                pcVar12 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6430;
              }
              else {
                if (puVar9 != (undefined1 *)0x0) goto LAB_004028bc;
                puVar13 = (undefined1 *)0x0;
              }
              goto LAB_0040296c;
            }
LAB_004028bc:
            iVar3 = (int)puVar9 - (int)puVar13;
            if (puVar9 <= puVar13) {
              iVar3 = 0;
            }
          }
          else {
            if (local_b8 < 0x10) {
              iVar3 = (*(code *)PTR__fpmaxtostr_0045ac04)
                                (param_1,local_b8,(int)*(undefined8 *)local_170[0],
                                 (int)((ulonglong)*(undefined8 *)local_170[0] >> 0x20),&local_d4,
                                 PTR_Elf32_Ehdr_0045ac00 + 0x23d0);
LAB_00402a94:
              puVar16 = puVar16 + iVar3;
              if (-1 < iVar3) goto LAB_00402cc0;
              goto LAB_00402cc8;
            }
            if (local_b8 < 0x12) {
              local_17c[1] = 0;
              if (local_b8 == 0x11) {
                local_17c[0] = *local_170[0];
                if (local_17c[0] == 0) {
LAB_00402abc:
                  cVar15 = ' ';
                  uVar14 = 0xb;
                  iVar3 = 0;
                  puVar13 = (undefined1 *)0x6;
                  pcVar12 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6428;
                  goto LAB_00402ad0;
                }
                puVar9 = local_d4;
                if ((int)local_d4 < 0) {
                  puVar9 = (undefined1 *)0xffffffff;
                }
                puVar13 = (undefined1 *)
                          (*(code *)PTR_wcsrtombs_0045ad80)(local_34,local_34,puVar9,local_30);
                if (puVar13 == (undefined1 *)0xffffffff) goto LAB_00402cc8;
                pcVar12 = (char *)0x0;
              }
              else {
                puVar13 = (undefined1 *)
                          (*(code *)PTR_wcrtomb_0045ad98)(local_38,*local_170[0],local_30);
                if (puVar13 == (undefined1 *)0xffffffff) goto LAB_00402cc8;
                local_38[(int)puVar13] = '\0';
                pcVar12 = local_38;
              }
            }
            else {
              if (local_b8 < 0x14) {
                if (local_b8 != 0x13) {
                  local_15c[1] = 0;
                  local_15c[0] = (char)*local_170[0];
                  cVar15 = ' ';
                  uVar14 = 0xb;
                  iVar3 = 0;
                  puVar13 = (undefined1 *)0x1;
                  pcVar12 = local_38;
                  goto LAB_00402ad0;
                }
                pcVar12 = (char *)*local_170[0];
                if (pcVar12 == (char *)0x0) goto LAB_00402abc;
              }
              else {
                if (local_b8 != 0x14) {
                  pcVar11 = *(char **)PTR__custom_printf_spec_0045ac08;
                  cVar15 = *pcVar11;
                  pcVar12 = pcVar11;
                  while (cVar15 != local_cc) {
                    pcVar12 = pcVar12 + 1;
                    if (pcVar11 + 10 <= pcVar12) {
                      return (undefined1 *)0xffffffff;
                    }
                    cVar15 = *pcVar12;
                  }
                  iVar3 = (**(code **)(PTR__custom_printf_handler_0045ac0c +
                                      ((int)pcVar12 - (int)pcVar11) * 4))
                                    (param_1,&local_d4,local_170);
                  goto LAB_00402a94;
                }
                puVar5 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
                pcVar12 = (char *)(*(code *)PTR___glibc_strerror_r_0045addc)(*puVar5,local_38,0x80);
              }
              puVar9 = (undefined1 *)0xffffffff;
              if (-1 < (int)local_d4) {
                puVar9 = local_d4;
              }
              puVar13 = (undefined1 *)(*(code *)PTR_strnlen_0045aca8)(pcVar12,puVar9);
            }
            cVar15 = ' ';
            uVar14 = 0xb;
LAB_0040296c:
            iVar3 = 0;
          }
LAB_00402ad0:
          puVar9 = puVar13 + iVar3;
          if (uVar14 != 0xb) {
            iVar4 = 2;
            if (uVar14 < 6) {
              iVar4 = 1;
            }
            puVar9 = puVar9 + iVar4;
          }
          iVar4 = 0;
          if (puVar9 < local_d0) {
            iVar4 = (int)local_d0 - (int)puVar9;
          }
          puVar16 = puVar9 + (int)puVar16 + iVar4;
          if (cVar15 == '0') {
            iVar3 = iVar3 + iVar4;
            iVar4 = 0;
          }
          if ((local_c8 & 8) == 0) {
            local_180[0] = 0x20;
            for (iVar7 = iVar4; iVar7 != 0; iVar7 = iVar7 + -1) {
              iVar6 = (*(code *)PTR___stdio_fwrite_0045abe0)(local_3c,1,param_1);
              if (iVar6 != 1) {
                if (iVar4 - iVar7 != iVar4) {
                  return (undefined1 *)0xffffffff;
                }
                break;
              }
            }
            iVar4 = 0;
          }
          (*(code *)PTR_fputs_unlocked_0045ada8)
                    (PTR_s__a__d__b__Y__T_0045abc0 + (uVar14 - 0x6420),param_1);
          local_180[0] = 0x30;
          for (iVar7 = iVar3; iVar7 != 0; iVar7 = iVar7 + -1) {
            iVar6 = (*(code *)PTR___stdio_fwrite_0045abe0)(local_3c,1,param_1);
            if (iVar6 != 1) {
              if (iVar3 - iVar7 != iVar3) {
                return (undefined1 *)0xffffffff;
              }
              break;
            }
          }
          puVar2 = PTR_wcsrtombs_0045ad80;
          if (local_17c[0] == 0) {
            if ((puVar13 != (undefined1 *)0x0) &&
               (puVar9 = (undefined1 *)
                         (*(code *)PTR___stdio_fwrite_0045abe0)(pcVar12,puVar13,param_1),
               puVar9 != puVar13)) goto LAB_00402cc8;
          }
          else {
            local_17c[1] = 0;
            while( true ) {
              puVar9 = (undefined1 *)0x80;
              if (puVar13 < (undefined1 *)0x81) {
                puVar9 = puVar13;
              }
              if (puVar13 == (undefined1 *)0x0) break;
              iVar3 = (*(code *)puVar2)(local_38,local_34,puVar9,local_30);
              iVar7 = 0;
              if ((iVar3 != 0) &&
                 (iVar7 = (*(code *)PTR___stdio_fwrite_0045abe0)(local_38,iVar3,param_1),
                 iVar7 != iVar3)) goto LAB_00402cc8;
              puVar13 = puVar13 + -iVar7;
            }
          }
          local_180[0] = 0x20;
          for (iVar3 = iVar4; iVar3 != 0; iVar3 = iVar3 + -1) {
            iVar7 = (*(code *)PTR___stdio_fwrite_0045abe0)(local_3c,1,param_1);
            if (iVar7 != 1) {
              if (iVar4 - iVar3 != iVar4) {
                return (undefined1 *)0xffffffff;
              }
              break;
            }
          }
        }
LAB_00402cc0:
        param_2 = local_d8;
        pcVar12 = local_d8;
      }
    }
  }
  return puVar16;
}



/* ---- _ppfs_init @ 00402d00 ----
 * (library/support function; see decompiled body)
 */

undefined4 _ppfs_init(undefined4 *param_1,char *param_2)

{
  undefined *puVar1;
  int iVar2;
  undefined4 *puVar3;
  char *pcVar4;
  char *pcVar5;
  char *local_28;
  undefined4 local_24 [4];
  
  (*(code *)PTR_memset_0045ad90)(param_1,0,0x98);
  *param_1 = param_2;
  puVar1 = PTR___curlocale_0045ad3c;
  param_1[6] = param_1[6] + -1;
  iVar2 = (*(code *)puVar1)();
  if (*(char *)(iVar2 + 0x92c) != '\0') {
    local_24[0] = 0;
    local_28 = param_2;
    iVar2 = (*(code *)PTR_mbsrtowcs_0045ad38)(0,&local_28,0xffffffff,local_24);
    if (iVar2 == -1) {
      *param_1 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6400;
      return 0xffffffff;
    }
  }
  puVar3 = param_1 + 10;
  iVar2 = 9;
  do {
    iVar2 = iVar2 + -1;
    *puVar3 = 8;
    puVar1 = PTR__ppfs_parsespec_0045abec;
    puVar3 = puVar3 + 1;
  } while (iVar2 != 0);
  pcVar4 = param_2;
  while( true ) {
    while( true ) {
      do {
        pcVar5 = pcVar4;
        if (*pcVar5 == '\0') {
          *param_1 = param_2;
          return 0;
        }
        pcVar4 = pcVar5 + 1;
      } while (*pcVar5 != '%');
      if (*pcVar4 != '%') break;
      pcVar4 = pcVar5 + 2;
    }
    *param_1 = pcVar4;
    iVar2 = (*(code *)puVar1)(param_1);
    if (iVar2 < 0) break;
    pcVar4 = (char *)*param_1;
  }
  return 0xffffffff;
}



/* ---- _ppfs_prepargs @ 00402e40 ----
 * (library/support function; see decompiled body)
 */

void _ppfs_prepargs(int param_1,undefined4 param_2)

{
  undefined *puVar1;
  int iVar2;
  
  iVar2 = *(int *)(param_1 + 0x18);
  *(undefined4 *)(param_1 + 0x4c) = param_2;
  puVar1 = PTR__ppfs_setargs_0045abf0;
  if (0 < iVar2) {
    *(int *)(param_1 + 0x1c) = iVar2;
    *(undefined4 *)(param_1 + 0x18) = 0;
    *(undefined4 *)(param_1 + 4) = 0;
    *(undefined4 *)(param_1 + 8) = 0;
    (*(code *)puVar1)();
    *(int *)(param_1 + 0x18) = iVar2;
  }
  return;
}



/* ---- _ppfs_setargs @ 00402eb0 ----
 * (library/support function; see decompiled body)
 */

void _ppfs_setargs(int param_1)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined8 *puVar3;
  int iVar4;
  undefined8 *puVar5;
  int iVar6;
  
  puVar5 = (undefined8 *)(param_1 + 0x50);
  if (*(int *)(param_1 + 0x18) == 0) {
    if (*(int *)(param_1 + 8) == -0x80000000) {
      puVar1 = *(undefined4 **)(param_1 + 0x4c);
      *(undefined4 **)(param_1 + 0x4c) = puVar1 + 1;
      uVar2 = *puVar1;
      *(undefined4 *)(param_1 + 8) = uVar2;
      *(undefined4 *)(param_1 + 0x50) = uVar2;
      iVar4 = *(int *)(param_1 + 4);
    }
    else {
      iVar4 = *(int *)(param_1 + 4);
    }
    iVar6 = 0;
    if (iVar4 == -0x80000000) {
      puVar1 = *(undefined4 **)(param_1 + 0x4c);
      *(undefined4 **)(param_1 + 0x4c) = puVar1 + 1;
      uVar2 = *puVar1;
      *(undefined4 *)(param_1 + 4) = uVar2;
      *(undefined4 *)(param_1 + 0x50) = uVar2;
    }
    while (iVar6 < *(int *)(param_1 + 0x1c)) {
      iVar4 = *(int *)(param_1 + (iVar6 + 10) * 4);
      iVar6 = iVar6 + 1;
      if (iVar4 != 8) {
        puVar1 = *(undefined4 **)(param_1 + 0x4c);
        if (iVar4 < 9) {
          if (((iVar4 == 2) || (iVar4 < 3)) || (iVar4 != 7)) goto LAB_00402fd4;
LAB_00402fbc:
          puVar3 = (undefined8 *)((int)puVar1 + 7U & 0xfffffff8);
          *(undefined8 **)(param_1 + 0x4c) = puVar3 + 1;
          *puVar5 = *puVar3;
        }
        else {
          if ((iVar4 != 0x400) && (0x400 < iVar4)) {
            if (iVar4 == 0x800) {
              puVar1 = (undefined4 *)((int)puVar1 + 7U & 0xfffffff8);
              *(undefined4 **)(param_1 + 0x4c) = puVar1 + 2;
              uVar2 = *puVar1;
              *(undefined4 *)((int)puVar5 + 4) = puVar1[1];
              *(undefined4 *)puVar5 = uVar2;
              goto LAB_00402fe0;
            }
            if (iVar4 == 0x807) goto LAB_00402fbc;
          }
LAB_00402fd4:
          *(undefined4 **)(param_1 + 0x4c) = puVar1 + 1;
          *(undefined4 *)puVar5 = *puVar1;
        }
LAB_00402fe0:
        puVar5 = puVar5 + 1;
      }
    }
    iVar4 = *(int *)(param_1 + 8);
  }
  else {
    if (*(int *)(param_1 + 8) == -0x80000000) {
      *(undefined4 *)(param_1 + 8) =
           *(undefined4 *)(puVar5 + -(1 - (uint)*(byte *)(param_1 + 0x24)));
      iVar4 = *(int *)(param_1 + 4);
    }
    else {
      iVar4 = *(int *)(param_1 + 4);
    }
    if (iVar4 == -0x80000000) {
      *(undefined4 *)(param_1 + 4) =
           *(undefined4 *)(puVar5 + -(1 - (uint)*(byte *)(param_1 + 0x25)));
      iVar4 = *(int *)(param_1 + 8);
    }
    else {
      iVar4 = *(int *)(param_1 + 8);
    }
  }
  if (iVar4 < 0) {
    *(undefined4 *)(param_1 + 0x14) = 0x20;
    *(int *)(param_1 + 8) = -iVar4;
    *(uint *)(param_1 + 0x10) = *(uint *)(param_1 + 0x10) & 0xfffffffe | 8;
  }
  return;
}



/* ---- _ppfs_parsespec @ 00403090 ----
 * (library/support function; see decompiled body)
 */

int _ppfs_parsespec(int *param_1)

{
  byte *pbVar1;
  bool bVar2;
  undefined *puVar3;
  undefined *puVar4;
  char cVar5;
  char cVar6;
  int *piVar7;
  int iVar8;
  short *psVar9;
  uint uVar10;
  char *pcVar11;
  short *psVar12;
  int iVar13;
  short *psVar14;
  uint uVar15;
  char *pcVar16;
  char *pcVar17;
  uint uVar18;
  uint uVar19;
  int iVar20;
  uint uVar21;
  byte local_78 [8];
  uint local_70;
  int local_6c [7];
  char local_50 [32];
  byte *local_30;
  
  local_6c[1] = 8;
  local_6c[0] = 8;
  uVar18 = param_1[4] & 0x80;
  uVar19 = param_1[6];
  if (uVar18 == 0) {
    pcVar16 = (char *)*param_1;
  }
  else {
    iVar20 = 1;
    uVar21 = 0;
    do {
      uVar15 = uVar21 + 1;
      piVar7 = (int *)(iVar20 * -4 + *param_1);
      cVar5 = (char)*piVar7;
      local_50[uVar21] = cVar5;
      if ((int)cVar5 != *piVar7) {
        return -1;
      }
      if (cVar5 == 0) {
        local_50[0x1f] = 0;
        goto LAB_00403144;
      }
      iVar20 = 1 - uVar15;
      uVar21 = uVar15;
    } while (uVar15 < 0x20);
    local_50[0x1f] = 0;
LAB_00403144:
    local_50[0x1f] = 0;
    pcVar16 = local_50 + 1;
  }
  local_78[0] = 0;
  local_78[1] = 0;
  local_78[2] = 0;
  local_78[3] = 0;
  local_78[4] = 0;
  local_78[5] = 0;
  local_78[6] = 0;
  local_78[7] = 0;
  uVar21 = 0;
  iVar20 = 0;
  local_30 = local_78;
  do {
    pcVar17 = pcVar16;
    if (*pcVar16 == '*') {
      pcVar17 = pcVar16 + 1;
      pbVar1 = local_30 + (3 - iVar20) * 4;
      pbVar1[0] = 0;
      pbVar1[1] = 0;
      pbVar1[2] = 0;
      pbVar1[3] = 0;
    }
    uVar15 = 0;
    while( true ) {
      piVar7 = (int *)(*(code *)PTR___ctype_b_loc_0045adb4)();
      iVar8 = (int)*pcVar17;
      if ((*(ushort *)(*piVar7 + iVar8 * 2) & 8) == 0) break;
      pcVar17 = pcVar17 + 1;
      if ((int)uVar15 < 0xfff) {
        uVar15 = uVar15 * 10 + -0x30 + iVar8;
      }
    }
    if (pcVar16[-1] == '%') {
      if ((iVar8 == 0x24) && (0 < (int)uVar15)) {
        pcVar16 = pcVar17 + 1;
        if (uVar19 == 0) {
          return -1;
        }
        if ((int)uVar19 < (int)uVar15) {
          uVar19 = uVar15;
        }
        local_70 = uVar15;
LAB_0040321c:
        pcVar17 = pcVar16;
LAB_00403260:
        do {
          uVar15 = 1;
          pcVar16 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6378;
          while (cVar5 = *pcVar16, pcVar16 = pcVar16 + 1, *pcVar17 != cVar5) {
            uVar15 = uVar15 << 1;
            if (*pcVar16 == '\0') {
              uVar18 = uVar18 & ~((int)(uVar18 & 10) >> 1);
              uVar15 = 0;
              if (pcVar17[-1] != '%') goto LAB_00403168;
              cVar5 = *pcVar16;
              goto LAB_004032c0;
            }
          }
          uVar18 = uVar18 | uVar15;
          pcVar17 = pcVar17 + 1;
        } while( true );
      }
      if ((int)uVar19 < 1) {
        uVar19 = 0;
        if (pcVar17 <= pcVar16) goto LAB_0040321c;
        if (*pcVar16 == '0') {
          pcVar17 = pcVar16;
          goto LAB_00403260;
        }
      }
      else if (iVar8 != 0x6d) {
        return -1;
      }
    }
    cVar5 = *pcVar16;
LAB_004032c0:
    if (cVar5 == '*') {
      if (uVar19 == 0) {
        if (pcVar16 + 1 != pcVar17) {
          return -1;
        }
      }
      else {
        if (*pcVar17 != '$') {
          return -1;
        }
        if ((int)uVar15 < 1) {
          return -1;
        }
        pcVar17 = pcVar17 + 1;
        *(uint *)(local_30 + iVar20 * -4) = uVar15;
      }
      uVar15 = 0x80000000;
    }
    puVar3 = PTR_s__a__d__b__Y__T_0045abc0;
    if (iVar20 != 0) {
      goto LAB_00403340;
    }
    uVar21 = uVar15;
    if (*pcVar17 != '.') break;
    iVar20 = -1;
    pcVar17 = pcVar17 + 1;
LAB_00403168:
    pcVar16 = pcVar17;
  } while( true );
  uVar15 = 0xffffffff;
LAB_00403340:
  pcVar16 = PTR_s__a__d__b__Y__T_0045abc0 + -0x63d0;
  cVar5 = *pcVar16;
  while (*pcVar17 != cVar5) {
    pcVar16 = pcVar16 + 1;
    if (*pcVar16 == '\0') goto LAB_0040336c;
    cVar5 = *pcVar16;
  }
  pcVar17 = pcVar17 + 1;
LAB_0040336c:
  if ((int)pcVar16 - (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x63d0) < 2) {
    cVar5 = *pcVar17;
    if (cVar5 != *pcVar16) {
      cVar6 = pcVar16[9];
      goto LAB_004033a4;
    }
    pcVar17 = pcVar17 + 1;
    cVar6 = pcVar16[0x12];
  }
  else {
    cVar6 = pcVar16[9];
  }
  cVar5 = *pcVar17;
LAB_004033a4:
  uVar10 = (int)cVar6 << 8;
  if (cVar5 == '\0') {
    return -1;
  }
  pcVar16 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6390;
  cVar6 = *pcVar16;
  while (cVar6 != cVar5) {
    pcVar16 = pcVar16 + 1;
    cVar6 = *pcVar16;
    if (cVar6 == '\0') goto LAB_00403458;
  }
  iVar20 = (int)pcVar16 - (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x6390);
  if ((0x11 < iVar20) && ((uVar10 & 0x400) != 0)) {
    iVar20 = iVar20 + -2;
  }
  param_1[8] = iVar20;
  for (pcVar16 = puVar3 + -0x639c; *pcVar16 < iVar20; pcVar16 = pcVar16 + 1) {
  }
  iVar20 = ((int)pcVar16 - (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x639c)) * 2;
  local_6c[2] = ((int)*(short *)(PTR_s__a__d__b__Y__T_0045abc0 + iVar20 + -0x63ac) | uVar10) &
                (int)*(short *)(PTR_s__a__d__b__Y__T_0045abc0 + iVar20 + -0x63bc);
  pcVar16 = PTR_s__a__d__b__Y__T_0045abc0 + -0x6390;
LAB_00403458:
  iVar20 = 0x30;
  if ((uVar18 & 4) == 0) {
    iVar20 = 0x20;
  }
  param_1[3] = (int)*pcVar17;
  param_1[1] = uVar15;
  param_1[2] = uVar21;
  param_1[5] = iVar20;
  param_1[4] = uVar10 & 0xf00 | uVar18 & 0xfffffffb;
  param_1[7] = 1;
  if (*pcVar16 == '\0') {
    if (*pcVar17 == 'm') {
      param_1[8] = 0x14;
      param_1[7] = 0;
    }
    else {
      param_1[8] = 0x15;
      pcVar11 = *(char **)PTR__custom_printf_spec_0045ac08;
      cVar5 = *pcVar11;
      pcVar16 = pcVar11;
      while (cVar5 != *pcVar17) {
        pcVar16 = pcVar16 + 1;
        if (pcVar11 + 10 <= pcVar16) {
          return -1;
        }
        cVar5 = *pcVar16;
      }
      iVar20 = (**(code **)(PTR__custom_printf_arginfo_0045ac10 + ((int)pcVar16 - (int)pcVar11) * 4)
               )(param_1 + 1,5,local_6c + 2);
      param_1[7] = iVar20;
      if (5 < iVar20) {
        return -1;
      }
    }
  }
  puVar4 = PTR_memcpy_0045ad34;
  puVar3 = PTR_s__a__d__b__Y__T_0045abc0;
  if ((int)uVar19 < 1) {
    *(undefined1 *)((int)param_1 + 0x26) = 1;
    (*(code *)puVar4)(param_1 + 10,local_6c + 2,param_1[7] << 2);
    uVar18 = param_1[4];
    goto LAB_00403694;
  }
  iVar20 = 0;
  psVar14 = (short *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x6370);
  bVar2 = true;
  do {
    if (bVar2) {
      uVar18 = (uint)local_78[iVar20 * 4];
      *(byte *)((int)param_1 + iVar20 + 0x24) = local_78[iVar20 * 4];
    }
    else {
      uVar18 = (local_70 - 2) + iVar20;
    }
    if (((int)uVar19 < (int)uVar18) && (uVar19 = uVar18, 9 < (int)uVar18)) {
      return -1;
    }
    iVar13 = local_6c[iVar20];
    iVar8 = param_1[uVar18 + 9];
    if (iVar13 == 8) {
LAB_0040364c:
      iVar8 = param_1[7];
    }
    else {
      if ((iVar8 == 8) || (iVar8 == iVar13)) {
LAB_00403640:
        param_1[uVar18 + 9] = iVar13;
        goto LAB_0040364c;
      }
      for (psVar9 = (short *)(puVar3 + -0x635a); (*psVar9 != iVar8 && (psVar14 < psVar9));
          psVar9 = psVar9 + -1) {
      }
      for (psVar12 = (short *)(puVar3 + -0x635a); (*psVar12 != iVar13 && (psVar14 < psVar12));
          psVar12 = psVar12 + -1) {
      }
      if ((byte)puVar3[((int)psVar9 - (int)psVar14 >> 1) + -0x6358] <=
          (byte)puVar3[((int)psVar12 - (int)psVar14 >> 1) + -0x6358]) goto LAB_00403640;
      iVar8 = param_1[7];
    }
    iVar20 = iVar20 + 1;
    if (iVar8 + 1 < iVar20) {
      uVar18 = param_1[4];
LAB_00403694:
      param_1[6] = uVar19;
      if ((uVar18 & 0x80) == 0) {
        *param_1 = (int)(pcVar17 + 1);
      }
      else {
        *param_1 = *param_1 + ((int)pcVar17 - (int)local_50) * 4;
      }
      return param_1[7] + 2;
    }
    bVar2 = iVar20 < 3;
  } while( true );
}



/* ---- fwrite @ 00403710 ----
 * (library/support function; see decompiled body)
 */

size_t fwrite(void *__ptr,size_t __size,size_t __n,FILE *__s)

{
  size_t sVar1;
  int iVar2;
  undefined1 auStack_30 [20];
  
  iVar2 = *(int *)&__s->field_0x4c;
  if (iVar2 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
              (auStack_30,PTR___pthread_mutex_trylock_0045ad64,&__s->_offset);
    (*(code *)PTR___pthread_mutex_trylock_0045adcc)(&__s->_offset);
  }
  sVar1 = (*(code *)PTR_fwrite_unlocked_0045ad7c)(__ptr,__size,__n,__s);
  if (iVar2 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_30,1);
  }
  return sVar1;
}



/* ---- fputc_unlocked @ 004037f0 ----
 * (library/support function; see decompiled body)
 */

int fputc_unlocked(int __c,FILE *__stream)

{
  int iVar1;
  uint uVar2;
  char *pcVar3;
  char cVar4;
  char local_18 [12];
  
  pcVar3 = __stream->_IO_write_base;
  cVar4 = (char)__c;
  if (pcVar3 < __stream->_IO_buf_base) {
    *pcVar3 = cVar4;
    __stream->_IO_write_base = pcVar3 + 1;
    return __c & 0xff;
  }
  if ((__stream->_flags & 0xc0) == 0xc0) {
    pcVar3 = __stream->_IO_read_ptr;
LAB_00403870:
    if (pcVar3 == &SUB_fffffffe) {
      return __c & 0xff;
    }
    if (__stream->_IO_read_base == __stream->_IO_read_end) {
      local_18[0] = cVar4;
      iVar1 = (*(code *)PTR___stdio_WRITE_0045abc8)(__stream,local_18,1);
      if (iVar1 == 0) goto LAB_00403924;
    }
    else {
      pcVar3 = __stream->_IO_write_base;
      if (__stream->_IO_read_base == pcVar3) {
        iVar1 = (*(code *)PTR___stdio_wcommit_0045abc4)(__stream);
        if (iVar1 != 0) goto LAB_00403924;
        pcVar3 = __stream->_IO_write_base;
        *pcVar3 = cVar4;
      }
      else {
        *pcVar3 = cVar4;
      }
      __stream->_IO_write_base = pcVar3 + 1;
      if ((__stream->_flags & 0x100) != 0) {
        if ((__c & 0xffU) != 10) {
          return __c & 0xff;
        }
        iVar1 = (*(code *)PTR___stdio_wcommit_0045abc4)(__stream);
        if (iVar1 != 0) {
          __stream->_IO_write_base = __stream->_IO_write_base + -1;
          return -1;
        }
      }
    }
    uVar2 = __c & 0xff;
  }
  else {
    iVar1 = (*(code *)PTR___stdio_trans2w_o_0045abdc)(__stream,0x80);
    if (iVar1 == 0) {
      pcVar3 = __stream->_IO_read_ptr;
      goto LAB_00403870;
    }
LAB_00403924:
    uVar2 = 0xffffffff;
  }
  return uVar2;
}



/* ---- fputs_unlocked @ 00403940 ----
 * (library/support function; see decompiled body)
 */

int fputs_unlocked(char *__s,FILE *__stream)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = (*(code *)PTR_strlen_0045aca0)();
  iVar2 = (*(code *)PTR_fwrite_unlocked_0045ad7c)(__s,1,iVar1,__stream);
  iVar3 = -1;
  if (iVar2 == iVar1) {
    iVar3 = iVar2;
  }
  return iVar3;
}



/* ---- fwrite_unlocked @ 004039c0 ----
 * (library/support function; see decompiled body)
 */

size_t fwrite_unlocked(void *__ptr,size_t __size,size_t __n,FILE *__stream)

{
  undefined *puVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  
  if (((__stream->_flags & 0xc0) == 0xc0) ||
     (iVar2 = (*(code *)PTR___stdio_trans2w_o_0045abdc)(__stream,0x80), iVar2 == 0)) {
    puVar1 = PTR___GI___errno_location_0045acc8;
    if (__size == 0) {
      return 0;
    }
    if (__n != 0) {
      if (__size == 0) {
        trap(7);
      }
      if (__n <= 0xffffffff / __size) {
        uVar3 = (*(code *)PTR___stdio_fwrite_0045abe0)(__ptr,__n * __size,__stream);
        if (__size == 0) {
          trap(7);
        }
        return uVar3 / __size;
      }
      *(ushort *)&__stream->_flags = (ushort)__stream->_flags | 8;
      puVar4 = (undefined4 *)(*(code *)puVar1)();
      *puVar4 = 0x16;
    }
  }
  return 0;
}



/* ---- memcpy @ 00403ab0 ----
 * (library/support function; see decompiled body)
 */

void * memcpy(void *__dest,void *__src,size_t __n)

{
  undefined1 uVar1;
  uint uVar2;
  uint *puVar3;
  uint uVar4;
  undefined4 *puVar5;
  size_t sVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  int iVar10;
  uint uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  
  puVar5 = __dest;
  sVar6 = __n;
  if (7 < (int)__n) {
    if ((((uint)__src ^ (uint)__dest) & 3) == 0) {
      uVar11 = -(int)__src & 3;
      sVar6 = __n - uVar11;
      if (uVar11 != 0) {
        uVar2 = (uint)__src & 3;
        puVar3 = (uint *)((int)__src - uVar2);
        __src = (void *)((int)__src + uVar11);
        uVar4 = (uint)__dest & 3;
        *(uint *)((int)__dest - uVar4) =
             *(uint *)((int)__dest - uVar4) & 0xffffffffU >> (4 - uVar4) * 8 |
             (*puVar3 >> uVar2 * 8) << uVar4 * 8;
        puVar5 = (undefined4 *)((int)__dest + uVar11);
      }
      uVar11 = sVar6 & 0x1f;
      if (uVar11 != sVar6) {
        puVar7 = (undefined4 *)((sVar6 - uVar11) + (int)__src);
        puVar8 = puVar5;
        do {
          uVar9 = *(undefined4 *)__src;
          uVar12 = *(undefined4 *)((int)__src + 4);
          uVar13 = *(undefined4 *)((int)__src + 8);
          uVar14 = *(undefined4 *)((int)__src + 0xc);
          uVar15 = *(undefined4 *)((int)__src + 0x10);
          uVar16 = *(undefined4 *)((int)__src + 0x14);
          uVar17 = *(undefined4 *)((int)__src + 0x18);
          uVar18 = *(undefined4 *)((int)__src + 0x1c);
          puVar5 = puVar8 + 8;
          __src = (void *)((int)__src + 0x20);
          *puVar8 = uVar9;
          puVar8[1] = uVar12;
          puVar8[2] = uVar13;
          puVar8[3] = uVar14;
          puVar8[4] = uVar15;
          puVar8[5] = uVar16;
          puVar8[6] = uVar17;
          puVar8[7] = uVar18;
          puVar8 = puVar5;
          sVar6 = uVar11;
        } while (__src != puVar7);
      }
      uVar11 = sVar6 & 3;
      if (uVar11 != sVar6) {
        puVar7 = (undefined4 *)((sVar6 - uVar11) + (int)__src);
        puVar8 = puVar5;
        do {
          uVar9 = *(undefined4 *)__src;
          puVar5 = puVar8 + 1;
          __src = (void *)((int)__src + 4);
          *puVar8 = uVar9;
          puVar8 = puVar5;
          sVar6 = uVar11;
        } while (__src != puVar7);
      }
    }
    else {
      uVar11 = -(int)__dest & 3;
      puVar8 = __dest;
      if (uVar11 != 0) {
        iVar10 = *(int *)__src;
        __src = (void *)((int)__src + uVar11);
        uVar2 = (uint)__dest & 3;
        *(uint *)((int)__dest - uVar2) =
             *(uint *)((int)__dest - uVar2) & 0xffffffffU >> (4 - uVar2) * 8 | iVar10 << uVar2 * 8;
        puVar8 = (undefined4 *)((int)__dest + uVar11);
      }
      sVar6 = __n - uVar11 & 3;
      puVar7 = (undefined4 *)(((__n - uVar11) - sVar6) + (int)__src);
      do {
        uVar9 = *(undefined4 *)__src;
        puVar5 = puVar8 + 1;
        __src = (void *)((int)__src + 4);
        *puVar8 = uVar9;
        puVar8 = puVar5;
      } while (__src != puVar7);
    }
  }
  puVar8 = (undefined4 *)(sVar6 + (int)__src);
  if (0 < (int)sVar6) {
    do {
      uVar1 = *(undefined1 *)__src;
      __src = (void *)((int)__src + 1);
      *(undefined1 *)puVar5 = uVar1;
      puVar5 = (undefined4 *)((int)puVar5 + 1);
    } while (__src != puVar8);
  }
  return __dest;
}



/* ---- memset @ 00403bf0 ----
 * (library/support function; see decompiled body)
 */

void * memset(void *__s,int __c,size_t __n)

{
  bool bVar1;
  uint uVar2;
  uint *puVar3;
  uint *puVar4;
  uint uVar5;
  uint *puVar6;
  uint uVar7;
  
  puVar3 = __s;
  if (7 < (int)__n) {
    bVar1 = __c != 0;
    __c = __c & 0xff;
    if (bVar1) {
      uVar5 = __c | __c << 8;
      __c = uVar5 | uVar5 << 0x10;
    }
    uVar7 = -(int)__s & 3;
    uVar5 = __n - uVar7;
    if (uVar7 != 0) {
      uVar2 = (uint)__s & 3;
      *(uint *)((int)__s - uVar2) =
           *(uint *)((int)__s - uVar2) & 0xffffffffU >> (4 - uVar2) * 8 | __c << uVar2 * 8;
      puVar3 = (uint *)((int)__s + uVar7);
    }
    uVar7 = uVar5 & 7;
    if (uVar7 != uVar5) {
      puVar6 = (uint *)((uVar5 - uVar7) + (int)puVar3);
      puVar4 = puVar3;
      do {
        puVar3 = puVar4 + 2;
        *puVar4 = __c;
        puVar4[1] = __c;
        puVar4 = puVar3;
        uVar5 = uVar7;
      } while (puVar3 != puVar6);
    }
    __n = uVar5 - (uVar5 & 4);
    if ((uVar5 & 4) != 0) {
      *puVar3 = __c;
      puVar3 = puVar3 + 1;
    }
  }
  puVar4 = puVar3;
  if (0 < (int)__n) {
    do {
      puVar6 = (uint *)((int)puVar4 + 1);
      *(char *)puVar4 = (char)__c;
      puVar4 = puVar6;
    } while (puVar6 != (uint *)(__n + (int)puVar3));
  }
  return __s;
}



/* ---- strcmp @ 00403c80 ----
 * (library/support function; see decompiled body)
 */

int strcmp(char *__s1,char *__s2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  
  bVar1 = *__s1;
  while( true ) {
    uVar3 = (uint)bVar1;
    uVar2 = (uint)(byte)*__s2;
    __s1 = __s1 + 1;
    __s2 = __s2 + 1;
    if (uVar3 == 0) {
      return -uVar2;
    }
    if (uVar3 != uVar2) break;
    bVar1 = *__s1;
  }
  return uVar3 - uVar2;
}



/* ---- strlen @ 00403cb0 ----
 * (library/support function; see decompiled body)
 */

size_t strlen(char *__s)

{
  int iVar1;
  int *piVar2;
  int *piVar3;
  
  piVar3 = (int *)__s;
  while( true ) {
    if (((uint)piVar3 & 3) == 0) {
      do {
        iVar1 = *piVar3;
        piVar2 = piVar3;
        while (piVar3 = piVar2 + 1, (iVar1 + 0xfefefeffU & 0x80808080) != 0) {
          if ((char)*piVar2 == '\0') goto LAB_00403d08;
          if (*(char *)((int)piVar2 + 1) == '\0') {
            return (size_t)((int)piVar2 + (1 - (int)__s));
          }
          if (*(char *)((int)piVar2 + 2) == '\0') {
            return (size_t)((int)piVar2 + (2 - (int)__s));
          }
          if (*(char *)((int)piVar2 + 3) == '\0') {
            return (size_t)((int)piVar2 + (3 - (int)__s));
          }
          piVar2 = piVar3;
          iVar1 = *piVar3;
        }
      } while( true );
    }
    piVar2 = piVar3;
    if ((char)*piVar3 == '\0') break;
    piVar3 = (int *)((int)piVar3 + 1);
  }
LAB_00403d08:
  return (int)piVar2 - (int)__s;
}



/* ---- strnlen @ 00403d50 ----
 * (library/support function; see decompiled body)
 */

size_t strnlen(char *__string,size_t __maxlen)

{
  size_t sVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  
  sVar1 = 0;
  if (__maxlen != 0) {
    piVar2 = (int *)(__string + __maxlen);
    if (piVar2 < __string) {
      piVar2 = (int *)0xffffffff;
    }
    for (piVar3 = (int *)__string; ((uint)piVar3 & 3) != 0; piVar3 = (int *)((int)piVar3 + 1)) {
      if ((char)*piVar3 == '\0') {
        if (piVar3 <= piVar2) {
          piVar2 = piVar3;
        }
        return (int)piVar2 - (int)__string;
      }
    }
    piVar4 = piVar3;
    while ((piVar3 < piVar2 &&
           (((*piVar3 + 0xfefefeffU & 0x80808080) == 0 ||
            ((((piVar4 = piVar3, (char)*piVar3 != '\0' &&
               (piVar4 = (int *)((int)piVar3 + 1), *(char *)((int)piVar3 + 1) != '\0')) &&
              (piVar4 = (int *)((int)piVar3 + 2), *(char *)((int)piVar3 + 2) != '\0')) &&
             (piVar4 = (int *)((int)piVar3 + 3), *(char *)((int)piVar3 + 3) != '\0'))))))) {
      piVar3 = piVar3 + 1;
      piVar4 = piVar2;
    }
    if (piVar4 <= piVar2) {
      piVar2 = piVar4;
    }
    sVar1 = (int)piVar2 - (int)__string;
  }
  return sVar1;
}



/* ---- __glibc_strerror_r @ 00403e20 ----
 * (library/support function; see decompiled body)
 */

undefined4 __glibc_strerror_r(undefined4 param_1,undefined4 param_2)

{
  (*(code *)PTR_strerror_r_0045ad28)();
  return param_2;
}



/* ---- strerror_r @ 00403e60 ----
 * (library/support function; see decompiled body)
 */

char * strerror_r(int __errnum,char *__buf,size_t __buflen)

{
  int iVar1;
  uint uVar2;
  undefined4 *puVar3;
  uint uVar4;
  char *pcVar5;
  char *pcVar6;
  undefined1 auStack_1f [11];
  
  for (uVar4 = 0; iVar1 = uVar4 - 0x6340, pcVar5 = PTR__string_syserrmsgs_0045ac14, uVar4 < 0x7e;
      uVar4 = uVar4 + 1) {
    if ((uint)(byte)PTR_s__a__d__b__Y__T_0045abc0[iVar1] == __errnum) goto LAB_00403ee0;
  }
  if (__errnum == 0x46d) {
    uVar4 = 0x7a;
LAB_00403ee0:
    while (uVar4 != 0) {
      uVar4 = uVar4 - (*pcVar5 == '\0');
      pcVar5 = pcVar5 + 1;
    }
    pcVar6 = (char *)0x0;
    if (*pcVar5 != '\0') goto LAB_00403f38;
  }
  iVar1 = (*(code *)PTR__uintmaxtostr_0045abfc)
                    (auStack_1f,PTR_s__a__d__b__Y__T_0045abc0 + iVar1,__errnum,__errnum >> 0x1f,
                     0xfffffff6,0);
  pcVar5 = (char *)(iVar1 + -0xe);
  (*(code *)PTR_memcpy_0045ad34)(pcVar5,PTR_s__a__d__b__Y__T_0045abc0 + -0x62c0,0xe);
  pcVar6 = (char *)0x16;
LAB_00403f38:
  if (__buf == (char *)0x0) {
    __buflen = 0;
  }
  iVar1 = (*(code *)PTR_strlen_0045aca0)(pcVar5);
  uVar2 = iVar1 + 1;
  uVar4 = __buflen;
  if (uVar2 <= __buflen) {
    uVar4 = uVar2;
  }
  if (uVar2 > __buflen) {
    pcVar6 = (char *)0x22;
  }
  if (uVar4 != 0) {
    (*(code *)PTR_memcpy_0045ad34)(__buf,pcVar5,uVar4);
    __buf[uVar4 - 1] = '\0';
  }
  if (pcVar6 != (char *)0x0) {
    puVar3 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar3 = pcVar6;
  }
  return pcVar6;
}



/* ---- isatty @ 00403fc0 ----
 * (library/support function; see decompiled body)
 */

int isatty(int __fd)

{
  int iVar1;
  undefined1 auStack_40 [60];
  
  iVar1 = (*(code *)PTR_tcgetattr_0045ad88)(__fd,auStack_40);
  return (uint)(iVar1 == 0);
}



/* ---- tcgetattr @ 00404000 ----
 * (library/support function; see decompiled body)
 */

int tcgetattr(int __fd,termios *__termios_p)

{
  undefined *puVar1;
  int iVar2;
  undefined4 uVar3;
  tcflag_t local_38;
  tcflag_t local_34;
  tcflag_t local_30;
  tcflag_t local_2c;
  cc_t local_28;
  undefined1 auStack_27 [27];
  
  iVar2 = (*(code *)PTR_ioctl_0045ac18)(__fd,0x540d,&local_38);
  puVar1 = PTR_mempcpy_0045ad74;
  if (iVar2 == 0) {
    __termios_p->c_oflag = local_34;
    __termios_p->c_cflag = local_30;
    __termios_p->c_lflag = local_2c;
    __termios_p->c_line = local_28;
    __termios_p->c_iflag = local_38;
    uVar3 = (*(code *)puVar1)(__termios_p->c_cc,auStack_27,0x17);
    (*(code *)PTR_memset_0045ad90)(uVar3,0,9);
  }
  return iVar2;
}



/* ---- calloc @ 004040b0 ----
 * (library/support function; see decompiled body)
 */

void * calloc(size_t __nmemb,size_t __size)

{
  undefined4 *puVar1;
  uint uVar2;
  undefined1 auStack_18 [16];
  
  if (__nmemb != 0) {
    if (__nmemb == 0) {
      trap(7);
    }
    if (__size != (__size * __nmemb) / __nmemb) {
      puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
      *puVar1 = 0xc;
      return (void *)0x0;
    }
  }
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
            (auStack_18,PTR___pthread_mutex_trylock_0045ad64,PTR___malloc_lock_0045ad0c);
  (*(code *)PTR___pthread_mutex_trylock_0045adcc)(PTR___malloc_lock_0045ad0c);
  puVar1 = (undefined4 *)(*(code *)PTR_malloc_0045ada0)(__size * __nmemb);
  if ((puVar1 != (undefined4 *)0x0) && ((puVar1[-1] & 2) == 0)) {
    uVar2 = (puVar1[-1] & 0xfffffffc) - 4 >> 2;
    if (uVar2 < 10) {
      *puVar1 = 0;
      puVar1[1] = 0;
      puVar1[2] = 0;
      if (4 < uVar2) {
        puVar1[3] = 0;
        puVar1[4] = 0;
        if (6 < uVar2) {
          puVar1[5] = 0;
          puVar1[6] = 0;
          if (uVar2 == 9) {
            puVar1[7] = 0;
            puVar1[8] = 0;
          }
        }
      }
    }
    else {
      (*(code *)PTR_memset_0045ad90)(puVar1,0);
    }
  }
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_18,1);
  return puVar1;
}



/* ---- FUN_004041f0 @ 004041f0 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004041f0(int param_1,int param_2)

{
  undefined *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  
  puVar1 = PTR_sbrk_0045adb8;
  uVar4 = *(uint *)(param_2 + 0x35c);
  uVar5 = *(uint *)(*(int *)(param_2 + 0x2c) + 4) & 0xfffffffc;
  if (uVar4 == 0) {
    trap(7);
  }
  iVar3 = ((((uVar4 - 0x11) + uVar5) - param_1) / uVar4 - 1) * uVar4;
  if ((0 < iVar3) &&
     (iVar2 = (*(code *)PTR_sbrk_0045adb8)(0), iVar2 == *(int *)(param_2 + 0x2c) + uVar5)) {
    (*(code *)puVar1)(-iVar3);
    iVar3 = (*(code *)puVar1)(0);
    if (iVar3 != -1) {
      iVar2 = iVar2 - iVar3;
      if (iVar2 != 0) {
        *(int *)(param_2 + 0x368) = *(int *)(param_2 + 0x368) - iVar2;
        *(uint *)(*(int *)(param_2 + 0x2c) + 4) = uVar5 - iVar2 | 1;
        return 1;
      }
    }
  }
  return 0;
}



/* ---- __malloc_consolidate @ 004042f4 ----
 * (library/support function; see decompiled body)
 */

void __malloc_consolidate(uint *param_1)

{
  bool bVar1;
  undefined *puVar2;
  uint uVar3;
  int *piVar4;
  uint uVar5;
  int iVar6;
  int iVar7;
  int *piVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  uint *puVar12;
  uint uVar13;
  int *piVar14;
  
  uVar3 = *param_1;
  if (uVar3 == 0) {
    iVar6 = 1;
    do {
      iVar7 = iVar6 + 1;
      puVar12 = param_1 + 0xd + iVar6 * 2 + -2;
      puVar12[2] = (uint)puVar12;
      puVar12[3] = (uint)puVar12;
      puVar2 = PTR_sysconf_0045acf4;
      iVar6 = iVar7;
    } while (iVar7 < 0x60);
    param_1[0xd8] = param_1[0xd8] | 1;
    param_1[0xd5] = 0x10000;
    param_1[0xd1] = 0x40000;
    *param_1 = *param_1 & 3 | 0x48;
    param_1[0xb] = (uint)(param_1 + 0xd);
    param_1[0xd2] = 0;
    param_1[0xd3] = 0x40000;
    uVar3 = (*(code *)puVar2)(0x1e);
    param_1[0xd7] = uVar3;
    return;
  }
  *param_1 = uVar3 & 0xfffffffd;
  puVar12 = param_1 + 1;
  piVar8 = (int *)*puVar12;
  do {
    if (piVar8 != (int *)0x0) {
      *puVar12 = 0;
      do {
        piVar14 = (int *)piVar8[2];
        uVar11 = piVar8[1] & 0xfffffffe;
        uVar9 = (int)piVar8 + uVar11;
        uVar13 = *(uint *)(uVar9 + 4);
        if ((piVar8[1] & 1U) == 0) {
          iVar10 = *piVar8;
          piVar4 = (int *)((int)piVar8 - iVar10);
          iVar7 = piVar4[2];
          piVar8 = *(int **)(iVar7 + 0xc);
          iVar6 = piVar4[3];
          if ((piVar8 != piVar4) || (*(int **)(iVar6 + 8) != piVar8)) goto LAB_004043f4;
          *(int *)(iVar6 + 8) = iVar7;
          uVar11 = uVar11 + iVar10;
          *(int *)(iVar7 + 0xc) = iVar6;
        }
        uVar13 = uVar13 & 0xfffffffc;
        if (uVar9 == param_1[0xb]) {
          piVar8[1] = uVar11 + uVar13 | 1;
          param_1[0xb] = (uint)piVar8;
        }
        else {
          uVar5 = *(uint *)(uVar9 + uVar13 + 4);
          *(uint *)(uVar9 + 4) = uVar13;
          if ((uVar5 & 1) == 0) {
            iVar7 = *(int *)(uVar9 + 8);
            iVar6 = *(int *)(uVar9 + 0xc);
            if ((*(uint *)(iVar7 + 0xc) != uVar9) || (*(uint *)(iVar6 + 8) != uVar9)) {
LAB_004043f4:
              (*(code *)PTR_abort_0045ad00)();
              return;
            }
            *(int *)(iVar6 + 8) = iVar7;
            uVar11 = uVar11 + uVar13;
            *(int *)(iVar7 + 0xc) = iVar6;
          }
          uVar13 = param_1[0xf];
          param_1[0xf] = (uint)piVar8;
          *(int **)(uVar13 + 0xc) = piVar8;
          *(uint *)((int)piVar8 + uVar11) = uVar11;
          piVar8[1] = uVar11 | 1;
          piVar8[2] = uVar13;
          piVar8[3] = (int)(param_1 + 0xd);
        }
        piVar8 = piVar14;
      } while (piVar14 != (int *)0x0);
    }
    bVar1 = puVar12 == param_1 + ((uVar3 >> 3) - 1);
    puVar12 = puVar12 + 1;
    if (bVar1) {
      return;
    }
    piVar8 = (int *)*puVar12;
  } while( true );
}



/* ---- free @ 004044d0 ----
 * (library/support function; see decompiled body)
 */

void free(void *__ptr)

{
  int *piVar1;
  undefined *puVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  int iVar12;
  code *pcVar13;
  undefined1 auStack_18 [16];
  
  if (__ptr == (void *)0x0) {
    return;
  }
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
            (auStack_18,PTR___pthread_mutex_trylock_0045ad64,PTR___malloc_lock_0045ad0c);
  (*(code *)PTR___pthread_mutex_trylock_0045adcc)(PTR___malloc_lock_0045ad0c);
  puVar2 = PTR___malloc_state_0045ad70;
  pcVar13 = (code *)PTR_munmap_0045ad14;
  iVar9 = (int)__ptr + -8;
  uVar11 = *(uint *)((int)__ptr + -4);
  uVar10 = uVar11 & 0xfffffffc;
  uVar5 = *(uint *)PTR___malloc_state_0045ad70;
  if (uVar10 <= uVar5) {
    iVar3 = (uVar11 >> 3) - 2;
    uVar7 = *(undefined4 *)(PTR___malloc_state_0045ad70 + iVar3 * 4 + 4);
    *(uint *)PTR___malloc_state_0045ad70 = uVar5 | 3;
    *(undefined4 *)__ptr = uVar7;
    *(int *)(puVar2 + iVar3 * 4 + 4) = iVar9;
    goto LAB_004046f0;
  }
  if ((uVar11 & 2) == 0) {
    *(uint *)PTR___malloc_state_0045ad70 = uVar5 | 1;
    iVar3 = iVar9 + uVar10;
    uVar5 = *(uint *)(iVar3 + 4);
    if ((uVar11 & 1) == 0) {
      iVar8 = *(int *)((int)__ptr + -8);
      iVar4 = iVar9 - iVar8;
      iVar12 = *(int *)(iVar4 + 8);
      iVar9 = *(int *)(iVar12 + 0xc);
      iVar6 = *(int *)(iVar4 + 0xc);
      if ((iVar9 != iVar4) || (*(int *)(iVar6 + 8) != iVar9)) goto LAB_00404600;
      *(int *)(iVar6 + 8) = iVar12;
      uVar10 = uVar10 + iVar8;
      *(int *)(iVar12 + 0xc) = iVar6;
    }
    puVar2 = PTR___malloc_state_0045ad70;
    uVar5 = uVar5 & 0xfffffffc;
    if (iVar3 == *(int *)(PTR___malloc_state_0045ad70 + 0x2c)) {
      uVar10 = uVar10 + uVar5;
      *(uint *)(iVar9 + 4) = uVar10 | 1;
      *(int *)(puVar2 + 0x2c) = iVar9;
    }
    else {
      uVar11 = *(uint *)(iVar3 + uVar5 + 4);
      *(uint *)(iVar3 + 4) = uVar5;
      if ((uVar11 & 1) == 0) {
        iVar6 = *(int *)(iVar3 + 8);
        iVar4 = *(int *)(iVar3 + 0xc);
        if ((*(int *)(iVar6 + 0xc) != iVar3) || (*(int *)(iVar4 + 8) != iVar3)) {
LAB_00404600:
          (*(code *)PTR_abort_0045ad00)();
          return;
        }
        *(int *)(iVar4 + 8) = iVar6;
        uVar10 = uVar10 + uVar5;
        *(int *)(iVar6 + 0xc) = iVar4;
      }
      puVar2 = PTR___malloc_state_0045ad70;
      iVar3 = *(int *)(PTR___malloc_state_0045ad70 + 0x3c);
      *(undefined **)(iVar9 + 0xc) = PTR___malloc_state_0045ad70 + 0x34;
      *(uint *)(iVar9 + 4) = uVar10 | 1;
      *(int *)(iVar9 + 8) = iVar3;
      *(uint *)(iVar9 + uVar10) = uVar10;
      *(int *)(iVar3 + 0xc) = iVar9;
      *(int *)(puVar2 + 0x3c) = iVar9;
    }
    if (uVar10 < 0x20000) goto LAB_004046f0;
    if ((*(uint *)PTR___malloc_state_0045ad70 & 2) != 0) {
      (*(code *)PTR___malloc_consolidate_0045ac1c)();
    }
    if ((*(uint *)(*(int *)(PTR___malloc_state_0045ad70 + 0x2c) + 4) & 0xfffffffc) <
        *(uint *)(PTR___malloc_state_0045ad70 + 0x344)) goto LAB_004046f0;
    iVar9 = *(int *)(PTR___malloc_state_0045ad70 + 0x348);
    pcVar13 = (code *)(PTR_Elf32_Ehdr_0045ac00 + 0x41f0);
  }
  else {
    piVar1 = (int *)(PTR___malloc_state_0045ad70 + 0x350);
    iVar9 = iVar9 - *(int *)((int)__ptr + -8);
    *(uint *)(PTR___malloc_state_0045ad70 + 0x364) =
         *(int *)(PTR___malloc_state_0045ad70 + 0x364) - (uVar10 + *(int *)((int)__ptr + -8));
    *(int *)(puVar2 + 0x350) = *piVar1 + -1;
  }
  (*pcVar13)(iVar9);
LAB_004046f0:
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_18,1);
  return;
}



/* ---- malloc_trim @ 00404710 ----
 * (library/support function; see decompiled body)
 */

void malloc_trim(undefined4 param_1)

{
  (*(code *)PTR___malloc_consolidate_0045ac1c)(PTR___malloc_state_0045ad70);
                    /* WARNING: Could not recover jumptable at 0x0040475c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(PTR_Elf32_Ehdr_0045ac00 + 0x41f0))(param_1,PTR___malloc_state_0045ad70);
  return;
}



/* ---- abort @ 00404770 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void abort(void)

{
  undefined *puVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 auStack_11c [33];
  undefined4 local_98 [36];
  
  puVar3 = auStack_11c + 1;
  (*(code *)PTR___pthread_mutex_trylock_0045adcc)(PTR_0045abbc + -0x5510);
  iVar2 = 0x20;
  while( true ) {
    iVar2 = iVar2 + -1;
    if (iVar2 < 0) break;
    puVar3[iVar2] = 0;
  }
  iVar2 = (*(code *)PTR___sigaddset_0045ad6c)(puVar3,6);
  if (iVar2 == 0) {
    (*(code *)PTR_sigprocmask_0045ad50)(2,puVar3,0);
  }
  if (*(int *)(PTR_0045abbc + -0x31b0) != 0) goto LAB_00404840;
  *(undefined4 *)(PTR_0045abbc + -0x31b0) = 1;
  while( true ) {
    puVar1 = PTR_0045abbc;
    (*(code *)PTR___pthread_mutex_trylock_0045ad64)(PTR_0045abbc + -0x5510);
    (*(code *)PTR_raise_0045acac)(6);
    (*(code *)PTR___pthread_mutex_trylock_0045adcc)(puVar1 + -0x5510);
LAB_00404840:
    puVar1 = PTR_memset_0045ad90;
    if (*(int *)(PTR_0045abbc + -0x31b0) != 1) break;
    *(undefined4 *)(PTR_0045abbc + -0x31b0) = 2;
    (*(code *)puVar1)(local_98,0,0x90);
    iVar2 = 0x20;
    while( true ) {
      if (iVar2 + -1 < 0) break;
      local_98[iVar2 + 1] = 0xffffffff;
      iVar2 = iVar2 + -1;
    }
    local_98[0] = 0;
    local_98[1] = 0;
    (*(code *)PTR_sigaction_0045add8)(6,local_98,0);
  }
  if (*(int *)(PTR_0045abbc + -0x31b0) == 2) {
    *(undefined4 *)(PTR_0045abbc + -0x31b0) = 3;
    trap(0x3fc00);
  }
  puVar1 = PTR__exit_0045acfc;
  if (*(int *)(PTR_0045abbc + -0x31b0) == 3) {
    *(undefined4 *)(PTR_0045abbc + -0x31b0) = 4;
    (*(code *)puVar1)(0x7f);
    return;
  }
  do {
    trap(0x3fc00);
  } while( true );
}



/* ---- atoi @ 00404900 ----
 * (library/support function; see decompiled body)
 */

int atoi(char *__nptr)

{
  int iVar1;
  
                    /* WARNING: Could not recover jumptable at 0x00404914. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  iVar1 = (*(code *)PTR_strtol_0045acb8)(__nptr,0,10);
  return iVar1;
}



/* ---- strtol @ 00404920 ----
 * (library/support function; see decompiled body)
 */

long strtol(char *__nptr,char **__endptr,int __base)

{
  long lVar1;
  
                    /* WARNING: Could not recover jumptable at 0x00404930. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  lVar1 = (*(code *)PTR__stdlib_strto_l_0045ac20)();
  return lVar1;
}



/* ---- _stdlib_strto_l @ 00404940 ----
 * (library/support function; see decompiled body)
 */

void _stdlib_strto_l(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  undefined4 uVar1;
  
  uVar1 = (*(code *)PTR___curlocale_0045ad3c)();
  (*(code *)PTR__stdlib_strto_l_l_0045ac24)(param_1,param_2,param_3,param_4,uVar1);
  return;
}



/* ---- _stdlib_strto_l_l @ 004049c0 ----
 * (library/support function; see decompiled body)
 */

uint _stdlib_strto_l_l(byte *param_1,undefined4 *param_2,uint param_3,uint param_4,int *param_5)

{
  undefined4 *puVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  byte *pbVar7;
  uint uVar8;
  
  pbVar6 = param_1;
  while (iVar4 = (int)(char)*pbVar6, (*(ushort *)(*param_5 + iVar4 * 2) & 0x20) != 0) {
    pbVar6 = pbVar6 + 1;
  }
  if (iVar4 == 0x2b) {
    uVar8 = 0;
  }
  else {
    uVar8 = 0;
    if (iVar4 != 0x2d) goto LAB_00404a50;
    uVar8 = 1;
  }
  pbVar6 = pbVar6 + 1;
LAB_00404a50:
  pbVar7 = pbVar6;
  if ((param_3 & 0xffffffef) == 0) {
    uVar5 = param_3 + 10;
    if (*pbVar6 == 0x30) {
      param_1 = pbVar6 + 1;
      pbVar7 = param_1;
      uVar5 = param_3 + 8;
      if ((*param_1 | 0x20) == 0x78) {
        pbVar7 = pbVar6 + 2;
        uVar5 = (param_3 + 8) * 2;
      }
    }
    param_3 = uVar5;
    if (0x10 < (int)param_3) {
      param_3 = 0x10;
    }
  }
  uVar5 = 0;
  if (param_3 - 2 < 0x23) {
    if (param_3 == 0) {
      trap(7);
    }
    while( true ) {
      bVar2 = *pbVar7;
      pbVar7 = pbVar7 + 1;
      uVar3 = (int)(char)bVar2 - 0x30U & 0xff;
      bVar2 = bVar2 | 0x20;
      if (9 < uVar3) {
        uVar3 = 0x28;
        if (0x60 < bVar2) {
          uVar3 = (uint)(byte)(bVar2 + 0xa9);
        }
      }
      if ((int)param_3 <= (int)uVar3) break;
      if ((0xffffffff / param_3 < uVar5) ||
         ((uVar5 == 0xffffffff / param_3 && ((0xffffffffU % param_3 & 0xff) < uVar3)))) {
        uVar8 = uVar8 & param_4;
        puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
        *puVar1 = 0x22;
        uVar5 = 0xffffffff;
      }
      else {
        uVar5 = uVar5 * param_3 + uVar3;
      }
      param_1 = pbVar7;
    }
  }
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = param_1;
  }
  uVar3 = 0x80000000;
  if (uVar8 == 0) {
    uVar3 = 0x7fffffff;
  }
  if ((param_4 != 0) && (uVar3 < uVar5)) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0x22;
    uVar5 = uVar3;
  }
  uVar3 = -uVar5;
  if (uVar8 == 0) {
    uVar3 = uVar5;
  }
  return uVar3;
}



/* ---- sysconf @ 00404be0 ----
 * (library/support function; see decompiled body)
 */

long sysconf(int __name)

{
  long lVar1;
  undefined4 *puVar2;
  
  if ((uint)__name < 0x96) {
                    /* WARNING: Could not emulate address calculation at 0x00404c10 */
                    /* WARNING: Treating indirect jump as call */
    lVar1 = (*(code *)(&_gp + *(int *)(PTR_s__a__d__b__Y__T_0045abc0 + __name * 4 + -0x5730)))();
    return lVar1;
  }
  puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
  *puVar2 = 0x16;
  return -1;
}



/* ---- __uClibc_fini @ 00404d70 ----
 * (library/support function; see decompiled body)
 */

void __uClibc_fini(void)

{
  undefined *puVar1;
  int iVar2;
  
  puVar1 = PTR___CTOR_LIST___0045ac28;
  iVar2 = 0;
  while (iVar2 != 0) {
    iVar2 = iVar2 + -1;
    (**(code **)(puVar1 + iVar2 * 4))();
  }
  if (*(code **)PTR___app_fini_0045ac2c != (code *)0x0) {
    (**(code **)PTR___app_fini_0045ac2c)();
  }
  if (*(code **)PTR___rtld_fini_0045ac30 == (code *)0x0) {
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x00404dfc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)PTR___rtld_fini_0045ac30)();
  return;
}



/* ---- __pthread_mutex_trylock @ 00404e14 ----
 * (library/support function; see decompiled body)
 */

undefined4 __pthread_mutex_trylock(void)

{
  return 0;
}



/* ---- _pthread_cleanup_push_defer @ 00404e1c ----
 * (library/support function; see decompiled body)
 */

void _pthread_cleanup_push_defer(void)

{
  return;
}



/* ---- FUN_00404e24 @ 00404e24 ----
 * (library/support function; see decompiled body)
 */

void FUN_00404e24(int param_1,undefined4 param_2)

{
  int iVar1;
  
  iVar1 = (*(code *)PTR_fcntl_0045ad94)(param_1,1);
  if (iVar1 == -1) {
    iVar1 = (*(code *)PTR_open_0045abb8)(PTR_s__a__d__b__Y__T_0045abc0 + -0x54d0,param_2);
    if (iVar1 != param_1) {
      (*(code *)PTR_abort_0045ad00)();
      return;
    }
  }
  return;
}



/* ---- __uClibc_init @ 00404ea0 ----
 * (library/support function; see decompiled body)
 */

void __uClibc_init(void)

{
  if (*(int *)(PTR_0045abbc + -0x318c) == 0) {
    *(undefined4 *)(PTR_0045abbc + -0x318c) = 1;
    *(undefined4 *)PTR___pagesize_0045acd8 = 0x1000;
    if (PTR_0045ad44 != (undefined *)0x0) {
      (*(code *)PTR_0045ad44)();
    }
    if (PTR__locale_init_0045ac34 != (undefined *)0x0) {
      (*(code *)PTR__locale_init_0045ac34)();
    }
    if (PTR__stdio_init_0045ac38 != (undefined *)0x0) {
      (*(code *)PTR__stdio_init_0045ac38)();
      return;
    }
  }
  return;
}



/* ---- __uClibc_main @ 00404f28 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00405174) */
/* WARNING: Removing unreachable block (ram,0x004051c0) */

void __uClibc_main(code *param_1,int param_2,int *param_3,code *param_4,undefined4 param_5,
                  undefined4 param_6,undefined4 param_7)

{
  undefined *puVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  uint *puVar7;
  code *pcVar8;
  undefined1 auStack_98 [52];
  int local_64;
  int local_3c;
  int local_34;
  int local_2c;
  int local_24;
  
  puVar1 = PTR_environ_0045ad84;
  *(undefined4 *)PTR___libc_stack_end_0045adc0 = param_7;
  *(undefined4 *)PTR___rtld_fini_0045ac30 = param_6;
  *(int **)puVar1 = param_3 + param_2 + 1;
  if (param_3 + param_2 + 1 == (int *)*param_3) {
    *(int **)puVar1 = param_3 + param_2;
  }
  (*(code *)PTR_memset_0045ad90)(auStack_98,0,0x78);
  puVar1 = PTR_memcpy_0045ad34;
  puVar7 = *(uint **)PTR_environ_0045ad84;
  do {
    uVar2 = *puVar7;
    puVar7 = puVar7 + 1;
  } while (uVar2 != 0);
  while( true ) {
    uVar2 = *puVar7;
    if (uVar2 == 0) break;
    if (uVar2 < 0xf) {
      (*(code *)puVar1)(auStack_98 + uVar2 * 8,puVar7,8);
      puVar7 = puVar7 + 2;
    }
    else {
      puVar7 = puVar7 + 2;
    }
  }
  (*(code *)PTR___uClibc_init_0045ac3c)();
  if (local_64 == 0) {
    local_64 = 0x1000;
  }
  *(int *)PTR___pagesize_0045acd8 = local_64;
  if (local_3c == -1) {
    iVar3 = (*(code *)PTR_getuid_0045acb4)();
    iVar4 = (*(code *)PTR_geteuid_0045acc4)();
    if (iVar3 == iVar4) {
      iVar3 = (*(code *)PTR_getgid_0045ad78)();
      iVar4 = (*(code *)PTR_getegid_0045ad10)();
      if (iVar3 == iVar4) goto LAB_004050a4;
    }
LAB_004050cc:
    pcVar8 = (code *)(PTR_Elf32_Ehdr_0045ac00 + 0x4e24);
    (*pcVar8)(0,0x20000);
    (*pcVar8)(1,0x20002);
    (*pcVar8)(2,0x20002);
  }
  else {
LAB_004050a4:
    if (local_3c != -1) {
      if ((local_3c == local_34) && (local_2c == local_24)) {
        iVar3 = *param_3;
        goto LAB_0040510c;
      }
      goto LAB_004050cc;
    }
  }
  iVar3 = *param_3;
LAB_0040510c:
  *(int *)PTR___uclibc_progname_0045ac40 = iVar3;
  puVar1 = PTR_rindex_0045acf8;
  iVar3 = *param_3;
  if (iVar3 != 0) {
    *(int *)PTR_program_invocation_name_0045acbc = iVar3;
    iVar4 = (*(code *)puVar1)(*param_3,0x2f);
    if (iVar4 == 0) {
      *(int *)PTR_program_invocation_short_name_0045ad30 = iVar3;
    }
    else {
      *(int *)PTR_program_invocation_short_name_0045ad30 = iVar4 + 1;
    }
  }
  *(undefined4 *)PTR___app_fini_0045ac2c = param_5;
  if (param_4 != (code *)0x0) {
    (*param_4)();
  }
  if (PTR___GI___errno_location_0045acc8 != (undefined *)0x0) {
    puVar5 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar5 = 0;
  }
  if (PTR_0045acf0 != (undefined *)0x0) {
    puVar5 = (undefined4 *)(*(code *)PTR_0045acf0)();
    *puVar5 = 0;
  }
  uVar6 = (*param_1)(param_2,param_3,*(undefined4 *)PTR_environ_0045ad84);
  (*(code *)PTR_exit_0045ad60)(uVar6);
  return;
}



/* ---- sigaction @ 00405240 ----
 * (library/support function; see decompiled body)
 */

int sigaction(int __sig,sigaction *__act,sigaction *__oact)

{
  undefined *puVar1;
  int iVar2;
  __sighandler_t *pp_Var3;
  __sighandler_t *pp_Var4;
  __sighandler_t local_50;
  ulong local_4c;
  undefined1 auStack_48 [16];
  _func_4849 *local_38;
  __sighandler_t local_30;
  ulong local_2c;
  undefined1 auStack_28 [16];
  _func_4849 *local_18;
  
  pp_Var3 = (__sighandler_t *)0x0;
  if (__act != (sigaction *)0x0) {
    local_4c = (__act->sa_mask).__val[0];
    (*(code *)PTR_memcpy_0045ad34)(auStack_48,(__act->sa_mask).__val + 1,0x10);
    local_38 = __act->sa_restorer;
    local_50 = (__act->__sigaction_handler).sa_handler;
    pp_Var3 = &local_50;
  }
  pp_Var4 = &local_30;
  if (__oact == (sigaction *)0x0) {
    pp_Var4 = (__sighandler_t *)0x0;
  }
  iVar2 = (*(code *)PTR___syscall_rt_sigaction_0045ac44)(__sig,pp_Var3,pp_Var4,0x10);
  puVar1 = PTR_memcpy_0045ad34;
  if ((__oact != (sigaction *)0x0) && (-1 < iVar2)) {
    (__oact->sa_mask).__val[0] = local_2c;
    (*(code *)puVar1)((__oact->sa_mask).__val + 1,auStack_28,0x80);
    __oact->sa_restorer = local_18;
    (__oact->__sigaction_handler).sa_handler = local_30;
  }
  return iVar2;
}



/* ---- fcntl @ 00405320 ----
 * (library/support function; see decompiled body)
 */

int fcntl(int __fd,int __cmd,...)

{
  int iVar1;
  undefined4 *puVar2;
  int in_a3;
  
  if (__cmd - 0x21U < 3) {
    iVar1 = (*(code *)PTR_fcntl64_0045ad18)();
  }
  else {
    syscall(0);
    iVar1 = 0xfd7;
    if (in_a3 != 0) {
      puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
      *puVar2 = 0xfd7;
      iVar1 = -1;
    }
  }
  return iVar1;
}



/* ---- fcntl64 @ 004053b0 ----
 * (library/support function; see decompiled body)
 */

undefined4 fcntl64(void)

{
  undefined4 *puVar1;
  int in_a3;
  undefined4 uVar2;
  
  syscall(0);
  uVar2 = 0x107c;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0x107c;
    uVar2 = 0xffffffff;
  }
  return uVar2;
}



/* ---- __syscall_rt_sigaction @ 00405420 ----
 * (library/support function; see decompiled body)
 */

undefined4 __syscall_rt_sigaction(void)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int in_a3;
  
  uVar1 = 0x1062;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0x1062;
    uVar1 = 0xffffffff;
  }
  return uVar1;
}



/* ---- _exit @ 00405480 ----
 * (library/support function; see decompiled body)
 */

void _exit(int __status)

{
  undefined4 *puVar1;
  int in_a3;
  
  do {
    do {
      syscall(0);
    } while (in_a3 == 0);
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)(__status);
    *puVar1 = 0xfa1;
  } while( true );
}



/* ---- clock_getres @ 004054d0 ----
 * (library/support function; see decompiled body)
 */

int clock_getres(clockid_t __clock_id,timespec *__res)

{
  int iVar1;
  undefined4 *puVar2;
  int in_a3;
  
  iVar1 = 0x10a8;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0x10a8;
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- getdtablesize @ 00405530 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int getdtablesize(void)

{
  int iVar1;
  int iVar2;
  int local_10 [3];
  
  iVar1 = (*(code *)PTR_getrlimit_0045ad20)(5,local_10);
  iVar2 = 0x100;
  if (-1 < iVar1) {
    iVar2 = local_10[0];
  }
  return iVar2;
}



/* ---- getegid @ 00405580 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

__gid_t getegid(void)

{
  undefined4 *puVar1;
  int in_a3;
  __gid_t _Var2;
  
  syscall(0);
  _Var2 = 0xfd2;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0xfd2;
    _Var2 = 0xffffffff;
  }
  return _Var2;
}



/* ---- geteuid @ 004055e0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

__uid_t geteuid(void)

{
  undefined4 *puVar1;
  int in_a3;
  __uid_t _Var2;
  
  syscall(0);
  _Var2 = 0xfd1;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0xfd1;
    _Var2 = 0xffffffff;
  }
  return _Var2;
}



/* ---- getgid @ 00405640 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

__gid_t getgid(void)

{
  __gid_t _Var1;
  undefined4 *puVar2;
  int in_a3;
  
  _Var1 = 0xfcf;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfcf;
    _Var1 = 0xffffffff;
  }
  return _Var1;
}



/* ---- getpagesize @ 004056a0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int getpagesize(void)

{
  int iVar1;
  
  iVar1 = 0x10000;
  if (*(int *)PTR___pagesize_0045acd8 != 0) {
    iVar1 = *(int *)PTR___pagesize_0045acd8;
  }
  return iVar1;
}



/* ---- getrlimit @ 004056c0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Enum "__rlimit_resource": Some values do not have unique names */

int getrlimit(__rlimit_resource_t __resource,rlimit *__rlimits)

{
  int iVar1;
  undefined4 *puVar2;
  int in_a3;
  
  iVar1 = 0xfec;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfec;
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- getuid @ 00405720 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

__uid_t getuid(void)

{
  __uid_t _Var1;
  undefined4 *puVar2;
  int in_a3;
  
  _Var1 = 0xfb8;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfb8;
    _Var1 = 0xffffffff;
  }
  return _Var1;
}



/* ---- lseek64 @ 00405780 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x004057ec) */

__off64_t lseek64(int __fd,__off64_t __offset,int __whence)

{
  int iVar1;
  undefined4 *puVar2;
  undefined1 auStackX_0 [16];
  
  iVar1 = 0x102c;
  syscall(0);
  if (auStackX_0 != (undefined1 *)0x18) {
    puVar2 = (undefined4 *)
             (*(code *)PTR___GI___errno_location_0045acc8)
                       (__fd,(int)((ulonglong)__offset >> 0x20),(int)__offset);
    *puVar2 = 0x102c;
    iVar1 = -1;
  }
  return (__off64_t)iVar1;
}



/* ---- munmap @ 00405820 ----
 * (library/support function; see decompiled body)
 */

int munmap(void *__addr,size_t __len)

{
  int iVar1;
  undefined4 *puVar2;
  int in_a3;
  
  iVar1 = 0xffb;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xffb;
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- sbrk @ 00405880 ----
 * (library/support function; see decompiled body)
 */

void * sbrk(intptr_t __delta)

{
  int iVar1;
  void *pvVar2;
  
  if ((*(int *)PTR___curbrk_0045ac48 != 0) || (iVar1 = (*(code *)PTR_brk_0045ad58)(0), -1 < iVar1))
  {
    if (__delta == 0) {
      return *(void **)PTR___curbrk_0045ac48;
    }
    pvVar2 = *(void **)PTR___curbrk_0045ac48;
    iVar1 = (*(code *)PTR_brk_0045ad58)((int)pvVar2 + __delta);
    if (-1 < iVar1) {
      return pvVar2;
    }
  }
  return (void *)0xffffffff;
}



/* ---- sigprocmask @ 00405910 ----
 * (library/support function; see decompiled body)
 */

int sigprocmask(int __how,sigset_t *__set,sigset_t *__oset)

{
  undefined4 *puVar1;
  
  if ((__set == (sigset_t *)0x0) || (__how - 1U < 3)) {
    syscall(0);
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0x1063;
  }
  else {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0x16;
  }
  return -1;
}



/* ---- __ctype_b_loc @ 004059a0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

ushort ** __ctype_b_loc(void)

{
  ushort **ppuVar1;
  
  ppuVar1 = (ushort **)(*(code *)PTR___curlocale_0045ad3c)();
  return ppuVar1;
}



/* ---- __curlocale @ 004059e0 ----
 * (library/support function; see decompiled body)
 */

undefined4 __curlocale(void)

{
  return *(undefined4 *)PTR___curlocale_var_0045adac;
}



/* ---- __curlocale_set @ 004059f8 ----
 * (library/support function; see decompiled body)
 */

undefined4 __curlocale_set(undefined4 param_1)

{
  undefined4 uVar1;
  
  uVar1 = *(undefined4 *)PTR___curlocale_var_0045adac;
  *(undefined4 *)PTR___curlocale_var_0045adac = param_1;
  return uVar1;
}



/* ---- wcrtomb @ 00405a20 ----
 * (library/support function; see decompiled body)
 */

size_t wcrtomb(char *__s,wchar_t __wc,mbstate_t *__ps)

{
  size_t sVar1;
  undefined2 in_register_00000016;
  undefined4 local_20;
  undefined4 *local_1c;
  char acStack_18 [20];
  
  local_20 = CONCAT22(in_register_00000016,__wc);
  if (__s == (char *)0x0) {
    __s = acStack_18;
    local_20 = 0;
  }
  local_1c = &local_20;
  sVar1 = (*(code *)PTR_wcsnrtombs_0045adbc)(__s,&local_1c,1,0x10,__ps);
  if (sVar1 == 0) {
    sVar1 = 1;
  }
  return sVar1;
}



/* ---- mbsrtowcs @ 00405a90 ----
 * (library/support function; see decompiled body)
 */

size_t mbsrtowcs(wchar_t *__dst,char **__src,size_t __len,mbstate_t *__ps)

{
  size_t sVar1;
  
  if (__ps == (mbstate_t *)0x0) {
    __ps = (mbstate_t *)(PTR_0045abbc + -0x3170);
  }
  sVar1 = (*(code *)PTR_mbsnrtowcs_0045ace0)(__dst,__src,0xffffffff,__len,__ps);
  return sVar1;
}



/* ---- wcsrtombs @ 00405ae0 ----
 * (library/support function; see decompiled body)
 */

size_t wcsrtombs(char *__dst,wchar_t **__src,size_t __len,mbstate_t *__ps)

{
  size_t sVar1;
  
  sVar1 = (*(code *)PTR_wcsnrtombs_0045adbc)(__dst,__src,0xffffffff,__len,__ps);
  return sVar1;
}



/* ---- mbsnrtowcs @ 00405b20 ----
 * (library/support function; see decompiled body)
 */

size_t mbsnrtowcs(wchar_t *__dst,char **__src,size_t __nmc,size_t __len,mbstate_t *__ps)

{
  int iVar1;
  size_t sVar2;
  undefined4 *puVar3;
  uint uVar4;
  int iVar5;
  byte *pbVar6;
  wchar_t local_20 [4];
  
  if (__ps == (mbstate_t *)0x0) {
    __ps = (mbstate_t *)(PTR_0045abbc + -0x3160);
  }
  iVar1 = (*(code *)PTR___curlocale_0045ad3c)();
  if (*(char *)(iVar1 + 0x92c) == '\x01') {
    sVar2 = (*(code *)PTR__wchar_utf8sntowcs_0045ac4c)(__dst,__len,__src,__nmc,__ps,1);
    if (sVar2 == 0xfffffffe) {
      return 0;
    }
    return sVar2;
  }
  if (__dst == (wchar_t *)0x0) {
    __len = 0xffffffff;
  }
  else {
    iVar5 = 1;
    if ((mbstate_t *)__dst != __ps) goto LAB_00405be4;
  }
  __dst = local_20;
  iVar5 = 0;
LAB_00405be4:
  if (__len <= __nmc) {
    __nmc = __len;
  }
  pbVar6 = (byte *)*__src;
  sVar2 = __nmc;
  if (*(char *)(iVar1 + 0x92c) == '\x02') {
    for (; sVar2 != 0; sVar2 = sVar2 - 1) {
      uVar4 = (uint)*pbVar6;
      pbVar6 = pbVar6 + 1;
      if ((0x7f < uVar4) &&
         (uVar4 = (uint)*(ushort *)
                         (*(int *)(iVar1 + 0x94c) +
                         ((uint)*(byte *)(*(int *)(iVar1 + 0x948) + ((int)(uVar4 - 0x80) >> 3)) * 8
                         + (uVar4 - 0x80 & 7)) * 2), uVar4 == 0)) goto LAB_00405cb0;
      *(uint *)__dst = uVar4;
      if (uVar4 == 0) {
        pbVar6 = (byte *)0x0;
        break;
      }
      __dst = __dst + iVar5 * 2;
    }
    if (__dst != local_20) {
      *__src = (char *)pbVar6;
    }
  }
  else {
    for (; sVar2 != 0; sVar2 = sVar2 - 1) {
      uVar4 = (uint)*pbVar6;
      pbVar6 = pbVar6 + 1;
      *(uint *)__dst = uVar4;
      if (uVar4 == 0) {
        pbVar6 = (byte *)0x0;
        break;
      }
      __dst = __dst + iVar5 * 2;
      if (0x7f < uVar4) {
LAB_00405cb0:
        puVar3 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
        *puVar3 = 0x58;
        return 0xffffffff;
      }
    }
    if (__dst != local_20) {
      *__src = (char *)pbVar6;
    }
  }
  return __nmc - sVar2;
}



/* ---- wcsnrtombs @ 00405d10 ----
 * (library/support function; see decompiled body)
 */

size_t wcsnrtombs(char *__dst,wchar_t **__src,size_t __nwc,size_t __len,mbstate_t *__ps)

{
  char cVar1;
  int iVar2;
  size_t sVar3;
  undefined4 *puVar4;
  uint uVar5;
  wchar_t *pwVar6;
  int iVar7;
  char local_28 [20];
  
  iVar2 = (*(code *)PTR___curlocale_0045ad3c)();
  if (*(char *)(iVar2 + 0x92c) == '\x01') {
    sVar3 = (*(code *)PTR__wchar_wcsntoutf8s_0045ac50)(__dst,__len,__src,__nwc);
    return sVar3;
  }
  if (__dst == (char *)0x0) {
    __len = 0xffffffff;
  }
  else {
    iVar7 = 1;
    if ((wchar_t **)__dst != __src) goto LAB_00405da4;
  }
  __dst = local_28;
  iVar7 = 0;
LAB_00405da4:
  if (__len <= __nwc) {
    __nwc = __len;
  }
  pwVar6 = *__src;
  sVar3 = __nwc;
  if (*(char *)(iVar2 + 0x92c) == '\x02') {
    for (; sVar3 != 0; sVar3 = sVar3 - 1) {
      uVar5 = *(uint *)pwVar6;
      pwVar6 = pwVar6 + 2;
      if (uVar5 < 0x80) {
        *__dst = (char)uVar5;
        if ((char)uVar5 == '\0') {
          pwVar6 = (wchar_t *)0x0;
          break;
        }
      }
      else {
        if ((0x25ff < uVar5) ||
           (cVar1 = *(char *)(*(int *)(iVar2 + 0x954) + (uVar5 & 0xf) +
                              (uint)*(byte *)(*(int *)(iVar2 + 0x954) +
                                              (uint)*(byte *)(*(int *)(iVar2 + 0x950) + (uVar5 >> 8)
                                                             ) * 0x10 + ((uVar5 & 0xff) >> 4)) *
                              0x10 + 0x430), cVar1 == '\0')) {
LAB_00405e78:
          puVar4 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
          *puVar4 = 0x58;
          return 0xffffffff;
        }
        *__dst = cVar1;
      }
      __dst = __dst + iVar7;
    }
    if (__dst != local_28) {
      *__src = pwVar6;
    }
  }
  else {
    for (; sVar3 != 0; sVar3 = sVar3 - 1) {
      uVar5 = *(uint *)pwVar6;
      cVar1 = (char)uVar5;
      pwVar6 = pwVar6 + 2;
      if (0x7f < uVar5) goto LAB_00405e78;
      *__dst = cVar1;
      if (cVar1 == '\0') {
        pwVar6 = (wchar_t *)0x0;
        break;
      }
      __dst = __dst + iVar7;
    }
    if (__dst != local_28) {
      *__src = pwVar6;
    }
  }
  return __nwc - sVar3;
}



/* ---- __stdio_WRITE @ 00405ee0 ----
 * (library/support function; see decompiled body)
 */

uint __stdio_WRITE(ushort *param_1,char *param_2,uint param_3)

{
  char cVar1;
  int iVar2;
  uint uVar3;
  char *pcVar4;
  uint uVar5;
  
  uVar5 = param_3;
  while( true ) {
    if (uVar5 == 0) {
      return param_3;
    }
    uVar3 = 0x7fffffff;
    if (-1 < (int)uVar5) {
      uVar3 = uVar5;
    }
    if ((*(code **)(param_1 + 0x16) == (code *)0x0) ||
       (iVar2 = (**(code **)(param_1 + 0x16))(*(undefined4 *)(param_1 + 0x12),param_2,uVar3),
       iVar2 < 0)) break;
    param_2 = param_2 + iVar2;
    uVar5 = uVar5 - iVar2;
  }
  pcVar4 = *(char **)(param_1 + 4);
  uVar3 = *(int *)(param_1 + 6) - (int)pcVar4;
  *param_1 = *param_1 | 8;
  if (uVar3 != 0) {
    if (uVar5 < uVar3) {
      uVar3 = uVar5;
    }
    do {
      cVar1 = *param_2;
      uVar3 = uVar3 - 1;
      param_2 = param_2 + 1;
      *pcVar4 = cVar1;
      if ((cVar1 == '\n') && ((*param_1 & 0x100) != 0)) {
        iVar2 = *(int *)(param_1 + 4);
        goto LAB_00405fa8;
      }
      pcVar4 = pcVar4 + 1;
    } while (uVar3 != 0);
    iVar2 = *(int *)(param_1 + 4);
LAB_00405fa8:
    *(char **)(param_1 + 8) = pcVar4;
    uVar5 = uVar5 + (iVar2 - (int)pcVar4);
  }
  return param_3 - uVar5;
}



/* ---- __stdio_fwrite @ 00405fe0 ----
 * (library/support function; see decompiled body)
 */

uint __stdio_fwrite(int param_1,uint param_2,ushort *param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  if ((*param_3 & 0x200) != 0) {
LAB_00406144:
                    /* WARNING: Could not recover jumptable at 0x00406168. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    uVar2 = (*(code *)PTR___stdio_WRITE_0045abc8)(param_3,param_1,param_2);
    return uVar2;
  }
  iVar1 = *(int *)(param_3 + 8);
  if (*(int *)(param_3 + 2) == -2) {
    uVar2 = *(int *)(param_3 + 6) - iVar1;
    if (param_2 <= uVar2) {
      uVar2 = param_2;
    }
    (*(code *)PTR_memcpy_0045ad34)(iVar1,param_1,uVar2);
    *(uint *)(param_3 + 8) = *(int *)(param_3 + 8) + uVar2;
  }
  else {
    if ((uint)(*(int *)(param_3 + 6) - iVar1) < param_2) {
      if ((iVar1 != *(int *)(param_3 + 4)) &&
         (iVar1 = (*(code *)PTR___stdio_wcommit_0045abc4)(param_3), iVar1 != 0)) {
        return 0;
      }
      goto LAB_00406144;
    }
    (*(code *)PTR_memcpy_0045ad34)(iVar1,param_1,param_2);
    *(uint *)(param_3 + 8) = *(int *)(param_3 + 8) + param_2;
    if ((((*param_3 & 0x100) != 0) &&
        (iVar1 = (*(code *)PTR_memrchr_0045accc)(param_1,10,param_2), iVar1 != 0)) &&
       (uVar2 = (*(code *)PTR___stdio_wcommit_0045abc4)(param_3), uVar2 != 0)) {
      uVar3 = param_2;
      if (uVar2 <= param_2) {
        uVar3 = uVar2;
      }
      param_1 = param_1 + (param_2 - uVar3);
      iVar1 = (*(code *)PTR_memchr_0045ade0)(param_1,10,uVar3);
      if (iVar1 != 0) {
        iVar1 = (param_1 + uVar3) - iVar1;
        *(int *)(param_3 + 8) = *(int *)(param_3 + 8) - iVar1;
        param_2 = param_2 - iVar1;
      }
    }
  }
  return param_2;
}



/* ---- __stdio_trans2w_o @ 00406190 ----
 * (library/support function; see decompiled body)
 */

undefined4 __stdio_trans2w_o(ushort *param_1,ushort param_2)

{
  ushort uVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined4 uVar4;
  
  uVar1 = *param_1;
  if ((param_2 & uVar1) == 0) {
    if ((uVar1 & 0x880) == 0) {
      *param_1 = param_2 | uVar1;
      goto LAB_004061d0;
    }
  }
  else {
LAB_004061d0:
    uVar1 = *param_1;
    if ((uVar1 & 0x20) == 0) {
      if ((uVar1 & 3) == 0) {
        uVar1 = *param_1;
      }
      else {
        if ((uVar1 & 4) == 0) {
          if ((*(int *)(param_1 + 10) == *(int *)(param_1 + 8)) && ((uVar1 & 2) == 0)) {
            uVar1 = *param_1;
          }
          else {
            uVar4 = 2;
            if ((uVar1 & 0x400) == 0) {
              uVar4 = 1;
            }
            iVar3 = (*(code *)PTR_fseek_0045ad4c)(param_1,0,uVar4);
            if (iVar3 != 0) goto LAB_004061f8;
            uVar1 = *param_1;
          }
        }
        else {
          uVar1 = *param_1;
        }
        uVar4 = *(undefined4 *)(param_1 + 4);
        *param_1 = uVar1 & 0xfffc;
        *(undefined4 *)(param_1 + 10) = uVar4;
        *(undefined4 *)(param_1 + 0xc) = uVar4;
        *(undefined4 *)(param_1 + 8) = uVar4;
        uVar1 = *param_1;
      }
      *param_1 = uVar1 | 0x40;
      if ((uVar1 & 0xb00) == 0) {
        *(undefined4 *)(param_1 + 0xe) = *(undefined4 *)(param_1 + 6);
        return 0;
      }
      return 0;
    }
  }
  puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
  *puVar2 = 9;
LAB_004061f8:
  *param_1 = *param_1 | 8;
  return 0xffffffff;
}



/* ---- _load_inttype @ 004062c0 ----
 * (library/support function; see decompiled body)
 */

ulonglong _load_inttype(uint param_1,uint *param_2,int param_3)

{
  uint uVar1;
  
  if (param_3 < 0) {
    if ((param_1 & 0x800) == 0) {
      uVar1 = *param_2;
      if (param_1 == 0x100) {
        uVar1 = (int)(char)*param_2;
      }
      if (param_1 == 0x200) {
        uVar1 = (int)(short)uVar1;
      }
      return (longlong)(int)uVar1;
    }
    uVar1 = param_2[1];
  }
  else {
    if ((param_1 & 0x800) == 0) {
      uVar1 = *param_2;
      if (param_1 == 0x100) {
        uVar1 = *param_2 & 0xff;
      }
      if (param_1 == 0x200) {
        uVar1 = uVar1 & 0xffff;
      }
      return (ulonglong)uVar1;
    }
    uVar1 = param_2[1];
  }
  return CONCAT44(uVar1,*param_2);
}



/* ---- _store_inttype @ 00406330 ----
 * (library/support function; see decompiled body)
 */

void _store_inttype(undefined4 *param_1,int param_2,undefined4 param_3,undefined4 param_4)

{
  if (param_2 == 0x100) {
    *(char *)param_1 = (char)param_3;
    return;
  }
  if (param_2 == 0x800) {
    param_1[1] = param_4;
  }
  else if (param_2 == 0x200) {
    *(short *)param_1 = (short)param_3;
    return;
  }
  *param_1 = param_3;
  return;
}



/* ---- _uintmaxtostr @ 00406370 ----
 * (library/support function; see decompiled body)
 */

char * _uintmaxtostr(char *param_1,undefined4 param_2,uint param_3,uint param_4,uint param_5,
                    uint param_6)

{
  bool bVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  char *unaff_s6;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  
  if (((int)param_5 < 0) && (param_5 = -param_5, (int)param_4 < 0)) {
    param_3 = -param_3;
    param_4 = -(uint)(param_3 != 0) - param_4;
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  uVar5 = param_6 & 0x80 ^ param_6;
  if (uVar5 == 0x2c) {
    iVar3 = (*(code *)PTR___curlocale_0045ad3c)();
    unaff_s6 = *(char **)(iVar3 + 0x9ac);
    iVar3 = (int)*unaff_s6;
    if (iVar3 != 0) goto LAB_0040642c;
  }
  iVar3 = -1;
LAB_0040642c:
  uVar7 = 0xffffffff / param_5;
  if (param_5 == 0) {
    trap(7);
  }
  *param_1 = '\0';
  uVar6 = 0xffffffffU % param_5 + 1;
  if (uVar6 == param_5) {
    uVar7 = uVar7 + 1;
    uVar6 = 0;
  }
  do {
    if (iVar3 == 0) {
      iVar3 = (*(code *)PTR___curlocale_0045ad3c)();
      param_1 = param_1 + -*(int *)(iVar3 + 0x974);
      (*(code *)PTR_memcpy_0045ad34)(param_1,*(undefined4 *)(iVar3 + 0x9a8));
      if (unaff_s6[1] != '\0') {
        unaff_s6 = unaff_s6 + 1;
      }
      iVar3 = (int)*unaff_s6;
    }
    if (param_4 == 0) {
      uVar8 = param_3 / param_5;
      if (param_5 == 0) {
        trap(7);
      }
    }
    else {
      uVar8 = param_3 / param_5;
      if (param_5 == 0) {
        trap(7);
      }
      if (param_5 == 0) {
        trap(7);
      }
      param_3 = (param_4 % param_5) * uVar6 + param_3 % param_5;
      if (param_5 == 0) {
        trap(7);
      }
      uVar8 = (param_4 % param_5) * uVar7 + uVar8 + param_3 / param_5;
      param_4 = param_4 / param_5;
    }
    param_3 = param_3 % param_5;
    if ((param_6 & 0x80) == 0) {
      param_1 = param_1 + -1;
      if (param_3 < 10) {
        cVar2 = (char)param_3 + '0';
      }
      else {
        cVar2 = (char)param_3 + (char)uVar5;
      }
      *param_1 = cVar2;
    }
    else {
      iVar4 = (*(code *)PTR___curlocale_0045ad3c)();
      param_1 = param_1 + -(uint)*(byte *)(iVar4 + param_3 + 0x92e);
      (*(code *)PTR_memcpy_0045ad34)(param_1,*(undefined4 *)(iVar4 + param_3 * 4 + 0x978));
    }
    iVar3 = iVar3 + -1;
    param_3 = uVar8;
  } while (uVar8 != 0 || param_4 != 0);
  if (bVar1) {
    param_1 = param_1 + -1;
    *param_1 = '-';
  }
  return param_1;
}



/* ---- _fpmaxtostr @ 004065c0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00406db4) */
/* WARNING: Removing unreachable block (ram,0x004066a8) */

int _fpmaxtostr(undefined4 param_1,undefined4 param_2,undefined4 param_3,uint param_4,
               undefined4 *param_5,code *param_6)

{
  int *piVar1;
  byte bVar2;
  char cVar3;
  undefined *puVar4;
  bool bVar5;
  char *pcVar6;
  int iVar7;
  char *pcVar8;
  int *piVar9;
  int iVar10;
  uint uVar11;
  char *pcVar12;
  char *pcVar13;
  double *pdVar14;
  char *pcVar15;
  int iVar16;
  uint uVar17;
  int *piVar18;
  int iVar19;
  char *pcVar20;
  byte bVar21;
  double dVar22;
  double dVar23;
  double dVar24;
  double dVar25;
  double dVar26;
  char local_d0 [8];
  char local_c8;
  char local_bc [28];
  int local_a0;
  uint local_9c;
  char *local_98;
  int local_94;
  int local_90;
  char *local_8c;
  int local_88;
  char *local_84;
  char *local_80;
  int local_7c;
  int local_78;
  undefined *local_74;
  int local_70 [12];
  int local_40;
  undefined *local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_2c;
  
  bVar2 = *(byte *)(param_5 + 2);
  local_c8 = 'e';
  local_40 = param_5[1];
  pcVar20 = (char *)*param_5;
  if ((bVar2 | 0x20) == 0x61) {
    local_c8 = 'p';
    local_2c = 0;
    if ((int)pcVar20 < 0) {
      local_2c = 1;
      pcVar20 = (char *)0xe;
    }
  }
  else {
    if ((int)pcVar20 < 0) {
      pcVar20 = (char *)0x6;
    }
    local_2c = 0;
  }
  local_d0[0] = '\0';
  if ((param_5[3] & 2) == 0) {
    if ((param_5[3] & 1) != 0) {
      local_d0[0] = ' ';
    }
  }
  else {
    local_d0[0] = '+';
  }
  local_d0[1] = 0;
  local_8c = (char *)0x0;
  if ((double)CONCAT44(param_4,param_3) == 0.0) {
    if (*(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x5410) / (double)CONCAT44(param_4,param_3) <
        0.0) {
      local_d0[0] = '-';
    }
    dVar26 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x5408);
    dVar23 = (double)CONCAT44(param_4,param_3);
    uVar17 = 10;
    iVar16 = 9;
    iVar19 = 0x11;
    pcVar13 = (char *)0xffffffff;
LAB_0040688c:
    pcVar15 = local_bc + 6;
    bVar5 = false;
    dVar24 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x53f0);
    dVar25 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x53e8);
    while( true ) {
      if (dVar23 < dVar24) {
        uVar11 = (uint)dVar23;
      }
      else {
        uVar11 = (int)(dVar23 - dVar24) | 0x80000000;
      }
      dVar22 = (double)(int)uVar11;
      if ((int)uVar11 < 0) {
        dVar22 = dVar22 + dVar25;
      }
      pcVar15 = pcVar15 + iVar16;
      iVar7 = 0;
      dVar23 = (dVar23 - dVar22) * dVar26;
      do {
        if (uVar17 == 0) {
          trap(7);
        }
        iVar7 = iVar7 + 1;
        pcVar15[-iVar7] = (char)(uVar11 % uVar17) + '0';
        uVar11 = uVar11 / uVar17;
      } while (iVar7 < iVar16);
      if (bVar5) break;
      bVar5 = true;
    }
    if ((char)bVar2 < 'a') {
      bVar2 = bVar2 + 0x20;
      local_c8 = local_c8 + -0x20;
    }
    if (bVar2 == 0x67) {
      pcVar20 = pcVar20 + -(uint)(pcVar20 != (char *)0x0);
      pcVar15 = pcVar20;
    }
    else {
      pcVar15 = pcVar20;
      if ((bVar2 == 0x66) && (pcVar15 = pcVar13 + (int)pcVar20, (int)(pcVar13 + (int)pcVar20) < -1))
      {
        (*(code *)PTR_memset_0045ad90)(local_bc + 4,0x30,0x11);
        pcVar15 = (char *)0xffffffff;
        pcVar13 = (char *)0xffffffff;
      }
    }
    local_bc[5] = 0x30;
    local_bc[4] = 0;
    if ((int)pcVar15 < iVar19) {
      pcVar12 = local_bc + (int)(pcVar15 + 7);
      bVar5 = (uVar17 >> 1) + 0x30 <= (uint)(int)*pcVar12;
    }
    else {
      pcVar12 = local_bc + iVar19 + 6;
      bVar5 = false;
    }
    do {
      do {
        pcVar12 = pcVar12 + -1;
        cVar3 = *pcVar12 + bVar5;
        *pcVar12 = cVar3;
      } while ((int)cVar3 == 0x30);
    } while (uVar17 + 0x2f < (uint)(int)cVar3);
    if ((bVar2 | 0x20) == 0x61) {
      cVar3 = *pcVar12;
      pcVar6 = pcVar12;
      while (cVar3 != '\0') {
        if ('9' < cVar3) {
          *pcVar6 = local_c8 + -0x49 + cVar3;
        }
        pcVar6 = pcVar6 + -1;
        cVar3 = *pcVar6;
      }
      if (local_bc + 5 < pcVar12) {
        pcVar13 = (char *)((int)pcVar13 << 2);
      }
    }
    pcVar6 = local_bc + 5;
    if (pcVar6 < pcVar12) {
      pcVar6 = local_bc + 6;
    }
    else {
      pcVar13 = pcVar13 + 1;
      pcVar12 = pcVar6;
    }
    pcVar12[1] = '\0';
    if (bVar2 == 0x67) {
      bVar21 = 0x67;
      if ((int)pcVar13 < -4) goto LAB_00407078;
      pcVar8 = (char *)0x0;
      if ((int)pcVar13 <= (int)pcVar15) {
        pcVar20 = pcVar15 + -(int)pcVar13;
        goto LAB_00406ab8;
      }
    }
    else {
      bVar21 = bVar2;
      if (bVar2 == 0x66) {
LAB_00406ab8:
        bVar21 = 0x66;
        pcVar8 = pcVar13;
        if ((int)pcVar13 < 0) {
          pcVar6 = pcVar6 + -1;
          *pcVar6 = '0';
        }
      }
      else {
LAB_00407078:
        pcVar8 = (char *)0x0;
      }
    }
    local_94 = 0xb0;
    local_90 = 1;
    local_8c = local_d0 + 4;
    local_d0[4] = *pcVar6;
    pcVar6 = pcVar6 + 1;
    local_d0[5] = 0;
    pcVar12 = pcVar12 + (1 - (int)pcVar6);
    iVar16 = 0;
    if ((int)pcVar8 < 0) {
LAB_00406c20:
      piVar18 = &local_88;
    }
    else {
      uVar17 = param_5[3];
      if ((uVar17 & 0x20) != 0) {
        iVar19 = (*(code *)PTR___curlocale_0045ad3c)();
        iVar7 = (int)**(char **)(iVar19 + 0x9ac);
        if (iVar7 != 0) {
          local_38 = (int)(*(char **)(iVar19 + 0x9ac))[1];
          if (local_38 == 0) {
            local_38 = iVar7;
          }
          if (iVar7 <= (int)pcVar8) {
            if (local_38 == 0) {
              trap(7);
            }
            iVar16 = ((int)pcVar8 - iVar7) / local_38 + 1;
            if ((uVar17 & 0x80) == 0) {
              local_34 = *(int *)(iVar19 + 0x974);
              local_3c = *(undefined **)(iVar19 + 0x9a8);
            }
            else {
              local_34 = 1;
              local_3c = PTR_s__a__d__b__Y__T_0045abc0 + -0x54ae;
            }
            if (local_38 == 0) {
              trap(7);
            }
            local_30 = ((int)pcVar8 - iVar7) % local_38;
            local_40 = local_40 - local_34 * iVar16;
          }
        }
      }
      local_88 = 0x70;
      local_80 = pcVar6;
      if ((int)pcVar8 < (int)pcVar12) {
        if (pcVar8 == (char *)0x0) {
          pcVar8 = (char *)0xffffffff;
          goto LAB_00406c20;
        }
        pcVar12 = pcVar12 + -(int)pcVar8;
        local_84 = pcVar8;
        pcVar6 = pcVar6 + (int)pcVar8;
        piVar18 = &local_7c;
        pcVar8 = (char *)0xffffffff;
      }
      else {
        local_84 = pcVar12;
        if ((int)pcVar8 - (int)pcVar12 < 1) {
          piVar18 = &local_7c;
        }
        else {
          local_7c = 0xb0;
          local_78 = (int)pcVar8 - (int)pcVar12;
          local_74 = PTR_s__a__d__b__Y__T_0045abc0 + -0x54bd;
          piVar18 = local_70;
        }
        pcVar8 = (char *)0xffffffff;
        pcVar12 = (char *)0x0;
      }
    }
    if ((((param_5[3] & 0x10) != 0) || (pcVar12 != (char *)0x0)) ||
       ((bVar2 != 0x67 && ((bVar2 != 0x61 && (0 < (int)pcVar20)))))) {
      uVar17 = param_5[3];
      *piVar18 = 0x70;
      if ((uVar17 & 0x80) == 0) {
        iVar19 = (*(code *)PTR___curlocale_0045ad3c)();
        piVar18[1] = *(int *)(iVar19 + 0x970);
        puVar4 = *(undefined **)(iVar19 + 0x9a4);
      }
      else {
        piVar18[1] = 1;
        puVar4 = PTR_s__a__d__b__Y__T_0045abc0 + -0x54b0;
      }
      piVar18[2] = (int)puVar4;
      piVar18 = piVar18 + 3;
    }
    pcVar8 = pcVar8 + 1;
    if ((int)pcVar8 < 0) {
      *piVar18 = 0xb0;
      piVar18[1] = -(int)pcVar8;
      piVar18[2] = (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x54bd);
      piVar18 = piVar18 + 3;
    }
    if (pcVar12 != (char *)0x0) {
      *piVar18 = 0x70;
      piVar18[2] = (int)pcVar6;
      piVar18[1] = (int)pcVar12;
      piVar18 = piVar18 + 3;
    }
    if ((((bVar2 != 0x67) || ((param_5[3] & 0x10) != 0)) && (local_2c == 0)) &&
       ((int)pcVar12 - (int)pcVar8 < (int)pcVar20)) {
      *piVar18 = 0xb0;
      piVar18[1] = (int)pcVar20 - ((int)pcVar12 - (int)pcVar8);
      piVar18[2] = (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x54bd);
      piVar18 = piVar18 + 3;
    }
    if (bVar21 == 0x66) {
      bVar2 = 0x66;
    }
    else {
      iVar19 = 3;
      if (bVar2 != 0x61) {
        iVar19 = 4;
      }
      if ((int)pcVar13 < 0) {
        pcVar13 = (char *)-(int)pcVar13;
        cVar3 = '-';
      }
      else {
        cVar3 = '+';
      }
      local_bc[3] = 0;
      iVar7 = 2;
      pcVar20 = local_bc + 3;
      do {
        while( true ) {
          pcVar15 = pcVar20;
          pcVar12 = (char *)((int)pcVar13 / 10);
          iVar7 = iVar7 + 1;
          pcVar20 = pcVar15 + -1;
          *pcVar20 = (char)((int)pcVar13 % 10) + '0';
          pcVar13 = pcVar12;
          if (iVar19 <= iVar7) break;
          iVar7 = 3;
        }
      } while (pcVar12 != (char *)0x0);
      pcVar15[-3] = local_c8;
      pcVar15[-2] = cVar3;
      *piVar18 = 0x70;
      piVar18[1] = iVar7;
      piVar18[2] = (int)(pcVar15 + -3);
      piVar18 = piVar18 + 3;
      bVar2 = bVar21;
    }
  }
  else {
    if ((double)CONCAT44(param_4,param_3) < 0.0) {
      param_4 = param_4 ^ 0x80000000;
      local_d0[0] = '-';
    }
    if ((double)CONCAT44(param_4,param_3) !=
        (double)CONCAT44(param_4,param_3) * *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x5400)) {
      if ((bVar2 | 0x20) == 0x61) {
        uVar17 = 0x10;
        iVar16 = 8;
        dVar24 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x53f0);
        iVar19 = 0xe;
        pcVar13 = (char *)0x7;
        dVar26 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x53e8);
        pdVar14 = (double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x54a8);
      }
      else {
        uVar17 = 10;
        iVar16 = 9;
        dVar24 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x53f8);
        iVar19 = 0x11;
        pcVar13 = (char *)0x8;
        dVar26 = *(double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x5408);
        pdVar14 = (double *)(PTR_s__a__d__b__Y__T_0045abc0 + -0x5460);
      }
      iVar10 = 9;
      iVar7 = 0x100;
      dVar25 = (double)CONCAT44(param_4,param_3);
      do {
        iVar10 = iVar10 + -1;
        if ((double)CONCAT44(param_4,param_3) < dVar24) {
          dVar23 = dVar25 * pdVar14[iVar10];
          if (dVar23 < dVar26) {
            pcVar13 = pcVar13 + -iVar7;
          }
          else {
            dVar23 = dVar25;
          }
        }
        else {
          dVar23 = dVar25 / pdVar14[iVar10];
          if (dVar24 <= dVar23) {
            pcVar13 = pcVar13 + iVar7;
          }
          else {
            dVar23 = dVar25;
          }
        }
        iVar7 = iVar7 >> 1;
        dVar25 = dVar23;
      } while (iVar10 != 0);
      if (dVar26 <= dVar23) {
        pcVar13 = pcVar13 + 1;
        dVar23 = dVar23 / *pdVar14;
      }
      goto LAB_0040688c;
    }
    param_5[4] = 0x20;
    local_94 = 0x70;
    local_90 = 3;
    if ((char)bVar2 < 'a') {
      local_8c = (char *)0x4;
    }
    iVar16 = 0;
    local_8c = PTR_s__a__d__b__Y__T_0045abc0 + (int)local_8c + -0x54c0;
    piVar18 = &local_88;
  }
  piVar9 = &local_90;
  do {
    iVar19 = *piVar9;
    piVar9 = piVar9 + 3;
    local_40 = local_40 - iVar19;
  } while (piVar9 < piVar18);
  uVar17 = (uint)(local_d0[0] != '\0');
  local_a0 = 0xa0;
  local_9c = uVar17;
  local_98 = local_d0;
  if ((bVar2 | 0x20) == 0x61) {
    iVar19 = local_40;
    if (local_94 < 0x10) goto LAB_00406e84;
    local_d0[uVar17] = '0';
    local_9c = uVar17 + 2;
    local_d0[uVar17 + 2] = '\0';
    local_d0[uVar17 + 1] = local_c8 + '\b';
  }
  iVar19 = local_40;
LAB_00406e84:
  iVar19 = iVar19 - local_9c;
  if (0 < iVar19) {
    if ((param_5[3] & 8) == 0) {
      if (param_5[4] == 0x30) {
        local_90 = local_90 + iVar19;
      }
      else {
        local_9c = local_9c + iVar19;
      }
    }
    else {
      *piVar18 = 0xa0;
      piVar18[1] = iVar19;
      piVar18[2] = (int)(PTR_s__a__d__b__Y__T_0045abc0 + -0x54bd);
      piVar18 = piVar18 + 3;
    }
  }
  iVar19 = 0;
  piVar9 = &local_a0;
  do {
    if (piVar9 == &local_88) {
      if (iVar16 == 0) {
        iVar7 = *piVar9;
        goto LAB_00406ff8;
      }
      iVar19 = iVar16 * local_34 + iVar19;
      iVar7 = local_30;
      pcVar20 = local_80;
      pcVar13 = local_84;
LAB_00406f38:
      while( true ) {
        if (iVar7 == 0) break;
        if (iVar7 <= (int)pcVar13) {
          iVar10 = (*param_6)(param_1,*piVar9,iVar7,pcVar20);
          if (iVar10 != iVar7) {
            return -1;
          }
          pcVar13 = pcVar13 + -iVar7;
          if (*pcVar20 != '\0') {
            pcVar20 = pcVar20 + iVar7;
          }
          break;
        }
        if (pcVar13 == (char *)0x0) {
          iVar10 = piVar9[3];
        }
        else {
          pcVar20 = (char *)(*param_6)(param_1,*piVar9,pcVar13,pcVar20);
          if (pcVar20 != pcVar13) {
            return -1;
          }
          iVar10 = piVar9[3];
        }
        if (iVar10 != 0xb0) goto LAB_00407018;
        iVar19 = iVar19 + piVar9[1];
        pcVar20 = (char *)piVar9[5];
        piVar1 = piVar9 + 4;
        iVar7 = iVar7 - (int)pcVar13;
        piVar9 = piVar9 + 3;
        pcVar13 = (char *)*piVar1;
      }
      if (0 < iVar16) {
        iVar7 = (*param_6)(param_1,0x70,local_34,local_3c);
        if (iVar7 != local_34) {
          return -1;
        }
        iVar16 = iVar16 + -1;
        iVar7 = local_38;
        goto LAB_00406f38;
      }
    }
    else {
      iVar7 = *piVar9;
LAB_00406ff8:
      iVar7 = (*param_6)(param_1,iVar7,piVar9[1],piVar9[2]);
      if (iVar7 != piVar9[1]) {
        return -1;
      }
    }
LAB_00407018:
    piVar1 = piVar9 + 1;
    piVar9 = piVar9 + 3;
    iVar19 = iVar19 + *piVar1;
    if (piVar18 <= piVar9) {
      return iVar19;
    }
  } while( true );
}



/* ---- memchr @ 00407080 ----
 * (library/support function; see decompiled body)
 */

void * memchr(void *__s,int __c,size_t __n)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  
  uVar5 = __c & 0xff;
  for (; (__n != 0 && (((uint)__s & 3) != 0)); __s = (void *)((int)__s + 1)) {
    __n = __n - 1;
    if ((byte)*(uint *)__s == uVar5) {
      return __s;
    }
  }
  uVar2 = uVar5 << 8 | uVar5;
  while( true ) {
    do {
      puVar4 = __s;
      if (__n < 4) {
        while( true ) {
          bVar1 = __n == 0;
          __n = __n - 1;
          if (bVar1) {
            return (void *)0x0;
          }
          if ((byte)*puVar4 == uVar5) break;
          puVar4 = (uint *)((int)puVar4 + 1);
        }
        return puVar4;
      }
      __n = __n - 4;
      uVar3 = (uVar2 << 0x10 | uVar2) ^ *puVar4;
      __s = puVar4 + 1;
    } while (((~uVar3 ^ uVar3 + 0x7efefeff) & 0x81010100) == 0);
    if ((byte)*puVar4 == uVar5) {
      return puVar4;
    }
    if (*(byte *)((int)puVar4 + 1) == uVar5) {
      return (byte *)((int)puVar4 + 1);
    }
    if (*(byte *)((int)puVar4 + 2) == uVar5) break;
    if (*(byte *)((int)puVar4 + 3) == uVar5) {
      return (byte *)((int)puVar4 + 3);
    }
  }
  return (byte *)((int)puVar4 + 2);
}



/* ---- mempcpy @ 00407170 ----
 * (library/support function; see decompiled body)
 */

void * mempcpy(void *__dest,void *__src,size_t __n)

{
  (*(code *)PTR_memcpy_0045ad34)();
  return (void *)((int)__dest + __n);
}



/* ---- memrchr @ 004071c0 ----
 * (library/support function; see decompiled body)
 */

void * memrchr(void *__s,int __c,size_t __n)

{
  bool bVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  
  uVar6 = __c & 0xff;
  puVar4 = (uint *)((int)__s + __n);
  while ((__n != 0 && (((uint)puVar4 & 3) != 0))) {
    puVar4 = (uint *)((int)puVar4 + -1);
    __n = __n - 1;
    if (*(byte *)puVar4 == uVar6) {
      return puVar4;
    }
  }
  uVar2 = uVar6 << 8 | uVar6;
  while( true ) {
    do {
      puVar5 = puVar4;
      if (__n < 4) {
        do {
          puVar5 = (uint *)((int)puVar5 + -1);
          bVar1 = __n == 0;
          __n = __n - 1;
          if (bVar1) {
            return (void *)0x0;
          }
        } while (*(byte *)puVar5 != uVar6);
        return puVar5;
      }
      puVar4 = puVar5 + -1;
      uVar3 = (uVar2 << 0x10 | uVar2) ^ *puVar4;
      __n = __n - 4;
    } while (((~uVar3 ^ uVar3 + 0x7efefeff) & 0x81010100) == 0);
    if (*(byte *)((int)puVar5 + -1) == uVar6) {
      return (byte *)((int)puVar5 + -1);
    }
    if (*(byte *)((int)puVar5 + -2) == uVar6) {
      return (byte *)((int)puVar5 + -2);
    }
    if (*(byte *)((int)puVar5 + -3) == uVar6) break;
    if ((byte)*puVar4 == uVar6) {
      return puVar4;
    }
  }
  return (byte *)((int)puVar5 + -3);
}



/* ---- rindex @ 004072b0 ----
 * (library/support function; see decompiled body)
 */

char * rindex(char *__s,int __c)

{
  undefined *puVar1;
  char *pcVar2;
  char *pcVar3;
  
  puVar1 = PTR_strchr_0045ad8c;
  if ((__c & 0xffU) == 0) {
                    /* WARNING: Could not recover jumptable at 0x00407304. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    pcVar2 = (char *)(*(code *)PTR_strchr_0045ad8c)(__s,0);
    return pcVar2;
  }
  pcVar2 = (char *)0x0;
  while (pcVar3 = (char *)(*(code *)puVar1)(__s,__c & 0xffU), pcVar3 != (char *)0x0) {
    __s = pcVar3 + 1;
    pcVar2 = pcVar3;
  }
  return pcVar2;
}



/* ---- raise @ 00407350 ----
 * (library/support function; see decompiled body)
 */

int raise(int __sig)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = (*(code *)PTR_getpid_0045ad08)();
                    /* WARNING: Could not recover jumptable at 0x00407390. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  iVar2 = (*(code *)PTR_kill_0045ad48)(uVar1,__sig);
  return iVar2;
}



/* ---- __sigismember @ 004073a0 ----
 * (library/support function; see decompiled body)
 */

int __sigismember(__sigset_t *param_1,int param_2)

{
  return (uint)((1 << (param_2 - 1U & 0x1f) & param_1->__val[param_2 - 1U >> 5]) != 0);
}



/* ---- __sigaddset @ 004073c8 ----
 * (library/support function; see decompiled body)
 */

int __sigaddset(__sigset_t *param_1,int param_2)

{
  param_1->__val[param_2 - 1U >> 5] = param_1->__val[param_2 - 1U >> 5] | 1 << (param_2 - 1U & 0x1f)
  ;
  return 0;
}



/* ---- __sigdelset @ 004073f4 ----
 * (library/support function; see decompiled body)
 */

int __sigdelset(__sigset_t *param_1,int param_2)

{
  param_1->__val[param_2 - 1U >> 5] =
       ~(1 << (param_2 - 1U & 0x1f)) & param_1->__val[param_2 - 1U >> 5];
  return 0;
}



/* ---- FUN_00407430 @ 00407430 ----
 * (library/support function; see decompiled body)
 */

int FUN_00407430(uint param_1)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar1 = param_1 >> 8;
  iVar2 = 0x5f;
  if (uVar1 < 0x10000) {
    uVar3 = uVar1 - 0x100 >> 0x10 & 8;
    iVar2 = uVar1 << uVar3;
    uVar5 = iVar2 - 0x1000U >> 0x10 & 4;
    iVar2 = iVar2 << uVar5;
    uVar4 = iVar2 - 0x4000U >> 0x10 & 2;
    uVar1 = iVar2 << uVar4;
    iVar2 = (((0xd - uVar3) - uVar5) - uVar4) + (~(uVar1 >> 0xf) & uVar1 >> 0xe);
    iVar2 = iVar2 * 4 + 0x20 + (param_1 >> (iVar2 + 6U & 0x1f) & 3);
  }
  return iVar2;
}



/* ---- malloc @ 004074bc ----
 * (library/support function; see decompiled body)
 */

void * malloc(size_t __size)

{
  int *piVar1;
  undefined4 *puVar2;
  void *pvVar3;
  uint uVar4;
  int iVar5;
  undefined *puVar6;
  uint uVar7;
  undefined *puVar8;
  undefined *puVar9;
  int iVar10;
  uint uVar11;
  undefined *puVar12;
  undefined *puVar13;
  uint uVar14;
  int *piVar15;
  uint uVar16;
  undefined4 uVar17;
  undefined *puVar18;
  uint uVar19;
  uint uVar20;
  uint uVar21;
  int iVar22;
  uint uVar23;
  undefined1 auStack_40 [16];
  uint local_30;
  
  if (__size == 0) {
LAB_00407544:
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xc;
    return (void *)0x0;
  }
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
            (auStack_40,PTR___pthread_mutex_trylock_0045ad64,PTR___malloc_lock_0045ad0c);
  (*(code *)PTR___pthread_mutex_trylock_0045adcc)(PTR___malloc_lock_0045ad0c);
  if (0xffffffdf < __size) goto LAB_00407544;
  uVar20 = 0x10;
  if (0xf < __size + 0xb) {
    uVar20 = __size + 0xb & 0xfffffff8;
  }
  uVar14 = *(uint *)PTR___malloc_state_0045ad70;
  if ((uVar14 & 1) != 0) {
    if (uVar20 <= uVar14) {
      iVar10 = *(int *)(PTR___malloc_state_0045ad70 + ((uVar20 >> 1) - 4));
      if (iVar10 != 0) {
        piVar15 = (int *)(iVar10 + 8);
        *(undefined4 *)(PTR___malloc_state_0045ad70 + ((uVar20 >> 1) - 4)) =
             *(undefined4 *)(iVar10 + 8);
        goto LAB_00407ed0;
      }
    }
    uVar16 = uVar20 >> 3;
    if (uVar20 < 0x100) {
      puVar12 = PTR___malloc_state_0045ad70 + (1 - uVar16) * -8 + 0x34;
      puVar8 = *(undefined **)(puVar12 + 0xc);
      if (puVar8 != puVar12) {
        iVar10 = *(int *)(puVar8 + 0xc);
        piVar15 = (int *)(puVar8 + 8);
        *(uint *)(puVar8 + uVar20 + 4) = *(uint *)(puVar8 + uVar20 + 4) | 1;
        *(undefined **)(iVar10 + 8) = puVar12;
        *(int *)(puVar12 + 0xc) = iVar10;
        goto LAB_00407ed0;
      }
    }
    else {
      uVar16 = (*(code *)(PTR_Elf32_Ehdr_0045ac00 + 0x7430))(uVar20);
      if ((uVar14 & 2) != 0) {
        (*(code *)PTR___malloc_consolidate_0045ac1c)(PTR___malloc_state_0045ad70);
      }
    }
    puVar8 = PTR___malloc_state_0045ad70;
    puVar12 = PTR___malloc_state_0045ad70 + 0x34;
    while( true ) {
      puVar6 = PTR___malloc_state_0045ad70;
      puVar18 = *(undefined **)(puVar8 + 0x40);
      if (puVar18 == puVar12) break;
      uVar14 = *(uint *)(puVar18 + 4);
      puVar6 = *(undefined **)(puVar18 + 0xc);
      uVar21 = uVar14 & 0xfffffffc;
      if (0xff < uVar20) {
        *(undefined **)(puVar6 + 8) = puVar12;
      }
      else if (puVar6 == puVar12) {
        if (puVar18 == *(undefined **)(puVar8 + 0x30)) {
          if (uVar20 + 0x10 < uVar21) {
            puVar6 = puVar18 + uVar20;
            uVar21 = uVar21 - uVar20;
            *(undefined **)(PTR___malloc_state_0045ad70 + 0x30) = puVar6;
            piVar15 = (int *)(puVar18 + 8);
            *(uint *)(puVar18 + 4) = uVar20 | 1;
            *(undefined **)(puVar8 + 0x3c) = puVar6;
            *(undefined **)(puVar8 + 0x40) = puVar6;
            *(uint *)(puVar6 + uVar21) = uVar21;
            *(uint *)(puVar6 + 4) = uVar21 | 1;
            *(undefined **)(puVar6 + 0xc) = puVar12;
            *(undefined **)(puVar6 + 8) = puVar12;
            goto LAB_00407ed0;
          }
          *(undefined **)(puVar6 + 8) = puVar12;
        }
        else {
          *(undefined **)(puVar6 + 8) = puVar12;
        }
      }
      else {
        *(undefined **)(puVar6 + 8) = puVar12;
      }
      *(undefined **)(puVar8 + 0x40) = puVar6;
      if (uVar21 == uVar20) {
        puVar8 = puVar18 + uVar21;
        uVar20 = *(uint *)(puVar8 + 4);
        piVar15 = (int *)(puVar18 + 8);
        goto LAB_00407838;
      }
      uVar14 = uVar14 >> 3;
      if (uVar21 < 0x100) {
        puVar6 = puVar12 + uVar14 * 8 + -8;
        puVar13 = *(undefined **)(puVar12 + uVar14 * 8 + -8 + 8);
      }
      else {
        uVar14 = (*(code *)(PTR_Elf32_Ehdr_0045ac00 + 0x7430))(uVar21);
        puVar9 = puVar12 + uVar14 * 8 + -8;
        puVar13 = *(undefined **)(puVar9 + 8);
        puVar6 = puVar9;
        if (puVar13 != puVar9) {
          puVar6 = *(undefined **)(puVar9 + 0xc);
          if (uVar21 < *(uint *)(puVar6 + 4)) {
            puVar13 = puVar9;
          }
          else {
            for (; (uVar21 | 1) < *(uint *)(puVar13 + 4); puVar13 = *(undefined **)(puVar13 + 8)) {
            }
            puVar6 = *(undefined **)(puVar13 + 0xc);
          }
        }
      }
      iVar10 = ((int)uVar14 >> 5) + 0xcc;
      *(uint *)(puVar8 + iVar10 * 4 + 4) = 1 << (uVar14 & 0x1f) | *(uint *)(puVar8 + iVar10 * 4 + 4)
      ;
      *(undefined **)(puVar18 + 0xc) = puVar6;
      *(undefined **)(puVar18 + 8) = puVar13;
      *(undefined **)(puVar13 + 0xc) = puVar18;
      *(undefined **)(puVar6 + 8) = puVar18;
    }
    if (0xff < uVar20) {
      puVar8 = *(undefined **)(puVar12 + (1 - uVar16) * -8 + 0xc);
      while (puVar8 != puVar12 + (1 - uVar16) * -8) {
        uVar14 = *(uint *)(puVar8 + 4) & 0xfffffffc;
        puVar13 = *(undefined **)(puVar8 + 0xc);
        if (uVar20 <= uVar14) {
          iVar10 = *(int *)(puVar8 + 8);
          if ((*(undefined **)(iVar10 + 0xc) != puVar8) ||
             (uVar16 = uVar14 - uVar20, *(undefined **)(puVar13 + 8) != puVar8)) {
            pvVar3 = (void *)(*(code *)PTR_abort_0045ad00)();
            return pvVar3;
          }
          *(int *)(puVar13 + 8) = iVar10;
          *(undefined **)(iVar10 + 0xc) = puVar13;
          piVar15 = (int *)(puVar8 + 8);
          if (0xf < uVar16) {
            puVar12 = puVar8 + uVar20;
            *(uint *)(puVar8 + 4) = uVar20 | 1;
            *(undefined **)(puVar18 + 8) = puVar12;
            *(undefined **)(puVar18 + 0xc) = puVar12;
            *(uint *)(puVar12 + uVar16) = uVar16;
            *(uint *)(puVar12 + 4) = uVar16 | 1;
            *(undefined **)(puVar12 + 0xc) = puVar18;
            *(undefined **)(puVar12 + 8) = puVar18;
            goto LAB_00407ed0;
          }
          puVar8 = puVar8 + uVar14;
          uVar20 = *(uint *)(puVar8 + 4);
          goto LAB_00407838;
        }
        puVar8 = puVar13;
      }
    }
    uVar16 = uVar16 + 1;
    uVar23 = uVar16 >> 5;
    uVar21 = *(uint *)(PTR___malloc_state_0045ad70 + (uVar23 + 0xcc) * 4 + 4);
    uVar14 = 1 << (uVar16 & 0x1f);
    puVar8 = puVar18 + (1 - uVar16) * -8;
    while( true ) {
      if (uVar21 < uVar14) {
        uVar23 = uVar23 + 1;
      }
      else {
        uVar16 = uVar14 & uVar21;
        if (uVar14 != 0) goto LAB_0040791c;
        uVar23 = uVar23 + 1;
      }
      while( true ) {
        if (2 < uVar23) goto LAB_004079cc;
        uVar21 = *(uint *)(puVar6 + (uVar23 + 0xcc) * 4 + 4);
        if (uVar21 != 0) break;
        uVar23 = uVar23 + 1;
      }
      uVar14 = 1;
      puVar8 = puVar18 + (uVar23 * -0x20 + 1) * -8;
      while( true ) {
        uVar16 = uVar14 & uVar21;
LAB_0040791c:
        if (uVar16 != 0) break;
        puVar8 = puVar8 + 8;
        uVar14 = uVar14 << 1;
      }
      puVar12 = *(undefined **)(puVar8 + 0xc);
      if (puVar12 != puVar8) break;
      uVar21 = uVar21 & ~uVar14;
      *(uint *)(puVar6 + (uVar23 + 0xcc) * 4 + 4) = uVar21;
      puVar8 = puVar8 + 8;
      uVar14 = uVar14 << 1;
    }
    uVar14 = *(uint *)(puVar12 + 4);
    iVar10 = *(int *)(puVar12 + 0xc);
    uVar16 = (uVar14 & 0xfffffffc) - uVar20;
    *(undefined **)(iVar10 + 8) = puVar8;
    *(int *)(puVar8 + 0xc) = iVar10;
    if (uVar16 < 0x10) {
      puVar8 = puVar12 + (uVar14 & 0xfffffffc);
      uVar20 = *(uint *)(puVar8 + 4);
      piVar15 = (int *)(puVar12 + 8);
LAB_00407838:
      *(uint *)(puVar8 + 4) = uVar20 | 1;
    }
    else {
      puVar8 = puVar12 + uVar20;
      *(undefined **)(puVar18 + 8) = puVar8;
      *(undefined **)(puVar18 + 0xc) = puVar8;
      *(undefined **)(puVar8 + 8) = puVar18;
      *(undefined **)(puVar8 + 0xc) = puVar18;
      if (uVar20 < 0x100) {
        *(undefined **)(PTR___malloc_state_0045ad70 + 0x30) = puVar8;
      }
      *(uint *)(puVar12 + 4) = uVar20 | 1;
      *(uint *)(puVar8 + uVar16) = uVar16;
      *(uint *)(puVar8 + 4) = uVar16 | 1;
      piVar15 = (int *)(puVar12 + 8);
    }
    goto LAB_00407ed0;
  }
  if (uVar14 == 0) {
    (*(code *)PTR___malloc_consolidate_0045ac1c)();
  }
LAB_004079cc:
  local_30 = uVar20 + 0x10;
  iVar10 = *(int *)(PTR___malloc_state_0045ad70 + 0x2c);
  uVar14 = *(uint *)(iVar10 + 4) & 0xfffffffc;
  if (local_30 <= uVar14) {
    *(uint *)(PTR___malloc_state_0045ad70 + 0x2c) = iVar10 + uVar20;
    piVar15 = (int *)(iVar10 + 8);
    *(uint *)(iVar10 + 4) = uVar20 | 1;
    *(uint *)(iVar10 + uVar20 + 4) = uVar14 - uVar20 | 1;
    goto LAB_00407ed0;
  }
  if ((*(uint *)PTR___malloc_state_0045ad70 & 2) != 0) {
    (*(code *)PTR___malloc_consolidate_0045ac1c)(PTR___malloc_state_0045ad70);
    piVar15 = (int *)(*(code *)PTR_malloc_0045ada0)(uVar20 - 7);
    goto LAB_00407ed0;
  }
  uVar14 = *(int *)(PTR___malloc_state_0045ad70 + 0x35c) - 1;
  if ((((*(uint *)(PTR___malloc_state_0045ad70 + 0x34c) <= uVar20) &&
       (*(int *)(PTR___malloc_state_0045ad70 + 0x350) <
        *(int *)(PTR___malloc_state_0045ad70 + 0x354))) &&
      (uVar16 = *(int *)(PTR___malloc_state_0045ad70 + 0x35c) + 10 + uVar20 & ~uVar14,
      uVar20 < uVar16)) &&
     (piVar15 = (int *)(*(code *)PTR_mmap_0045acec)(0,uVar16,3,0x802,0,0),
     piVar15 != (int *)0xffffffff)) {
    if (((uint)piVar15 & 7) == 0) {
      piVar15[1] = uVar16 | 2;
      *piVar15 = 0;
    }
    else {
      iVar10 = 8 - ((uint)piVar15 & 7);
      piVar15 = (int *)((int)piVar15 + iVar10);
      piVar15[1] = uVar16 - iVar10 | 2;
      *piVar15 = iVar10;
    }
    puVar8 = PTR___malloc_state_0045ad70;
    piVar1 = (int *)(PTR___malloc_state_0045ad70 + 0x358);
    iVar10 = *(int *)(PTR___malloc_state_0045ad70 + 0x350) + 1;
    *(int *)(PTR___malloc_state_0045ad70 + 0x350) = iVar10;
    if (*piVar1 < iVar10) {
      *(int *)(puVar8 + 0x358) = iVar10;
      puVar8 = PTR___malloc_state_0045ad70;
    }
    uVar16 = uVar16 + *(int *)(puVar8 + 0x364);
    *(uint *)(puVar8 + 0x364) = uVar16;
    if (*(uint *)(puVar8 + 0x370) < uVar16) {
      *(uint *)(puVar8 + 0x370) = uVar16;
    }
    if (*(uint *)(PTR___malloc_state_0045ad70 + 0x374) <
        uVar16 + *(int *)(PTR___malloc_state_0045ad70 + 0x368)) {
      *(uint *)(PTR___malloc_state_0045ad70 + 0x374) =
           uVar16 + *(int *)(PTR___malloc_state_0045ad70 + 0x368);
    }
    piVar15 = piVar15 + 2;
    goto LAB_00407ed0;
  }
  uVar23 = ~uVar14;
  uVar16 = 0xffffffff;
  iVar22 = *(int *)(PTR___malloc_state_0045ad70 + 0x2c);
  iVar10 = *(int *)(PTR___malloc_state_0045ad70 + 0x348) + 0x10 + uVar20;
  uVar21 = *(uint *)(iVar22 + 4) & 0xfffffffc;
  if ((*(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 1) != 0) {
    iVar10 = iVar10 - uVar21;
  }
  uVar19 = iVar10 + uVar14 & uVar23;
  if (0 < (int)uVar19) {
    uVar16 = (*(code *)PTR_sbrk_0045adb8)(uVar19);
  }
  uVar11 = 0xffffffff;
  if (uVar16 == 0xffffffff) {
    if ((*(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 1) != 0) {
      uVar19 = uVar21 + uVar14 + uVar19 & uVar23;
    }
    if (uVar19 < 0x100000) {
      uVar19 = 0x100000;
    }
    if ((uVar20 < uVar19) &&
       (uVar16 = (*(code *)PTR_mmap_0045acec)(0,uVar19,3,0x802,0,0), uVar16 != 0xffffffff)) {
      uVar11 = uVar16 + uVar19;
      *(uint *)(PTR___malloc_state_0045ad70 + 0x360) =
           *(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 0xfffffffe;
    }
    else {
      uVar11 = 0xffffffff;
      uVar16 = 0xffffffff;
    }
  }
  puVar8 = PTR___malloc_state_0045ad70;
  if (uVar16 != 0xffffffff) {
    uVar7 = iVar22 + uVar21;
    *(uint *)(PTR___malloc_state_0045ad70 + 0x368) =
         uVar19 + *(int *)(PTR___malloc_state_0045ad70 + 0x368);
    if ((uVar16 == uVar7) && (uVar11 == 0xffffffff)) {
      *(uint *)(iVar22 + 4) = uVar19 + uVar21 | 1;
    }
    else {
      if ((*(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 1) == 0) {
LAB_00407cb4:
        uVar4 = *(uint *)(PTR___malloc_state_0045ad70 + 0x360);
      }
      else if (uVar21 == 0) {
        uVar4 = *(uint *)(PTR___malloc_state_0045ad70 + 0x360);
      }
      else {
        if (uVar16 < uVar7) {
          *(uint *)(PTR___malloc_state_0045ad70 + 0x360) =
               *(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 0xfffffffe;
          goto LAB_00407cb4;
        }
        uVar4 = *(uint *)(PTR___malloc_state_0045ad70 + 0x360);
      }
      puVar8 = PTR___malloc_state_0045ad70;
      if ((uVar4 & 1) == 0) {
        iVar10 = 0;
        if (uVar11 == 0xffffffff) {
          uVar11 = (*(code *)PTR_sbrk_0045adb8)(0);
          *(uint *)(puVar8 + 0x368) = ((uVar11 - uVar16) - uVar19) + *(int *)(puVar8 + 0x368);
        }
      }
      else {
        if (uVar21 != 0) {
          *(uint *)(PTR___malloc_state_0045ad70 + 0x368) =
               (uVar16 - uVar7) + *(int *)(PTR___malloc_state_0045ad70 + 0x368);
        }
        puVar8 = PTR_sbrk_0045adb8;
        if ((uVar16 & 7) == 0) {
          iVar10 = 0;
          uVar7 = uVar16;
        }
        else {
          iVar10 = 8 - (uVar16 & 7);
          uVar7 = uVar16 + iVar10;
        }
        iVar5 = uVar16 + iVar10 + uVar21 + uVar19;
        iVar10 = ((iVar10 + uVar21) - iVar5) + (iVar5 + uVar14 & uVar23);
        uVar11 = (*(code *)PTR_sbrk_0045adb8)(iVar10);
        if (uVar11 == 0xffffffff) {
          uVar11 = (*(code *)puVar8)(0);
          iVar10 = 0;
          uVar16 = uVar7;
        }
        else if (uVar11 < uVar16) {
          uVar11 = uVar16 + uVar19;
          iVar10 = 0;
          *(uint *)(PTR___malloc_state_0045ad70 + 0x360) =
               *(uint *)(PTR___malloc_state_0045ad70 + 0x360) & 0xfffffffe;
          uVar16 = uVar7;
        }
        else {
          uVar16 = uVar7;
        }
      }
      puVar8 = PTR___malloc_state_0045ad70;
      if (uVar11 != 0xffffffff) {
        *(uint *)(PTR___malloc_state_0045ad70 + 0x2c) = uVar16;
        *(uint *)(uVar16 + 4) = (uVar11 + iVar10) - uVar16 | 1;
        *(int *)(puVar8 + 0x368) = iVar10 + *(int *)(puVar8 + 0x368);
        if (uVar21 != 0) {
          uVar14 = uVar21 - 0xc & 0xfffffff8;
          *(uint *)(iVar22 + 4) = uVar14 | 1;
          *(undefined4 *)(iVar22 + uVar14 + 8) = 5;
          *(undefined4 *)(iVar22 + uVar14 + 4) = 5;
          puVar12 = PTR_free_0045ac9c;
          if (0xf < uVar14) {
            uVar17 = *(undefined4 *)(puVar8 + 0x344);
            *(undefined4 *)(puVar8 + 0x344) = 0xffffffff;
            (*(code *)puVar12)(iVar22 + 8);
            *(undefined4 *)(puVar8 + 0x344) = uVar17;
          }
        }
      }
      puVar8 = PTR___malloc_state_0045ad70;
    }
    uVar14 = *(uint *)(puVar8 + 0x368);
    if (*(uint *)(puVar8 + 0x36c) < uVar14) {
      *(uint *)(puVar8 + 0x36c) = uVar14;
    }
    if (*(uint *)(PTR___malloc_state_0045ad70 + 0x374) <
        uVar14 + *(int *)(PTR___malloc_state_0045ad70 + 0x364)) {
      *(uint *)(PTR___malloc_state_0045ad70 + 0x374) =
           uVar14 + *(int *)(PTR___malloc_state_0045ad70 + 0x364);
    }
    iVar10 = *(int *)(PTR___malloc_state_0045ad70 + 0x2c);
    uVar14 = *(uint *)(iVar10 + 4) & 0xfffffffc;
    if (local_30 <= uVar14) {
      *(uint *)(PTR___malloc_state_0045ad70 + 0x2c) = iVar10 + uVar20;
      *(uint *)(iVar10 + 4) = uVar20 | 1;
      *(uint *)(iVar10 + uVar20 + 4) = uVar14 - uVar20 | 1;
      piVar15 = (int *)(iVar10 + 8);
      goto LAB_00407ed0;
    }
  }
  puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
  *puVar2 = 0xc;
  piVar15 = (int *)0x0;
LAB_00407ed0:
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_40,1);
  return piVar15;
}



/* ---- exit @ 00407f20 ----
 * (library/support function; see decompiled body)
 */

void exit(int __status)

{
  undefined1 auStack_18 [16];
  
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
            (auStack_18,PTR___pthread_mutex_trylock_0045ad64,PTR___atexit_lock_0045ace4);
  (*(code *)PTR___pthread_mutex_trylock_0045adcc)(PTR___atexit_lock_0045ace4);
  if (*(code **)PTR___exit_cleanup_0045ac54 != (code *)0x0) {
    (**(code **)PTR___exit_cleanup_0045ac54)(__status);
  }
  (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_18,1);
  (*(code *)PTR___uClibc_fini_0045ac58)();
  if (PTR__stdio_term_0045ac5c != (undefined *)0x0) {
    (*(code *)PTR__stdio_term_0045ac5c)();
  }
  (*(code *)PTR__exit_0045acfc)(__status);
  return;
}



/* ---- brk @ 00407fd0 ----
 * (library/support function; see decompiled body)
 */

int brk(void *__addr)

{
  int iVar1;
  undefined4 *puVar2;
  
  syscall(0);
  *(undefined4 *)PTR___curbrk_0045ac48 = 0xfcd;
  iVar1 = 0;
  if ((void *)0xfcd < __addr) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)(__addr);
    *puVar2 = 0xc;
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- mmap @ 00408040 ----
 * (library/support function; see decompiled body)
 */

void * mmap(void *__addr,size_t __len,int __prot,int __flags,int __fd,__off_t __offset)

{
  void *pvVar1;
  undefined4 *puVar2;
  
  pvVar1 = (void *)0xffa;
  syscall(0);
  if (__flags != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xffa;
    pvVar1 = (void *)0xffffffff;
  }
  return pvVar1;
}



/* ---- getpid @ 004080c0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

__pid_t getpid(void)

{
  __pid_t _Var1;
  undefined4 *puVar2;
  int in_a3;
  
  _Var1 = 0xfb4;
  syscall(0);
  if (in_a3 != 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0xfb4;
    _Var1 = -1;
  }
  return _Var1;
}



/* ---- kill @ 00408120 ----
 * (library/support function; see decompiled body)
 */

int kill(__pid_t __pid,int __sig)

{
  undefined4 *puVar1;
  int in_a3;
  int iVar2;
  
  syscall(0);
  iVar2 = 0xfc5;
  if (in_a3 != 0) {
    puVar1 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar1 = 0xfc5;
    iVar2 = -1;
  }
  return iVar2;
}



/* ---- _locale_set_l @ 00408180 ----
 * (library/support function; see decompiled body)
 */

undefined4 _locale_set_l(int param_1,int *param_2)

{
  byte bVar1;
  byte bVar2;
  short sVar3;
  undefined *puVar4;
  undefined *puVar5;
  undefined *puVar6;
  undefined1 uVar7;
  uint uVar8;
  int *piVar9;
  int iVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  ushort uVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined2 uVar18;
  int iVar19;
  undefined1 *puVar20;
  char cVar21;
  int iVar22;
  int iVar23;
  int iVar24;
  int iVar25;
  ushort *puVar26;
  ushort *puVar27;
  ushort *puVar28;
  int iVar29;
  byte *pbVar30;
  byte *pbVar31;
  undefined2 local_88 [2];
  ushort local_84;
  ushort local_82;
  ushort local_7a;
  ushort local_78;
  ushort local_76;
  short local_6c;
  short local_6a;
  int local_68;
  int local_64;
  ushort *local_60;
  int local_5c;
  ushort *local_58;
  int local_54;
  int local_50;
  int local_4c;
  int local_48;
  undefined2 local_44;
  int *local_40;
  int *local_3c;
  int local_38;
  int *local_34;
  int *local_30;
  int *local_2c;
  
  puVar5 = PTR_memcpy_0045ad34;
  pbVar31 = (byte *)((int)param_2 + 0x90d);
  pbVar30 = (byte *)(param_1 + 1);
  local_4c = 0;
  if ((*(char *)(param_1 + 9) != *(char *)((int)param_2 + 0x915)) ||
     (*(char *)(param_1 + 10) != *(char *)((int)param_2 + 0x916))) {
    iVar29 = *(int *)PTR___locale_mmap_0045adc4;
    uVar8 = (uint)*(byte *)(((*(byte *)(param_1 + 2) & 0x7f) +
                            (*(byte *)(param_1 + 1) & 0x7f) * 0x80) * 9 + iVar29 + 0x3e40b);
    if (uVar8 == 0) {
      local_88[0] = 0;
    }
    else {
      puVar26 = (ushort *)(iVar29 + 0x11b94);
      puVar27 = puVar26 + (uVar8 - 1) * 4 + (uint)*puVar26 * 0x13 + 8;
      puVar28 = puVar26 + (uint)*puVar27 * 0x13 + 8;
      (*(code *)PTR_memcpy_0045ad34)(local_88,puVar28,0x1c);
      local_76 = puVar27[1];
      local_6a = (short)(1 << (local_82 & 0x1f)) + -1;
      local_6c = (short)(1 << (local_84 & 0x1f)) + -1;
      uVar8 = (uint)*puVar26 * 0x26 + (uint)*(ushort *)(iVar29 + 0x11b96) * 8 + 0x10 >> 1;
      local_68 = iVar29 + (uVar8 + puVar28[0xe] + 0x8dc8) * 2 + 4;
      iVar25 = uVar8 + *(ushort *)(iVar29 + 0x11b9a);
      local_64 = iVar29 + (iVar25 + (uint)puVar28[0xf] + 0x8dc8) * 2 + 4;
      iVar25 = iVar25 + (uint)*(ushort *)(iVar29 + 0x11b9a);
      local_60 = (ushort *)(iVar29 + (iVar25 + (uint)puVar27[3] + 0x8dc8) * 2 + 4);
      iVar25 = iVar25 + (uint)*(ushort *)(iVar29 + 0x11b9e);
      local_58 = (ushort *)(iVar29 + (iVar25 + (uint)puVar27[2] + 0x8dc8) * 2 + 4);
      iVar25 = iVar25 + (uint)*(ushort *)(iVar29 + 0x11ba0);
      local_50 = iVar29 + (iVar25 + 0x8dc8) * 2 + 4;
      iVar25 = iVar25 + (uint)*(ushort *)(iVar29 + 0x11ba2);
      local_54 = iVar29 + (iVar25 + 0x8dc8) * 2 + 4;
      local_5c = iVar29 + ((uint)puVar28[0x11] + (uint)*(ushort *)(iVar29 + 0x11b9c) +
                           (uint)puVar28[0x12] * 0x10000 + iVar25 + 0x8dc8) * 2 + 4;
      local_44 = *(undefined2 *)(iVar29 + 0x11b98);
      local_4c = (*(code *)PTR_calloc_0045ad68)((local_78 + 1) * 2,2);
      if (local_4c == 0) {
        return 0;
      }
      local_48 = local_4c + (local_78 + 1) * 2;
      (*(code *)puVar5)(local_4c,local_68,(uint)local_7a << 1);
      (*(code *)puVar5)(local_48,local_64,(uint)local_7a << 1);
      puVar26 = local_58;
      while (uVar8 = (uint)*puVar26, 1 < uVar8) {
        puVar27 = puVar26 + 1;
        puVar26 = puVar26 + 2;
        uVar14 = *puVar27;
        do {
          iVar29 = (1 - (uint)*puVar26) * -2;
          uVar8 = uVar8 - 1;
          *(ushort *)(local_4c + iVar29) = uVar14;
          *(ushort *)(local_48 + iVar29) = puVar26[1];
          puVar26 = puVar26 + 2;
          uVar14 = uVar14 + 1;
        } while (uVar8 != 0);
      }
      while (puVar26[1] != 0) {
        iVar29 = (1 - (uint)puVar26[1]) * -2;
        puVar27 = puVar26 + 2;
        puVar26 = puVar26 + 3;
        *(ushort *)(local_4c + iVar29) = *puVar27;
        *(ushort *)(local_48 + iVar29) = *puVar26;
      }
      for (uVar8 = 0; uVar8 < *local_60; uVar8 = uVar8 + 1 & 0xffff) {
        puVar26 = local_60 + local_60[uVar8];
        while( true ) {
          puVar26 = puVar26 + 1;
          if (*puVar26 == 0) break;
          do {
            uVar14 = *puVar26;
            puVar26 = puVar26 + 1;
          } while (uVar14 != 0);
        }
      }
    }
    (*(code *)PTR_free_0045ac9c)(param_2[0x2c7]);
    (*(code *)PTR_memcpy_0045ad34)(param_2 + 0x2b8,local_88,0x48);
  }
  puVar6 = PTR___locale_mmap_0045adc4;
  puVar5 = PTR_memcpy_0045ad34;
  local_40 = param_2 + 0x25a;
  local_3c = param_2 + 0x25b;
  local_38 = (int)param_2 + 0x92e;
  local_34 = param_2 + 0x25e;
  local_30 = param_2 + 0xc3;
  local_2c = param_2 + 0x183;
  iVar29 = 0;
  while( true ) {
    bVar1 = *pbVar30;
    bVar2 = pbVar30[1];
    if (((uint)*pbVar31 != (uint)bVar1) || ((uint)bVar2 != (uint)pbVar31[1])) {
      *pbVar31 = bVar1;
      iVar25 = (bVar2 & 0x7f) + (bVar1 & 0x7f) * 0x80;
      pbVar31[1] = pbVar30[1];
      if (iVar29 != 4) {
        iVar24 = *(int *)puVar6;
        uVar8 = (uint)*(byte *)(iVar24 + iVar29 + 0x3e39e);
        if (uVar8 != 0) {
          uVar14 = *(ushort *)((int)param_2 + (iVar29 + 0x48c) * 2 + 2);
          piVar9 = (int *)(iVar24 + 0x3e3a4 + iVar29 * 0x10);
          iVar22 = piVar9[1];
          iVar15 = piVar9[3];
          iVar19 = *piVar9;
          iVar10 = piVar9[2];
          bVar1 = *(byte *)(iVar25 * 9 + iVar24 + iVar29 + 0x3e407);
          iVar23 = 0;
          do {
            iVar11 = iVar23 * 2;
            iVar16 = iVar24 + iVar19 + iVar23;
            iVar17 = iVar23 * 4;
            iVar23 = iVar23 + 1;
            *(uint *)((int)param_2 + iVar17 + (uint)uVar14) =
                 iVar24 + iVar15 +
                 (uint)*(ushort *)
                        (iVar24 + iVar10 +
                        ((uint)*(ushort *)(iVar24 + iVar22 + iVar11) +
                        (uint)*(byte *)(iVar16 + bVar1 * uVar8)) * 2);
          } while (iVar23 < (int)uVar8);
        }
      }
      puVar4 = PTR_s__a__d__b__Y__T_0045abc0;
      if (iVar29 == 0) {
        iVar24 = *(int *)puVar6;
        uVar8 = (uint)*(byte *)(iVar25 * 9 + iVar24 + 0x3e406);
        if (uVar8 < 3) {
          if (uVar8 == 2) {
            *(undefined1 *)(param_2 + 0x24b) = 1;
            *(undefined1 *)((int)param_2 + 0x92d) = 6;
            param_2[0x268] = (int)(puVar4 + -0x53b0);
          }
          else {
            *(undefined1 *)((int)param_2 + 0x92d) = 1;
            param_2[0x268] = (int)(puVar4 + -0x53a8);
            *(undefined1 *)(param_2 + 0x24b) = 0;
          }
        }
        else {
          iVar25 = (uVar8 - 3) * 0x56;
          param_2[0x268] = iVar24 + 0x3f2a8 + (uint)*(byte *)(iVar24 + (uVar8 - 3) + 0x3f2a8);
          *(undefined1 *)(param_2 + 0x24b) = 2;
          *(undefined1 *)((int)param_2 + 0x92d) = 1;
          puVar4 = PTR___C_ctype_b_0045ad2c;
          iVar10 = *(int *)puVar6;
          param_2[0x24e] = iVar10 + iVar25 + 0xc2c6;
          param_2[0x252] = iVar10 + iVar25 + 0xc2e6;
          param_2[0x250] = iVar10 + iVar25 + 0xc2d6;
          param_2[0x254] = iVar10 + iVar25 + 0xc2f6;
          (*(code *)puVar5)(param_2 + 3,*(int *)puVar4 + -0x100,0x300);
          (*(code *)puVar5)(local_30,*(int *)PTR___GI___C_ctype_tolower_0045ad24 + -0x100,0x300);
          (*(code *)puVar5)(local_2c,*(int *)PTR___C_ctype_toupper_0045ada4 + -0x100,0x300);
          uVar8 = 0;
          iVar24 = 0;
          while( true ) {
            uVar12 = (uint)*(byte *)(param_2[0x24f] +
                                     (uint)*(byte *)(iVar10 + iVar25 + iVar24 + 0xc2c6) * 4 +
                                    ((uVar8 & 7) >> 1));
            uVar13 = (int)uVar12 >> 4;
            if ((uVar8 & 1) == 0) {
              uVar13 = uVar12 & 0xf;
            }
            uVar12 = uVar8 & 0xff;
            uVar14 = *(ushort *)(param_2[0x256] + uVar13 * 2);
            *(ushort *)((int)param_2 + (uVar8 + 0x104) * 2 + 4) = uVar14;
            uVar18 = (undefined2)(uVar8 + 0x80);
            if (uVar12 != 0x7f) {
              *(ushort *)((int)param_2 + ((char)((char)uVar8 + -0x80) + 0x84) * 2 + 4) = uVar14;
            }
            *(undefined2 *)((int)param_2 + (uVar8 + 0x404) * 2 + 4) = uVar18;
            *(undefined2 *)((int)param_2 + (uVar8 + 0x284) * 2 + 4) = uVar18;
            if ((uVar14 & 3) != 0) {
              bVar1 = *(byte *)(param_2[0x251] +
                                (uint)*(byte *)(iVar10 + iVar25 + iVar24 + 0xc2d6) * 8 +
                               (uVar8 + 0x80 & 7));
              cVar21 = (char)(uVar12 - 0x80);
              sVar3 = (short)(uVar12 - 0x80);
              if ((uVar14 & 2) == 0) {
                iVar24 = cVar21 + 0x204;
                uVar14 = sVar3 - (ushort)bVar1 & 0xff;
                *(ushort *)((int)param_2 + (uVar8 + 0x284) * 2 + 4) = uVar14;
              }
              else {
                iVar24 = cVar21 + 900;
                uVar14 = (ushort)bVar1 + sVar3 & 0xff;
                *(ushort *)((int)param_2 + (uVar8 + 0x404) * 2 + 4) = uVar14;
              }
              if (uVar12 != 0x7f) {
                *(ushort *)((int)param_2 + iVar24 * 2 + 4) = uVar14;
              }
            }
            uVar8 = uVar8 + 1;
            if (0x7f < (int)uVar8) break;
            iVar24 = (int)uVar8 >> 3;
          }
          *param_2 = (int)(param_2 + 0x43);
          param_2[1] = (int)(param_2 + 0x103);
          param_2[2] = (int)(param_2 + 0x1c3);
        }
        iVar25 = 0;
        do {
          uVar7 = (*(code *)PTR_strlen_0045aca0)(local_34[iVar25]);
          puVar20 = (undefined1 *)(local_38 + iVar25);
          iVar25 = iVar25 + 1;
          *puVar20 = uVar7;
        } while (iVar25 < 10);
      }
      else if (iVar29 == 1) {
        iVar25 = (*(code *)PTR___locale_mbrtowc_l_0045ac60)(local_40,param_2[0x269],param_2);
        param_2[0x25c] = iVar25;
        if (*(char *)param_2[0x26b] != '\0') {
          iVar25 = (*(code *)PTR___locale_mbrtowc_l_0045ac60)(local_3c,param_2[0x26a],param_2);
          param_2[0x25d] = iVar25;
          if (iVar25 == 0) {
            param_2[0x26b] = param_2[0x26a];
          }
        }
      }
    }
    iVar29 = iVar29 + 1;
    pbVar30 = pbVar30 + 2;
    if (5 < iVar29) break;
    pbVar31 = pbVar31 + 2;
  }
  return 1;
}



/* ---- _locale_init_l @ 00408a44 ----
 * (library/support function; see decompiled body)
 */

void _locale_init_l(undefined4 *param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined *UNRECOVERED_JUMPTABLE;
  
  puVar2 = PTR___locale_mmap_0045adc4;
  (*(code *)PTR_memset_0045ad90)(param_1 + 0x243,0,0xe);
  *(undefined1 *)(param_1 + 0x243) = 0x23;
  (*(code *)PTR_memcpy_0045ad34)((int)param_1 + 0x926,*(int *)puVar2 + 0x3e39e,6);
  *(char *)((int)param_1 + 0x926) = *(char *)((int)param_1 + 0x926) + '\x01';
  *(undefined2 *)((int)param_1 + 0x91a) = 0x978;
  *(undefined2 *)(param_1 + 0x247) = 0x9a4;
  *(undefined2 *)((int)param_1 + 0x91e) = 0x9b0;
  *(undefined2 *)(param_1 + 0x248) = 0xa08;
  *(undefined2 *)(param_1 + 0x249) = 0xad0;
  UNRECOVERED_JUMPTABLE = PTR__locale_set_l_0045ac64;
  puVar1 = PTR_s__a__d__b__Y__T_0045abc0;
  param_1[0x24f] = *(undefined4 *)puVar2;
  param_1[0x251] = *(int *)puVar2 + 0x1a8;
  param_1[0x253] = *(int *)puVar2 + 0x3a0;
  param_1[0x255] = *(int *)puVar2 + 0x1160;
  param_1[599] = *(int *)puVar2 + 0x2310;
  param_1[600] = *(int *)puVar2 + 0x39a0;
  param_1[0x259] = *(int *)puVar2 + 0x4168;
  *param_1 = *(undefined4 *)PTR___C_ctype_b_0045ad2c;
  param_1[1] = *(undefined4 *)PTR___GI___C_ctype_tolower_0045ad24;
  puVar2 = PTR_s__a__d__b__Y__T_0045abc0;
  param_1[2] = *(undefined4 *)PTR___C_ctype_toupper_0045ada4;
  param_1[0x256] = puVar2 + -0x53d0;
                    /* WARNING: Could not recover jumptable at 0x00408b80. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)UNRECOVERED_JUMPTABLE)(puVar1 + -0x53e0,param_1);
  return;
}



/* ---- _locale_init @ 00408b88 ----
 * (library/support function; see decompiled body)
 */

void _locale_init(void)

{
                    /* WARNING: Could not recover jumptable at 0x00408b9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR__locale_init_l_0045ac68)(*(undefined4 *)PTR___global_locale_0045aca4);
  return;
}



/* ---- __locale_mbrtowc_l @ 00408bb0 ----
 * (library/support function; see decompiled body)
 */

uint __locale_mbrtowc_l(uint *param_1,byte *param_2,int param_3)

{
  uint uVar1;
  byte *local_20;
  undefined4 local_1c [4];
  
  if (*(char *)(param_3 + 0x92c) == '\x01') {
    local_1c[0] = 0;
    local_20 = param_2;
    uVar1 = (*(code *)PTR__wchar_utf8sntowcs_0045ac4c)(param_1,1,&local_20,0xffffffff,local_1c,1);
    if (uVar1 == 1) {
      uVar1 = (int)local_20 - (int)param_2;
    }
  }
  else {
    uVar1 = (uint)*param_2;
    *param_1 = uVar1;
    if (uVar1 < 0x80) {
      uVar1 = (uint)(*param_2 != 0);
    }
    else {
      if ((*(char *)(param_3 + 0x92c) == '\x02') &&
         (uVar1 = (uint)*(ushort *)
                         (*(int *)(param_3 + 0x94c) +
                         ((uint)*(byte *)(*(int *)(param_3 + 0x948) + ((int)(uVar1 - 0x80) >> 3)) *
                          8 + (uVar1 - 0x80 & 7)) * 2), *param_1 = uVar1, uVar1 != 0)) {
        return 1;
      }
      uVar1 = 0xffffffff;
    }
  }
  return uVar1;
}



/* ---- _wchar_utf8sntowcs @ 00408cb0 ----
 * (library/support function; see decompiled body)
 */

int _wchar_utf8sntowcs(uint *param_1,int param_2,undefined4 *param_3,int param_4,uint *param_5,
                      int param_6)

{
  bool bVar1;
  byte bVar2;
  undefined *puVar3;
  undefined4 *puVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  byte *pbVar8;
  uint uVar9;
  uint local_10 [3];
  
  puVar3 = PTR___GI___errno_location_0045acc8;
  pbVar8 = (byte *)*param_3;
  if (param_1 == (uint *)0x0) {
    uVar9 = 0;
    param_1 = local_10;
    param_2 = -1;
  }
  else {
    bVar1 = param_1 == param_5;
    if (bVar1) {
      param_1 = local_10;
    }
    uVar9 = (uint)!bVar1;
    if (param_2 == 0) {
      return 0;
    }
  }
  uVar6 = *param_5;
  iVar5 = param_2;
  if (uVar6 == 0) goto LAB_00408d30;
  uVar7 = param_5[1];
  if (uVar7 != 0xffff) {
    if (param_4 == 0) {
LAB_00408e8c:
      *param_5 = 0;
      if (param_1 != local_10) {
        *param_3 = pbVar8;
      }
      return param_2 - iVar5;
    }
    bVar2 = *pbVar8;
    while (((int)(char)bVar2 & 0xc0U) == 0x80) {
      uVar7 = ((int)(char)bVar2 & 0x3fU) + uVar7 * 0x40;
      param_4 = param_4 + -1;
      uVar6 = uVar6 << 5;
      pbVar8 = pbVar8 + 1;
      while (uVar7 = uVar7 & ~(uVar6 << 1), (uVar7 & uVar6) == 0) {
        uVar6 = uVar6 >> 5;
        if (uVar6 == 0x40) {
          uVar6 = 0x80;
        }
        if (((uVar7 < uVar6) || (uVar7 - 0xfffe < 2)) || (uVar7 - 0xd800 < 0x800))
        goto LAB_00408d6c;
        do {
          *param_1 = uVar7;
          param_1 = param_1 + uVar9;
          if (uVar7 == 0) {
            pbVar8 = (byte *)0x0;
            goto LAB_00408e8c;
          }
          iVar5 = iVar5 + -1;
          if (iVar5 == 0) goto LAB_00408e8c;
LAB_00408d30:
          if (param_4 == 0) goto LAB_00408e8c;
          uVar7 = (uint)*pbVar8;
          param_4 = param_4 + -1;
          pbVar8 = pbVar8 + 1;
        } while (uVar7 < 0x80);
        uVar6 = 0x40;
        if (0x3b < (uVar7 + 0x3e & 0xff)) goto LAB_00408d6c;
      }
      if (param_4 == 0) {
        if (param_6 != 0) {
          param_5[1] = uVar7;
          *param_5 = uVar6;
          *param_3 = pbVar8;
          return -2;
        }
        if (iVar5 != param_2) {
          return 0;
        }
        do {
          uVar6 = uVar6 >> 5;
          pbVar8 = pbVar8 + -1;
          iVar5 = param_2;
        } while (0x3f < uVar6);
        goto LAB_00408e8c;
      }
      bVar2 = *pbVar8;
    }
LAB_00408d6c:
    param_5[1] = 0xffff;
    *param_5 = uVar6;
  }
  puVar4 = (undefined4 *)(*(code *)puVar3)();
  *puVar4 = 0x58;
  return -1;
}



/* ---- _wchar_wcsntoutf8s @ 00408eb0 ----
 * (library/support function; see decompiled body)
 */

int _wchar_wcsntoutf8s(byte *param_1,uint param_2,byte *param_3,int param_4)

{
  bool bVar1;
  byte bVar2;
  bool bVar3;
  undefined4 *puVar4;
  byte *pbVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint *puVar10;
  byte local_18 [16];
  
  if (param_1 == (byte *)0x0) {
    param_2 = 0xffffffff;
  }
  else {
    bVar3 = true;
    if (param_1 != param_3) goto LAB_00408ef0;
  }
  param_1 = local_18;
  bVar3 = false;
LAB_00408ef0:
  puVar10 = *(uint **)param_3;
  uVar7 = param_2;
  do {
    if ((param_4 == 0) || (uVar7 == 0)) goto LAB_00408ff4;
    uVar6 = *puVar10;
    *param_1 = (byte)uVar6;
    if (uVar6 < 0x80) {
      uVar9 = 1;
      if (uVar6 == 0) {
        puVar10 = (uint *)0x0;
LAB_00408ff4:
        if (bVar3) {
          *(uint **)param_3 = puVar10;
        }
        return param_2 - uVar7;
      }
    }
    else {
      if ((((int)uVar6 < 0) || (uVar6 - 0xfffe < 2)) || (uVar6 - 0xd800 < 0x800)) {
        puVar4 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
        *puVar4 = 0x58;
        return -1;
      }
      uVar6 = uVar6 >> 1;
      pbVar5 = param_1;
      do {
        uVar6 = uVar6 >> 5;
        pbVar5 = pbVar5 + 1;
      } while (uVar6 != 0);
      uVar9 = (int)pbVar5 - (int)param_1;
      uVar6 = *puVar10;
      if (uVar7 < uVar9) goto LAB_00408ff4;
      uVar8 = 0xffffff80;
      while( true ) {
        bVar2 = (byte)uVar6;
        bVar1 = pbVar5 <= param_1;
        uVar6 = uVar6 >> 6;
        pbVar5 = pbVar5 + -1;
        if (bVar1) break;
        *pbVar5 = bVar2 & 0x3f | 0x80;
        uVar8 = (int)uVar8 >> 1 | 0xffffff80;
      }
      *param_1 = (byte)(uVar8 << 1) | *param_1;
    }
    if (bVar3) {
      param_1 = param_1 + uVar9;
    }
    puVar10 = puVar10 + 1;
    param_4 = param_4 + -1;
    uVar7 = uVar7 - uVar9;
  } while( true );
}



/* ---- fseek @ 00409010 ----
 * (library/support function; see decompiled body)
 */

int fseek(FILE *__stream,__off_t __off,int __whence)

{
  int iVar1;
  
  iVar1 = (*(code *)PTR_fseeko64_0045acdc)(__stream,__off,__off,__off >> 0x1f,__whence);
  return iVar1;
}



/* ---- fseeko64 @ 00409050 ----
 * (library/support function; see decompiled body)
 */

int fseeko64(FILE *__stream,__off64_t __off,int __whence)

{
  ushort uVar1;
  undefined4 *puVar2;
  int iVar3;
  char *pcVar4;
  int iVar5;
  _IO_marker *p_Var6;
  __off64_t local_30;
  undefined1 auStack_28 [20];
  
  local_30 = __off;
  if (2 < (uint)__whence) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0x16;
    return -1;
  }
  iVar5 = *(int *)&__stream->field_0x4c;
  if (iVar5 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad04)
              (auStack_28,PTR___pthread_mutex_trylock_0045ad64,&__stream->_offset);
    (*(code *)PTR___pthread_mutex_trylock_0045adcc)(&__stream->_offset);
    uVar1 = (ushort)__stream->_flags;
  }
  else {
    uVar1 = (ushort)__stream->_flags;
  }
  if (((uVar1 & 0x40) == 0) ||
     (iVar3 = (*(code *)PTR___stdio_wcommit_0045abc4)(__stream), iVar3 == 0)) {
    if (__whence == 1) {
      iVar3 = (*(code *)PTR___stdio_adjust_position_0045ac6c)(__stream,&local_30);
      if (iVar3 < 0) goto LAB_0040917c;
      p_Var6 = __stream->_markers;
    }
    else {
      p_Var6 = __stream->_markers;
    }
    if ((p_Var6 != (_IO_marker *)0x0) &&
       (iVar3 = (*(code *)p_Var6)(__stream->_IO_save_base,&local_30,__whence), -1 < iVar3)) {
      iVar3 = __stream->_flags;
      pcVar4 = __stream->_IO_read_end;
      *(undefined1 *)((int)&__stream->_flags + 2) = 0;
      *(ushort *)&__stream->_flags = (ushort)iVar3 & 0xffb8;
      __stream->_IO_buf_base = pcVar4;
      __stream->_IO_write_base = pcVar4;
      __stream->_IO_write_ptr = pcVar4;
      __stream->_IO_write_end = pcVar4;
      __stream->_old_offset = 0;
      iVar3 = 0;
      goto LAB_00409180;
    }
  }
LAB_0040917c:
  iVar3 = -1;
LAB_00409180:
  if (iVar5 == 0) {
    (*(code *)PTR__pthread_cleanup_push_defer_0045ad9c)(auStack_28,1);
  }
  return iVar3;
}



/* ---- __stdio_adjust_position @ 004091c0 ----
 * (library/support function; see decompiled body)
 */

int __stdio_adjust_position(ushort *param_1,uint *param_2)

{
  ushort uVar1;
  undefined4 *puVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  
  uVar1 = *param_1;
  iVar4 = 0;
  if ((uVar1 & 3) != 0) {
    iVar4 = (uVar1 & 3) - 1;
    if ((iVar4 != 0) && ((uVar1 & 0x800) != 0)) {
      if (iVar4 == 2) {
        return -1;
      }
      if (*(int *)(param_1 + 0x1e) != 0) {
        return -1;
      }
      iVar4 = -(uint)*(byte *)((int)param_1 + 3);
      if (0 < *(int *)(param_1 + 0x20)) {
        iVar4 = iVar4 - (uint)(byte)param_1[1];
      }
    }
  }
  if ((uVar1 & 0x40) == 0) {
    iVar7 = *(int *)(param_1 + 10);
  }
  else {
    iVar7 = *(int *)(param_1 + 4);
  }
  uVar6 = param_2[1];
  iVar7 = iVar7 + (iVar4 - *(int *)(param_1 + 8));
  uVar5 = *param_2 - iVar7;
  uVar8 = (uint)(*param_2 < uVar5);
  uVar3 = (uVar6 - (iVar7 >> 0x1f)) - uVar8;
  *param_2 = uVar5;
  param_2[1] = uVar3;
  if (((int)uVar6 < (int)uVar3) || ((uVar3 == uVar6 && (uVar8 != 0)))) {
    iVar7 = -iVar7;
  }
  if (iVar7 < 0) {
    puVar2 = (undefined4 *)(*(code *)PTR___GI___errno_location_0045acc8)();
    *puVar2 = 0x4f;
  }
  return iVar7;
}



/* ---- strchr @ 004092d0 ----
 * (library/support function; see decompiled body)
 */

char * strchr(char *__s,int __c)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  
  uVar5 = __c & 0xff;
  while (((uint)__s & 3) != 0) {
    uVar1 = *(uint *)__s;
    if ((byte)uVar1 == uVar5) {
      return (char *)(uint *)__s;
    }
    __s = (char *)((int)__s + 1);
    if ((byte)uVar1 == 0) {
      return (char *)0x0;
    }
  }
  uVar1 = uVar5 << 8 | uVar5;
  do {
    uVar3 = *(uint *)__s;
    puVar4 = (uint *)__s;
    while (__s = (char *)(puVar4 + 1), uVar2 = uVar3 ^ (uVar1 << 0x10 | uVar1),
          ((~uVar2 ^ uVar2 + 0x7efefeff | ~uVar3 ^ uVar3 + 0x7efefeff) & 0x81010100) != 0) {
      if ((byte)*puVar4 == uVar5) {
        return (char *)puVar4;
      }
      if ((byte)*puVar4 == 0) {
        return (char *)0x0;
      }
      if (*(byte *)((int)puVar4 + 1) == uVar5) {
        return (char *)(byte *)((int)puVar4 + 1);
      }
      if (*(byte *)((int)puVar4 + 1) == 0) {
        return (char *)0x0;
      }
      if (*(byte *)((int)puVar4 + 2) == uVar5) {
        return (char *)(byte *)((int)puVar4 + 2);
      }
      if (*(byte *)((int)puVar4 + 2) == 0) {
        return (char *)0x0;
      }
      if (*(byte *)((int)puVar4 + 3) == uVar5) {
        return (char *)(byte *)((int)puVar4 + 3);
      }
      if (*(byte *)((int)puVar4 + 3) == 0) {
        return (char *)0x0;
      }
      uVar3 = *(uint *)__s;
      puVar4 = (uint *)__s;
    }
  } while( true );
}



/* ---- __do_global_ctors_aux @ 004093c0 ----
 * (library/support function; see decompiled body)
 */

void __do_global_ctors_aux(void)

{
  undefined4 *puVar1;
  code *pcVar2;
  
  if (__CTOR_LIST__ != (code *)0xffffffff) {
    puVar1 = &__CTOR_LIST__;
    pcVar2 = __CTOR_LIST__;
    do {
      puVar1 = puVar1 + -1;
      (*pcVar2)();
      pcVar2 = (code *)*puVar1;
    } while (pcVar2 != (code *)0xffffffff);
  }
  return;
}



/* ---- _fini @ 00409420 ----
 * (library/support function; see decompiled body)
 */

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


