"/*\n * mtd_char_op.annotated.c  --  annotated Ghidra decompile of root/bin/mtd_char_op\n * ============================================================================\n * Low-level NAND MTD utility: dump / erase / read / write raw flash partitions\n * (/dev/mtd0../dev/mtd9). Used to flash the boot and kernel partitions during\n * firmware upgrade/recovery (the A/B image mechanism lives in actimage). See\n * docs/boot-process.md section 6.4.\n *\n * FUN_<addr> libc PLT stubs have been renamed to their real names (from the\n * ELF .rel.plt). Function names are original DWARF symbols; locals keep\n * Ghidra's auto-names. Nothing here was produced by running the binary.\n */"

/* ---- _init @ 00400784 ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  if (PTR___gmon_start___004132e4 != (undefined *)0x0) {
    (*(code *)PTR___gmon_start___004132e4)();
  }
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/* ---- open @ 00400800 ----
 * (library/support function; see decompiled body)
 */

void open(void)

{
  (*(code *)PTR_open_00413258)();
  return;
}



/* ---- memcmp @ 00400810 ----
 * (library/support function; see decompiled body)
 */

void memcmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040081c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memcmp_0041325c)();
  return;
}



/* ---- write @ 00400820 ----
 * (library/support function; see decompiled body)
 */

void write(void)

{
  (*(code *)PTR_write_00413260)();
  return;
}



/* ---- memset @ 00400830 ----
 * (library/support function; see decompiled body)
 */

void memset(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040083c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memset_00413264)();
  return;
}



/* ---- __libc_start_main @ 00400840 ----
 * (library/support function; see decompiled body)
 */

void __libc_start_main(void)

{
  (*(code *)PTR___libc_start_main_00413268)();
  return;
}



/* ---- read @ 00400850 ----
 * (library/support function; see decompiled body)
 */

void read(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040085c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_read_0041326c)();
  return;
}



/* ---- perror @ 00400860 ----
 * (library/support function; see decompiled body)
 */

void perror(void)

{
  (*(code *)PTR_perror_00413270)();
  return;
}



/* ---- strtol @ 00400870 ----
 * (library/support function; see decompiled body)
 */

void strtol(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040087c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strtol_00413274)();
  return;
}



/* ---- free @ 00400880 ----
 * (library/support function; see decompiled body)
 */

void free(void)

{
  (*(code *)PTR_free_00413278)();
  return;
}



/* ---- ioctl @ 00400890 ----
 * (library/support function; see decompiled body)
 */

void ioctl(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040089c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_ioctl_0041327c)();
  return;
}



/* ---- fseek @ 004008a0 ----
 * (library/support function; see decompiled body)
 */

void fseek(void)

{
  (*(code *)PTR_fseek_00413280)();
  return;
}



/* ---- fclose @ 004008b0 ----
 * (library/support function; see decompiled body)
 */

void fclose(void)

{
                    /* WARNING: Could not recover jumptable at 0x004008bc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fclose_00413284)();
  return;
}



/* ---- ftell @ 004008c0 ----
 * (library/support function; see decompiled body)
 */

void ftell(void)

{
  (*(code *)PTR_ftell_00413288)();
  return;
}



/* ---- printf @ 004008d0 ----
 * (library/support function; see decompiled body)
 */

void printf(void)

{
  (*(code *)PTR_printf_0041328c)();
  return;
}



/* ---- close @ 004008e0 ----
 * (library/support function; see decompiled body)
 */

void close(void)

{
                    /* WARNING: Could not recover jumptable at 0x004008ec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_00413290)();
  return;
}



/* ---- malloc @ 004008f0 ----
 * (library/support function; see decompiled body)
 */

void malloc(void)

{
  (*(code *)PTR_malloc_00413294)();
  return;
}



/* ---- fopen @ 00400900 ----
 * (library/support function; see decompiled body)
 */

void fopen(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040090c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fopen_00413298)();
  return;
}



/* ---- strcat @ 00400910 ----
 * (library/support function; see decompiled body)
 */

void strcat(void)

{
  (*(code *)PTR_strcat_0041329c)();
  return;
}



/* ---- puts @ 00400920 ----
 * (library/support function; see decompiled body)
 */

void puts(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040092c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_puts_004132a0)();
  return;
}



/* ---- strncmp @ 00400930 ----
 * (library/support function; see decompiled body)
 */

void strncmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040093c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strncmp_004132a4)();
  return;
}



/* ---- lseek @ 00400940 ----
 * (library/support function; see decompiled body)
 */

void lseek(void)

{
  (*(code *)PTR_lseek_004132a8)();
  return;
}



/* ---- strcmp @ 00400950 ----
 * (library/support function; see decompiled body)
 */

void strcmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040095c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strcmp_004132ac)();
  return;
}



/* ---- __start @ 00400970 ----
 * (library/support function; see decompiled body)
 */

void processEntry __start(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined1 auStack_20 [16];
  code *local_10;
  undefined4 local_c;
  undefined1 *local_8;
  
  local_8 = auStack_20;
  local_10 = __libc_csu_fini;
  local_c = param_1;
  __libc_start_main(main,param_2,&param_3,__libc_csu_init);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* ---- __do_global_dtors_aux @ 004009c0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400a14) */
/* WARNING: Removing unreachable block (ram,0x00400a18) */
/* WARNING: Removing unreachable block (ram,0x00400a3c) */

void __do_global_dtors_aux(void)

{
  if (completed_5578 == '\0') {
    completed_5578 = '\x01';
  }
  return;
}



/* ---- frame_dummy @ 00400a60 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400a7c) */

void frame_dummy(void)

{
  return;
}



/* ---- endian_change @ 00400a90 ----
 * Byte-swap helper for boot headers.
 */

uint endian_change(uint param_1)

{
  return (param_1 & 0xff00) << 8 | param_1 >> 8 & 0xff00 | param_1 << 0x18 | param_1 >> 0x18;
}



/* ---- print_usage @ 00400ab8 ----
 * Print the subcommand usage.
 */

void print_usage(undefined4 param_1)

