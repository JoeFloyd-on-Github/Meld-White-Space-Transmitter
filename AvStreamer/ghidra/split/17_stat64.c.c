// ==== stat64 @ 0041f490 ====

int stat64(char *file,stat64 *buf)

{
  int iVar1;
  
  iVar1 = __xstat64(3,file,(stat64 *)buf);
  return iVar1;
}



