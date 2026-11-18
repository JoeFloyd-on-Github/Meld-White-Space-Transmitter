/*
 * wsa_decompiled.c  --  RAW Ghidra decompiler output for WhiteSpaceApp/wsa
 * =======================================================================
 *
 * This file is the UNEDITED decompilation of the 127 application + ezxml
 * functions in the wsa binary, exactly as produced by the Ghidra 12.2
 * decompiler. It is checked in for reference and diffing only.
 *
 * How it was generated:
 *   - Imported WhiteSpaceApp/wsa (ELF32 MIPS32r2, not stripped, full DWARF)
 *     into Ghidra 12.2 headless with auto-analysis.
 *   - Ran the p-code decompiler over every function in 0x400290..0x40f800
 *     (application code + the bundled ezxml parser). libc/OpenSSL code
 *     beyond that range is not included.
 *   - Function names, parameter names and most struct/enum types come from
 *     the binary's own DWARF debug info, so they are the ORIGINAL symbol
 *     names, not Ghidra guesses. LOCAL variables, however, were mostly not
 *     in the debug info, so Ghidra auto-named them (iVar1, local_10,
 *     acStack_90, ...). See wsa_annotated.c and README.md for those.
 *
 * For a human-readable walkthrough with the local variables explained and
 * the ezxml usage described, read wsa_annotated.c alongside this file.
 *
 * Nothing here was produced by running the binary.
 */

// ==== usage @ 00400290

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void usage(void)

{
  fwrite("USAGE: whitespaceagent certname certpath xmldeviceinfofilename\n",1,0x3f,(FILE *)stderr);
  return;
}


// ==== signalAgentToProcess @ 004002e4

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void signalAgentToProcess(void)

{
  pthread_mutex_lock((pthread_mutex_t *)&agent_mutex);
  pthread_cond_signal((pthread_cond_t *)&agent_cond);
  pthread_mutex_unlock((pthread_mutex_t *)&agent_mutex);
  return;
}


// ==== whiteSpaceStateMachine @ 00400354

void * whiteSpaceStateMachine(void *threadData)

{
                    /* Unresolved local var: InterThreadData * data@[???] */
  pthread_mutex_lock((pthread_mutex_t *)&agent_mutex);
  pthread_cond_wait((pthread_cond_t *)&agent_cond,(pthread_mutex_t *)&agent_mutex);
  pthread_mutex_unlock((pthread_mutex_t *)&agent_mutex);
  initProcessStateFunctions();
  do {
    processWSStateMachine
              (*(WhiteSpaceContext **)((int)threadData + 8),threadData,(SSL **)((int)threadData + 4)
               ,*(pthread_mutex_t **)((int)threadData + 0xc));
  } while( true );
}


// ==== open_port_Read @ 0040040c

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int open_port_Read(void)

{
  int __fd;
  undefined1 auStack_40 [56];
  
                    /* Unresolved local var: int fd@[???]
                       Unresolved local var: speed_t baud@[???] */
  __fd = open("/dev/ttyS0",0);
  if (__fd == -1) {
    perror("open_port: Unable to open /dev/ttyS0 - ");
  }
  else {
                    /* Unresolved local var: termios settings@[???] */
    tcgetattr(__fd,(termios *)auStack_40);
    cfsetospeed((termios *)auStack_40,0x1002);
    tcsetattr(__fd,0x540e,(termios *)auStack_40);
    tcflush(__fd,1);
    fcntl(__fd,4);
  }
  return __fd;
}


// ==== main @ 0040051c

int main(int argc,char **argv)

{
  int iVar1;
  WhiteSpaceContext WStack_1488;
  pthread_t local_18;
  pthread_t local_14;
  int local_10;
  
                    /* Unresolved local var: SSL_CTX * sslCtx@[???]
                       Unresolved local var: SSL * ssl@[???]
                       Unresolved local var: WhiteSpaceContext wsCtx@[???]
                       Unresolved local var: int condition@[???]
                       Unresolved local var: int rc@[???]
                       Unresolved local var: pthread_t[2] threads@[???] */
  initializeSecurityCtx();
  initializeWhiteSpaceContext(&WStack_1488);
  if ((argc == 2) && (iVar1 = strcmp(argv[1],"-v"), iVar1 == 0)) {
    printf("whitespaceagent Version:%s Build Date:%s\n","44","Fri Nov 22 16:35:57 2013");
    local_10 = 0;
  }
  else {
    if (argc < 3) {
      usage();
                    /* WARNING: Subroutine does not return */
      exit(2);
    }
    if (1 < argc) {
      WStack_1488.securityCertFileName = argv[1];
    }
    if (argc < 3) {
      WStack_1488.securityCertPath = (char *)0x0;
    }
    else {
      WStack_1488.securityCertPath = argv[2];
    }
    if (3 < argc) {
      readXmlIntoContext(argv[3],&WStack_1488);
    }
    whitespaceThreadData.sslCtx = (SSL_CTX *)0x0;
    whitespaceThreadData.ssl = (SSL *)0x0;
    whitespaceThreadData.wsCtx = &WStack_1488;
    whitespaceThreadData.whiteSpaceMutex = &whitespace_mutex;
    iVar1 = pthread_create(&local_18,(pthread_attr_t *)0x0,whiteSpaceStateMachine,
                           &whitespaceThreadData);
    if (iVar1 == 0) {
      iVar1 = pthread_create(&local_14,(pthread_attr_t *)0x0,serialProcessorThread,
                             &whitespaceThreadData);
      if (iVar1 == 0) {
        pthread_join(local_18,(void **)0x0);
        pthread_join(local_14,(void **)0x0);
        local_10 = 0;
      }
      else {
        puts("Unable to create a SerialServer Thread ");
        local_10 = 1;
      }
    }
    else {
      local_10 = 1;
    }
  }
  return local_10;
}


// ==== ezxml_child @ 004007e0

ezxml_t_conflict ezxml_child(ezxml_t_conflict xml,char *name)

{
  int iVar1;
  ezxml_t_conflict local_res0;
  ezxml_t_conflict local_10;
  
  if (xml == (ezxml_t_conflict)0x0) {
    local_10 = (ezxml_t_conflict)0x0;
  }
  else {
    local_10 = xml->child;
  }
  local_res0 = local_10;
  while ((local_res0 != (ezxml_t_conflict)0x0 && (iVar1 = strcmp(name,local_res0->name), iVar1 != 0)
         )) {
    local_res0 = local_res0->sibling;
  }
  return local_res0;
}


// ==== ezxml_idx @ 00400888

ezxml_t_conflict ezxml_idx(ezxml_t_conflict xml,int idx)

{
  ezxml_t_conflict local_res0;
  int local_res4;
  
  local_res0 = xml;
  for (local_res4 = idx; (local_res0 != (ezxml_t_conflict)0x0 && (local_res4 != 0));
      local_res4 = local_res4 + -1) {
    local_res0 = local_res0->next;
  }
  return local_res0;
}


// ==== ezxml_attr @ 004008ec

char * ezxml_attr(ezxml_t_conflict xml,char *attr)

{
  int iVar1;
  ezxml_t_conflict local_20;
  int local_1c;
  int local_18;
  char *local_10;
  char *local_c;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: ezxml_root_t root@[???] */
  local_18 = 0;
  local_1c = 1;
  if ((xml == (ezxml_t_conflict)0x0) || (xml->attr == (char **)0x0)) {
    local_c = (char *)0x0;
  }
  else {
    while ((xml->attr[local_18] != (char *)0x0 &&
           (iVar1 = strcmp(attr,xml->attr[local_18]), iVar1 != 0))) {
      local_18 = local_18 + 2;
    }
    local_20 = xml;
    if (xml->attr[local_18] == (char *)0x0) {
      for (; local_20->parent != (ezxml_t)0x0; local_20 = local_20->parent) {
      }
      local_18 = 0;
      while (((&(local_20[1].child)->name)[local_18] != (char *)0x0 &&
             (iVar1 = strcmp(xml->name,*(char **)(&(local_20[1].child)->name)[local_18]), iVar1 != 0
             ))) {
        local_18 = local_18 + 1;
      }
      if ((&(local_20[1].child)->name)[local_18] == (char *)0x0) {
        local_c = (char *)0x0;
      }
      else {
        while ((*(int *)((&(local_20[1].child)->name)[local_18] + local_1c * 4) != 0 &&
               (iVar1 = strcmp(attr,*(char **)((&(local_20[1].child)->name)[local_18] + local_1c * 4
                                              )), iVar1 != 0))) {
          local_1c = local_1c + 3;
        }
        if (*(int *)((&(local_20[1].child)->name)[local_18] + local_1c * 4) == 0) {
          local_10 = (char *)0x0;
        }
        else {
          local_10 = *(char **)((&(local_20[1].child)->name)[local_18] + (local_1c + 1) * 4);
        }
        local_c = local_10;
      }
    }
    else {
      local_c = xml->attr[local_18 + 1];
    }
  }
  return local_c;
}


// ==== ezxml_vget @ 00400bc0

ezxml_t_conflict ezxml_vget(ezxml_t_conflict xml,va_list ap)

{
  int *piVar1;
  char *name;
  ezxml_t_conflict xml_00;
  ezxml_t_conflict local_res0;
  int *local_res4;
  int local_18;
  ezxml_t_conflict local_10;
  
                    /* Unresolved local var: char * name@[???]
                       Unresolved local var: int idx@[???] */
  piVar1 = (int *)((int)ap + 4);
  name = *(char **)ap;
  local_18 = -1;
  local_res0 = xml;
  local_res4 = piVar1;
  if ((name != (char *)0x0) && (*name != '\0')) {
    local_res4 = (int *)((int)ap + 8);
    local_18 = *piVar1;
    local_res0 = ezxml_child(xml,name);
  }
  if (local_18 < 0) {
    local_10 = local_res0;
  }
  else {
    xml_00 = ezxml_idx(local_res0,local_18);
    local_10 = ezxml_vget(xml_00,local_res4);
  }
  return local_10;
}


// ==== ezxml_get @ 00400c9c

ezxml_t_conflict ezxml_get(ezxml_t_conflict xml,...)

{
  ezxml_t_conflict peVar1;
  undefined4 in_a1;
  undefined4 in_a2;
  undefined4 in_a3;
  undefined4 local_res4;
  undefined4 local_res8;
  undefined4 local_resc;
  
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: ezxml_t.conflict r@[???] */
  local_res4 = in_a1;
  local_res8 = in_a2;
  local_resc = in_a3;
  peVar1 = ezxml_vget(xml,&local_res4);
  return peVar1;
}


// ==== ezxml_pi @ 00400cf8

char ** ezxml_pi(ezxml_t_conflict xml,char *target)

{
  int iVar1;
  int local_18;
  ezxml_t_conflict local_14;
  char **local_10;
  char **local_c;
  
                    /* Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: int i@[???] */
  local_18 = 0;
  local_14 = xml;
  if (xml == (ezxml_t_conflict)0x0) {
    local_c = EZXML_NIL;
  }
  else {
    for (; local_14->parent != (ezxml_t)0x0; local_14 = local_14->parent) {
    }
    while (((&(local_14[1].parent)->name)[local_18] != (char *)0x0 &&
           (iVar1 = strcmp(target,*(char **)(&(local_14[1].parent)->name)[local_18]), iVar1 != 0)))
    {
      local_18 = local_18 + 1;
    }
    if ((&(local_14[1].parent)->name)[local_18] == (char *)0x0) {
      local_10 = EZXML_NIL;
    }
    else {
      local_10 = (char **)((&(local_14[1].parent)->name)[local_18] + 4);
    }
    local_c = local_10;
  }
  return local_c;
}


// ==== ezxml_err @ 00400e40

ezxml_t_conflict ezxml_err(ezxml_root_t root,char *s,char *err,...)

{
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_98;
  int local_94;
  char acStack_8c [132];
  
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: int line@[???]
                       Unresolved local var: char * t@[???]
                       Unresolved local var: char[128] fmt@[???] */
  local_94 = 1;
  for (local_98 = root->s; local_98 < s; local_98 = local_98 + 1) {
    if (*local_98 == '\n') {
      local_94 = local_94 + 1;
    }
  }
  local_resc = in_a3;
  snprintf(acStack_8c,0x80,"[error near line %d]: %s",local_94,err);
  vsnprintf(root->err,0x80,acStack_8c,&local_resc);
  return &root->xml;
}


// ==== ezxml_decode @ 00400f3c

char * ezxml_decode(char *s,char **ent,char t)

{
  byte *__s;
  ushort **ppuVar1;
  int iVar2;
  char *pcVar3;
  char *__s_00;
  size_t sVar4;
  size_t sVar5;
  byte *local_res0;
  int local_34;
  int local_30;
  int local_2c;
  char *local_24;
  char *local_20 [2];
  char *local_18;
  
                    /* Unresolved local var: char * e@[???]
                       Unresolved local var: char * r@[???]
                       Unresolved local var: char * m@[???]
                       Unresolved local var: long b@[???]
                       Unresolved local var: long c@[???]
                       Unresolved local var: long d@[???]
                       Unresolved local var: long l@[???] */
  for (local_res0 = (byte *)s; __s = local_res0, *local_res0 != '\0'; local_res0 = local_res0 + 1) {
    while (local_res0 = __s, *local_res0 == '\r') {
      *local_res0 = '\n';
      __s = local_res0 + 1;
      if (*__s == '\n') {
        sVar4 = strlen((char *)__s);
        memmove(__s,local_res0 + 2,sVar4);
      }
    }
  }
  local_res0 = (byte *)s;
  local_24 = s;
  do {
    while( true ) {
      while( true ) {
        while ((((*local_res0 != 0 && (*local_res0 != 0x26)) &&
                ((*local_res0 != 0x25 || (t != '%')))) &&
               (ppuVar1 = __ctype_b_loc(), ((*ppuVar1)[(char)*local_res0] & 0x20) == 0))) {
          local_res0 = local_res0 + 1;
        }
        if (*local_res0 == 0) {
          if (t == '*') {
            for (local_res0 = (byte *)local_24; *local_res0 != '\0'; local_res0 = local_res0 + 1) {
              sVar4 = strspn((char *)local_res0," ");
              if (sVar4 != 0) {
                sVar5 = strlen((char *)(local_res0 + sVar4));
                memmove(local_res0,local_res0 + sVar4,sVar5 + 1);
              }
              for (; (*local_res0 != '\0' && (*local_res0 != ' ')); local_res0 = local_res0 + 1) {
              }
            }
            pcVar3 = (char *)(local_res0 + -1);
            if ((local_24 <= pcVar3) && (*pcVar3 == ' ')) {
              *pcVar3 = '\0';
            }
          }
          return local_24;
        }
        if ((t == 'c') || (iVar2 = strncmp((char *)local_res0,"&#",2), iVar2 != 0)) break;
        if (local_res0[2] == 0x78) {
          local_30 = strtol((char *)(local_res0 + 3),local_20,0x10);
        }
        else {
          local_30 = strtol((char *)(local_res0 + 2),local_20,10);
        }
        if ((local_30 == 0) || (*local_20[0] != ';')) {
          local_res0 = local_res0 + 1;
        }
        else {
          if (local_30 < 0x80) {
            *local_res0 = (byte)local_30;
            local_res0 = local_res0 + 1;
          }
          else {
            local_2c = 0;
            for (local_34 = local_30; local_34 != 0; local_34 = local_34 / 2) {
              local_2c = local_2c + 1;
            }
            local_2c = (local_2c + -2) / 5;
            *local_res0 = (byte)(0xff << (7U - local_2c & 0x1f)) |
                          (byte)(local_30 >> (local_2c * 6 & 0x1fU));
            while (local_res0 = local_res0 + 1, local_2c != 0) {
              local_2c = local_2c + -1;
              *local_res0 = (byte)(local_30 >> (local_2c * 6 & 0x1fU)) & 0x3f | 0x80;
            }
          }
          pcVar3 = strchr((char *)local_res0,0x3b);
          __s_00 = strchr((char *)local_res0,0x3b);
          sVar4 = strlen(__s_00);
          memmove(local_res0,pcVar3 + 1,sVar4);
        }
      }
      if (((*local_res0 == 0x26) && (((t == '&' || (t == ' ')) || (t == '*')))) ||
         ((*local_res0 == 0x25 && (t == '%')))) break;
      if (((t == ' ') || (t == '*')) &&
         (ppuVar1 = __ctype_b_loc(), ((*ppuVar1)[(char)*local_res0] & 0x20) != 0)) {
        *local_res0 = 0x20;
        local_res0 = local_res0 + 1;
      }
      else {
        local_res0 = local_res0 + 1;
      }
    }
    for (local_2c = 0; ent[local_2c] != (char *)0x0; local_2c = local_2c + 2) {
      pcVar3 = ent[local_2c];
      sVar4 = strlen(ent[local_2c]);
      iVar2 = strncmp((char *)(local_res0 + 1),pcVar3,sVar4);
      if (iVar2 == 0) break;
    }
    if (ent[local_2c] == (char *)0x0) {
      local_res0 = local_res0 + 1;
    }
    else {
      sVar4 = strlen(ent[local_2c + 1]);
      local_20[0] = strchr((char *)local_res0,0x3b);
      if ((int)local_20[0] - (int)local_res0 < (int)(sVar4 - 1)) {
        iVar2 = (int)local_res0 - (int)local_24;
        sVar5 = strlen(local_20[0]);
        sVar5 = iVar2 + sVar4 + sVar5;
        if (local_24 == s) {
          pcVar3 = malloc(sVar5);
          local_18 = strcpy(pcVar3,local_24);
        }
        else {
          local_18 = realloc(local_24,sVar5);
        }
        local_24 = local_18;
        local_res0 = (byte *)(local_18 + iVar2);
        local_20[0] = strchr((char *)local_res0,0x3b);
      }
      pcVar3 = local_20[0] + 1;
      sVar5 = strlen(local_20[0]);
      memmove(local_res0 + sVar4,pcVar3,sVar5);
      strncpy((char *)local_res0,ent[local_2c + 1],sVar4);
    }
  } while( true );
}


// ==== ezxml_open_tag @ 00401844

void ezxml_open_tag(ezxml_root_t root,char *name,char **attr)

{
  size_t off;
  ezxml_t_conflict local_10;
  
                    /* Unresolved local var: ezxml_t.conflict xml@[???] */
  local_10 = root->cur;
  if (local_10->name == (char *)0x0) {
    local_10->name = name;
  }
  else {
    off = strlen(local_10->txt);
    local_10 = ezxml_add_child(local_10,name,off);
  }
  local_10->attr = attr;
  root->cur = local_10;
  return;
}


// ==== ezxml_char_content @ 004018fc

void ezxml_char_content(ezxml_root_t root,char *s,size_t len,char t)

{
  ezxml_t_conflict xml;
  char *__s;
  size_t sVar1;
  char *pcVar2;
  size_t local_28;
  char *local_18;
  
                    /* Unresolved local var: ezxml_t.conflict xml@[???]
                       Unresolved local var: char * m@[???]
                       Unresolved local var: size_t l@[???] */
  xml = root->cur;
  if (((xml != (ezxml_t_conflict)0x0) && (xml->name != (char *)0x0)) && (len != 0)) {
    s[len] = '\0';
    __s = ezxml_decode(s,root->ent,t);
    sVar1 = strlen(__s);
    if (*xml->txt == '\0') {
      xml->txt = __s;
    }
    else {
      if ((xml->flags & 0x40U) == 0) {
        local_28 = strlen(xml->txt);
        pcVar2 = malloc(local_28 + sVar1 + 1);
        local_18 = strcpy(pcVar2,xml->txt);
      }
      else {
        pcVar2 = xml->txt;
        local_28 = strlen(xml->txt);
        local_18 = realloc(pcVar2,local_28 + sVar1 + 1);
      }
      xml->txt = local_18;
      strcpy(xml->txt + local_28,__s);
      if (__s != s) {
        free(__s);
      }
    }
    if (xml->txt != s) {
      ezxml_set_flag(xml,0x40);
    }
  }
  return;
}


// ==== ezxml_close_tag @ 00401b50

ezxml_t_conflict ezxml_close_tag(ezxml_root_t root,char *name,char *s)

{
  int iVar1;
  ezxml_t_conflict local_10;
  
  if (((root->cur == (ezxml_t_conflict)0x0) || (root->cur->name == (char *)0x0)) ||
     (iVar1 = strcmp(name,root->cur->name), iVar1 != 0)) {
    local_10 = ezxml_err(root,s,"unexpected closing tag </%s>",name);
  }
  else {
    root->cur = root->cur->parent;
    local_10 = (ezxml_t_conflict)0x0;
  }
  return local_10;
}


// ==== ezxml_ent_ok @ 00401c18

int ezxml_ent_ok(char *name,char *s,char **ent)

{
  size_t sVar1;
  int iVar2;
  char *__s1;
  char *local_res4;
  int local_20;
  
  local_res4 = s;
                    /* Unresolved local var: int i@[???] */
  do {
    for (; (*local_res4 != '\0' && (*local_res4 != '&')); local_res4 = local_res4 + 1) {
    }
    if (*local_res4 == '\0') {
      return 1;
    }
    sVar1 = strlen(name);
    iVar2 = strncmp(local_res4 + 1,name,sVar1);
    if (iVar2 == 0) {
      return 0;
    }
    for (local_20 = 0; ent[local_20] != (char *)0x0; local_20 = local_20 + 2) {
      __s1 = ent[local_20];
      sVar1 = strlen(ent[local_20]);
      iVar2 = strncmp(__s1,local_res4 + 1,sVar1);
      if (iVar2 == 0) break;
    }
    if ((ent[local_20] != (char *)0x0) &&
       (iVar2 = ezxml_ent_ok(name,ent[local_20 + 1],ent), iVar2 == 0)) {
      return 0;
    }
    local_res4 = local_res4 + 1;
  } while( true );
}