{
  puts("Usage options:");
  printf("    %s dump\n",param_1);
  printf("    %s erase mtdx\n",param_1);
  printf("    %s write mtdx <file>\n",param_1);
  printf("    %s read mtdx size <file>\n",param_1);
  return;
}



/* ---- is_bad_block @ 00400b58 ----
 * MEMGETBADBLOCK ioctl - skip factory/worn bad blocks.
 */

void is_bad_block(undefined4 param_1,undefined4 param_2,uint param_3,int param_4)

{
  undefined8 uVar1;
  uint local_18;
  int local_14;
  
  uVar1 = __moddi3(param_3,param_4,blocksize,blocksize >> 0x1f);
  local_18 = param_3 - (int)uVar1;
  local_14 = (param_4 - (int)((ulonglong)uVar1 >> 0x20)) - (uint)(param_3 < local_18);
  ioctl(param_1,0x80084d0b,&local_18);
  return;
}



/* ---- mtd_erase @ 00400be8 ----
 * MEMERASE ioctl loop over the partition, skipping bad blocks.
 */

int mtd_erase(undefined4 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_30;
  int local_2c;
  
  iVar2 = 0;
  if (0 < param_2) {
    local_2c = blocksize;
    local_30 = 0;
    do {
      iVar1 = is_bad_block(param_1);
      if ((iVar1 == 0) && (iVar1 = ioctl(param_1,0x80084d02,&local_30), iVar1 < 0)) {
        printf("erase error %d at 0x%x\n",iVar1,local_30);
        local_30 = blocksize + local_30;
      }
      else {
        local_30 = blocksize + local_30;
      }
      iVar2 = iVar2 + blocksize;
    } while (iVar2 < param_2);
  }
  return iVar2;
}



/* ---- mtd_open @ 00400d08 ----
 * open() the /dev/mtdX char device and MEMGETINFO ioctl to learn its size
 * (global mtdsize) and geometry.
 */

int mtd_open(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  undefined1 auStack_28 [8];
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  iVar1 = open(param_1,0x12);
  if (0 < iVar1) {
    iVar2 = ioctl(iVar1,0x40204d01,auStack_28);
    if (iVar2 == 0) {
      blocksize = local_1c;
      pagesize = local_18;
      mtdsize = local_20;
      ooblen = local_14;
    }
    else {
      iVar1 = -1;
    }
  }
  return iVar1;
}



/* ---- mtd_write_page @ 00400d9c ----
 * Write one NAND page via pwrite/ioctl; verifies, reports failure.
 */

uint mtd_write_page(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = param_2;
  while (iVar1 = is_bad_block(param_1), iVar1 != 0) {
    do {
      uVar3 = uVar3 + pagesize;
      if (blocksize == 0) {
        trap(7);
      }
    } while ((int)uVar3 % blocksize != 0);
    printf("0x%x is a bad block. Jump to 0x%x.\n",param_2,uVar3);
    if (mtdsize <= uVar3) {
      puts("reached the end of this mtd partition");
      return 0xffffffff;
    }
  }
  uVar2 = lseek(param_1,uVar3,0);
  if (uVar2 != uVar3) {
    perror("lseek");
    return 0xffffffff;
  }
  iVar1 = write(param_1,param_3,pagesize);
  if (iVar1 == pagesize) {
    return uVar2;
  }
  perror("write");
  return 0xffffffff;
}



/* ---- write_kernel @ 00400f14 ----
 * Write the kernel partition image.
 */

undefined4 write_kernel(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  code *pcVar5;
  
  iVar1 = open(param_2,0);
  if (iVar1 < 0) {
    perror("open kernel file failed");
    return 0xffffffff;
  }
  pcVar2 = (char *)malloc(pagesize);
  if (pcVar2 == (char *)0x0) {
    pcVar2 = "malloc failed";
    pcVar5 = perror;
  }
  else {
    memset(pcVar2,0xff,pagesize);
    iVar3 = read(iVar1,pcVar2,pagesize);
    if (iVar3 < 1) {
LAB_00401090:
      free(pcVar2);
      close(iVar1);
      return 0;
    }
    iVar4 = 0;
    while (iVar4 = mtd_write_page(param_1,iVar4,pcVar2), -1 < iVar4) {
      if (iVar3 < pagesize) goto LAB_00401090;
      memset(pcVar2,0xff,pagesize);
      iVar3 = read(iVar1,pcVar2,pagesize);
      if (iVar3 < 1) goto LAB_00401090;
      iVar4 = iVar4 + pagesize;
    }
    puts("mtd_write_page failed");
    pcVar5 = free;
  }
  (*pcVar5)(pcVar2);
  close(iVar1);
  return 0xffffffff;
}



/* ---- mtd_read_page @ 00401114 ----
 * Read one NAND page (+ OOB where relevant) via pread/ioctl.
 */

int mtd_read_page(undefined4 param_1,int param_2,undefined4 param_3)

{
  int iVar1;
  int iVar2;
  
  iVar1 = is_bad_block(param_1,param_2,param_2,param_2 >> 0x1f);
  if (iVar1 == 0) {
    iVar1 = lseek(param_1,param_2,0);
    if (iVar1 == param_2) {
      iVar2 = read(param_1,param_3,pagesize);
      if (iVar2 != pagesize) {
        perror("write");
        iVar1 = -3;
      }
    }
    else {
      perror("lseek");
      iVar1 = -2;
    }
  }
  else {
    printf("0x%x is a bad block.\n",param_2);
    iVar1 = -1;
  }
  return iVar1;
}



/* ---- read_data_oob @ 00401204 ----
 * Read a page's out-of-band (spare) bytes.
 */

