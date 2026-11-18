"/*\n * boxdiag.annotated.c  --  annotated Ghidra decompile of root/bin/boxdiag\n * ============================================================================\n * Hardware diagnostic / initialization tool for the ViXS XCode A/V capture\n * path. Through /dev/vixs/xcodedrv it initializes and tests the video decoder\n * chips (ADV7185, SAA711x, 'Mot') with ioctl register sequences and host<->\n * frame-buffer DMA. See docs/boot-process.md 6.2.\n *\n * NOTE: this binary is STRIPPED, so only the libc PLT stubs (renamed here from\n * the ELF .rel.plt) carry names; the program's own functions appear as Ghidra\n * FUN_<addr>. The hardware intent is read from the embedded strings\n * (Adv_7185_Init, Ic106_Sa711x_Init, Sa711x_Init, Mot_Init, 'DMA ... Frame\n * Buffer Failed'). Not produced by running the binary.\n */"

/* ---- _init @ 00400954 ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  if (PTR___gmon_start___0041db54 != (undefined *)0x0) {
    (*(code *)PTR___gmon_start___0041db54)();
  }
  FUN_00400ce0();
  iVar1 = FUN_0040bc20();
  return iVar1;
}



/* ---- sprintf @ 004009e0 ----
 * (library/support function; see decompiled body)
 */

void sprintf(void)

{
  (*(code *)PTR_sprintf_0041d788)();
  return;
}



/* ---- open @ 004009f0 ----
 * (library/support function; see decompiled body)
 */

void open(void)

{
                    /* WARNING: Could not recover jumptable at 0x004009fc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_open_0041d78c)();
  return;
}



/* ---- memcmp @ 00400a00 ----
 * (library/support function; see decompiled body)
 */

void memcmp(void)

{
  (*(code *)PTR_memcmp_0041d790)();
  return;
}



/* ---- getchar @ 00400a10 ----
 * (library/support function; see decompiled body)
 */

void getchar(void)

{
  (*(code *)PTR_getchar_0041d794)();
  return;
}



/* ---- signal @ 00400a20 ----
 * (library/support function; see decompiled body)
 */

void signal(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400a2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_signal_0041d798)();
  return;
}



/* ---- calloc @ 00400a30 ----
 * (library/support function; see decompiled body)
 */

void calloc(void)

{
  (*(code *)PTR_calloc_0041d79c)();
  return;
}



/* ---- putchar @ 00400a40 ----
 * (library/support function; see decompiled body)
 */

void putchar(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400a4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_putchar_0041d7a0)();
  return;
}



/* ---- memset @ 00400a50 ----
 * (library/support function; see decompiled body)
 */

void memset(void)

{
  (*(code *)PTR_memset_0041d7a4)();
  return;
}



/* ---- __libc_start_main @ 00400a60 ----
 * (library/support function; see decompiled body)
 */

void __libc_start_main(void)

{
  (*(code *)PTR___libc_start_main_0041d7a8)();
  return;
}



/* ---- tcgetattr @ 00400a70 ----
 * (library/support function; see decompiled body)
 */

void tcgetattr(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400a7c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_tcgetattr_0041d7ac)();
  return;
}



/* ---- usleep @ 00400a80 ----
 * (library/support function; see decompiled body)
 */

void usleep(void)

{
  (*(code *)PTR_usleep_0041d7b0)();
  return;
}



/* ---- free @ 00400a90 ----
 * (library/support function; see decompiled body)
 */

void free(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400a9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_free_0041d7b4)();
  return;
}



/* ---- fflush @ 00400aa0 ----
 * (library/support function; see decompiled body)
 */

void fflush(void)

{
  (*(code *)PTR_fflush_0041d7b8)();
  return;
}



/* ---- ioctl @ 00400ab0 ----
 * (library/support function; see decompiled body)
 */

void ioctl(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400abc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_ioctl_0041d7bc)();
  return;
}



/* ---- fclose @ 00400ac0 ----
 * (library/support function; see decompiled body)
 */

void fclose(void)

{
  (*(code *)PTR_fclose_0041d7c0)();
  return;
}



/* ---- memcpy @ 00400ad0 ----
 * (library/support function; see decompiled body)
 */

void memcpy(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400adc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memcpy_0041d7c4)();
  return;
}



/* ---- strlen @ 00400ae0 ----
 * (library/support function; see decompiled body)
 */

void strlen(void)

{
  (*(code *)PTR_strlen_0041d7c8)();
  return;
}



/* ---- feof @ 00400af0 ----
 * (library/support function; see decompiled body)
 */

void feof(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400afc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_feof_0041d7cc)();
  return;
}



/* ---- strcpy @ 00400b00 ----
 * (library/support function; see decompiled body)
 */

void strcpy(void)

{
  (*(code *)PTR_strcpy_0041d7d0)();
  return;
}



/* ---- printf @ 00400b10 ----
 * (library/support function; see decompiled body)
 */

void printf(void)

{
  (*(code *)PTR_printf_0041d7d4)();
  return;
}



/* ---- close @ 00400b20 ----
 * (library/support function; see decompiled body)
 */

void close(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_0041d7d8)();
  return;
}



/* ---- fwrite @ 00400b30 ----
 * (library/support function; see decompiled body)
 */

void fwrite(void)

{
  (*(code *)PTR_fwrite_0041d7dc)();
  return;
}



/* ---- fprintf @ 00400b40 ----
 * (library/support function; see decompiled body)
 */

void fprintf(void)

{
  (*(code *)PTR_fprintf_0041d7e0)();
  return;
}



/* ---- fopen @ 00400b50 ----
 * (library/support function; see decompiled body)
 */

void fopen(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b5c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fopen_0041d7e4)();
  return;
}



/* ---- puts @ 00400b60 ----
 * (library/support function; see decompiled body)
 */

void puts(void)

{
  (*(code *)PTR_puts_0041d7e8)();
  return;
}



/* ---- tcsetattr @ 00400b70 ----
 * (library/support function; see decompiled body)
 */

void tcsetattr(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b7c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_tcsetattr_0041d7ec)();
  return;
}



/* ---- sscanf @ 00400b80 ----
 * (library/support function; see decompiled body)
 */

void sscanf(void)

{
  (*(code *)PTR_sscanf_0041d7f0)();
  return;
}



/* ---- htonl @ 00400b90 ----
 * (library/support function; see decompiled body)
 */

void htonl(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_htonl_0041d7f4)();
  return;
}



/* ---- strncmp @ 00400ba0 ----
 * (library/support function; see decompiled body)
 */

void strncmp(void)

{
  (*(code *)PTR_strncmp_0041d7f8)();
  return;
}



/* ---- fread @ 00400bb0 ----
 * (library/support function; see decompiled body)
 */

void fread(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400bbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fread_0041d7fc)();
  return;
}



/* ---- strcmp @ 00400bc0 ----
 * (library/support function; see decompiled body)
 */

void strcmp(void)

{
  (*(code *)PTR_strcmp_0041d800)();
  return;
}



/* ---- exit @ 00400bd0 ----
 * (library/support function; see decompiled body)
 */

void exit(void)

{
  (*(code *)PTR_exit_0041d804)();
  return;
}



/* ---- entry @ 00400bf0 ----
 * (library/support function; see decompiled body)
 */

void processEntry entry(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined1 auStack_20 [16];
  undefined1 *local_10;
  undefined4 local_c;
  undefined1 *local_8;
  
  local_8 = auStack_20;
  local_10 = &LAB_0040bb60;
  local_c = param_1;
  __libc_start_main(FUN_0040aec0,param_2,&param_3,FUN_0040bb68);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* ---- FUN_00400c40 @ 00400c40 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400c94) */
/* WARNING: Removing unreachable block (ram,0x00400c98) */
/* WARNING: Removing unreachable block (ram,0x00400cbc) */

void FUN_00400c40(void)

{
  if (DAT_0041db80 == '\0') {
    DAT_0041db80 = '\x01';
  }
  return;
}



/* ---- FUN_00400ce0 @ 00400ce0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400cfc) */

void FUN_00400ce0(void)

{
  return;
}



/* ---- FUN_00400d10 @ 00400d10 ----
 * (library/support function; see decompiled body)
 */

int FUN_00400d10(int param_1,undefined4 param_2)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 uVar3;
  int local_18;
  
  local_18 = 0;
  while( true ) {
    if (*(int *)(param_1 + local_18 * 8) == 0) {
      return -1;
    }
    uVar3 = *(undefined4 *)(param_1 + local_18 * 8);
    uVar1 = strlen(param_2);
    iVar2 = strncmp(uVar3,param_2,uVar1);
    if (iVar2 == 0) break;
    local_18 = local_18 + 1;
  }
  return local_18;
}



/* ---- FUN_00400de4 @ 00400de4 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00400de4(int param_1,char *param_2)

{
  int iVar1;
  undefined4 local_28;
  char *local_20;
  undefined1 auStack_1c [20];
  
  local_28 = 0;
  for (local_20 = param_2; (*local_20 != '\0' && (*local_20 == ' ')); local_20 = local_20 + 1) {
  }
  sscanf(local_20,&DAT_0040bcd0,auStack_1c);
  for (; (*local_20 != '\0' && (*local_20 != ' ')); local_20 = local_20 + 1) {
  }
  for (; (*local_20 != '\0' && (*local_20 == ' ')); local_20 = local_20 + 1) {
  }
  iVar1 = strcmp(auStack_1c,&DAT_0040bcd4);
  if (iVar1 == 0) {
    memcpy(auStack_1c,&DAT_0040bcd8,3);
  }
  else {
    iVar1 = strcmp(auStack_1c,&DAT_0040bcdc);
    if (iVar1 == 0) {
      memcpy(auStack_1c,&DAT_0040bce0,3);
    }
  }
  iVar1 = FUN_00400d10(param_1,auStack_1c);
  if (iVar1 < 0) {
    puts("Error: command not found\n");
  }
  else {
    local_28 = (**(code **)(param_1 + iVar1 * 8 + 4))(auStack_1c,iVar1,local_20);
  }
  return local_28;
}



/* ---- FUN_00401010 @ 00401010 ----
 * (library/support function; see decompiled body)
 */

void FUN_00401010(void)

{
  int iVar1;
  
  do {
    printf(&DAT_0040bdf8);
    FUN_0040b410(&DAT_0041e428);
    iVar1 = FUN_00400de4(&PTR_DAT_0041d820,&DAT_0041e428);
  } while (iVar1 != 1);
  return;
}



/* ---- FUN_0040109c @ 0040109c ----
 * (library/support function; see decompiled body)
 */

bool FUN_0040109c(int param_1,int param_2)

{
  if (param_1 == 0) {
    printf("Usage: %s\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo exit program\n");
  }
  return param_1 != 0;
}



/* ---- FUN_00401140 @ 00401140 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00401140(undefined4 param_1,undefined4 param_2,char *param_3)

{
  int iVar1;
  uint local_20;
  undefined1 auStack_1c [20];
  
  if (*param_3 == '\0') {
    puts("Currently supported command(s):");
    for (local_20 = 0; (&PTR_DAT_0041d820)[local_20 * 2] != (undefined *)0x0;
        local_20 = local_20 + 1) {
      printf("%-18s",(&PTR_DAT_0041d820)[local_20 * 2]);
      if ((local_20 & 3) == 3) {
        putchar(10);
      }
    }
    if ((local_20 & 3) != 0) {
      putchar(10);
    }
    putchar(10);
  }
  else {
    iVar1 = sscanf(param_3,&DAT_0040be44,auStack_1c);
    if (iVar1 != 0) {
      for (local_20 = 0; (&PTR_DAT_0041d820)[local_20 * 2] != (undefined *)0x0;
          local_20 = local_20 + 1) {
        iVar1 = strcmp((&PTR_DAT_0041d820)[local_20 * 2],auStack_1c);
        if (iVar1 == 0) {
          (*(code *)(&PTR_FUN_0041d824)[local_20 * 2])(0,local_20,0);
          return 0;
        }
      }
    }
  }
  return 0;
}



/* ---- FUN_0040134c @ 0040134c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040134c(int param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  undefined4 uVar2;
  uint local_1c;
  int local_14;
  uint local_10 [2];
  
  if (param_1 == 0) {
    printf("Usage: %s <address/offset>\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tRead the content of a register with the specified address\n");
  }
  else {
    uVar1 = sscanf(param_3,"%lx %lx",&local_14,local_10);
    if (uVar1 == 0) {
      printf("Usage: %s <register offset>\n\n",param_1);
    }
    else {
      if (uVar1 < 2) {
        local_10[0] = 1;
      }
      for (local_1c = 0; local_1c < local_10[0]; local_1c = local_1c + 1) {
        uVar2 = FUN_0040b1f4(local_14);
        printf("0x%lx > 0x%lx\n",local_14,uVar2);
        local_14 = local_14 + 4;
      }
    }
  }
  return 0;
}



/* ---- FUN_004014d0 @ 004014d0 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004014d0(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_18;
  undefined4 local_14 [2];
  
  if (param_1 == 0) {
    printf("Usage: %s <address/offset> <32 bit data>\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tWrite the specified data to the register at ");
    puts("the specified address\n");
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx",&local_18,local_14);
    if (iVar1 == 2) {
      FUN_0040b1b0(local_18,local_14[0]);
      uVar2 = FUN_0040b1f4(local_18);
      printf("0x%lx < 0x%lx\n\n",local_18,uVar2);
    }
    else {
      printf("Usage: %s <register offset> <32 bits data>\n\n",param_1);
    }
  }
  return 0;
}



/* ---- FUN_00401648 @ 00401648 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00401648(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 *puVar2;
  undefined2 *puVar3;
  undefined4 *puVar4;
  undefined4 *local_30;
  undefined2 *local_2c;
  undefined1 *local_28;
  uint local_1c;
  int local_18;
  uint local_14;
  int local_10;
  char local_c [4];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] [+ / -] [new offset]\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo display/examine frame buffer memory\n");
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx %c %lx",&local_18,&local_14,local_c,&local_10);
    if (0 < iVar1) {
      DAT_0041db98 = local_18;
    }
    if (1 < iVar1) {
      DAT_0041d938 = local_14;
    }
    if (3 < iVar1) {
      if (local_c[0] == '+') {
        DAT_0041db98 = local_18 + local_10;
        local_18 = DAT_0041db98;
      }
      else if (local_c[0] == '-') {
        DAT_0041db98 = local_18 - local_10;
        local_18 = DAT_0041db98;
      }
    }
    if (param_2 == 4) {
      puVar2 = (undefined1 *)calloc(1,DAT_0041d938);
      iVar1 = FUN_00407c1c(puVar2,DAT_0041db98,DAT_0041d938);
      if (iVar1 == 0) {
        puts("DMA Failed ");
      }
      else {
        local_28 = puVar2;
        for (local_1c = 0; local_1c < DAT_0041d938; local_1c = local_1c + 1) {
          if ((local_1c & 0xf) == 0) {
            printf("0x%08lX   ",DAT_0041db98);
          }
          printf(&DAT_0040c008,*local_28);
          if ((local_1c & 3) == 3) {
            putchar(0x20);
          }
          if ((local_1c & 0xf) == 0xf) {
            putchar(10);
          }
          else if ((local_1c & 7) == 7) {
            putchar(0x20);
          }
          DAT_0041db98 = DAT_0041db98 + 1;
          local_28 = local_28 + 1;
        }
        if ((local_1c & 0xf) != 0xf) {
          putchar(10);
        }
        DAT_0041db98 = DAT_0041db98 + DAT_0041d938;
      }
      free(puVar2);
    }
    else if (param_2 == 5) {
      puVar3 = (undefined2 *)calloc(1,DAT_0041d938 << 1);
      iVar1 = FUN_00407c1c(puVar3,DAT_0041db98,DAT_0041d938 << 1);
      if (iVar1 == 0) {
        puts("DMA Failed ");
      }
      else {
        local_2c = puVar3;
        for (local_1c = 0; local_1c < DAT_0041d938; local_1c = local_1c + 1) {
          if ((local_1c & 7) == 0) {
            printf("0x%08lX   ",DAT_0041db98);
          }
          printf(&DAT_0040c01c,*local_2c);
          if ((local_1c & 1) != 0) {
            putchar(0x20);
          }
          if ((local_1c & 7) == 7) {
            putchar(10);
          }
          else if ((local_1c & 3) == 3) {
            putchar(0x20);
          }
          DAT_0041db98 = DAT_0041db98 + 2;
          local_2c = local_2c + 1;
        }
        if ((local_1c & 7) != 7) {
          putchar(10);
        }
        DAT_0041db98 = DAT_0041db98 + DAT_0041d938;
      }
      free(puVar3);
    }
    else if (param_2 == 6) {
      puVar4 = (undefined4 *)calloc(1,DAT_0041d938 << 2);
      iVar1 = FUN_00407c1c(puVar4,DAT_0041db98,DAT_0041d938 << 2);
      if (iVar1 == 0) {
        puts("DMA Failed ");
      }
      else {
        local_30 = puVar4;
        for (local_1c = 0; local_1c < DAT_0041d938; local_1c = local_1c + 1) {
          if ((local_1c & 3) == 0) {
            printf("0x%08lX   ",DAT_0041db98);
          }
          printf(&DAT_0040c024,*local_30);
          putchar(0x20);
          if ((local_1c & 3) == 3) {
            putchar(10);
          }
          else if ((local_1c & 1) != 0) {
            putchar(0x20);
          }
          DAT_0041db98 = DAT_0041db98 + 4;
          local_30 = local_30 + 1;
        }
        if ((local_1c & 7) != 7) {
          putchar(10);
        }
        DAT_0041db98 = DAT_0041db98 + DAT_0041d938;
      }
      free(puVar4);
    }
  }
  return 0;
}



/* ---- FUN_00401e04 @ 00401e04 ----
 * (library/support function; see decompiled body)
 */

