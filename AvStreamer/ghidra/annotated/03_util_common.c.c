/*
 * 03_util_common.c.c  --  annotated Ghidra decompilation of util_common.c
 * =======================================================================
 *
 * Original source : /media/WD_passport/Meld_AV_Script/2p12/avstreamer/src/libxcode4encoder/util_common.c
 * Compiler        : GNU C 4.5.3 (MIPS32r2 little-endian, O0/O1 with DWARF)
 * Code range      : 0x00410500 - 0x00410da0
 *
 * Role in av_streamer:
 *   Provides low-level POSIX networking (connected UDP socket with multicast TTL
 *   and 7-TS-packet scatter/gather padding) and pthread lifecycle helpers
 *   (create with startup synchronization, destroy/join, and readiness notify)
 *   used by vixs_encoder.c and libxcode4encoder.c.
 *
 * Main data structures (from DWARF):
 *   typedef enum { RET_OK = 0, RET_ERR = 1 } return_code_t;
 *
 *   typedef struct util_thread {           // size = 96 (0x60) bytes
 *     pthread_t       threadId;            // +0x00
 *     pthread_mutex_t readyMutex;          // +0x04 (24 bytes)
 *     pthread_cond_t  readyCond;           // +0x20 (48 bytes)
 *     void           *private;             // +0x50
 *     int             isReady;             // +0x54
 *     int             toExit;              // +0x58
 *     return_code_t   result;              // +0x5c
 *   } util_thread_t;
 *
 *   typedef struct util_thread_arg {       // size = 8 bytes
 *     util_thread_t  *mThread;             // +0x00
 *     void           *arg;                 // +0x04
 *   } util_thread_arg_t;
 *
 * Globals owned:
 *   DummyTsPkt[188] @ 0x00436580 (.data)   -- 188-byte MPEG-2 TS null packet:
 *     0x47 0x1f 0xff 0x00 followed by 184 zero bytes (PID 0x1fff = null packet,
 *     TEI=0, PUSI=0, priority=0; note AFC=00 is technically reserved in ISO 13818-1).
 */