undefined4 read_data_oob(undefined4 param_1,uint param_2,undefined4 param_3)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  code *pcVar6;
  uint local_38;
  undefined4 local_34;
  int local_30;
  
  iVar1 = open(param_3,0x301,0x1ff);
  if (iVar1 < 0) {
    perror("open file failed");
  }
  else {
    pcVar2 = (char *)malloc(pagesize);
    if (pcVar2 == (char *)0x0) {
      pcVar2 = "data malloc";
      pcVar6 = perror;
    }
    else {
      iVar3 = malloc(ooblen);
      if (iVar3 != 0) {
        uVar5 = 0;
        local_34 = ooblen;
        local_30 = iVar3;
joined_r0x004012bc:
        do {
          if (param_2 <= uVar5) {
LAB_00401324:
            close(iVar1);
            free(pcVar2);
            free(iVar3);
            return 0;
          }
          memset(pcVar2,0xff,pagesize);
          memset(iVar3,0xff,ooblen);
          iVar4 = mtd_read_page(param_1,uVar5,pcVar2);
          if (iVar4 == -1) {
            uVar5 = uVar5 + blocksize;
            goto joined_r0x004012bc;
          }
          write(iVar1,pcVar2,pagesize);
          local_38 = uVar5;
          iVar4 = ioctl(param_1,0xc00c4d04,&local_38);
          if (iVar4 != 0) {
            perror("MEMREADOOB",iVar3);
            goto LAB_00401324;
          }
          write(iVar1,iVar3,ooblen);
          uVar5 = uVar5 + pagesize;
        } while( true );
      }
      perror("oob malloc");
      pcVar6 = free;
    }
    (*pcVar6)(pcVar2);
    close(iVar1);
  }
  return 0xffffffff;
}



