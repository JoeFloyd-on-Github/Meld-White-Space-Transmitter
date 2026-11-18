/*
 * pm_daemon.annotated.c  --  annotated Ghidra decompilation of root/bin/pm_daemon
 * ============================================================================
 *
 * pm_daemon is the MT300's POWER / RESET BUTTON + IR-REMOTE daemon, started
 * from /etc/init.d/rcS as "/bin/pm_daemon &". It is the software that notices
 * the front-panel button and the (optional) IR remote and asks the kernel to
 * change power state. See docs/boot-process.md sections 2-3.
 *
 * WHAT IT DOES (shipped configuration, no -f option):
 *   - opens /dev/micro  (the box microcontroller char device) and
 *     /sys/power/state (kernel power-state control, global `sysfd`)
 *   - sits in event_loop() -> select() on /dev/micro
 *   - on a button event: micro_evt_handler reads one "reason" byte and calls
 *     mode_change(), which sync()s and drives the power state
 *
 * WITH an IR config (-f <file>, NOT used on the MT300): also opens /dev/irc
 * (RX) and /dev/irc1 (TX); ir_rx_evt_handler decodes NEC/RC6 frames and, on the
 * "POWER" symbol, calls mode_change(). A large IR-remote library (irr_*, nec_*,
 * rc6_*) and a key/value config parser (kv_*) support this path.
 *
 * ABOUT THIS FILE:
 *   - This is the raw Ghidra 12.2 decompile with (a) the libc PLT stubs renamed
 *     from Ghidra's FUN_<addr> to their real names (resolved from the ELF
 *     .rel.plt), (b) the referenced .rodata constants substituted in, and
 *     (c) a purpose comment added above every function. The binary ships with
 *     DWARF, so the function NAMES are original symbols, not guesses; locals
 *     keep Ghidra's auto-names (iVar/uVar/local_/auStack_ - see
 *     WhiteSpaceApp/ghidra/README.md for the legend).
 *   - A few helpers still show as FUN_<addr> where they are internal (non-PLT)
 *     routines; their bodies are present in this file.
 *   - Nothing here was produced by running the binary.
 *
 * libc PLT map applied (FUN_<addr> -> name):
 *   open pwrite isalpha setbuf memcmp getchar system strncpy putchar write
 *   fgets memset tcgetattr read perror cfmakeraw free fflush pread tcflush
 *   ioctl fclose getopt memcpy strtoul strlen strcpy printf atoi select close
 *   malloc strtok fopen puts tcsetattr strncmp index tolower strcmp strcasestr
 *
 * Device files:  /dev/micro (power button), /sys/power/state (power control),
 *                /dev/irc + /dev/irc1 (IR RX/TX, unused on MT300).
 */


/* ---- _init @ 00400b14 ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  if (PTR___gmon_start___00415c7c != (undefined *)0x0) {
    (*(code *)PTR___gmon_start___00415c7c)();
  }
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/* ---- open @ 00400ba0 ----
 * (library/support function; see decompiled body)
 */

void open(void)

{
  (*(code *)PTR_open_00415748)();
  return;
}



/* ---- pwrite @ 00400bb0 ----
 * (library/support function; see decompiled body)
 */

void pwrite(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400bbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_pwrite_0041574c)();
  return;
}



/* ---- isalpha @ 00400bc0 ----
 * (library/support function; see decompiled body)
 */

void isalpha(void)

{
  (*(code *)PTR_isalpha_00415750)();
  return;
}



/* ---- setbuf @ 00400bd0 ----
 * (library/support function; see decompiled body)
 */

void setbuf(void)

{
  (*(code *)PTR_setbuf_00415754)();
  return;
}



/* ---- memcmp @ 00400be0 ----
 * (library/support function; see decompiled body)
 */

void memcmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400bec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memcmp_00415758)();
  return;
}



/* ---- getchar @ 00400bf0 ----
 * (library/support function; see decompiled body)
 */

void getchar(void)

{
  (*(code *)PTR_getchar_0041575c)();
  return;
}



/* ---- system @ 00400c00 ----
 * (library/support function; see decompiled body)
 */

void system(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c0c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_system_00415760)();
  return;
}



/* ---- strncpy @ 00400c10 ----
 * (library/support function; see decompiled body)
 */

void strncpy(void)

{
  (*(code *)PTR_strncpy_00415764)();
  return;
}



/* ---- putchar @ 00400c20 ----
 * (library/support function; see decompiled body)
 */

void putchar(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_putchar_00415768)();
  return;
}



/* ---- write @ 00400c30 ----
 * (library/support function; see decompiled body)
 */

void write(void)

{
  (*(code *)PTR_write_0041576c)();
  return;
}



/* ---- fgets @ 00400c40 ----
 * (library/support function; see decompiled body)
 */

void fgets(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fgets_00415770)();
  return;
}



/* ---- memset @ 00400c50 ----
 * (library/support function; see decompiled body)
 */

void memset(void)

{
  (*(code *)PTR_memset_00415774)();
  return;
}



/* ---- FUN_00400c60 @ 00400c60 ----
 * (library/support function; see decompiled body)
 */

void FUN_00400c60(void)

{
  (*(code *)PTR___libc_start_main_00415778)();
  return;
}



/* ---- tcgetattr @ 00400c70 ----
 * (library/support function; see decompiled body)
 */

void tcgetattr(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c7c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_tcgetattr_0041577c)();
  return;
}



/* ---- read @ 00400c80 ----
 * (library/support function; see decompiled body)
 */

void read(void)

{
  (*(code *)PTR_read_00415780)();
  return;
}



/* ---- perror @ 00400c90 ----
 * (library/support function; see decompiled body)
 */

void perror(void)

{
  (*(code *)PTR_perror_00415784)();
  return;
}



/* ---- cfmakeraw @ 00400ca0 ----
 * (library/support function; see decompiled body)
 */

void cfmakeraw(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400cac. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_cfmakeraw_00415788)();
  return;
}



/* ---- free @ 00400cb0 ----
 * (library/support function; see decompiled body)
 */

void free(void)

{
  (*(code *)PTR_free_0041578c)();
  return;
}



/* ---- fflush @ 00400cc0 ----
 * (library/support function; see decompiled body)
 */

void fflush(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400ccc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fflush_00415790)();
  return;
}



/* ---- pread @ 00400cd0 ----
 * (library/support function; see decompiled body)
 */

void pread(void)

{
  (*(code *)PTR_pread_00415794)();
  return;
}



/* ---- tcflush @ 00400ce0 ----
 * (library/support function; see decompiled body)
 */

void tcflush(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400cec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_tcflush_00415798)();
  return;
}



/* ---- ioctl @ 00400cf0 ----
 * (library/support function; see decompiled body)
 */

void ioctl(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400cfc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_ioctl_0041579c)();
  return;
}



/* ---- fclose @ 00400d00 ----
 * (library/support function; see decompiled body)
 */

void fclose(void)

{
  (*(code *)PTR_fclose_004157a0)();
  return;
}



/* ---- getopt @ 00400d10 ----
 * (library/support function; see decompiled body)
 */

void getopt(void)

{
  (*(code *)PTR_getopt_004157a4)();
  return;
}



/* ---- memcpy @ 00400d20 ----
 * (library/support function; see decompiled body)
 */

void memcpy(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400d2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_memcpy_004157a8)();
  return;
}



/* ---- strtoul @ 00400d30 ----
 * (library/support function; see decompiled body)
 */

void strtoul(void)