// ==== ezxml_proc_inst @ 00401e10

void ezxml_proc_inst(ezxml_root_t root,char *s,size_t len)

{
  size_t sVar1;
  int iVar2;
  char *pcVar3;
  char ***pppcVar4;
  char **ppcVar5;
  char *local_res4;
  int local_24;
  int local_20;
  undefined *local_18;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: char * target@[???] */
  local_20 = 0;
  local_24 = 1;
  s[len] = '\0';
  sVar1 = strcspn(s,"\t\r\n ");
  local_res4 = s + sVar1;
  if (*local_res4 != '\0') {
    *local_res4 = '\0';
    sVar1 = strspn(local_res4 + 1,"\t\r\n ");
    local_res4 = local_res4 + sVar1 + 1;
  }
  iVar2 = strcmp(s,"xml");
  if (iVar2 == 0) {
    pcVar3 = strstr(local_res4,"standalone");
    if (pcVar3 != (char *)0x0) {
      sVar1 = strspn(pcVar3 + 10,"\t\r\n =\'\"");
      iVar2 = strncmp(pcVar3 + sVar1 + 10,"yes",3);
      if (iVar2 == 0) {
        root->standalone = 1;
      }
    }
  }
  else {
    if (*root->pi == (char **)0x0) {
      pppcVar4 = malloc(4);
      root->pi = pppcVar4;
      *root->pi = (char **)0x0;
    }
    while ((root->pi[local_20] != (char **)0x0 &&
           (iVar2 = strcmp(s,*root->pi[local_20]), iVar2 != 0))) {
      local_20 = local_20 + 1;
    }
    if (root->pi[local_20] == (char **)0x0) {
      pppcVar4 = realloc(root->pi,(local_20 + 2) * 4);
      root->pi = pppcVar4;
      pppcVar4 = root->pi;
      ppcVar5 = malloc(0xc);
      pppcVar4[local_20] = ppcVar5;
      *root->pi[local_20] = s;
      root->pi[local_20 + 1] = (char **)0x0;
      root->pi[local_20][1] = (char *)0x0;
      ppcVar5 = root->pi[local_20];
      pcVar3 = strdup("");
      ppcVar5[2] = pcVar3;
    }
    for (; root->pi[local_20][local_24] != (char *)0x0; local_24 = local_24 + 1) {
    }
    pppcVar4 = root->pi;
    ppcVar5 = realloc(root->pi[local_20],(local_24 + 3) * 4);
    pppcVar4[local_20] = ppcVar5;
    ppcVar5 = root->pi[local_20];
    pcVar3 = realloc(root->pi[local_20][local_24 + 1],local_24 + 1);
    ppcVar5[local_24 + 2] = pcVar3;
    if ((root->xml).name == (char *)0x0) {
      local_18 = &DAT_005171b0;
    }
    else {
      local_18 = &DAT_005171ac;
    }
    memcpy(root->pi[local_20][local_24 + 2] + local_24 + -1,local_18,2);
    root->pi[local_20][local_24 + 1] = (char *)0x0;
    root->pi[local_20][local_24] = local_res4;
  }
  return;
}


// ==== ezxml_internal_dtd @ 0040235c

short ezxml_internal_dtd(ezxml_root_t root,char *s,size_t len)

{
  char cVar1;
  void *__dest;
  int iVar2;
  size_t sVar3;
  char *pcVar4;
  char *__s;
  char **ppcVar5;
  size_t len_00;
  char ***pppcVar6;
  char *local_res4;
  int local_50;
  int local_4c;
  char **local_48;
  char *local_40;
  char *local_3c;
  char *local_28;
  char ***local_20;
  char *local_1c;
  char *local_18;
  char **local_14;
  
                    /* Unresolved local var: char q@[???]
                       Unresolved local var: char * c@[???]
                       Unresolved local var: char * t@[???]
                       Unresolved local var: char * n@[???]
                       Unresolved local var: char * v@[???]
                       Unresolved local var: char * * ent@[???]
                       Unresolved local var: char * * pe@[???]
                       Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  local_3c = (char *)0x0;
  __dest = malloc(4);
  local_48 = memcpy(__dest,EZXML_NIL,4);
  s[len] = '\0';
  local_res4 = s;
LAB_004030cc:
  do {
    if (local_res4 == (char *)0x0) goto LAB_004030d8;
    for (; ((*local_res4 != '\0' && (*local_res4 != '<')) && (*local_res4 != '%'));
        local_res4 = local_res4 + 1) {
    }
    if (*local_res4 == '\0') goto LAB_004030d8;
    iVar2 = strncmp(local_res4,"<!ENTITY",8);
    if (iVar2 != 0) {
      iVar2 = strncmp(local_res4,"<!ATTLIST",9);
      if (iVar2 == 0) {
        sVar3 = strspn(local_res4 + 9,"\t\r\n ");
        pcVar4 = local_res4 + sVar3 + 9;
        if (*pcVar4 != '\0') {
          sVar3 = strcspn(pcVar4,"\t\r\n >");
          local_res4 = pcVar4 + sVar3;
          if (*local_res4 != '>') {
            *local_res4 = '\0';
            local_4c = 0;
            while ((root->attr[local_4c] != (char **)0x0 &&
                   (iVar2 = strcmp(local_3c,*root->attr[local_4c]), iVar2 != 0))) {
              local_4c = local_4c + 1;
            }
LAB_00402f08:
            local_res4 = local_res4 + 1;
            sVar3 = strspn(local_res4,"\t\r\n ");
            local_3c = local_res4 + sVar3;
            if ((*local_3c != '\0') && (*local_3c != '>')) {
              sVar3 = strcspn(local_3c,"\t\r\n ");
              local_res4 = local_3c + sVar3;
              if (*local_res4 == '\0') {
                ezxml_err(root,pcVar4,"malformed <!ATTLIST");
              }
              else {
                *local_res4 = '\0';
                sVar3 = strspn(local_res4 + 1,"\t\r\n ");
                local_res4 = local_res4 + sVar3 + 1;
                iVar2 = strncmp(local_res4,"CDATA",5);
                if (iVar2 == 0) {
                  local_18 = " ";
                }
                else {
                  local_18 = "*";
                }
                iVar2 = strncmp(local_res4,"NOTATION",8);
                if (iVar2 == 0) {
                  sVar3 = strspn(local_res4 + 8,"\t\r\n ");
                  local_res4 = local_res4 + sVar3 + 8;
                }
                if (*local_res4 == '(') {
                  local_1c = strchr(local_res4,0x29);
                }
                else {
                  sVar3 = strcspn(local_res4,"\t\r\n ");
                  local_1c = local_res4 + sVar3;
                }
                local_res4 = local_1c;
                if (local_1c == (char *)0x0) {
                  ezxml_err(root,pcVar4,"malformed <!ATTLIST");
                }
                else {
                  sVar3 = strspn(local_1c,"\t\r\n )");
                  local_res4 = local_1c + sVar3;
                  iVar2 = strncmp(local_res4,"#FIXED",6);
                  if (iVar2 == 0) {
                    sVar3 = strspn(local_res4 + 6,"\t\r\n ");
                    local_res4 = local_res4 + sVar3 + 6;
                  }
                  if (*local_res4 == '#') break;
                  if ((*local_res4 == '\"') || (*local_res4 == '\'')) {
                    local_40 = local_res4 + 1;
                    local_res4 = strchr(local_40,(int)*local_res4);
                    if (local_res4 == (char *)0x0) goto LAB_00402c38;
                    *local_res4 = '\0';
                    goto LAB_00402c58;
                  }
LAB_00402c38:
                  ezxml_err(root,pcVar4,"malformed <!ATTLIST");
                }
              }
            }
          }
          goto LAB_004030cc;
        }
        ezxml_err(root,pcVar4,"unclosed <!ATTLIST");
        goto LAB_004030d8;
      }
      iVar2 = strncmp(local_res4,"<!--",4);
      if (iVar2 == 0) {
        local_res4 = strstr(local_res4 + 4,"-->");
      }
      else {
        iVar2 = strncmp(local_res4,"<?",2);
        if (iVar2 == 0) {
          pcVar4 = local_res4 + 2;
          local_res4 = strstr(pcVar4,"?>");
          if (local_res4 != (char *)0x0) {
            len_00 = (int)local_res4 - (int)pcVar4;
            local_res4 = local_res4 + 1;
            ezxml_proc_inst(root,pcVar4,len_00);
          }
        }
        else if (*local_res4 == '<') {
          local_res4 = strchr(local_res4,0x3e);
        }
        else {
          cVar1 = *local_res4;
          local_res4 = local_res4 + 1;
          if ((cVar1 == '%') && (root->standalone == 0)) {
LAB_004030d8:
            free(local_48);
            return (short)(root->err[0] == '\0');
          }
        }
      }
      goto LAB_004030cc;
    }
    sVar3 = strspn(local_res4 + 8,"\t\r\n ");
    local_res4 = local_res4 + sVar3 + 8;
    sVar3 = strspn(local_res4,"\t\r\n %");
    local_3c = local_res4 + sVar3;
    sVar3 = strcspn(local_3c,"\t\r\n ");
    pcVar4 = local_3c + sVar3;
    *pcVar4 = ';';
    sVar3 = strspn(pcVar4 + 1,"\t\r\n ");
    cVar1 = pcVar4[sVar3 + 1];
    __s = pcVar4 + sVar3 + 1 + 1;
    if ((cVar1 == '\"') || (cVar1 == '\'')) {
      local_4c = 0;
      if (*local_res4 == '%') {
        local_14 = local_48;
      }
      else {
        local_14 = root->ent;
      }
      for (; local_14[local_4c] != (char *)0x0; local_4c = local_4c + 1) {
      }
      ppcVar5 = realloc(local_14,(local_4c + 3) * 4);
      if (*local_res4 == '%') {
        local_48 = ppcVar5;
      }
      else {
        root->ent = ppcVar5;
      }
      pcVar4[1] = '\0';
      local_res4 = strchr(__s,(int)cVar1);
      if (local_res4 != (char *)0x0) {
        *local_res4 = '\0';
        local_res4 = local_res4 + 1;
      }
      pcVar4 = ezxml_decode(__s,local_48,'%');
      ppcVar5[local_4c + 1] = pcVar4;
      ppcVar5[local_4c + 2] = (char *)0x0;
      iVar2 = ezxml_ent_ok(local_3c,ppcVar5[local_4c + 1],ppcVar5);
      if (iVar2 == 0) {
        if (ppcVar5[local_4c + 1] != __s) {
          free(ppcVar5[local_4c + 1]);
        }
        ezxml_err(root,__s,"circular entity declaration &%s",local_3c);
        goto LAB_004030d8;
      }
      ppcVar5[local_4c] = local_3c;
    }
    else {
      local_res4 = strchr(pcVar4,0x3e);
    }
  } while( true );
  sVar3 = strcspn(local_res4,"\t\r\n >");
  local_res4 = local_res4 + (sVar3 - 1);
  if (*local_18 != ' ') {
    local_40 = (char *)0x0;
LAB_00402c58:
    if (root->attr[local_4c] == (char **)0x0) {
      if (local_4c == 0) {
        local_20 = malloc(8);
      }
      else {
        local_20 = realloc(root->attr,(local_4c + 2) * 4);
      }
      root->attr = local_20;
      pppcVar6 = root->attr;
      ppcVar5 = malloc(8);
      pppcVar6[local_4c] = ppcVar5;
      *root->attr[local_4c] = pcVar4;
      root->attr[local_4c + 1] = (char **)0x0;
      root->attr[local_4c][1] = (char *)0x0;
    }
    for (local_50 = 1; root->attr[local_4c][local_50] != (char *)0x0; local_50 = local_50 + 3) {
    }
    pppcVar6 = root->attr;
    ppcVar5 = realloc(root->attr[local_4c],(local_50 + 4) * 4);
    pppcVar6[local_4c] = ppcVar5;
    root->attr[local_4c][local_50 + 3] = (char *)0x0;
    root->attr[local_4c][local_50 + 2] = local_18;
    ppcVar5 = root->attr[local_4c];
    if (local_40 == (char *)0x0) {
      local_28 = (char *)0x0;
    }
    else {
      local_28 = ezxml_decode(local_40,root->ent,*local_18);
    }
    ppcVar5[local_50 + 1] = local_28;
    root->attr[local_4c][local_50] = local_3c;
  }
  goto LAB_00402f08;
}


// ==== ezxml_str2utf8 @ 00403114

char * ezxml_str2utf8(char **s,size_t *len)

{
  ushort uVar1;
  uint uVar2;
  char *pcVar3;
  int local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  size_t local_28;
  void *local_24;
  uint local_20;
  uint local_1c;
  char *local_18;
  int local_14;
  int local_10;
  
                    /* Unresolved local var: char * u@[???]
                       Unresolved local var: size_t l@[???]
                       Unresolved local var: size_t sl@[???]
                       Unresolved local var: size_t max@[???]
                       Unresolved local var: long c@[???]
                       Unresolved local var: long d@[???]
                       Unresolved local var: int b@[???]
                       Unresolved local var: int be@[???] */
  local_28 = 0;
  local_30 = *len;
  if (**s == -2) {
    local_10 = 1;
  }
  else {
    if (**s == -1) {
      local_14 = 0;
    }
    else {
      local_14 = -1;
    }
    local_10 = local_14;
  }
  if (local_10 == -1) {
    local_18 = (char *)0x0;
  }
  else {
    local_24 = malloc(local_30);
    for (local_2c = 2; local_2c < *len - 1; local_2c = local_2c + 2) {
      if (local_10 == 0) {
        uVar1 = CONCAT11((*s)[local_2c + 1],(*s)[local_2c]);
      }
      else {
        uVar1 = CONCAT11((*s)[local_2c],(*s)[local_2c + 1]);
      }
      local_1c = (uint)uVar1;
      local_34 = local_1c;
      uVar2 = local_2c;
      if (((0xd7ff < local_1c) && (local_1c < 0xe000)) && (uVar2 = local_2c + 2, uVar2 < *len - 1))
      {
        if (local_10 == 0) {
          uVar1 = CONCAT11((*s)[local_2c + 3],(*s)[uVar2]);
        }
        else {
          uVar1 = CONCAT11((*s)[uVar2],(*s)[local_2c + 3]);
        }
        local_20 = (uint)uVar1;
        local_34 = ((local_1c & 0x3ff) << 10 | local_20 & 0x3ff) + 0x10000;
      }
      while (local_2c = uVar2, local_30 < local_28 + 6) {
        local_30 = local_30 + 0x400;
        local_24 = realloc(local_24,local_30);
        uVar2 = local_2c;
      }
      if (local_34 < 0x80) {
        *(char *)((int)local_24 + local_28) = (char)local_34;
        local_28 = local_28 + 1;
      }
      else {
        local_3c = 0;
        for (local_38 = local_34; local_38 != 0; local_38 = (int)local_38 / 2) {
          local_3c = local_3c + 1;
        }
        local_3c = (local_3c + -2) / 5;
        *(byte *)((int)local_24 + local_28) =
             (byte)(0xff << (7U - local_3c & 0x1f)) |
             (byte)((int)local_34 >> (local_3c * 6 & 0x1fU));
        while (local_28 = local_28 + 1, local_3c != 0) {
          local_3c = local_3c + -1;
          *(byte *)((int)local_24 + local_28) =
               (byte)((int)local_34 >> (local_3c * 6 & 0x1fU)) & 0x3f | 0x80;
        }
      }
    }
    *len = local_28;
    pcVar3 = realloc(local_24,*len);
    *s = pcVar3;
    local_18 = *s;
  }
  return local_18;
}


// ==== ezxml_free_attr @ 004035c8

void ezxml_free_attr(char **attr)

{
  char *__ptr;
  int local_c;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: char * m@[???] */
  local_c = 0;
  if ((attr != (char **)0x0) && (attr != EZXML_NIL)) {
    for (; attr[local_c] != (char *)0x0; local_c = local_c + 2) {
    }
    __ptr = attr[local_c + 1];
    for (local_c = 0; __ptr[local_c] != '\0'; local_c = local_c + 1) {
      if (__ptr[local_c] < '\0') {
        free(attr[local_c * 2]);
      }
      if ((__ptr[local_c] & 0x40U) != 0) {
        free(attr[local_c * 2 + 1]);
      }
    }
    free(__ptr);
    free(attr);
  }
  return;
}


// ==== ezxml_parse_str @ 00403750

ezxml_t_conflict ezxml_parse_str(char *s,size_t len)