// ==== util_init_tx_udp_socket @ 00410500 ====
/*
 * util_init_tx_udp_socket():
 *   Creates, configures, and connects a UDP transmit socket to remoteAddr:port
 *   with IP_MULTICAST_TTL set to ttl.
 *
 * Parameters:
 *   remoteAddr      - Host-order 32-bit IPv4 destination address (passed to htonl).
 *   port            - Host-order UDP destination port (passed to htons).
 *   ttl             - IP multicast TTL value (written via setsockopt IP_MULTICAST_TTL).
 *   pSocket         - [out] Receives the created socket fd (or -1 on socket() failure).
 *   pRemoteSockAddr - [out] Caller-allocated sockaddr_in populated with AF_INET,
 *                     remoteAddr, and port.
 *
 * Returns:
 *   RET_OK (0) on success, RET_ERR (1) if socket() or connect() fails.
 *
 * Callers:
 *   vixs_encoder_init() in vixs_encoder.c (when outMode == UDP).
 *
 * Side effects / notes:
 *   - Logs TTL to syslog(LOG_NOTICE (5), ...).
 *   - On MIPS Linux, SOCK_DGRAM is 1 and SOCK_STREAM is 2 (reversed vs x86), so
 *     socket(2, 1, 0x11) is socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP).
 *   - setsockopt(fd, 0, 0x21, &ttl, 4) = SOL_IP (0), IP_MULTICAST_TTL (33 = 0x21).
 *   - Calls connect() on the UDP socket so util_sendto() can use writev()
 *     without passing a destination address.
 *
 * Locals:
 *   uVar1      - htons(port)
 *   uVar2      - htonl(remoteAddr)
 *   iVar3      - socket() / connect() return code
 *   rVar4      - return_code_t result
 *   piVar5     - __errno_location() pointer
 *   param3     - strerror(errno) string
 *   local_res8 - stack copy of ttl passed to setsockopt()
 *   local_resc - stack copy of pSocket
 *   acStack_110 - log_buf[256] (260 bytes allocated by Ghidra) for syslog()
 */
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
  syslog(5,acStack_110);                       // LOG_NOTICE (5)
  memset(pRemoteSockAddr,0,0x10);              // sizeof(struct sockaddr_in) == 16
  pRemoteSockAddr->sin_family = 2;             // AF_INET
  uVar2 = htonl(remoteAddr);
  (pRemoteSockAddr->sin_addr).s_addr = uVar2;
  uVar1 = htons((uint16_t)port);
  pRemoteSockAddr->sin_port = uVar1;
  iVar3 = socket(2,1,0x11);                    // AF_INET (2), SOCK_DGRAM on MIPS (1), IPPROTO_UDP (17)
  *local_resc = iVar3;
  if (*local_resc < 0) {
    printf("%s:Create Socket failed.\n","util_init_tx_udp_socket");
    rVar4 = RET_ERR;
  }
  else {
    setsockopt(*local_resc,0,0x21,&local_res8,4); // SOL_IP (0), IP_MULTICAST_TTL (0x21)
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
/*
 * util_sendto():
 *   Sends an MPEG-2 Transport Stream buffer over a connected UDP socket in
 *   fixed 1316-byte (0x524 = 7 * 188) datagrams. If the final chunk is shorter
 *   than 1316 bytes, pads the datagram to 7 TS packets using DummyTsPkt
 *   (PID 0x1fff null packets) via scatter-gather writev().
 *
 * Parameters:
 *   socket    - Connected UDP socket fd from util_init_tx_udp_socket().
 *   message   - Pointer to TS packet buffer (assumed multiple of 188 bytes).
 *   length    - Total byte length to send.
 *   flags     - Unused (socket is connected; writev() is used).
 *   dest_addr - Unused.
 *   dest_len  - Unused.
 *
 * Returns:
 *   RET_OK (0) if all chunks succeeded, RET_ERR (1) if writev() returned < 0.
 *
 * Callers:
 *   encoder_recv_video_thread_entry() in libxcode4encoder.c.
 *
 * Locals:
 *   uVar1    - toSend: min(remainLen, 0x524) (at most 7 TS packets = 1316 bytes)
 *   sVar2    - rc: writev() byte count or -1 on error
 *   piVar3   - __errno_location()
 *   param3   - strerror(errno)
 *   local_60 - remainLen: remaining bytes to send
 *   local_5c - curPtr: current read pointer into message
 *   local_58 - iovNum: number of iovec entries (1..7)
 *   local_54 - i: loop index for padding iov[1..iovNum-1]
 *   local_48 - iov[0]: { curPtr, toSend }
 *   local_40 - overlays iov[1..6] (6 * 2 words = 12 words, plus 1 word padding)
 */
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
    uVar1 = 0x524;                             // 1316 bytes = 7 * 188-byte TS packets
    if (local_60 < 0x525) {
      uVar1 = local_60;
    }
    if (uVar1 == 0x524) {
      local_58 = 1;
    }
    else {
      // Pad short final datagram up to 7 TS packets using DummyTsPkt (PID 0x1fff)
      local_58 = (0x524 - uVar1) / 0xbc + 1;   // 0xbc == 188 bytes/packet
      for (local_54 = 1; local_54 < local_58; local_54 = local_54 + 1) {
        local_40[local_54 * 2 + -2] = DummyTsPkt; // iov[i].iov_base
        local_40[local_54 * 2 + -1] = 0xbc;       // iov[i].iov_len = 188
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
      return (uint)(sVar2 < 0);                // RET_OK (0) or RET_ERR (1)
    }
  }
  piVar3 = __errno_location();
  param3 = strerror(*piVar3);
  printf("%s:writev failed size %d (%s)","util_sendto",length,param3);
  goto LAB_00410978;
}



