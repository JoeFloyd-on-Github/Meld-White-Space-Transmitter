// ==== _init @ 00400658

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


// ==== FUN_004006e0 @ 004006e0

void FUN_004006e0(void)

{
  (*(code *)PTR_connect_00410ed8)();
  return;
}


// ==== FUN_004006f0 @ 004006f0

void FUN_004006f0(void)

{
  (*(code *)PTR_getenv_00410edc)();
  return;
}


// ==== FUN_00400700 @ 00400700

void FUN_00400700(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040070c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_write_00410ee0)();
  return;
}


// ==== FUN_00400710 @ 00400710

void FUN_00400710(void)

{
  (*(code *)PTR___libc_start_main_00410ee4)();
  return;
}


// ==== FUN_00400720 @ 00400720

void FUN_00400720(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040072c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_perror_00410ee8)();
  return;
}


// ==== FUN_00400730 @ 00400730

void FUN_00400730(void)

{
  (*(code *)PTR_socket_00410eec)();
  return;
}


// ==== FUN_00400740 @ 00400740

void FUN_00400740(void)

{
  (*(code *)PTR_strlen_00410ef0)();
  return;
}


// ==== FUN_00400750 @ 00400750

void FUN_00400750(void)

{
  (*(code *)PTR_strcpy_00410ef4)();
  return;
}


// ==== FUN_00400760 @ 00400760

void FUN_00400760(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040076c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_close_00410ef8)();
  return;
}


// ==== FUN_00400770 @ 00400770

void FUN_00400770(void)

{
                    /* WARNING: Could not recover jumptable at 0x0040077c. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)PTR_fprintf_00410efc)();
  return;
}


// ==== FUN_00400780 @ 00400780

void FUN_00400780(void)

{
  (*(code *)PTR_bzero_00410f00)();
  return;
}


// ==== FUN_00400790 @ 00400790

void FUN_00400790(void)

{
  (*(code *)PTR_strcmp_00410f04)();
  return;
}


// ==== __start @ 004007b0

void processEntry __start(undefined4 param_1,undefined4 param_2,undefined1 param_3)

{
  undefined1 auStack_20 [16];
  code *local_10;
  undefined4 local_c;
  undefined1 *local_8;
  
  local_8 = auStack_20;
  local_10 = __libc_csu_fini;
  local_c = param_1;
  FUN_00400710(main,param_2,&param_3,__libc_csu_init);
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ==== __do_global_dtors_aux @ 00400800

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


// ==== frame_dummy @ 004008a0

/* WARNING: Removing unreachable block (ram,0x004008bc) */

void frame_dummy(void)

{
  return;
}


// ==== main @ 004008d0

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
  iVar1 = FUN_004006f0("TARGETDEV");
  if (iVar1 == 0) {
    FUN_00400720("No ENV TARGETDEV");
  }
  else {
    FUN_00400750(auStack_fc,iVar1);
    iVar2 = FUN_004006f0("ACTION");
    if (iVar2 == 0) {
      FUN_00400720("Not ENV ACTION\n");
    }
    else {
      iVar3 = FUN_00400790(iVar2,&DAT_00400e20);
      if (iVar3 == 0) {
        FUN_00400770(stderr,"Add device %s\n",iVar1);
        local_7c = 1;
      }
      else {
        iVar3 = FUN_00400790(iVar2,"remove");
        if (iVar3 == 0) {
          FUN_00400770(stderr,"Remove device %s\n",iVar1);
          local_7c = 2;
        }
        else {
          iVar3 = FUN_00400790(iVar2,"change");
          if (iVar3 != 0) {
            FUN_00400770(stderr,"device %s, action %s - ",iVar1,iVar2);
            FUN_00400720("Not supported action");
            goto LAB_00400c20;
          }
          FUN_00400770(stderr,"Change device %s\n",iVar1);
          local_7c = 3;
        }
      }
      FUN_00400780(&local_78,0x6e);
      local_78 = 1;
      FUN_00400750(auStack_75,notify_socket);
      iVar1 = FUN_00400740(auStack_75);
      local_100 = FUN_00400730(1,2,0);
      if (local_100 != -1) {
        iVar1 = FUN_004006e0(local_100,&local_78,iVar1 + 3);
        if (iVar1 < 0) {
          FUN_00400720("Connecting failed");
        }
        else {
          FUN_00400700(local_100,auStack_fc,0x84);
        }
      }
    }
  }
LAB_00400c20:
  if (-1 < local_100) {
    FUN_00400760(local_100);
  }
  return 0;
}


// ==== __libc_csu_fini @ 00400c60

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}


// ==== __libc_csu_init @ 00400c68

/* WARNING: Removing unreachable block (ram,0x00400cc8) */
/* WARNING: Removing unreachable block (ram,0x00400ccc) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  (*(code *)PTR__init_00410f50)();
                    /* Unresolved local var: size_t i@[???] */
  return;
}


// ==== __do_global_ctors_aux @ 00400d20

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


// ==== _fini @ 00400d80

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