/* ---- dump_flash @ 0040146c ----
 * Dump a partition's contents (used by 'dump').
 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void dump_flash(void)

{
  undefined4 uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 *puVar7;
  undefined4 *puVar8;
  int iVar9;
  char *pcVar10;
  undefined4 local_158 [25];
  char local_f4 [4];
  char local_f0 [4];
  char local_ec;
  undefined1 local_eb;
  undefined1 local_ea;
  undefined1 local_e9;
  undefined1 local_e8;
  undefined1 local_e7;
  undefined1 local_e6;
  undefined1 local_e5;
  undefined1 local_e4;
  undefined1 local_e3;
  undefined1 local_e2;
  undefined1 local_e1;
  char local_e0 [4];
  char local_dc [4];
  char local_d8 [4];
  char local_d4;
  undefined1 local_d3;
  undefined1 local_d2;
  undefined1 local_d1;
  undefined1 local_d0;
  undefined1 local_cf;
  undefined1 local_ce;
  undefined1 local_cd;
  char local_cc [4];
  char local_c8 [4];
  char local_c4 [4];
  char local_c0;
  undefined1 local_bf;
  undefined1 local_be;
  undefined1 local_bd;
  undefined1 local_bc;
  undefined1 local_bb;
  undefined1 local_ba;
  undefined1 local_b9;
  char local_b8 [4];
  char local_b4 [4];
  char local_b0 [4];
  char local_ac [2];
  undefined2 local_aa;
  undefined2 local_a8;
  undefined2 local_a6;
  char local_a4 [4];
  char local_a0 [4];
  char local_9c [4];
  char local_98;
  undefined1 local_97;
  undefined1 local_96;
  undefined1 local_95;
  undefined1 local_94;
  undefined1 local_93;
  undefined1 local_92;
  undefined1 local_91;
  char local_90 [4];
  char local_8c [4];
  char local_88 [4];
  char local_84;
  undefined1 local_83;
  undefined1 local_82;
  undefined1 local_81;
  undefined1 local_80;
  undefined1 local_7f;
  undefined1 local_7e;
  undefined1 local_7d;
  char local_7c [4];
  char local_78 [4];
  char local_74 [4];
  char local_70;
  undefined1 local_6f;
  undefined1 local_6e;
  undefined1 local_6d;
  undefined1 local_6c;
  undefined1 local_6b;
  undefined1 local_6a;
  undefined1 local_69;
  char local_68 [4];
  char local_64 [4];
  char local_60 [4];
  char local_5c;
  undefined1 local_5b;
  undefined1 local_5a;
  undefined1 local_59;
  undefined1 local_58;
  undefined1 local_57;
  undefined1 local_56;
  undefined1 local_55;
  undefined4 local_54;
  undefined4 local_50;
  undefined2 local_4c;
  undefined2 local_4a;
  undefined2 local_48;
  undefined2 local_46;
  undefined2 local_44;
  undefined2 local_42;
  char local_40 [4];
  char local_3c [4];
  char local_38 [2];
  undefined2 local_36;
  undefined2 local_34;
  undefined2 local_32;
  undefined2 local_30;
  undefined2 local_2e;
  
  puVar7 = &C_69_3625;
  puVar8 = local_158;
  do {
    uVar1 = *puVar7;
    uVar4 = puVar7[1];
    uVar5 = puVar7[2];
    uVar6 = puVar7[3];
    puVar7 = puVar7 + 4;
    *puVar8 = uVar1;
    puVar8[1] = uVar4;
    puVar8[2] = uVar5;
    puVar8[3] = uVar6;
    puVar8 = puVar8 + 4;
  } while (puVar7 != (undefined4 *)&UNK_004031ec);
  *puVar8 = _UNK_004031ec;
  memset(local_f4,0,200);
  local_f4[0] = s_boot_mem_00403030[0];
  local_f4[1] = s_boot_mem_00403030[1];
  local_f4[2] = s_boot_mem_00403030[2];
  local_f4[3] = s_boot_mem_00403030[3];
  local_f0[0] = s_boot_mem_00403030[4];
  local_f0[1] = s_boot_mem_00403030[5];
  local_f0[2] = s_boot_mem_00403030[6];
  local_f0[3] = s_boot_mem_00403030[7];
  local_ec = s_boot_mem_00403030[8];
  local_e0[0] = s_loader_0_mem_00403044[0];
  local_e0[1] = s_loader_0_mem_00403044[1];
  local_e0[2] = s_loader_0_mem_00403044[2];
  local_e0[3] = s_loader_0_mem_00403044[3];
  local_dc[0] = s_loader_0_mem_00403044[4];
  local_dc[1] = s_loader_0_mem_00403044[5];
  local_dc[2] = s_loader_0_mem_00403044[6];
  local_dc[3] = s_loader_0_mem_00403044[7];
  local_d8[0] = s_loader_0_mem_00403044[8];
  local_d8[1] = s_loader_0_mem_00403044[9];
  local_d8[2] = s_loader_0_mem_00403044[10];
  local_d8[3] = s_loader_0_mem_00403044[0xb];
  local_d4 = s_loader_0_mem_00403044[0xc];
  local_eb = 0;
  local_ea = 0;
  local_e9 = 0;
  local_e8 = 0;
  local_e7 = 0;
  local_e6 = 0;
  local_e5 = 0;
  local_e4 = 0;
  local_e3 = 0;
  local_e2 = 0;
  local_e1 = 0;
  local_d3 = 0;
  local_d2 = 0;
  local_d1 = 0;
  local_d0 = 0;
  local_cf = 0;
  local_ce = 0;
  local_cd = 0;
  local_cc[0] = s_loader_1_mem_00403058[0];
  local_cc[1] = s_loader_1_mem_00403058[1];
  local_cc[2] = s_loader_1_mem_00403058[2];
  local_cc[3] = s_loader_1_mem_00403058[3];
  local_c8[0] = s_loader_1_mem_00403058[4];
  local_c8[1] = s_loader_1_mem_00403058[5];
  local_c8[2] = s_loader_1_mem_00403058[6];
  local_c8[3] = s_loader_1_mem_00403058[7];
  local_c4[0] = s_loader_1_mem_00403058[8];
  local_c4[1] = s_loader_1_mem_00403058[9];
  local_c4[2] = s_loader_1_mem_00403058[10];
  local_c4[3] = s_loader_1_mem_00403058[0xb];
  local_c0 = s_loader_1_mem_00403058[0xc];
  local_b8[0] = s_uboot_env_mem_0040306c[0];
  local_b8[1] = s_uboot_env_mem_0040306c[1];
  local_b8[2] = s_uboot_env_mem_0040306c[2];
  local_b8[3] = s_uboot_env_mem_0040306c[3];
  local_b4[0] = s_uboot_env_mem_0040306c[4];
  local_b4[1] = s_uboot_env_mem_0040306c[5];
  local_b4[2] = s_uboot_env_mem_0040306c[6];
  local_b4[3] = s_uboot_env_mem_0040306c[7];
  local_b0[0] = s_uboot_env_mem_0040306c[8];
  local_b0[1] = s_uboot_env_mem_0040306c[9];
  local_b0[2] = s_uboot_env_mem_0040306c[10];
  local_b0[3] = s_uboot_env_mem_0040306c[0xb];
  local_ac[0] = s_uboot_env_mem_0040306c[0xc];
  local_ac[1] = s_uboot_env_mem_0040306c[0xd];
  local_a4[0] = s_kernel_0_mem_00403080[0];
  local_a4[1] = s_kernel_0_mem_00403080[1];
  local_a4[2] = s_kernel_0_mem_00403080[2];
  local_a4[3] = s_kernel_0_mem_00403080[3];
  local_a0[0] = s_kernel_0_mem_00403080[4];
  local_a0[1] = s_kernel_0_mem_00403080[5];
  local_a0[2] = s_kernel_0_mem_00403080[6];
  local_a0[3] = s_kernel_0_mem_00403080[7];
  local_9c[0] = s_kernel_0_mem_00403080[8];
  local_9c[1] = s_kernel_0_mem_00403080[9];
  local_9c[2] = s_kernel_0_mem_00403080[10];
  local_9c[3] = s_kernel_0_mem_00403080[0xb];
  local_98 = s_kernel_0_mem_00403080[0xc];
  local_bf = 0;
  local_be = 0;
  local_bd = 0;
  local_bc = 0;
  local_bb = 0;
  local_ba = 0;
  local_b9 = 0;
  local_aa = 0;
  local_a8 = 0;
  local_a6 = 0;
  local_97 = 0;
  local_96 = 0;
  local_95 = 0;
  local_94 = 0;
  local_90[0] = s_kernel_1_mem_00403094[0];
  local_90[1] = s_kernel_1_mem_00403094[1];
  local_90[2] = s_kernel_1_mem_00403094[2];
  local_90[3] = s_kernel_1_mem_00403094[3];
  local_8c[0] = s_kernel_1_mem_00403094[4];
  local_8c[1] = s_kernel_1_mem_00403094[5];
  local_8c[2] = s_kernel_1_mem_00403094[6];
  local_8c[3] = s_kernel_1_mem_00403094[7];
  local_88[0] = s_kernel_1_mem_00403094[8];
  local_88[1] = s_kernel_1_mem_00403094[9];
  local_88[2] = s_kernel_1_mem_00403094[10];
  local_88[3] = s_kernel_1_mem_00403094[0xb];
  local_84 = s_kernel_1_mem_00403094[0xc];
  local_7c[0] = s_rootfs_0_mem_004030a8[0];
  local_7c[1] = s_rootfs_0_mem_004030a8[1];
  local_7c[2] = s_rootfs_0_mem_004030a8[2];
  local_7c[3] = s_rootfs_0_mem_004030a8[3];
  local_78[0] = s_rootfs_0_mem_004030a8[4];
  local_78[1] = s_rootfs_0_mem_004030a8[5];
  local_78[2] = s_rootfs_0_mem_004030a8[6];
  local_78[3] = s_rootfs_0_mem_004030a8[7];
  local_74[0] = s_rootfs_0_mem_004030a8[8];
  local_74[1] = s_rootfs_0_mem_004030a8[9];
  local_74[2] = s_rootfs_0_mem_004030a8[10];
  local_74[3] = s_rootfs_0_mem_004030a8[0xb];
  local_70 = s_rootfs_0_mem_004030a8[0xc];
  local_93 = 0;
  local_92 = 0;
  local_91 = 0;
  local_83 = 0;
  local_82 = 0;
  local_81 = 0;
  local_80 = 0;
  local_7f = 0;
  local_7e = 0;
  local_7d = 0;
  local_6f = 0;
  local_6e = 0;
  local_6d = 0;
  local_6c = 0;
  local_6b = 0;
  local_6a = 0;
  local_69 = 0;
  local_68[0] = s_rootfs_1_mem_004030bc[0];
  local_68[1] = s_rootfs_1_mem_004030bc[1];
  local_68[2] = s_rootfs_1_mem_004030bc[2];
  local_68[3] = s_rootfs_1_mem_004030bc[3];
  local_54 = DAT_004030d0;
  local_64[0] = s_rootfs_1_mem_004030bc[4];
  local_64[1] = s_rootfs_1_mem_004030bc[5];
  local_64[2] = s_rootfs_1_mem_004030bc[6];
  local_64[3] = s_rootfs_1_mem_004030bc[7];
  local_60[0] = s_rootfs_1_mem_004030bc[8];
  local_60[1] = s_rootfs_1_mem_004030bc[9];
  local_60[2] = s_rootfs_1_mem_004030bc[10];
  local_60[3] = s_rootfs_1_mem_004030bc[0xb];
  local_5c = s_rootfs_1_mem_004030bc[0xc];
  local_50 = DAT_004030d4;
  local_4c = DAT_004030d8;
  local_40[0] = s_app_1_mem_004030e4[0];
  local_40[1] = s_app_1_mem_004030e4[1];
  local_40[2] = s_app_1_mem_004030e4[2];
  local_40[3] = s_app_1_mem_004030e4[3];
  local_3c[0] = s_app_1_mem_004030e4[4];
  local_3c[1] = s_app_1_mem_004030e4[5];
  local_3c[2] = s_app_1_mem_004030e4[6];
  local_3c[3] = s_app_1_mem_004030e4[7];
  local_38[0] = s_app_1_mem_004030e4[8];
  local_38[1] = s_app_1_mem_004030e4[9];
  local_5b = 0;
  local_5a = 0;
  local_59 = 0;
  local_58 = 0;
  local_57 = 0;
  local_56 = 0;
  local_55 = 0;
  local_4a = 0;
  local_48 = 0;
  local_46 = 0;
  local_44 = 0;
  local_42 = 0;
  local_36 = 0;
  local_34 = 0;
  local_32 = 0;
  local_30 = 0;
  local_2e = 0;
  iVar9 = 0;
  pcVar10 = local_f4;
  do {
    while( true ) {
      iVar3 = (int)local_158 + iVar9;
      printf("Dumping %s .....\n",pcVar10);
      iVar2 = mtd_open(iVar3);
      iVar9 = iVar9 + 10;
      if (iVar2 < 0) {
        printf("fail to open %s: error %d\n",iVar3,iVar2);
        return;
      }
      iVar3 = read_data_oob(iVar2,mtdsize,pcVar10);
      if (iVar3 < 0) break;
      close(iVar2);
      pcVar10 = pcVar10 + 0x14;
      if (iVar9 == 100) {
        return;
      }
    }
    puts("Dumping boot partition failed.");
    close(iVar2);
    pcVar10 = pcVar10 + 0x14;
  } while (iVar9 != 100);
  return;
}



/* ---- read_nandboot @ 0040189c ----
 * Read the NAND boot block.
 */