{
  (*(code *)PTR_strtoul_004157ac)();
  return;
}



/* ---- strlen @ 00400d40 ----
 * (library/support function; see decompiled body)
 */

void strlen(void)

{
  (*(code *)PTR_strlen_004157b0)();
  return;
}



/* ---- strcpy @ 00400d50 ----
 * (library/support function; see decompiled body)
 */

void strcpy(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400d5c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strcpy_004157b4)();
  return;
}



/* ---- printf @ 00400d60 ----
 * (library/support function; see decompiled body)
 */

void printf(void)

{
  (*(code *)PTR_printf_004157b8)();
  return;
}



/* ---- atoi @ 00400d70 ----
 * (library/support function; see decompiled body)
 */

void atoi(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400d7c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_atoi_004157bc)();
  return;
}



/* ---- select @ 00400d80 ----
 * (library/support function; see decompiled body)
 */

void select(void)

{
  (*(code *)PTR_select_004157c0)();
  return;
}



/* ---- close @ 00400d90 ----
 * (library/support function; see decompiled body)
 */

void close(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400d9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_004157c4)();
  return;
}



/* ---- malloc @ 00400da0 ----
 * (library/support function; see decompiled body)
 */

void malloc(void)

{
  (*(code *)PTR_malloc_004157c8)();
  return;
}



/* ---- strtok @ 00400db0 ----
 * (library/support function; see decompiled body)
 */

void strtok(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400dbc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strtok_004157cc)();
  return;
}



/* ---- fopen @ 00400dc0 ----
 * (library/support function; see decompiled body)
 */

void fopen(void)

{
  (*(code *)PTR_fopen_004157d0)();
  return;
}



/* ---- puts @ 00400dd0 ----
 * (library/support function; see decompiled body)
 */

void puts(void)

{
  (*(code *)PTR_puts_004157d4)();
  return;
}



/* ---- tcsetattr @ 00400de0 ----
 * (library/support function; see decompiled body)
 */

void tcsetattr(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400dec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_tcsetattr_004157d8)();
  return;
}



/* ---- strncmp @ 00400df0 ----
 * (library/support function; see decompiled body)
 */

void strncmp(void)

{
  (*(code *)PTR_strncmp_004157dc)();
  return;
}



/* ---- index @ 00400e00 ----
 * (library/support function; see decompiled body)
 */

void index(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400e0c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_index_004157e0)();
  return;
}



/* ---- tolower @ 00400e10 ----
 * (library/support function; see decompiled body)
 */

void tolower(void)

{
  (*(code *)PTR_tolower_004157e4)();
  return;
}



/* ---- strcmp @ 00400e20 ----
 * (library/support function; see decompiled body)
 */

void strcmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400e2c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strcmp_004157e8)();
  return;
}



/* ---- strcasestr @ 00400e30 ----
 * (library/support function; see decompiled body)
 */

void strcasestr(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400e3c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strcasestr_004157ec)();
  return;
}



/* ---- __start @ 00400e50 ----
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
  FUN_00400c60(main,param_2,&param_3,__libc_csu_init);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}



/* ---- __do_global_dtors_aux @ 00400ea0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400ef4) */
/* WARNING: Removing unreachable block (ram,0x00400ef8) */
/* WARNING: Removing unreachable block (ram,0x00400f1c) */

void __do_global_dtors_aux(void)

{
  if (completed_5578 == '\0') {
    completed_5578 = '\x01';
  }
  return;
}



/* ---- frame_dummy @ 00400f40 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400f5c) */

void frame_dummy(void)

{
  return;
}



/* ---- getch @ 00400f70 ----
 * conio-style single-char read: put the terminal in raw mode (tcgetattr/
 * cfmakeraw-ish via a copied termios with ICANON/ECHO cleared, tcsetattr),
 * getchar(), then restore. Interactive-mode helper.
 */

undefined4 getch(void)

{
  undefined4 uVar1;
  undefined1 auStack_74 [52];
  undefined1 auStack_40 [12];
  uint local_34;
  
  tcflush(0,0);
  tcgetattr(0,auStack_74);
  memcpy(auStack_40,auStack_74,0x34);
  local_34 = local_34 & 0xfffffff5;
  tcsetattr(0,0x540e,auStack_40);
  uVar1 = getchar();
  tcsetattr(0,0x540e,auStack_74);
  return uVar1;
}



/* ---- user @ 00401058 ----
 * Small helper (see body).
 */

int user(void)

{
  char cVar1;
  
  printf("Change mode to on/active/passive standby/off? (o/a/p/x)");
  cVar1 = getch();
  return (int)cVar1;
}



/* ---- mode_change @ 004010a4 ----
 * Act on a power/standby request. Calls system("sync") to flush filesystems,
 * then pwrite()s the mode byte 'x' (0x78 = off) to the fd. (Ghidra pruned the
 * unreachable interactive branches; the full routine also drives
 * /sys/power/state via the global `sysfd` and, interactively, prompts
 * "Change mode to on/active/passive standby/off? (o/a/p/x)".)
 */

/* WARNING: Removing unreachable block (ram,0x004011bc) */
/* WARNING: Removing unreachable block (ram,0x004012d0) */
/* WARNING: Removing unreachable block (ram,0x004010f8) */
/* WARNING: Removing unreachable block (ram,0x00401138) */
/* WARNING: Removing unreachable block (ram,0x00401108) */
/* WARNING: Removing unreachable block (ram,0x00401130) */

undefined4 mode_change(undefined4 param_1)

{
  bool bVar1;
  undefined1 local_1c [4];
  undefined4 local_18;
  
  bVar1 = false;
  while (!bVar1) {
    local_1c[0] = 0x78;
    local_18 = 0x78;
    system("sync");
    pwrite(param_1,local_1c,1,0);
    bVar1 = true;
  }
  return 0;
}



/* ---- dm_add_entry @ 0040135c ----
 * Append a (fd, handler) pair to the dispatch table used by event_dispatcher.
 */

undefined4 dm_add_entry(int param_1,undefined4 param_2,undefined4 param_3)

{
  int local_20;
  int local_18;
  undefined4 local_10;
  
  local_20 = 0;
  for (local_18 = param_1; *(int *)(local_18 + 4) != 0; local_18 = local_18 + 8) {
    local_20 = local_20 + 1;
  }
  if (local_20 < 4) {
    *(undefined4 *)(param_1 + local_20 * 8) = param_2;
    *(undefined4 *)(param_1 + local_20 * 8 + 4) = param_3;
  }
  else {
    local_10 = 0xfffffff4;
  }
  return local_10;
}



/* ---- send_key @ 00401418 ----
 * Transmit an IR symbol: look up its raw code (irr_get_code_from_symbol),
 * convert it to hardware pulse/space instructions
 * (irr_convert_code_to_hw_instruction), write() it to the IR TX fd, and
 * optionally repeat it N times. IR transmit path (unused on MT300).
 */