int FUN_00401e04(uint param_1,uint param_2,int param_3)

{
  int local_10;
  
  if (param_2 < param_1) {
    local_10 = (param_2 - param_1) + param_3;
  }
  else {
    local_10 = param_2 - param_1;
  }
  return local_10;
}



/* ---- FUN_00401e78 @ 00401e78 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00401e78(int param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined4 local_10;
  undefined4 local_c;
  
  iVar1 = *(int *)(param_1 + 0x10);
  iVar2 = *(int *)(param_1 + 8);
  iVar3 = *(int *)(param_1 + 0xc);
  iVar4 = *(int *)(param_1 + 4);
  iVar5 = FUN_00401e04(iVar3,iVar2,iVar1);
  if (iVar5 == 0) {
    local_10 = 1;
  }
  else if ((uint)(iVar4 + iVar1) < (uint)(iVar5 + iVar3)) {
    iVar2 = FUN_00407c1c(&DAT_0041f520,iVar3,(iVar4 + iVar1) - iVar3);
    if (iVar2 != 0) {
      (&DAT_0041f520)[(iVar4 + iVar1) - iVar3] = 0;
      *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)(param_1 + 4);
      *(int *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + ((iVar4 + iVar1) - iVar3);
LAB_0040207c:
      printf(&DAT_0041f520);
      fflush(stdout);
      return local_c;
    }
    printf("\nDMA Fail");
    local_10 = 0;
  }
  else {
    iVar1 = FUN_00407c1c(&DAT_0041f520,iVar3,iVar2 - iVar3);
    if (iVar1 != 0) {
      (&DAT_0041f520)[iVar2 - iVar3] = 0;
      *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)(param_1 + 8);
      *(int *)(param_1 + 0x18) = *(int *)(param_1 + 0x18) + (iVar2 - iVar3);
      goto LAB_0040207c;
    }
    printf("\nDMA Fail");
    local_10 = 0;
  }
  return local_10;
}



/* ---- FUN_004020dc @ 004020dc ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004020dc(int param_1,int param_2)

{
  int iVar1;
  char local_4c;
  int local_44 [2];
  int local_3c;
  int local_38 [3];
  undefined1 auStack_2c [36];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length]\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    printf("\tTo print a frame buffer adress as text. Shortcut: Try \'%s\' RETURN\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    return 0;
  }
LAB_00402184:
  do {
    do {
      while( true ) {
        usleep(30000);
        iVar1 = FUN_00407c1c(local_44,0x106000,0x30);
        if (iVar1 != 0) break;
        printf("[boxdiag:DMA Failed]");
      }
    } while (local_3c == local_38[0]);
    if (local_44[0] != -0x55555556) {
      printf("Debug communication pattern not found at %x \n",0x106000);
      return 0;
    }
    iVar1 = FUN_00401e78(local_44);
    if (iVar1 == 0) {
      puts("DMA Failed ");
      return 0;
    }
    iVar1 = FUN_00407d98(local_38,0x10600c,4);
    if (iVar1 == 0) {
      puts("DMA From Host to Frame Buffer Failed ");
    }
    else {
      iVar1 = FUN_00407d98(auStack_2c,0x106018,4);
      if (iVar1 != 0) goto LAB_00402184;
      puts("DMA From Host to Frame Buffer Failed ");
    }
    if ((((local_4c == 'q') || (local_4c == 'Q')) || (local_4c == 'x')) || (local_4c == 'X')) {
      putchar(10);
      return 0;
    }
  } while( true );
}



/* ---- FUN_0040239c @ 0040239c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040239c(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  undefined4 local_6c;
  int local_60;
  uint local_50;
  int local_4c;
  undefined1 auStack_48 [4];
  undefined1 auStack_44 [4];
  uint local_40;
  int local_3c;
  uint local_38;
  int local_2c;
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length]\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    printf("\tTo print a frame buffer adress as text. Shortcut: Try \'%s\' RETURN\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    local_50 = DAT_0041db90;
    local_60 = 0;
    local_6c = 0x104800;
    iVar1 = sscanf(param_3,"%lx %lx %c %lx",&local_50,&local_4c,auStack_44,auStack_48);
    if (iVar1 == -1) {
      while (iVar1 = FUN_00407c1c(&local_3c,0x106000,0x30), iVar1 == 0) {
        local_60 = local_60 + 1;
        if (5 < local_60) {
          puts("DMA Failed tring to output debug comm address");
        }
      }
      if (local_3c == -0x55555556) {
        DAT_0041d93c = local_2c;
        DAT_0041db9c = local_38;
        DAT_0041db90 = local_38;
      }
      else {
        printf("Debug comm pattern not found at %x\n",0x106000);
      }
      if (DAT_0041db90 == 0) {
        if (param_2 == 0x1f) {
          local_6c = 0x10480c;
        }
        iVar1 = FUN_00407c1c(&local_40,local_6c,4);
        if (iVar1 == 0) {
          puts("DMA Failed ");
        }
        else {
          DAT_0041db9c = local_40;
          DAT_0041db90 = local_40;
          if (0x4000000 < local_40) {
            printf("\nWARNING: dt address too big: 0x%x",local_40);
          }
        }
      }
    }
    else {
      if (0 < iVar1) {
        DAT_0041db9c = local_50;
        DAT_0041db90 = local_50;
      }
      if (1 < iVar1) {
        DAT_0041d93c = local_4c;
      }
    }
    if (param_2 != 0) {
      iVar1 = calloc(1,DAT_0041d93c + 1);
      iVar2 = FUN_00407c1c(iVar1,DAT_0041db9c,DAT_0041d93c);
      if (iVar2 == 0) {
        puts("DMA Failed ");
      }
      else {
        puts("\n         ============================================================");
        *(undefined1 *)(iVar1 + DAT_0041d93c) = 0;
        puts(iVar1);
      }
      free(iVar1);
    }
  }
  return 0;
}



/* ---- FUN_004027a0 @ 004027a0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00402a18) */

undefined4 FUN_004027a0(int param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  uint local_ac;
  uint local_a4;
  undefined4 *local_a0;
  uint local_80;
  int local_7c;
  undefined1 auStack_78 [112];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] [filename] [+ / -] [new offset]\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo dump frame buffer memory into a file\n");
  }
  else {
    piVar2 = (int *)calloc(1,4);
    FUN_00407c1c(piVar2,0x104808,4);
    iVar3 = *piVar2;
    *(undefined1 *)piVar2 = 1;
    while (iVar4 = FUN_00407d98(piVar2,iVar3 + 4,1), iVar4 == 0) {
      puts("DMA From Host to Frame Buffer Failed ");
    }
    usleep(100000);
    iVar4 = sscanf(param_3,&DAT_0040c23c,auStack_78);
    if (iVar4 < 1) {
      puts("Invalid parameters");
    }
    else {
      FUN_00407c1c(piVar2,iVar3 + 0x10,4);
      local_7c = *piVar2;
      free(piVar2);
      iVar3 = fopen(auStack_78,&DAT_0040c240);
      if (iVar3 == 0) {
        printf("Can\'t open the %s file \n",auStack_78);
      }
      else {
        local_ac = 35000;
        local_80 = 35000;
        puVar5 = (undefined4 *)calloc(1,35000);
        while (local_ac != 0) {
          iVar4 = FUN_00407c1c(puVar5,local_7c,local_80);
          if (iVar4 == 0) {
            puts("DMA Failed ");
            break;
          }
          local_a0 = puVar5;
          for (local_a4 = 0; local_a4 < local_80; local_a4 = local_a4 + 4) {
            uVar6 = htonl(*local_a0);
            *local_a0 = uVar6;
            uVar1 = *(undefined1 *)local_a0;
            *(undefined1 *)local_a0 = *(undefined1 *)((int)local_a0 + 3);
            *(undefined1 *)((int)local_a0 + 3) = uVar1;
            uVar1 = *(undefined1 *)((int)local_a0 + 1);
            *(undefined1 *)((int)local_a0 + 1) = *(undefined1 *)((int)local_a0 + 2);
            *(undefined1 *)((int)local_a0 + 2) = uVar1;
            local_a0 = local_a0 + 1;
          }
          fwrite(puVar5,1,local_80,iVar3);
          local_ac = local_ac - local_80;
          local_7c = local_7c + local_80;
          local_80 = 0x20000;
          if (local_ac < 0x20001) {
            local_80 = local_ac;
          }
        }
        free(puVar5);
        if (iVar3 != 0) {
          fclose(iVar3);
        }
      }
    }
  }
  return 0;
}