{
  char cVar1;
  char cVar2;
  bool bVar3;
  char *pcVar4;
  short sVar5;
  char *pcVar6;
  ushort **ppuVar7;
  ezxml_t_conflict peVar8;
  size_t sVar9;
  int iVar10;
  size_t len_00;
  undefined2 extraout_var;
  ushort *puVar11;
  char *local_res0;
  size_t local_res4 [3];
  int local_58;
  int local_54;
  int local_50;
  char **local_4c;
  char **local_48;
  char *local_44;
  char *local_38;
  char *local_20;
  char **local_18;
  ezxml_root_t local_14;
  
  local_res0 = s;
  local_res4[0] = len;
                    /* Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: char q@[???]
                       Unresolved local var: char e@[???]
                       Unresolved local var: char * d@[???]
                       Unresolved local var: char * * attr@[???]
                       Unresolved local var: char * * a@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???] */
  local_14 = (ezxml_root_t)ezxml_new((char *)0x0);
  local_4c = (char **)0x0;
  local_14->m = local_res0;
  if (local_res4[0] == 0) {
    local_14 = (ezxml_root_t)ezxml_err(local_14,(char *)0x0,"root tag missing");
  }
  else {
    pcVar6 = ezxml_str2utf8(&local_res0,local_res4);
    local_14->u = pcVar6;
    local_14->s = local_res0;
    local_14->e = local_14->s + local_res4[0];
    cVar1 = local_res0[local_res4[0] - 1];
    local_res0[local_res4[0] - 1] = '\0';
    for (; (*local_res0 != '\0' && (*local_res0 != '<')); local_res0 = local_res0 + 1) {
    }
    if (*local_res0 == '\0') {
      local_14 = (ezxml_root_t)ezxml_err(local_14,local_res0,"root tag missing");
    }
    else {
LAB_004038a4:
      do {
        pcVar6 = local_res0;
        local_48 = EZXML_NIL;
        local_44 = local_res0 + 1;
        local_res0 = local_44;
        ppuVar7 = __ctype_b_loc();
        pcVar4 = local_res0;
        if ((((((*ppuVar7)[*local_res0] & 4) == 0) && (*local_res0 != '_')) && (*local_res0 != ':'))
           && (-1 < *local_res0)) {
          if (*local_res0 == '/') {
            local_44 = local_res0 + 1;
            sVar9 = strcspn(local_44,"\t\r\n >");
            local_res0 = pcVar4 + sVar9 + 1;
            cVar2 = *local_res0;
            if ((cVar2 == '\0') && (cVar1 != '>')) {
              peVar8 = ezxml_err(local_14,local_44,"missing >");
              return peVar8;
            }
            *local_res0 = '\0';
            peVar8 = ezxml_close_tag(local_14,local_44,local_res0);
            if (peVar8 != (ezxml_t_conflict)0x0) {
              return &local_14->xml;
            }
            ppuVar7 = __ctype_b_loc();
            pcVar6 = local_res0;
            puVar11 = *ppuVar7;
            *local_res0 = cVar2;
            if ((puVar11[*local_res0] & 0x20) != 0) {
              sVar9 = strspn(local_res0,"\t\r\n ");
              local_res0 = pcVar6 + sVar9;
            }
          }
          else {
            iVar10 = strncmp(local_res0,"!--",3);
            if (iVar10 == 0) {
              local_res0 = strstr(local_res0 + 3,"--");
              if (((local_res0 == (char *)0x0) ||
                  ((local_res0 = local_res0 + 2, *local_res0 != '>' && (*local_res0 != '\0')))) ||
                 ((*local_res0 == '\0' && (cVar1 != '>')))) {
                peVar8 = ezxml_err(local_14,local_44,"unclosed <!--");
                return peVar8;
              }
            }
            else {
              iVar10 = strncmp(local_res0,"![CDATA[",8);
              if (iVar10 == 0) {
                local_res0 = strstr(local_res0,"]]>");
                if (local_res0 == (char *)0x0) {
                  peVar8 = ezxml_err(local_14,local_44,"unclosed <![CDATA[");
                  return peVar8;
                }
                local_res0 = local_res0 + 2;
                ezxml_char_content(local_14,pcVar6 + 9,(size_t)(local_res0 + (-10 - (int)local_44)),
                                   'c');
              }
              else {
                iVar10 = strncmp(local_res0,"!DOCTYPE",8);
                if (iVar10 == 0) {
                  bVar3 = false;
                  while ((pcVar6 = local_res0, *local_res0 != '\0' &&
                         (((!bVar3 && (*local_res0 != '>')) ||
                          ((bVar3 && ((*local_res0 != ']' ||
                                      (sVar9 = strspn(local_res0 + 1,"\t\r\n "),
                                      pcVar6[sVar9 + 1] != '>'))))))))) {
                    pcVar6 = local_res0;
                    sVar9 = strcspn(local_res0 + 1,"[]>");
                    local_res0 = pcVar6 + sVar9 + 1;
                    if (*local_res0 == '[') {
                      bVar3 = true;
                    }
                  }
                  if ((*local_res0 == '\0') && (cVar1 != '>')) {
                    peVar8 = ezxml_err(local_14,local_44,"unclosed <!DOCTYPE");
                    return peVar8;
                  }
                  if (bVar3) {
                    local_38 = strchr(local_44,0x5b);
                    local_38 = local_38 + 1;
                  }
                  else {
                    local_38 = local_44;
                  }
                  local_44 = local_38;
                  if (bVar3) {
                    len_00 = (int)local_res0 - (int)local_38;
                    local_res0 = local_res0 + 1;
                    sVar5 = ezxml_internal_dtd(local_14,local_38,len_00);
                    if (CONCAT22(extraout_var,sVar5) == 0) {
                      return &local_14->xml;
                    }
                  }
                }
                else {
                  if (*local_res0 != '?') {
                    peVar8 = ezxml_err(local_14,local_44,"unexpected <");
                    return peVar8;
                  }
                  do {
                    local_res0 = strchr(local_res0,0x3f);
                    if ((local_res0 == (char *)0x0) ||
                       (local_res0 = local_res0 + 1, *local_res0 == '\0')) break;
                  } while (*local_res0 != '>');
                  if ((local_res0 == (char *)0x0) || ((*local_res0 == '\0' && (cVar1 != '>')))) {
                    peVar8 = ezxml_err(local_14,local_44,"unclosed <?");
                    return peVar8;
                  }
                  ezxml_proc_inst(local_14,pcVar6 + 2,(size_t)(local_res0 + (-2 - (int)local_44)));
                }
              }
            }
          }
        }
        else {
          if (local_14->cur == (ezxml_t_conflict)0x0) {
            peVar8 = ezxml_err(local_14,local_44,"markup outside of root element");
            return peVar8;
          }
          sVar9 = strcspn(local_res0,"\t\r\n />");
          local_res0 = pcVar4 + sVar9;
          while (ppuVar7 = __ctype_b_loc(), ((*ppuVar7)[*local_res0] & 0x20) != 0) {
            *local_res0 = '\0';
            local_res0 = local_res0 + 1;
          }
          if (((*local_res0 != '\0') && (*local_res0 != '/')) && (*local_res0 != '>')) {
            local_54 = 0;
            while ((local_4c = local_14->attr[local_54], local_4c != (char **)0x0 &&
                   (iVar10 = strcmp(*local_4c,local_44), iVar10 != 0))) {
              local_54 = local_54 + 1;
            }
          }
          local_50 = 0;
          while (((*local_res0 != '\0' && (*local_res0 != '/')) && (*local_res0 != '>'))) {
            if (local_50 == 0) {
              local_18 = malloc(0x10);
            }
            else {
              local_18 = realloc(local_48,(local_50 + 4) * 4);
            }
            local_48 = local_18;
            if (local_50 == 0) {
              local_20 = malloc(2);
            }
            else {
              local_20 = realloc(local_18[local_50 + 1],local_50 / 2 + 2);
            }
            local_18[local_50 + 3] = local_20;
            memcpy(local_18[local_50 + 3] + local_50 / 2,&DAT_00517160,2);
            pcVar6 = local_res0;
            local_18[local_50 + 2] = (char *)0x0;
            local_18[local_50 + 1] = "";
            local_18[local_50] = local_res0;
            sVar9 = strcspn(local_res0,"\t\r\n =/>");
            local_res0 = pcVar6 + sVar9;
            if ((*local_res0 == '=') ||
               (ppuVar7 = __ctype_b_loc(), ((*ppuVar7)[*local_res0] & 0x20) != 0)) {
              *local_res0 = '\0';
              pcVar6 = local_res0 + 1;
              local_res0 = pcVar6;
              sVar9 = strspn(pcVar6,"\t\r\n =");
              local_res0 = pcVar6 + sVar9;
              cVar2 = *local_res0;
              if ((cVar2 == '\"') || (cVar2 == '\'')) {
                local_res0 = local_res0 + 1;
                local_18[local_50 + 1] = local_res0;
                for (; (*local_res0 != '\0' && (cVar2 != *local_res0)); local_res0 = local_res0 + 1)
                {
                }
                if (*local_res0 == '\0') {
                  ezxml_free_attr(local_18);
                  peVar8 = ezxml_err(local_14,local_44,"missing %c",(int)cVar2);
                  return peVar8;
                }
                *local_res0 = '\0';
                local_res0 = local_res0 + 1;
                local_58 = 1;
                while (((local_4c != (char **)0x0 && (local_4c[local_58] != (char *)0x0)) &&
                       (iVar10 = strcmp(local_4c[local_58],local_18[local_50]), iVar10 != 0))) {
                  local_58 = local_58 + 3;
                }
                if ((local_4c == (char **)0x0) || (local_4c[local_58] == (char *)0x0)) {
                  cVar2 = ' ';
                }
                else {
                  cVar2 = *local_4c[local_58 + 2];
                }
                pcVar6 = ezxml_decode(local_18[local_50 + 1],local_14->ent,cVar2);
                local_18[local_50 + 1] = pcVar6;
                if ((local_18[local_50 + 1] < local_44) || (local_res0 < local_18[local_50 + 1])) {
                  local_18[local_50 + 3][local_50 / 2] = '@';
                }
              }
            }
            while (ppuVar7 = __ctype_b_loc(), ((*ppuVar7)[*local_res0] & 0x20) != 0) {
              local_res0 = local_res0 + 1;
            }
            local_50 = local_50 + 2;
          }
          if (*local_res0 == '/') {
            *local_res0 = '\0';
            local_res0 = local_res0 + 1;
            if (((*local_res0 != '\0') && (*local_res0 != '>')) ||
               ((*local_res0 == '\0' && (cVar1 != '>')))) {
              if (local_50 != 0) {
                ezxml_free_attr(local_48);
              }
              peVar8 = ezxml_err(local_14,local_44,"missing >");
              return peVar8;
            }
            ezxml_open_tag(local_14,local_44,local_48);
            ezxml_close_tag(local_14,local_44,local_res0);
          }
          else {
            cVar2 = *local_res0;
            if ((cVar2 != '>') && ((*local_res0 != '\0' || (cVar1 != '>')))) {
              if (local_50 != 0) {
                ezxml_free_attr(local_48);
              }
              peVar8 = ezxml_err(local_14,local_44,"missing >");
              return peVar8;
            }
            *local_res0 = '\0';
            ezxml_open_tag(local_14,local_44,local_48);
            *local_res0 = cVar2;
          }
        }
        if ((local_res0 == (char *)0x0) || (*local_res0 == '\0')) break;
        *local_res0 = '\0';
        local_44 = local_res0 + 1;
        local_res0 = local_44;
        if ((*local_44 != '\0') && (*local_44 != '<')) {
          for (; (*local_res0 != '\0' && (*local_res0 != '<')); local_res0 = local_res0 + 1) {
          }
          if (*local_res0 == '\0') break;
          ezxml_char_content(local_14,local_44,(int)local_res0 - (int)local_44,'&');
          goto LAB_004038a4;
        }
      } while (*local_44 != '\0');
      if (local_14->cur != (ezxml_t_conflict)0x0) {
        if (local_14->cur->name == (char *)0x0) {
          local_14 = (ezxml_root_t)ezxml_err(local_14,local_44,"root tag missing");
        }
        else {
          local_14 = (ezxml_root_t)
                     ezxml_err(local_14,local_44,"unclosed tag <%s>",local_14->cur->name);
        }
      }
    }
  }
  return &local_14->xml;
}


// ==== ezxml_parse_fp @ 004048f8

ezxml_t_conflict ezxml_parse_fp(FILE *fp)

{
  size_t sVar1;
  char *local_20;
  size_t local_1c;
  ezxml_t_conflict local_10;
  
                    /* Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: size_t l@[???]
                       Unresolved local var: size_t len@[???]
                       Unresolved local var: char * s@[???] */
  local_1c = 0;
  local_20 = malloc(0x400);
  if (local_20 == (char *)0x0) {
    local_10 = (ezxml_t_conflict)0x0;
  }
  else {
    do {
      sVar1 = fread(local_20 + local_1c,1,0x400,(FILE *)fp);
      local_1c = local_1c + sVar1;
      if (sVar1 == 0x400) {
        local_20 = realloc(local_20,local_1c + 0x400);
      }
    } while ((local_20 != (char *)0x0) && (sVar1 == 0x400));
    if (local_20 == (char *)0x0) {
      local_10 = (ezxml_t_conflict)0x0;
    }
    else {
      local_10 = ezxml_parse_str(local_20,local_1c);
      local_10[1].txt = (char *)0xffffffff;
    }
  }
  return local_10;
}


// ==== ezxml_parse_fd @ 00404a2c

ezxml_t_conflict ezxml_parse_fd(int fd)

{
  char *s;
  size_t len;
  stat sStack_a8;
  ezxml_t_conflict local_10;
  
                    /* Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: stat st@[???]
                       Unresolved local var: size_t l@[???]
                       Unresolved local var: void * m@[???] */
  if (fd < 0) {
    local_10 = (ezxml_t_conflict)0x0;
  }
  else {
    fstat(fd,&sStack_a8);
    s = malloc(sStack_a8.st_blocks);
    len = read(fd,s,sStack_a8.st_blocks);
    local_10 = ezxml_parse_str(s,len);
    local_10[1].txt = (char *)0xffffffff;
  }
  return local_10;
}


// ==== ezxml_parse_file @ 00404afc

ezxml_t_conflict ezxml_parse_file(char *file)

{
  int fd;
  ezxml_t_conflict peVar1;
  
                    /* Unresolved local var: int fd@[???]
                       Unresolved local var: ezxml_t.conflict xml@[???] */
  fd = open(file,0);
  peVar1 = ezxml_parse_fd(fd);
  if (-1 < fd) {
    close(fd);
  }
  return peVar1;
}


// ==== ezxml_ampencode @ 00404b7c

char * ezxml_ampencode(char *s,size_t len,char **dst,size_t *dlen,size_t *max,short a)

{
  int iVar1;
  char *pcVar2;
  size_t sVar3;
  char *local_res0;
  char *local_34;
  char *local_28;
  char *local_1c;
  
                    /* Unresolved local var: char * e@[???] */
  for (local_res0 = s; local_res0 != s + len; local_res0 = local_res0 + 1) {
    while (*max < *dlen + 10) {
      pcVar2 = *dst;
      *max = *max + 0x400;
      pcVar2 = realloc(pcVar2,*max);
      *dst = pcVar2;
    }
    switch(*local_res0) {
    case '\0':
      return *dst;
    default:
      sVar3 = *dlen;
      (*dst)[sVar3] = *local_res0;
      *dlen = sVar3 + 1;
      break;
    case '\t':
      sVar3 = *dlen;
      if (a == 0) {
        local_34 = "\t";
      }
      else {
        local_34 = "&#x9;";
      }
      iVar1 = sprintf(*dst + *dlen,local_34);
      *dlen = sVar3 + iVar1;
      break;
    case '\n':
      sVar3 = *dlen;
      if (a == 0) {
        local_28 = "\n";
      }
      else {
        local_28 = "&#xA;";
      }
      iVar1 = sprintf(*dst + *dlen,local_28);
      *dlen = sVar3 + iVar1;
      break;
    case '\r':
      memcpy(*dst + *dlen,"&#xD;",6);
      *dlen = *dlen + 5;
      break;
    case '\"':
      sVar3 = *dlen;
      if (a == 0) {
        local_1c = "\"";
      }
      else {
        local_1c = "&quot;";
      }
      iVar1 = sprintf(*dst + *dlen,local_1c);
      *dlen = sVar3 + iVar1;
      break;
    case '&':
      memcpy(*dst + *dlen,"&amp;",6);
      *dlen = *dlen + 5;
      break;
    case '<':
      memcpy(*dst + *dlen,&DAT_0051735c,5);
      *dlen = *dlen + 4;
      break;
    case '>':
      memcpy(*dst + *dlen,&DAT_00517364,5);
      *dlen = *dlen + 4;
    }
  }
  return *dst;
}


// ==== ezxml_toxml_r @ 00404fa0

char * ezxml_toxml_r(ezxml_t_conflict xml,char **s,size_t *len,size_t *max,size_t start,char ***attr
                    )

{
  char *pcVar1;
  size_t sVar2;
  int iVar3;
  size_t sVar4;
  uint local_30;
  int local_28;
  int local_24;
  char *local_20;
  char *local_1c;
  char *local_18;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: char * txt@[???]
                       Unresolved local var: size_t off@[???] */
  if (xml->parent == (ezxml_t)0x0) {
    local_18 = "";
  }
  else {
    local_18 = xml->parent->txt;
  }
  local_30 = 0;
  pcVar1 = ezxml_ampencode(local_18 + start,xml->off - start,s,len,max,0);
  *s = pcVar1;
  while (sVar4 = *len, sVar2 = strlen(xml->name), *max < sVar4 + sVar2 + 4) {
    pcVar1 = *s;
    *max = *max + 0x400;
    pcVar1 = realloc(pcVar1,*max);
    *s = pcVar1;
  }
  sVar4 = *len;
  iVar3 = sprintf(*s + *len,"<%s",xml->name);
  *len = sVar4 + iVar3;
  for (local_24 = 0; xml->attr[local_24] != (char *)0x0; local_24 = local_24 + 2) {
    pcVar1 = ezxml_attr(xml,xml->attr[local_24]);
    if (pcVar1 == xml->attr[local_24 + 1]) {
      while (sVar4 = *len, sVar2 = strlen(xml->attr[local_24]), *max < sVar4 + sVar2 + 7) {
        pcVar1 = *s;
        *max = *max + 0x400;
        pcVar1 = realloc(pcVar1,*max);
        *s = pcVar1;
      }
      sVar4 = *len;
      iVar3 = sprintf(*s + *len," %s=\"",xml->attr[local_24]);
      *len = sVar4 + iVar3;
      ezxml_ampencode(xml->attr[local_24 + 1],0xffffffff,s,len,max,1);
      memcpy(*s + *len,&DAT_00517374,2);
      *len = *len + 1;
    }
  }
  local_24 = 0;
  while ((attr[local_24] != (char **)0x0 && (iVar3 = strcmp(*attr[local_24],xml->name), iVar3 != 0))
        ) {
    local_24 = local_24 + 1;
  }
  for (local_28 = 1; (attr[local_24] != (char **)0x0 && (attr[local_24][local_28] != (char *)0x0));
      local_28 = local_28 + 3) {
    if ((attr[local_24][local_28 + 1] != (char *)0x0) &&
       (pcVar1 = ezxml_attr(xml,attr[local_24][local_28]), pcVar1 == attr[local_24][local_28 + 1]))
    {
      while (sVar4 = *len, sVar2 = strlen(attr[local_24][local_28]), *max < sVar4 + sVar2 + 7) {
        pcVar1 = *s;
        *max = *max + 0x400;
        pcVar1 = realloc(pcVar1,*max);
        *s = pcVar1;
      }
      sVar4 = *len;
      iVar3 = sprintf(*s + *len," %s=\"",attr[local_24][local_28]);
      *len = sVar4 + iVar3;
      ezxml_ampencode(attr[local_24][local_28 + 1],0xffffffff,s,len,max,1);
      memcpy(*s + *len,&DAT_00517374,2);
      *len = *len + 1;
    }
  }
  memcpy(*s + *len,&DAT_005171ac,2);
  *len = *len + 1;
  if (xml->child == (ezxml_t)0x0) {
    local_1c = ezxml_ampencode(xml->txt,0xffffffff,s,len,max,0);
  }
  else {
    local_1c = ezxml_toxml_r(xml->child,s,len,max,0,attr);
  }
  *s = local_1c;
  while (sVar4 = *len, sVar2 = strlen(xml->name), *max < sVar4 + sVar2 + 4) {
    pcVar1 = *s;
    *max = *max + 0x400;
    pcVar1 = realloc(pcVar1,*max);
    *s = pcVar1;
  }
  sVar4 = *len;
  iVar3 = sprintf(*s + *len,"</%s>",xml->name);
  *len = sVar4 + iVar3;
  for (; (local_18[local_30] != '\0' && (local_30 < xml->off)); local_30 = local_30 + 1) {
  }
  if (xml->ordered == (ezxml_t)0x0) {
    local_20 = ezxml_ampencode(local_18 + local_30,0xffffffff,s,len,max,0);
  }
  else {
    local_20 = ezxml_toxml_r(xml->ordered,s,len,max,local_30,attr);
  }
  return local_20;
}


// ==== ezxml_toxml @ 004058d0

char * ezxml_toxml(ezxml_t_conflict xml)

{
  ezxml_t peVar1;
  ezxml_t peVar2;
  void *__dest;
  char *pcVar3;
  size_t sVar4;
  char *pcVar5;
  int iVar6;
  int local_60;
  int local_5c;
  int local_58;
  ezxml_t_conflict local_4c;
  size_t local_40;
  size_t local_3c;
  char *local_38 [2];
  char *local_30;
  char *local_2c;
  char *local_28;
  char *local_24;
  char *local_20;
  ezxml_t local_1c;
  ezxml_t local_18;
  
                    /* Unresolved local var: ezxml_t.conflict p@[???]
                       Unresolved local var: ezxml_t.conflict o@[???]
                       Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: size_t len@[???]
                       Unresolved local var: size_t max@[???]
                       Unresolved local var: char * s@[???]
                       Unresolved local var: char * t@[???]
                       Unresolved local var: char * n@[???]
                       Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: int k@[???] */
  if (xml == (ezxml_t_conflict)0x0) {
    local_18 = (ezxml_t)0x0;
    local_1c = (ezxml_t)0x0;
    peVar1 = local_18;
  }
  else {
    local_1c = xml->ordered;
    peVar1 = xml->parent;
  }
  peVar2 = local_1c;
  local_40 = 0;
  local_3c = 0x400;
  local_18 = peVar1;
  __dest = malloc(0x400);
  local_38[0] = memcpy(__dest,&DAT_005171a8,1);
  if ((xml == (ezxml_t_conflict)0x0) || (local_4c = xml, xml->name == (char *)0x0)) {
    local_20 = realloc(local_38[0],local_40 + 1);
  }
  else {
    for (; local_4c->parent != (ezxml_t)0x0; local_4c = local_4c->parent) {
    }
    for (local_58 = 0;
        (peVar1 == (ezxml_t)0x0 && ((&(local_4c[1].parent)->name)[local_58] != (char *)0x0));
        local_58 = local_58 + 1) {
      local_60 = 2;
      while (*(int *)((&(local_4c[1].parent)->name)[local_58] + (1 - local_60) * -4) != 0) {
        local_60 = local_60 + 1;
      }
      for (local_5c = 1; pcVar5 = *(char **)((&(local_4c[1].parent)->name)[local_58] + local_5c * 4)
          , pcVar5 != (char *)0x0; local_5c = local_5c + 1) {
        if (*(char *)(*(int *)((&(local_4c[1].parent)->name)[local_58] + local_60 * 4) +
                     local_5c + -1) != '>') {
          while( true ) {
            pcVar3 = *(char **)(&(local_4c[1].parent)->name)[local_58];
            sVar4 = strlen(pcVar3);
            iVar6 = sVar4 + local_40;
            sVar4 = strlen(pcVar5);
            if (iVar6 + sVar4 + 7 <= local_3c) break;
            local_3c = local_3c + 0x400;
            local_38[0] = realloc(local_38[0],local_3c);
          }
          local_24 = local_38[0] + local_40;
          if (*pcVar5 == '\0') {
            local_28 = "";
          }
          else {
            local_28 = " ";
          }
          iVar6 = sprintf(local_24,"<?%s%s%s?>\n",pcVar3,local_28,pcVar5);
          local_40 = iVar6 + local_40;
        }
      }
    }
    xml->ordered = (ezxml_t)0x0;
    xml->parent = xml->ordered;
    local_38[0] = ezxml_toxml_r(xml,local_38,&local_40,&local_3c,0,(char ***)local_4c[1].child);
    xml->parent = peVar1;
    xml->ordered = peVar2;
    for (local_58 = 0;
        (peVar1 == (ezxml_t)0x0 && ((&(local_4c[1].parent)->name)[local_58] != (char *)0x0));
        local_58 = local_58 + 1) {
      local_60 = 2;
      while (*(int *)((&(local_4c[1].parent)->name)[local_58] + (1 - local_60) * -4) != 0) {
        local_60 = local_60 + 1;
      }
      for (local_5c = 1; pcVar5 = *(char **)((&(local_4c[1].parent)->name)[local_58] + local_5c * 4)
          , pcVar5 != (char *)0x0; local_5c = local_5c + 1) {
        if (*(char *)(*(int *)((&(local_4c[1].parent)->name)[local_58] + local_60 * 4) +
                     local_5c + -1) != '<') {
          while( true ) {
            pcVar3 = *(char **)(&(local_4c[1].parent)->name)[local_58];
            sVar4 = strlen(pcVar3);
            iVar6 = sVar4 + local_40;
            sVar4 = strlen(pcVar5);
            if (iVar6 + sVar4 + 7 <= local_3c) break;
            local_3c = local_3c + 0x400;
            local_38[0] = realloc(local_38[0],local_3c);
          }
          local_2c = local_38[0] + local_40;
          if (*pcVar5 == '\0') {
            local_30 = "";
          }
          else {
            local_30 = " ";
          }
          iVar6 = sprintf(local_2c,"\n<?%s%s%s?>",pcVar3,local_30,pcVar5);
          local_40 = iVar6 + local_40;
        }
      }
    }
    local_20 = realloc(local_38[0],local_40 + 1);
  }
  return local_20;
}


// ==== ezxml_free @ 00405f48

void ezxml_free(ezxml_t_conflict xml)

