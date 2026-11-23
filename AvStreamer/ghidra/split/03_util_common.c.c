// ==== util_init_tx_udp_socket @ 00410500 ====

return_code_t
util_init_tx_udp_socket
          (int remoteAddr,int port,int ttl,int *pSocket,sockaddr_in_conflict *pRemoteSockAddr)

{
  uint16_t uVar1;
  uint32_t uVar2;
  int iVar3;
  return_code_t rVar4;
  int *piVar5;
  char *param3;
  int local_res8;
  int *local_resc;
  char acStack_110 [260];
  
                    /* Unresolved local var: char[256] log_buf@[???] */
  local_res8 = ttl;
  local_resc = pSocket;
  sprintf(acStack_110,"util_common.c : TTL is = %d\n",ttl);
  syslog(5,acStack_110);
  memset(pRemoteSockAddr,0,0x10);
  pRemoteSockAddr->sin_family = 2;
  uVar2 = htonl(remoteAddr);
  (pRemoteSockAddr->sin_addr).s_addr = uVar2;
  uVar1 = htons((uint16_t)port);
  pRemoteSockAddr->sin_port = uVar1;
  iVar3 = socket(2,1,0x11);
  *local_resc = iVar3;
  if (*local_resc < 0) {
    printf("%s:Create Socket failed.\n","util_init_tx_udp_socket");
    rVar4 = RET_ERR;
  }
  else {
    setsockopt(*local_resc,0,0x21,&local_res8,4);
    iVar3 = connect(*local_resc,(sockaddr *)pRemoteSockAddr,0x10);
    if (iVar3 < 0) {
      piVar5 = __errno_location();
      param3 = strerror(*piVar5);
      printf("%s:Couldn\'t connect socket to target %x (%s)","util_init_tx_udp_socket",remoteAddr,
             param3);
      close(*local_resc);
      rVar4 = RET_ERR;
    }
    else {
      rVar4 = RET_OK;
    }
  }
  return rVar4;
}



// ==== util_sendto @ 00410784 ====

return_code_t
util_sendto(int socket,char *message,size_t length,int flags,sockaddr *dest_addr,socklen_t dest_len)

{
  uint uVar1;
  ssize_t sVar2;
  int *piVar3;
  char *param3;
  uint local_60;
  char *local_5c;
  int local_58;
  int local_54;
  iovec local_48;
  undefined4 local_40 [13];
  
                    /* Unresolved local var: int rc@[???]
                       Unresolved local var: size_t remainLen@[???]
                       Unresolved local var: size_t toSend@[???]
                       Unresolved local var: char * curPtr@[???]
                       Unresolved local var: iovec[7] iov@[???]
                       Unresolved local var: int iovNum@[???]
                       Unresolved local var: int i@[???] */
  local_60 = length;
  local_5c = message;
  while( true ) {
    uVar1 = 0x524;
    if (local_60 < 0x525) {
      uVar1 = local_60;
    }
    if (uVar1 == 0x524) {
      local_58 = 1;
    }
    else {
      local_58 = (0x524 - uVar1) / 0xbc + 1;
      for (local_54 = 1; local_54 < local_58; local_54 = local_54 + 1) {
        local_40[local_54 * 2 + -2] = DummyTsPkt;
        local_40[local_54 * 2 + -1] = 0xbc;
      }
    }
    local_48.iov_base = local_5c;
    local_48.iov_len = uVar1;
    sVar2 = writev(socket,&local_48,local_58);
    if (sVar2 < 0) break;
    local_5c = local_5c + uVar1;
    local_60 = local_60 - uVar1;
    if (local_60 == 0) {
LAB_00410978:
      return (uint)(sVar2 < 0);
    }
  }
  piVar3 = __errno_location();
  param3 = strerror(*piVar3);
  printf("%s:writev failed size %d (%s)","util_sendto",length,param3);
  goto LAB_00410978;
}



// ==== util_create_thread @ 004109b0 ====

return_code_t
util_create_thread(util_thread_t *mThread,_func_void_ptr_void_ptr *start_routine,void *arg)

{
  int iVar1;
  return_code_t local_18;
  util_thread_t *local_14;
  void *local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: util_thread_arg_t threadArg@[???] */
  mThread->isReady = 0;
  mThread->toExit = 0;
  mThread->result = RET_ERR;
  local_14 = mThread;
  local_10 = arg;
  pthread_cond_init((pthread_cond_t *)&mThread->readyCond,(pthread_condattr_t *)0x0);
  pthread_mutex_init((pthread_mutex_t *)&mThread->readyMutex,(pthread_mutexattr_t *)0x0);
  iVar1 = pthread_create(&mThread->threadId,(pthread_attr_t *)0x0,(__start_routine *)start_routine,
                         &local_14);
  if (iVar1 == 0) {
    pthread_mutex_lock((pthread_mutex_t *)&mThread->readyMutex);
    if (mThread->isReady == 0) {
      pthread_cond_wait((pthread_cond_t *)&mThread->readyCond,
                        (pthread_mutex_t *)&mThread->readyMutex);
    }
    pthread_mutex_unlock((pthread_mutex_t *)&mThread->readyMutex);
    local_18 = mThread->result;
  }
  else {
    printf("%s:Fail to createthread\n","util_create_thread");
    local_18 = RET_ERR;
  }
  return local_18;
}



// ==== util_destroy_thread @ 00410b5c ====

return_code_t util_destroy_thread(util_thread_t *mThread)

{
  return_code_t rVar1;
  int iVar2;
  void *apvStack_10 [2];
  
                    /* Unresolved local var: void * res@[???] */
  if (mThread->isReady == 0) {
    printf("%s:terminate None-active thread %x\n","util_destroy_thread",mThread->threadId);
    rVar1 = RET_ERR;
  }
  else if (mThread->toExit == 0) {
    mThread->toExit = 1;
    iVar2 = pthread_join(mThread->threadId,apvStack_10);
    if (iVar2 != 0) {
      printf("%s:Error to terminate thread %x\n","util_destroy_thread",mThread->threadId);
    }
    pthread_cond_destroy((pthread_cond_t *)&mThread->readyCond);
    pthread_mutex_destroy((pthread_mutex_t *)&mThread->readyMutex);
    mThread->isReady = 0;
    rVar1 = RET_OK;
  }
  else {
    while (mThread->isReady != 0) {
      usleep(1000);
    }
    rVar1 = RET_OK;
  }
  return rVar1;
}



// ==== util_thread_notify_ready @ 00410cec ====

void util_thread_notify_ready(util_thread_t *mThread,return_code_t result)

{
  pthread_mutex_unlock((pthread_mutex_t *)&mThread->readyMutex);
  mThread->result = result;
  mThread->isReady = 1;
  pthread_cond_signal((pthread_cond_t *)&mThread->readyCond);
  pthread_mutex_unlock((pthread_mutex_t *)&mThread->readyMutex);
  return;
}