/* ---- FUN_00402c38 @ 00402c38 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00402c38(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  char *pcVar2;
  char *local_38;
  char *local_34;
  uint local_2c;
  uint local_24;
  uint local_20;
  int local_1c;
  uint local_18;
  int local_14;
  char local_10 [8];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] [+ / -] [new offset]\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo display/examine frame buffer memory in hex and ASCII\n");
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx %c %lx",&local_1c,&local_18,local_10,&local_14);
    if (0 < iVar1) {
      DAT_0041dba0 = local_1c;
    }
    if (1 < iVar1) {
      DAT_0041d940 = local_18;
    }
    if (3 < iVar1) {
      if (local_10[0] == '+') {
        DAT_0041dba0 = local_1c + local_14;
        local_1c = DAT_0041dba0;
      }
      else if (local_10[0] == '-') {
        DAT_0041dba0 = local_1c - local_14;
        local_1c = DAT_0041dba0;
      }
    }
    if ((DAT_0041d940 & 3) == 0) {
      local_2c = (DAT_0041d940 & 0x3fffffff) >> 2;
    }
    else {
      local_2c = ((DAT_0041d940 & 0x3fffffff) >> 2) + 1;
    }
    pcVar2 = (char *)calloc(1,DAT_0041d940 << 2);
    iVar1 = FUN_00407c1c(pcVar2,DAT_0041dba0,DAT_0041d940 << 2);
    if (iVar1 == 0) {
      puts("DMA Failed ");
    }
    else {
      local_38 = pcVar2;
      local_34 = pcVar2;
      for (local_20 = 0; local_20 < local_2c; local_20 = local_20 + 1) {
        printf("0x%08lX   ",DAT_0041dba0);
        for (local_24 = 0; local_24 < 4; local_24 = local_24 + 1) {
          printf(&DAT_0040c024,*(undefined4 *)local_34);
          putchar(0x20);
          if (local_24 == 3) {
            printf(&DAT_0040c2b0);
          }
          local_34 = local_34 + 4;
        }
        for (local_24 = 0; local_24 < 0x10; local_24 = local_24 + 1) {
          if (*local_38 < ' ') {
            putchar(0x2e);
          }
          else {
            putchar((int)*local_38);
          }
          local_38 = local_38 + 1;
        }
        DAT_0041dba0 = DAT_0041dba0 + 0x10;
        putchar(10);
      }
    }
    free(pcVar2);
  }
  return 0;
}



/* ---- FUN_0040302c @ 0040302c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040302c(int param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  int iVar3;
  undefined4 *puVar4;
  undefined4 uVar5;
  uint local_a4;
  uint local_9c;
  undefined4 *local_98;
  int local_88;
  uint local_84;
  int local_80;
  char local_7c [4];
  undefined1 auStack_78 [112];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] [filename] [+ / -] [new offset]\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo dump frame buffer memory into a file\n");
  }
  else {
    iVar2 = sscanf(param_3,"%lx %lx %s %c %lx",&local_88,&local_84,auStack_78,local_7c,
                         &local_80);
    if (0 < iVar2) {
      DAT_0041dba4 = local_88;
    }
    if (1 < iVar2) {
      DAT_0041d944 = local_84;
    }
    if (iVar2 < 3) {
      puts("Invalid parameters");
    }
    else {
      iVar3 = fopen(auStack_78,&DAT_0040c240);
      if (iVar3 == 0) {
        printf("Can\'t open the %s file \n",auStack_78);
      }
      else {
        if (4 < iVar2) {
          if (local_7c[0] == '+') {
            DAT_0041dba4 = local_88 + local_80;
            local_88 = DAT_0041dba4;
          }
          else if (local_7c[0] == '-') {
            DAT_0041dba4 = local_88 - local_80;
            local_88 = DAT_0041dba4;
          }
        }
        local_a4 = DAT_0041d944;
        if (0x20000 < DAT_0041d944) {
          DAT_0041d944 = 0x20000;
        }
        puVar4 = (undefined4 *)calloc(1,DAT_0041d944);
        while (local_a4 != 0) {
          iVar2 = FUN_00407c1c(puVar4,DAT_0041dba4,DAT_0041d944);
          if (iVar2 == 0) {
            puts("DMA Failed ");
            break;
          }
          local_98 = puVar4;
          for (local_9c = 0; local_9c < DAT_0041d944; local_9c = local_9c + 4) {
            uVar5 = htonl(*local_98);
            *local_98 = uVar5;
            uVar1 = *(undefined1 *)local_98;
            *(undefined1 *)local_98 = *(undefined1 *)((int)local_98 + 3);
            *(undefined1 *)((int)local_98 + 3) = uVar1;
            uVar1 = *(undefined1 *)((int)local_98 + 1);
            *(undefined1 *)((int)local_98 + 1) = *(undefined1 *)((int)local_98 + 2);
            *(undefined1 *)((int)local_98 + 2) = uVar1;
            local_98 = local_98 + 1;
          }
          fwrite(puVar4,1,DAT_0041d944,iVar3);
          local_a4 = local_a4 - DAT_0041d944;
          DAT_0041dba4 = DAT_0041dba4 + DAT_0041d944;
          DAT_0041d944 = 0x20000;
          if (local_a4 < 0x20001) {
            DAT_0041d944 = local_a4;
          }
        }
        free(puVar4);
        if (iVar3 != 0) {
          fclose(iVar3);
        }
      }
    }
  }
  return 0;
}



/* ---- FUN_004034ac @ 004034ac ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004034ac(int param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int local_94;
  undefined4 *local_90;
  int local_84;
  int local_7c;
  int local_78;
  undefined1 auStack_74 [108];
  
  local_7c = 1;
  if (param_1 == 0) {
    printf("Usage: %s [file name] [num banks]\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo dump the whole frame buffer memory\n");
  }
  else {
    local_84 = 0;
    iVar2 = sscanf(param_3,"%s %d",auStack_74,&local_78);
    if ((iVar2 < 1) || (local_84 = fopen(auStack_74,&DAT_0040c240), local_84 != 0)) {
      if (1 < iVar2) {
        if ((local_78 < 1) || (0x10 < local_78)) {
          printf("\nPlease enter a valid bank number between 1 and 16 (in terms of 64MB)");
        }
        else {
          local_7c = local_78;
        }
      }
      DAT_0041dba8 = 0;
      puVar3 = (undefined4 *)calloc(1,0x4000);
      for (; DAT_0041dba8 < (uint)(local_7c << 0x1a); DAT_0041dba8 = DAT_0041dba8 + 0x4000) {
        putchar(0x2e);
        fflush(stdout);
        iVar2 = FUN_00407c1c(puVar3,DAT_0041dba8,DAT_0041d948);
        if (iVar2 == 0) {
          puts("DMA Failed ");
        }
        else {
          local_90 = puVar3;
          for (local_94 = 0; local_94 < 0x4000; local_94 = local_94 + 4) {
            uVar4 = htonl(*local_90);
            *local_90 = uVar4;
            uVar1 = *(undefined1 *)local_90;
            *(undefined1 *)local_90 = *(undefined1 *)((int)local_90 + 3);
            *(undefined1 *)((int)local_90 + 3) = uVar1;
            uVar1 = *(undefined1 *)((int)local_90 + 1);
            *(undefined1 *)((int)local_90 + 1) = *(undefined1 *)((int)local_90 + 2);
            *(undefined1 *)((int)local_90 + 2) = uVar1;
            local_90 = local_90 + 1;
          }
          fwrite(puVar3,1,DAT_0041d948,local_84);
        }
      }
      puts(".done");
      free(puVar3);
      if (local_84 != 0) {
        fclose(local_84);
      }
    }
    else {
      printf("Can\'t open the %s file \n",auStack_74);
    }
  }
  return 0;
}



/* ---- FUN_00403870 @ 00403870 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00403870(int param_1,int param_2,undefined4 param_3)

{
  undefined1 uVar1;
  int iVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int local_94;
  undefined4 *local_90;
  int local_84;
  int local_7c;
  int local_78;
  undefined1 auStack_74 [108];
  
  local_7c = 0;
  if (param_1 == 0) {
    printf("Usage: %s [file name] [num of chunks in 64Mbs]\n",(&PTR_DAT_0041d820)[param_2 * 2]
                );
    puts("\tTo dump frame buffer memory in 64Mbs chunk\n");
  }
  else {
    local_84 = 0;
    iVar2 = sscanf(param_3,"%s %d",auStack_74,&local_78);
    if ((iVar2 < 1) || (local_84 = fopen(auStack_74,&DAT_0040c240), local_84 != 0)) {
      if (1 < iVar2) {
        local_7c = local_78;
      }
      DAT_0041dbac = 0;
      puVar3 = (undefined4 *)calloc(1,0x4000);
      for (; DAT_0041dbac < (uint)(local_7c << 0x1a); DAT_0041dbac = DAT_0041dbac + 0x4000) {
        putchar(0x2e);
        fflush(stdout);
        iVar2 = FUN_00407c1c(puVar3,DAT_0041dbac,DAT_0041d94c);
        if (iVar2 == 0) {
          puts("DMA Failed ");
        }
        else {
          local_90 = puVar3;
          for (local_94 = 0; local_94 < 0x4000; local_94 = local_94 + 4) {
            uVar4 = htonl(*local_90);
            *local_90 = uVar4;
            uVar1 = *(undefined1 *)local_90;
            *(undefined1 *)local_90 = *(undefined1 *)((int)local_90 + 3);
            *(undefined1 *)((int)local_90 + 3) = uVar1;
            uVar1 = *(undefined1 *)((int)local_90 + 1);
            *(undefined1 *)((int)local_90 + 1) = *(undefined1 *)((int)local_90 + 2);
            *(undefined1 *)((int)local_90 + 2) = uVar1;
            local_90 = local_90 + 1;
          }
          fwrite(puVar3,1,DAT_0041d94c,local_84);
          usleep(0x32);
        }
      }
      puts(".done");
      free(puVar3);
      if (local_84 != 0) {
        fclose(local_84);
      }
    }
    else {
      printf("Can\'t open the %s file \n",auStack_74);
    }
  }
  return 0;
}



/* ---- FUN_00403c08 @ 00403c08 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00403c08(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 *local_34;
  undefined2 *local_30;
  undefined1 *local_2c;
  uint local_20;
  int local_1c;
  uint local_18;
  int local_14;
  char local_10 [8];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] [+ / -] [new offset]\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo display/examine frame buffer memory\n");
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx %c %lx",&local_1c,&local_18,local_10,&local_14);
    if (0 < iVar1) {
      DAT_0041dbb0 = local_1c;
    }
    if (1 < iVar1) {
      DAT_0041d950 = local_18;
    }
    if (3 < iVar1) {
      if (local_10[0] == '+') {
        DAT_0041dbb0 = local_1c + local_14;
        local_1c = DAT_0041dbb0;
      }
      else if (local_10[0] == '-') {
        DAT_0041dbb0 = local_1c - local_14;
        local_1c = DAT_0041dbb0;
      }
    }
    iVar1 = FUN_00407b2c();
    if (param_2 == 7) {
      local_2c = (undefined1 *)(DAT_0041dbb0 + iVar1);
      for (local_20 = 0; local_20 < DAT_0041d950; local_20 = local_20 + 1) {
        if ((local_20 & 0xf) == 0) {
          printf("0x%08lX   ",(int)local_2c - iVar1);
        }
        printf(&DAT_0040c008,*local_2c);
        if ((local_20 & 3) == 3) {
          putchar(0x20);
        }
        if ((local_20 & 0xf) == 0xf) {
          putchar(10);
        }
        else if ((local_20 & 7) == 7) {
          putchar(0x20);
        }
        local_2c = local_2c + 1;
      }
      if ((local_20 & 0xf) != 0xf) {
        putchar(10);
      }
    }
    else if (param_2 == 8) {
      local_30 = (undefined2 *)(DAT_0041dbb0 + iVar1);
      for (local_20 = 0; local_20 < DAT_0041d950; local_20 = local_20 + 1) {
        if ((local_20 & 7) == 0) {
          printf("0x%08lX   ",(int)local_30 - iVar1);
        }
        printf(&DAT_0040c01c,*local_30);
        if ((local_20 & 1) != 0) {
          putchar(0x20);
        }
        if ((local_20 & 7) == 7) {
          putchar(10);
        }
        else if ((local_20 & 3) == 3) {
          putchar(0x20);
        }
        local_30 = local_30 + 1;
      }
      if ((local_20 & 7) != 7) {
        putchar(10);
      }
    }
    else if (param_2 == 9) {
      local_34 = (undefined4 *)(DAT_0041dbb0 + iVar1);
      for (local_20 = 0; local_20 < DAT_0041d950; local_20 = local_20 + 1) {
        if ((local_20 & 3) == 0) {
          printf("0x%08lX   ",(int)local_34 - iVar1);
        }
        printf(&DAT_0040c024,*local_34);
        if ((local_20 & 3) == 3) {
          putchar(10);
        }
        else {
          putchar(0x20);
        }
        local_34 = local_34 + 1;
      }
      if ((local_20 & 3) != 3) {
        putchar(10);
      }
    }
  }
  return 0;
}



/* ---- FUN_0040419c @ 0040419c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040419c(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  undefined4 *local_ac;
  int local_9c;
  uint local_90;
  undefined4 local_88;
  uint local_84;
  uint local_80;
  int local_7c;
  char local_78 [4];
  undefined1 auStack_74 [108];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] filename\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo dump frame buffer memory to a file\n");
  }
  else {
    local_9c = 0;
    iVar1 = sscanf(param_3,"%lx %lx %s %c %lx",&local_84,&local_80,auStack_74,local_78,
                         &local_7c);
    if (0 < iVar1) {
      DAT_0041dbb8 = local_84;
    }
    if (1 < iVar1) {
      DAT_0041d954 = local_80;
    }
    if ((iVar1 < 3) || (local_9c = fopen(auStack_74,&DAT_0040c240), local_9c != 0)) {
      if (4 < iVar1) {
        if (local_78[0] == '+') {
          DAT_0041dbb8 = local_84 + local_7c;
          local_84 = DAT_0041dbb8;
        }
        else if (local_78[0] == '-') {
          DAT_0041dbb8 = local_84 - local_7c;
          local_84 = DAT_0041dbb8;
        }
      }
      DAT_0041dbb4 = DAT_0041d954 * 4 + DAT_0041dbb8;
      if (0xc000000 < DAT_0041dbb4) {
        puts("Address exceed 192M limitation, force it to 192M");
        DAT_0041dbb4 = 0xc000000;
      }
      iVar1 = FUN_00407b2c();
      for (; DAT_0041dbb8 < DAT_0041dbb4; DAT_0041dbb8 = DAT_0041d954 * 4 + DAT_0041dbb8) {
        uVar2 = DAT_0041dbb8 >> 0x14;
        uVar3 = DAT_0041dbb8 & 0xfffff;
        uVar4 = 0x100000 - uVar3;
        uVar5 = DAT_0041dbb4 - DAT_0041dbb8;
        if (uVar4 <= DAT_0041dbb4 - DAT_0041dbb8) {
          uVar5 = uVar4;
        }
        DAT_0041d954 = uVar5 >> 2;
        FUN_0040b1f4(0x43c);
        printf("dumping %d Mega length is 0x%x\n",uVar2,DAT_0041d954 << 2);
        local_ac = (undefined4 *)(iVar1 + uVar3);
        for (local_90 = 0; local_90 < DAT_0041d954; local_90 = local_90 + 1) {
          local_88 = *local_ac;
          fwrite(&local_88,1,4,local_9c);
          local_ac = local_ac + 1;
        }
      }
      if (local_9c != 0) {
        fclose(local_9c);
      }
    }
    else {
      printf("Can\'t open the %s file \n",auStack_74);
    }
  }
  return 0;
}



/* ---- FUN_004045b8 @ 004045b8 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004045b8(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int local_a0;
  undefined4 *local_9c;
  undefined4 local_84;
  uint local_80;
  int local_7c;
  undefined1 auStack_78 [112];
  
  if (param_1 == 0) {
    printf("Usage: %s [offset] [length] filename\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo dump frame buffer memory to a file for SOC mode\n");
  }
  else {
    local_a0 = 0;
    iVar1 = sscanf(param_3,"%lx %lx %s",&local_80,&local_7c,auStack_78);
    if (0 < iVar1) {
      DAT_0041dbc0 = local_80;
    }
    if (1 < iVar1) {
      DAT_0041d958 = local_7c;
    }
    if ((iVar1 < 3) || (local_a0 = fopen(auStack_78,&DAT_0040c240), local_a0 != 0)) {
      if (DAT_0041dbc0 < 0x4000001) {
        DAT_0041dbbc = DAT_0041dbc0 + DAT_0041d958;
        if (0x4000000 < DAT_0041dbbc) {
          puts("Address exceed 64M limitation, force it to 64M");
          DAT_0041dbbc = 0x4000000;
        }
        iVar1 = FUN_00407b2c();
        local_9c = (undefined4 *)(DAT_0041dbc0 + iVar1);
        for (; DAT_0041dbc0 < DAT_0041dbbc; DAT_0041dbc0 = DAT_0041dbc0 + 4) {
          local_84 = *local_9c;
          fwrite(&local_84,1,4,local_a0);
          local_9c = local_9c + 1;
        }
        if (local_a0 != 0) {
          fclose(local_a0);
        }
      }
      else {
        puts("unable to read beyond 64M in framebuffer due to driver limitations");
      }
    }
    else {
      printf("Can\'t open the %s file \n",auStack_78);
    }
  }
  return 0;
}



/* ---- FUN_00404884 @ 00404884 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00404884(int param_1,int param_2,undefined4 param_3)

{
  uint uVar1;
  int iVar2;
  undefined1 *puVar3;
  undefined1 *local_34;
  uint local_30;
  int local_2c;
  uint local_24;
  int local_20;
  uint local_1c;
  undefined1 local_18 [8];
  uint local_10;
  uint local_c;
  
  if (param_1 == 0) {
    printf("Usage: %s <offset> <length(hex)> <data byte>\n\n",(&PTR_DAT_0041d820)[param_2 * 2]
                );
  }
  else {
    local_20 = 0;
    local_1c = 0;
    local_2c = 0;
    iVar2 = sscanf(param_3,"%lx %lx %lx",&local_20,&local_1c,local_18);
    if (iVar2 == 3) {
      if ((local_1c & 0x1ffff) == 0) {
        local_c = local_1c >> 0x11;
      }
      else {
        local_c = (local_1c >> 0x11) + 1;
      }
      uVar1 = local_c;
      local_30 = local_c;
      puVar3 = (undefined1 *)calloc(1,0x20000);
      if (uVar1 == 1) {
        local_10 = local_1c % 0x20001;
      }
      else {
        local_10 = 0x20000;
      }
      local_34 = puVar3;
      for (local_24 = local_10; local_24 != 0; local_24 = local_24 - 1) {
        *local_34 = local_18[0];
        local_34 = local_34 + 1;
      }
      for (; 1 < local_30; local_30 = local_30 - 1) {
        iVar2 = FUN_00407d98(puVar3,local_2c * 0x20000 + local_20,0x20000);
        if (iVar2 == 0) {
          puts("DMA From Host to Frame Buffer Failed ");
        }
        local_2c = local_2c + 1;
      }
      if ((local_1c & 0x1ffff) == 0) {
        local_24 = 0x20000;
      }
      else {
        local_24 = local_1c & 0x1ffff;
      }
      iVar2 = FUN_00407d98(puVar3,local_2c * 0x20000 + local_20,local_24);
      if (iVar2 == 0) {
        puts("DMA From Host to Frame Buffer Failed ");
      }
      free(puVar3);
      puts(&DAT_0040c564);
    }
  }
  return 0;
}



/* ---- FUN_00404b90 @ 00404b90 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00404b90(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 *puVar2;
  int iVar3;
  undefined1 *local_88;
  int local_84;
  int local_80;
  int local_7c;
  undefined1 local_78 [4];
  undefined1 auStack_74 [108];
  
  if (param_1 == 0) {
    printf("Usage: %s <offset> <binary file name>\n\n",(&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    iVar1 = sscanf(param_3,"%lx %s",&local_7c,auStack_74,0);
    if (iVar1 == 2) {
      puVar2 = (undefined1 *)calloc(1,0x40000);
      iVar1 = fopen(auStack_74,&DAT_0040c59c);
      if (iVar1 == 0) {
        printf("Can\'t open the %s file \n",auStack_74);
      }
      else {
        local_80 = 0;
        local_84 = 0;
        local_88 = puVar2;
        do {
          if (local_80 == 0x40000) {
            iVar3 = FUN_00407d98(puVar2,local_7c,0x40000);
            if (iVar3 == 0) {
              puts("DMA From Host to Frame Buffer Failed ");
              break;
            }
            local_84 = local_84 + 0x40000;
            local_7c = local_7c + 0x40000;
            local_80 = 0;
            local_88 = puVar2;
          }
          iVar3 = fread(local_78,1,1,iVar1);
          if (iVar3 != 1) break;
          *local_88 = local_78[0];
          local_88 = local_88 + 1;
          local_80 = local_80 + 1;
          iVar3 = feof(iVar1);
        } while (iVar3 == 0);
        if (local_80 != 0) {
          iVar1 = FUN_00407d98(puVar2,local_7c,local_80);
          if (iVar1 == 0) {
            puts("DMA From Host to Frame Buffer Failed ");
          }
          local_84 = local_84 + local_80;
        }
        printf("Transfered file size %d\n",local_84);
        free(puVar2);
      }
    }
  }
  return 0;
}



/* ---- FUN_00404e60 @ 00404e60 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00404e60(int param_1,int param_2,char *param_3)

{
  int iVar1;
  undefined1 *puVar2;
  undefined1 *local_24;
  char *local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  undefined1 local_10 [8];
  
  if (param_1 == 0) {
    printf("Usage: %s <offset> <length(hex)> <data byte> .. <data byte>\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx",&local_18,&local_14);
    if (iVar1 == 2) {
      puVar2 = (undefined1 *)calloc(1,local_14);
      local_1c = local_14;
      for (local_20 = param_3; (*local_20 != '\0' && (*local_20 == ' ')); local_20 = local_20 + 1) {
      }
      for (; (*local_20 != '\0' && (*local_20 != ' ')); local_20 = local_20 + 1) {
      }
      for (; (*local_20 != '\0' && (*local_20 == ' ')); local_20 = local_20 + 1) {
      }
      for (; (local_24 = puVar2, *local_20 != '\0' && (*local_20 != ' ')); local_20 = local_20 + 1)
      {
      }
      while ((local_1c != 0 && (iVar1 = sscanf(local_20,&DAT_0040c5fc,local_10), iVar1 != 0)))
      {
        *local_24 = local_10[0];
        for (; (*local_20 != '\0' && (*local_20 == ' ')); local_20 = local_20 + 1) {
        }
        for (; (*local_20 != '\0' && (*local_20 != ' ')); local_20 = local_20 + 1) {
        }
        local_1c = local_1c + -1;
        local_24 = local_24 + 1;
      }
      if (local_1c != 0) {
        puts("Error: invalid parameters");
      }
      FUN_00407d98(puVar2,local_18,local_14);
      free(puVar2);
    }
  }
  return 0;
}



/* ---- FUN_00405164 @ 00405164 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405164(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 *local_1c;
  int local_18;
  int local_14;
  undefined1 local_10 [8];
  
  if (param_1 == 0) {
    printf("Usage: %s <offset> <length(hex)> <data byte>\n\n",(&PTR_DAT_0041d820)[param_2 * 2]
                );
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx %lx",&local_18,&local_14,local_10);
    if (iVar1 == 3) {
      iVar1 = FUN_00407b2c();
      local_1c = (undefined1 *)(local_18 + iVar1);
      for (; local_14 != 0; local_14 = local_14 + -1) {
        *local_1c = local_10[0];
        local_1c = local_1c + 1;
      }
    }
  }
  return 0;
}



/* ---- FUN_00405284 @ 00405284 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405284(int param_1,int param_2,char *param_3)

{
  int iVar1;
  undefined1 *local_1c;
  char *local_18;
  int local_14;
  int local_10;
  undefined1 local_c [4];
  
  if (param_1 == 0) {
    printf("Usage: %s <offset> <length(hex)> <data byte> .. <data byte>\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    iVar1 = sscanf(param_3,"%lx %lx",&local_14,&local_10);
    if (iVar1 == 2) {
      iVar1 = FUN_00407b2c();
      local_1c = (undefined1 *)(local_14 + iVar1);
      for (local_18 = param_3; (*local_18 != '\0' && (*local_18 == ' ')); local_18 = local_18 + 1) {
      }
      for (; (*local_18 != '\0' && (*local_18 != ' ')); local_18 = local_18 + 1) {
      }
      for (; (*local_18 != '\0' && (*local_18 == ' ')); local_18 = local_18 + 1) {
      }
      for (; (*local_18 != '\0' && (*local_18 != ' ')); local_18 = local_18 + 1) {
      }
      while ((local_10 != 0 && (iVar1 = sscanf(local_18,&DAT_0040c5fc,local_c), iVar1 != 0)))
      {
        *local_1c = local_c[0];
        for (; (*local_18 != '\0' && (*local_18 == ' ')); local_18 = local_18 + 1) {
        }
        for (; (*local_18 != '\0' && (*local_18 != ' ')); local_18 = local_18 + 1) {
        }
        local_1c = local_1c + 1;
        local_10 = local_10 + -1;
      }
      if (local_10 != 0) {
        puts("Error: invalid parameters");
      }
    }
  }
  return 0;
}



/* ---- FUN_00405544 @ 00405544 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405544(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_10;
  uint local_c;
  
  if (param_1 == 0) {
    printf("Usage: %s <I2c address of decoder chip> <Register Index> \n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    local_10 = 0;
    local_c = 0;
    iVar1 = sscanf(param_3,"%lx %lx",&local_10,&local_c);
    if (iVar1 == 2) {
      uVar2 = FUN_00407f88();
      FUN_00409878(uVar2,1,0,0,local_c & 0xff,0,local_10);
    }
    else {
      printf("Usage: %s <I2c address of decoder chip> <Register Index> \n\n",
                   (&PTR_DAT_0041d820)[param_2 * 2]);
    }
  }
  return 0;
}



/* ---- FUN_00405698 @ 00405698 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405698(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_18;
  uint local_14;
  uint local_10 [2];
  
  if (param_1 == 0) {
    printf("Usage: %s <I2C address of decoder chip> <Register Index> <Register Value>\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    local_18 = 0;
    local_14 = 0;
    local_10[0] = 0;
    iVar1 = sscanf(param_3,"%lx %lx %lx",&local_18,&local_14,local_10);
    if (iVar1 == 3) {
      uVar2 = FUN_00407f88();
      FUN_00409878(uVar2,1,0,1,local_14 & 0xff,local_10[0] & 0xff,local_18);
    }
    else {
      printf("Usage: %s <I2C address of decoder chip> <Register Index> <Register Value>\n\n",
                   (&PTR_DAT_0041d820)[param_2 * 2]);
    }
  }
  return 0;
}



/* ---- FUN_00405804 @ 00405804 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405804(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_c;
  
  if (param_1 == 0) {
    printf("Usage: %s <i2c address of video decoder>\n\n",(&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    local_c = 0;
    iVar1 = sscanf(param_3,&DAT_0040c5fc,&local_c);
    if (iVar1 == 1) {
      uVar2 = FUN_00407f88();
      FUN_00409878(uVar2,1,0,0,0xff,0,local_c);
    }
    else {
      printf("Usage: %s <i2c address of video decoder>\n\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    }
  }
  return 0;
}



/* ---- FUN_00405950 @ 00405950 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405950(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_24;
  
  if (param_1 == 0) {
    printf("Usage: %s <diplays firmware build date and time>\n\n",
                 (&PTR_DAT_0041d820)[param_2 * 2]);
  }
  else {
    iVar1 = calloc(1,0x24);
    if (iVar1 == 0) {
      puts("Calloc failed ");
    }
    else {
      iVar2 = FUN_00407c1c(iVar1,0x1004dc,0x20);
      if (iVar2 == 0) {
        puts("DMA From Host to Frame Buffer Failed ");
      }
      else {
        for (local_24 = 0; local_24 < 0x20; local_24 = local_24 + 1) {
          if (*(char *)(iVar1 + local_24) == '\0') {
            *(undefined1 *)(iVar1 + local_24) = 0x2e;
          }
        }
        *(undefined1 *)(iVar1 + 0x14) = 0;
        *(undefined1 *)(iVar1 + local_24) = 0;
        puts(iVar1);
      }
    }
  }
  return 0;
}



/* ---- FUN_00405b18 @ 00405b18 ----
 * (library/support function; see decompiled body)
 */