undefined4 send_key(undefined4 param_1,undefined4 param_2,int param_3,undefined4 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  uint local_34;
  int local_30;
  int local_1c;
  int local_18 [2];
  undefined4 local_10;
  
  local_18[0] = 0;
  iVar1 = irr_get_code_from_symbol(param_2,local_18);
  if (iVar1 == 0) {
    printf("Symbol %s not found\n",param_2);
    local_10 = 0xffffffea;
  }
  else {
    puts("RAW CODE SEND: ");
    for (local_34 = 0; (int)local_34 < local_18[0]; local_34 = local_34 + 1) {
      if (((local_34 != 0) && ((local_34 & 0xf) == 0)) && (0xf < (int)local_34)) {
        putchar(10);
      }
      printf("%02x ",*(undefined1 *)(iVar1 + local_34));
    }
    putchar(10);
    iVar1 = irr_convert_code_to_hw_instruction(iVar1,local_18[0],&local_1c);
    printf("inst_len %d\n",local_1c);
    if (iVar1 == 0) {
      local_10 = 0xffffffea;
    }
    else {
      iVar2 = local_1c << 2;
      iVar3 = write(param_1,iVar1,iVar2);
      free(iVar1);
      if (iVar3 == iVar2) {
        if (0 < param_3) {
          uVar4 = irr_tx_get_repeated_code(param_4,local_18);
          uVar4 = irr_convert_code_to_hw_instruction(uVar4,local_18[0],&local_1c);
          for (local_30 = 0; local_30 < param_3; local_30 = local_30 + 1) {
            iVar1 = write(param_1,uVar4,local_1c << 2);
            if (iVar1 != local_1c << 2) {
              return 0xfffffffb;
            }
          }
          free(uVar4);
          printf("repeated last code %d times\n",param_3);
        }
        local_10 = 0;
      }
      else {
        local_10 = 0xfffffffb;
      }
    }
  }
  return local_10;
}



/* ---- ir_rx_evt_handler @ 00401750 ----
 * Handler for /dev/irc (IR receiver). read()s a raw IR frame, decodes it to a
 * symbol via irr_get_symbol_from_code; if it is a repeat of the last code it
 * is ignored, otherwise if the symbol == "POWER" it calls mode_change(-1) -
 * the same power action as the physical button. Only active when the IR
 * config was loaded.
 */

undefined4 ir_rx_evt_handler(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  uint local_21c;
  undefined1 auStack_20c [256];
  undefined1 auStack_10c [260];
  
  iVar1 = read(param_1,auStack_10c,0x100);
  printf("%s:read n bytes %d from fd %d\n","ir_rx_evt_handler",iVar1,param_1);
  puts("RAW RX: ");
  for (local_21c = 0; (int)local_21c < iVar1; local_21c = local_21c + 1) {
    if (((local_21c != 0) && ((local_21c & 0xf) == 0)) && (0xf < (int)local_21c)) {
      putchar(10);
    }
    printf("%02x ",auStack_10c[local_21c]);
  }
  putchar(10);
  memset(auStack_20c,0,0x100);
  memcpy(auStack_20c,auStack_10c,iVar1);
  memcpy(rx_last_code,auStack_10c,iVar1);
  iVar2 = irr_get_symbol_from_code(auStack_20c,iVar1);
  if (iVar2 == 0) {
    iVar1 = irr_rx_is_repeated_code(rx_last_code,auStack_20c,iVar1);
    if (iVar1 == 0) {
      puts("Cannot get symbol");
    }
    else {
      puts("Repeated last");
    }
  }
  else {
    printf("Get symbol: %s\n",iVar2);
    iVar1 = strcmp(iVar2,"POWER");
    if (iVar1 == 0) {
      mode_change(0xffffffff);
    }
  }
  return 0;
}



/* ---- print_cmd_help @ 004019f4 ----
 * Print the interactive command help (s/d/c/q/h).
 */

void print_cmd_help(void)

{
  int local_28;
  char *local_24 [4];
  char *local_14;
  undefined4 local_10;
  
  local_24[0] = "s <SYMBOL>: transmit signals of <SYMBOL> from TX0";
  local_24[1] = "d <SYMBOL>: transmit signals of <SYMBOL> from TX1";
  local_24[2] = 
  "c <SYMBOL> <COUNT>: repeatedly transmit signals of <SYMBOL> for <COUNT> times from TX0";
  local_24[3] = "h: this help";
  local_14 = "q: quit";
  local_10 = 0;
  for (local_28 = 0; local_24[local_28] != (char *)0x0; local_28 = local_28 + 1) {
    puts(local_24[local_28]);
  }
  return;
}



/* ---- kb_evt_handler @ 00401ab8 ----
 * Interactive console command handler (stdin), used for bench testing. Parses
 * a line split on ' ' (strtok) and dispatches single-letter commands:
 * 's <sym>' / 'd <sym>' send an IR symbol from TX0/TX1 (send_key), 'q' sets
 * event_loop_stop, 'h'/'?' print help. Not used in the shipped daemon mode.
 */

void kb_evt_handler(undefined4 param_1)

{
  int iVar1;
  char *pcVar2;
  undefined4 uVar3;
  char local_15c;
  undefined1 auStack_154 [63];
  char acStack_115 [265];
  int local_c;
  
  uVar3 = 0;
  iVar1 = read(param_1,acStack_115 + 1,0x100);
  printf("%s:read n bytes %d from fd %d\n","kb_evt_handler",iVar1,param_1,iVar1,uVar3);
  if (acStack_115[iVar1] == '\n') {
    acStack_115[iVar1] = '\0';
  }
  local_15c = acStack_115[1];
  iVar1 = isalpha((int)acStack_115[1]);
  if (iVar1 != 0) {
    local_15c = tolower((int)acStack_115[1]);
  }
  local_c = local_15c + -99;
  switch(local_c) {
  case 0:
    iVar1 = strtok(acStack_115 + 1," ");
    if ((iVar1 != 0) && (iVar1 = strtok(0," "), iVar1 != 0)) {
      strncpy(auStack_154,iVar1,0x40);
      iVar1 = strtok(0," ");
      if (iVar1 == 0) {
        uVar3 = 0;
      }
      else {
        uVar3 = atoi(iVar1);
      }
      send_key(ir_fd,auStack_154,uVar3,auStack_154);
    }
    break;
  case 1:
    strtok(acStack_115 + 1," ");
    pcVar2 = (char *)strtok(0," ");
    if ((pcVar2 != (char *)0x0) && (*pcVar2 != '\0')) {
      iVar1 = strlen(pcVar2);
      if (pcVar2[iVar1 + -1] == '\n') {
        iVar1 = strlen(pcVar2);
        pcVar2[iVar1 + -1] = '\0';
      }
      send_key(tx1_ir_fd,pcVar2,0,0);
    }
    break;
  default:
    print_cmd_help();
    break;
  case 5:
    print_cmd_help();
    break;
  case 0xe:
    event_loop_stop = 1;
    break;
  case 0x10:
    strtok(acStack_115 + 1," ");
    pcVar2 = (char *)strtok(0," ");
    if ((pcVar2 != (char *)0x0) && (*pcVar2 != '\0')) {
      iVar1 = strlen(pcVar2);
      if (pcVar2[iVar1 + -1] == '\n') {
        iVar1 = strlen(pcVar2);
        pcVar2[iVar1 + -1] = '\0';
      }
      send_key(ir_fd,pcVar2,0,0);
    }
  }
  return;
}



/* ---- event_dispatcher @ 00401ec4 ----
 * Walk the (fd,handler) table param_1 until the entry whose fd == param_2,
 * then call that handler(fd). The table is terminated by a NULL handler.
 */

undefined4 event_dispatcher(int *param_1,int param_2)

{
  int *local_10;
  
  local_10 = param_1;
  while( true ) {
    if (local_10[1] == 0) {
      return 0;
    }
    if (*local_10 == param_2) break;
    local_10 = local_10 + 2;
  }
  (*(code *)local_10[1])(param_2);
  return 0;
}



