"/*\n * uevent_daemon.annotated.c  --  annotated Ghidra decompile of\n *   root/disk/usr/bin/uevent_daemon\n * ============================================================================\n * The kernel-uevent -> application notification broker. It listens for kernel\n * device events (hotplug) and forwards them to client apps that registered over\n * a UNIX socket. uevent_client (run from blkdev_detect.sh on block-device\n * hotplug) is such a client. Started from rcS. See docs/boot-process.md 6.5.\n *\n * FUN_<addr> libc PLT stubs renamed from the ELF .rel.plt (socket/bind/listen/\n * accept/read/write/fork/pthread_create/...). Function names are original DWARF\n * symbols; locals keep Ghidra auto-names. Not produced by running the binary.\n */"

/* ---- _init @ 004009ec ----
 * (library/support function; see decompiled body)
 */

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  if (PTR___gmon_start___00412534 != (undefined *)0x0) {
    (*(code *)PTR___gmon_start___00412534)();
  }
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/* ---- sigemptyset @ 00400a80 ----
 * (library/support function; see decompiled body)
 */

void sigemptyset(void)

{
  (*(code *)PTR_sigemptyset_00412468)();
  return;
}



/* ---- sprintf @ 00400a90 ----
 * (library/support function; see decompiled body)
 */

void sprintf(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400a9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_sprintf_0041246c)();
  return;
}



/* ---- open @ 00400aa0 ----
 * (library/support function; see decompiled body)
 */

void open(void)

{
  (*(code *)PTR_open_00412470)();
  return;
}



/* ---- connect @ 00400ab0 ----
 * (library/support function; see decompiled body)
 */

void connect(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400abc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_connect_00412474)();
  return;
}



/* ---- getpid @ 00400ac0 ----
 * (library/support function; see decompiled body)
 */

void getpid(void)

{
  (*(code *)PTR_getpid_00412478)();
  return;
}



/* ---- getdtablesize @ 00400ad0 ----
 * (library/support function; see decompiled body)
 */

void getdtablesize(void)

{
  (*(code *)PTR_getdtablesize_0041247c)();
  return;
}



/* ---- strncpy @ 00400ae0 ----
 * (library/support function; see decompiled body)
 */

void strncpy(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400aec. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strncpy_00412480)();
  return;
}



/* ---- write @ 00400af0 ----
 * (library/support function; see decompiled body)
 */

void write(void)

{
  (*(code *)PTR_write_00412484)();
  return;
}



/* ---- listen @ 00400b00 ----
 * (library/support function; see decompiled body)
 */

void listen(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b0c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_listen_00412488)();
  return;
}



/* ---- setsid @ 00400b10 ----
 * (library/support function; see decompiled body)
 */

void setsid(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b1c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_setsid_0041248c)();
  return;
}



/* ---- __libc_start_main @ 00400b20 ----
 * (library/support function; see decompiled body)
 */

void __libc_start_main(void)

{
  (*(code *)PTR___libc_start_main_00412490)();
  return;
}



/* ---- _exit @ 00400b30 ----
 * (library/support function; see decompiled body)
 */

void _exit(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b3c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR__exit_00412494)();
  return;
}



/* ---- read @ 00400b40 ----
 * (library/support function; see decompiled body)
 */

void read(void)

{
  (*(code *)PTR_read_00412498)();
  return;
}



/* ---- free @ 00400b50 ----
 * (library/support function; see decompiled body)
 */

void free(void)

{
  (*(code *)PTR_free_0041249c)();
  return;
}



/* ---- sigaction @ 00400b60 ----
 * (library/support function; see decompiled body)
 */

void sigaction(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b6c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_sigaction_004124a0)();
  return;
}



/* ---- accept @ 00400b70 ----
 * (library/support function; see decompiled body)
 */

void accept(void)

{
  (*(code *)PTR_accept_004124a4)();
  return;
}



/* ---- socket @ 00400b80 ----
 * (library/support function; see decompiled body)
 */

void socket(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b8c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_socket_004124a8)();
  return;
}



/* ---- umask @ 00400b90 ----
 * (library/support function; see decompiled body)
 */

void umask(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400b9c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_umask_004124ac)();
  return;
}



/* ---- dup @ 00400ba0 ----
 * (library/support function; see decompiled body)
 */

void dup(void)

{
  (*(code *)PTR_dup_004124b0)();
  return;
}



/* ---- memcpy @ 00400bb0 ----
 * (library/support function; see decompiled body)
 */