{
  ezxml_t __ptr;
  int iVar1;
  char *__ptr_00;
  int local_18;
  int local_14;
  
                    /* Unresolved local var: ezxml_root_t root@[???]
                       Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: char * * a@[???]
                       Unresolved local var: char * s@[???] */
  if (xml != (ezxml_t_conflict)0x0) {
    ezxml_free(xml->child);
    ezxml_free(xml->ordered);
    if (xml->parent == (ezxml_t)0x0) {
      for (local_14 = 10; (&(xml[1].ordered)->name)[local_14] != (char *)0x0;
          local_14 = local_14 + 2) {
        __ptr = (ezxml_t)(&(xml[1].ordered)->attr)[local_14];
        if ((__ptr < xml[1].next) || (xml[1].sibling < __ptr)) {
          free(__ptr);
        }
      }
      free(xml[1].ordered);
      for (local_14 = 0; __ptr_00 = (&(xml[1].child)->name)[local_14], __ptr_00 != (char *)0x0;
          local_14 = local_14 + 1) {
        for (local_18 = 1; iVar1 = local_18 + 1, *(int *)(__ptr_00 + local_18 * 4) != 0;
            local_18 = local_18 + 3) {
          if ((*(int *)(__ptr_00 + iVar1 * 4) != 0) &&
             ((*(ezxml_t *)(__ptr_00 + iVar1 * 4) < xml[1].next ||
              (xml[1].sibling < *(ezxml_t *)(__ptr_00 + iVar1 * 4))))) {
            free(*(void **)(__ptr_00 + iVar1 * 4));
          }
        }
        free(__ptr_00);
      }
      if ((xml[1].child)->name != (char *)0x0) {
        free(xml[1].child);
      }
      for (local_14 = 0; (&(xml[1].parent)->name)[local_14] != (char *)0x0; local_14 = local_14 + 1)
      {
        for (local_18 = 1; *(int *)((&(xml[1].parent)->name)[local_14] + local_18 * 4) != 0;
            local_18 = local_18 + 1) {
        }
        free(*(void **)((&(xml[1].parent)->name)[local_14] + (local_18 + 1) * 4));
        free((&(xml[1].parent)->name)[local_14]);
      }
      if ((xml[1].parent)->name != (char *)0x0) {
        free(xml[1].parent);
      }
      if (xml[1].txt == (char *)0xffffffff) {
        free(xml[1].attr);
      }
      if (xml[1].off != 0) {
        free((void *)xml[1].off);
      }
    }
    ezxml_free_attr(xml->attr);
    if ((xml->flags & 0x40U) != 0) {
      free(xml->txt);
    }
    if ((xml->flags & 0x80U) != 0) {
      free(xml->name);
    }
    free(xml);
  }
  return;
}


// ==== ezxml_error @ 004063ec

char * ezxml_error(ezxml_t_conflict xml)

{
  ezxml_t_conflict local_res0;
  char *local_10;
  
  for (local_res0 = xml;
      (local_res0 != (ezxml_t_conflict)0x0 && (local_res0->parent != (ezxml_t)0x0));
      local_res0 = local_res0->parent) {
  }
  if (local_res0 == (ezxml_t_conflict)0x0) {
    local_10 = "";
  }
  else {
    local_10 = &local_res0[1].field_0x26;
  }
  return local_10;
}


// ==== ezxml_new @ 00406470

ezxml_t_conflict ezxml_new(char *name)

{
  void *pvVar1;
  ezxml_t_conflict peVar2;
  ezxml_t peVar3;
  
                    /* Unresolved local var: ezxml_root_t root@[???] */
  pvVar1 = malloc(0xd0);
  peVar2 = memset(pvVar1,0,0xd0);
  peVar2->name = name;
  peVar2[1].name = (char *)peVar2;
  peVar2->txt = "";
  memcpy(&peVar2[1].field_0x26,&DAT_005171a8,1);
  pvVar1 = malloc(0x2c);
  peVar3 = memcpy(pvVar1,ezxml_new::ent,0x2c);
  peVar2[1].ordered = peVar3;
  peVar2->attr = EZXML_NIL;
  peVar2[1].parent = (ezxml_t)EZXML_NIL;
  peVar2[1].child = peVar2[1].parent;
  return peVar2;
}


// ==== ezxml_insert @ 00406590

ezxml_t_conflict ezxml_insert(ezxml_t_conflict xml,ezxml_t_conflict dest,size_t off)

{
  ezxml_t peVar1;
  int iVar2;
  ezxml_t local_14;
  ezxml_t local_10;
  
                    /* Unresolved local var: ezxml_t.conflict cur@[???]
                       Unresolved local var: ezxml_t.conflict prev@[???]
                       Unresolved local var: ezxml_t.conflict head@[???] */
  xml->ordered = (ezxml_t)0x0;
  xml->sibling = xml->ordered;
  xml->next = xml->sibling;
  xml->off = off;
  xml->parent = dest;
  peVar1 = dest->child;
  if (peVar1 == (ezxml_t)0x0) {
    dest->child = xml;
  }
  else {
    if (off < peVar1->off) {
      xml->ordered = peVar1;
      dest->child = xml;
    }
    else {
      local_10 = peVar1;
      while ((local_10->ordered != (ezxml_t)0x0 && (local_10->ordered->off <= off))) {
        local_10 = local_10->ordered;
      }
      xml->ordered = local_10->ordered;
      local_10->ordered = xml;
    }
    local_14 = (ezxml_t)0x0;
    local_10 = peVar1;
    while ((local_10 != (ezxml_t)0x0 && (iVar2 = strcmp(local_10->name,xml->name), iVar2 != 0))) {
      local_14 = local_10;
      local_10 = local_10->sibling;
    }
    if ((local_10 == (ezxml_t)0x0) || (off < local_10->off)) {
      if ((local_14 != (ezxml_t)0x0) && (local_10 != (ezxml_t)0x0)) {
        local_14->sibling = local_10->sibling;
      }
      xml->next = local_10;
      local_14 = (ezxml_t)0x0;
      for (local_10 = peVar1; (local_10 != (ezxml_t)0x0 && (local_10->off <= off));
          local_10 = local_10->sibling) {
        local_14 = local_10;
      }
      xml->sibling = local_10;
      if (local_14 != (ezxml_t)0x0) {
        local_14->sibling = xml;
      }
    }
    else {
      while ((local_10->next != (ezxml_t)0x0 && (local_10->next->off <= off))) {
        local_10 = local_10->next;
      }
      xml->next = local_10->next;
      local_10->next = xml;
    }
  }
  return xml;
}


// ==== ezxml_add_child @ 00406860

ezxml_t_conflict ezxml_add_child(ezxml_t_conflict xml,char *name,size_t off)

{
  void *__s;
  ezxml_t_conflict xml_00;
  ezxml_t_conflict local_10;
  
                    /* Unresolved local var: ezxml_t.conflict child@[???] */
  if (xml == (ezxml_t_conflict)0x0) {
    local_10 = (ezxml_t_conflict)0x0;
  }
  else {
    __s = malloc(0x28);
    xml_00 = memset(__s,0,0x28);
    xml_00->name = name;
    xml_00->attr = EZXML_NIL;
    xml_00->txt = "";
    local_10 = ezxml_insert(xml_00,xml,off);
  }
  return local_10;
}


// ==== ezxml_set_txt @ 00406928

ezxml_t_conflict ezxml_set_txt(ezxml_t_conflict xml,char *txt)

{
  ezxml_t_conflict local_10;
  
  if (xml == (ezxml_t_conflict)0x0) {
    local_10 = (ezxml_t_conflict)0x0;
  }
  else {
    if ((xml->flags & 0x40U) != 0) {
      free(xml->txt);
    }
    xml->flags = xml->flags & 0xffbf;
    xml->txt = txt;
    local_10 = xml;
  }
  return local_10;
}


// ==== ezxml_set_attr @ 004069d8

ezxml_t_conflict ezxml_set_attr(ezxml_t_conflict xml,char *name,char *value)

{
  int iVar1;
  char **ppcVar2;
  char *pcVar3;
  size_t sVar4;
  int local_20;
  int local_1c;
  ezxml_t_conflict local_18;
  
                    /* Unresolved local var: int l@[???]
                       Unresolved local var: int c@[???] */
  local_1c = 0;
  if (xml == (ezxml_t_conflict)0x0) {
    local_18 = (ezxml_t_conflict)0x0;
  }
  else {
    while ((xml->attr[local_1c] != (char *)0x0 &&
           (iVar1 = strcmp(xml->attr[local_1c],name), iVar1 != 0))) {
      local_1c = local_1c + 2;
    }
    if (xml->attr[local_1c] == (char *)0x0) {
      if (value == (char *)0x0) {
        return xml;
      }
      if (xml->attr == EZXML_NIL) {
        ppcVar2 = malloc(0x10);
        xml->attr = ppcVar2;
        ppcVar2 = xml->attr;
        pcVar3 = strdup("");
        ppcVar2[1] = pcVar3;
      }
      else {
        ppcVar2 = realloc(xml->attr,(local_1c + 4) * 4);
        xml->attr = ppcVar2;
      }
      xml->attr[local_1c] = name;
      xml->attr[local_1c + 2] = (char *)0x0;
      ppcVar2 = xml->attr;
      pcVar3 = xml->attr[local_1c + 1];
      sVar4 = strlen(xml->attr[local_1c + 1]);
      pcVar3 = realloc(pcVar3,sVar4 + 2);
      ppcVar2[local_1c + 3] = pcVar3;
      memcpy(xml->attr[local_1c + 3] + sVar4,&DAT_00517160,2);
      if ((xml->flags & 0x20U) != 0) {
        xml->attr[local_1c + 3][sVar4] = -0x80;
      }
    }
    else if ((xml->flags & 0x20U) != 0) {
      free(name);
    }
    for (local_20 = local_1c; xml->attr[local_20] != (char *)0x0; local_20 = local_20 + 2) {
    }
    if ((xml->attr[local_20 + 1][local_1c / 2] & 0x40U) != 0) {
      free(xml->attr[local_1c + 1]);
    }
    if ((xml->flags & 0x20U) == 0) {
      xml->attr[local_20 + 1][local_1c / 2] = xml->attr[local_20 + 1][local_1c / 2] & 0xbf;
    }
    else {
      xml->attr[local_20 + 1][local_1c / 2] = xml->attr[local_20 + 1][local_1c / 2] | 0x40;
    }
    if (value == (char *)0x0) {
      if (xml->attr[local_20 + 1][local_1c / 2] < '\0') {
        free(xml->attr[local_1c]);
      }
      memmove(xml->attr + local_1c,xml->attr + local_1c + 2,((local_20 - local_1c) + 2) * 4);
      ppcVar2 = realloc(xml->attr,(local_20 + 2) * 4);
      xml->attr = ppcVar2;
      memmove(xml->attr[local_20 + 1] + local_1c / 2,xml->attr[local_20 + 1] + local_1c / 2 + 1,
              local_20 / 2 - local_1c / 2);
    }
    else {
      xml->attr[local_1c + 1] = value;
    }
    xml->flags = xml->flags & 0xffdf;
    local_18 = xml;
  }
  return local_18;
}


// ==== ezxml_set_flag @ 0040709c

ezxml_t_conflict ezxml_set_flag(ezxml_t_conflict xml,short flag)

{
  if (xml != (ezxml_t_conflict)0x0) {
    xml->flags = xml->flags | flag;
  }
  return xml;
}


// ==== ezxml_cut @ 004070f4

ezxml_t_conflict ezxml_cut(ezxml_t_conflict xml)

{
  int iVar1;
  ezxml_t_conflict local_18;
  ezxml_t local_10;
  ezxml_t_conflict local_c;
  
                    /* Unresolved local var: ezxml_t.conflict cur@[???] */
  if (xml == (ezxml_t_conflict)0x0) {
    local_c = (ezxml_t_conflict)0x0;
  }
  else {
    if (xml->next != (ezxml_t)0x0) {
      xml->next->sibling = xml->sibling;
    }
    if (xml->parent != (ezxml_t)0x0) {
      local_18 = xml->parent->child;
      if (local_18 == xml) {
        xml->parent->child = xml->ordered;
      }
      else {
        for (; local_18->ordered != xml; local_18 = local_18->ordered) {
        }
        local_18->ordered = local_18->ordered->ordered;
        local_18 = xml->parent->child;
        iVar1 = strcmp(local_18->name,xml->name);
        if (iVar1 != 0) {
          while (iVar1 = strcmp(local_18->sibling->name,xml->name), iVar1 != 0) {
            local_18 = local_18->sibling;
          }
          if (local_18->sibling == xml) {
            if (xml->next == (ezxml_t)0x0) {
              local_10 = local_18->sibling->sibling;
            }
            else {
              local_10 = xml->next;
            }
            local_18->sibling = local_10;
          }
          else {
            local_18 = local_18->sibling;
          }
        }
        for (; (local_18->next != (ezxml_t)0x0 && (local_18->next != xml));
            local_18 = local_18->next) {
        }
        if (local_18->next != (ezxml_t)0x0) {
          local_18->next = local_18->next->next;
        }
      }
    }
    xml->next = (ezxml_t)0x0;
    xml->sibling = xml->next;
    xml->ordered = xml->sibling;
    local_c = xml;
  }
  return local_c;
}


// ==== buildRegistrationRequest @ 00407360

int buildRegistrationRequest(WhiteSpaceContext *wsCtx)

{
  size_t sVar1;
  
                    /* Unresolved local var: char * tpl@[???]
                       Unresolved local var: char * soapEnv@[???] */
  memset(wsCtx->tempProcessingBuffer,0,0x400);
  sprintf(wsCtx->tempProcessingBuffer,
          "<RegistrationRequest xmlns=\"http://schemas.datacontract.org/2004/07/SpectrumBridge.WhiteSpaces.Services.v3\"><AntennaHeight>%d</AntennaHeight><ContactCity>%s</ContactCity><ContactCountry>%s</ContactCountry><ContactEmail>%s</ContactEmail><ContactName>%s</ContactName><ContactPhone>%s</ContactPhone><ContactState>%s</ContactState><ContactStreet>%s</ContactStreet><ContactZip>%s</ContactZip><DeviceOwner>%s</DeviceOwner><DeviceType>%d</DeviceType><Latitude>%s</Latitude><Longitude>%s</Longitude></RegistrationRequest>"
          ,wsCtx->antennaeHeightInMeters,(wsCtx->addressInfo).contactCity,
          (wsCtx->addressInfo).contactCountry,(wsCtx->addressInfo).contactEmail,
          (wsCtx->addressInfo).contactName,(wsCtx->addressInfo).contactPhoneNumber,
          (wsCtx->addressInfo).contactProvince,(wsCtx->addressInfo).contactStreetAddress,
          (wsCtx->addressInfo).contactPostalCode,(wsCtx->addressInfo).ownerName,wsCtx->mode,
          wsCtx->latitudeString,wsCtx->longitudeString);
  sVar1 = strlen(wsCtx->tempProcessingBuffer);
  sprintf(wsCtx->transmitBuffer,
          "PUT https://%s/v3/devices/%s/%s/%s HTTP/1.1\r\nHost: %s\r\nContent-Type: Application/xml \r\nContent-Length: %d\r\n\n%s\r\n\r\n"
          ,wsCtx->hostURL,wsCtx->regulatoryDomain,wsCtx->fccID,wsCtx->serialNumber,wsCtx->hostURL,
          sVar1,wsCtx->tempProcessingBuffer);
  sVar1 = strlen(wsCtx->transmitBuffer);
  return sVar1;
}


// ==== buildChannelRequest @ 00407550

int buildChannelRequest(WhiteSpaceContext *wsCtx)

{
  size_t sVar1;
  
                    /* Unresolved local var: char * tpl@[???] */
  sprintf(wsCtx->transmitBuffer,
          "GET https://%s/v3/channels/%s/%s/%s/?fccid=%s&serial=%s&type=%d&ant=%d HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n"
          ,wsCtx->hostURL,wsCtx->regulatoryDomain,wsCtx->latitudeString,wsCtx->longitudeString,
          wsCtx->fccID,wsCtx->serialNumber,wsCtx->channelLookUpMode,wsCtx->antennaeHeightInMeters,
          wsCtx->hostURL,"HTMLGET 1.0");
  sVar1 = strlen(wsCtx->transmitBuffer);
  return sVar1;
}


// ==== processXmlResponseMessage @ 00407634

int processXmlResponseMessage(WhiteSpaceContext *wsCtx)

{
  size_t len;
  ezxml_t_conflict xml;
  ezxml_t_conflict peVar1;
  int iVar2;
  uint uVar3;
  char *local_680;
  uint local_67c;
  uint local_678;
  char local_66c [4];
  char local_668 [4];
  char local_664 [4];
  char local_660;
  undefined1 auStack_65f [288];
  char local_53f [4];
  char local_53b [4];
  char local_537 [4];
  undefined1 auStack_533 [289];
  char local_412 [4];
  char local_40e [4];
  char local_40a [2];
  undefined1 auStack_408 [292];
  undefined1 auStack_2e4 [4];
  int aiStack_2e0 [180];
  int local_10;
  uint local_c;
  
                    /* Unresolved local var: ezxml_t xml@[???]
                       Unresolved local var: ezxml_t result@[???]
                       Unresolved local var: size_t i@[???]
                       Unresolved local var: char[3][301] channelTags@[???]
                       Unresolved local var: ChannelInfo[90] chanMap@[???]
                       Unresolved local var: int channelCount@[???]
                       Unresolved local var: char * pch@[???]
                       Unresolved local var: char * response@[???]
                       Unresolved local var: int length@[???] */
  memset(local_66c,0,0x387);
  local_66c[0] = s_ChannelCount_00517858[0];
  local_66c[1] = s_ChannelCount_00517858[1];
  local_66c[2] = s_ChannelCount_00517858[2];
  local_66c[3] = s_ChannelCount_00517858[3];
  local_668[0] = s_ChannelCount_00517858[4];
  local_668[1] = s_ChannelCount_00517858[5];
  local_668[2] = s_ChannelCount_00517858[6];
  local_668[3] = s_ChannelCount_00517858[7];
  local_664[0] = s_ChannelCount_00517858[8];
  local_664[1] = s_ChannelCount_00517858[9];
  local_664[2] = s_ChannelCount_00517858[10];
  local_664[3] = s_ChannelCount_00517858[0xb];
  local_660 = s_ChannelCount_00517858[0xc];
  memset(auStack_65f,0,0x120);
  local_53f[0] = s_ChannelList_00517988[0];
  local_53f[1] = s_ChannelList_00517988[1];
  local_53f[2] = s_ChannelList_00517988[2];
  local_53f[3] = s_ChannelList_00517988[3];
  local_53b[0] = s_ChannelList_00517988[4];
  local_53b[1] = s_ChannelList_00517988[5];
  local_53b[2] = s_ChannelList_00517988[6];
  local_53b[3] = s_ChannelList_00517988[7];
  local_537[0] = s_ChannelList_00517988[8];
  local_537[1] = s_ChannelList_00517988[9];
  local_537[2] = s_ChannelList_00517988[10];
  local_537[3] = s_ChannelList_00517988[0xb];
  memset(auStack_533,0,0x121);
  local_412[0] = s_RefreshIn_00517ab8[0];
  local_412[1] = s_RefreshIn_00517ab8[1];
  local_412[2] = s_RefreshIn_00517ab8[2];
  local_412[3] = s_RefreshIn_00517ab8[3];
  local_40e[0] = s_RefreshIn_00517ab8[4];
  local_40e[1] = s_RefreshIn_00517ab8[5];
  local_40e[2] = s_RefreshIn_00517ab8[6];
  local_40e[3] = s_RefreshIn_00517ab8[7];
  local_40a[0] = s_RefreshIn_00517ab8[8];
  local_40a[1] = s_RefreshIn_00517ab8[9];
  memset(auStack_408,0,0x123);
  local_67c = 0;
  len = strlen(wsCtx->receiveBuffer);
  (wsCtx->availableChannelMap).channelCount = 0;
  memset(auStack_2e4,0,0x2d0);
  xml = ezxml_parse_str(wsCtx->receiveBuffer,len);
  if (xml == (ezxml_t_conflict)0x0) {
    puts("Malformed Response");
    local_10 = 1;
  }
  else {
    for (local_678 = 0; local_678 < 3; local_678 = local_678 + 1) {
      local_c = local_678;
      if (local_678 == 1) {
        peVar1 = ezxml_get(xml);
        local_680 = strtok(peVar1->txt,",");
        while (local_680 != (char *)0x0) {
          iVar2 = atoi(local_680);
          aiStack_2e0[iVar2 * 2] = 1;
          local_680 = strtok((char *)0x0,",");
        }
      }
      else if (local_678 == 0) {
        peVar1 = ezxml_get(xml);
        local_67c = atoi(peVar1->txt);
      }
      else if (local_678 == 2) {
        peVar1 = ezxml_get(xml);
        iVar2 = atoi(peVar1->txt);
        wsCtx->refreshInHrs = iVar2;
      }
    }
    for (local_678 = 0; local_678 < 0x5a; local_678 = local_678 + 1) {
      if (aiStack_2e0[local_678 * 2] != 0) {
        uVar3 = (wsCtx->availableChannelMap).channelCount;
        (wsCtx->availableChannelMap).channels[uVar3] = local_678;
        (wsCtx->availableChannelMap).channelCount = uVar3 + 1;
      }
    }
    ezxml_free(xml);
    if (local_67c == (wsCtx->availableChannelMap).channelCount) {
      local_10 = 0;
    }
    else {
      printf("ChannelCount %d doesn\'t match actual chCount:%d\n",local_67c,
             (wsCtx->availableChannelMap).channelCount);
      local_10 = 1;
    }
  }
  return local_10;
}


// ==== httpHeaderStatus @ 00407b20

int httpHeaderStatus(WhiteSpaceContext *wsCtx,MessageType msgType)