undefined4 read_nandboot(undefined4 param_1,undefined4 param_2,uint param_3)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  iVar1 = open(param_2,0x301,0x1ff);
  if (iVar1 < 0) {
    perror("open fd file failed");
    uVar3 = 0xffffffff;
  }
  else {
    iVar2 = malloc(pagesize);
    if (iVar2 == 0) {
      perror("malloc");
      close(iVar1);
      uVar3 = 0xfffffffe;
    }
    else {
      if (param_3 != 0) {
        uVar4 = 0;
        do {
          mtd_read_page(param_1,uVar4,iVar2);
          write(iVar1,iVar2,pagesize);
          uVar4 = uVar4 + pagesize;
        } while (uVar4 < param_3);
      }
      free(iVar2);
      close(iVar1);
      uVar3 = 0;
    }
  }
  return uVar3;
}



/* ---- read_bootrom @ 004019e0 ----
 * Read the boot ROM/partition image.
 */

int read_bootrom(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  uint *puVar2;
  int iVar3;
  uint local_28 [2];
  
  local_28[0] = 0;
  iVar3 = 0;
  if (0 < param_3) {
    iVar3 = 0;
    do {
      iVar1 = read(param_1,local_28,4);
      puVar2 = (uint *)(param_2 + iVar3);
      local_28[0] = (local_28[0] & 0xff00) << 8 | (int)(local_28[0] & 0xff0000) >> 8 |
                    local_28[0] << 0x18 | local_28[0] >> 0x18;
      iVar3 = iVar3 + iVar1;
      *puVar2 = local_28[0];
      if (iVar1 < 4) {
        return iVar3;
      }
    } while (iVar3 < param_3);
  }
  return iVar3;
}



/* ---- compare_boot @ 00401ac4 ----
 * Compare a written boot image against the source (verify).
 */

undefined4 compare_boot(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  
  iVar1 = open(param_1,0);
  if (iVar1 < 0) {
    perror("open boot file failed");
    uVar7 = 0xffffffff;
  }
  else {
    iVar2 = malloc(pagesize);
    if (iVar2 == 0) {
      perror("malloc srcbuf failed");
      close(iVar1);
      uVar7 = 0xffffffff;
    }
    else {
      iVar3 = malloc(pagesize);
      if (iVar3 == 0) {
        perror("malloc mtdbuf failed");
        close(iVar1);
        free(iVar2);
        uVar7 = 0xffffffff;
      }
      else {
        read(iVar1,iVar2,3);
        iVar6 = 0;
        iVar4 = pagesize;
        do {
          iVar4 = read_bootrom(iVar1,iVar2,iVar4);
          mtd_read_page(param_2,iVar6,iVar3);
          iVar5 = memcmp(iVar2,iVar3,iVar4);
          if (iVar5 != 0) {
            printf("page addr 0x%x: data doesn\'t match\n",iVar6);
            uVar7 = 1;
            goto LAB_00401bb0;
          }
          iVar6 = iVar6 + iVar4;
        } while (iVar4 == pagesize);
        uVar7 = 0;
LAB_00401bb0:
        free(iVar2);
        free(iVar3);
        close(iVar1);
      }
    }
  }
  return uVar7;
}