// ==== util_create_thread @ 004109b0 ====
/*
 * util_create_thread():
 *   Initializes a util_thread_t synchronization block, spawns a pthread running
 *   start_routine(&threadArg), and blocks on readyCond until the child calls
 *   util_thread_notify_ready().
 *
 * Parameters:
 *   mThread       - [in/out] Caller-owned util_thread_t struct.
 *   start_routine - Thread entry point; receives util_thread_arg_t* (&local_14).
 *   arg           - Opaque user argument stored in threadArg.arg (local_10).
 *
 * Returns:
 *   mThread->result (set by child via util_thread_notify_ready) on success,
 *   or RET_ERR (1) if pthread_create() fails.
 *
 * Callers:
 *   encoder_recv_video_thread_create() and encoder_send_data_thread_create()
 *   in libxcode4encoder.c.
 *
 * Quirks / stack lifetime note:
 *   threadArg (local_14, local_10) lives on util_create_thread()'s stack, which
 *   remains valid only until util_thread_notify_ready() wakes readyCond; the
 *   child thread must copy mThread and arg before calling util_thread_notify_ready().
 *
 * Locals:
 *   iVar1    - pthread_create() return code
 *   local_18 - rc (return_code_t)
 *   local_14 - threadArg.mThread
 *   local_10 - threadArg.arg
 */
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
/*
 * util_destroy_thread():
 *   Signals a running util_thread_t to exit (mThread->toExit = 1), joins it,
 *   and destroys its condition variable and mutex. If another caller is already
 *   tearing down the thread (toExit != 0), polls until isReady becomes 0.
 *
 * Parameters:
 *   mThread - [in/out] Active util_thread_t struct.
 *
 * Returns:
 *   RET_OK (0) on success, RET_ERR (1) if mThread->isReady == 0 (not active).
 *
 * Callers:
 *   encoder_recv_video_thread_destroy() and encoder_send_data_thread_destroy()
 *   in libxcode4encoder.c.
 *
 * Locals:
 *   rVar1      - return_code_t result
 *   iVar2      - pthread_join() return code
 *   apvStack_10 - res (void * return value from pthread_join)
 */
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
    // Another thread is already destroying this thread; wait for completion
    while (mThread->isReady != 0) {
      usleep(1000);
    }
    rVar1 = RET_OK;
  }
  return rVar1;
}



// ==== util_thread_notify_ready @ 00410cec ====
/*
 * util_thread_notify_ready():
 *   Called by a newly spawned worker thread to record its startup status and
 *   wake util_create_thread() from pthread_cond_wait().
 *
 * Parameters:
 *   mThread - [in/out] util_thread_t passed via util_thread_arg_t.
 *   result  - Startup status (RET_OK / RET_ERR) returned by util_create_thread().
 *
 * Callers:
 *   encoder_recv_video_thread_entry() and encoder_send_data_thread_entry()
 *   in libxcode4encoder.c.
 *
 * BUG:
 *   Line 196 calls pthread_mutex_unlock() instead of pthread_mutex_lock()!
 *   Because readyMutex is not locked before setting result/isReady and calling
 *   pthread_cond_signal(), there is a lost-wakeup race if the child runs
 *   between util_create_thread()'s `if (mThread->isReady == 0)` check and its
 *   entry into pthread_cond_wait().
 */
void util_thread_notify_ready(util_thread_t *mThread,return_code_t result)

{
  pthread_mutex_unlock((pthread_mutex_t *)&mThread->readyMutex); // BUG: should be pthread_mutex_lock
  mThread->result = result;
  mThread->isReady = 1;
  pthread_cond_signal((pthread_cond_t *)&mThread->readyCond);
  pthread_mutex_unlock((pthread_mutex_t *)&mThread->readyMutex);
  return;
}