{
  int iVar1;
  size_t local_48;
  int local_44;
  char *local_40;
  char acStack_3c [44];
  MessageType local_10;
  
  local_44 = 0;
  local_48 = strlen(wsCtx->receiveBuffer);
                    /* Unresolved local var: char * buff@[???]
                       Unresolved local var: char[40] intBuff@[???]
                       Unresolved local var: int count@[???]
                       Unresolved local var: int totalLength@[???] */
  for (local_40 = wsCtx->receiveBuffer; (0 < (int)local_48 && (*local_40 != ' '));
      local_40 = local_40 + 1) {
    local_48 = local_48 - 1;
  }
  while( true ) {
    local_40 = local_40 + 1;
    local_48 = local_48 - 1;
    if (((int)local_48 < 1) || (*local_40 == ' ')) break;
    acStack_3c[local_44] = *local_40;
    local_44 = local_44 + 1;
  }
  local_10 = msgType;
  if (msgType == WS_REGISTRATION_MESSAGE) {
    iVar1 = atoi(acStack_3c);
    wsCtx->registrationErrorCode = iVar1;
    if (wsCtx->registrationErrorCode != 200) {
      wsCtx->libraryState = LIB_STATE_REGISTRATION_FAILURE;
    }
  }
  else if (msgType == WS_CHANNEL_REQUEST_MESSAGE) {
    iVar1 = atoi(acStack_3c);
    wsCtx->channelRequestErrorCode = iVar1;
    if (wsCtx->channelRequestErrorCode != 200) {
      wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
    }
  }
  iVar1 = atoi(acStack_3c);
  return iVar1;
}


// ==== getWhiteSpaceStatus @ 00407d20

WSServiceStatus getWhiteSpaceStatus(WhiteSpaceContext *wsCtx,MessageType msgType)

{
  WSServiceStatus WVar1;
  char *param0;
  char *pcVar2;
  WSServiceStatus local_a0;
  char acStack_90 [100];
  char acStack_2c [8];
  int iStack_24;
  char acStack_20 [4];
  int iStack_1c;
  char acStack_18 [4];
  WSServiceStatus local_14;
  MessageType local_10;
  
                    /* Unresolved local var: char * buff@[???]
                       Unresolved local var: char * statusStart@[???]
                       Unresolved local var: char[100] junkBuffer@[???]
                       Unresolved local var: char[5] datBuff@[???]
                       Unresolved local var: int mDay@[???]
                       Unresolved local var: char[4] month@[???]
                       Unresolved local var: int year@[???]
                       Unresolved local var: char[4] timeFmt@[???]
                       Unresolved local var: char * tempStart@[???]
                       Unresolved local var: WSServiceStatus wsServiceStatus@[???]
                       Unresolved local var: int status@[???] */
  pcVar2 = wsCtx->receiveBuffer;
  local_a0 = WS_UNKNOWN_STATUS;
  local_14 = WS_UNKNOWN_STATUS;
  if (pcVar2 != (char *)0x0) {
    param0 = strstr(pcVar2,"SBI-Status:");
    sscanf(param0,"%s %d",acStack_90,(int *)&local_14);
    WVar1 = local_14;
    local_a0 = local_14;
    pcVar2 = strstr(pcVar2,"Date:");
    local_10 = msgType;
    if (msgType == WS_REGISTRATION_MESSAGE) {
      wsCtx->registrationStatus = WVar1;
      wsCtx->libraryState = LIB_STATE_REGISTRATION_SUCCESS;
      if (pcVar2 != (char *)0x0) {
        strncpy(wsCtx->registrationTimeStamp,pcVar2 + 6,0x1d);
      }
    }
    else if ((msgType == WS_CHANNEL_REQUEST_MESSAGE) &&
            (wsCtx->channelRequestStatus = WVar1, pcVar2 != (char *)0x0)) {
      strncpy(wsCtx->channelRequestTimeStamp,pcVar2 + 6,0x1d);
    }
    if (pcVar2 != (char *)0x0) {
      sscanf(pcVar2,"%s %s %d %s %d %s %s",acStack_90,acStack_2c,&iStack_24,acStack_20,&iStack_1c,
             wsCtx->absoluteTimeBuffer,acStack_18);
    }
  }
  return local_a0;
}


// ==== initializeWhiteSpaceContext @ 00407f20

void initializeWhiteSpaceContext(WhiteSpaceContext *context)

{
  memset(context,0,0x1470);
  context->libraryState = LIB_STATE_AWAITING_INITIALIZATION;
  (context->availableChannelMap).channelCount = 0;
  (context->preferredChannelMap).channelCount = 0;
  context->enumeratedTransmitPower = 10;
  memset(context->absoluteTimeBuffer,0,100);
  memset(context->registrationTimeStamp,0,100);
  memset(context->channelRequestTimeStamp,0,100);
  memset(context->meldStreamingIpAddress,0,100);
  return;
}


// ==== setWhiteSpaceContext @ 0040801c

LibraryInterfaceStatus
setWhiteSpaceContext
          (WhiteSpaceContext *wsCtx,char *hostURL,int comPortNumber,DevMode mode,DevMode chanDevMode
          ,int antHeightInMeters,char *regDomain,char *fccId,char *serialNumber,char *latString,
          char *lonString,char *ownerName,char *contName,char *contStreetAddress,char *contactCity,
          char *contactPostalCode,char *contactProvince,char *contactCountry,char *contactEmail,
          char *contactPhoneNumber)

{
  LibraryInterfaceStatus LVar1;
  LibraryInterfaceStatus local_10;
  
                    /* Unresolved local var: LibraryInterfaceStatus status@[???] */
  LVar1 = setHostURL(wsCtx,hostURL);
  if (LVar1 == LIB_STATUS_SET_FAILURE) {
    puts("URL Failure");
    wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  else {
    setComPortNumber(wsCtx,comPortNumber);
    setModeInContext(wsCtx,mode);
    setChannelLookUpModeInContext(wsCtx,chanDevMode);
    setAntennaeHeightInMeters(wsCtx,antHeightInMeters);
    LVar1 = setRegulatoryDomain(wsCtx,regDomain);
    if (LVar1 == LIB_STATUS_SET_FAILURE) {
      puts("Reg Dom Failure");
      wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
      local_10 = LIB_STATUS_SET_FAILURE;
    }
    else {
      LVar1 = setFccId(wsCtx,fccId);
      if (LVar1 == LIB_STATUS_SET_FAILURE) {
        puts("FCCID Failure");
        wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
        local_10 = LIB_STATUS_SET_FAILURE;
      }
      else {
        LVar1 = setSerialNumber(wsCtx,serialNumber);
        if (LVar1 == LIB_STATUS_SET_FAILURE) {
          puts("SERIAL NUMBER Failure");
          wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
          local_10 = LIB_STATUS_SET_FAILURE;
        }
        else {
          LVar1 = setLatitudeAndLongitude(wsCtx,latString,lonString);
          if (LVar1 == LIB_STATUS_SET_FAILURE) {
            puts("Lat and Long Failure");
            wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
            local_10 = LIB_STATUS_SET_FAILURE;
          }
          else {
            local_10 = setWhiteSpaceContextAddress
                                 (wsCtx,ownerName,contName,contStreetAddress,contactCity,
                                  contactPostalCode,contactProvince,contactCountry,contactEmail,
                                  contactPhoneNumber);
            if (local_10 == LIB_STATUS_SET_FAILURE) {
              puts("Address Failure");
              wsCtx->libraryState = LIB_STATE_INIT_FAILURE;
            }
            else {
              wsCtx->libraryState = LIB_STATE_INIT_SUCCESS;
            }
          }
        }
      }
    }
  }
  return local_10;
}


// ==== setHostURL @ 00408348

LibraryInterfaceStatus setHostURL(WhiteSpaceContext *context,char *hostURL)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(hostURL);
  if (sVar1 < 200) {
    strcpy(context->hostURL,hostURL);
    local_10 = LIB_STATUS_SET_SUCCESS;
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== setComPortNumber @ 004083d0

void setComPortNumber(WhiteSpaceContext *context,int portNumber)

{
  context->comPortNumber = portNumber;
  return;
}


// ==== setRegulatoryDomain @ 00408404

LibraryInterfaceStatus setRegulatoryDomain(WhiteSpaceContext *context,char *regDomain)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(regDomain);
  if (sVar1 < 3) {
    strcpy(context->regulatoryDomain,regDomain);
    local_10 = LIB_STATUS_SET_SUCCESS;
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== setWhiteSpaceContextAddress @ 00408490

LibraryInterfaceStatus
setWhiteSpaceContextAddress
          (WhiteSpaceContext *wsCtx,char *ownerName,char *contName,char *contStreetAddress,
          char *contactCity,char *contactPostalCode,char *contactProvince,char *contactCountry,
          char *contactEmail,char *contactPhoneNumber)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(ownerName);
  if (sVar1 < 100) {
    strcpy((wsCtx->addressInfo).ownerName,ownerName);
    sVar1 = strlen(contName);
    if (sVar1 < 100) {
      strcpy((wsCtx->addressInfo).contactName,contName);
      sVar1 = strlen(contStreetAddress);
      if (sVar1 < 100) {
        strcpy((wsCtx->addressInfo).contactStreetAddress,contStreetAddress);
        sVar1 = strlen(contactCity);
        if (sVar1 < 0x30) {
          strcpy((wsCtx->addressInfo).contactCity,contactCity);
          sVar1 = strlen(contactPostalCode);
          if (sVar1 < 0x14) {
            strcpy((wsCtx->addressInfo).contactCity,contactCity);
            sVar1 = strlen(contactProvince);
            if (sVar1 < 4) {
              strcpy((wsCtx->addressInfo).contactProvince,contactProvince);
              sVar1 = strlen(contactCountry);
              if (sVar1 < 4) {
                strcpy((wsCtx->addressInfo).contactCountry,contactCountry);
                sVar1 = strlen(contactEmail);
                if (sVar1 < 100) {
                  strcpy((wsCtx->addressInfo).contactEmail,contactEmail);
                  sVar1 = strlen(contactPhoneNumber);
                  if (sVar1 < 0x20) {
                    strcpy((wsCtx->addressInfo).contactPhoneNumber,contactPhoneNumber);
                    local_10 = LIB_STATUS_SET_SUCCESS;
                  }
                  else {
                    local_10 = LIB_STATUS_SET_FAILURE;
                  }
                }
                else {
                  local_10 = LIB_STATUS_SET_FAILURE;
                }
              }
              else {
                local_10 = LIB_STATUS_SET_FAILURE;
              }
            }
            else {
              local_10 = LIB_STATUS_SET_FAILURE;
            }
          }
          else {
            local_10 = LIB_STATUS_SET_FAILURE;
          }
        }
        else {
          local_10 = LIB_STATUS_SET_FAILURE;
        }
      }
      else {
        local_10 = LIB_STATUS_SET_FAILURE;
      }
    }
    else {
      local_10 = LIB_STATUS_SET_FAILURE;
    }
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== setModeInContext @ 004087e4

void setModeInContext(WhiteSpaceContext *context,DevMode devMode)

{
  context->mode = devMode;
  return;
}


// ==== setChannelLookUpModeInContext @ 00408818

void setChannelLookUpModeInContext(WhiteSpaceContext *context,DevMode chDevMode)

{
  context->channelLookUpMode = chDevMode;
  return;
}


// ==== setAntennaeHeightInMeters @ 0040884c

void setAntennaeHeightInMeters(WhiteSpaceContext *context,int antHeight)

{
  context->antennaeHeightInMeters = antHeight;
  return;
}


// ==== setFccId @ 00408880

LibraryInterfaceStatus setFccId(WhiteSpaceContext *context,char *fccId)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(fccId);
  if (sVar1 < 0x18) {
    strncpy(context->fccID,fccId,0x18);
    local_10 = LIB_STATUS_SET_SUCCESS;
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== setSerialNumber @ 00408910

LibraryInterfaceStatus setSerialNumber(WhiteSpaceContext *context,char *serialNumber)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(serialNumber);
  if (sVar1 < 0x20) {
    strncpy(context->serialNumber,serialNumber,0x20);
    local_10 = LIB_STATUS_SET_SUCCESS;
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== setLatitudeAndLongitude @ 004089a0

LibraryInterfaceStatus
setLatitudeAndLongitude(WhiteSpaceContext *context,char *latString,char *lonString)

{
  size_t sVar1;
  LibraryInterfaceStatus local_10;
  
  sVar1 = strlen(latString);
  if ((sVar1 < 100) && (sVar1 = strlen(lonString), sVar1 < 100)) {
    strncpy(context->latitudeString,latString,100);
    strncpy(context->longitudeString,lonString,100);
    local_10 = LIB_STATUS_SET_SUCCESS;
  }
  else {
    local_10 = LIB_STATUS_SET_FAILURE;
  }
  return local_10;
}


// ==== getRegistrationStatus @ 00408a78

WSServiceStatus getRegistrationStatus(WhiteSpaceContext *context)

{
  return context->mode;
}


// ==== getRegistrationErrorCode @ 00408aa4

int getRegistrationErrorCode(WhiteSpaceContext *context)

{
  return context->registrationErrorCode;
}


// ==== getChannelRequestStatus @ 00408ad0

WSServiceStatus getChannelRequestStatus(WhiteSpaceContext *context)

{
  return context->channelRequestStatus;
}


// ==== getChannelRequestErrorCode @ 00408afc

int getChannelRequestErrorCode(WhiteSpaceContext *context)

{
  return context->channelRequestErrorCode;
}


// ==== getLibraryState @ 00408b28

LibraryState getLibraryState(WhiteSpaceContext *context)

{
  return context->libraryState;
}


// ==== printChannelMap @ 00408b54

void printChannelMap(WhiteSpaceContext *context)

{
  return;
}


// ==== createChannelRequestStatusFile @ 00408b78

void createChannelRequestStatusFile(WhiteSpaceContext *context)

{
  FILE *__s;
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: FILE * fp@[???] */
  __s = fopen("/etc/config/channel_status","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'current\' \'channel_request_status\'\n",1,0x2b,__s);
    fprintf(__s,"\t option \'status\' \'%d\'\n",context->channelRequestStatus);
    fwrite(" config \'current\' \'channel_request_timestamp\'\n",1,0x2e,__s);
    fprintf(__s,"\t option \'time_stamp\' \'%s\'\n",context->channelRequestTimeStamp);
  }
  putc(10,__s);
  fflush(__s);
  fclose(__s);
  return;
}


// ==== createChannelMapFile @ 00408cb0

void createChannelMapFile(WhiteSpaceContext *context)

{
  FILE *__s;
  uint local_c;
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: FILE * fp@[???] */
  __s = fopen("/etc/config/channels","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'channel\' \'channel\'\n",1,0x1c,__s);
    fwrite("\t option \'channels\' \'",1,0x15,__s);
    if ((context->availableChannelMap).channelCount != 0) {
      for (local_c = 0; local_c < (context->availableChannelMap).channelCount; local_c = local_c + 1
          ) {
        if (local_c + 1 == (context->availableChannelMap).channelCount) {
          fprintf(__s,"%d",(context->availableChannelMap).channels[local_c]);
        }
        else {
          fprintf(__s,"%d,",(context->availableChannelMap).channels[local_c]);
        }
      }
    }
    fwrite(&DAT_00517da4,1,2,__s);
    fflush(__s);
    fclose(__s);
  }
  return;
}


// ==== createRegistrationStatusFile @ 00408e78

void createRegistrationStatusFile(WhiteSpaceContext *context)

{
  FILE *__s;
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: FILE * fp@[???] */
  __s = fopen("/etc/config/registration","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'current\' \'registration_status\'\n",1,0x28,__s);
    fprintf(__s,"\t option \'status\' \'%d\'\n",context->registrationStatus);
    fwrite(" config \'current\' \'registration_request_timestamp\'\n",1,0x33,__s);
    fprintf(__s,"\t option \'time_stamp\' \'%s\'\n",context->registrationTimeStamp);
  }
  putc(10,__s);
  fflush(__s);
  fclose(__s);
  return;
}


// ==== printPreferredChannelMap @ 00408fb0

void printPreferredChannelMap(WhiteSpaceContext *context)

{
  return;
}


// ==== getChannelCountFromChannelMap @ 00408fd4

int getChannelCountFromChannelMap(WhiteSpaceContext *context)

{
  return (context->availableChannelMap).channelCount;
}


// ==== calculateDeltaTimeFromCurrentInSecs @ 00409000

int calculateDeltaTimeFromCurrentInSecs(WhiteSpaceContext *context)

{
  double in_f0_1;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  
                    /* Unresolved local var: int hr@[???]
                       Unresolved local var: int min@[???]
                       Unresolved local var: int sec@[???]
                       Unresolved local var: int deltaHr@[???]
                       Unresolved local var: int deltaMin@[???]
                       Unresolved local var: int deltaSec@[???]
                       Unresolved local var: int totalDeltaSecCount@[???]
                       Unresolved local var: int hrOffset@[???] */
  sscanf(context->absoluteTimeBuffer,"%d:%d:%d",&local_14,&local_10,&local_c);
  atof(context->longitudeString);
  in_f0_1 = in_f0_1 / DAT_00517e30;
  ceil(in_f0_1);
  local_18 = 0x17 - (local_14 + (int)in_f0_1);
  if (local_18 < 0) {
    local_18 = 0x18 - local_18;
  }
  return local_18 * 0xe10 + (0x3b - local_10) * 0x3c + (0x3b - local_c);
}


// ==== reAdjustChannelMapToMeldCapabilities @ 00409140

void reAdjustChannelMapToMeldCapabilities(WhiteSpaceContext *context)

{
  uint local_110;
  uint local_10c;
  uint auStack_108 [65];
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: int nIndex@[???]
                       Unresolved local var: int[64] availChannels@[???] */
  local_110 = 0;
  if ((context->availableChannelMap).channelCount != 0) {
    for (local_10c = 0; local_10c < (context->availableChannelMap).channelCount;
        local_10c = local_10c + 1) {
      if ((0x14 < (context->availableChannelMap).channels[local_10c]) &&
         ((context->availableChannelMap).channels[local_10c] < 0x34)) {
        auStack_108[local_110] = (context->availableChannelMap).channels[local_10c];
        local_110 = local_110 + 1;
      }
    }
    (context->availableChannelMap).channelCount = local_110;
    for (local_10c = 0; (int)local_10c < (int)local_110; local_10c = local_10c + 1) {
      (context->availableChannelMap).channels[local_10c] = auStack_108[local_10c];
    }
  }
  return;
}


// ==== initializeSecurityCtx @ 00409290

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void initializeSecurityCtx(void)

{
  SSL_library_init();
  SSL_load_error_strings();
  RAND_seed("This is a Random Number",0x18);
  return;
}


// ==== pexit @ 004092f4

void pexit(char *s)

{
  perror(s);
  return;
}


// ==== log_ssl @ 00409334

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int log_ssl(void)

{
  ulong e;
  char acStack_10c [260];
  
                    /* Unresolved local var: char[256] buf@[???]
                       Unresolved local var: u_long err@[???] */
  while( true ) {
    e = ERR_get_error();
    if (e == 0) break;
    ERR_error_string_n(e,acStack_10c,0x100);
    printf("*** %s\n",acStack_10c);
  }
  return 0;
}


// ==== recv_to @ 004093c8

int recv_to(int fd,char *buffer,int len,int flags,int to)

{
  int iVar1;
  uint local_a4;
  fd_set fStack_98;
  timeval local_18;
  ssize_t local_10;
  
                    /* Unresolved local var: fd_set readset@[???]
                       Unresolved local var: int result@[???]
                       Unresolved local var: int iof@[???]
                       Unresolved local var: timeval tv@[???] */
                    /* Unresolved local var: uint32_t __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_a4 = 0; local_a4 < 0x20; local_a4 = local_a4 + 1) {
    fStack_98.fds_bits[local_a4] = 0;
  }
  fStack_98.fds_bits[(uint)fd >> 5] = fStack_98.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_18.tv_sec = 0;
  local_18.tv_usec = to * 1000;
  iVar1 = select(fd + 1,&fStack_98,(fd_set *)0x0,(fd_set *)0x0,&local_18);
  if (iVar1 < 0) {
    local_10 = -1;
  }
  else if ((iVar1 < 1) || ((fStack_98.fds_bits[(uint)fd >> 5] >> (fd & 0x1fU) & 1U) == 0)) {
    local_10 = -2;
  }
  else {
    iVar1 = fcntl(fd,3);
    if (iVar1 != -1) {
      fcntl(fd,4);
    }
    local_10 = recv(fd,buffer,len,flags);
    if (iVar1 != -1) {
      fcntl(fd,4);
    }
  }
  return local_10;
}


// ==== recv_to_ssl @ 00409604

int recv_to_ssl(SSL *ssl,char *buffer,int len,int to)

{
  int iVar1;
  
                    /* Unresolved local var: int result@[???]
                       Unresolved local var: timeval tv@[???] */
  iVar1 = SSL_read((SSL *)ssl,buffer,len);
  return iVar1;
}


// ==== create_tcp_socket @ 0040968c

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int create_tcp_socket(void)

{
  int iVar1;
  
                    /* Unresolved local var: int sock@[???] */
  iVar1 = socket(2,2,6);
  if (iVar1 < 0) {
    pexit("Can\'t create TCP socket");
  }
  return iVar1;
}


// ==== get_ip @ 004096f8

int get_ip(char *host,char *ip)

{
  hostent *phVar1;
  char *pcVar2;
  int local_10;
  
                    /* Unresolved local var: hostent * hent@[???]
                       Unresolved local var: int iplen@[???] */
  memset(ip,0,0x10);
  phVar1 = gethostbyname(host);
  if (phVar1 == (hostent *)0x0) {
    pexit("Can\'t get IP");
    local_10 = 0;
  }
  else {
    pcVar2 = inet_ntop(2,*phVar1->h_addr_list,ip,0xf);
    if (pcVar2 == (char *)0x0) {
      pexit("Can\'t resolve host");
      local_10 = 0;
    }
    else {
      local_10 = 1;
    }
  }
  return local_10;
}


// ==== verify_callback @ 004097f4

int verify_callback(int ok,X509_STORE_CTX *ctx)

{
  X509_NAME *a;
  int iVar1;
  int local_res0;
  char acStack_110 [264];
  
                    /* Unresolved local var: char * s@[???]
                       Unresolved local var: char[256] buf@[???]
                       Unresolved local var: X509 * err_cert@[???] */
  X509_STORE_CTX_get_current_cert((X509_STORE_CTX *)ctx);
  a = X509_get_subject_name((X509 *)ctx->current_cert);
  X509_NAME_oneline(a,acStack_110,0x100);
  local_res0 = ok;
  if (ok == 0) {
    iVar1 = ctx->error;
    if (iVar1 == 10) {
      local_res0 = 1;
    }
    else if (iVar1 == 0x12) {
      local_res0 = 1;
    }
    else if (iVar1 == 9) {
      local_res0 = 1;
    }
    else {
      local_res0 = 0;
    }
  }
  return local_res0;
}


// ==== sslShutDown @ 004098fc

void sslShutDown(WhiteSpaceContext *wsCtx,SSL_CTX **ctx,SSL **ssl)

{
  SSL_shutdown((SSL *)*ssl);
  if (0 < wsCtx->activeSockForSession) {
    close(wsCtx->activeSockForSession);
  }
  SSL_free((SSL *)*ssl);
  SSL_CTX_free((SSL_CTX *)*ctx);
  return;
}


// ==== establishASecureSession @ 004099b0

SslStatus establishASecureSession(WhiteSpaceContext *wsCtx,SSL_CTX **ctx,SSL **ssl)

{
  SSL_METHOD *meth;
  SSL_CTX *pSVar1;
  SSL *pSVar2;
  int iVar3;
  int *piVar4;
  long lVar5;
  SslStatus local_48;
  char acStack_38 [20];
  sockaddr local_24;
  SslStatus local_10;
  
                    /* Unresolved local var: int status@[???]
                       Unresolved local var: char[20] ipAddress@[???]
                       Unresolved local var: sockaddr_in remote@[???]
                       Unresolved local var: int tmpres@[???]
                       Unresolved local var: int err_ssl@[???]
                       Unresolved local var: SslStatus sslStatus@[???] */
  wsCtx->activeSockForSession = 0;
  meth = SSLv23_client_method();
  pSVar1 = SSL_CTX_new(meth);
  *ctx = (SSL_CTX *)pSVar1;
  if (*ctx == (SSL_CTX *)0x0) {
    puts("SSl CTX New returned NULL");
    local_10 = SSL_CONTEXT_ALLOC_FAILURE;
  }
  else {
    pSVar2 = SSL_new((SSL_CTX *)*ctx);
    *ssl = (SSL *)pSVar2;
    if (ssl == (SSL **)0x0) {
      puts("SSl New returned NULL");
      SSL_CTX_free((SSL_CTX *)*ctx);
      local_10 = SSL_NEW_FAILURE;
    }
    else {
      iVar3 = SSL_CTX_load_verify_locations
                        ((SSL_CTX *)*ctx,wsCtx->securityCertFileName,wsCtx->securityCertPath);
      if (iVar3 == 0) {
        log_ssl();
      }
      SSL_CTX_set_verify((SSL_CTX *)*ctx,1,verify_callback);
      memset(acStack_38,0,0x14);
      iVar3 = get_ip(wsCtx->hostURL,acStack_38);
      if (iVar3 == 0) {
        SSL_shutdown((SSL *)*ssl);
        SSL_free((SSL *)*ssl);
        SSL_CTX_free((SSL_CTX *)*ctx);
        local_10 = SSL_SESSION_FAILURE;
      }
      else {
        iVar3 = create_tcp_socket();
        wsCtx->activeSockForSession = iVar3;
        if (wsCtx->activeSockForSession < 0) {
          SSL_shutdown((SSL *)*ssl);
          SSL_free((SSL *)*ssl);
          SSL_CTX_free((SSL_CTX *)*ctx);
          local_10 = SSL_SESSION_FAILURE;
        }
        else {
          local_24.sa_family = 2;
          local_24.sa_data._0_2_ = htons(0x1bb);
          iVar3 = inet_pton(2,acStack_38,local_24.sa_data + 2);
          if (iVar3 < 0) {
            pexit("Can\'t set remote->sin_addr.s_addr");
            SSL_shutdown((SSL *)*ssl);
            close(wsCtx->activeSockForSession);
            SSL_free((SSL *)*ssl);
            SSL_CTX_free((SSL_CTX *)*ctx);
            local_10 = SSL_SESSION_FAILURE;
          }
          else if (iVar3 == 0) {
            fprintf((FILE *)stderr,"%s is not a valid IP address\n",acStack_38);
            SSL_shutdown((SSL *)*ssl);
            close(wsCtx->activeSockForSession);
            SSL_free((SSL *)*ssl);
            SSL_CTX_free((SSL_CTX *)*ctx);
            local_10 = SSL_SESSION_FAILURE;
          }
          else {
            iVar3 = connect(wsCtx->activeSockForSession,&local_24,0x10);
            if (iVar3 < 0) {
              piVar4 = __errno_location();
              printf("Could not connect %d Errno:%d \n",iVar3,*piVar4);
              (wsCtx->availableChannelMap).channelCount = 0;
              SSL_shutdown((SSL *)*ssl);
              close(wsCtx->activeSockForSession);
              SSL_free((SSL *)*ssl);
              SSL_CTX_free((SSL_CTX *)*ctx);
              local_10 = SSL_SESSION_FAILURE;
            }
            else {
              iVar3 = SSL_set_fd((SSL *)*ssl,wsCtx->activeSockForSession);
              if (iVar3 == 0) {
                iVar3 = SSL_get_error((SSL *)*ssl,0);
                printf("SSL SetFd failed: Status %d Reason:%d \n",0,iVar3);
                SSL_shutdown((SSL *)*ssl);
                close(wsCtx->activeSockForSession);
                SSL_free((SSL *)*ssl);
                SSL_CTX_free((SSL_CTX *)*ctx);
                local_10 = SSL_SESSION_FAILURE;
              }
              else {
                iVar3 = SSL_connect((SSL *)*ssl);
                if (iVar3 == 1) {
                  local_10 = SSL_STATUS_SUCCESS;
                }
                else {
                  puts("SSL Handshake failed");
                  lVar5 = SSL_get_verify_result((SSL *)*ssl);
                  if (lVar5 == 0) {
                    local_48 = SSL_SESSION_FAILURE;
                  }
                  else {
                    local_48 = SSL_CERTIFICATE_VERIFY_FAILURE;
                  }
                  log_ssl();
                  SSL_shutdown((SSL *)*ssl);
                  close(wsCtx->activeSockForSession);
                  SSL_free((SSL *)*ssl);
                  SSL_CTX_free((SSL_CTX *)*ctx);
                  local_10 = local_48;
                }
              }
            }
          }
        }
      }
    }
  }
  return local_10;
}


// ==== sslSendMessageToService @ 0040a110

SslStatus sslSendMessageToService(WhiteSpaceContext *wsCtx,SSL_CTX *ctx,SSL *ssl)

{
  size_t num;
  int iVar1;
  SslStatus local_18;
  
                    /* Unresolved local var: int tmpres@[???] */
  num = strlen(wsCtx->transmitBuffer);
  iVar1 = SSL_write((SSL *)ssl,wsCtx->transmitBuffer,num);
  if (iVar1 == -1) {
    printf("Can\'t send query %d\n",-1);
    close(wsCtx->activeSockForSession);
    SSL_shutdown((SSL *)ssl);
    SSL_free((SSL *)ssl);
    SSL_CTX_free((SSL_CTX *)ctx);
    local_18 = SSL_SESSION_FAILURE;
  }
  else {
    SSL_write((SSL *)ssl,&DAT_00517f8c,3);
    local_18 = SSL_STATUS_SUCCESS;
  }
  return local_18;
}


// ==== sslReceiveMessage @ 0040a24c

int sslReceiveMessage(WhiteSpaceContext *wsCtx,SSL_CTX *ctx,SSL *ssl)

{
  size_t __n;
  char *local_20;
  size_t local_1c;
  int local_14;
  
                    /* Unresolved local var: int timeout@[???]
                       Unresolved local var: int length@[???]
                       Unresolved local var: int tmpres@[???]
                       Unresolved local var: int delta@[???]
                       Unresolved local var: char * startLocation@[???] */
  local_14 = 0;
  local_1c = 0;
  memset(wsCtx->receiveBuffer,0,0x400);
  memset(wsCtx->tempProcessingBuffer,0,0x400);
  local_20 = wsCtx->receiveBuffer;
  while( true ) {
    __n = recv_to_ssl(ssl,wsCtx->tempProcessingBuffer,0x400,5000);
    if ((int)__n < 1) break;
    local_20 = local_20 + local_1c;
    strncpy(local_20,wsCtx->tempProcessingBuffer,__n);
    local_14 = local_14 + __n;
    local_1c = __n;
  }
  return local_14;
}


// ==== processWS_STATE_IDLE @ 0040a380

WhiteSpaceState
processWS_STATE_IDLE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  return WS_STATE_BACKGROUND_SCANNING;
}


// ==== processWS_STATE_BACKGROUND_SCANNING @ 0040a3b4

WhiteSpaceState
processWS_STATE_BACKGROUND_SCANNING
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_CHANNEL_LOCKED;
  return WS_STATE_ASSOCIATING;
}


// ==== processWS_STATE_ASSOCIATING @ 0040a3f4

WhiteSpaceState
processWS_STATE_ASSOCIATING
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_ASSOCIATED;
  return WS_STATE_OBTAINING_IP;
}