int FUN_00405b18(void)

{
  char cVar1;
  
  cVar1 = FUN_0040b348();
  return (int)cVar1;
}



/* ---- FUN_00405b58 @ 00405b58 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00405b58(int param_1,int param_2,undefined4 param_3)

{
  bool bVar1;
  int iVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  undefined4 local_1c;
  undefined4 local_18 [4];
  
  DAT_0041dbc8 = calloc(1,0x1148);
  if (DAT_0041dbc8 == 0) {
    puts("Calloc failed ");
  }
  else if (param_1 == 0) {
    printf("Usage: %s\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo execute firmware test terminal\n");
  }
  else {
    sscanf(param_3,"%d %d",&local_1c,local_18);
    iVar4 = FUN_004063a8(local_1c);
    if (iVar4 == 0) {
      printf("Error doesn\'t exist basexprintf cpu_no =%x\n",local_1c);
    }
    else {
      iVar4 = FUN_0040651c(iVar4,local_18[0]);
      if (iVar4 == 0) {
        printf("Error doesn\'t exist basexprintf cpu_no =%x\n",local_18[0]);
      }
      else {
        FUN_0040b230();
        iVar2 = DAT_0041dbc8;
        do {
          usleep(200);
          iVar5 = FUN_00407c1c(iVar2,iVar4,0x1128);
          if (iVar5 == 0) {
            puts("DMA From Host to Frame Buffer Failed ");
            break;
          }
          bVar1 = false;
          if (*(int *)(iVar2 + 0x118) != *(int *)(iVar2 + 8)) {
            bVar1 = true;
            while (*(int *)(iVar2 + 0x118) != *(int *)(iVar2 + 8)) {
              cVar3 = *(char *)(iVar2 + *(int *)(iVar2 + 8) + 0x120);
              *(int *)(iVar2 + 8) = *(int *)(iVar2 + 8) + 1;
              putchar((int)cVar3);
              if (0xfff < *(int *)(iVar2 + 8)) {
                *(undefined4 *)(iVar2 + 8) = 0;
              }
            }
          }
          fflush(stdout);
          cVar3 = FUN_00405b18();
          if (cVar3 != '\0') {
            iVar5 = *(int *)(iVar2 + 0xc);
            *(char *)(iVar2 + iVar5 + 0x10) = cVar3;
            *(int *)(iVar2 + 0xc) = iVar5 + 1;
            if (0xff < *(int *)(iVar2 + 0xc)) {
              *(undefined4 *)(iVar2 + 0xc) = 0;
            }
          }
          if (((bVar1) || (cVar3 != '\0')) && (iVar5 = FUN_00407d98(iVar2,iVar4,0x110), iVar5 == 0))
          {
            puts("DMA From Host to Frame Buffer Failed ");
            break;
          }
          if ((cVar3 == '\n') || (cVar3 == '\r')) {
            if (DAT_0041dbc4 < 1) {
              if (DAT_0041e4f3 == 'e') {
                putchar(10);
                break;
              }
            }
            else if ((&DAT_0041e4ef)[DAT_0041dbc4] == 'e') {
              putchar(10);
              break;
            }
          }
          if (cVar3 != '\0') {
            (&DAT_0041e4f0)[DAT_0041dbc4] = cVar3;
            DAT_0041dbc4 = DAT_0041dbc4 + 1;
            if (3 < DAT_0041dbc4) {
              DAT_0041dbc4 = 0;
            }
          }
        } while (cVar3 != '\x04');
        for (DAT_0041dbc4 = 0; DAT_0041dbc4 < 4; DAT_0041dbc4 = DAT_0041dbc4 + 1) {
          (&DAT_0041e4f0)[DAT_0041dbc4] = 0x6c;
        }
        FUN_0040b2c0();
        free(DAT_0041dbc8);
        DAT_0041dbc8 = 0;
      }
    }
  }
  return 0;
}



/* ---- FUN_004060ac @ 004060ac ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004060ac(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int local_28;
  char *local_24;
  undefined4 local_1c;
  undefined1 auStack_18 [16];
  
  DAT_0041dbc8 = (char *)calloc(1,0x1148);
  if (DAT_0041dbc8 == (char *)0x0) {
    puts("Calloc failed ");
  }
  else if (param_1 == 0) {
    printf("Usage: %s\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo execute firmware test terminal\n");
  }
  else {
    sscanf(param_3,"%d %d",&local_1c,auStack_18);
    iVar1 = FUN_004063a8(local_1c);
    if (iVar1 == 0) {
      printf("Error doesn\'t exist basexprintf cpu_no =%x\n",local_1c);
    }
    else {
      iVar1 = FUN_00407c1c(DAT_0041dbc8,iVar1,0x100);
      if (iVar1 == 0) {
        puts("DMA From Host to Frame Buffer Failed ");
        free(DAT_0041dbc8);
      }
      else {
        local_24 = DAT_0041dbc8;
        if (*(int *)(DAT_0041dbc8 + 0xc) == 0) {
          puts("read data pointer error ");
          free(DAT_0041dbc8);
        }
        else {
          puts("==== Firmware debug console lists ====");
          for (local_28 = 0; local_28 < 0x10; local_28 = local_28 + 1) {
            if (*local_24 != '\0') {
              printf(" %2d : %s - %x\n",local_28,local_24,*(undefined4 *)(local_24 + 0xc));
            }
            local_24 = local_24 + 0x10;
          }
          puts("=====");
          free(DAT_0041dbc8);
        }
      }
    }
  }
  return 0;
}



/* ---- FUN_004063a8 @ 004063a8 ----
 * (library/support function; see decompiled body)
 */