/* ---- reset_terminal_mode @ 00401f54 ----
 * Restore the saved termios saved by set_conio_terminal_mode.
 */

void reset_terminal_mode(void)

{
  tcsetattr(0,0x540e,orig_termios);
  return;
}



/* ---- set_conio_terminal_mode @ 00401f9c ----
 * Save the current termios and switch stdin to raw (no canonical/echo) for
 * interactive key handling.
 */

void set_conio_terminal_mode(void)

{
  undefined1 auStack_40 [56];
  
  tcgetattr(0,orig_termios);
  memcpy(auStack_40,orig_termios,0x34);
  atexit(reset_terminal_mode);
  cfmakeraw(auStack_40);
  tcsetattr(0,0x540e,auStack_40);
  return;
}



/* ---- kbhit @ 00402058 ----
 * Non-blocking check for pending stdin input (select with zero timeout).
 */

void kbhit(void)

{
  undefined4 local_90;
  undefined4 local_8c;
  uint local_88 [32];
  
  local_90 = 0;
  local_8c = 0;
  local_88[0] = local_88[0] | 1;
  select(1,local_88,0,0,&local_90);
  return;
}



/* ---- max_iarray @ 004020c0 ----
 * Return the maximum fd in the fd array (for the select() nfds argument).
 */

int max_iarray(int param_1,int param_2)

{
  int local_10;
  int local_c;
  
  local_10 = -0x80000000;
  for (local_c = 0; local_c < param_2; local_c = local_c + 1) {
    if (local_10 <= *(int *)(param_1 + local_c * 4)) {
      local_10 = *(int *)(param_1 + local_c * 4);
    }
  }
  return local_10;
}



/* ---- event_loop @ 0040215c ----
 * The daemon's core: build an fd_set over all registered fds (param_2 =
 * array of fds, param_3 = count) and block in select() forever (no timeout).
 * When a fd is readable, call event_dispatcher to invoke its handler. Loops
 * until the global event_loop_stop is set.
 */

void event_loop(undefined4 param_1,int param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  uint local_ac;
  int local_98;
  uint auStack_8c [33];
  
  iVar1 = max_iarray(param_2,param_3);
  while (event_loop_stop == 0) {
    for (local_ac = 0; local_ac < 0x20; local_ac = local_ac + 1) {
      auStack_8c[local_ac] = 0;
    }
    for (local_98 = 0; local_98 < param_3; local_98 = local_98 + 1) {
      uVar2 = *(uint *)(param_2 + local_98 * 4) >> 5;
      auStack_8c[uVar2] = auStack_8c[uVar2] | 1 << (*(uint *)(param_2 + local_98 * 4) & 0x1f);
    }
    iVar3 = select(iVar1 + 1,auStack_8c,0,0,0);
    if (iVar3 < 0) {
      perror("select");
    }
    else if (iVar3 < 1) {
      puts("select timeout");
    }
    else {
      for (local_98 = 0; local_98 < param_3; local_98 = local_98 + 1) {
        if (((int)auStack_8c[*(uint *)(param_2 + local_98 * 4) >> 5] >>
             (*(uint *)(param_2 + local_98 * 4) & 0x1f) & 1U) != 0) {
          event_dispatcher(param_1,*(undefined4 *)(param_2 + local_98 * 4));
        }
      }
    }
  }
  return;
}



/* ---- process_conf_file @ 004023d0 ----
 * Parse the IR remote config file (fopen mode "r") via the kv_* key/value
 * parser and build the remote's symbol<->code tables. Returns <0 on failure,
 * which disables the IR power key.
 */

undefined4 process_conf_file(undefined4 param_1)

{
  int iVar1;
  undefined4 local_10;
  
  kv_init();
  iVar1 = fopen(param_1,"r");
  if (iVar1 == 0) {
    printf("Failed to open %s\n",param_1);
    local_10 = 0xfffffffe;
  }
  else {
    kv_parse_file(iVar1);
    fclose(iVar1);
    local_10 = 0;
  }
  return local_10;
}



/* ---- print_opt_help @ 00402494 ----
 * Print the command-line usage (-f config, -h help).
 */

void print_opt_help(void)

{
  int local_18;
  char *local_14 [3];
  
  local_14[0] = "-f <config file>: specify the config file";
  local_14[1] = "-h: this help";
  local_14[2] = (char *)0x0;
  for (local_18 = 0; local_14[local_18] != (char *)0x0; local_18 = local_18 + 1) {
    puts(local_14[local_18]);
  }
  return;
}



/* ---- micro_evt_handler @ 00402534 ----
 * Handler for /dev/micro (the box microcontroller). pread()s ONE byte (the
 * "reason" code) from the device, logs "Power switch is pressed. (reason %c)",
 * and calls mode_change(). This is how the physical power/reset button press
 * reaches software: the microcontroller senses/debounces the switch and
 * pushes a one-byte event up the char device.
 */

void micro_evt_handler(undefined4 param_1)

{
  char local_88 [128];
  
  puts("read micro");
  pread(param_1,local_88,1,0);
  printf("Power switch is pressed. (reason %c)\n",(int)local_88[0]);
  mode_change(param_1);
  return;
}



/* ---- main @ 004025cc ----
 * Entry point. getopt(argc,argv,"f:h"): -f <irc-config> enables the IR
 * remote; -h prints help. Opens /dev/micro (O_RDWR) and /sys/power/state
 * (O_WRONLY, kept in global `sysfd`); exits if either fails. If no valid IR
 * config (the shipped case: rcS runs pm_daemon with NO -f), prints
 * "IRC config file reading error. IRDA power key will not work!" and
 * registers ONLY /dev/micro -> micro_evt_handler, then event_loop(). With a
 * valid config it also opens /dev/irc (RX, ir_fd) and /dev/irc1 (TX,
 * tx1_ir_fd), sets up the IR driver, and registers /dev/irc ->
 * ir_rx_evt_handler as well. dm_add_entry builds the (fd->handler) table.
 */

undefined4 main(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int local_128;
  char local_88 [64];
  int aiStack_48 [3];
  undefined1 auStack_3c [36];
  undefined4 local_18;
  undefined4 local_14;
  int local_10;
  
  bVar1 = false;
  local_128 = 0;
  state = 1;
  setbuf(stdout,0);
  while (iVar2 = getopt(param_1,param_2,"f:h"), iVar2 != -1) {
    local_10 = iVar2;
    if (iVar2 != 0x66) {
      if (iVar2 == 0x68) {
        print_opt_help();
        return 0xffffffea;
      }
      print_opt_help();
      return 0xffffffea;
    }
    bVar1 = true;
    strncpy(local_88,optarg,0x3f);
  }
  if ((bVar1) && ((local_88[0] == '\0' || (iVar2 = process_conf_file(local_88), iVar2 < 0)))) {
    puts("IRC config file reading error. IRDA power key will not work!");
    bVar1 = false;
  }
  iVar2 = open("/dev/micro",2);
  if (iVar2 < 0) {
    local_18 = 0xffffffff;
  }
  else {
    sysfd = open("/sys/power/state",1);
    if (sysfd < 0) {
      local_18 = 0xffffffff;
    }
    else {
      if (!bVar1) {
LAB_004029a4:
        aiStack_48[local_128] = iVar2;
        dm_add_entry(auStack_3c,aiStack_48[local_128],micro_evt_handler);
        event_loop(auStack_3c,aiStack_48,local_128 + 1);
        close(sysfd);
        close(iVar2);
        return local_14;
      }
      irr_new_remote();
      irr_init_remote();
      fflush(stdout);
      iVar3 = open("/dev/irc",2);
      tx1_ir_fd = open("/dev/irc1",2);
      if (iVar3 < 0) {
        printf("failed to open %s\n","/dev/irc");
        local_18 = 0xfffffffe;
      }
      else {
        if (-1 < tx1_ir_fd) {
          irr_setup_driver(iVar3);
          irr_setup_driver(tx1_ir_fd);
          memset(auStack_3c,0,0x20);
          memset(kb_command,0,0x48);
          local_128 = 1;
          aiStack_48[0] = iVar3;
          dm_add_entry(auStack_3c,iVar3,ir_rx_evt_handler);
          ir_fd = iVar3;
          goto LAB_004029a4;
        }
        printf("failed to open %s\n","/dev/irc1");
        local_18 = 0xfffffffe;
      }
    }
  }
  return local_18;
}



