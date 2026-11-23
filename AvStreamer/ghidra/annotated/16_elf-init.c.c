/*
 * 16_elf-init.c.c  --  glibc ELF startup/teardown stubs (elf-init.c)
 * ==================================================================
 *
 * Linked from glibc's crt / libc_nonshared (elf-init.c). Provides
 * __libc_csu_init and __libc_csu_fini, which __libc_start_main calls
 * around main() to run .init / .init_array and .fini / .fini_array.
 */

/*
 * __libc_csu_init @ 0041f3e0
 *
 * Called by __libc_start_main before main(argc, argv, envp).
 * Invokes _init() (which runs __gmon_start__, frame_dummy, and
 * __do_global_ctors_aux for C++ static constructors). The .init_array loop
 * was either empty or unreachable/elided by Ghidra (note the warning).
 *
 * Parameters: argc, argv, envp (forwarded from __start / __libc_start_main).
 */
// ==== __libc_csu_init @ 0041f3e0 ====

/* WARNING: Removing unreachable block (ram,0x0041f440) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  _init((EVP_PKEY_CTX *)argc);
                    /* Unresolved local var: size_t i@[???] */
  return;
}



/*
 * __libc_csu_fini @ 0041f488
 *
 * Registered as the fini callback with __libc_start_main. No-op here;
 * _fini() is called separately by the dynamic linker / exit path.
 */
// ==== __libc_csu_fini @ 0041f488 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}