// ==== processWS_STATE_OBTAINING_IP @ 0040a434

WhiteSpaceState
processWS_STATE_OBTAINING_IP
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_IP_OBTAINED;
  return WS_STATE_SERVICE_RESOLUTION;
}


// ==== processWS_STATE_SERVICE_RESOLUTION @ 0040a474

WhiteSpaceState
processWS_STATE_SERVICE_RESOLUTION
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_SERVICE_RESOLVED;
  return WS_STATE_REGISTRATION;
}


// ==== processWS_STATE_REGISTRATION @ 0040a4b4

WhiteSpaceState
processWS_STATE_REGISTRATION
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_REGISTRATION_REQUEST_SEND;
  return WS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION;
}


// ==== processWS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION @ 0040a4f4

WhiteSpaceState
processWS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  SslStatus SVar1;
  WhiteSpaceState local_10;
  
                    /* Unresolved local var: WhiteSpaceState state@[???] */
  SVar1 = establishASecureSession(wsCtx,sslCtx,ssl);
  if (SVar1 == SSL_STATUS_SUCCESS) {
    if (*event == WS_EVENT_REGISTRATION_REQUEST_SEND) {
      local_10 = WS_STATE_REGISTRATION_SEND;
    }
    else {
      local_10 = WS_STATE_CHANNEL_REQUEST_SEND;
    }
    *event = WS_EVENT_SECURE_SESSION_ESTABLISHED;
  }
  else {
    if (*event == WS_EVENT_REGISTRATION_REQUEST_SEND) {
      pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
      wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
      wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
      (wsCtx->availableChannelMap).channelCount = 0;
      pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      createRegistrationStatusFile(wsCtx);
      createChannelMapFile(wsCtx);
      createChannelRequestStatusFile(wsCtx);
      local_10 = WS_STATE_REGISTRATION;
    }
    else {
      pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
      wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
      (wsCtx->availableChannelMap).channelCount = 0;
      pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      createChannelMapFile(wsCtx);
      createChannelRequestStatusFile(wsCtx);
      local_10 = WS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE;
    }
    *event = WS_EVENT_SECURE_SESSION_FAILED;
    wsCtx->libraryState = LIB_STATE_SECURITY_FAILURE;
  }
  return local_10;
}


// ==== processWS_STATE_REGISTRATION_SEND @ 0040a6c0

WhiteSpaceState
processWS_STATE_REGISTRATION_SEND
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  int iVar1;
  SslStatus SVar2;
  WhiteSpaceState local_18;
  
                    /* Unresolved local var: int lengthOfTheXmitMsg@[???]
                       Unresolved local var: WhiteSpaceState state@[???] */
  local_18 = WS_STATE_WAIT_FOR_REGISTRATION_RESPONSE;
  if (*event == WS_EVENT_SECURE_SESSION_ESTABLISHED) {
    iVar1 = buildRegistrationRequest(wsCtx);
    if (0 < iVar1) {
      SVar2 = sslSendMessageToService(wsCtx,*sslCtx,*ssl);
      if (SVar2 == SSL_STATUS_SUCCESS) {
        *event = WS_EVENT_REGISTRATION_REQUEST_SEND;
        wsCtx->libraryState = LIB_STATE_AWAITING_REGISTRATION_REPLY;
      }
      else {
        *event = WS_EVENT_REGISTRATION_REQUEST_SEND_FAILED;
        wsCtx->libraryState = LIB_STATE_REGISTRATION_FAILURE;
        pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
        wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
        wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
        createChannelRequestStatusFile(wsCtx);
        createRegistrationStatusFile(wsCtx);
      }
    }
  }
  else if (*event == WS_EVENT_SECURE_SESSION_FAILED) {
    pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
    wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
    wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
    pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
    createChannelRequestStatusFile(wsCtx);
    createRegistrationStatusFile(wsCtx);
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_18 = WS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION;
    wsCtx->libraryState = LIB_STATE_SECURITY_FAILURE;
  }
  return local_18;
}


// ==== processWS_STATE_WAIT_FOR_REGISTRATION_RESPONSE @ 0040a8e0

WhiteSpaceState
processWS_STATE_WAIT_FOR_REGISTRATION_RESPONSE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  int iVar1;
  WSServiceStatus WVar2;
  
                    /* Unresolved local var: WhiteSpaceState state@[???] */
  wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
  if (*event == WS_EVENT_REGISTRATION_REQUEST_SEND) {
    iVar1 = sslReceiveMessage(wsCtx,*sslCtx,*ssl);
    if (iVar1 < 1) {
      pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
      createRegistrationStatusFile(wsCtx);
      (wsCtx->availableChannelMap).channelCount = 0;
      wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
      pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      createChannelMapFile(wsCtx);
      createChannelRequestStatusFile(wsCtx);
      *event = WS_EVENT_REGISTRATION_RESPONSE_TIMEOUT;
      wsCtx->libraryState = LIB_STATE_REGISTRATION_RESPONSE_TIMEOUT;
    }
    else {
      iVar1 = httpHeaderStatus(wsCtx,WS_REGISTRATION_MESSAGE);
      if (iVar1 == 200) {
        WVar2 = getWhiteSpaceStatus(wsCtx,WS_REGISTRATION_MESSAGE);
        if (WVar2 == WS_SUCCESS) {
          createRegistrationStatusFile(wsCtx);
          *event = WS_EVENT_REGISTRATION_RESPONSE_RCVD;
          wsCtx->libraryState = LIB_STATE_REGISTRATION_SUCCESS;
        }
        else {
          createRegistrationStatusFile(wsCtx);
          pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
          (wsCtx->availableChannelMap).channelCount = 0;
          wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
          pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
          createChannelMapFile(wsCtx);
          createChannelRequestStatusFile(wsCtx);
          *event = WS_EVENT_REGISTRATION_RESPONSE_FAILED;
          wsCtx->libraryState = LIB_STATE_REGISTRATION_FAILURE;
        }
      }
      else {
        pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
        createRegistrationStatusFile(wsCtx);
        (wsCtx->availableChannelMap).channelCount = 0;
        wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
        createChannelMapFile(wsCtx);
        createChannelRequestStatusFile(wsCtx);
        *event = WS_EVENT_REGISTRATION_RESPONSE_FAILED;
        wsCtx->libraryState = LIB_STATE_REGISTRATION_FAILURE;
      }
    }
  }
  return WS_STATE_SECURE_SESSION_SHUTDOWN;
}


// ==== processWS_STATE_SECURE_SESSION_SHUTDOWN @ 0040abb4

WhiteSpaceState
processWS_STATE_SECURE_SESSION_SHUTDOWN
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  WhiteSpaceState local_18;
  
                    /* Unresolved local var: WhiteSpaceState state@[???] */
  local_18 = WS_STATE_CHANNEL_REQUEST;
  sslShutDown(wsCtx,sslCtx,ssl);
  switch(*event) {
  case WS_EVENT_REGISTRATION_REQUEST_SEND_FAILED:
  case WS_EVENT_REGISTRATION_RESPONSE_TIMEOUT:
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_18 = WS_STATE_IDLE;
    break;
  case WS_EVENT_REGISTRATION_RESPONSE_RCVD:
    local_18 = WS_STATE_CHANNEL_REQUEST;
    break;
  case WS_EVENT_REGISTRATION_RESPONSE_FAILED:
  case WS_EVENT_CHANNEL_RESPONSE_RCVD:
  case WS_EVENT_CHANNEL_RESPONSE_FAILED:
  case WS_EVENT_NO_CHANNEL_AVAILABLE:
    local_18 = WS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE;
    break;
  case WS_EVENT_CHANNEL_REQUEST_SEND_FAILED:
  case WS_EVENT_CHANNEL_RESPONSE_TIMEOUT:
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_18 = WS_STATE_CHANNEL_REQUEST;
  }
  return local_18;
}


// ==== processWS_STATE_CHANNEL_REQUEST @ 0040acdc

WhiteSpaceState
processWS_STATE_CHANNEL_REQUEST
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  *event = WS_EVENT_CHANNEL_REQUEST_SEND;
  return WS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION;
}


// ==== processWS_STATE_CHANNEL_REQUEST_SEND @ 0040ad1c

WhiteSpaceState
processWS_STATE_CHANNEL_REQUEST_SEND
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  int iVar1;
  SslStatus SVar2;
  WhiteSpaceState local_18;
  
                    /* Unresolved local var: int lengthOfTheXmitMsg@[???]
                       Unresolved local var: WhiteSpaceState state@[???] */
  local_18 = WS_STATE_WAIT_FOR_CHANNEL_RESPONSE;
  if (*event == WS_EVENT_SECURE_SESSION_ESTABLISHED) {
    iVar1 = buildChannelRequest(wsCtx);
    if (0 < iVar1) {
      SVar2 = sslSendMessageToService(wsCtx,*sslCtx,*ssl);
      if (SVar2 == SSL_STATUS_SUCCESS) {
        *event = WS_EVENT_CHANNEL_REQUEST_SEND;
        wsCtx->libraryState = LIB_STATE_AWAITING_CHANNEL_RESPONSE;
      }
      else {
        *event = WS_EVENT_CHANNEL_REQUEST_SEND_FAILED;
        wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
        pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
        (wsCtx->availableChannelMap).channelCount = 0;
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
        createChannelMapFile(wsCtx);
      }
    }
  }
  else if (*event == WS_EVENT_SECURE_SESSION_FAILED) {
    pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
    (wsCtx->availableChannelMap).channelCount = 0;
    pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
    createChannelMapFile(wsCtx);
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_18 = WS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION;
    wsCtx->libraryState = LIB_STATE_SECURITY_FAILURE;
  }
  return local_18;
}


// ==== processWS_STATE_WAIT_FOR_CHANNEL_RESPONSE @ 0040aef4

WhiteSpaceState
processWS_STATE_WAIT_FOR_CHANNEL_RESPONSE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  int iVar1;
  WSServiceStatus WVar2;
  
                    /* Unresolved local var: WhiteSpaceState state@[???] */
  if (*event == WS_EVENT_CHANNEL_REQUEST_SEND) {
    iVar1 = sslReceiveMessage(wsCtx,*sslCtx,*ssl);
    if (iVar1 < 1) {
      pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
      (wsCtx->availableChannelMap).channelCount = 0;
      pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      createChannelMapFile(wsCtx);
      *event = WS_EVENT_CHANNEL_RESPONSE_TIMEOUT;
      wsCtx->libraryState = LIB_STATE_CHANNEL_RESPONSE_TIMEOUT;
    }
    else {
      iVar1 = httpHeaderStatus(wsCtx,WS_CHANNEL_REQUEST_MESSAGE);
      if (iVar1 == 200) {
        pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
        WVar2 = getWhiteSpaceStatus(wsCtx,WS_CHANNEL_REQUEST_MESSAGE);
        if (WVar2 == WS_SUCCESS) {
          iVar1 = processXmlResponseMessage(wsCtx);
          if (iVar1 == 0) {
            iVar1 = getChannelCountFromChannelMap(wsCtx);
            if (iVar1 < 1) {
              (wsCtx->availableChannelMap).channelCount = 0;
              createChannelMapFile(wsCtx);
              *event = WS_EVENT_NO_CHANNEL_AVAILABLE;
              wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
            }
            else {
              reAdjustChannelMapToMeldCapabilities(wsCtx);
              printChannelMap(wsCtx);
              createChannelMapFile(wsCtx);
              *event = WS_EVENT_CHANNEL_RESPONSE_RCVD;
              wsCtx->libraryState = LIB_STATE_CHANNEL_AVAILABLE;
            }
          }
          else {
            (wsCtx->availableChannelMap).channelCount = 0;
            createChannelMapFile(wsCtx);
            *event = WS_EVENT_CHANNEL_RESPONSE_FAILED;
            wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
          }
        }
        else {
          (wsCtx->availableChannelMap).channelCount = 0;
          createChannelMapFile(wsCtx);
          *event = WS_EVENT_CHANNEL_RESPONSE_FAILED;
          wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
        }
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      }
      else {
        pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
        (wsCtx->availableChannelMap).channelCount = 0;
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
        createChannelMapFile(wsCtx);
        *event = WS_EVENT_CHANNEL_RESPONSE_FAILED;
        wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
      }
    }
  }
  createChannelRequestStatusFile(wsCtx);
  return WS_STATE_SECURE_SESSION_SHUTDOWN;
}


// ==== processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE @ 0040b20c

WhiteSpaceState
processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)