uint FUN_004063a8(uint param_1)

{
  int iVar1;
  uint local_18;
  uint local_10;
  
  if (DAT_0041dbc8 == (uint *)0x0) {
    local_10 = 0;
  }
  else {
    iVar1 = FUN_00407c1c(DAT_0041dbc8,0x105fe0,0x20);
    if (iVar1 == 0) {
      puts("DMA From Host to Frame Buffer Failed ");
      local_10 = 0;
    }
    else {
      if ((param_1 == 0) || (7 < param_1)) {
        local_18 = *DAT_0041dbc8;
        if ((local_18 == 0) || ((local_18 & 3) != 0)) {
          puts("origin cotrol bf pointer error 0x105fe0");
          return 0;
        }
      }
      else {
        local_18 = DAT_0041dbc8[param_1];
        if ((local_18 == 0) || ((local_18 & 3) != 0)) {
          puts("origin cotrol bf pointer error 0x105fe0");
          return 0;
        }
      }
      local_10 = local_18;
    }
  }
  return local_10;
}



/* ---- FUN_0040651c @ 0040651c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040651c(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 local_18;
  undefined4 local_10;
  
  if (DAT_0041dbc8 == 0) {
    local_10 = 0;
  }
  else {
    iVar1 = FUN_00407c1c(DAT_0041dbc8,param_1,0x100);
    if (iVar1 == 0) {
      puts("DMA From Host to Frame Buffer Failed ");
      local_10 = 0;
    }
    else if (*(int *)(DAT_0041dbc8 + 0xc) == 0) {
      puts("read data pointer error ");
      local_10 = 0;
    }
    else {
      if (param_2 < 8) {
        iVar1 = DAT_0041dbc8 + param_2 * 0x10;
        if (*(int *)(iVar1 + 0xc) == 0) {
          puts("read data pointer error ");
          return 0;
        }
        local_18 = *(undefined4 *)(iVar1 + 0xc);
        printf("Start console:\"%s\', addr =%x\n",iVar1,*(undefined4 *)(iVar1 + 0xc));
      }
      else {
        if (*(int *)(DAT_0041dbc8 + 0xc) == 0) {
          puts("read data pointer error ");
          return 0;
        }
        local_18 = *(undefined4 *)(DAT_0041dbc8 + 0xc);
        printf("Start console:\"%s\', addr =%x\n",DAT_0041dbc8,
                     *(undefined4 *)(DAT_0041dbc8 + 0xc));
      }
      local_10 = local_18;
    }
  }
  return local_10;
}



/* ---- FUN_00406718 @ 00406718 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00406718(int param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 *local_28;
  undefined1 *local_24;
  uint local_20;
  uint local_18;
  int local_14 [2];
  undefined4 local_c;
  
  if (param_1 == 0) {
    if (param_2 == 0x1c) {
      printf("Usage: %s [offset] [search length] [data], [print size]\n",PTR_DAT_0041d900);
      puts("\tTo find a pattern in the FB\n");
      return local_c;
    }
    printf("Usage: %s [print size]\n",(&PTR_DAT_0041d820)[param_2 * 2]);
    puts("\tTo find the next/previous pattern based on previously called Find command\n");
    return local_c;
  }
  if (param_2 == 0x1c) {
    DAT_0041e420 = 0;
    iVar1 = sscanf(param_3,"%lx %lx %s %lx",&local_18,local_14,&DAT_0041e018,&DAT_0041d95c);
    DAT_0041e424 = 0;
    if (iVar1 < 3) {
      puts("Insufficient parameters");
      return 0;
    }
    DAT_0041e41c = local_18;
    DAT_0041e414 = local_18;
    DAT_0041e404 = local_18;
    DAT_0041e408 = local_14[0];
    DAT_0041e410 = strlen(&DAT_0041e018);
    if (1000 < DAT_0041e410) {
      printf("search pattern is too large");
      return 0;
    }
    if ((DAT_0041e410 & 1) != 0) {
      (&DAT_0041e017)[DAT_0041e410] = 0;
    }
    for (local_20 = 0; local_20 < DAT_0041e410; local_20 = local_20 + 1) {
      if (((local_20 + 1 & 1) == 0) && (local_20 != 0)) {
        if (((byte)(&DAT_0041e018)[local_20] < 0x30) || (0x39 < (byte)(&DAT_0041e018)[local_20])) {
          if (((byte)(&DAT_0041e018)[local_20] < 0x41) || (0x46 < (byte)(&DAT_0041e018)[local_20]))
          {
            if (((byte)(&DAT_0041e018)[local_20] < 0x61) || (0x66 < (byte)(&DAT_0041e018)[local_20])
               ) {
              puts("invalid search item");
              return 0;
            }
            (&DAT_0041e018)[local_20] = (&DAT_0041e018)[local_20] + -0x57;
          }
          else {
            (&DAT_0041e018)[local_20] = (&DAT_0041e018)[local_20] + -0x37;
          }
        }
        else {
          (&DAT_0041e018)[local_20] = (&DAT_0041e018)[local_20] + -0x30;
        }
        if (((byte)(&DAT_0041e017)[local_20] < 0x30) || (0x39 < (byte)(&DAT_0041e017)[local_20])) {
          if (((byte)(&DAT_0041e017)[local_20] < 0x41) || (0x46 < (byte)(&DAT_0041e017)[local_20]))
          {
            if (((byte)(&DAT_0041e017)[local_20] < 0x61) || (0x66 < (byte)(&DAT_0041e017)[local_20])
               ) {
              puts("invalid search item");
              return 0;
            }
            (&DAT_0041e017)[local_20] = (&DAT_0041e017)[local_20] + -0x57;
          }
          else {
            (&DAT_0041e017)[local_20] = (&DAT_0041e017)[local_20] + -0x37;
          }
        }
        else {
          (&DAT_0041e017)[local_20] = (&DAT_0041e017)[local_20] + -0x30;
        }
        (&DAT_0041dc30)[DAT_0041e424] =
             (&DAT_0041e017)[local_20] * '\x10' + (&DAT_0041e018)[local_20];
        DAT_0041e424 = DAT_0041e424 + 1;
      }
    }
    DAT_0041e40c = DAT_0041e424;
    DAT_0041e418 = (DAT_0041e41c + DAT_0041e408) - DAT_0041e424;
    DAT_0041e404 = DAT_0041e41c;
    DAT_0041e424 = DAT_0041e424 + -1;
    local_24 = (undefined1 *)calloc(1,0x10000);
    puts("searching...");
    for (; DAT_0041e404 <= DAT_0041e418; DAT_0041e404 = (DAT_0041e404 - DAT_0041e40c) + 0x10001) {
      if (DAT_0041e418 < (DAT_0041e404 - DAT_0041e40c) + 0x10000) {
        DAT_0041e400 = DAT_0041e418;
      }
      else {
        DAT_0041e400 = (DAT_0041e404 - DAT_0041e40c) + 0x10000;
      }
      iVar1 = FUN_00407c1c(local_24,DAT_0041e404,0x10000);
      if (iVar1 == 0) {
        puts("DMA Failed ");
        goto LAB_00407738;
      }
      local_28 = local_24;
      for (; DAT_0041e414 <= DAT_0041e400; DAT_0041e414 = DAT_0041e414 + 1) {
        iVar1 = memcmp(local_28,&DAT_0041dc30,DAT_0041e40c);
        if (iVar1 == 0) {
          sprintf(&DAT_0041dbcc,"%lx %lx",DAT_0041e414,DAT_0041d95c);
          printf("found at 0x%08X\n",DAT_0041e414);
          FUN_00401648(&DAT_0040bd20,4,&DAT_0041dbcc);
          DAT_0041e420 = 1;
          printf("startSearchDma:%X\tAddress:%X\tendSearchDma:%X\n",DAT_0041e404,DAT_0041e414,
                       DAT_0041e400);
          goto LAB_00407738;
        }
        local_28 = local_28 + 1;
      }
    }
    DAT_0041e404 = (DAT_0041e40c + DAT_0041e404) - 0x10001;
    puts("Data not found");
  }
  else {
    if ((param_2 != 0x1c) && (param_2 != 0x1d)) {
      return 0;
    }
    if (DAT_0041e420 == 0) {
      puts("Find command must be called sucessfully first");
      return 0;
    }
    if (param_2 == 0x1d) {
      if (DAT_0041e418 <= DAT_0041e414) {
        puts("Already at end of search area");
        return 0;
      }
      local_24 = (undefined1 *)calloc(1,0x10000);
      puts("searching..");
      iVar1 = DAT_0041e414 - DAT_0041e404;
      DAT_0041e414 = DAT_0041e414 + 1;
      local_28 = local_24 + iVar1 + 1;
      while (DAT_0041e404 <= DAT_0041e418) {
        iVar1 = FUN_00407c1c(local_24,DAT_0041e404,0x10000);
        if (iVar1 == 0) {
          puts("DMA Failed ");
          goto LAB_00407738;
        }
        for (; DAT_0041e414 <= DAT_0041e400; DAT_0041e414 = DAT_0041e414 + 1) {
          iVar1 = memcmp(local_28,&DAT_0041dc30,DAT_0041e40c);
          if (iVar1 == 0) {
            printf("address:%x pbPtr:%d, data length:%d\n",DAT_0041e414,*local_28,DAT_0041e40c
                        );
            sprintf(&DAT_0041dbcc,"%lx %lx",DAT_0041e414,DAT_0041d95c);
            printf("found at 0x%08X\n",DAT_0041e414);
            FUN_00401648(&DAT_0040bd20,4,&DAT_0041dbcc);
            DAT_0041e420 = 1;
            goto LAB_00407738;
          }
          local_28 = local_28 + 1;
        }
        DAT_0041e404 = (DAT_0041e404 - DAT_0041e40c) + 0x10001;
        if (DAT_0041e418 < (DAT_0041e404 - DAT_0041e40c) + 0x10000) {
          DAT_0041e400 = DAT_0041e418;
        }
        else {
          DAT_0041e400 = (DAT_0041e404 - DAT_0041e40c) + 0x10000;
        }
        local_28 = local_24;
      }
      DAT_0041e404 = (DAT_0041e40c + DAT_0041e404) - 0x10001;
      puts("Data not found");
    }
    else {
      if (param_2 != 0x1e) {
        return 0;
      }
      if (DAT_0041e414 <= DAT_0041e41c) {
        puts("Already at start of search area");
        return 0;
      }
      local_24 = (undefined1 *)calloc(1,0x10000);
      puts("searching..");
      iVar1 = (DAT_0041e414 - DAT_0041e404) + -1;
      DAT_0041e414 = DAT_0041e414 - 1;
      while (local_28 = local_24 + iVar1, DAT_0041e41c <= DAT_0041e414) {
        iVar1 = FUN_00407c1c(local_24,DAT_0041e404,0x10000);
        if (iVar1 == 0) {
          puts("DMA Failed ");
          goto LAB_00407738;
        }
        for (; DAT_0041e404 <= DAT_0041e414; DAT_0041e414 = DAT_0041e414 - 1) {
          iVar1 = memcmp(local_28,&DAT_0041dc30,DAT_0041e40c);
          if (iVar1 == 0) {
            sprintf(&DAT_0041dbcc,"%lx %lx",DAT_0041e414,DAT_0041d95c);
            printf("found at 0x%08X\n",DAT_0041e414);
            FUN_00401648(&DAT_0040bd20,4,&DAT_0041dbcc);
            DAT_0041e420 = 1;
            goto LAB_00407738;
          }
          local_28 = local_28 + -1;
        }
        DAT_0041e400 = (DAT_0041e400 + DAT_0041e40c) - 0x10001;
        if ((DAT_0041e400 + DAT_0041e40c) - 0x10000 < DAT_0041e41c) {
          DAT_0041e404 = DAT_0041e41c;
        }
        else {
          DAT_0041e404 = (DAT_0041e400 + DAT_0041e40c) - 0x10000;
        }
        iVar1 = 0x10000 - DAT_0041e40c;
      }
      DAT_0041e400 = (DAT_0041e404 - DAT_0041e40c) + 0x10000;
      puts("Data not found");
    }
  }
LAB_00407738:
  free(local_24);
  return 0;
}



/* ---- FUN_00407780 @ 00407780 ----
 * (library/support function; see decompiled body)
 */