/* ---- irr_new_remote @ 00402a70 ----
 * IR-remote library: allocate a new remote descriptor.
 */

undefined4 irr_new_remote(void)

{
  remote = malloc(0x324);
  memset(remote,0,0x324);
  memset(noop_protocol_ops,0,0x10c);
  return remote;
}



/* ---- str_tolower @ 00402b04 ----
 * Lowercase a string in place (config parsing helper).
 */

int str_tolower(int param_1)

{
  undefined1 uVar1;
  int iVar2;
  int local_18;
  
  iVar2 = strlen(param_1);
  for (local_18 = 0; local_18 < iVar2; local_18 = local_18 + 1) {
    uVar1 = tolower((int)*(char *)(param_1 + local_18));
    *(undefined1 *)(param_1 + local_18) = uVar1;
  }
  return param_1;
}



/* ---- pack_instruction @ 00402bb8 ----
 * IR-remote library: pack a pulse/space duration into a driver instruction
 * word.
 */

uint pack_instruction(uint param_1,uint param_2,uint param_3,uint param_4)

{
  return (param_1 & 0xfffff) + (param_2 & 0xfffff) | (param_3 & 1) << 0x14 | (param_4 & 1) << 0x15;
}



/* ---- irr_convert_code_to_hw_instruction @ 00402c54 ----
 * IR-remote library: turn a logical code into the pulse/space hardware
 * instruction list the IR TX driver expects.
 */

int irr_convert_code_to_hw_instruction(int param_1,int param_2,int *param_3)

{
  bool bVar1;
  bool bVar2;
  byte bVar3;
  byte bVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int local_44;
  int local_2c;
  int local_28;
  int local_18;
  
  uVar5 = malloc(param_2);
  bVar2 = true;
  local_44 = 0;
  bVar1 = false;
  iVar7 = 0;
  if (param_1 == 0) {
    *param_3 = 0;
    local_18 = 0;
  }
  else if (param_2 < 1) {
    *param_3 = 0;
    local_18 = 0;
  }
  else {
    local_18 = malloc(0x200);
    if (local_18 == 0) {
      *param_3 = 0;
      local_18 = 0;
    }
    else {
      for (local_28 = 0; local_28 < param_2; local_28 = local_28 + 1) {
        bVar4 = *(byte *)(param_1 + local_28);
        for (local_2c = 0; local_2c < 8; local_2c = local_2c + 1) {
          bVar3 = bVar4 & 1;
          bVar4 = (char)bVar4 >> 1;
          iVar7 = iVar7 + 1;
          if (bVar2) {
            bVar1 = bVar3 != 0;
            if (bVar1) {
              uVar6 = pack_instruction(0,carr_per_bittime,0,1,0,iVar7);
              *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
            }
            else {
              uVar6 = pack_instruction(0,carr_per_bittime,0,0,0,iVar7);
              *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
            }
            bVar2 = false;
          }
          else if ((bVar3 != 0) || (bVar1)) {
            if ((bVar3 == 0) && (bVar1)) {
              local_44 = local_44 + 1;
              uVar6 = pack_instruction(0,carr_per_bittime,0,0,0,iVar7);
              *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
              bVar1 = false;
            }
            else if ((bVar3 == 0) || (!bVar1)) {
              if ((bVar3 != 0) && (!bVar1)) {
                local_44 = local_44 + 1;
                uVar6 = pack_instruction(0,carr_per_bittime,0,1,0,iVar7);
                *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
                bVar1 = true;
              }
            }
            else {
              uVar6 = pack_instruction(*(undefined4 *)(local_18 + local_44 * 4),carr_per_bittime,0,1
                                       ,0,iVar7);
              *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
            }
          }
          else {
            uVar6 = pack_instruction(*(undefined4 *)(local_18 + local_44 * 4),carr_per_bittime,0,0,0
                                     ,iVar7);
            *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
          }
        }
        if (param_2 <= local_28 + 1) {
          if (bVar1) {
            uVar6 = pack_instruction(*(undefined4 *)(local_18 + local_44 * 4),0,0,1);
            *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
            local_44 = local_44 + 1;
            uVar6 = pack_instruction(0,*(int *)(remote + 0x110) * carr_per_bittime,1,0);
            *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
          }
          else {
            uVar6 = pack_instruction(*(undefined4 *)(local_18 + local_44 * 4),
                                     *(int *)(remote + 0x110) * carr_per_bittime,1,0);
            *(undefined4 *)(local_18 + local_44 * 4) = uVar6;
          }
        }
      }
      free(uVar5);
      *param_3 = local_44 + 1;
    }
  }
  return local_18;
}



/* ---- irr_make_key @ 00403158 ----
 * IR-remote library: build a key entry (symbol + code) from config.
 */

int irr_make_key(undefined4 param_1,int param_2)

{
  undefined1 uVar1;
  int iVar2;
  int local_30;
  int local_2c;
  uint local_28;
  undefined1 local_20;
  undefined1 local_1f;
  undefined1 local_1e;
  int local_18;
  
  local_30 = 0;
  local_1e = 0;
  iVar2 = malloc(0x248);
  if (iVar2 == 0) {
    local_18 = 0;
  }
  else {
    strncpy(iVar2,param_1,0x3f);
    strncpy(iVar2 + 0x40,param_2,0xff);
    local_28 = strlen(param_2);
    if ((local_28 & 1) != 0) {
      local_28 = local_28 + 1;
    }
    for (local_2c = 0; local_2c < (int)local_28; local_2c = local_2c + 2) {
      local_20 = *(undefined1 *)(param_2 + local_2c);
      local_1f = *(undefined1 *)(param_2 + local_2c + 1);
      uVar1 = strtoul(&local_20,0,0x10);
      *(undefined1 *)(iVar2 + local_30 + 0x140) = uVar1;
      local_30 = local_30 + 1;
    }
    *(int *)(iVar2 + 0x240) = local_30;
    *(undefined4 *)(iVar2 + 0x244) = 0;
    local_18 = iVar2;
  }
  return local_18;
}



/* ---- irr_init_remote @ 004032fc ----
 * IR-remote library: initialize the remote's protocol/tables.
 */