void memcpy(void)

{
  (*(code *)PTR_memcpy_004124b4)();
  return;
}



/* ---- strlen @ 00400bc0 ----
 * (library/support function; see decompiled body)
 */

void strlen(void)

{
  (*(code *)PTR_strlen_004124b8)();
  return;
}



/* ---- getppid @ 00400bd0 ----
 * (library/support function; see decompiled body)
 */

void getppid(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400bdc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_getppid_004124bc)();
  return;
}



/* ---- strcpy @ 00400be0 ----
 * (library/support function; see decompiled body)
 */

void strcpy(void)

{
  (*(code *)PTR_strcpy_004124c0)();
  return;
}



/* ---- chdir @ 00400bf0 ----
 * (library/support function; see decompiled body)
 */

void chdir(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400bfc. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_chdir_004124c4)();
  return;
}



/* ---- bind @ 00400c00 ----
 * (library/support function; see decompiled body)
 */

void bind(void)

{
  (*(code *)PTR_bind_004124c8)();
  return;
}



/* ---- atoi @ 00400c10 ----
 * (library/support function; see decompiled body)
 */

void atoi(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c1c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_atoi_004124cc)();
  return;
}



/* ---- close @ 00400c20 ----
 * (library/support function; see decompiled body)
 */

void close(void)

{
  (*(code *)PTR_close_004124d0)();
  return;
}



/* ---- malloc @ 00400c30 ----
 * (library/support function; see decompiled body)
 */

void malloc(void)

{
  (*(code *)PTR_malloc_004124d4)();
  return;
}



/* ---- fork @ 00400c40 ----
 * (library/support function; see decompiled body)
 */

void fork(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c4c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fork_004124d8)();
  return;
}



/* ---- bzero @ 00400c50 ----
 * (library/support function; see decompiled body)
 */

void bzero(void)

{
  (*(code *)PTR_bzero_004124dc)();
  return;
}



/* ---- strncmp @ 00400c60 ----
 * (library/support function; see decompiled body)
 */

void strncmp(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c6c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_strncmp_004124e0)();
  return;
}



/* ---- strcmp @ 00400c70 ----
 * (library/support function; see decompiled body)
 */

void strcmp(void)

{
  (*(code *)PTR_strcmp_004124e4)();
  return;
}



/* ---- exit @ 00400c80 ----
 * (library/support function; see decompiled body)
 */

void exit(void)