/* ---- write_boot @ 00401cb4 ----
 * Write the boot partition image (bad-block aware).
 */

undefined4 write_boot(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  
  iVar1 = open(param_2,0);
  if (iVar1 < 0) {
    perror("open boot file failed");
    uVar4 = 0xffffffff;
  }
  else {
    iVar2 = malloc(pagesize);
    if (iVar2 == 0) {
      perror("malloc failed");
      close(iVar1);
      uVar4 = 0xffffffff;
    }
    else {
      read(iVar1,iVar2,3);
      memset(iVar2,0xff,pagesize);
      iVar5 = 0;
      do {
        iVar3 = read_bootrom(iVar1,iVar2,pagesize);
        if (iVar3 != 0) {
          mtd_write_page(param_1,iVar5,iVar2);
          iVar5 = iVar5 + pagesize;
        }
        memset(iVar2,0xff,pagesize);
      } while (iVar5 != 0x20000);
      iVar5 = 1;
      do {
        lseek(iVar1,3,0);
        iVar3 = 0;
        do {
          read_bootrom(iVar1,iVar2,pagesize);
          mtd_write_page(param_1,iVar5 * 0x20000 + iVar3,iVar2);
          iVar3 = iVar3 + pagesize;
        } while (iVar3 < 0x20000);
        iVar5 = iVar5 + 1;
      } while (iVar5 != 4);
      free(iVar2);
      close(iVar1);
      uVar4 = 0;
    }
  }
  return uVar4;
}



/* ---- fgetsize @ 00401eac ----
 * Return a file's size (for the 'write' source file).
 */

undefined4 fgetsize(undefined4 param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = fopen(param_1,&DAT_00402e78);
  if (iVar1 == 0) {
    perror("fopen");
    uVar2 = 0xffffffff;
  }
  else {
    fseek(iVar1,0,2);
    uVar2 = ftell(iVar1);
    fclose(iVar1);
  }
  return uVar2;
}



/* ---- main @ 00401f48 ----
 * CLI dispatcher. Subcommands: 'dump', 'erase mtdx', 'write mtdx <file>',
 * 'read mtdx size <file>'. Builds the /dev/mtdX path, opens it (mtd_open),
 * and performs the erase/read/write via the mtd_* helpers. Used for low-level
 * NAND flashing during upgrade/recovery.
 */

undefined4 main(int param_1,undefined4 *param_2)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  undefined4 local_28;
  undefined2 local_24;
  undefined2 local_22;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  
  local_28 = DAT_004030f8;
  local_24 = DAT_004030fc;
  local_22 = 0;
  local_20 = 0;
  local_1e = 0;
  local_1c = 0;
  local_1a = 0;
  if (param_1 == 3) {
    uVar3 = param_2[1];
    iVar1 = strcmp(uVar3,"erase");
    if (iVar1 != 0) {
      iVar1 = strcmp(uVar3,"write");
      if ((iVar1 == 0) || (iVar1 = strcmp(uVar3,&DAT_00402eb8), iVar1 == 0)) {
        printf("filename is missing...");
        print_usage(*param_2);
        return 0xffffffff;
      }
LAB_00401fb4:
      print_usage(*param_2);
      return 0xffffffff;
    }
    iVar2 = param_2[2];
    iVar1 = strncmp(iVar2,&DAT_00402e9c,3);
    uVar3 = 7;
    if (iVar1 != 0) goto LAB_00401fb4;
    uVar4 = 0;
  }
  else if (param_1 < 4) {
    if ((param_1 != 2) || (iVar1 = strcmp(param_2[1],&DAT_00402e84), iVar1 != 0))
    goto LAB_00401fb4;
    puts(&DAT_00402e8c);
    uVar3 = 0xc;
    uVar4 = 0;
    iVar2 = param_2[2];
  }
  else if (param_1 == 4) {
    iVar1 = strcmp(param_2[1],"write");
    if (iVar1 != 0) goto LAB_00401fb4;
    iVar2 = param_2[2];
    iVar1 = strncmp(iVar2,&DAT_00402ec0,4);
    uVar3 = 9;
    if (iVar1 == 0) {
      uVar4 = 0;
    }
    else {
      iVar1 = strncmp(iVar2,&DAT_00402e9c,3);
      if (iVar1 != 0) {
        return 0xffffffff;
      }
      uVar3 = 8;
      uVar4 = 0;
    }
  }
  else {
    if ((param_1 != 5) || (iVar1 = strcmp(param_2[1],&DAT_00402eb8), iVar1 != 0))
    goto LAB_00401fb4;
    iVar2 = param_2[2];
    iVar1 = strncmp(iVar2,&DAT_00402ec0,4);
    uVar3 = 0xb;
    if (iVar1 == 0) {
      uVar4 = 0;
    }
    else {
      iVar1 = strncmp(iVar2,&DAT_00402e9c,3);
      if (iVar1 != 0) goto LAB_00401fb4;
      uVar4 = strtol(param_2[3],0,0);
      uVar3 = 10;
      iVar2 = param_2[2];
    }
  }
  if (iVar2 != 0) {
    strcat(&local_28,iVar2);
    printf("MTD device %s\n",&local_28);
  }
  iVar1 = 0;
  switch(uVar3) {
  case 7:
    iVar1 = mtd_open(&local_28);
    if (iVar1 < 0) {
LAB_0040257c:
      printf("fail to open %s: error %d\n",&local_28,iVar1);
      return 0xffffffff;
    }
    iVar2 = mtd_erase(iVar1,mtdsize);
    if (iVar2 < 0) {
      puts("operation failed");
    }
    goto switchD_00402094_default;
  case 8:
    puts("Write Other block");
    iVar1 = mtd_open(&local_28);
    if (iVar1 < 0) goto LAB_0040257c;
    uVar4 = fgetsize(param_2[3]);
    if ((uVar4 == 0xffffffff) || (mtdsize < uVar4)) {
      printf("Wrong file size: %d, part. size %d\n",uVar4,mtdsize);
      return 0xffffffff;
    }
    printf("write file size: %d\n");
    iVar2 = mtd_erase(iVar1,mtdsize);
    if (iVar2 < 0) {
      puts("failed to write from erase failure");
      uVar3 = param_2[3];
    }
    else {
      uVar3 = param_2[3];
    }
    iVar2 = write_kernel(iVar1,uVar3);
    if (iVar2 < 0) {
      puts("operation failed");
    }
    break;
  case 9:
    puts("Write Boot block");
    iVar1 = mtd_open(&local_28);
    if (iVar1 < 0) goto LAB_0040257c;
    uVar4 = fgetsize(param_2[3]);
    printf("boot file size: %d\n",uVar4);
    if ((uVar4 == 0xffffffff) || (mtdsize < uVar4)) {
      printf("Wrong file size: %d\n",uVar4);
      return 0xffffffff;
    }
    iVar2 = mtd_erase(iVar1,0x80000);
    if (iVar2 < 0) {
      puts("failed to write from erase failure");
      uVar3 = param_2[3];
    }
    else {
      uVar3 = param_2[3];
    }
    iVar2 = write_boot(iVar1,uVar3);
    if (iVar2 < 0) {
      puts("operation failed");
      uVar3 = param_2[3];
    }
    else {
      uVar3 = param_2[3];
    }
    iVar2 = compare_boot(uVar3,iVar1);
    if (iVar2 != 0) {
      puts("Update failed. Please try again");
    }
    break;
  case 10:
    puts("Read Other block");
    iVar1 = mtd_open(&local_28);
    if (iVar1 < 0) goto LAB_0040257c;
    if (uVar4 == 0) {
      uVar4 = mtdsize;
    }
    printf("Read size %x, save to file %s\n",uVar4,param_2[4]);
    iVar2 = read_data_oob(iVar1,uVar4,param_2[4]);
    goto joined_r0x00402254;
  case 0xb:
    puts("Read boot block");
    iVar1 = mtd_open(&local_28);
    if (iVar1 < 0) goto LAB_0040257c;
    iVar2 = read_nandboot(iVar1,param_2[4],0x80000);
joined_r0x00402254:
    if (iVar2 < 0) {
      puts("operation failed");
    }
    goto switchD_00402094_default;
  case 0xc:
    puts("Dump Nand flash to file");
    dump_flash();
    return 0;
  default:
    goto switchD_00402094_default;
  }
  puts("Update success");