{
  uint local_18;
  WhiteSpaceState local_14;
  
                    /* Unresolved local var: WhiteSpaceState state@[???]
                       Unresolved local var: int timeOut@[???] */
  wsCtx->libraryState = LIB_STATE_WAITING_FOR_RENEWAL;
  switch(*event) {
  case WS_EVENT_SECURE_SESSION_FAILED:
  case WS_EVENT_CHANNEL_REQUEST_SEND_FAILED:
  case WS_EVENT_CHANNEL_RESPONSE_TIMEOUT:
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 0;
    break;
  case WS_EVENT_CHANNEL_RESPONSE_RCVD:
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 0;
  default:
    sleep(0x14cd0);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0x14cd0;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    break;
  case WS_EVENT_HTTP_STATUS_FAILED:
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount =
         processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount + 1;
    local_18 = processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount * 2;
    if (0x3c < (int)local_18) {
      processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 1;
      local_18 = 2;
    }
    sleep(local_18);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + local_18;
    break;
  case WS_EVENT_REGISTRATION_STATUS_FAILED:
    local_14 = WS_STATE_REGISTRATION;
    sleep(0xe10);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 0;
    break;
  case WS_EVENT_CHANNEL_REQUEST_STATUS_FAILED:
    local_14 = WS_STATE_CHANNEL_REQUEST;
    sleep(0xe10);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 0;
    break;
  case WS_EVENT_NO_CHANNEL_AVAILABLE:
    sleep(0xe10);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE::httpFailureCount = 0;
  }
  return local_14;
}


// ==== initProcessStateFunctions @ 0040b464

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void initProcessStateFunctions(void)

{
  processFunctions[1] = processWS_STATE_IDLE;
  processFunctions[2] = processWS_STATE_BACKGROUND_SCANNING;
  processFunctions[3] = processWS_STATE_ASSOCIATING;
  processFunctions[4] = processWS_STATE_OBTAINING_IP;
  processFunctions[5] = processWS_STATE_SERVICE_RESOLUTION;
  processFunctions[6] = processWS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION;
  processFunctions[7] = processWS_STATE_SECURE_SESSION_SHUTDOWN;
  processFunctions[8] = processWS_STATE_REGISTRATION;
  processFunctions[9] = processWS_STATE_REGISTRATION_SEND;
  processFunctions[10] = processWS_STATE_WAIT_FOR_REGISTRATION_RESPONSE;
  processFunctions[0xb] = processWS_STATE_CHANNEL_REQUEST;
  processFunctions[0xc] = processWS_STATE_CHANNEL_REQUEST_SEND;
  processFunctions[0xd] = processWS_STATE_WAIT_FOR_CHANNEL_RESPONSE;
  processFunctions[0xe] = processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE;
  return;
}


// ==== processWSStateMachine @ 0040b59c

void processWSStateMachine
               (WhiteSpaceContext *wsCtx,SSL_CTX **sslCtx,SSL **ssl,pthread_mutex_t *threadCondition
               )

{
  processWSStateMachine::wsState =
       (*processFunctions[processWSStateMachine::wsState])
                 (wsCtx,&processWSStateMachine::event,sslCtx,ssl,threadCondition);
  return;
}


// ==== postProcessDataForEscapeSequence @ 0040b620

void postProcessDataForEscapeSequence(char *data,char *convData)

{
  bool bVar1;
  size_t local_18;
  char *local_14;
  char *local_10;
  
                    /* Unresolved local var: char * loc@[???]
                       Unresolved local var: char * nloc@[???]
                       Unresolved local var: int length@[???] */
  local_18 = strlen(data);
  local_14 = convData;
  local_10 = data;
  while (bVar1 = local_18 != 0, local_18 = local_18 - 1, bVar1) {
    if (*local_10 == '&') {
      *local_14 = *local_10;
      local_10 = local_10 + 1;
      local_14[1] = 'a';
      local_14[2] = 'm';
      local_14[3] = 'p';
      local_14[4] = ';';
      local_14 = local_14 + 5;
    }
    else if (*local_10 == '<') {
      *local_14 = *local_10;
      local_10 = local_10 + 1;
      local_14[1] = 'l';
      local_14[2] = 't';
      local_14[3] = ';';
      local_14 = local_14 + 4;
    }
    else if (*local_10 == '>') {
      *local_14 = *local_10;
      local_10 = local_10 + 1;
      local_14[1] = 'g';
      local_14[2] = 't';
      local_14[3] = ';';
      local_14 = local_14 + 4;
    }
    else if (*local_10 == '\"') {
      *local_14 = *local_10;
      local_10 = local_10 + 1;
      local_14[1] = 'q';
      local_14[2] = 'u';
      local_14[3] = 'o';
      local_14[4] = 't';
      local_14[5] = ';';
      local_14 = local_14 + 6;
    }
    else if (*local_10 == '\'') {
      *local_14 = *local_10;
      local_10 = local_10 + 1;
      local_14[1] = 'a';
      local_14[2] = 'p';
      local_14[3] = 'o';
      local_14[4] = 's';
      local_14[5] = ';';
      local_14 = local_14 + 6;
    }
    else {
      *local_14 = *local_10;
      local_14 = local_14 + 1;
      local_10 = local_10 + 1;
    }
  }
  return;
}


// ==== readXmlIntoContext @ 0040b9fc

int readXmlIntoContext(char *fileName,WhiteSpaceContext *wsCtx)

{
  ezxml_t_conflict xml;
  ezxml_t_conflict peVar1;
  DevMode DVar2;
  uint uVar3;
  int iVar4;
  ezxml_t_conflict peVar5;
  MeldModulationMode MVar6;
  uint uVar7;
  char *local_510;
  uint local_4f4;
  uint local_4f0;
  char acStack_4ec [100];
  undefined2 local_488 [2];
  char acStack_484 [1028];
  char *local_80;
  char *local_7c;
  char *local_78;
  char *local_74;
  char *local_70;
  char *local_6c;
  char *local_68;
  char *local_64;
  char *local_60;
  char *local_5c;
  char *local_58;
  char *local_54;
  char *local_50;
  char *local_4c;
  char *local_48;
  char *local_44;
  char *local_40;
  char *local_3c;
  char *local_38;
  char *local_34;
  char *local_30;
  char *local_2c;
  char *local_28;
  char *local_24;
  char *local_20;
  char *local_1c;
  WhiteSpaceContext *local_18;
  uint local_14;
  
                    /* Unresolved local var: char[100] buffer@[???]
                       Unresolved local var: size_t i@[???]
                       Unresolved local var: size_t k@[???]
                       Unresolved local var: size_t j@[???]
                       Unresolved local var: ezxml_t f1@[???]
                       Unresolved local var: ezxml_t node@[???]
                       Unresolved local var: ezxml_t block@[???]
                       Unresolved local var: ezxml_t blockData@[???]
                       Unresolved local var: ezxml_t freqRangeMhz@[???] */
  xml = ezxml_parse_file(fileName);
  memcpy(wsCtx->regulatoryDomain,&DAT_00519e98,3);
  (wsCtx->preferredChannelMap).channelCount = 0;
  memset(wsCtx->meldStreamingIpAddress,0,100);
  for (local_4f0 = 0; local_4f0 < 9; local_4f0 = local_4f0 + 1) {
    switch(local_4f0) {
    case 0:
                    /* Unresolved local var: char[1024] chBuffer@[???]
                       Unresolved local var: char[2] delims@[???]
                       Unresolved local var: char * result@[???] */
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_1c = "";
      }
      else {
        local_1c = peVar5->txt;
      }
      local_18 = wsCtx;
      strcpy(wsCtx->hostURL,local_1c);
      break;
    case 1:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_20 = "";
      }
      else {
        local_20 = peVar5->txt;
      }
      iVar4 = atoi(local_20);
      wsCtx->comPortNumber = iVar4;
      break;
    case 2:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      local_24 = wsCtx->regulatoryDomain;
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_28 = "";
      }
      else {
        local_28 = peVar5->txt;
      }
      strcpy(local_24,local_28);
      break;
    case 3:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      for (local_4f4 = 0; local_4f4 < 2; local_4f4 = local_4f4 + 1) {
        peVar1 = ezxml_child(peVar5,readXmlIntoContext::locationTags[local_4f4]);
        local_14 = local_4f4;
        if (local_4f4 == 0) {
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_2c = "";
          }
          else {
            local_2c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_2c);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->latitudeString);
        }
        else if (local_4f4 == 1) {
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_30 = "";
          }
          else {
            local_30 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_30);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->longitudeString);
        }
      }
      break;
    case 4:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      for (local_4f4 = 0; local_4f4 < 0xf; local_4f4 = local_4f4 + 1) {
        peVar1 = ezxml_child(peVar5,readXmlIntoContext::tvwsDeviceTags[local_4f4]);
        switch(local_4f4) {
        case 0:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_34 = "";
          }
          else {
            local_34 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_34);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->fccID);
          break;
        case 1:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_38 = "";
          }
          else {
            local_38 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_38);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->serialNumber);
          break;
        case 2:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_3c = "";
          }
          else {
            local_3c = peVar1->txt;
          }
          DVar2 = atoi(local_3c);
          wsCtx->mode = DVar2;
          break;
        case 3:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_40 = "";
          }
          else {
            local_40 = peVar1->txt;
          }
          DVar2 = atoi(local_40);
          wsCtx->channelLookUpMode = DVar2;
          break;
        case 4:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_44 = "";
          }
          else {
            local_44 = peVar1->txt;
          }
          iVar4 = atoi(local_44);
          wsCtx->antennaeHeightInMeters = iVar4;
          break;
        case 5:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_48 = "";
          }
          else {
            local_48 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_48);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactCity);
          break;
        case 6:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_4c = "";
          }
          else {
            local_4c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_4c);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactCountry);
          break;
        case 7:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_50 = "";
          }
          else {
            local_50 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_50);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactEmail);
          break;
        case 8:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_54 = "";
          }
          else {
            local_54 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_54);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactName);
          break;
        case 9:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_58 = "";
          }
          else {
            local_58 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_58);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactPhoneNumber);
          break;
        case 10:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_5c = "";
          }
          else {
            local_5c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_5c);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactProvince);
          break;
        case 0xb:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_60 = "";
          }
          else {
            local_60 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_60);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactStreetAddress);
          break;
        case 0xc:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_64 = "";
          }
          else {
            local_64 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_64);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactPostalCode);
          break;
        case 0xd:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_68 = "";
          }
          else {
            local_68 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_68);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->regulatoryDomain);
          break;
        case 0xe:
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_6c = "";
          }
          else {
            local_6c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_6c);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).ownerName);
        }
      }
      break;
    case 5:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      local_488[0] = DAT_00519ea0;
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_70 = "";
      }
      else {
        local_70 = peVar5->txt;
      }
      strcpy(acStack_484,local_70);
      local_510 = strtok(acStack_484,(char *)local_488);
      while (local_510 != (char *)0x0) {
        uVar7 = (wsCtx->preferredChannelMap).channelCount;
        uVar3 = atoi(local_510);
        (wsCtx->preferredChannelMap).channels[uVar7] = uVar3;
        (wsCtx->preferredChannelMap).channelCount = (wsCtx->preferredChannelMap).channelCount + 1;
        local_510 = strtok((char *)0x0,(char *)local_488);
      }
      printPreferredChannelMap(wsCtx);
      break;
    case 6:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_74 = "";
      }
      else {
        local_74 = peVar5->txt;
      }
      iVar4 = atoi(local_74);
      wsCtx->enumeratedTransmitPower = iVar4;
      break;
    case 7:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      local_78 = wsCtx->meldStreamingIpAddress;
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_7c = "";
      }
      else {
        local_7c = peVar5->txt;
      }
      strcpy(local_78,local_7c);
      break;
    case 8:
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_80 = "";
      }
      else {
        local_80 = peVar5->txt;
      }
      MVar6 = atoi(local_80);
      wsCtx->meldModulationMode = MVar6;
    }
  }
  ezxml_free(xml);
  return 0;
}


// ==== open_port @ 0040c8d0

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int open_port(void)

{
  int __fd;
  undefined1 auStack_40 [56];
  
                    /* Unresolved local var: int fd@[???]
                       Unresolved local var: speed_t baud@[???] */
  __fd = open("/dev/ttyS1",2);
  if (__fd == -1) {
    perror("open_port: Unable to open /dev/ttyS0 - ");
  }
  else {
                    /* Unresolved local var: termios settings@[???] */
    tcgetattr(__fd,(termios *)auStack_40);
    cfsetospeed((termios *)auStack_40,0x1002);
    tcsetattr(__fd,0x540e,(termios *)auStack_40);
    tcflush(__fd,1);
    fcntl(__fd,4);
  }
  return __fd;
}


// ==== flush_port @ 0040c9e0

int flush_port(int fd)

{
  int in_v0;
  
  return in_v0;
}


// ==== packChannelMapIntoBufferAndGetLength @ 0040ca04

int packChannelMapIntoBufferAndGetLength(WhiteSpaceContext *context,char *buf)

{
  size_t sVar1;
  uint local_18;
  char acStack_14 [12];
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: char[10] chBuffer@[???] */
  if ((context->availableChannelMap).channelCount != 0) {
    memset(buf,0,4);
    for (local_18 = 0; local_18 < (context->availableChannelMap).channelCount;
        local_18 = local_18 + 1) {
      sprintf(acStack_14,"%d ",(context->availableChannelMap).channels[local_18]);
      strcat(buf,acStack_14);
    }
    memcpy(acStack_14,&DAT_00519f48,2);
    strcat(buf,acStack_14);
  }
  sVar1 = strlen(buf);
  return sVar1;
}


// ==== setOperatingChannelFile @ 0040cb40

void setOperatingChannelFile(int chNumber)

{
  FILE *__s;
  
                    /* Unresolved local var: FILE * fp@[???] */
  __s = fopen("/etc/config/operating","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'current\' \'operating_channel\'\n",1,0x26,__s);
    fprintf(__s,"\t option \'channel\' \'%d\'\n",chNumber);
  }
  putc(10,__s);
  fflush(__s);
  fclose(__s);
  return;
}


// ==== dumpAllSerialReceive @ 0040cc20

void dumpAllSerialReceive(int fd)

{
  int iVar1;
  uint local_10a4;
  int local_10a0;
  undefined1 auStack_1094 [4096];
  fd_set fStack_94;
  timeval local_14;
  
                    /* Unresolved local var: int rv@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int count@[???]
                       Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???] */
  local_10a0 = 0;
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_10a4 = 0; local_10a4 < 0x20; local_10a4 = local_10a4 + 1) {
    fStack_94.fds_bits[local_10a4] = 0;
  }
  fStack_94.fds_bits[(uint)fd >> 5] = fStack_94.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_14.tv_sec = 0;
  local_14.tv_usec = 100000;
  memset(auStack_1094,0,0x1000);
  while( true ) {
    iVar1 = select(fd + 1,&fStack_94,(fd_set *)0x0,(fd_set *)0x0,&local_14);
    if (iVar1 < 1) break;
    read(fd,auStack_1094,0x1000);
    if (3 < local_10a0) {
      local_10a0 = 0;
    }
    local_10a0 = local_10a0 + 1;
    sleep(1);
  }
  return;
}


// ==== getSerialReceive @ 0040cda8

char * getSerialReceive(int fd,char *buf)

{
  int iVar1;
  uint local_a4;
  fd_set fStack_94;
  timeval local_14;
  
                    /* Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int count@[???] */
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_a4 = 0; local_a4 < 0x20; local_a4 = local_a4 + 1) {
    fStack_94.fds_bits[local_a4] = 0;
  }
  fStack_94.fds_bits[(uint)fd >> 5] = fStack_94.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_14.tv_sec = 0;
  local_14.tv_usec = 100000;
  memset(buf,0,4);
  while( true ) {
    iVar1 = select(fd + 1,&fStack_94,(fd_set *)0x0,(fd_set *)0x0,&local_14);
    if (iVar1 < 1) break;
    read(fd,buf,0x1000);
  }
  return buf;
}


// ==== isChannelInAvailableChannelMap @ 0040cefc

int isChannelInAvailableChannelMap(WhiteSpaceContext *context,int channelNumber)

{
  uint local_18;
  
                    /* Unresolved local var: int index@[???] */
  if ((context->availableChannelMap).channelCount != 0) {
    for (local_18 = 0; local_18 < (context->availableChannelMap).channelCount;
        local_18 = local_18 + 1) {
      if (channelNumber == (context->availableChannelMap).channels[local_18]) {
        if ((((channelNumber == 0x23) || (channelNumber == 0x24)) || (channelNumber == 0x25)) ||
           (channelNumber == 0x26)) {
          return 0;
        }
        return 1;
      }
    }
  }
  return 0;
}


// ==== getChannelToOperateOnFrom @ 0040cff0

int getChannelToOperateOnFrom(WhiteSpaceContext *context)

{
  int iVar1;
  uint local_28;
  uint local_24;
  uint local_20;
  uint local_10;
  
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: int index1@[???]
                       Unresolved local var: char[10] chBuffer@[???]
                       Unresolved local var: int chToOperate@[???] */
  local_28 = 0;
  if (context->channelRequestStatus == WS_SUCCESS) {
    if ((context->preferredChannelMap).channelCount != 0) {
      for (local_24 = 0; local_24 < (context->preferredChannelMap).channelCount;
          local_24 = local_24 + 1) {
        if (((0x14 < (context->preferredChannelMap).channels[local_24]) &&
            ((context->preferredChannelMap).channels[local_24] < 0x34)) &&
           (iVar1 = isChannelInAvailableChannelMap
                              (context,(context->preferredChannelMap).channels[local_24]),
           iVar1 != 0)) {
          return (context->preferredChannelMap).channels[local_24];
        }
      }
    }
    if ((context->availableChannelMap).channelCount != 0) {
      for (local_20 = 0; local_20 < (context->availableChannelMap).channelCount;
          local_20 = local_20 + 1) {
        if ((0x14 < (context->availableChannelMap).channels[local_20]) &&
           ((context->availableChannelMap).channels[local_20] < 0x34)) {
          local_28 = (context->availableChannelMap).channels[local_20];
          break;
        }
      }
    }
    local_10 = local_28;
  }
  else {
    local_10 = 0;
  }
  return local_10;
}


// ==== setTransmitterFlagFile @ 0040d1e0

void setTransmitterFlagFile(int txFlag)

{
  FILE *__s;
  
                    /* Unresolved local var: FILE * fp@[???] */
  __s = fopen("/etc/config/transmitter","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'current\' \'radio_status\'\n",1,0x21,__s);
    fprintf(__s,"\t option \'status\' \'%d\'\n",txFlag);
  }
  putc(10,__s);
  fflush(__s);
  fclose(__s);
  return;
}


// ==== setPPowerValue @ 0040d2c0

int setPPowerValue(int fd,int val)

{
  size_t sVar1;
  ssize_t sVar2;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: int ret@[???]
                       Unresolved local var: char[4096] buf@[???] */
  switch(val) {
  case 0:
    memcpy(acStack_1014,&DAT_0051a000,5);
    break;
  case 1:
    memcpy(acStack_1014,&DAT_0051a008,5);
    break;
  case 2:
    memcpy(acStack_1014,&DAT_0051a010,5);
    break;
  case 3:
    memcpy(acStack_1014,&DAT_0051a018,5);
    break;
  case 4:
    memcpy(acStack_1014,&DAT_0051a020,5);
    break;
  case 5:
    memcpy(acStack_1014,&DAT_0051a028,5);
    break;
  case 6:
    memcpy(acStack_1014,&DAT_0051a030,5);
    break;
  case 7:
    memcpy(acStack_1014,&DAT_0051a038,5);
    break;
  case 8:
    memcpy(acStack_1014,&DAT_0051a040,5);
    break;
  case 9:
    memcpy(acStack_1014,&DAT_0051a048,5);
    break;
  case 10:
    memcpy(acStack_1014,&DAT_0051a050,5);
    break;
  default:
    memcpy(acStack_1014,&DAT_0051a058,5);
  }
  sVar1 = strlen(acStack_1014);
  sVar2 = write(fd,acStack_1014,sVar1 + 1);
  if (-1 >= sVar2) {
    fwrite("write() powerLevel failed!\n",1,0x1b,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar2);
  return local_10;
}


// ==== enableRadio @ 0040d5b8

int enableRadio(int fd)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: int ret@[???]
                       Unresolved local var: char[4096] buf@[???] */
  memcpy(acStack_1014,&DAT_0051a0a8,4);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 < sVar1) {
    setTransmitterFlagFile(1);
  }
  else {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== disableRadio @ 0040d6a4

int disableRadio(int fd)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: int ret@[???]
                       Unresolved local var: char[4096] buf@[???] */
  memcpy(acStack_1014,&DAT_0051a0cc,4);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 < sVar1) {
    setTransmitterFlagFile(0);
  }
  else {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== stripOutExcessBlanks @ 0040d790

int stripOutExcessBlanks(char *inBuffer,int len,char *outBuffer)

{
  int local_10;
  int local_c;
  
  local_10 = 0;
                    /* Unresolved local var: int index@[???]
                       Unresolved local var: int oindex@[???] */
  for (local_c = 0; local_c < len; local_c = local_c + 1) {
    if ((inBuffer[local_c] != ' ') && (inBuffer[local_c] != '\n')) {
      outBuffer[local_10] = inBuffer[local_c];
      local_10 = local_10 + 1;
    }
  }
  return local_10;
}


// ==== recreateFccSerialMeldFile @ 0040d854

void recreateFccSerialMeldFile
               (FILE *fp,char *serialNumberString,char *fccIdString,char *addressString)

{
  int param2;
  
  param2 = atoi(serialNumberString);
  fprintf((FILE *)fp,"Serial Number:%d\n",param2);
  fprintf((FILE *)fp,"FCCID:%s\n",fccIdString);
  fprintf((FILE *)fp,"StreamingIpAddress:%s\n",addressString);
  fflush((FILE *)fp);
  return;
}


// ==== validateAndLoadFieldType @ 0040d918

int validateAndLoadFieldType
              (WhiteSpaceContext *wsCtx,FILE *fp,char *buffer,int length,FieldType fType,
              char *retString)

{
  size_t len;
  int iVar1;
  int local_70;
  char *local_6c;
  char acStack_68 [60];
  char local_2c;
  char local_2b;
  uint local_28 [4];
  FieldType local_18;
  
                    /* Unresolved local var: char * tempBuff@[???]
                       Unresolved local var: char[60] oBuff@[???]
                       Unresolved local var: char[3] tBuff@[???]
                       Unresolved local var: int[4] ipBytes@[???]
                       Unresolved local var: int count@[???] */
  local_70 = 0;
  local_18 = fType;
  if (fType == SERIAL) {
    local_6c = strstr(buffer,"S/N = ");
    if (local_6c != (char *)0x0) {
      local_6c = local_6c + 6;
    }
    if (local_6c != (char *)0x0) {
      iVar1 = atoi(local_6c);
      sprintf(wsCtx->serialNumber,"%d",iVar1);
      iVar1 = atoi(local_6c);
      fprintf((FILE *)fp,"Serial Number:%d\n",iVar1);
      strcpy(retString,local_6c);
    }
  }
  else if (fType == FCCID) {
    local_6c = strstr(buffer,"FCC ID = ");
    if (local_6c != (char *)0x0) {
      local_6c = local_6c + 9;
    }
    if (local_6c != (char *)0x0) {
      memset(acStack_68,0,0x3c);
      len = strlen(local_6c);
      stripOutExcessBlanks(local_6c,len,acStack_68);
      strcpy(wsCtx->fccID,acStack_68);
      fprintf((FILE *)fp,"FCCID:%s\n",acStack_68);
      strcpy(retString,acStack_68);
    }
  }
  else if (fType == STREAMING_IP) {
    local_6c = buffer;
    if (buffer != (char *)0x0) {
      local_6c = buffer + 3;
    }
    for (; (local_6c != (char *)0x0 && (local_70 < 4)); local_70 = local_70 + 1) {
      local_2c = *local_6c;
      local_2b = local_6c[1];
      local_6c = local_6c + 2;
      sscanf(&local_2c,"%x",local_28 + local_70);
    }
    sprintf(retString,"%d.%d.%d.%d",local_28[0],local_28[1],local_28[2],local_28[3]);
    fprintf((FILE *)fp,"StreamingIpAddress:%s\n",retString);
  }
  return 1;
}


// ==== changeChannelOnRadio @ 0040dc80

int changeChannelOnRadio(int fd,int channel)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???] */
  sprintf(acStack_1014,"c%d\r",channel);
  setOperatingChannelFile(channel);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== changeATSCChannelNumber @ 0040dd70