{
                    /* WARNING: Could not recover jumptable at 0x00400c8c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_exit_004124e8)();
  return;
}



/* ---- pthread_create @ 00400c90 ----
 * (library/support function; see decompiled body)
 */

void pthread_create(void)

{
  (*(code *)PTR_pthread_create_004124ec)();
  return;
}



/* ---- __start @ 00400cb0 ----
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



/* ---- __do_global_dtors_aux @ 00400d00 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400d54) */
/* WARNING: Removing unreachable block (ram,0x00400d58) */
/* WARNING: Removing unreachable block (ram,0x00400d7c) */

void __do_global_dtors_aux(void)

{
  if (completed_5578 == '\0') {
    completed_5578 = '\x01';
  }
  return;
}



/* ---- frame_dummy @ 00400da0 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x00400dbc) */

void frame_dummy(void)

{
  return;
}



/* ---- daemonize @ 00400dd0 ----
 * Standard double-fork/setsid/dup to detach from the controlling terminal.
 */

void daemonize(void)

{
  int iVar1;
  undefined4 uVar2;
  int local_10;
  
  iVar1 = getppid();
  if (iVar1 != 1) {
    iVar1 = fork();
    if (iVar1 < 0) {
      exit(1);
    }
    if (0 < iVar1) {
      exit(0);
    }
    setsid();
    for (local_10 = getdtablesize(); -1 < local_10; local_10 = local_10 + -1) {
      close(local_10);
    }
    uVar2 = open("/dev/null",2);
    dup(uVar2);
    dup(uVar2);
    getpid();
  }
  return;
}



/* ---- SignalHandler @ 00400f1c ----
 * On SIGINT/SIGTERM: clean up and exit.
 */

void SignalHandler(int param_1)

{
  if (param_1 == 2) {
    _exit(1);
  }
  if (param_1 == 0xf) {
    _exit(1);
  }
  return;
}



/* ---- mainloop @ 00400f98 ----
 * Open the kernel uevent/udev netlink-style socket, announce the
 * notify_socket name, then loop: receive a uevent message, and on an
 * 'add'/'remove' match update the device slots and notify_apps().
 */

undefined4 mainloop(void)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint local_280;
  uint local_27c;
  int local_274;
  int local_270;
  char acStack_259 [129];
  int local_1d8;
  undefined4 local_1d4;
  undefined1 auStack_1d0 [112];
  undefined2 local_160;
  undefined1 auStack_15d [189];
  undefined1 auStack_a0 [128];
  int local_20;
  int local_1c;
  
  udev_socket_4087 = socket(1,2,0);
  if (-1 < udev_socket_4087) {
    bzero(&local_160,0x6e);
    local_160 = 1;
    strcpy(auStack_15d,notify_socket);
    iVar1 = strlen(auStack_15d);
    iVar1 = bind(udev_socket_4087,&local_160,iVar1 + 3);
    if (-1 < iVar1) {
      do {
        do {
          listen(udev_socket_4087,5);
          local_1d4 = 0x6e;
          iVar1 = accept(udev_socket_4087,auStack_1d0,&local_1d4);
        } while (iVar1 < 0);
        read(iVar1,acStack_259 + 1,0x84);
        iVar2 = strncmp(acStack_259 + 1,&DAT_004023e4,2);
        if (iVar2 == 0) {
          notify_apps(1,acStack_259 + 1);
          local_1c = local_1d8;
          if (local_1d8 == 1) {
            local_270 = 0;
            while ((local_270 < 4 && (*(int *)(udev_status + local_270 * 0x84 + 0x80) != 2))) {
              local_270 = local_270 + 1;
            }
            if (local_270 < 4) {
              *(undefined4 *)(udev_status + local_270 * 0x84 + 0x80) = 1;
              strcpy(udev_status + local_270 * 0x84,acStack_259 + 1);
            }
          }
          else if (local_1d8 == 2) {
            local_274 = 0;
            while ((local_274 < 4 &&
                   (iVar2 = strcmp(udev_status + local_274 * 0x84,acStack_259 + 1), iVar2 != 0
                   ))) {
              local_274 = local_274 + 1;
            }
            if (local_274 < 4) {
              *(undefined4 *)(udev_status + local_274 * 0x84 + 0x80) = 2;
            }
          }
        }
        else {
          iVar2 = strncmp(acStack_259 + 1,&DAT_004023e8,2);
          if (iVar2 == 0) {
            notify_apps(2,acStack_259 + 1);
            uVar3 = strlen(acStack_259 + 1);
            do {
              local_280 = uVar3;
              if (acStack_259[local_280] + -0x30 < 0) break;
              uVar3 = local_280 - 1;
            } while (acStack_259[local_280] + -0x30 < 10);
            iVar2 = strlen(acStack_259 + 1);
            if (local_280 - 1 < iVar2 - 1U) {
              local_27c = atoi(acStack_259 + local_280 + 1);
              if (0x1e < local_27c) {
                local_27c = 0x1f;
              }
              strncpy(auStack_a0,acStack_259 + 1,local_280);
            }
            else {
              strcpy(auStack_a0,acStack_259 + 1);
              local_27c = 0;
            }
            local_20 = local_1d8;
            if (local_1d8 == 1) {
              if (local_27c == 0) {
                local_280 = 0;
                while (((int)local_280 < 8 &&
                       (*(int *)(udev_status + local_280 * 0x84 + 0x290) != 0))) {
                  local_280 = local_280 + 1;
                }
                if ((int)local_280 < 8) {
                  *(uint *)(udev_status + local_280 * 0x84 + 0x290) =
                       *(uint *)(udev_status + local_280 * 0x84 + 0x290) | 1;
                  strcpy(local_280 * 0x84 + 0x412768,acStack_259 + 1);
                }
              }
              else {
                local_280 = 0;
                while (((int)local_280 < 8 &&
                       (iVar2 = strcmp(local_280 * 0x84 + 0x412768,auStack_a0), iVar2 != 0)))
                {
                  local_280 = local_280 + 1;
                }
                if ((int)local_280 < 8) {
                  *(uint *)(udev_status + local_280 * 0x84 + 0x290) =
                       *(uint *)(udev_status + local_280 * 0x84 + 0x290) | 1 << (local_27c & 0x1f);
                }
              }
            }
            else if (local_1d8 == 2) {
              local_280 = 0;
              while (((int)local_280 < 8 &&
                     (iVar2 = strcmp(local_280 * 0x84 + 0x412768,auStack_a0), iVar2 != 0))) {
                local_280 = local_280 + 1;
              }
              if ((int)local_280 < 8) {
                if (local_27c == 0) {
                  *(undefined4 *)(udev_status + local_280 * 0x84 + 0x290) = 0;
                }
                else {
                  *(uint *)(udev_status + local_280 * 0x84 + 0x290) =
                       *(uint *)(udev_status + local_280 * 0x84 + 0x290) &
                       ~(1 << (local_27c & 0x1f));
                }
              }
            }
          }
          else {
            notify_apps(2,acStack_259 + 1);
          }
        }
        close(iVar1);
      } while( true );
    }
  }
  return 0xffffffff;
}



