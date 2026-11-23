/*
 * 17_stat64.c.c  --  glibc LFS stat64() wrapper (stat64.c)
 * ========================================================
 *
 * Linked from libc_nonshared (stat64.c). Translates stat64() calls into
 * __xstat64(_STAT_VER=3, file, buf) in libc.so.6. Called by main() when
 * checking the size of a -d data-insertion file.
 */

/*
 * stat64 @ 0041f490
 *
 * Wrapper around __xstat64(3, file, buf) (3 = _STAT_VER_LINUX on MIPS).
 * Returns 0 on success, -1 on error (errno set).
 */
// ==== stat64 @ 0041f490 ====

int stat64(char *file,stat64 *buf)

{
  int iVar1;
  
  iVar1 = __xstat64(3,file,(stat64 *)buf);
  return iVar1;
}