int irr_init_remote(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int local_168;
  int local_164;
  undefined1 auStack_15c [64];
  undefined1 auStack_11c [260];
  int local_18;
  
  bVar1 = false;
  if (remote == 0) {
    local_18 = 0;
  }
  else {
    while (iVar3 = kv_next_key_and_value(auStack_15c,auStack_11c), iVar3 != 0) {
      iVar3 = strcmp(auStack_15c,"key_start");
      if (iVar3 == 0) {
        bVar1 = true;
      }
      else {
        iVar3 = strcmp(auStack_15c,"key_end");
        if (iVar3 == 0) {
          bVar1 = false;
        }
        else {
          iVar2 = strcmp(auStack_15c,"frequency");
          iVar3 = remote;
          if (iVar2 == 0) {
            uVar4 = strtoul(auStack_11c,0,10);
            *(undefined4 *)(iVar3 + 0x100) = uVar4;
          }
          else {
            iVar2 = strcmp(auStack_15c,"current_clock");
            iVar3 = remote;
            if (iVar2 == 0) {
              uVar4 = strtoul(auStack_11c,0,10);
              *(undefined4 *)(iVar3 + 0x104) = uVar4;
            }
            else {
              iVar2 = strcmp(auStack_15c,"duty_cycle");
              iVar3 = remote;
              if (iVar2 == 0) {
                uVar4 = strtoul(auStack_11c,0,10);
                *(undefined4 *)(iVar3 + 0x108) = uVar4;
              }
              else {
                iVar2 = strcmp(auStack_15c,"bit_time");
                iVar3 = remote;
                if (iVar2 == 0) {
                  uVar4 = strtoul(auStack_11c,0,10);
                  *(undefined4 *)(iVar3 + 0x10c) = uVar4;
                }
                else {
                  iVar2 = strcmp(auStack_15c,"rx_max_space");
                  iVar3 = remote;
                  if (iVar2 == 0) {
                    uVar4 = strtoul(auStack_11c,0,10);
                    *(undefined4 *)(iVar3 + 0x110) = uVar4;
                  }
                  else {
                    iVar2 = strcmp(auStack_15c,"tx_repeat_seq");
                    iVar3 = remote;
                    if (iVar2 == 0) {
                      uVar4 = strtoul(auStack_11c,0,10);
                      *(undefined4 *)(iVar3 + 0x114) = uVar4;
                    }
                    else {
                      iVar3 = strcmp(auStack_15c,"protocol");
                      if (iVar3 == 0) {
                        strncpy(remote + 0x21c,auStack_11c,0x100);
                        printf("protocol %s\n",remote + 0x21c);
                      }
                      else {
                        iVar2 = strcmp(auStack_15c,"pattern_match");
                        iVar3 = remote;
                        if (iVar2 == 0) {
                          uVar4 = strtoul(auStack_11c,0,10);
                          *(undefined4 *)(iVar3 + 0x118) = uVar4;
                        }
                        else {
                          iVar2 = strcmp(auStack_15c,"pattern");
                          iVar3 = remote;
                          if (iVar2 == 0) {
                            strncpy(remote + 0x11c,auStack_11c,0x100);
                          }
                          else if (bVar1) {
                            if (*(int *)(remote + 0x31c) == 0) {
                              uVar4 = irr_make_key(auStack_15c,auStack_11c);
                              *(undefined4 *)(iVar3 + 0x31c) = uVar4;
                              local_164 = *(int *)(remote + 0x31c);
                            }
                            else {
                              uVar4 = irr_make_key(auStack_15c,auStack_11c);
                              *(undefined4 *)(local_164 + 0x244) = uVar4;
                              local_164 = *(int *)(local_164 + 0x244);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    for (local_168 = 0; local_168 < 2; local_168 = local_168 + 1) {
      iVar3 = strcmp(&irc_registered_protocols + local_168 * 0x104,remote + 0x21c);
      if (iVar3 == 0) {
        printf("Using %s protocol\n",&irc_registered_protocols + local_168 * 0x104);
        iVar3 = remote;
        uVar4 = (*(code *)(&PTR_nec_protocol_setup_00415914)[local_168 * 0x41])();
        *(undefined4 *)(iVar3 + 800) = uVar4;
      }
    }
    if (*(int *)(remote + 800) == 0) {
      *(undefined1 **)(remote + 800) = noop_protocol_ops;
    }
    local_18 = remote;
  }
  return local_18;
}



/* ---- irr_get_code_from_symbol @ 004038cc ----
 * IR-remote library: look up the raw code bytes for a named symbol.
 */

int irr_get_code_from_symbol(undefined4 param_1,undefined4 *param_2)

{
  int iVar1;
  int local_18;
  
  local_18 = *(int *)(remote + 0x31c);
  while( true ) {
    if (local_18 == 0) {
      return 0;
    }
    iVar1 = strncmp(param_1,local_18,0x40);
    if (iVar1 == 0) break;
    local_18 = *(int *)(local_18 + 0x244);
  }
  *param_2 = *(undefined4 *)(local_18 + 0x240);
  return local_18 + 0x140;
}



/* ---- irr_get_code_str_from_symbol @ 00403980 ----
 * IR-remote library: code-string lookup from a symbol.
 */

int irr_get_code_str_from_symbol(undefined4 param_1)

{
  int iVar1;
  int local_18;
  
  local_18 = *(int *)(remote + 0x31c);
  while( true ) {
    if (local_18 == 0) {
      return 0;
    }
    iVar1 = strncmp(param_1,local_18,0x40);
    if (iVar1 == 0) break;
    local_18 = *(int *)(local_18 + 0x244);
  }
  return local_18 + 0x40;
}



/* ---- irr_match_code @ 00403a20 ----
 * IR-remote library: match a received code against the table.
 */

undefined4 irr_match_code(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined4 local_10;
  
  if (*(int *)(*(int *)(remote + 800) + 0x100) == 0) {
    iVar1 = memcmp(param_1,param_2,param_3);
    if (iVar1 == 0) {
      local_10 = param_1;
    }
    else {
      local_10 = 0;
    }
  }
  else {
    local_10 = (**(code **)(*(int *)(remote + 800) + 0x100))(param_1,param_2,param_3);
  }
  return local_10;
}



/* ---- irr_match_code_str @ 00403ae4 ----
 * IR-remote library: match a code string against the table.
 */

int irr_match_code_str(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int local_28;
  int local_24;
  int local_20;
  int local_10;
  int local_c;
  
  iVar1 = strlen(param_1);
  iVar2 = strlen(param_2);
  if ((iVar1 < 1) || (iVar2 < 1)) {
    local_10 = 0;
  }
  else {
    str_tolower(param_1);
    str_tolower(param_2);
    if (iVar1 < iVar2) {
      local_28 = param_1;
      local_24 = param_2;
    }
    else {
      local_28 = param_2;
      local_24 = param_1;
    }
    if (*(int *)(*(int *)(remote + 800) + 0x100) == 0) {
      local_20 = strcasestr(local_24,local_28);
    }
    else {
      local_20 = (**(code **)(*(int *)(remote + 800) + 0x100))(local_24,local_28,iVar1);
    }
    if (local_20 == 0) {
      local_10 = 0;
    }
    else {
      iVar1 = strlen(local_28);
      iVar2 = strlen(local_24);
      if (*(char *)(local_28 + iVar1 + -1) != *(char *)(local_24 + iVar2 + -1)) {
        return local_c;
      }
      local_10 = local_20;
    }
  }
  return local_10;
}



/* ---- irr_get_symbol_from_code_str @ 00403cb4 ----
 * IR-remote library: symbol lookup from a code string.
 */

int irr_get_symbol_from_code_str(undefined4 param_1)

{
  int iVar1;
  int local_114;
  undefined1 auStack_110 [264];
  
  local_114 = *(int *)(remote + 0x31c);
  while( true ) {
    if (local_114 == 0) {
      return 0;
    }
    strcpy(auStack_110,local_114 + 0x40);
    iVar1 = irr_match_code_str(auStack_110,param_1);
    if (iVar1 != 0) break;
    local_114 = *(int *)(local_114 + 0x244);
  }
  return local_114;
}



/* ---- irr_get_symbol_from_code @ 00403d68 ----
 * IR-remote library: reverse lookup - find the symbol name for a received
 * raw code (used by ir_rx_evt_handler to recognize POWER).
 */

int irr_get_symbol_from_code(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int local_114;
  undefined1 auStack_110 [264];
  
  local_114 = *(int *)(remote + 0x31c);
  while( true ) {
    if (local_114 == 0) {
      return 0;
    }
    memcpy(auStack_110,local_114 + 0x140,param_2);
    iVar1 = irr_match_code(auStack_110,param_1,param_2);
    if (iVar1 != 0) break;
    local_114 = *(int *)(local_114 + 0x244);
  }
  return local_114;
}



/* ---- irr_get_first_key @ 00403e28 ----
 * IR-remote library: first key in the remote's table.
 */

undefined4 irr_get_first_key(void)

{
  return *(undefined4 *)(remote + 0x31c);
}



/* ---- irr_get_tx_repeat_seq @ 00403e54 ----
 * IR-remote library: the protocol's repeat sequence.
 */

undefined4 irr_get_tx_repeat_seq(void)

{
  return *(undefined4 *)(remote + 0x114);
}



/* ---- irr_rx_is_repeated_code @ 00403e80 ----
 * IR-remote library: detect a protocol repeat frame vs a new key.
 */

undefined4 irr_rx_is_repeated_code(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 local_10;
  
  if (*(int *)(*(int *)(remote + 800) + 0x104) == 0) {
    local_10 = 0;
  }
  else {
    local_10 = (**(code **)(*(int *)(remote + 800) + 0x104))(param_1,param_2,param_3);
  }
  return local_10;
}



/* ---- irr_tx_get_repeated_code @ 00403f04 ----
 * IR-remote library: produce the repeat frame for TX.
 */

undefined4 irr_tx_get_repeated_code(undefined4 param_1,undefined4 *param_2)

{
  undefined4 local_10;
  
  if (*(int *)(*(int *)(remote + 800) + 0x108) == 0) {
    *param_2 = 0;
    local_10 = 0;
  }
  else {
    local_10 = (**(code **)(*(int *)(remote + 800) + 0x108))(param_1,param_2);
  }
  return local_10;
}



/* ---- irr_get_str_pattern_mask @ 00403f88 ----
 * IR-remote library: mask from a string pattern.
 */

undefined4 irr_get_str_pattern_mask(char *param_1,uint *param_2,uint *param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  uint local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  int local_14;
  char local_c [4];
  
  local_20 = 0;
  local_24 = 0;
  uVar2 = 0;
  iVar1 = strlen(param_1);
  if (*param_1 == '0') {
    local_18 = iVar1 << 2;
  }
  else {
    local_c[0] = *param_1;
    local_c[1] = 0;
    local_1c = strtoul(local_c,0,0x10,param_4,uVar2);
    for (local_14 = 1; local_14 < 5; local_14 = local_14 + 1) {
      if ((local_1c & 1) != 0) {
        local_18 = local_14;
      }
      local_1c = local_1c >> 1;
    }
    local_18 = local_18 + (iVar1 + -1) * 4;
  }
  if (local_18 < 0x20) {
    local_20 = 1 << (local_18 - 1 & 0x1f);
    local_20 = local_20 | (~local_20 << (0x20 - local_18 & 0x1f)) >> (0x20 - local_18 & 0x1f);
  }
  else if ((0x1f < local_18) && (local_18 < 0x40)) {
    local_24 = 1 << (local_18 - 0x21 & 0x1f);
    local_24 = local_24 |
               (~local_24 << (0x20 - (local_18 - 0x20) & 0x1f)) >> (0x20 - (local_18 - 0x20) & 0x1f)
    ;
    local_20 = 0xffffffff;
  }
  printf("mask_l 0x%08x, mask_h 0x%08x\n",local_20,local_24);
  *param_2 = local_20;
  *param_3 = local_24;
  return 0;
}



/* ---- irr_convert_pattern_str @ 0040420c ----
 * IR-remote library: parse a pattern string into mask/value bits.
 */

void irr_convert_pattern_str(int param_1,undefined4 *param_2,undefined4 *param_3)

{
  undefined4 uVar1;
  int iVar2;
  int local_1c [2];
  undefined1 auStack_14 [8];
  undefined1 local_c;
  
  local_1c[0] = strlen(param_1);
  if (local_1c[0] < 8) {
    strcpy(auStack_14,param_1);
    uVar1 = strtoul(auStack_14,0,0x10);
    *param_2 = uVar1;
  }
  else {
    iVar2 = local_1c[0] + -8;
    memcpy(auStack_14,param_1 + iVar2,8);
    local_c = 0;
    uVar1 = strtoul(auStack_14,0,0x10);
    *param_2 = uVar1;
    printf("LOW R %s len %d p %d\n",auStack_14,local_1c[0],iVar2);
    if (local_1c[0] < 0x11) {
      memcpy(auStack_14,param_1,local_1c[0] + -8);
      *(undefined1 *)((int)local_1c + local_1c[0]) = 0;
      uVar1 = strtoul(auStack_14,0,0x10);
      *param_3 = uVar1;
      printf("HIGH R %s\n",auStack_14);
    }
    else {
      memcpy(auStack_14,param_1 + local_1c[0] + -0x10,8);
      local_c = 0;
      printf("HIGH R %s\n",auStack_14);
      uVar1 = strtoul(auStack_14,0,0x10);
      *param_3 = uVar1;
    }
  }
  return;
}



/* ---- irr_get_pattern_mask @ 00404474 ----
 * IR-remote library: compute the bit mask for a protocol pattern.
 */

uint irr_get_pattern_mask(uint param_1)

{
  uint local_res0;
  uint local_14;
  int local_10;
  int local_c;
  
  local_10 = -1;
  local_14 = 0;
  local_res0 = param_1;
  for (local_c = 0; local_c < 0x20; local_c = local_c + 1) {
    if ((local_res0 & 1) != 0) {
      local_10 = local_c;
    }
    local_res0 = local_res0 >> 1;
  }
  for (local_c = 0; local_c <= local_10; local_c = local_c + 1) {
    local_14 = local_14 << 1 | 1;
  }
  printf("patt 0x%x, mask 0x%x\n",param_1,local_14);
  return local_14;
}



/* ---- irr_setup_driver @ 00404568 ----
 * IR-remote library: configure the kernel IR driver (carrier, duty cycle,
 * bit timing) on the given fd via ioctl.
 */

undefined4 irr_setup_driver(undefined4 param_1)

{
  uint uVar1;
  uint uVar2;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20 [2];
  undefined4 local_18;
  
  local_2c = 0;
  local_28 = 0;
  local_24 = 0;
  local_20[0] = 0;
  if (remote == 0) {
    puts("Remote has not been allocated yet");
    local_18 = 0xfffffffe;
  }
  else {
    uVar1 = *(uint *)(remote + 0x104) / *(uint *)(remote + 0x100);
    if (*(uint *)(remote + 0x100) == 0) {
      trap(7);
    }
    uVar2 = uVar1 / *(uint *)(remote + 0x108);
    if (*(uint *)(remote + 0x108) == 0) {
      trap(7);
    }
    ioctl(param_1,0x80046901,uVar1 - uVar2);
    ioctl(param_1,0x80046902,uVar2);
    carr_per_bittime = (uint)(*(int *)(remote + 0x100) * *(int *)(remote + 0x10c)) / 1000000;
    printf("carr_per_bittime %d\n",carr_per_bittime);
    ioctl(param_1,0x80046903,carr_per_bittime * uVar1);
    ioctl(param_1,0x80046904,*(undefined4 *)(remote + 0x110));
    irr_convert_pattern_str(remote + 0x11c,&local_2c,&local_28);
    irr_get_str_pattern_mask(remote + 0x11c,&local_24,local_20);
    printf("patt_l 0x%08x, patt_h 0x%08x, mask_l 0x%08x, mask_h 0x%08x\n",local_2c,local_28,
                 local_24,local_20[0]);
    if (*(int *)(remote + 0x118) == 0) {
      local_30 = ioctl(param_1,0x80046905,0);
    }
    else {
      ioctl(param_1,0x80046905,1);
      ioctl(param_1,0x80046906,local_2c);
      ioctl(param_1,0x80046907,local_28);
      ioctl(param_1,0x80046908,local_24);
      local_30 = ioctl(param_1,0x80046909,local_20[0]);
    }
    local_18 = local_30;
  }
  return local_18;
}



/* ---- nec_received_repeated_code @ 004048e0 ----
 * IR-remote library: NEC repeat-frame detection.
 */

bool nec_received_repeated_code(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  iVar1 = irr_match_code(param_2,&nec_repeat_header,param_3);
  return iVar1 != 0;
}



/* ---- nec_generate_repeated_code @ 00404954 ----
 * IR-remote library: NEC repeat-frame generation.
 */

undefined3 * nec_generate_repeated_code(undefined4 param_1,undefined4 *param_2)

{
  *param_2 = 3;
  return &nec_repeat_header;
}



/* ---- nec_protocol_setup @ 00404990 ----
 * IR-remote library: register the NEC protocol handlers.
 */

undefined1 * nec_protocol_setup(void)

{
  return nec_protocol_ops;
}



/* ---- rc6_match_code @ 004049c0 ----
 * IR-remote library: RC6 code matching.
 */

undefined4 rc6_match_code(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  undefined1 auStack_210 [2];
  undefined1 local_20e;
  undefined1 auStack_110 [2];
  undefined1 local_10e;
  undefined4 local_10;
  
  memcpy(auStack_210,param_1,param_3);
  memcpy(auStack_110,param_2,param_3);
  local_20e = 0;
  local_10e = 0;
  iVar1 = memcmp(auStack_210,auStack_110,param_3);
  if (iVar1 == 0) {
    local_10 = param_1;
  }
  else {
    local_10 = 0;
  }
  return local_10;
}



/* ---- rc6_protocol_setup @ 00404a88 ----
 * IR-remote library: register the RC6 protocol handlers.
 */

undefined1 * rc6_protocol_setup(void)

{
  return rc6_protocol_ops;
}



/* ---- kv_next_key_and_value @ 00404ab0 ----
 * Iterator over the parsed key/value node list.
 */

bool kv_next_key_and_value(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  
  if (current_node_first_use != 0) {
    current_node = list_head;
    current_node_first_use = 0;
  }
  bVar1 = current_node != 0;
  if (bVar1) {
    strncpy(param_1,current_node,0x3f);
    strncpy(param_2,current_node + 0x40,0xff);
    current_node = *(int *)(current_node + 0x140);
  }
  return bVar1;
}



/* ---- kv_init @ 00404b94 ----
 * (library/support function; see decompiled body)
 */

undefined4 kv_init(void)

{
  list_head = 0;
  DAT_00415c8c = &list_head;
  return 0;
}



/* ---- kv_print_all_nodes @ 00404bd4 ----
 * Debug dump of the parsed key/value list.
 */

void kv_print_all_nodes(void)

{
  int local_18;
  
  for (local_18 = list_head; local_18 != 0; local_18 = *(int *)(local_18 + 0x140)) {
    printf("key: %s\n",local_18);
    printf("value: %s\n",local_18 + 0x40);
    puts("=====");
  }
  return;
}



/* ---- kv_chop_writespaces @ 00404c84 ----
 * Trim whitespace around a token during config parsing.
 */

int kv_chop_writespaces(int param_1,int param_2)

{
  int iVar1;
  int local_14;
  int local_10;
  
  iVar1 = strlen(param_1);
  local_14 = 0;
  for (local_10 = 0; local_10 < iVar1; local_10 = local_10 + 1) {
    if ((((*(char *)(param_1 + local_10) != ' ') && (*(char *)(param_1 + local_10) != '\t')) &&
        (*(char *)(param_1 + local_10) != '\r')) && (*(char *)(param_1 + local_10) != '\n')) {
      *(undefined1 *)(param_2 + local_14) = *(undefined1 *)(param_1 + local_10);
      local_14 = local_14 + 1;
    }
  }
  *(undefined1 *)(param_2 + local_14) = 0;
  return local_14;
}



/* ---- kv_parse_file @ 00404dac ----
 * Key/value config parser: read a file into an in-memory node list of
 * key=value pairs (used by process_conf_file for the IR config).
 */

int kv_parse_file(undefined4 param_1)

{
  int iVar1;
  undefined1 *puVar2;
  int iVar3;
  int local_210;
  undefined1 auStack_208 [256];
  undefined1 auStack_108 [256];
  
  local_210 = 0;
  while (iVar1 = fgets(auStack_208,0xff,param_1), iVar1 != 0) {
    puVar2 = (undefined1 *)index(auStack_208,0x23);
    if (puVar2 != (undefined1 *)0x0) {
      *puVar2 = 0;
    }
    iVar1 = strtok(auStack_208,&DAT_00405710);
    if (iVar1 != 0) {
      kv_chop_writespaces(iVar1,auStack_108);
      iVar1 = malloc(0x148);
      strncpy(iVar1,auStack_108,0x40);
      while( true ) {
        local_210 = local_210 + 1;
        iVar3 = strtok(0,&DAT_00405710);
        if (iVar3 == 0) break;
        kv_chop_writespaces(iVar3,auStack_108);
        strncpy(iVar1 + 0x40,auStack_108,0x100);
      }
      *(undefined4 *)(iVar1 + 0x140) = 0;
      *(int **)(iVar1 + 0x144) = DAT_00415c8c;
      *DAT_00415c8c = iVar1;
      DAT_00415c8c = (int *)(iVar1 + 0x140);
    }
  }
  return local_210;
}



/* ---- __libc_csu_fini @ 00404fa0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}



/* ---- __libc_csu_init @ 00404fa8 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00405008) */
/* WARNING: Removing unreachable block (ram,0x0040500c) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  (*(code *)PTR__init_00415c74)();
                    /* Unresolved local var: size_t i@[???] */
  return;
}



/* ---- atexit @ 00405060 ----
 * libc atexit registration thunk.
 */

int atexit(_func_void *func)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if (PTR___dso_handle_00415c6c != (undefined *)0x0) {
    uVar2 = *(undefined4 *)PTR___dso_handle_00415c6c;
  }
                    /* WARNING: Could not recover jumptable at 0x00405080. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  iVar1 = (*(code *)PTR___cxa_atexit_00415c78)(func,0,uVar2);
  return iVar1;
}



/* ---- __do_global_ctors_aux @ 00405090 ----
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



/* ---- _fini @ 00405100 ----
 * (library/support function; see decompiled body)
 */

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