/* ---- main @ 004017f0 ----
 * Daemon entry. umask(0x17), chdir(/var/log), daemonize(), install
 * SignalHandler for SIGINT(2)/SIGTERM(15), initialize the device-slot table
 * (udev_status: 4 slots), spawn reg_thread (pthread) to accept client
 * registrations, then run mainloop().
 */

undefined4 main(void)

{
  int local_a0;
  undefined4 local_9c;
  code *local_98;
  undefined1 auStack_94 [140];
  
  umask(0x17);
  chdir("/var/log");
  daemonize();
  sigemptyset(auStack_94);
  local_9c = 8;
  local_98 = SignalHandler;
  sigaction(2,&local_9c,0);
  sigaction(0xf,&local_9c,0);
  for (local_a0 = 0; local_a0 < 4; local_a0 = local_a0 + 1) {
    *(undefined4 *)(udev_status + local_a0 * 0x84 + 0x80) = 2;
  }
  for (local_a0 = 0; local_a0 < 8; local_a0 = local_a0 + 1) {
    *(undefined4 *)(udev_status + local_a0 * 0x84 + 0x290) = 0;
  }
  pthread_create(&reg_th_4245,0,reg_thread,udev_status);
  mainloop();
  return 0;
}



/* ---- add_app_to_list @ 00401990 ----
 * Append a registered client to the notify list.
 */

void add_app_to_list(undefined4 param_1)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  undefined4 *local_10;
  
  if (head == (undefined4 *)0x0) {
    head = (undefined4 *)malloc(0xc);
    *head = param_1;
    head[1] = 0;
    head[2] = 0;
  }
  else {
    for (local_10 = head; local_10[2] != 0; local_10 = (undefined4 *)local_10[2]) {
    }
    uVar1 = malloc(0xc);
    local_10[2] = uVar1;
    *(undefined4 **)(local_10[2] + 4) = local_10;
    puVar2 = (undefined4 *)local_10[2];
    *puVar2 = param_1;
    puVar2[2] = 0;
  }
  return;
}



/* ---- remove_app_from_list @ 00401a9c ----
 * Drop a client (on disconnect).
 */

undefined4 remove_app_from_list(int param_1)

{
  int *piVar1;
  int *local_18;
  undefined4 local_10;
  
  piVar1 = head;
  local_18 = head;
  if (head == (int *)0x0) {
    local_10 = 0xffffffff;
  }
  else if (*(int *)(*head + 0x80) == param_1) {
    if (head[2] != 0) {
      *(undefined4 *)(head[2] + 4) = 0;
    }
    head = (int *)head[2];
    free(*piVar1);
    free(piVar1);
    local_10 = 0;
  }
  else {
    for (; local_18 != (int *)0x0; local_18 = (int *)local_18[2]) {
      if (*(int *)(*local_18 + 0x80) == param_1) {
        *(int *)(local_18[1] + 8) = local_18[2];
        if (local_18[2] != 0) {
          *(int *)(local_18[2] + 4) = local_18[1];
        }
        free(*local_18);
        free(local_18);
        return 0;
      }
    }
    local_10 = 0xffffffff;
  }
  return local_10;
}



/* ---- delete_list @ 00401c34 ----
 * Free the whole client list.
 */

void delete_list(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  
  while (puVar1 = head, head != (undefined4 *)0x0) {
    uVar2 = *head;
    head = (undefined4 *)head[2];
    free(uVar2);
    free(puVar1);
  }
  return;
}



/* ---- print_list @ 00401cc0 ----
 * Debug dump of the client list.
 */

void print_list(void)

{
  int local_10;
  
  for (local_10 = head; local_10 != 0; local_10 = *(int *)(local_10 + 8)) {
  }
  return;
}