int FUN_00407780(void)

{
  int iVar1;
  int local_bc;
  undefined4 local_b8;
  undefined4 local_b4;
  undefined4 local_ac;
  undefined4 local_24;
  undefined4 local_14;
  
  local_bc = 0;
  DAT_0041db5c = fopen("db656_reg_val.txt",&DAT_0040ca54);
  if (DAT_0041db5c == 0) {
    puts("Open db656 reg value log file failed ");
  }
  else {
    puts("Open db656 reg value log file successed ");
  }
  puts("Open Device Enter");
  DAT_0041e500 = open("/dev/vixs/xcodedrv",2);
  puts("Open Device Leave");
  if (DAT_0041e500 != 0) {
    memset(&local_b8,0,0xac);
    local_b4 = 9;
    local_b8 = 0xac;
    local_24 = 0xffffffff;
    puts("making, ioctl call to device to get mmr location");
    iVar1 = ioctl(DAT_0041e500,0x17241724,&local_b8);
    if (iVar1 == -1) {
      printf("GET MMR ADDRESS FAILED return val: %x\n",local_ac);
    }
    else {
      puts("GET MMR ADDRESS SUCCESS ");
      DAT_0041db60 = local_14;
      printf("The MMR pointer value is: %Xh\n",local_14);
      local_b4 = 8;
      local_b8 = 0xac;
      puts("making, ioctl call to device to get frame buffer location");
      iVar1 = ioctl(DAT_0041e500,0x17241724,&local_b8);
      if (iVar1 == -1) {
        puts("GET FB ADDRESS FAILED ");
      }
      else {
        puts("GET FB ADDRESS SUCCESS ");
        DAT_0041db64 = local_14;
        printf("The FRAMEBUFFER pointer value is: %Xh\n",local_14);
        local_bc = 1;
      }
    }
    if (local_bc == 0) {
      if (DAT_0041e500 != 0) {
        close(DAT_0041e500);
      }
      DAT_0041e500 = 0;
    }
  }
  return local_bc;
}



/* ---- FUN_00407aa4 @ 00407aa4 ----
 * (library/support function; see decompiled body)
 */

void FUN_00407aa4(void)

{
  if (DAT_0041e500 != 0) {
    close(DAT_0041e500);
  }
  DAT_0041e500 = 0;
  if (DAT_0041db5c != 0) {
    fclose(DAT_0041db5c);
  }
  return;
}



/* ---- FUN_00407b2c @ 00407b2c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00407b2c(void)

{
  return DAT_0041db64;
}



/* ---- FUN_00407b54 @ 00407b54 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00407b54(void)

{
  int *piVar1;
  undefined4 local_18;
  uint local_10;
  
  local_18 = 0;
  for (local_10 = 0; local_10 < 0x1000000; local_10 = local_10 + 4) {
    piVar1 = (int *)(DAT_0041db64 + local_10);
    *piVar1 = 0x55555555;
    if (*piVar1 != 0x55555555) {
      local_18 = 1;
    }
    *piVar1 = -0x55555556;
    if (*piVar1 != -0x55555556) {
      local_18 = 1;
    }
  }
  return local_18;
}



/* ---- FUN_00407c1c @ 00407c1c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00407c1c(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 local_10;
  
  memset(&DAT_0042f7d4,0,0xb4,param_4,0,0,0);
  DAT_0042f7d8 = 0xc;
  DAT_0042f7d4 = 0xb4;
  DAT_0042f868 = 0xffffffff;
  DAT_0042f878 = param_3;
  DAT_0042f87c = param_1;
  DAT_0042f884 = param_2;
  iVar1 = ioctl(DAT_0041e500,0x17241724,&DAT_0042f7d4);
  if (iVar1 == 0) {
    puts(" ERROR in DeviceIOControl: Failed to DMA data from FB to Host \n");
    local_10 = 0;
  }
  else if (DAT_0042f7e0 == 1) {
    local_10 = 1;
  }
  else {
    local_10 = 0;
  }
  return local_10;
}



/* ---- FUN_00407d98 @ 00407d98 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00407d98(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 local_10;
  
  memset(&DAT_0042f720,0,0xb4,param_4,0,0,0);
  DAT_0042f724 = 0xb;
  DAT_0042f720 = 0xb4;
  DAT_0042f7b4 = 0xffffffff;
  DAT_0042f7c4 = param_3;
  DAT_0042f7c8 = param_1;
  DAT_0042f7d0 = param_2;
  iVar1 = ioctl(DAT_0041e500,0x17241724,&DAT_0042f720);
  if (iVar1 == 0) {
    puts(" ERROR in DeviceIOControl: Failed to DMA data from Host to FB \n");
    local_10 = 0;
  }
  else if (DAT_0042f72c == 1) {
    local_10 = 1;
  }
  else {
    local_10 = 0;
  }
  return local_10;
}



/* ---- FUN_00407f14 @ 00407f14 ----
 * (library/support function; see decompiled body)
 */

int FUN_00407f14(undefined4 param_1,undefined4 param_2)

{
  int local_10;
  
  local_10 = ioctl(param_1,0x17241724,param_2);
  if (local_10 == -1) {
    local_10 = 0;
  }
  return local_10;
}



/* ---- FUN_00407f88 @ 00407f88 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00407f88(void)

{
  int iVar1;
  undefined4 local_10;
  
  DAT_0042f520 = 0xf0;
  DAT_0042f524 = 0x100;
  DAT_0042f5b4 = 5000;
  DAT_0042f52c = 2;
  DAT_0042f5b8 = 5;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f520,0xf0);
  if (iVar1 == 0) {
    puts("FAILED to identify board id");
    local_10 = 0xffffffff;
  }
  else if (DAT_0042f52c == 1) {
    local_10 = DAT_0042f5d0;
  }
  else {
    puts("FAILED to identify board id");
    local_10 = 0xffffffff;
  }
  return local_10;
}



/* ---- FUN_004080c8 @ 004080c8 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_004080c8(undefined4 param_1,int param_2)

{
  int iVar1;
  uint local_14;
  uint local_10;
  
  printf("\nProgramming Ic106_Sa711x_Init io expander : Src %d\n",param_2);
  local_14 = (uint)(param_2 == 0);
  local_10 = 0;
  do {
    if (0x185 < local_10) {
      printf("\nProgramming Ic106_Sa711x_Init io expander : Src %d Success >>>\n",param_2);
      return 0;
    }
    DAT_0042f6f8 = 1;
    DAT_0042f708 = 800;
    DAT_0042f6fc = local_14 << 1 | 0x40;
    DAT_0042f6b4 = (&DAT_0041d964)[local_10];
    DAT_0042f6b5 = (&DAT_0041d965)[local_10];
    DAT_0042f6f4 = 2;
    DAT_0042f710 = 1;
    DAT_0042f714 = 1;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Ic106_Sa711x_Init");
      return 1;
    }
    if (DAT_0042f61c == 1) {
      putchar(0x2e);
    }
    else {
      if ((DAT_0042f61c == 0xd) && (DAT_0041d960 != 0)) {
        printf("Failed Ic106_Sa711x_Init in array counter %d \n",local_10);
        printf("Failed .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
        usleep(50000);
        return 1;
      }
      if ((DAT_0042f61c == 0xf) && (DAT_0041d960 != 0)) {
        printf("Timeout Ic106_Sa711x_Init in array counter %d \n",local_10);
        printf("Timeout .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
        usleep(50000);
        return 1;
      }
    }
    local_10 = local_10 + 2;
  } while( true );
}



/* ---- FUN_0040840c @ 0040840c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040840c(undefined4 param_1,uint param_2)

{
  int iVar1;
  uint local_10;
  undefined4 local_c;
  
  local_c = 1;
  printf("Programming io expander to enable port %d \n",param_2);
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  DAT_0042f6fc = 0x40;
  DAT_0042f6b4 = 1;
  DAT_0042f6b5 = (char)(0x80 >> (param_2 & 0x1f)) + '\x0f';
  DAT_0042f6f4 = 2;
  DAT_0042f710 = 1;
  DAT_0042f714 = 1;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Sa711x_Init");
  }
  else if (((DAT_0042f61c == 1) || (DAT_0042f61c != 0xd)) || (DAT_0041d960 == 0)) {
    printf("Programming io expander to enable port %d Success >>>\n",param_2);
    usleep(50000);
    puts("Programming SA7115 device ");
    for (local_10 = 0; local_10 < 0x186; local_10 = local_10 + 2) {
      DAT_0042f6f8 = 1;
      DAT_0042f708 = 800;
      DAT_0042f6fc = 0x42;
      DAT_0042f6b4 = (&DAT_0041d964)[local_10];
      DAT_0042f6b5 = (&DAT_0041d965)[local_10];
      DAT_0042f6f4 = 2;
      DAT_0042f710 = 1;
      DAT_0042f714 = 1;
      DAT_0042f70c = param_1;
      iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
      if (iVar1 == 0) {
        puts("Failed to connect to the driver via IOCTL in Sa711x_Init");
        return 1;
      }
      if (DAT_0042f61c == 1) {
        putchar(0x2e);
      }
      else {
        if ((DAT_0042f61c == 0xd) && (DAT_0041d960 != 0)) {
          printf("Failed Sa711x_Init in array counter %d \n",local_10);
          printf("Failed .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
          usleep(50000);
          return 1;
        }
        if ((DAT_0042f61c == 0xf) && (DAT_0041d960 != 0)) {
          printf("Timeout Sa711x_Init in array counter %d \n",local_10);
          printf("Timeout .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
          usleep(50000);
          return 1;
        }
      }
    }
    puts("\nProgramming SA7115 device Success >>>");
    local_c = 0;
  }
  else {
    printf("Failed Sa711x_Init in array counter %d \n",0);
    printf("Failed .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
    usleep(50000);
  }
  return local_c;
}



/* ---- FUN_00408924 @ 00408924 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00408924(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  uint uVar2;
  undefined4 uVar3;
  uint local_10;
  
  uVar3 = 0;
  printf("\nProgramming Adv_7185_Init io expander : Src %d\n",param_2);
  uVar2 = 0x44;
  if (param_1 == 0) {
    DAT_0042f6fc = 0x8a;
  }
  else {
    DAT_0042f6fc = 0x88;
  }
  local_10 = 0;
  do {
    if (uVar2 <= local_10) {
      printf("\nProgramming Adv_7185_Init io expander : Src %d Success >>>\n",param_2);
      usleep(50000);
      return 0;
    }
    DAT_0042f6f8 = 1;
    DAT_0042f708 = 800;
    DAT_0042f6b4 = (&DAT_0041daec)[local_10];
    DAT_0042f6b5 = (&DAT_0041daed)[local_10];
    DAT_0042f6f4 = 2;
    DAT_0042f710 = 1;
    DAT_0042f714 = 1;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110,param_4,uVar2,uVar3);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Adv_7185_Init");
      return 1;
    }
    if (DAT_0042f61c == 1) {
      putchar(0x2e);
    }
    else {
      if ((DAT_0042f61c == 0xd) && (DAT_0041d960 != 0)) {
        printf("Failed Adv_7185_Init in array counter %d \n",local_10);
        printf("Failed .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
        usleep(50000);
        return 1;
      }
      if ((DAT_0042f61c == 0xf) && (DAT_0041d960 != 0)) {
        printf("Timeout Adv_7185_Init in array counter %d \n",local_10);
        printf("Timeout .... error code %.8Xh ... we are going to retry\n",DAT_0042f61c);
        usleep(50000);
        return 1;
      }
    }
    local_10 = local_10 + 2;
  } while( true );
}



/* ---- FUN_00408c7c @ 00408c7c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00408c7c(undefined4 param_1,int param_2,undefined1 param_3)

{
  int iVar1;
  uint local_10;
  undefined4 local_c;
  
  local_c = 1;
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  local_10 = (uint)(param_2 == 0);
  DAT_0042f6fc = local_10 << 1 | 0x40;
  DAT_0042f6f4 = 1;
  DAT_0042f710 = 1;
  DAT_0042f714 = 0;
  DAT_0042f6b4 = param_3;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Sa711x_Init (WRITE)");
  }
  else if (DAT_0042f61c == 1) {
    DAT_0042f6f8 = 0;
    DAT_0042f708 = 800;
    local_10 = (uint)(param_2 == 0);
    DAT_0042f6fc = local_10 << 1 | 0x40;
    DAT_0042f6b4 = 0;
    DAT_0042f6b5 = 0;
    DAT_0042f6f4 = 1;
    DAT_0042f710 = 1;
    DAT_0042f714 = 1;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Sa711x_Init (READ)");
    }
    else if (DAT_0042f61c == 1) {
      printf("%#02lx %#02lx \n",param_3,DAT_0042f6b4);
      fprintf(DAT_0041db5c,"%#02lx %#02lx \n",param_3,DAT_0042f6b4);
      fflush(DAT_0041db5c);
      local_c = 0;
    }
    else {
      printf("Failed Ic106_Sa711x_Init Read (Read Operation) : Error %lx \n",DAT_0042f61c);
      usleep(50000);
    }
  }
  else {
    printf("Failed Ic106_Sa711x_Init Write (Read Operation) : Error %lx \n",DAT_0042f61c);
    usleep(50000);
  }
  return local_c;
}



/* ---- FUN_0040902c @ 0040902c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040902c(undefined4 param_1,uint param_2,undefined1 param_3)

{
  int iVar1;
  undefined4 local_c;
  
  local_c = 1;
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  DAT_0042f6fc = 0x40;
  DAT_0042f6b4 = 1;
  DAT_0042f6b5 = (char)(0x80 >> (param_2 & 0x1f)) + '\x0f';
  DAT_0042f6f4 = 2;
  DAT_0042f710 = 1;
  DAT_0042f714 = 1;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Sa711x_Init");
  }
  else if (DAT_0042f61c == 1) {
    DAT_0042f6f8 = 1;
    DAT_0042f708 = 800;
    DAT_0042f6fc = 0x42;
    DAT_0042f6f4 = 1;
    DAT_0042f710 = 1;
    DAT_0042f714 = 0;
    DAT_0042f6b4 = param_3;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Sa711x_Init");
    }
    else if (DAT_0042f61c == 1) {
      DAT_0042f6f8 = 0;
      DAT_0042f708 = 800;
      DAT_0042f6fc = 0x42;
      DAT_0042f6b4 = 0;
      DAT_0042f6b5 = '\0';
      DAT_0042f6f4 = 1;
      DAT_0042f710 = 1;
      DAT_0042f714 = 1;
      DAT_0042f70c = param_1;
      iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
      if (iVar1 == 0) {
        puts("Failed to connect to the driver via IOCTL in Sa711x_Init");
      }
      else if (DAT_0042f61c == 1) {
        printf("%#02lx %#02lx \n",param_3,DAT_0042f6b4);
        fprintf(DAT_0041db5c,"%#02lx %#02lx \n",param_3,DAT_0042f6b4);
        fflush(DAT_0041db5c);
        local_c = 0;
      }
      else {
        printf("Failed Ic106_Sa711x_Init Read (Read Operation) : Error %lx \n",DAT_0042f61c);
        usleep(50000);
      }
    }
    else {
      printf("Failed Ic106_Sa711x_Init Write (Read Operation) : Error %lx \n",DAT_0042f61c);
      usleep(50000);
    }
  }
  else {
    printf("Failed Ic106_Sa711x_Init Write (Read Operation) : Error %lx \n",DAT_0042f61c);
    usleep(50000);
  }
  return local_c;
}



/* ---- FUN_00409500 @ 00409500 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_00409500(undefined4 param_1,int param_2,undefined1 param_3)

{
  int iVar1;
  
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  if (param_2 == 0) {
    DAT_0042f6fc = 0x8a;
  }
  else {
    DAT_0042f6fc = 0x88;
  }
  DAT_0042f6f4 = 1;
  DAT_0042f710 = 1;
  DAT_0042f714 = 0;
  DAT_0042f6b4 = param_3;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Adv_7185_Init (WRITE)");
    return 1;
  }
  if (DAT_0042f61c == 1) {
    DAT_0042f6f8 = 0;
    DAT_0042f708 = 800;
    if (param_2 == 0) {
      DAT_0042f6fc = 0x8a;
    }
    else {
      DAT_0042f6fc = 0x88;
    }
    DAT_0042f6b4 = 0;
    DAT_0042f6b5 = 0;
    DAT_0042f6f4 = 1;
    DAT_0042f710 = 1;
    DAT_0042f714 = 1;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Adv_7185_Init (READ)");
      return 1;
    }
    if (DAT_0042f61c != 1) {
      printf("Failed Adv_7185_Init Read (Read Operation) : Error %lx \n",DAT_0042f61c);
      usleep(50000);
      return 1;
    }
    printf("%#02lx %#02lx \n",param_3,DAT_0042f6b4);
    fprintf(DAT_0041db5c,"%#02lx %#02lx \n",param_3,DAT_0042f6b4);
    fflush(DAT_0041db5c);
  }
  usleep(50000);
  return 0;
}



/* ---- FUN_00409878 @ 00409878 ----
 * (library/support function; see decompiled body)
 */