int changeATSCChannelNumber(int fd,int channel)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???] */
  sprintf(acStack_1014,"m#[01%08d]",channel);
  setOperatingChannelFile(channel);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== changeNewPowerLevel @ 0040de60

int changeNewPowerLevel(int fd,int power)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???] */
  sprintf(acStack_1014,"m#[02%08d]",power);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== changeNewModulation @ 0040df44

int changeNewModulation(int fd,int modulation)

{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???] */
  sprintf(acStack_1014,"m#[03%08d]",modulation);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}


// ==== queryInformationFromRadio @ 0040e028

int queryInformationFromRadio(int fd)

{
  FILE *__stream;
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_10ac;
  fd_set fStack_109c;
  timeval local_101c;
  char acStack_1014 [4100];
  int local_10;
  
                    /* Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: FILE * fp@[???] */
  memcpy(acStack_1014,&DAT_0051a15c,2);
  __stream = fopen("/etc/config/queryInfo","w+");
  dumpAllSerialReceive(fd);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (sVar1 < 0) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
    local_10 = 0;
  }
  else {
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
    for (local_10ac = 0; local_10ac < 0x20; local_10ac = local_10ac + 1) {
      fStack_109c.fds_bits[local_10ac] = 0;
    }
    fStack_109c.fds_bits[(uint)fd >> 5] = fStack_109c.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
    local_101c.tv_sec = 0;
    local_101c.tv_usec = 100000;
    memset(acStack_1014,0,0x1000);
    while( true ) {
      iVar2 = select(fd + 1,&fStack_109c,(fd_set *)0x0,(fd_set *)0x0,&local_101c);
      if (iVar2 < 1) break;
      read(fd,acStack_1014,0x1000);
      iVar2 = strncmp(acStack_1014,"EOF",3);
      if (iVar2 == 0) break;
      fputs(acStack_1014,__stream);
      fflush(__stream);
      memset(acStack_1014,0,0x1000);
    }
    fclose(__stream);
    local_10 = 1;
  }
  return local_10;
}


// ==== convertChannelNumberToFrequency @ 0040e2e4

int convertChannelNumberToFrequency(int channelNumber)

{
  undefined **local_18;
  
                    /* Unresolved local var: int frequency@[???] */
  switch(channelNumber) {
  case 0x19:
    local_18 = (undefined **)0x4d35a0;
    break;
  case 0x1a:
    local_18 = (undefined **)0x4e6e20;
    break;
  case 0x1b:
    local_18 = (undefined **)0x4fa6a0;
    break;
  case 0x1c:
    local_18 = (undefined **)0x50df20;
    break;
  case 0x1d:
    local_18 = (undefined **)0x5217a0;
    break;
  case 0x1e:
    local_18 = (undefined **)(tag2bit + 0x44);
    break;
  case 0x1f:
    local_18 = (undefined **)0x5488a0;
    break;
  case 0x20:
    local_18 = (undefined **)&DAT_0055c120;
    break;
  case 0x21:
    local_18 = (undefined **)&DAT_0056f9a0;
    break;
  case 0x22:
    local_18 = (undefined **)&DAT_00583220;
    break;
  case 0x23:
    local_18 = (undefined **)&DAT_0055c120;
    break;
  case 0x24:
    local_18 = (undefined **)&DAT_0056f9a0;
    break;
  case 0x25:
    local_18 = (undefined **)&DAT_00583220;
    break;
  case 0x26:
    local_18 = (undefined **)0x596aa0;
    break;
  case 0x27:
    local_18 = &PTR_gethostbyname_r_005aa320;
    break;
  case 0x28:
    local_18 = (undefined **)0x5f8520;
    break;
  case 0x29:
    local_18 = (undefined **)0x60bda0;
    break;
  case 0x2a:
    local_18 = (undefined **)0x61f620;
    break;
  case 0x2b:
    local_18 = (undefined **)0x632ea0;
    break;
  case 0x2c:
    local_18 = (undefined **)0x646720;
    break;
  case 0x2d:
    local_18 = (undefined **)0x659fa0;
    break;
  case 0x2e:
    local_18 = (undefined **)0x66d820;
    break;
  case 0x2f:
    local_18 = (undefined **)0x6810a0;
    break;
  case 0x30:
    local_18 = (undefined **)0x694920;
    break;
  case 0x31:
    local_18 = (undefined **)0x6a81a0;
    break;
  case 0x32:
    local_18 = (undefined **)0x6bba20;
    break;
  default:
    local_18 = (undefined **)0xffffffff;
  }
  return (int)local_18;
}


// ==== changeNewFrequency @ 0040e558

int changeNewFrequency(int fd,int channelNumber)

{
  int param2;
  size_t __n;
  ssize_t sVar1;
  char acStack_1010 [4096];
  int local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int frequency@[???] */
  setOperatingChannelFile(channelNumber);
  param2 = convertChannelNumberToFrequency(channelNumber);
  if (param2 < 0) {
    fwrite("Invalid channel Number for this regulatory Domain!\n",1,0x33,(FILE *)stderr);
    local_10 = 0;
  }
  else {
    sprintf(acStack_1010,"m#[04%08d]",param2);
    __n = strlen(acStack_1010);
    sVar1 = write(fd,acStack_1010,__n);
    if (sVar1 < 0) {
      fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
      local_10 = 0;
    }
    else {
      local_10 = 1;
    }
  }
  return local_10;
}


// ==== processMeldVersionNumber @ 0040e69c

void processMeldVersionNumber(FILE *fp,char *buf)

{
  char *local_10;
  
                    /* Unresolved local var: char * tempBuff@[???] */
  local_10 = strstr(buf,"Firmware version = ");
  if (local_10 != (char *)0x0) {
    local_10 = local_10 + 0x13;
  }
  fprintf((FILE *)fp,"NXP Version:%s\n",local_10);
  fflush((FILE *)fp);
  return;
}


// ==== getMeldVersionAndOtherInfo @ 0040e73c

int getMeldVersionAndOtherInfo(int fd,FILE *ifp)

{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_10a4;
  char acStack_1098 [4096];
  fd_set fStack_98;
  timeval local_18;
  int local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???] */
  dumpAllSerialReceive(fd);
  memcpy(acStack_1098,&DAT_0051a248,3);
  __n = strlen(acStack_1098);
  sVar1 = write(fd,acStack_1098,__n);
  if (sVar1 < 0) {
    fwrite("write() of 11 bytes for MeldVersionAndOtherInfo !\n",1,0x32,(FILE *)stderr);
    local_10 = 0;
  }
  else {
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
    for (local_10a4 = 0; local_10a4 < 0x20; local_10a4 = local_10a4 + 1) {
      fStack_98.fds_bits[local_10a4] = 0;
    }
    fStack_98.fds_bits[(uint)fd >> 5] = fStack_98.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
    local_18.tv_sec = 0;
    local_18.tv_usec = 100000;
    memset(acStack_1098,0,0x1000);
    while( true ) {
      iVar2 = select(fd + 1,&fStack_98,(fd_set *)0x0,(fd_set *)0x0,&local_18);
      if (iVar2 < 1) break;
      sVar1 = read(fd,acStack_1098,0x1000);
      if (sVar1 < 0) {
        fwrite("Read Meldversion query failed!\n",1,0x1f,(FILE *)stderr);
      }
      if (1 < sVar1) {
        processMeldVersionNumber(ifp,acStack_1098);
      }
    }
    local_10 = 1;
  }
  return local_10;
}


// ==== getMeldSerialNumber @ 0040e9a8

int getMeldSerialNumber(int fd,FILE *ifp,WhiteSpaceContext *wsCtx)

{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_10b4;
  char acStack_10a8 [4096];
  char acStack_a8 [20];
  fd_set fStack_94;
  timeval local_14;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: char[20] serialNumberString@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???] */
  dumpAllSerialReceive(fd);
  memcpy(acStack_10a8,&DAT_0051a2a0,2);
  __n = strlen(acStack_10a8);
  sVar1 = write(fd,acStack_10a8,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_10b4 = 0; local_10b4 < 0x20; local_10b4 = local_10b4 + 1) {
    fStack_94.fds_bits[local_10b4] = 0;
  }
  fStack_94.fds_bits[(uint)fd >> 5] = fStack_94.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_14.tv_sec = 0;
  local_14.tv_usec = 100000;
  memset(acStack_10a8,0,0x1000);
  while( true ) {
    iVar2 = select(fd + 1,&fStack_94,(fd_set *)0x0,(fd_set *)0x0,&local_14);
    if (iVar2 < 1) break;
    sVar1 = read(fd,acStack_10a8,0x1000);
    if (-1 < sVar1) break;
    fwrite("Read Serial Number failed!\n",1,0x1b,(FILE *)stderr);
  }
  if (ifp != (FILE *)0x0) {
    validateAndLoadFieldType(wsCtx,ifp,acStack_10a8,0x1000,SERIAL,acStack_a8);
    fflush((FILE *)ifp);
  }
  return 1;
}


// ==== getMeldFccIdInfo @ 0040ec2c

int getMeldFccIdInfo(int fd,FILE *ifp,WhiteSpaceContext *wsCtx)

{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_10b4;
  char acStack_10a8 [4096];
  char acStack_a8 [20];
  fd_set fStack_94;
  timeval local_14;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: char[20] fccIDString@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???] */
  dumpAllSerialReceive(fd);
  memcpy(acStack_10a8,&DAT_0051a2d4,2);
  __n = strlen(acStack_10a8);
  sVar1 = write(fd,acStack_10a8,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_10b4 = 0; local_10b4 < 0x20; local_10b4 = local_10b4 + 1) {
    fStack_94.fds_bits[local_10b4] = 0;
  }
  fStack_94.fds_bits[(uint)fd >> 5] = fStack_94.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_14.tv_sec = 0;
  local_14.tv_usec = 100000;
  memset(acStack_10a8,0,0x1000);
  while( true ) {
    iVar2 = select(fd + 1,&fStack_94,(fd_set *)0x0,(fd_set *)0x0,&local_14);
    if (iVar2 < 1) break;
    sVar1 = read(fd,acStack_10a8,0x1000);
    if (-1 < sVar1) break;
    fwrite("Read FCCID failed!\n",1,0x13,(FILE *)stderr);
  }
  if (ifp != (FILE *)0x0) {
    validateAndLoadFieldType(wsCtx,ifp,acStack_10a8,0x1000,FCCID,acStack_a8);
    fflush((FILE *)ifp);
  }
  return 1;
}


// ==== getStreamingIpAddress @ 0040eeac

int getStreamingIpAddress(int fd,FILE *ifp,WhiteSpaceContext *wsCtx,char *buffer)

{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_109c;
  char acStack_1090 [4096];
  fd_set fStack_90;
  timeval local_10;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int rv@[???]
                       Unresolved local var: fd_set set@[???]
                       Unresolved local var: timeval timeout@[???] */
  dumpAllSerialReceive(fd);
  memcpy(acStack_1090,&DAT_0051a2ec,3);
  __n = strlen(acStack_1090);
  sVar1 = write(fd,acStack_1090,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
                    /* Unresolved local var: uint __i@[???]
                       Unresolved local var: fd_set * __arr@[???] */
  for (local_109c = 0; local_109c < 0x20; local_109c = local_109c + 1) {
    fStack_90.fds_bits[local_109c] = 0;
  }
  fStack_90.fds_bits[(uint)fd >> 5] = fStack_90.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_10.tv_sec = 0;
  local_10.tv_usec = 100000;
  memset(acStack_1090,0,0x1000);
  while( true ) {
    iVar2 = select(fd + 1,&fStack_90,(fd_set *)0x0,(fd_set *)0x0,&local_10);
    if (iVar2 < 1) break;
    sVar1 = read(fd,acStack_1090,0x1000);
    if (-1 < sVar1) break;
    fwrite("Read IP Address failed!\n",1,0x18,(FILE *)stderr);
  }
  validateAndLoadFieldType(wsCtx,ifp,acStack_1090,0x1000,STREAMING_IP,buffer);
  fflush((FILE *)ifp);
  return 1;
}


// ==== convertIpOctetsToMeldHexFormatIpStringAndSize @ 0040f128

int convertIpOctetsToMeldHexFormatIpStringAndSize(char *ipAddress,char *returnString)

{
  size_t sVar1;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  
                    /* Unresolved local var: int[4] octets@[???] */
  if (*ipAddress != '\0') {
    sscanf(ipAddress,"%d.%d.%d.%d",(int *)&local_18,(int *)&local_14,(int *)&local_10,
           (int *)&local_c);
    sprintf(returnString,"%02x%02x%02x%02x",local_18,local_14,local_10,local_c);
  }
  sVar1 = strlen(returnString);
  return sVar1;
}


// ==== setStreamingIpAddress @ 0040f1ec

int setStreamingIpAddress(int fd,WhiteSpaceContext *wsCtx,char *ipAddressString)

{
  size_t sVar1;
  int iVar2;
  ssize_t sVar3;
  char acStack_102c [4096];
  char acStack_2c [20];
  int local_18;
  
                    /* Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: char[20] convIp@[???] */
  memset(acStack_2c,0,0x14);
  if (wsCtx->meldStreamingIpAddress[0] != '\0') {
    sVar1 = strlen(ipAddressString);
    iVar2 = strncmp(wsCtx->meldStreamingIpAddress,ipAddressString,sVar1);
    if (iVar2 == 0) {
      return 0;
    }
  }
  iVar2 = convertIpOctetsToMeldHexFormatIpStringAndSize(wsCtx->meldStreamingIpAddress,acStack_2c);
  if (iVar2 == 0) {
    local_18 = 0;
  }
  else {
    sprintf(acStack_102c,"b#[01%s]\r",acStack_2c);
    sVar1 = strlen(acStack_102c);
    sVar3 = write(fd,acStack_102c,sVar1);
    if (sVar3 < 0) {
      fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
      local_18 = 0;
    }
    else {
      local_18 = 1;
    }
  }
  return local_18;
}


// ==== serialProcessorThread @ 0040f390

void * serialProcessorThread(void *arg)

{
  bool bVar1;
  bool bVar2;
  WhiteSpaceContext *wsCtx;
  size_t __n;
  ssize_t sVar3;
  int iVar4;
  int iVar5;
  FILE *local_1088;
  int local_1084;
  int local_1080;
  int local_1048;
  char acStack_1044 [4096];
  char acStack_44 [60];
  
                    /* Unresolved local var: int fd@[???]
                       Unresolved local var: int ret@[???]
                       Unresolved local var: int cmd@[???]
                       Unresolved local var: int chan@[???]
                       Unresolved local var: char[4096] buf@[???]
                       Unresolved local var: InterThreadData * data@[???]
                       Unresolved local var: int msgSize@[???]
                       Unresolved local var: int lastIndex@[???]
                       Unresolved local var: int chToWrite@[???]
                       Unresolved local var: int index@[???]
                       Unresolved local var: int sleepSeconds@[???]
                       Unresolved local var: WhiteSpaceContext * ctx@[???]
                       Unresolved local var: int initCondition@[???]
                       Unresolved local var: int txEnable@[???]
                       Unresolved local var: int tempIndex@[???]
                       Unresolved local var: char[20] ipAddressString@[???]
                       Unresolved local var: char[20] fccIDString@[???]
                       Unresolved local var: char[20] serialNumberString@[???]
                       Unresolved local var: int lastChannel@[???]
                       Unresolved local var: int timeCounter@[???]
                       Unresolved local var: FILE * fp@[???] */
  wsCtx = *(WhiteSpaceContext **)((int)arg + 8);
  bVar1 = false;
  bVar2 = false;
  local_1080 = 0;
  local_1084 = 0;
  local_1088 = fopen("/etc/config/fccidSerialInfo","w+");
  local_1048 = open_port();
  do {
    if (local_1048 == -1) {
      local_1048 = open_port();
    }
    else {
      if (!bVar1) {
        memcpy(acStack_1044,"MELD_TECH_WS_ENABLE\r",0x15);
        __n = strlen(acStack_1044);
        sVar3 = write(local_1048,acStack_1044,__n);
        if (sVar3 < 0) {
          fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
        }
        usleep(500000);
        queryInformationFromRadio(local_1048);
        usleep(500000);
        disableRadio(local_1048);
        usleep(500000);
        bVar2 = false;
        getMeldVersionAndOtherInfo(local_1048,(FILE *)local_1088);
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getMeldSerialNumber(local_1048,(FILE *)local_1088,wsCtx);
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getMeldFccIdInfo(local_1048,(FILE *)local_1088,wsCtx);
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getStreamingIpAddress(local_1048,(FILE *)local_1088,wsCtx,acStack_44);
        usleep(500000);
        iVar4 = setStreamingIpAddress(local_1048,wsCtx,acStack_44);
        if (iVar4 != 0) {
          fclose(local_1088);
          local_1088 = fopen("/etc/config/fccidSerialInfo","w+");
          recreateFccSerialMeldFile
                    ((FILE *)local_1088,wsCtx->serialNumber,wsCtx->fccID,
                     wsCtx->meldStreamingIpAddress);
        }
        usleep(500000);
        if (wsCtx != (WhiteSpaceContext *)0x0) {
          iVar4 = strcmp(wsCtx->regulatoryDomain,"US");
          if (iVar4 == 0) {
            setPPowerValue(local_1048,wsCtx->enumeratedTransmitPower);
            usleep(500000);
            changeNewModulation(local_1048,wsCtx->meldModulationMode);
            usleep(500000);
          }
          else {
            changeNewPowerLevel(local_1048,wsCtx->enumeratedTransmitPower);
            usleep(500000);
            changeNewModulation(local_1048,wsCtx->meldModulationMode);
            usleep(500000);
          }
        }
        bVar1 = true;
        if ((wsCtx->fccID[0] != '\0') && (wsCtx->serialNumber[0] != '\0')) {
          signalAgentToProcess();
        }
      }
      if (wsCtx != (WhiteSpaceContext *)0x0) {
        iVar4 = getChannelToOperateOnFrom(wsCtx);
        usleep(500000);
        if ((iVar4 == 0) && (local_1084 < 0x14371)) {
          if ((bVar2) || (0x14370 < local_1084)) {
            disableRadio(local_1048);
            usleep(500000);
            bVar2 = false;
            queryInformationFromRadio(local_1048);
            usleep(500000);
          }
        }
        else if ((local_1080 != iVar4) || (0x14370 < local_1084)) {
          iVar5 = strcmp(wsCtx->regulatoryDomain,"US");
          if (iVar5 == 0) {
            changeChannelOnRadio(local_1048,iVar4);
            usleep(500000);
          }
          else {
            changeNewFrequency(local_1048,iVar4);
            usleep(500000);
          }
          if ((!bVar2) || (0x14370 < local_1084)) {
            enableRadio(local_1048);
            usleep(500000);
            bVar2 = true;
          }
          queryInformationFromRadio(local_1048);
          usleep(500000);
          local_1084 = 0;
          setPPowerValue(local_1048,wsCtx->enumeratedTransmitPower);
          usleep(500000);
          local_1080 = iVar4;
        }
      }
    }
    fflush((FILE *)stdout);
    tcflush(local_1048,1);
    sleep(0xf);
    local_1084 = local_1084 + 0xf;
  } while( true );
}


