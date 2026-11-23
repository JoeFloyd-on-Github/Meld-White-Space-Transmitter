// ==== _init @ 004022e8 ====

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  __gmon_start__();
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



// ==== __start @ 00402360 ====

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



// ==== __do_global_dtors_aux @ 004023c0 ====

/* WARNING: Removing unreachable block (ram,0x00402414) */

void __do_global_dtors_aux(void)

{
  if (completed_5354 == '\0') {
    completed_5354 = '\x01';
  }
  return;
}



// ==== frame_dummy @ 0040245c ====

void frame_dummy(void)

{
  if (__JCR_END__ == 0) {
    return;
  }
  _Jv_RegisterClasses(&__JCR_END__);
  return;
}



// ==== __do_global_ctors_aux @ 0041f4c0 ====

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



// ==== _fini @ 0041fa00 ====

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