undefined4
FUN_00409878(int param_1,int param_2,int param_3,int param_4,char param_5,undefined1 param_6,
            uint param_7)

{
  uint local_24;
  uint local_20;
  
  memset(&DAT_0042f610,0,0x110,param_4,0);
  DAT_0042f614 = 0x1b;
  DAT_0042f610 = 0x110;
  DAT_0042f61c = 2;
  DAT_0042f704 = 0;
  DAT_0042f700 = 0x58;
  if (param_1 == 5) {
    puts(&DAT_0040d304);
    if (param_4 != 0) {
      if (param_2 == 0) {
        DAT_0041d967 = 0x85;
        DAT_0041d975 = 0x48;
      }
      else {
        DAT_0041d967 = 0x89;
        DAT_0041d975 = 0x88;
      }
      if (param_3 == 0) {
        DAT_0041da11 = 0xb0;
      }
      else {
        DAT_0041da11 = 0xb1;
      }
    }
    for (local_20 = 0; local_20 < 0x186; local_20 = local_20 + 2) {
      if ((param_5 == (&DAT_0041d964)[local_20]) && (param_4 != 0)) {
        (&DAT_0041d965)[local_20] = param_6;
      }
    }
    if ((param_7 == 2) || (param_7 == 3)) {
      DAT_0041e504 = 1;
      if (param_4 != 0) {
        FUN_004080c8(1,param_7 - 2);
        return 1;
      }
      if (param_5 != '\0') {
        printf("RegIndex = %d \n",param_5);
        FUN_00408c7c(DAT_0041e504,param_7,param_5);
        return 1;
      }
      for (local_24 = 0; local_24 < 0x186; local_24 = local_24 + 2) {
        FUN_00408c7c(DAT_0041e504,param_7 - 2,(&DAT_0041d964)[local_24]);
      }
      fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                   param_7);
      return 1;
    }
    if ((param_7 != 0) && (param_7 != 1)) {
      printf("SME Card Only has 4 decoder chipsets (0/1/2/3) : Error to program -> %d decoder chipset\n"
                   ,param_7);
      return 1;
    }
    DAT_0041e504 = 0;
    if (param_4 != 0) {
      FUN_004080c8(0,param_7);
      return 1;
    }
    if (param_5 != '\0') {
      FUN_00408c7c(0,param_7,param_5);
      return 1;
    }
    for (local_24 = 0; local_24 < 0x186; local_24 = local_24 + 2) {
      fprintf(DAT_0041db5c,"%d : ",param_7);
      FUN_00408c7c(DAT_0041e504,param_7,(&DAT_0041d964)[local_24]);
    }
    fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                 param_7);
    return 1;
  }
  if (param_1 < 6) {
    if (param_1 == 0) {
      puts("SLP3 ");
      if (param_4 != 0) {
        if (param_2 == 0) {
          DAT_0041d967 = 0x85;
          DAT_0041d975 = 0x48;
        }
        else {
          DAT_0041d967 = 0x89;
          DAT_0041d975 = 0x88;
        }
        if (param_3 == 0) {
          DAT_0041da11 = 0xb0;
        }
        else {
          DAT_0041da11 = 0xb1;
        }
      }
      for (local_20 = 0; local_20 < 0x186; local_20 = local_20 + 2) {
        if ((param_5 == (&DAT_0041d964)[local_20]) && (param_4 != 0)) {
          (&DAT_0041d965)[local_20] = param_6;
        }
      }
      if (3 < param_7) {
        printf("SLP3 Card Only has 2 decoder chipsets (0/1) : Error to program -> %d decoder chipset\n"
                     ,param_7);
        return 1;
      }
      DAT_0041e504 = 0;
      if (param_4 != 0) {
        FUN_004080c8(0,param_7);
        return 1;
      }
      if (param_5 != '\0') {
        FUN_00408c7c(0,param_7,param_5);
        return 1;
      }
      fprintf(DAT_0041db5c,"===========================================> START FOR SRC %d\n",
                   param_7);
      for (local_24 = 0; local_24 < 0x186; local_24 = local_24 + 2) {
        fprintf(DAT_0041db5c,"%d : ",param_7);
        FUN_00408c7c(DAT_0041e504,param_7,(&DAT_0041d964)[local_24]);
      }
      fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                   param_7);
      return 1;
    }
    if (param_1 == 1) {
      puts("SLP2 ");
      if (param_4 != 0) {
        if (param_3 == 0) {
          DAT_0041db21 = 0x3c;
        }
        else {
          DAT_0041db21 = 0xbc;
        }
      }
      for (local_20 = 0; local_20 < 0x44; local_20 = local_20 + 2) {
        if ((param_5 == (&DAT_0041daec)[local_20]) && (param_4 != 0)) {
          (&DAT_0041daed)[local_20] = param_6;
        }
      }
      if (1 < param_7) {
        printf("SLP2 Card Only has 2 decoder chipsets (0/1) : Error to program -> %d decoder chipset\n"
                     ,param_7);
        return 1;
      }
      DAT_0041e504 = 0;
      if (param_4 != 0) {
        FUN_00408924(0,param_7);
        return 1;
      }
      if (param_5 != '\0') {
        FUN_00409500(0,param_7,param_5);
        return 1;
      }
      fprintf(DAT_0041db5c,"===========================================> START FOR SRC %d\n",
                   param_7);
      for (local_24 = 0; local_24 < 0x44; local_24 = local_24 + 2) {
        fprintf(DAT_0041db5c,"%d : ",param_7);
        FUN_00409500(DAT_0041e504,param_7,(&DAT_0041daec)[local_24]);
      }
      fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                   param_7);
      return 1;
    }
  }
  else {
    if (param_1 == 0x20) {
      puts("High Profile ");
      if (param_4 != 0) {
        if (param_2 == 0) {
          DAT_0041d967 = 0x85;
          DAT_0041d975 = 0x48;
        }
        else {
          DAT_0041d967 = 0x89;
          DAT_0041d975 = 0x88;
        }
        if (param_3 == 0) {
          DAT_0041da11 = 0xb0;
        }
        else {
          DAT_0041da11 = 0xb1;
        }
      }
      for (local_20 = 0; local_20 < 0x186; local_20 = local_20 + 2) {
        if ((param_5 == (&DAT_0041d964)[local_20]) && (param_4 != 0)) {
          (&DAT_0041d965)[local_20] = param_6;
        }
      }
      if (3 < param_7) {
        printf("High Profile Card Only has 4 decoder chipsets (0/1/2/3) : Error to program -> %d decoder chipset\n"
                     ,param_7);
        return 1;
      }
      DAT_0041e504 = 0;
      if (param_4 != 0) {
        FUN_0040840c(0,param_7);
        return 1;
      }
      if (param_5 != '\0') {
        FUN_0040902c(0,param_7,param_5);
        return 1;
      }
      fprintf(DAT_0041db5c,"===========================================> START FOR SRC %d\n",
                   param_7);
      for (local_24 = 0; local_24 < 0x186; local_24 = local_24 + 2) {
        fprintf(DAT_0041db5c,"%d : ",param_7);
        FUN_0040902c(DAT_0041e504,param_7,(&DAT_0041d964)[local_24]);
      }
      fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                   param_7);
      return 1;
    }
    if (param_1 == 0x1000001) {
      puts("Mini ");
      if (param_4 != 0) {
        if (param_2 == 0) {
          DAT_0041d967 = 0x85;
          DAT_0041d975 = 0x48;
        }
        else {
          DAT_0041d967 = 0x89;
          DAT_0041d975 = 0x88;
        }
        if (param_3 == 0) {
          DAT_0041da11 = 0xb0;
        }
        else {
          DAT_0041da11 = 0xb1;
        }
      }
      for (local_20 = 0; local_20 < 0x186; local_20 = local_20 + 2) {
        if ((param_5 == (&DAT_0041d964)[local_20]) && (param_4 != 0)) {
          (&DAT_0041d965)[local_20] = param_6;
        }
      }
      if ((param_7 != 0) && (param_7 != 1)) {
        printf("Mini Card Only has 2 decoder chipsets (0/1) : Error to program -> %d decoder chipset\n"
                     ,param_7);
        return 1;
      }
      DAT_0041e504 = 0;
      if (param_4 != 0) {
        FUN_004080c8(0,param_7);
        return 1;
      }
      if (param_5 != '\0') {
        FUN_00408c7c(0,param_7,param_5);
        return 1;
      }
      for (local_24 = 0; local_24 < 0x186; local_24 = local_24 + 2) {
        fprintf(DAT_0041db5c,"%d : ",param_7);
        FUN_00408c7c(DAT_0041e504,param_7,(&DAT_0041d964)[local_24]);
      }
      fprintf(DAT_0041db5c,"===========================================> END FOR SRC %d\n",
                   param_7);
      return 1;
    }
  }
  DAT_0041e504 = 0;
  if (param_4 == 0) {
    printf("read i2c address 0x%x register 0x%x\n",param_7,param_5);
    FUN_0040aacc(DAT_0041e504,param_7,param_5);
  }
  else {
    printf("write i2c address 0x%x register 0x%x with 0x%x\n",param_7,param_5,param_6);
    FUN_0040a808(DAT_0041e504,param_7,param_5,param_6);
  }
  return 1;
}