/* ---- create_new_id @ 00401d0c ----
 * Allocate a unique id for a registering client.
 */

int create_new_id(void)

{
  bool bVar1;
  int local_14;
  int *local_10;
  
  local_14 = 0;
  bVar1 = false;
  do {
    if (bVar1) {
      return local_14;
    }
    bVar1 = true;
    for (local_10 = head; local_10 != (int *)0x0; local_10 = (int *)local_10[2]) {
      if (*(int *)(*local_10 + 0x80) == local_14) {
        local_14 = local_14 + 1;
        bVar1 = false;
        break;
      }
    }
  } while( true );
}



/* ---- reg_thread @ 00401db8 ----
 * Thread that listens on the UNIX domain registration socket (socket/bind/
 * listen/accept) so client apps (uevent_client) can subscribe; add_app_to_list
 * on connect.
 */

undefined4 reg_thread(undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 local_7ac;
  undefined1 auStack_7a8 [112];
  undefined2 local_738;
  undefined1 auStack_735 [109];
  undefined1 auStack_6c8 [128];
  int local_648;
  undefined1 auStack_640 [1592];
  
  udev_socket_4167 = socket(1,2,0);
  if (-1 < udev_socket_4167) {
    bzero(&local_738,0x6e);
    local_738 = 1;
    memcpy(auStack_735,"/xcodesoc/uevent_reg",0x15);
    iVar1 = strlen(auStack_735);
    iVar1 = bind(udev_socket_4167,&local_738,iVar1 + 3);
    if (-1 < iVar1) {
      do {
        do {
          listen(udev_socket_4167,5);
          local_7ac = 0x6e;
          iVar1 = accept(udev_socket_4167,auStack_7a8,&local_7ac);
        } while (iVar1 < 0);
        uVar2 = read(iVar1,auStack_6c8,0x6b8);
        if (local_648 == -1) {
          local_648 = create_new_id();
          memcpy(auStack_640,param_1,0x630);
          uVar3 = malloc(0x6b8);
          memcpy(uVar3,auStack_6c8,0x6b8,param_4,uVar3,uVar2);
          add_app_to_list(uVar3);
        }
        else {
          iVar4 = remove_app_from_list(local_648);
          if (iVar4 == 0) {
            local_648 = -1;
          }
        }
        write(iVar1,auStack_6c8,0x6b8);
        close(iVar1);
      } while( true );
    }
  }
  return 0;
}



/* ---- send_msg @ 0040204c ----
 * write() one message to a client fd.
 */

undefined4 send_msg(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int iVar2;
  undefined4 local_80;
  undefined2 local_78;
  undefined1 auStack_75 [109];
  
  local_80 = 0;
  bzero(&local_78,0x6e);
  local_78 = 1;
  sprintf(auStack_75,"%s_%d","/xcodesoc/uevent_notify",param_1);
  iVar1 = strlen(auStack_75);
  iVar2 = socket(1,2,0);
  if (iVar2 == -1) {
    local_80 = 0xffffffff;
  }
  else {
    iVar1 = connect(iVar2,&local_78,iVar1 + 3);
    if (iVar1 < 0) {
      local_80 = 0xffffffff;
    }
    else {
      write(iVar2,param_2,0x84);
    }
    if (-1 < iVar2) {
      close(iVar2);
    }
  }
  return local_80;
}



/* ---- notify_apps @ 004021b8 ----
 * Send the current event (type + device string) to every registered client
 * over its socket (send_msg).
 */

void notify_apps(uint param_1,undefined4 param_2)

{
  int *local_10;
  
  for (local_10 = head; local_10 != (int *)0x0; local_10 = (int *)local_10[2]) {
    if ((*(uint *)(*local_10 + 0x84) & param_1) != 0) {
      send_msg(*(undefined4 *)(*local_10 + 0x80),param_2);
    }
  }
  return;
}



/* ---- __libc_csu_fini @ 00402250 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}



/* ---- __libc_csu_init @ 00402258 ----
 * (library/support function; see decompiled body)
 */

/* WARNING: Removing unreachable block (ram,0x004022b8) */
/* WARNING: Removing unreachable block (ram,0x004022bc) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  (*(code *)PTR__init_00412530)();
                    /* Unresolved local var: size_t i@[???] */
  return;
}



/* ---- __do_global_ctors_aux @ 00402310 ----
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



/* ---- _fini @ 00402370 ----
 * (library/support function; see decompiled body)
 */

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


