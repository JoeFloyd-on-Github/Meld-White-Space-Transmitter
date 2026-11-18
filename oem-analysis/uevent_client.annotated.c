"/*\n * uevent_client.annotated.c  --  annotated Ghidra decompile of\n *   root/disk/usr/bin/uevent_client\n * ============================================================================\n * Tiny client for uevent_daemon, invoked by blkdev_detect.sh on block-device\n * hotplug. See docs/boot-process.md 6.5.\n *\n * FUN_<addr> libc PLT stubs renamed from the ELF .rel.plt. Not produced by\n * running the binary.\n */"

/* ---- _init @ 00400658 ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  if (PTR___gmon_start___00410f54 != (undefined *)0x0) {
    (*(code *)PTR___gmon_start___00410f54)();
  }
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/* ---- connect @ 004006e0 ----
 * (library/support function; see decompiled body)
 */

void connect(void)

{
  (*(code *)PTR_connect_00410ed8)();
  return;
}



/* ---- getenv @ 004006f0 ----
 * (library/support function; see decompiled body)
 */

void getenv(void)

{
  (*(code *)PTR_getenv_00410edc)();
  return;
}



/* ---- write @ 00400700 ----
 * (library/support function; see decompiled body)
 */

void write(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040070c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_write_00410ee0)();
  return;
}



/* ---- __libc_start_main @ 00400710 ----
 * (library/support function; see decompiled body)
 */

void __libc_start_main(void)

{
  (*(code *)PTR___libc_start_main_00410ee4)();
  return;
}



/* ---- perror @ 00400720 ----
 * (library/support function; see decompiled body)
 */

void perror(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040072c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_perror_00410ee8)();
  return;
}



/* ---- socket @ 00400730 ----
 * (library/support function; see decompiled body)
 */

void socket(void)

{
  (*(code *)PTR_socket_00410eec)();
  return;
}



/* ---- strlen @ 00400740 ----
 * (library/support function; see decompiled body)
 */

void strlen(void)

{
  (*(code *)PTR_strlen_00410ef0)();
  return;
}



/* ---- strcpy @ 00400750 ----
 * (library/support function; see decompiled body)
 */

void strcpy(void)

{
  (*(code *)PTR_strcpy_00410ef4)();
  return;
}



/* ---- close @ 00400760 ----
 * (library/support function; see decompiled body)
 */

void close(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040076c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_00410ef8)();
  return;
}



/* ---- fprintf @ 00400770 ----
 * (library/support function; see decompiled body)
 */

void fprintf(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040077c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fprintf_00410efc)();
  return;
}



/* ---- bzero @ 00400780 ----
 * (library/support function; see decompiled body)
 */

void bzero(void)

{
  (*(code *)PTR_bzero_00410f00)();
  return;
}



/* ---- strcmp @ 00400790 ----
 * (library/support function; see decompiled body)
 */

void strcmp(void)

{
  (*(code *)PTR_strcmp_00410f04)();
  return;
}



/* ---- __start @ 004007b0 ----
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



/* ---- __do_global_dtors_aux @ 00400800 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400854) */
/* WARNING: Removing unreachable block (ram,0x00400858) */
/* WARNING: Removing unreachable block (ram,0x0040087c) */

void __do_global_dtors_aux(void)

{
  if (completed_5578 == '\0') {
    completed_5578 = '\x01';
  }
  return;
}



/* ---- frame_dummy @ 004008a0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x004008bc) */

void frame_dummy(void)

{
  return;
}



/* ---- main @ 004008d0 ----
 * Connect to uevent_daemon's UNIX socket and relay/act on the current uevent.
 * Invoked by blkdev_detect.sh when the kernel reports a block-device hotplug
 * (the env vars TARGETDEV/ACTION/SUBSYSTEM carry the event). Minimal client.
 */

undefined4 main(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int local_100;
  undefined1 auStack_fc [128];
  undefined4 local_7c;
  undefined2 local_78;
  undefined1 auStack_75 [109];
  
  local_100 = -1;
  iVar1 = getenv("TARGETDEV");
  if (iVar1 == 0) {
    perror("No ENV TARGETDEV");
  }
  else {
    strcpy(auStack_fc,iVar1);
    iVar2 = getenv("ACTION");
    if (iVar2 == 0) {
      perror("Not ENV ACTION\n");
    }
    else {
      iVar3 = strcmp(iVar2,&DAT_00400e20);
      if (iVar3 == 0) {
        fprintf(stderr,"Add device %s\n",iVar1);
        local_7c = 1;
      }
      else {
        iVar3 = strcmp(iVar2,"remove");
        if (iVar3 == 0) {
          fprintf(stderr,"Remove device %s\n",iVar1);
          local_7c = 2;
        }
        else {
          iVar3 = strcmp(iVar2,"change");
          if (iVar3 != 0) {
            fprintf(stderr,"device %s, action %s - ",iVar1,iVar2);
            perror("Not supported action");
            goto LAB_00400c20;
          }
          fprintf(stderr,"Change device %s\n",iVar1);
          local_7c = 3;
        }
      }
      bzero(&local_78,0x6e);
      local_78 = 1;
      strcpy(auStack_75,notify_socket);
      iVar1 = strlen(auStack_75);
      local_100 = socket(1,2,0);
      if (local_100 != -1) {
        iVar1 = connect(local_100,&local_78,iVar1 + 3);
        if (iVar1 < 0) {
          perror("Connecting failed");
        }
        else {
          write(local_100,auStack_fc,0x84);
        }
      }
    }
  }
LAB_00400c20:
  if (-1 < local_100) {
    close(local_100);
  }
  return 0;
}



/* ---- __libc_csu_fini @ 00400c60 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}



/* ---- __libc_csu_init @ 00400c68 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400cc8) */
/* WARNING: Removing unreachable block (ram,0x00400ccc) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  (*(code *)PTR__init_00410f50)();
                    /* Unresolved local var: size_t i@[???] */
  return;
}



/* ---- __do_global_ctors_aux @ 00400d20 ----
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



/* ---- _fini @ 00400d80 ----
 * (library/support function; see decompiled body)
 */

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