/* ---- FUN_0040a808 @ 0040a808 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040a808(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  printf("\nProgramming Mot_Init io i2c address 0x%x\n",param_2,param_3,param_4,0,0);
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  DAT_0042f6b4 = (undefined1)param_3;
  DAT_0042f6b5 = (undefined1)param_4;
  DAT_0042f6f4 = 2;
  DAT_0042f710 = 1;
  DAT_0042f714 = 1;
  DAT_0042f6fc = param_2;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Mot_Init");
    return 1;
  }
  if (DAT_0042f61c != 1) {
    if ((DAT_0042f61c == 0xd) && (DAT_0041d960 != 0)) {
      printf("Transfer error, ret = %x\n",0xd);
      usleep(50000);
      return 1;
    }
    if ((DAT_0042f61c == 0xf) && (DAT_0041d960 != 0)) {
      printf("Timeout .... error code %x\n",0xf);
      usleep(50000);
      return 1;
    }
  }
  printf("\nProgramming Mot_init io i2c address 0x%x Success >>>\n",param_2);
  usleep(50000);
  return 0;
}



/* ---- FUN_0040aacc @ 0040aacc ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040aacc(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  int iVar1;
  
  DAT_0042f6f8 = 1;
  DAT_0042f708 = 800;
  DAT_0042f6f4 = 1;
  DAT_0042f710 = 1;
  DAT_0042f714 = 0;
  DAT_0042f6b4 = param_3;
  DAT_0042f6fc = param_2;
  DAT_0042f70c = param_1;
  iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
  if (iVar1 == 0) {
    puts("Failed to connect to the driver via IOCTL in Mot_Init (READ)");
    return 1;
  }
  if (DAT_0042f61c == 1) {
    DAT_0042f6f8 = 0;
    DAT_0042f708 = 800;
    DAT_0042f6b4 = 0;
    DAT_0042f6b5 = 0;
    DAT_0042f6f4 = 1;
    DAT_0042f710 = 1;
    DAT_0042f714 = 1;
    DAT_0042f6fc = param_2;
    DAT_0042f70c = param_1;
    iVar1 = FUN_00407f14(DAT_0041e500,&DAT_0042f610,0x110);
    if (iVar1 == 0) {
      puts("Failed to connect to the driver via IOCTL in Mot_Init (READ)");
      return 1;
    }
    if (DAT_0042f61c != 1) {
      printf("Failed Mot_Init Read (Read Operation) : Error %lx \n",DAT_0042f61c);
      usleep(50000);
      return 1;
    }
    printf("%#02lx %#02lx \n",param_3,DAT_0042f6b4);
  }
  else {
    if (DAT_0042f61c == 0xd) {
      printf("Transfer error, ret = %x\n",0xd);
      usleep(50000);
      return 1;
    }
    if (DAT_0042f61c == 0xf) {
      printf("Timeout .... error code %x\n",0xf);
      usleep(50000);
      return 1;
    }
  }
  usleep(50000);
  return 0;
}



/* ---- FUN_0040aec0 @ 0040aec0 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040aec0(int param_1,undefined4 *param_2)

{
  bool bVar1;
  int iVar2;
  int local_res0;
  undefined4 *local_res4;
  undefined1 local_70 [100];
  undefined4 local_c;
  
  bVar1 = true;
  local_c = 0;
  printf("Carrera FPGA Diagnostic %s\n\n","v3.1.1");
  iVar2 = FUN_0040b12c();
  if (iVar2 == 1) {
    local_70[0] = 0;
    while( true ) {
      local_res4 = param_2 + 1;
      local_res0 = param_1 + -1;
      if (local_res0 < 1) break;
      iVar2 = strcmp(*local_res4,&DAT_0040d6d0);
      if (iVar2 == 0) {
        strcpy(local_70,param_2[2]);
        param_1 = param_1 + -2;
        param_2 = param_2 + 2;
      }
      else {
        iVar2 = strcmp(*local_res4,"-loop");
        if (iVar2 == 0) {
          sscanf(param_2[2],&DAT_0040d6dc,&local_c);
          param_1 = param_1 + -2;
          param_2 = param_2 + 2;
        }
        else {
          iVar2 = strcmp(*local_res4,"-exit");
          param_1 = local_res0;
          param_2 = local_res4;
          if (iVar2 == 0) {
            bVar1 = false;
          }
          else {
            printf("Warning: unsupported cmdline param \'%s\'\n",*local_res4);
            bVar1 = false;
          }
        }
      }
    }
    if (bVar1) {
      FUN_00401010();
    }
  }
  else {
    puts("Error: failed appInit");
  }
  FUN_0040b16c();
  return 0;
}



/* ---- FUN_0040b12c @ 0040b12c ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040b12c(void)

{
  undefined4 uVar1;
  
  uVar1 = FUN_00407780();
  return uVar1;
}



/* ---- FUN_0040b16c @ 0040b16c ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b16c(void)

{
  FUN_00407aa4();
  return;
}



/* ---- FUN_0040b1b0 @ 0040b1b0 ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b1b0(int param_1,undefined4 param_2)

{
  *(undefined4 *)(DAT_0041db60 + param_1) = param_2;
  return;
}



/* ---- FUN_0040b1f4 @ 0040b1f4 ----
 * (library/support function; see decompiled body)
 */

undefined4 FUN_0040b1f4(int param_1)

{
  return *(undefined4 *)(DAT_0041db60 + param_1);
}



/* ---- FUN_0040b230 @ 0040b230 ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b230(void)

{
  undefined1 auStack_40 [12];
  uint local_34;
  
  tcgetattr(0,auStack_40);
  local_34 = local_34 & 0xfffffff5;
  tcsetattr(0,0x540e,auStack_40);
  signal(2,FUN_0040b3cc);
  return;
}



/* ---- FUN_0040b2c0 @ 0040b2c0 ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b2c0(void)

{
  undefined1 auStack_40 [12];
  uint local_34;
  
  tcgetattr(0,auStack_40);
  local_34 = local_34 | 10;
  tcsetattr(0,0x540e,auStack_40);
  signal(2,0);
  return;
}



/* ---- FUN_0040b348 @ 0040b348 ----
 * (library/support function; see decompiled body)
 */

int FUN_0040b348(void)

{
  char cVar1;
  int local_14;
  int local_10;
  
  local_14 = 0;
  ioctl(0,0x467f,&local_14);
  if (local_14 == 0) {
    local_10 = 0;
  }
  else {
    cVar1 = getchar();
    local_10 = (int)cVar1;
  }
  return local_10;
}



/* ---- FUN_0040b3cc @ 0040b3cc ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b3cc(void)

{
  char *pcVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  code *pcVar6;
  char cStack_c0;
  int iStack_bc;
  int iStack_ac;
  char acStack_a8 [128];
  int iStack_28;
  int iStack_24;
  undefined1 *puStack_20;
  code *pcStack_1c;
  undefined1 *apuStack_18 [4];
  
  FUN_0040b2c0();
  putchar(10);
  puVar5 = (undefined1 *)0x0;
  pcVar6 = FUN_0040b410;
  exit();
  iStack_ac = DAT_0041f510;
  if (DAT_0041f510 < DAT_0041f514) {
    iStack_24 = DAT_0041f510;
  }
  else {
    iStack_24 = 0;
  }
  iVar4 = iStack_24;
  iStack_28 = DAT_0041f514;
  if (0 < DAT_0041f510) {
    iStack_28 = DAT_0041f510;
  }
  iStack_28 = iStack_28 + -1;
  iVar3 = iStack_28;
  bVar2 = false;
  puStack_20 = (undefined1 *)apuStack_18;
  pcStack_1c = pcVar6;
  apuStack_18[0] = puVar5;
  memset(acStack_a8,0,0x80);
  iStack_bc = 0;
  FUN_0040b230();
  do {
    cStack_c0 = getchar();
    if ((cStack_c0 < ' ') || (cStack_c0 == '\x7f')) {
      if ((cStack_c0 == '\b') || (cStack_c0 == '\x7f')) {
        if (0 < iStack_bc) {
          acStack_a8[iStack_bc] = '\0';
          iStack_bc = iStack_bc + -1;
          putchar(8);
          putchar(0x20);
          putchar(8);
        }
      }
      else if (cStack_c0 == '\x1b') {
        getchar();
        cStack_c0 = getchar();
        if (cStack_c0 == 'A') {
          if ((!bVar2) || (iStack_ac != iVar4)) {
            bVar2 = true;
            while (0 < iStack_bc) {
              acStack_a8[iStack_bc] = '\0';
              iStack_bc = iStack_bc + -1;
              putchar(8);
              putchar(0x20);
              putchar(8);
            }
            iStack_ac = iStack_ac + -1;
            if (iStack_ac < 0) {
              iStack_ac = DAT_0041f514 + -1;
            }
            strcpy(acStack_a8,iStack_ac * 0x80 + 0x41e510);
            while (acStack_a8[iStack_bc] != '\0') {
              pcVar1 = acStack_a8 + iStack_bc;
              iStack_bc = iStack_bc + 1;
              putchar((int)*pcVar1);
            }
          }
        }
        else if (cStack_c0 == 'B') {
          if ((iStack_ac != iVar3) && (bVar2)) {
            while (0 < iStack_bc) {
              acStack_a8[iStack_bc] = '\0';
              iStack_bc = iStack_bc + -1;
              putchar(8);
              putchar(0x20);
              putchar(8);
            }
            iStack_ac = iStack_ac + 1;
            if (DAT_0041f514 <= iStack_ac) {
              iStack_ac = 0;
            }
            strcpy(acStack_a8,iStack_ac * 0x80 + 0x41e510);
            while (acStack_a8[iStack_bc] != '\0') {
              pcVar1 = acStack_a8 + iStack_bc;
              iStack_bc = iStack_bc + 1;
              putchar((int)*pcVar1);
            }
          }
        }
        else if ((((cStack_c0 != 'D') && (cStack_c0 != 'C')) && ('0' < cStack_c0)) &&
                (cStack_c0 < '7')) {
          cStack_c0 = getchar();
        }
      }
    }
    else if (iStack_bc < 0x80) {
      acStack_a8[iStack_bc] = cStack_c0;
      iStack_bc = iStack_bc + 1;
      putchar((int)cStack_c0);
    }
  } while (cStack_c0 != '\n');
  putchar(10);
  acStack_a8[iStack_bc] = '\0';
  if (iStack_bc < 1) {
    if (iStack_bc == 0) {
      iStack_ac = iStack_ac + -1;
      if (iStack_ac < 0) {
        iStack_ac = DAT_0041f514 + -1;
      }
      strcpy(acStack_a8,iStack_ac * 0x80 + 0x41e510);
    }
  }
  else {
    iStack_ac = DAT_0041f510 + -1;
    if (iStack_ac < 0) {
      iStack_ac = DAT_0041f514 + -1;
    }
    iVar4 = strcmp(iStack_ac * 0x80 + 0x41e510,acStack_a8);
    if (iVar4 != 0) {
      strcpy(DAT_0041f510 * 0x80 + 0x41e510,acStack_a8);
      if (DAT_0041f514 < 0x20) {
        DAT_0041f514 = DAT_0041f514 + 1;
      }
      DAT_0041f510 = DAT_0041f510 + 1;
      if (0x1f < DAT_0041f510) {
        DAT_0041f510 = 0;
      }
    }
  }
  if ((DAT_0041f514 == 0) && (iStack_bc == 0)) {
    puts("Error: no previous command available");
    *apuStack_18[0] = 0x48;
  }
  else {
    strcpy(apuStack_18[0],acStack_a8);
  }
  FUN_0040b2c0();
  return;
}



/* ---- FUN_0040b410 @ 0040b410 ----
 * (library/support function; see decompiled body)
 */

void FUN_0040b410(undefined1 *param_1)

{
  char *pcVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  char local_a8;
  int local_a4;
  int local_94;
  char acStack_90 [128];
  int local_10;
  int local_c;
  
  local_94 = DAT_0041f510;
  if (DAT_0041f510 < DAT_0041f514) {
    local_c = DAT_0041f510;
  }
  else {
    local_c = 0;
  }
  iVar4 = local_c;
  local_10 = DAT_0041f514;
  if (0 < DAT_0041f510) {
    local_10 = DAT_0041f510;
  }
  local_10 = local_10 + -1;
  iVar3 = local_10;
  bVar2 = false;
  memset(acStack_90,0,0x80);
  local_a4 = 0;
  FUN_0040b230();
  do {
    local_a8 = getchar();
    if ((local_a8 < ' ') || (local_a8 == '\x7f')) {
      if ((local_a8 == '\b') || (local_a8 == '\x7f')) {
        if (0 < local_a4) {
          acStack_90[local_a4] = '\0';
          local_a4 = local_a4 + -1;
          putchar(8);
          putchar(0x20);
          putchar(8);
        }
      }
      else if (local_a8 == '\x1b') {
        getchar();
        local_a8 = getchar();
        if (local_a8 == 'A') {
          if ((!bVar2) || (local_94 != iVar4)) {
            bVar2 = true;
            while (0 < local_a4) {
              acStack_90[local_a4] = '\0';
              local_a4 = local_a4 + -1;
              putchar(8);
              putchar(0x20);
              putchar(8);
            }
            local_94 = local_94 + -1;
            if (local_94 < 0) {
              local_94 = DAT_0041f514 + -1;
            }
            strcpy(acStack_90,local_94 * 0x80 + 0x41e510);
            while (acStack_90[local_a4] != '\0') {
              pcVar1 = acStack_90 + local_a4;
              local_a4 = local_a4 + 1;
              putchar((int)*pcVar1);
            }
          }
        }
        else if (local_a8 == 'B') {
          if ((local_94 != iVar3) && (bVar2)) {
            while (0 < local_a4) {
              acStack_90[local_a4] = '\0';
              local_a4 = local_a4 + -1;
              putchar(8);
              putchar(0x20);
              putchar(8);
            }
            local_94 = local_94 + 1;
            if (DAT_0041f514 <= local_94) {
              local_94 = 0;
            }
            strcpy(acStack_90,local_94 * 0x80 + 0x41e510);
            while (acStack_90[local_a4] != '\0') {
              pcVar1 = acStack_90 + local_a4;
              local_a4 = local_a4 + 1;
              putchar((int)*pcVar1);
            }
          }
        }
        else if ((((local_a8 != 'D') && (local_a8 != 'C')) && ('0' < local_a8)) && (local_a8 < '7'))
        {
          local_a8 = getchar();
        }
      }
    }
    else if (local_a4 < 0x80) {
      acStack_90[local_a4] = local_a8;
      local_a4 = local_a4 + 1;
      putchar((int)local_a8);
    }
  } while (local_a8 != '\n');
  putchar(10);
  acStack_90[local_a4] = '\0';
  if (local_a4 < 1) {
    if (local_a4 == 0) {
      local_94 = local_94 + -1;
      if (local_94 < 0) {
        local_94 = DAT_0041f514 + -1;
      }
      strcpy(acStack_90,local_94 * 0x80 + 0x41e510);
    }
  }
  else {
    local_94 = DAT_0041f510 + -1;
    if (local_94 < 0) {
      local_94 = DAT_0041f514 + -1;
    }
    iVar4 = strcmp(local_94 * 0x80 + 0x41e510,acStack_90);
    if (iVar4 != 0) {
      strcpy(DAT_0041f510 * 0x80 + 0x41e510,acStack_90);
      if (DAT_0041f514 < 0x20) {
        DAT_0041f514 = DAT_0041f514 + 1;
      }
      DAT_0041f510 = DAT_0041f510 + 1;
      if (0x1f < DAT_0041f510) {
        DAT_0041f510 = 0;
      }
    }
  }
  if ((DAT_0041f514 == 0) && (local_a4 == 0)) {
    puts("Error: no previous command available");
    *param_1 = 0x48;
  }
  else {
    strcpy(param_1,acStack_90);
  }
  FUN_0040b2c0();
  return;
}



/* ---- FUN_0040bb68 @ 0040bb68 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x0040bbc8) */
/* WARNING: Removing unreachable block (ram,0x0040bbcc) */

void FUN_0040bb68(void)

{
  (*(code *)PTR__init_0041db50)();
  return;
}



/* ---- FUN_0040bc20 @ 0040bc20 ----
 * (library/support function; see decompiled body)
 */

void FUN_0040bc20(void)

{
  undefined4 *puVar1;
  code *pcVar2;
  
  if (__init_array_end != (code *)0xffffffff) {
    puVar1 = &__init_array_end;
    pcVar2 = __init_array_end;
    do {
      puVar1 = puVar1 + -1;
      (*pcVar2)();
      pcVar2 = (code *)*puVar1;
    } while (pcVar2 != (code *)0xffffffff);
  }
  return;
}



/* ---- _DT_FINI @ 0040bc80 ----
 * (library/support function; see decompiled body)
 */

void _DT_FINI(void)

{
  FUN_00400c40();
  return;
}