switchD_00402094_default:
  close(iVar1);
  return 0;
}



/* ---- __moddi3 @ 004025e0 ----
 * (library/support function; see decompiled body)
 */

undefined8 __moddi3(uint param_1,uint param_2,uint param_3,uint param_4)

{
  longlong lVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  int iVar13;
  uint local_10;
  uint local_c;
  
  iVar13 = 0;
  local_c = param_2;
  local_10 = param_1;
  if ((int)param_2 < 0) {
    local_10 = -param_1;
    local_c = -(uint)(local_10 != 0) - param_2;
    iVar13 = -1;
  }
  if ((int)param_4 < 0) {
    param_3 = -param_3;
    param_4 = -(uint)(param_3 != 0) - param_4;
  }
  if (param_4 == 0) {
    uVar10 = LZCOUNT(param_3);
    if (local_c < param_3) {
      if (uVar10 != 0) {
        local_c = local_10 >> (-uVar10 & 0x1f) | local_c << (uVar10 & 0x1f);
        param_3 = param_3 << (uVar10 & 0x1f);
        local_10 = local_10 << (uVar10 & 0x1f);
      }
      uVar6 = param_3 >> 0x10;
      if (uVar6 == 0) {
        trap(7);
      }
      uVar5 = local_c % uVar6 << 0x10 | local_10 >> 0x10;
      uVar11 = (local_c / uVar6) * (param_3 & 0xffff);
      if (uVar6 == 0) {
        trap(7);
      }
      uVar8 = uVar5 - uVar11;
      if (uVar5 < uVar11) {
        uVar5 = uVar5 + param_3;
        uVar8 = uVar5 - uVar11;
        if (param_3 <= uVar5) {
          if (uVar5 < uVar11) {
            uVar5 = uVar5 + param_3;
          }
          uVar8 = uVar5 - uVar11;
        }
      }
      if (uVar6 == 0) {
        trap(7);
      }
      uVar5 = uVar8 % uVar6 << 0x10 | local_10 & 0xffff;
      uVar11 = (uVar8 / uVar6) * (param_3 & 0xffff);
      if (uVar6 == 0) {
        trap(7);
      }
      local_10 = uVar5 - uVar11;
      if (uVar5 < uVar11) {
        uVar5 = uVar5 + param_3;
        goto LAB_004026f8;
      }
    }
    else {
      if (param_3 == 0) {
        param_3 = 1 / 0;
        trap(7);
        uVar10 = LZCOUNT(param_3);
      }
      if (uVar10 == 0) {
        local_c = local_c - param_3;
        uVar8 = param_3 >> 0x10;
        uVar11 = param_3 & 0xffff;
      }
      else {
        param_3 = param_3 << (uVar10 & 0x1f);
        uVar6 = local_c >> (0x20 - uVar10 & 0x1f);
        uVar8 = param_3 >> 0x10;
        if (uVar8 == 0) {
          trap(7);
        }
        uVar11 = param_3 & 0xffff;
        uVar3 = local_10 >> (0x20 - uVar10 & 0x1f) | local_c << (uVar10 & 0x1f);
        uVar5 = (uVar6 / uVar8) * uVar11;
        uVar6 = uVar6 % uVar8 << 0x10 | uVar3 >> 0x10;
        if (uVar8 == 0) {
          trap(7);
        }
        uVar4 = uVar6 - uVar5;
        if (uVar6 < uVar5) {
          uVar6 = uVar6 + param_3;
          if (param_3 <= uVar6) {
            if (uVar6 < uVar5) {
              uVar6 = uVar6 + param_3;
            }
          }
          uVar4 = uVar6 - uVar5;
        }
        if (uVar8 == 0) {
          trap(7);
        }
        local_c = uVar4 % uVar8 << 0x10 | uVar3 & 0xffff;
        uVar6 = (uVar4 / uVar8) * uVar11;
        if (uVar8 == 0) {
          trap(7);
        }
        if (local_c < uVar6) {
          local_c = local_c + param_3;
          if (param_3 <= local_c) {
            if (local_c < uVar6) {
              local_c = local_c + param_3;
            }
          }
        }
        local_c = local_c - uVar6;
        local_10 = local_10 << (uVar10 & 0x1f);
      }
      if (uVar8 == 0) {
        trap(7);
      }
      uVar5 = local_c % uVar8 << 0x10 | local_10 >> 0x10;
      uVar6 = (local_c / uVar8) * uVar11;
      if (uVar8 == 0) {
        trap(7);
      }
      uVar3 = uVar5 - uVar6;
      if (uVar5 < uVar6) {
        uVar5 = uVar5 + param_3;
        uVar3 = uVar5 - uVar6;
        if (param_3 <= uVar5) {
          if (uVar5 < uVar6) {
            uVar5 = uVar5 + param_3;
          }
          uVar3 = uVar5 - uVar6;
        }
      }
      if (uVar8 == 0) {
        trap(7);
      }
      uVar5 = uVar3 % uVar8 << 0x10 | local_10 & 0xffff;
      uVar11 = (uVar3 / uVar8) * uVar11;
      if (uVar8 == 0) {
        trap(7);
      }
      local_10 = uVar5 - uVar11;
      if (uVar5 < uVar11) {
        uVar5 = uVar5 + param_3;
LAB_004026f8:
        local_10 = uVar5 - uVar11;
        if (param_3 <= uVar5) {
          if (uVar5 < uVar11) {
            uVar5 = uVar5 + param_3;
          }
          local_10 = uVar5 - uVar11;
        }
      }
    }
    local_10 = local_10 >> (uVar10 & 0x1f);
    local_c = 0;
  }
  else {
    uVar10 = LZCOUNT(param_4);
    if (local_c < param_4) {
      if (iVar13 == 0) goto LAB_0040276c;
      goto LAB_00402728;
    }
    if (uVar10 == 0) {
      if ((param_4 < local_c) || (param_3 <= local_10)) {
        local_c = (local_c - param_4) - (uint)(local_10 < local_10 - param_3);
        local_10 = local_10 - param_3;
      }
      goto LAB_00402720;
    }
    uVar8 = 0x20 - uVar10;
    uVar5 = param_3 >> (uVar8 & 0x1f) | param_4 << (uVar10 & 0x1f);
    uVar6 = local_c >> (uVar8 & 0x1f);
    uVar11 = uVar5 >> 0x10;
    uVar3 = uVar6 / uVar11;
    if (uVar11 == 0) {
      trap(7);
    }
    uVar12 = local_10 >> (uVar8 & 0x1f) | local_c << (uVar10 & 0x1f);
    param_3 = param_3 << (uVar10 & 0x1f);
    uVar7 = uVar6 % uVar11 << 0x10 | uVar12 >> 0x10;
    uVar6 = uVar3 * (uVar5 & 0xffff);
    local_10 = local_10 << (uVar10 & 0x1f);
    uVar4 = uVar3;
    if (uVar7 < uVar6) {
      uVar7 = uVar7 + uVar5;
      uVar4 = uVar3 - 1;
      if (uVar7 < uVar5) goto LAB_004029e0;
      uVar2 = uVar7 - uVar6;
      if (uVar7 < uVar6) {
        uVar4 = uVar3 - 2;
        uVar7 = uVar7 + uVar5;
        goto LAB_004029e0;
      }
    }
    else {
LAB_004029e0:
      uVar2 = uVar7 - uVar6;
    }
    uVar6 = uVar2 / uVar11;
    if (uVar11 == 0) {
      trap(7);
    }
    uVar7 = uVar2 % uVar11 << 0x10 | uVar12 & 0xffff;
    uVar11 = uVar6 * (uVar5 & 0xffff);
    uVar3 = uVar6;
    if (uVar7 < uVar11) {
      uVar7 = uVar7 + uVar5;
      uVar3 = uVar6 - 1;
      if ((uVar5 <= uVar7) && (uVar7 < uVar11)) {
        uVar3 = uVar6 - 2;
        uVar7 = uVar7 + uVar5;
      }
    }
    uVar7 = uVar7 - uVar11;
    lVar1 = (ulonglong)(uVar3 | uVar4 << 0x10) * (ulonglong)param_3;
    uVar11 = (uint)lVar1;
    uVar6 = (uint)((ulonglong)lVar1 >> 0x20);
    if ((uVar7 < uVar6) ||
       ((iVar9 = uVar7 - uVar6, uVar7 == uVar6 && (iVar9 = 0, local_10 < uVar11)))) {
      param_3 = uVar11 - param_3;
      iVar9 = uVar7 - ((uVar6 - uVar5) - (uint)(uVar11 < param_3));
      uVar11 = param_3;
    }
    local_c = iVar9 - (uint)(local_10 < local_10 - uVar11);
    local_10 = local_10 - uVar11 >> (uVar10 & 0x1f) | local_c << (uVar8 & 0x1f);
    local_c = local_c >> (uVar10 & 0x1f);
  }
LAB_00402720:
  if (iVar13 == 0) {
LAB_0040276c:
    return CONCAT44(local_c,local_10);
  }
LAB_00402728:
  return CONCAT44(-(uint)(-local_10 != 0) - local_c,-local_10);
}



/* ---- __libc_csu_fini @ 00402ad0 ----
 * (library/support function; see decompiled body)
 */

void __libc_csu_fini(void)

{
  return;
}



/* ---- __libc_csu_init @ 00402ad8 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00402b38) */
/* WARNING: Removing unreachable block (ram,0x00402b3c) */

void __libc_csu_init(void)

{
  (*(code *)PTR__init_004132e0)();
  return;
}



/* ---- __do_global_ctors_aux @ 00402b90 ----
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



/* ---- _fini @ 00402bf0 ----
 * (library/support function; see decompiled body)
 */

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


