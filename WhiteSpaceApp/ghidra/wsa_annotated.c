/*
 * wsa_annotated.c  --  annotated decompilation of WhiteSpaceApp/wsa
 * ================================================================
 *
 * This is the Ghidra decompiler output for the APPLICATION functions of wsa
 * (the ezxml library internals are left in wsa_decompiled.c only), with
 * comments added by analysis to explain:
 *   - what each Ghidra auto-named local variable actually holds, and
 *   - how the code uses the ezxml XML parser.
 *
 * The CODE is reproduced from Ghidra; only the line and block comments were
 * added. Function names, parameter names and the struct/enum types are the
 * binary's own DWARF symbols (originals, not guesses).
 *
 * Variable-name scheme (full legend in README.md):
 *   iVar/uVar = int/unsigned temp   sVar = size_t (strlen/write result)
 *   pcVar = char*    peVar = ezxml_t*    local_NN = stack var at -0xNN
 *   acStack_NN = char[] buffer on stack   fStack_NN = fd_set for select()
 *   local_10 is very often the function's return value.
 *   ezxml_t_conflict == ezxml_t (the "conflict" suffix is a Ghidra artifact).
 *
 * Grouped by original source file. Nothing here was produced by running
 * the binary.
 */

/* =====================================================================
 * whitespaceagent.c  --  entry point and the two worker threads
 * =====================================================================
 *
 * main() builds one WhiteSpaceContext on its own stack and hands a pointer
 * to it (inside whitespaceThreadData) to BOTH threads:
 *   - whiteSpaceStateMachine : the TLS database client (thread A)
 *   - serialProcessorThread  : the radio controller (thread B)
 * The two share that single context; whitespace_mutex guards some updates.
 */

// usage(): prints the argv help line to stderr. No locals.
void usage(void)
{
  fwrite("USAGE: whitespaceagent certname certpath xmldeviceinfofilename\n",1,0x3f,
         (FILE *)stderr);
  return;
}

/* signalAgentToProcess(): thread B calls this once the radio has given up a
 * usable FCC ID + serial number, to wake thread A out of its initial
 * pthread_cond_wait. The mutex/cond pair is agent_mutex/agent_cond. */
void signalAgentToProcess(void)
{
  pthread_mutex_lock((pthread_mutex_t *)&agent_mutex);
  pthread_cond_signal((pthread_cond_t *)&agent_cond);
  pthread_mutex_unlock((pthread_mutex_t *)&agent_mutex);
  return;
}

/* whiteSpaceStateMachine(): thread A's entry point.
 * threadData -> the shared InterThreadData (whitespace_date) struct:
 *     +0  sslCtx   (SSL_CTX*)        +4  ssl (SSL*)
 *     +8  wsCtx    (WhiteSpaceContext*)
 *     +0xc whiteSpaceMutex (pthread_mutex_t*)
 * It blocks until thread B signals agent_cond, then spins the state machine
 * forever (each call advances one state). */
void * whiteSpaceStateMachine(void *threadData)
{
  pthread_mutex_lock((pthread_mutex_t *)&agent_mutex);
  pthread_cond_wait((pthread_cond_t *)&agent_cond,(pthread_mutex_t *)&agent_mutex); // wait for radio init
  pthread_mutex_unlock((pthread_mutex_t *)&agent_mutex);
  initProcessStateFunctions();                 // populate the processFunctions[] dispatch table
  do {
    processWSStateMachine
              (*(WhiteSpaceContext **)((int)threadData + 8),  // wsCtx
               threadData,                                    // (unused extra arg)
               (SSL **)((int)threadData + 4),                 // &ssl
               *(pthread_mutex_t **)((int)threadData + 0xc)); // whiteSpaceMutex
  } while( true );                              // never returns
}

/* open_port_Read(): opens /dev/ttyS0 read-only at 115200 (cfsetospeed 0x1002).
 * __fd = the serial file descriptor. NOTE: this function is DEAD CODE - the
 * live serial path is open_port() on /dev/ttyS1. auStack_40 is the termios
 * struct (56 bytes). */
int open_port_Read(void)
{
  int __fd;
  undefined1 auStack_40 [56];        // struct termios

  __fd = open("/dev/ttyS0",0);       // O_RDONLY
  if (__fd == -1) {
    perror("open_port: Unable to open /dev/ttyS0 - ");
  }
  else {
    tcgetattr(__fd,(termios *)auStack_40);
    cfsetospeed((termios *)auStack_40,0x1002);   // B115200
    tcsetattr(__fd,0x540e,(termios *)auStack_40); // TCSETSW
    tcflush(__fd,1);                              // TCOFLUSH
    fcntl(__fd,4);                                // F_SETFL
  }
  return __fd;
}

/* main(argc, argv)
 *   argv[1] = TLS CA file     argv[2] = TLS CA path
 *   argv[3] = device-info XML file (optional)
 * Locals:
 *   WStack_1488 = the one and only WhiteSpaceContext (on main's stack)
 *   local_18    = thread A handle (whiteSpaceStateMachine)
 *   local_14    = thread B handle (serialProcessorThread)
 *   local_10    = process exit code
 *   iVar1       = scratch for strcmp / pthread_create results
 */
int main(int argc,char **argv)
{
  int iVar1;
  WhiteSpaceContext WStack_1488;     // the shared context
  pthread_t local_18;                // thread A id
  pthread_t local_14;                // thread B id
  int local_10;                      // return code

  initializeSecurityCtx();                       // OpenSSL init + RAND_seed
  initializeWhiteSpaceContext(&WStack_1488);     // zero context, power=10
  if ((argc == 2) && (iVar1 = strcmp(argv[1],"-v"), iVar1 == 0)) {
    printf("whitespaceagent Version:%s Build Date:%s\n","44","Fri Nov 22 16:35:57 2013");
    local_10 = 0;
  }
  else {
    if (argc < 3) {                              // need at least CAfile + CApath
      usage();
      exit(2);
    }
    if (1 < argc) {
      WStack_1488.securityCertFileName = argv[1]; // CA file  -> SSL_CTX_load_verify_locations
    }
    if (argc < 3) {
      WStack_1488.securityCertPath = (char *)0x0;
    }
    else {
      WStack_1488.securityCertPath = argv[2];     // CA path
    }
    if (3 < argc) {
      readXmlIntoContext(argv[3],&WStack_1488);   // load device-info XML (optional)
    }
    // Fill the inter-thread handoff struct, then start both threads.
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
        pthread_join(local_18,(void **)0x0);     // blocks forever; threads never exit
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

/* =====================================================================
 * registration_packunpack.c / channelrequest_packunpack.c /
 * http_process_header.c / whitespace_status.c
 *   --  build the outbound requests and pull apart the responses
 * =====================================================================
 */

/* buildRegistrationRequest(wsCtx): formats the registration PUT into
 * wsCtx->transmitBuffer and returns its length.
 *   sVar1 = length of the XML body, then (reused) length of the whole request.
 * The body is built in tempProcessingBuffer first so its length can go into
 * Content-Length; then the request line + headers + body go into transmitBuffer.
 * Note the header block ends with "\r\n\n" (one CR-LF then a bare LF). */
int buildRegistrationRequest(WhiteSpaceContext *wsCtx)
{
  size_t sVar1;                      // body length, then request length
  memset(wsCtx->tempProcessingBuffer,0,0x400);
  sprintf(wsCtx->tempProcessingBuffer,
          "<RegistrationRequest xmlns=\"http://schemas.datacontract.org/2004/07/SpectrumBridge.WhiteSpaces.Services.v3\"><AntennaHeight>%d</AntennaHeight><ContactCity>%s</ContactCity><ContactCountry>%s</ContactCountry><ContactEmail>%s</ContactEmail><ContactName>%s</ContactName><ContactPhone>%s</ContactPhone><ContactState>%s</ContactState><ContactStreet>%s</ContactStreet><ContactZip>%s</ContactZip><DeviceOwner>%s</DeviceOwner><DeviceType>%d</DeviceType><Latitude>%s</Latitude><Longitude>%s</Longitude></RegistrationRequest>"
          ,wsCtx->antennaeHeightInMeters,(wsCtx->addressInfo).contactCity,
          (wsCtx->addressInfo).contactCountry,(wsCtx->addressInfo).contactEmail,
          (wsCtx->addressInfo).contactName,(wsCtx->addressInfo).contactPhoneNumber,
          (wsCtx->addressInfo).contactProvince,(wsCtx->addressInfo).contactStreetAddress,
          (wsCtx->addressInfo).contactPostalCode,(wsCtx->addressInfo).ownerName,wsCtx->mode,
          wsCtx->latitudeString,wsCtx->longitudeString);
  sVar1 = strlen(wsCtx->tempProcessingBuffer);   // body length for Content-Length
  sprintf(wsCtx->transmitBuffer,
          "PUT https://%s/v3/devices/%s/%s/%s HTTP/1.1\r\nHost: %s\r\nContent-Type: Application/xml \r\nContent-Length: %d\r\n\n%s\r\n\r\n"
          ,wsCtx->hostURL,wsCtx->regulatoryDomain,wsCtx->fccID,wsCtx->serialNumber,wsCtx->hostURL,
          sVar1,wsCtx->tempProcessingBuffer);
  sVar1 = strlen(wsCtx->transmitBuffer);         // whole-request length
  return sVar1;
}

/* buildChannelRequest(wsCtx): formats the channel GET into transmitBuffer.
 * All parameters go in the URL; there is no body. type=channelLookUpMode,
 * ant=antennaeHeightInMeters. Returns the request length (sVar1). */
int buildChannelRequest(WhiteSpaceContext *wsCtx)
{
  size_t sVar1;
  sprintf(wsCtx->transmitBuffer,
          "GET https://%s/v3/channels/%s/%s/%s/?fccid=%s&serial=%s&type=%d&ant=%d HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n"
          ,wsCtx->hostURL,wsCtx->regulatoryDomain,wsCtx->latitudeString,wsCtx->longitudeString,
          wsCtx->fccID,wsCtx->serialNumber,wsCtx->channelLookUpMode,wsCtx->antennaeHeightInMeters,
          wsCtx->hostURL,"HTMLGET 1.0");
  sVar1 = strlen(wsCtx->transmitBuffer);
  return sVar1;
}

/* processXmlResponseMessage(wsCtx): parse the channel-list XML in
 * wsCtx->receiveBuffer into wsCtx->availableChannelMap.
 *
 * The many local_NNN char[4] / memset blocks near the top are just the three
 * literal tag strings being copied onto the stack ("ChannelCount",
 * "ChannelList", "RefreshIn"); Ghidra rendered the compiler's inlined string
 * initialization byte by byte. They are not interesting.
 *
 * Variables that matter:
 *   xml        = root ezxml node from ezxml_parse_str (NULL on parse failure)
 *   peVar1     = the current child node being read
 *   local_680  = strtok cursor over the comma-separated channel list
 *   local_67c  = ChannelCount value reported by the server
 *   local_678  = outer loop index 0..2 (0=count, 1=list, 2=RefreshIn)
 *   aiStack_2e0 = a 90-entry presence table; aiStack_2e0[ch*2] = 1 marks
 *                 channel `ch` as present (stride 2 is a Ghidra view of an
 *                 array of 2-int structs). Duplicates collapse here.
 *   local_c    = copy of the loop index (debug leftover)
 * Returns 0 on success, 1 on malformed response or count mismatch.
 */
int processXmlResponseMessage(WhiteSpaceContext *wsCtx)
{
  size_t len;                        // length of receiveBuffer
  ezxml_t_conflict xml;              // parsed root
  ezxml_t_conflict peVar1;           // current node
  int iVar2;                         // atoi scratch / channel number
  uint uVar3;                        // running channelCount
  char *local_680;                   // strtok cursor
  uint local_67c;                    // server-reported ChannelCount
  uint local_678;                    // loop index
  char local_66c [4];                // "Chan" ... (inlined "ChannelCount")
  char local_668 [4];
  char local_664 [4];
  char local_660;
  undefined1 auStack_65f [288];
  char local_53f [4];                // "Chan" ... (inlined "ChannelList")
  char local_53b [4];
  char local_537 [4];
  undefined1 auStack_533 [289];
  char local_412 [4];                // "Refr" ... (inlined "RefreshIn")
  char local_40e [4];
  char local_40a [2];
  undefined1 auStack_408 [292];
  undefined1 auStack_2e4 [4];
  int aiStack_2e0 [180];             // 90-channel presence table (stride 2)
  int local_10;                      // return value
  uint local_c;                      // index copy
  memset(local_66c,0,0x387);
  local_66c[0] = s_ChannelCount_00517858[0];     // --- begin inlined "ChannelCount" ---
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
  local_660 = s_ChannelCount_00517858[0xc];      // --- end "ChannelCount" ---
  memset(auStack_65f,0,0x120);
  local_53f[0] = s_ChannelList_00517988[0];      // --- inlined "ChannelList" ---
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
  local_412[0] = s_RefreshIn_00517ab8[0];        // --- inlined "RefreshIn" ---
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
  memset(auStack_2e4,0,0x2d0);                    // clear the presence table
  xml = ezxml_parse_str(wsCtx->receiveBuffer,len); // ezxml: skips the HTTP headers before '<'
  if (xml == (ezxml_t_conflict)0x0) {
    puts("Malformed Response");
    local_10 = 1;
  }
  else {
    for (local_678 = 0; local_678 < 3; local_678 = local_678 + 1) {
      local_c = local_678;
      if (local_678 == 1) {                        // <ChannelList>
        peVar1 = ezxml_get(xml);                   // node holding the CSV text
        local_680 = strtok(peVar1->txt,",");       // split on commas
        while (local_680 != (char *)0x0) {
          iVar2 = atoi(local_680);                 // channel number
          aiStack_2e0[iVar2 * 2] = 1;              // mark present (de-dups)
          local_680 = strtok((char *)0x0,",");
        }
      }
      else if (local_678 == 0) {                   // <ChannelCount>
        peVar1 = ezxml_get(xml);
        local_67c = atoi(peVar1->txt);             // server-claimed count
      }
      else if (local_678 == 2) {                   // <RefreshIn>
        peVar1 = ezxml_get(xml);
        iVar2 = atoi(peVar1->txt);
        wsCtx->refreshInHrs = iVar2;               // stored but never used elsewhere
      }
    }
    // Flatten the presence table (0..89) into the available channel list.
    for (local_678 = 0; local_678 < 0x5a; local_678 = local_678 + 1) {
      if (aiStack_2e0[local_678 * 2] != 0) {
        uVar3 = (wsCtx->availableChannelMap).channelCount;
        (wsCtx->availableChannelMap).channels[uVar3] = local_678;
        (wsCtx->availableChannelMap).channelCount = uVar3 + 1;
      }
    }
    ezxml_free(xml);                               // release the tree
    if (local_67c == (wsCtx->availableChannelMap).channelCount) {   // count must match
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

/* httpHeaderStatus(wsCtx, msgType): returns the numeric HTTP status code from
 * the response line ("HTTP/1.1 200 OK" -> 200) and records it on the context.
 *   local_48  = bytes remaining to scan (starts at strlen(receiveBuffer))
 *   local_40  = cursor into receiveBuffer
 *   local_44  = index into acStack_3c while copying the status token
 *   acStack_3c = the status-code token being assembled (e.g. "200")
 * The first loop skips to the first space; the second copies the next token. */
int httpHeaderStatus(WhiteSpaceContext *wsCtx,MessageType msgType)
{
  int iVar1;                         // atoi result (the status code)
  size_t local_48;                   // remaining length
  int local_44;                      // write index into acStack_3c
  char *local_40;                    // scan cursor
  char acStack_3c [44];              // status-code token
  MessageType local_10;              // copy of msgType
  local_44 = 0;
  local_48 = strlen(wsCtx->receiveBuffer);
  for (local_40 = wsCtx->receiveBuffer; (0 < (int)local_48 && (*local_40 != ' '));
      local_40 = local_40 + 1) {                  // skip "HTTP/1.x"
    local_48 = local_48 - 1;
  }
  while( true ) {                                  // copy the status number
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

/* getWhiteSpaceStatus(wsCtx, msgType): reads the "SBI-Status:" and "Date:"
 * headers. Returns the SBI status code (0 = WS_SUCCESS).
 *   pcVar2    = cursor into receiveBuffer
 *   param0    = pointer to the "SBI-Status:" substring
 *   local_14  = parsed SBI-Status integer (the real result)
 *   local_a0  = return value / scratch copy
 *   acStack_90 = throwaway buffer catching the "%s" label tokens in sscanf
 *   acStack_2c, iStack_24, ... = the Date header's day/number/month fields;
 *     only wsCtx->absoluteTimeBuffer (the time-of-day token) is kept.
 * WARNING: if "SBI-Status:" is absent, param0 is NULL and sscanf will fault. */
WSServiceStatus getWhiteSpaceStatus(WhiteSpaceContext *wsCtx,MessageType msgType)
{
  WSServiceStatus WVar1;             // parsed SBI status
  char *param0;                      // -> "SBI-Status:"
  char *pcVar2;                      // scan cursor / -> "Date:"
  WSServiceStatus local_a0;          // return value
  char acStack_90 [100];             // scratch for label tokens
  char acStack_2c [8];               // Date: weekday token
  int iStack_24;                     // Date: day number
  char acStack_20 [4];               // Date: month token
  int iStack_1c;                     // Date: year
  char acStack_18 [4];               // Date: timezone token
  WSServiceStatus local_14;          // SBI-Status integer
  MessageType local_10;
  pcVar2 = wsCtx->receiveBuffer;
  local_a0 = WS_UNKNOWN_STATUS;
  local_14 = WS_UNKNOWN_STATUS;
  if (pcVar2 != (char *)0x0) {
    param0 = strstr(pcVar2,"SBI-Status:");
    sscanf(param0,"%s %d",acStack_90,(int *)&local_14);   // needs a space after the colon
    WVar1 = local_14;
    local_a0 = local_14;
    pcVar2 = strstr(pcVar2,"Date:");
    local_10 = msgType;
    if (msgType == WS_REGISTRATION_MESSAGE) {
      wsCtx->registrationStatus = WVar1;
      wsCtx->libraryState = LIB_STATE_REGISTRATION_SUCCESS;
      if (pcVar2 != (char *)0x0) {
        strncpy(wsCtx->registrationTimeStamp,pcVar2 + 6,0x1d);  // 29 chars after "Date: "
      }
    }
    else if ((msgType == WS_CHANNEL_REQUEST_MESSAGE) &&
            (wsCtx->channelRequestStatus = WVar1, pcVar2 != (char *)0x0)) {
      strncpy(wsCtx->channelRequestTimeStamp,pcVar2 + 6,0x1d);
    }
    if (pcVar2 != (char *)0x0) {
      sscanf(pcVar2,"%s %s %d %s %d %s %s",acStack_90,acStack_2c,&iStack_24,acStack_20,&iStack_1c,
             wsCtx->absoluteTimeBuffer,acStack_18);           // only absoluteTimeBuffer is kept
    }
  }
  return local_a0;
}

/* =====================================================================
 * whitespace_context.c  --  the WhiteSpaceContext accessors, the
 *   /etc/config status-file writers, and channel-map filtering
 * =====================================================================
 *
 * The simple setters/getters are summarized here rather than reproduced; all
 * take (context, value) and copy into the matching field, and the length-
 * checked setters return LIB_STATUS_SET_SUCCESS/FAILURE. NONE of the length-
 * checked setters is actually reached at runtime (they are only called by
 * setWhiteSpaceContext(), which is itself dead code - readXmlIntoContext uses
 * raw strcpy instead). See the raw file for their full bodies.
 *
 *   setHostURL            len<200  -> strcpy context->hostURL
 *   setRegulatoryDomain   len<3    -> strcpy context->regulatoryDomain
 *   setFccId              len<0x18 -> strncpy context->fccID
 *   setSerialNumber       len<0x20 -> strncpy context->serialNumber
 *   setLatitudeAndLongitude  each len<100 -> strncpy lat/long strings
 *   setWhiteSpaceContextAddress  nested length checks on the 9 contact fields
 *   setComPortNumber / setModeInContext / setChannelLookUpModeInContext /
 *     setAntennaeHeightInMeters   plain field stores
 *   getRegistrationStatus  -- returns context->mode (looks like a BUG: it
 *     returns mode, not registrationStatus; unused anyway)
 *   getRegistrationErrorCode / getChannelRequestStatus /
 *     getChannelRequestErrorCode / getLibraryState / getChannelCountFromChannelMap
 *     -- field reads
 *   printChannelMap / printPreferredChannelMap  -- empty (stubbed out)
 *   setWhiteSpaceContext  -- dead orchestrator that calls all the setters
 */

/* initializeWhiteSpaceContext(context): zero the whole 5232-byte context and
 * set the two non-zero defaults. Called once from main before XML load. */
void initializeWhiteSpaceContext(WhiteSpaceContext *context)
{
  memset(context,0,0x1470);                       // 0x1470 = 5232 bytes
  context->libraryState = LIB_STATE_AWAITING_INITIALIZATION;
  (context->availableChannelMap).channelCount = 0;
  (context->preferredChannelMap).channelCount = 0;
  context->enumeratedTransmitPower = 10;          // default power index (-> "p32")
  memset(context->absoluteTimeBuffer,0,100);
  memset(context->registrationTimeStamp,0,100);
  memset(context->channelRequestTimeStamp,0,100);
  memset(context->meldStreamingIpAddress,0,100);
  return;
}

/* createChannelRequestStatusFile(context): rewrite /etc/config/channel_status
 * (OpenWrt UCI format) with the last channel-request status code and timestamp.
 * __s = the FILE*. NOTE: putc/fflush/fclose run even when fopen returned NULL
 * (__s == 0), so a non-writable /etc/config will crash here. */
void createChannelRequestStatusFile(WhiteSpaceContext *context)
{
  FILE *__s;
  __s = fopen("/etc/config/channel_status","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'current\' \'channel_request_status\'\n",1,0x2b,__s);
    fprintf(__s,"\t option \'status\' \'%d\'\n",context->channelRequestStatus);
    fwrite(" config \'current\' \'channel_request_timestamp\'\n",1,0x2e,__s);
    fprintf(__s,"\t option \'time_stamp\' \'%s\'\n",context->channelRequestTimeStamp);
  }
  putc(10,__s);                                   // '\n' - runs even if __s==NULL
  fflush(__s);
  fclose(__s);
  return;
}

/* createChannelMapFile(context): rewrite /etc/config/channels with the
 * comma-separated available-channel list (post-filter). local_c = loop index.
 * The DAT_00517da4 fwrite is the closing "'\n" of the UCI option line. */
void createChannelMapFile(WhiteSpaceContext *context)
{
  FILE *__s;
  uint local_c;                      // channel index
  __s = fopen("/etc/config/channels","w+");
  if (__s != (FILE *)0x0) {
    fwrite(" config \'channel\' \'channel\'\n",1,0x1c,__s);
    fwrite("\t option \'channels\' \'",1,0x15,__s);
    if ((context->availableChannelMap).channelCount != 0) {
      for (local_c = 0; local_c < (context->availableChannelMap).channelCount; local_c = local_c + 1
          ) {
        if (local_c + 1 == (context->availableChannelMap).channelCount) {
          fprintf(__s,"%d",(context->availableChannelMap).channels[local_c]);   // last: no comma
        }
        else {
          fprintf(__s,"%d,",(context->availableChannelMap).channels[local_c]);
        }
      }
    }
    fwrite(&DAT_00517da4,1,2,__s);                // "'\n"
    fflush(__s);
    fclose(__s);
  }
  return;
}

/* createRegistrationStatusFile(context): rewrite /etc/config/registration with
 * the registration status code and timestamp. Same NULL-FILE* hazard as above. */
void createRegistrationStatusFile(WhiteSpaceContext *context)
{
  FILE *__s;
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

/* calculateDeltaTimeFromCurrentInSecs(context): DEAD CODE. Intended to compute
 * seconds until some refresh time from absoluteTimeBuffer ("%d:%d:%d") and the
 * longitude, but the result (in_f0_1 comes from an unseen FPU register, a
 * decompiler artifact) is never consumed. local_14/10/c = parsed H:M:S. */
int calculateDeltaTimeFromCurrentInSecs(WhiteSpaceContext *context)
{
  double in_f0_1;                    // decompiler artifact (unresolved $f0)
  int local_18;                      // delta hours
  int local_14;                      // hours
  int local_10;                      // minutes
  int local_c;                       // seconds
  sscanf(context->absoluteTimeBuffer,"%d:%d:%d",&local_14,&local_10,&local_c);
  atof(context->longitudeString);
  in_f0_1 = in_f0_1 / DAT_00517e30;              // DAT_00517e30 == 15.0 (degrees/hour)
  ceil(in_f0_1);
  local_18 = 0x17 - (local_14 + (int)in_f0_1);   // 23 - (hour + lon/15)
  if (local_18 < 0) {
    local_18 = 0x18 - local_18;
  }
  return local_18 * 0xe10 + (0x3b - local_10) * 0x3c + (0x3b - local_c);  // h*3600 + m*60 + s
}

/* reAdjustChannelMapToMeldCapabilities(context): filter availableChannelMap in
 * place, keeping only channels in (0x14, 0x34) i.e. 21..51. This is the filter
 * that reduces the server's channel list to what the Meld radio can use.
 *   auStack_108 = scratch list of the kept channels (max 65)
 *   local_110   = count of kept channels
 *   local_10c   = source index
 */
void reAdjustChannelMapToMeldCapabilities(WhiteSpaceContext *context)
{
  uint local_110;                    // kept count
  uint local_10c;                    // source index
  uint auStack_108 [65];             // kept channels
  local_110 = 0;
  if ((context->availableChannelMap).channelCount != 0) {
    for (local_10c = 0; local_10c < (context->availableChannelMap).channelCount;
        local_10c = local_10c + 1) {
      if ((0x14 < (context->availableChannelMap).channels[local_10c]) &&    // > 20
         ((context->availableChannelMap).channels[local_10c] < 0x34)) {     // < 52
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

/* =====================================================================
 * ssl_interface.c  --  TCP + OpenSSL transport (statically linked
 *   OpenSSL 1.0.0, so SSLv3 / TLS 1.0 only)
 * =====================================================================
 *
 * Helpers summarized (full bodies in the raw file):
 *   initializeSecurityCtx() -- SSL_library_init + SSL_load_error_strings +
 *       RAND_seed("This is a Random Number",0x18)  (a fixed, weak seed)
 *   pexit(s)      -- perror(s), then returns (does NOT exit despite the name)
 *   log_ssl()     -- drain the OpenSSL error queue to stdout via "*** %s"
 *   create_tcp_socket() -- socket(AF_INET, SOCK_STREAM=2, IPPROTO_TCP=6)
 *   get_ip(host,ip) -- gethostbyname + inet_ntop; fills `ip`; 1 ok / 0 fail
 *   recv_to(...)  -- select()-with-timeout wrapper around recv(); DEAD CODE,
 *       the TLS path uses recv_to_ssl instead
 *   recv_to_ssl(ssl,buf,len,to) -- just SSL_read(ssl,buf,len); the `to`
 *       (5000 ms) timeout argument is IGNORED
 *   sslShutDown(wsCtx,ctx,ssl) -- SSL_shutdown, close(activeSockForSession),
 *       SSL_free, SSL_CTX_free
 */

/* verify_callback(ok, ctx): OpenSSL per-certificate verify hook.
 * Returns 1 to accept the certificate, 0 to reject. When OpenSSL already
 * says ok, it passes through. When OpenSSL flags an error, three specific
 * X509_V_ERR codes are turned into "accept":
 *     9  = CERT_NOT_YET_VALID
 *     10 = CERT_HAS_EXPIRED
 *     18 = DEPTH_ZERO_SELF_SIGNED_CERT  (a self-signed leaf)
 * Any other error (e.g. 19 self-signed-in-chain, 20 unable-to-get-issuer) is
 * still rejected. No hostname matching is done here. This is why the bundled
 * spoof servers can present a self-signed / expired test certificate.
 *   a          = subject name of the current cert
 *   acStack_110 = buffer for the X509_NAME_oneline() text (read but unused)
 *   local_res0 = the accept/reject result
 */
int verify_callback(int ok,X509_STORE_CTX *ctx)
{
  X509_NAME *a;                      // subject name
  int iVar1;                         // ctx->error
  int local_res0;                    // result (1=accept)
  char acStack_110 [264];            // subject-name text (unused)
  X509_STORE_CTX_get_current_cert((X509_STORE_CTX *)ctx);
  a = X509_get_subject_name((X509 *)ctx->current_cert);
  X509_NAME_oneline(a,acStack_110,0x100);
  local_res0 = ok;
  if (ok == 0) {
    iVar1 = ctx->error;
    if (iVar1 == 10) {               // expired
      local_res0 = 1;
    }
    else if (iVar1 == 0x12) {        // self-signed leaf (18)
      local_res0 = 1;
    }
    else if (iVar1 == 9) {           // not yet valid
      local_res0 = 1;
    }
    else {
      local_res0 = 0;                // everything else: reject
    }
  }
  return local_res0;
}

/* establishASecureSession(wsCtx, ctx, ssl): open one TCP+TLS session to
 * wsCtx->hostURL:443 and perform the handshake. Returns an SslStatus.
 * A fresh SSL_CTX/SSL is built per call and torn down on any failure.
 *   meth      = SSLv23_client_method()  (negotiates down to SSLv3/TLS1.0)
 *   acStack_38 = resolved dotted-quad IP string from get_ip()
 *   local_24  = sockaddr_in {family=AF_INET, port=htons(0x1bb=443)}
 *   local_48  = failure sub-status (verify failure vs session failure)
 *   local_10  = return value
 * The listen port 443 and the verify callback are both hard-coded here.
 */
SslStatus establishASecureSession(WhiteSpaceContext *wsCtx,SSL_CTX **ctx,SSL **ssl)
{
  SSL_METHOD *meth;
  SSL_CTX *pSVar1;
  SSL *pSVar2;
  int iVar3;                         // misc OpenSSL/socket return codes
  int *piVar4;                       // errno location
  long lVar5;                        // SSL_get_verify_result
  SslStatus local_48;                // failure sub-status
  char acStack_38 [20];              // resolved IP string
  sockaddr local_24;                 // server address (port 443)
  SslStatus local_10;                // return
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
    if (ssl == (SSL **)0x0) {                     // (checks the wrong pointer, but *ssl!=NULL here)
      puts("SSl New returned NULL");
      SSL_CTX_free((SSL_CTX *)*ctx);
      local_10 = SSL_NEW_FAILURE;
    }
    else {
      iVar3 = SSL_CTX_load_verify_locations
                        ((SSL_CTX *)*ctx,wsCtx->securityCertFileName,wsCtx->securityCertPath);
      if (iVar3 == 0) {
        log_ssl();                                // load failure is only logged
      }
      SSL_CTX_set_verify((SSL_CTX *)*ctx,1,verify_callback);   // 1 = SSL_VERIFY_PEER
      memset(acStack_38,0,0x14);
      iVar3 = get_ip(wsCtx->hostURL,acStack_38);  // DNS A record
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
          local_24.sa_family = 2;                       // AF_INET
          local_24.sa_data._0_2_ = htons(0x1bb);        // port 443 (hard-coded)
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
                iVar3 = SSL_connect((SSL *)*ssl);           // the TLS handshake
                if (iVar3 == 1) {
                  local_10 = SSL_STATUS_SUCCESS;
                }
                else {
                  puts("SSL Handshake failed");
                  lVar5 = SSL_get_verify_result((SSL *)*ssl);
                  if (lVar5 == 0) {
                    local_48 = SSL_SESSION_FAILURE;         // handshake failed, cert was fine
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

/* sslSendMessageToService(wsCtx, ctx, ssl): write transmitBuffer over TLS,
 * then write a 3-byte "\n\n\0" trailer (DAT_00517f8c). On SSL_write == -1 it
 * tears the session down. num = strlen(transmitBuffer); local_18 = status. */
SslStatus sslSendMessageToService(WhiteSpaceContext *wsCtx,SSL_CTX *ctx,SSL *ssl)
{
  size_t num;                        // request length
  int iVar1;                         // SSL_write result
  SslStatus local_18;                // status
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
    SSL_write((SSL *)ssl,&DAT_00517f8c,3);        // "\n\n\0" trailer
    local_18 = SSL_STATUS_SUCCESS;
  }
  return local_18;
}

/* sslReceiveMessage(wsCtx, ctx, ssl): read the whole response into
 * receiveBuffer by looping SSL_read until it returns <= 0 (i.e. the server
 * closes the connection). Returns the total byte count.
 *   local_20 = write cursor into receiveBuffer
 *   local_1c = length of the PREVIOUS chunk (used to advance the cursor)
 *   local_14 = running total
 *   __n      = bytes from the current SSL_read
 * CAUTION: receiveBuffer is 1024 bytes and the strncpy below is bounded by the
 * chunk size, not by remaining space, so a response > ~1 KB overruns it. The
 * cursor advance (local_20 += local_1c) also lags by one chunk. */
int sslReceiveMessage(WhiteSpaceContext *wsCtx,SSL_CTX *ctx,SSL *ssl)
{
  size_t __n;                        // current chunk length
  char *local_20;                    // write cursor
  size_t local_1c;                   // previous chunk length
  int local_14;                      // total bytes
  local_14 = 0;
  local_1c = 0;
  memset(wsCtx->receiveBuffer,0,0x400);
  memset(wsCtx->tempProcessingBuffer,0,0x400);
  local_20 = wsCtx->receiveBuffer;
  while( true ) {
    __n = recv_to_ssl(ssl,wsCtx->tempProcessingBuffer,0x400,5000);  // 5000ms arg ignored
    if ((int)__n < 1) break;                      // <=0 -> server closed, done
    local_20 = local_20 + local_1c;
    strncpy(local_20,wsCtx->tempProcessingBuffer,__n);
    local_14 = local_14 + __n;
    local_1c = __n;
  }
  return local_14;
}

/* =====================================================================
 * whitespace_statemachine.c  --  the database-client state machine (thread A)
 * =====================================================================
 *
 * processWSStateMachine() keeps two statics: `wsState` (current
 * WHITESPACE_STATES_ENUM, starts at WS_STATE_IDLE=1) and `event` (current
 * WHITESPACE_EVENTS_ENUM, starts at WS_EVENT_STARTED=1). Each tick it calls
 * processFunctions[wsState](...) and stores the returned next state. Every
 * state handler has the same signature:
 *     (WhiteSpaceContext *wsCtx, WhiteSpaceEvent *event,
 *      SSL_CTX **sslCtx, SSL **ssl, pthread_mutex_t *whiteSpaceMutex)
 * and returns the next WhiteSpaceState.
 *
 * The scan-related early states are stubs that only advance and set an event
 * (full bodies in the raw file):
 *     IDLE                -> BACKGROUND_SCANNING
 *     BACKGROUND_SCANNING -> ASSOCIATING        (event = CHANNEL_LOCKED)
 *     ASSOCIATING         -> OBTAINING_IP        (event = ASSOCIATED)
 *     OBTAINING_IP        -> SERVICE_RESOLUTION  (event = IP_OBTAINED)
 *     SERVICE_RESOLUTION  -> REGISTRATION        (event = SERVICE_RESOLVED)
 *     REGISTRATION        -> WAIT_TO_ESTABLISH_SECURE_SESSION (event=REG_REQUEST_SEND)
 *     CHANNEL_REQUEST     -> WAIT_TO_ESTABLISH_SECURE_SESSION (event=CHANNEL_REQUEST_SEND)
 *
 * The global freeRunningClockForReferenceInSeconds just accumulates every
 * sleep() below; nothing reads it back.
 */

/* processWS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION: open the TLS session and
 * branch on whether we are here to register or to ask for channels (decided by
 * the current event). On success, go to the matching SEND state. On failure,
 * clear the relevant status, write the status files, and either retry
 * registration immediately or fall through to the renewal wait for channels.
 *   SVar1    = establishASecureSession result
 *   local_10 = next state
 */
WhiteSpaceState
processWS_STATE_WAIT_TO_ESTABLISH_SECURE_SESSION
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  SslStatus SVar1;                   // TLS connect result
  WhiteSpaceState local_10;          // next state
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
    if (*event == WS_EVENT_REGISTRATION_REQUEST_SEND) {    // failed while registering
      pthread_mutex_lock((pthread_mutex_t *)whiteSpaceMutex);
      wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
      wsCtx->channelRequestStatus = WS_UNKNOWN_STATUS;
      (wsCtx->availableChannelMap).channelCount = 0;
      pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      createRegistrationStatusFile(wsCtx);
      createChannelMapFile(wsCtx);
      createChannelRequestStatusFile(wsCtx);
      local_10 = WS_STATE_REGISTRATION;                    // retry registration (no sleep here)
    }
    else {                                                 // failed while asking for channels
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

/* processWS_STATE_REGISTRATION_SEND: build and send the registration PUT.
 * local_18 = next state (default WAIT_FOR_REGISTRATION_RESPONSE). iVar1 =
 * request length; SVar2 = send status. On a send failure after a prior secure-
 * session failure it sleeps 5 s and loops back to re-establish. */
WhiteSpaceState
processWS_STATE_REGISTRATION_SEND
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  int iVar1;                         // request length
  SslStatus SVar2;                   // send result
  WhiteSpaceState local_18;          // next state
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

/* processWS_STATE_WAIT_FOR_REGISTRATION_RESPONSE: read the reply, check the
 * HTTP status (must be 200) and the SBI-Status (must be WS_SUCCESS). Sets the
 * event accordingly and always hands off to SECURE_SESSION_SHUTDOWN next.
 * iVar1 = receive length / HTTP status; WVar2 = SBI status. */
WhiteSpaceState
processWS_STATE_WAIT_FOR_REGISTRATION_RESPONSE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  int iVar1;                         // recv length / HTTP code
  WSServiceStatus WVar2;             // SBI status
  wsCtx->registrationStatus = WS_UNKNOWN_STATUS;
  if (*event == WS_EVENT_REGISTRATION_REQUEST_SEND) {
    iVar1 = sslReceiveMessage(wsCtx,*sslCtx,*ssl);
    if (iVar1 < 1) {                              // nothing received -> timeout
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
        else {                                     // HTTP 200 but SBI error
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
      else {                                       // HTTP != 200
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

/* processWS_STATE_SECURE_SESSION_SHUTDOWN: tear down the TLS session, then pick
 * the next state from the event that got us here. local_18 = next state.
 * This is the junction that turns a successful registration into the first
 * channel request, and routes every terminal event into the renewal wait. */
WhiteSpaceState
processWS_STATE_SECURE_SESSION_SHUTDOWN
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  WhiteSpaceState local_18;          // next state
  local_18 = WS_STATE_CHANNEL_REQUEST;
  sslShutDown(wsCtx,sslCtx,ssl);
  switch(*event) {
  case WS_EVENT_REGISTRATION_REQUEST_SEND_FAILED:
  case WS_EVENT_REGISTRATION_RESPONSE_TIMEOUT:
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_18 = WS_STATE_IDLE;                      // restart from the top
    break;
  case WS_EVENT_REGISTRATION_RESPONSE_RCVD:
    local_18 = WS_STATE_CHANNEL_REQUEST;           // registered -> ask for channels
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

/* processWS_STATE_CHANNEL_REQUEST_SEND: build and send the channel GET.
 * local_18 = next state (default WAIT_FOR_CHANNEL_RESPONSE). iVar1 = request
 * length; SVar2 = send status. Mirrors REGISTRATION_SEND. */
WhiteSpaceState
processWS_STATE_CHANNEL_REQUEST_SEND
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  int iVar1;
  SslStatus SVar2;
  WhiteSpaceState local_18;
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

/* processWS_STATE_WAIT_FOR_CHANNEL_RESPONSE: read reply, require HTTP 200 and
 * SBI WS_SUCCESS, then parse the channel list (processXmlResponseMessage) and,
 * if any channels survive, filter them to the Meld-capable range. Emits the
 * CHANNEL_RESPONSE_* / NO_CHANNEL_AVAILABLE event and goes to shutdown next.
 * iVar1 = recv length / HTTP code / channel count; WVar2 = SBI status. */
WhiteSpaceState
processWS_STATE_WAIT_FOR_CHANNEL_RESPONSE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  int iVar1;
  WSServiceStatus WVar2;
  if (*event == WS_EVENT_CHANNEL_REQUEST_SEND) {
    iVar1 = sslReceiveMessage(wsCtx,*sslCtx,*ssl);
    if (iVar1 < 1) {                              // timeout
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
          iVar1 = processXmlResponseMessage(wsCtx);         // parse ChannelList XML
          if (iVar1 == 0) {                                 // parsed OK, count matched
            iVar1 = getChannelCountFromChannelMap(wsCtx);
            if (iVar1 < 1) {                                // zero channels
              (wsCtx->availableChannelMap).channelCount = 0;
              createChannelMapFile(wsCtx);
              *event = WS_EVENT_NO_CHANNEL_AVAILABLE;
              wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
            }
            else {
              reAdjustChannelMapToMeldCapabilities(wsCtx);  // keep 21..51
              printChannelMap(wsCtx);
              createChannelMapFile(wsCtx);
              *event = WS_EVENT_CHANNEL_RESPONSE_RCVD;
              wsCtx->libraryState = LIB_STATE_CHANNEL_AVAILABLE;
            }
          }
          else {                                            // parse/count failure
            (wsCtx->availableChannelMap).channelCount = 0;
            createChannelMapFile(wsCtx);
            *event = WS_EVENT_CHANNEL_RESPONSE_FAILED;
            wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
          }
        }
        else {                                              // SBI error
          (wsCtx->availableChannelMap).channelCount = 0;
          createChannelMapFile(wsCtx);
          *event = WS_EVENT_CHANNEL_RESPONSE_FAILED;
          wsCtx->libraryState = LIB_STATE_CHANNEL_NOT_AVAILABLE;
        }
        pthread_mutex_unlock((pthread_mutex_t *)whiteSpaceMutex);
      }
      else {                                                // HTTP != 200
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

/* processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE: the idle/renewal timer.
 * Sleeps for a duration chosen by the event, then returns to CHANNEL_REQUEST
 * (or REGISTRATION). httpFailureCount is a static exponential-backoff counter
 * used only by the HTTP_STATUS_FAILED branch (which is never actually reached).
 *   local_18 = computed backoff seconds
 *   local_14 = next state
 * Key durations: 0x14cd0 = 85200 s (23h40m, the normal refresh), 0xe10 = 3600 s
 * (1 h). The `default` arm (includes CHANNEL_RESPONSE_RCVD success) is the
 * 23h40m wait. */
WhiteSpaceState
processWS_STATE_WAIT_FOR_RENEWAL_OR_LOCATION_UPDATE
          (WhiteSpaceContext *wsCtx,WhiteSpaceEvent *event,SSL_CTX **sslCtx,SSL **ssl,
          pthread_mutex_t *whiteSpaceMutex)
{
  uint local_18;                     // backoff seconds
  WhiteSpaceState local_14;          // next state
  wsCtx->libraryState = LIB_STATE_WAITING_FOR_RENEWAL;
  switch(*event) {
  case WS_EVENT_SECURE_SESSION_FAILED:
  case WS_EVENT_CHANNEL_REQUEST_SEND_FAILED:
  case WS_EVENT_CHANNEL_RESPONSE_TIMEOUT:
    sleep(5);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 5;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    httpFailureCount = 0;
    break;
  case WS_EVENT_CHANNEL_RESPONSE_RCVD:
    httpFailureCount = 0;
  default:                                          // success path -> long sleep
    sleep(0x14cd0);                                 // 85200 s = 23 h 40 m
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0x14cd0;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    break;
  case WS_EVENT_HTTP_STATUS_FAILED:                 // (event never generated)
    httpFailureCount = httpFailureCount + 1;
    local_18 = httpFailureCount * 2;
    if (0x3c < (int)local_18) {
      httpFailureCount = 1;
      local_18 = 2;
    }
    sleep(local_18);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + local_18;
    break;
  case WS_EVENT_REGISTRATION_STATUS_FAILED:         // (event never generated)
    local_14 = WS_STATE_REGISTRATION;
    sleep(0xe10);                                   // 3600 s
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    httpFailureCount = 0;
    break;
  case WS_EVENT_CHANNEL_REQUEST_STATUS_FAILED:      // (event never generated)
    local_14 = WS_STATE_CHANNEL_REQUEST;
    sleep(0xe10);
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    httpFailureCount = 0;
    break;
  case WS_EVENT_NO_CHANNEL_AVAILABLE:
    sleep(0xe10);                                   // 1 h, then retry
    freeRunningClockForReferenceInSeconds = freeRunningClockForReferenceInSeconds + 0xe10;
    local_14 = WS_STATE_CHANNEL_REQUEST;
    httpFailureCount = 0;
  }
  return local_14;
}

/* initProcessStateFunctions(): fill the processFunctions[] dispatch table,
 * indexed by WhiteSpaceState (1..14). Called once by thread A at startup. */
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

/* processWSStateMachine(wsCtx, sslCtx, ssl, threadCondition): one tick.
 * Calls the handler for the static current state and stores the next state.
 * The static `event` is passed by address so handlers can update it. */
void processWSStateMachine
               (WhiteSpaceContext *wsCtx,SSL_CTX **sslCtx,SSL **ssl,pthread_mutex_t *threadCondition
               )
{
  wsState = (*processFunctions[wsState])(wsCtx,&event,sslCtx,ssl,threadCondition);
  return;
}

/* =====================================================================
 * processWhiteSpaceDeviceData.c  --  load the device-info XML into context
 * =====================================================================
 */

/* postProcessDataForEscapeSequence(data, convData): copy `data` to `convData`,
 * expanding the five XML metacharacters to their entities (& -> &amp;, < -> &lt;,
 * > -> &gt;, " -> &quot;, ' -> &apos;). Used on every text field read from the
 * XML before it is stored, because those fields are later pasted back into XML
 * bodies / URLs. convData must be large enough for the growth.
 *   local_18 = remaining input count   local_10 = read cursor   local_14 = write cursor */
void postProcessDataForEscapeSequence(char *data,char *convData)
{
  bool bVar1;                        // loop continue flag
  size_t local_18;                   // remaining chars
  char *local_14;                    // write cursor
  char *local_10;                    // read cursor
  local_18 = strlen(data);
  local_14 = convData;
  local_10 = data;
  while (bVar1 = local_18 != 0, local_18 = local_18 - 1, bVar1) {
    if (*local_10 == '&') {          // -> "&amp;"
      *local_14 = *local_10; local_10 = local_10 + 1;
      local_14[1] = 'a'; local_14[2] = 'm'; local_14[3] = 'p'; local_14[4] = ';';
      local_14 = local_14 + 5;
    }
    else if (*local_10 == '<') {     // -> "&lt;"
      *local_14 = *local_10; local_10 = local_10 + 1;
      local_14[1] = 'l'; local_14[2] = 't'; local_14[3] = ';';
      local_14 = local_14 + 4;
    }
    else if (*local_10 == '>') {     // -> "&gt;"
      *local_14 = *local_10; local_10 = local_10 + 1;
      local_14[1] = 'g'; local_14[2] = 't'; local_14[3] = ';';
      local_14 = local_14 + 4;
    }
    else if (*local_10 == '\"') {    // -> "&quot;"
      *local_14 = *local_10; local_10 = local_10 + 1;
      local_14[1] = 'q'; local_14[2] = 'u'; local_14[3] = 'o'; local_14[4] = 't'; local_14[5] = ';';
      local_14 = local_14 + 6;
    }
    else if (*local_10 == '\'') {    // -> "&apos;"
      *local_14 = *local_10; local_10 = local_10 + 1;
      local_14[1] = 'a'; local_14[2] = 'p'; local_14[3] = 'o'; local_14[4] = 's'; local_14[5] = ';';
      local_14 = local_14 + 6;
    }
    else {                           // ordinary char
      *local_14 = *local_10; local_14 = local_14 + 1; local_10 = local_10 + 1;
    }
  }
  return;
}

/* readXmlIntoContext(fileName, wsCtx): parse the device-info XML file and copy
 * its values into the context. This is the primary ezxml consumer.
 *
 * ezxml usage pattern (repeated throughout):
 *     peVar5 = ezxml_child(xml, responseTags[i]);   // find a direct child tag
 *     text   = peVar5 ? peVar5->txt : "";           // missing tag -> ""
 *     ... strcpy/atoi/postProcessDataForEscapeSequence(text, destField) ...
 * Nested blocks (<Location>, <TvwsDeviceInfo>) call ezxml_child AGAIN on the
 * sub-node (peVar5) using locationTags[] / tvwsDeviceTags[].
 *
 * The tag-name tables (string arrays in .rodata):
 *   responseTags[9]   = HostAddress, HostPortNumber, RegulatoryDomain, Location,
 *                       TvwsDeviceInfo, PreferredChannel, EnumeratedTransmitPower,
 *                       StreamingIpAddress, ModulationMode
 *   locationTags[2]   = Latitude, Longitude
 *   tvwsDeviceTags[15]= FccID, MfgSerialNumber, DeviceType, ChLookupDeviceType,
 *                       AntennaHeightAglMeters, ContactCity, ContactCountry,
 *                       ContactEmail, ContactName, ContactPhone, ContactState,
 *                       ContactStreet, ContactZip, CountryCode, DeviceOwner
 *
 * Key locals:
 *   xml        = parsed root node (ezxml_parse_file)
 *   peVar5     = current top-level child;  peVar1 = current nested child
 *   local_4f0  = outer index over responseTags (0..8)
 *   local_4f4  = inner index over locationTags / tvwsDeviceTags
 *   acStack_4ec[100] = staging buffer for a raw text value before escaping
 *   acStack_484[1028], local_488 = staging + the "," delimiter for the
 *                       PreferredChannel strtok split
 *   local_510  = strtok cursor over the preferred-channel CSV
 *   The many local_1c..local_80 char* are just the per-case "node ? txt : \"\""
 *   temporaries Ghidra split out; they are not distinct variables of interest.
 *
 * NOTE: regulatoryDomain is pre-seeded to "US" (memcpy DAT_00519e98), then
 * overwritten by <RegulatoryDomain> (case 2) and again by
 * <TvwsDeviceInfo><CountryCode> (case 4, sub-index 13). FccID/serialNumber set
 * here are later overwritten by the values read from the radio.
 */
int readXmlIntoContext(char *fileName,WhiteSpaceContext *wsCtx)
{
  ezxml_t_conflict xml;              // root node from ezxml_parse_file
  ezxml_t_conflict peVar1;           // nested child (Location / TvwsDeviceInfo sub-tag)
  DevMode DVar2;                     // atoi -> device mode
  uint uVar3;                        // atoi -> channel number
  int iVar4;                         // atoi scratch (port / antenna height)
  ezxml_t_conflict peVar5;           // current top-level child
  MeldModulationMode MVar6;          // atoi -> modulation
  uint uVar7;                        // preferred channel count
  char *local_510;                   // strtok cursor over PreferredChannel CSV
  uint local_4f4;                    // inner index (locationTags / tvwsDeviceTags)
  uint local_4f0;                    // outer index (responseTags, 0..8)
  char acStack_4ec [100];            // raw text staging before escaping
  undefined2 local_488 [2];          // holds the ',' strtok delimiter
  char acStack_484 [1028];           // PreferredChannel text staging
  /* local_80 .. local_1c are the per-arm "node ? node->txt : \"\"" string
   * temporaries Ghidra materialized (one distinct stack slot per switch arm).
   * Each holds the text pointer just long enough to strcpy/atoi it out. */
  char *local_80;                    // case 8 text (ModulationMode)
  char *local_7c;                    // case 7 text (StreamingIpAddress)
  char *local_78;                    // case 7 dest (= meldStreamingIpAddress)
  char *local_74;                    // case 6 text (EnumeratedTransmitPower)
  char *local_70;                    // case 5 text (PreferredChannel)
  char *local_6c;                    // case 4 sub 14 text (DeviceOwner)
  char *local_68;                    // case 4 sub 13 text (CountryCode)
  char *local_64;                    // case 4 sub 12 text (ContactZip)
  char *local_60;                    // case 4 sub 11 text (ContactStreet)
  char *local_5c;                    // case 4 sub 10 text (ContactState)
  char *local_58;                    // case 4 sub 9  text (ContactPhone)
  char *local_54;                    // case 4 sub 8  text (ContactName)
  char *local_50;                    // case 4 sub 7  text (ContactEmail)
  char *local_4c;                    // case 4 sub 6  text (ContactCountry)
  char *local_48;                    // case 4 sub 5  text (ContactCity)
  char *local_44;                    // case 4 sub 4  text (AntennaHeightAglMeters)
  char *local_40;                    // case 4 sub 3  text (ChLookupDeviceType)
  char *local_3c;                    // case 4 sub 2  text (DeviceType)
  char *local_38;                    // case 4 sub 1  text (MfgSerialNumber)
  char *local_34;                    // case 4 sub 0  text (FccID)
  char *local_30;                    // case 3 text (Longitude)
  char *local_2c;                    // case 3 text (Latitude)
  char *local_28;                    // case 2 text (RegulatoryDomain)
  char *local_24;                    // case 2 dest (= wsCtx->regulatoryDomain)
  char *local_20;                    // case 1 text (HostPortNumber)
  char *local_1c;                    // case 0 text (HostAddress)
  WhiteSpaceContext *local_18;       // scratch copy of wsCtx (case 0)
  uint local_14;                     // copy of the inner index (debug leftover)
  xml = ezxml_parse_file(fileName);                // ezxml: read + parse the whole file
  memcpy(wsCtx->regulatoryDomain,&DAT_00519e98,3); // seed regulatoryDomain = "US"
  (wsCtx->preferredChannelMap).channelCount = 0;
  memset(wsCtx->meldStreamingIpAddress,0,100);
  for (local_4f0 = 0; local_4f0 < 9; local_4f0 = local_4f0 + 1) {   // walk responseTags
    switch(local_4f0) {
    case 0:   /* <HostAddress> -> wsCtx->hostURL (raw strcpy, no length check) */
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      if (peVar5 == (ezxml_t_conflict)0x0) {
        local_1c = "";                             // missing tag -> empty string
      }
      else {
        local_1c = peVar5->txt;                    // ezxml node text
      }
      local_18 = wsCtx;
      strcpy(wsCtx->hostURL,local_1c);
      break;
    case 1:   /* <HostPortNumber> -> wsCtx->comPortNumber (atoi; never used later) */
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
    case 2:   /* <RegulatoryDomain> -> wsCtx->regulatoryDomain (raw strcpy) */
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
    case 3:   /* <Location> -> Latitude / Longitude via locationTags[0..1] */
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      for (local_4f4 = 0; local_4f4 < 2; local_4f4 = local_4f4 + 1) {
        peVar1 = ezxml_child(peVar5,readXmlIntoContext::locationTags[local_4f4]); // nested child
        local_14 = local_4f4;
        if (local_4f4 == 0) {                      // <Latitude>
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_2c = "";
          }
          else {
            local_2c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_2c);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->latitudeString);   // escape + store
        }
        else if (local_4f4 == 1) {                 // <Longitude>
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
    case 4:   /* <TvwsDeviceInfo> -> 15 fields via tvwsDeviceTags[0..14].
               * Text fields are escaped via postProcessDataForEscapeSequence;
               * sub 2/3/4 are numeric (atoi). sub 13 (CountryCode) overwrites
               * regulatoryDomain set in case 2; sub 0/1 (FccID/MfgSerialNumber)
               * are later overwritten by the radio-read values. */
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      for (local_4f4 = 0; local_4f4 < 0xf; local_4f4 = local_4f4 + 1) {
        peVar1 = ezxml_child(peVar5,readXmlIntoContext::tvwsDeviceTags[local_4f4]); // nested child
        switch(local_4f4) {
        case 0:   /* <FccID> -> wsCtx->fccID */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_34 = "";
          }
          else {
            local_34 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_34);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->fccID);
          break;
        case 1:   /* <MfgSerialNumber> -> wsCtx->serialNumber */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_38 = "";
          }
          else {
            local_38 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_38);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->serialNumber);
          break;
        case 2:   /* <DeviceType> -> wsCtx->mode (atoi, DEVICE_MODE_ENUM) */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_3c = "";
          }
          else {
            local_3c = peVar1->txt;
          }
          DVar2 = atoi(local_3c);
          wsCtx->mode = DVar2;
          break;
        case 3:   /* <ChLookupDeviceType> -> wsCtx->channelLookUpMode (atoi) */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_40 = "";
          }
          else {
            local_40 = peVar1->txt;
          }
          DVar2 = atoi(local_40);
          wsCtx->channelLookUpMode = DVar2;
          break;
        case 4:   /* <AntennaHeightAglMeters> -> wsCtx->antennaeHeightInMeters (atoi) */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_44 = "";
          }
          else {
            local_44 = peVar1->txt;
          }
          iVar4 = atoi(local_44);
          wsCtx->antennaeHeightInMeters = iVar4;
          break;
        case 5:   /* <ContactCity> -> addressInfo.contactCity */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_48 = "";
          }
          else {
            local_48 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_48);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactCity);
          break;
        case 6:   /* <ContactCountry> -> addressInfo.contactCountry */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_4c = "";
          }
          else {
            local_4c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_4c);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactCountry);
          break;
        case 7:   /* <ContactEmail> -> addressInfo.contactEmail */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_50 = "";
          }
          else {
            local_50 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_50);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactEmail);
          break;
        case 8:   /* <ContactName> -> addressInfo.contactName */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_54 = "";
          }
          else {
            local_54 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_54);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactName);
          break;
        case 9:   /* <ContactPhone> -> addressInfo.contactPhoneNumber */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_58 = "";
          }
          else {
            local_58 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_58);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactPhoneNumber);
          break;
        case 10:  /* <ContactState> -> addressInfo.contactProvince */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_5c = "";
          }
          else {
            local_5c = peVar1->txt;
          }
          strcpy(acStack_4ec,local_5c);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactProvince);
          break;
        case 0xb: /* <ContactStreet> -> addressInfo.contactStreetAddress */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_60 = "";
          }
          else {
            local_60 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_60);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactStreetAddress);
          break;
        case 0xc: /* <ContactZip> -> addressInfo.contactPostalCode */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_64 = "";
          }
          else {
            local_64 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_64);
          postProcessDataForEscapeSequence(acStack_4ec,(wsCtx->addressInfo).contactPostalCode);
          break;
        case 0xd: /* <CountryCode> -> wsCtx->regulatoryDomain (OVERWRITES case 2) */
          if (peVar1 == (ezxml_t_conflict)0x0) {
            local_68 = "";
          }
          else {
            local_68 = peVar1->txt;
          }
          strcpy(acStack_4ec,local_68);
          postProcessDataForEscapeSequence(acStack_4ec,wsCtx->regulatoryDomain);
          break;
        case 0xe: /* <DeviceOwner> -> addressInfo.ownerName */
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
    case 5:   /* <PreferredChannel> -> preferredChannelMap (comma-split, atoi) */
      peVar5 = ezxml_child(xml,readXmlIntoContext::responseTags[local_4f0]);
      local_488[0] = DAT_00519ea0;                 // the ',' delimiter byte
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
      printPreferredChannelMap(wsCtx);             // (stub, does nothing)
      break;
    case 6:   /* <EnumeratedTransmitPower> -> wsCtx->enumeratedTransmitPower (atoi) */
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
    case 7:   /* <StreamingIpAddress> -> wsCtx->meldStreamingIpAddress (raw strcpy) */
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
    case 8:   /* <ModulationMode> -> wsCtx->meldModulationMode (atoi) */
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
  ezxml_free(xml);                                 // release the parsed tree
  return 0;
}

/* =====================================================================
 * SerialServer.c  --  the radio controller (thread B) over /dev/ttyS1
 * =====================================================================
 *
 * In the command senders below, acStack_1014[4100] is the command/response
 * scratch buffer and the write length is strlen (so the trailing NUL is
 * usually NOT sent, except setPPowerValue which deliberately writes strlen+1).
 * The DAT_0051a0xx constants are the literal command byte strings.
 */

/* open_port(): open /dev/ttyS1 O_RDWR at output speed B115200 (cfsetospeed
 * only). auStack_40 = struct termios. The error text wrongly says ttyS0.
 * Returns the fd (or -1). */
int open_port(void)
{
  int __fd;
  undefined1 auStack_40 [56];        // struct termios
  __fd = open("/dev/ttyS1",2);       // O_RDWR
  if (__fd == -1) {
    perror("open_port: Unable to open /dev/ttyS0 - ");
  }
  else {
    tcgetattr(__fd,(termios *)auStack_40);
    cfsetospeed((termios *)auStack_40,0x1002);    // B115200
    tcsetattr(__fd,0x540e,(termios *)auStack_40); // TCSETSW
    tcflush(__fd,1);                              // TCOFLUSH
    fcntl(__fd,4);                                // F_SETFL
  }
  return __fd;
}

/* flush_port(fd): empty stub - returns an uninitialized register (in_v0).
 * Effectively a no-op; nothing calls it meaningfully. */
int flush_port(int fd)
{
  int in_v0;                         // uninitialized return (decompiler artifact)
  return in_v0;
}

/* packChannelMapIntoBufferAndGetLength(context, buf): format the available
 * channel list as "ch ch ch ..." into buf, returning the length. DEAD CODE -
 * nothing calls it. local_18 = index; acStack_14 = per-number "%d " scratch. */
int packChannelMapIntoBufferAndGetLength(WhiteSpaceContext *context,char *buf)
{
  size_t sVar1;
  uint local_18;                     // channel index
  char acStack_14 [12];              // "%d " scratch
  if ((context->availableChannelMap).channelCount != 0) {
    memset(buf,0,4);
    for (local_18 = 0; local_18 < (context->availableChannelMap).channelCount;
        local_18 = local_18 + 1) {
      sprintf(acStack_14,"%d ",(context->availableChannelMap).channels[local_18]);
      strcat(buf,acStack_14);
    }
    memcpy(acStack_14,&DAT_00519f48,2);           // trailing "\n"
    strcat(buf,acStack_14);
  }
  sVar1 = strlen(buf);
  return sVar1;
}

/* setOperatingChannelFile(chNumber): rewrite /etc/config/operating (UCI) with
 * the current operating channel. Same NULL-FILE* hazard as the other writers. */
void setOperatingChannelFile(int chNumber)
{
  FILE *__s;
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

/* dumpAllSerialReceive(fd): drain and discard whatever the radio is currently
 * sending, using select() with a 100 ms timeout. Used before a query to clear
 * stale bytes. auStack_1094 = 4 KB sink; fStack_94 = fd_set; local_14 = timeval;
 * local_10a0 = a 0..3 rolling counter (only used to pace the 1 s sleeps). */
void dumpAllSerialReceive(int fd)
{
  int iVar1;                         // select() result
  uint local_10a4;                   // fd_set clear index
  int local_10a0;                    // rolling counter
  undefined1 auStack_1094 [4096];    // discard buffer
  fd_set fStack_94;
  timeval local_14;                  // 100 ms timeout
  local_10a0 = 0;
  for (local_10a4 = 0; local_10a4 < 0x20; local_10a4 = local_10a4 + 1) {
    fStack_94.fds_bits[local_10a4] = 0;
  }
  fStack_94.fds_bits[(uint)fd >> 5] = fStack_94.fds_bits[(uint)fd >> 5] | 1 << (fd & 0x1fU);
  local_14.tv_sec = 0;
  local_14.tv_usec = 100000;
  memset(auStack_1094,0,0x1000);
  while( true ) {
    iVar1 = select(fd + 1,&fStack_94,(fd_set *)0x0,(fd_set *)0x0,&local_14);
    if (iVar1 < 1) break;            // nothing left to read
    read(fd,auStack_1094,0x1000);
    if (3 < local_10a0) {
      local_10a0 = 0;
    }
    local_10a0 = local_10a0 + 1;
    sleep(1);
  }
  return;
}

/* getSerialReceive(fd, buf): read all currently available serial bytes into
 * buf (select() + 100 ms). Returns buf. Note each read overwrites the last
 * (buf is not advanced), so only the final chunk survives. */
char * getSerialReceive(int fd,char *buf)
{
  int iVar1;
  uint local_a4;
  fd_set fStack_94;
  timeval local_14;
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

/* setTransmitterFlagFile(txFlag): rewrite /etc/config/transmitter radio_status
 * (1 = on, 0 = off). Called by enableRadio/disableRadio. */
void setTransmitterFlagFile(int txFlag)
{
  FILE *__s;
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

/* enableRadio(fd): send "e1\r" (DAT_0051a0a8) to turn the transmitter on and,
 * on success, set the transmitter flag file to 1. Returns 1 ok / 0 fail. */
int enableRadio(int fd)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  memcpy(acStack_1014,&DAT_0051a0a8,4);           // "e1\r"
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

/* disableRadio(fd): send "e0\r" (DAT_0051a0cc) to turn the transmitter off and,
 * on success, set the transmitter flag file to 0. */
int disableRadio(int fd)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  memcpy(acStack_1014,&DAT_0051a0cc,4);           // "e0\r"
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

/* stripOutExcessBlanks(inBuffer, len, outBuffer): copy inBuffer to outBuffer
 * dropping spaces and newlines; returns the output length. Used to clean the
 * FCC-ID token read from the radio. local_10 = write idx; local_c = read idx. */
int stripOutExcessBlanks(char *inBuffer,int len,char *outBuffer)
{
  int local_10;                      // output index
  int local_c;                       // input index
  local_10 = 0;
  for (local_c = 0; local_c < len; local_c = local_c + 1) {
    if ((inBuffer[local_c] != ' ') && (inBuffer[local_c] != '\n')) {
      outBuffer[local_10] = inBuffer[local_c];
      local_10 = local_10 + 1;
    }
  }
  return local_10;
}

/* recreateFccSerialMeldFile(fp, serialNumberString, fccIdString, addressString):
 * write the three identity lines to the already-open fccidSerialInfo file.
 * param2 = the serial number parsed to int. */
void recreateFccSerialMeldFile
               (FILE *fp,char *serialNumberString,char *fccIdString,char *addressString)
{
  int param2;                        // serial as int
  param2 = atoi(serialNumberString);
  fprintf((FILE *)fp,"Serial Number:%d\n",param2);
  fprintf((FILE *)fp,"FCCID:%s\n",fccIdString);
  fprintf((FILE *)fp,"StreamingIpAddress:%s\n",addressString);
  fflush((FILE *)fp);
  return;
}

/* changeChannelOnRadio(fd, channel): US path - send "c%d\r" and record the
 * operating channel. Returns 1 ok / 0 fail. */
int changeChannelOnRadio(int fd,int channel)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
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

/* changeATSCChannelNumber(fd, channel): send "m#[01%08d]". DEAD CODE - never
 * called (the US path uses changeChannelOnRadio, non-US uses changeNewFrequency). */
int changeATSCChannelNumber(int fd,int channel)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
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

/* changeNewPowerLevel(fd, power): non-US power command "m#[02%08d]". */
int changeNewPowerLevel(int fd,int power)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  sprintf(acStack_1014,"m#[02%08d]",power);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}

/* changeNewModulation(fd, modulation): modulation command "m#[03%08d]". */
int changeNewModulation(int fd,int modulation)
{
  size_t __n;
  ssize_t sVar1;
  char acStack_1014 [4100];
  uint local_10;
  sprintf(acStack_1014,"m#[03%08d]",modulation);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (-1 >= sVar1) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar1);
  return local_10;
}

/* changeNewFrequency(fd, channelNumber): non-US path - look up the carrier
 * frequency for the channel and send "m#[04%08d]". param2 = frequency in
 * 100-Hz units (or -1 for an out-of-range channel, which is rejected). */
int changeNewFrequency(int fd,int channelNumber)
{
  int param2;                        // frequency (100-Hz units)
  size_t __n;
  ssize_t sVar1;
  char acStack_1010 [4096];
  int local_10;
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

/* convertIpOctetsToMeldHexFormatIpStringAndSize(ipAddress, returnString):
 * "a.b.c.d" -> 8 hex digits "%02x%02x%02x%02x" in returnString; returns its
 * length. local_18/14/10/c = the four parsed octets. */
int convertIpOctetsToMeldHexFormatIpStringAndSize(char *ipAddress,char *returnString)
{
  size_t sVar1;
  uint local_18;                     // octet 0
  uint local_14;                     // octet 1
  uint local_10;                     // octet 2
  uint local_c;                      // octet 3
  if (*ipAddress != '\0') {
    sscanf(ipAddress,"%d.%d.%d.%d",(int *)&local_18,(int *)&local_14,(int *)&local_10,
           (int *)&local_c);
    sprintf(returnString,"%02x%02x%02x%02x",local_18,local_14,local_10,local_c);
  }
  sVar1 = strlen(returnString);
  return sVar1;
}

/* isChannelInAvailableChannelMap(context, ch): returns 1 if `ch` is in the
 * available map AND is not one of 35..38 (0x23..0x26), else 0. local_18 = idx. */
int isChannelInAvailableChannelMap(WhiteSpaceContext *context,int channelNumber)
{
  uint local_18;                     // index
  if ((context->availableChannelMap).channelCount != 0) {
    for (local_18 = 0; local_18 < (context->availableChannelMap).channelCount;
        local_18 = local_18 + 1) {
      if (channelNumber == (context->availableChannelMap).channels[local_18]) {
        if ((((channelNumber == 0x23) || (channelNumber == 0x24)) || (channelNumber == 0x25)) ||
           (channelNumber == 0x26)) {
          return 0;                  // 35..38 explicitly excluded
        }
        return 1;
      }
    }
  }
  return 0;
}

/* getChannelToOperateOnFrom(context): choose the channel the radio should use.
 * Returns 0 if the last channel request did not succeed. Otherwise prefers the
 * first PreferredChannel that is 21..51, present in the available map, and not
 * 35..38; failing that, the first available channel in 21..51 (this fallback
 * does NOT exclude 35..38).
 *   local_28 = fallback choice   local_24 = preferred idx   local_20 = avail idx
 *   local_10 = return value */
int getChannelToOperateOnFrom(WhiteSpaceContext *context)
{
  int iVar1;                         // isChannelInAvailableChannelMap result
  uint local_28;                     // fallback channel
  uint local_24;                     // preferred-list index
  uint local_20;                     // available-list index
  uint local_10;                     // return
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
          return (context->preferredChannelMap).channels[local_24];     // preferred wins
        }
      }
    }
    if ((context->availableChannelMap).channelCount != 0) {
      for (local_20 = 0; local_20 < (context->availableChannelMap).channelCount;
          local_20 = local_20 + 1) {
        if ((0x14 < (context->availableChannelMap).channels[local_20]) &&
           ((context->availableChannelMap).channels[local_20] < 0x34)) {
          local_28 = (context->availableChannelMap).channels[local_20];  // first available 21..51
          break;
        }
      }
    }
    local_10 = local_28;
  }
  else {
    local_10 = 0;                    // no successful channel request -> no channel
  }
  return local_10;
}

/* setPPowerValue(fd, val): send the "pNN\r" power command for power index
 * val (0..10, else a default). The DAT_0051a0xx constants are the literal
 * "p00\r".."p32\r"/"p35\r" strings. acStack_1014 = command buffer.
 * NOTE: the write length is strlen+1, so the trailing NUL IS sent here. */
int setPPowerValue(int fd,int val)
{
  size_t sVar1;                      // command length
  ssize_t sVar2;                     // write result
  char acStack_1014 [4100];          // command buffer
  uint local_10;                     // return (1 ok / 0 fail)
  switch(val) {
  case 0:  memcpy(acStack_1014,&DAT_0051a000,5); break;   // "p00\r"
  case 1:  memcpy(acStack_1014,&DAT_0051a008,5); break;   // "p05\r"
  case 2:  memcpy(acStack_1014,&DAT_0051a010,5); break;   // "p0a\r"
  case 3:  memcpy(acStack_1014,&DAT_0051a018,5); break;   // "p0f\r"
  case 4:  memcpy(acStack_1014,&DAT_0051a020,5); break;   // "p14\r"
  case 5:  memcpy(acStack_1014,&DAT_0051a028,5); break;   // "p19\r"
  case 6:  memcpy(acStack_1014,&DAT_0051a030,5); break;   // "p1e\r"
  case 7:  memcpy(acStack_1014,&DAT_0051a038,5); break;   // "p23\r"
  case 8:  memcpy(acStack_1014,&DAT_0051a040,5); break;   // "p28\r"
  case 9:  memcpy(acStack_1014,&DAT_0051a048,5); break;   // "p2d\r"
  case 10: memcpy(acStack_1014,&DAT_0051a050,5); break;   // "p32\r" (default power)
  default: memcpy(acStack_1014,&DAT_0051a058,5);          // "p35\r"
  }
  sVar1 = strlen(acStack_1014);
  sVar2 = write(fd,acStack_1014,sVar1 + 1);               // +1: send trailing NUL
  if (-1 >= sVar2) {
    fwrite("write() powerLevel failed!\n",1,0x1b,(FILE *)stderr);
  }
  local_10 = (uint)(-1 < sVar2);
  return local_10;
}

/* convertChannelNumberToFrequency(ch): return the carrier frequency (in units
 * of 100 Hz, i.e. value/10000 = MHz) for the non-US raster, or -1 for channels
 * outside 25..50. Ghidra shows the per-case constants as pointer values; the
 * real returns (decimal, 100-Hz units) are noted inline. The 0x1e/0x27 cases
 * reference labeled data (tag2bit+0x44, PTR_...) but hold the same literal. */
int convertChannelNumberToFrequency(int channelNumber)
{
  undefined **local_18;              // frequency value (100-Hz units)
  switch(channelNumber) {
  case 0x19: local_18 = (undefined **)0x4d35a0; break;    // ch25 = 506.0 MHz
  case 0x1a: local_18 = (undefined **)0x4e6e20; break;    // ch26 = 514.0
  case 0x1b: local_18 = (undefined **)0x4fa6a0; break;    // ch27 = 522.0
  case 0x1c: local_18 = (undefined **)0x50df20; break;    // ch28 = 530.0
  case 0x1d: local_18 = (undefined **)0x5217a0; break;    // ch29 = 538.0
  case 0x1e: local_18 = (undefined **)(tag2bit + 0x44); break; // ch30 = 546.0
  case 0x1f: local_18 = (undefined **)0x5488a0; break;    // ch31 = 554.0
  case 0x20: local_18 = (undefined **)&DAT_0055c120; break; // ch32 = 562.0
  case 0x21: local_18 = (undefined **)&DAT_0056f9a0; break; // ch33 = 570.0
  case 0x22: local_18 = (undefined **)&DAT_00583220; break; // ch34 = 578.0
  case 0x23: local_18 = (undefined **)&DAT_0055c120; break; // ch35 = 562.0 (== ch32!)
  case 0x24: local_18 = (undefined **)&DAT_0056f9a0; break; // ch36 = 570.0 (== ch33!)
  case 0x25: local_18 = (undefined **)&DAT_00583220; break; // ch37 = 578.0 (== ch34!)
  case 0x26: local_18 = (undefined **)0x596aa0; break;    // ch38 = 586.0
  case 0x27: local_18 = &PTR_gethostbyname_r_005aa320; break; // ch39 = 594.0
  case 0x28: local_18 = (undefined **)0x5f8520; break;    // ch40 = 626.0
  case 0x29: local_18 = (undefined **)0x60bda0; break;    // ch41 = 634.0
  case 0x2a: local_18 = (undefined **)0x61f620; break;    // ch42 = 642.0
  case 0x2b: local_18 = (undefined **)0x632ea0; break;    // ch43 = 650.0
  case 0x2c: local_18 = (undefined **)0x646720; break;    // ch44 = 658.0
  case 0x2d: local_18 = (undefined **)0x659fa0; break;    // ch45 = 666.0
  case 0x2e: local_18 = (undefined **)0x66d820; break;    // ch46 = 674.0
  case 0x2f: local_18 = (undefined **)0x6810a0; break;    // ch47 = 682.0
  case 0x30: local_18 = (undefined **)0x694920; break;    // ch48 = 690.0
  case 0x31: local_18 = (undefined **)0x6a81a0; break;    // ch49 = 698.0
  case 0x32: local_18 = (undefined **)0x6bba20; break;    // ch50 = 706.0
  default:   local_18 = (undefined **)0xffffffff;         // -1 (invalid)
  }
  return (int)local_18;
}

/* validateAndLoadFieldType(wsCtx, fp, buffer, length, fType, retString):
 * parse one field out of a radio reply `buffer` and store it.
 *   fType SERIAL : find "S/N = ", atoi it, store serialNumber ("%d"), log it
 *   fType FCCID  : find "FCC ID = ", strip blanks, store fccID, log it
 *   fType STREAMING_IP : read 4 hex byte pairs from offset 3, format a.b.c.d
 * local_6c = cursor to the value; local_28[4] = the 4 IP octets; acStack_68 =
 * the blank-stripped FCC id; local_2c/2b = a 2-char window for hex parse. */
int validateAndLoadFieldType
              (WhiteSpaceContext *wsCtx,FILE *fp,char *buffer,int length,FieldType fType,
              char *retString)
{
  size_t len;
  int iVar1;                         // atoi result
  int local_70;                      // IP octet index
  char *local_6c;                    // value cursor
  char acStack_68 [60];              // stripped FCC id
  char local_2c;                     // hex pair hi
  char local_2b;                     // hex pair lo
  uint local_28 [4];                 // IP octets
  FieldType local_18;
  local_70 = 0;
  local_18 = fType;
  if (fType == SERIAL) {
    local_6c = strstr(buffer,"S/N = ");
    if (local_6c != (char *)0x0) { local_6c = local_6c + 6; }
    if (local_6c != (char *)0x0) {
      iVar1 = atoi(local_6c);
      sprintf(wsCtx->serialNumber,"%d",iVar1);              // store serial
      iVar1 = atoi(local_6c);
      fprintf((FILE *)fp,"Serial Number:%d\n",iVar1);
      strcpy(retString,local_6c);
    }
  }
  else if (fType == FCCID) {
    local_6c = strstr(buffer,"FCC ID = ");
    if (local_6c != (char *)0x0) { local_6c = local_6c + 9; }
    if (local_6c != (char *)0x0) {
      memset(acStack_68,0,0x3c);
      len = strlen(local_6c);
      stripOutExcessBlanks(local_6c,len,acStack_68);
      strcpy(wsCtx->fccID,acStack_68);                      // store FCC id
      fprintf((FILE *)fp,"FCCID:%s\n",acStack_68);
      strcpy(retString,acStack_68);
    }
  }
  else if (fType == STREAMING_IP) {
    local_6c = buffer;
    if (buffer != (char *)0x0) { local_6c = buffer + 3; }   // skip "b#[" style prefix
    for (; (local_6c != (char *)0x0 && (local_70 < 4)); local_70 = local_70 + 1) {
      local_2c = *local_6c;
      local_2b = local_6c[1];
      local_6c = local_6c + 2;
      sscanf(&local_2c,"%x",local_28 + local_70);           // one hex byte -> octet
    }
    sprintf(retString,"%d.%d.%d.%d",local_28[0],local_28[1],local_28[2],local_28[3]);
    fprintf((FILE *)fp,"StreamingIpAddress:%s\n",retString);
  }
  return 1;
}

/* The five radio-query functions below share one shape: drain stale input
 * (dumpAllSerialReceive), write a one-shot query command, then select()-loop
 * (100 ms timeout) reading replies into a ~4 KB buffer. Only the command byte
 * and what they do with the reply differ. In each: acStack_10xx = reply buffer,
 * fStack_xx = select() fd_set, local_1x = the 100 ms timeval, local_10xx =
 * fd_set clear index, and the DAT_0051a2xx constant = the command bytes. */

/* queryInformationFromRadio(fd): cmd "h" (DAT_0051a15c). Copies each reply
 * chunk to /etc/config/queryInfo until a read begins with "EOF" or input stops.
 * __stream = the queryInfo file. Returns 1 ok / 0 on write failure. */
int queryInformationFromRadio(int fd)
{
  FILE *__stream;                    // /etc/config/queryInfo
  size_t __n;
  ssize_t sVar1;
  int iVar2;                         // select() result
  uint local_10ac;                   // fd_set clear index
  fd_set fStack_109c;
  timeval local_101c;                // 100 ms timeout
  char acStack_1014 [4100];          // command / reply buffer
  int local_10;                      // return
  memcpy(acStack_1014,&DAT_0051a15c,2);           // "h"
  __stream = fopen("/etc/config/queryInfo","w+");
  dumpAllSerialReceive(fd);
  __n = strlen(acStack_1014);
  sVar1 = write(fd,acStack_1014,__n);
  if (sVar1 < 0) {
    fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
    local_10 = 0;
  }
  else {
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
      iVar2 = strncmp(acStack_1014,"EOF",3);       // radio marks end with "EOF"
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

/* processMeldVersionNumber(fp, buf): find "Firmware version = " in buf and write
 * the text after it (offset +0x13) to fp as "NXP Version:%s". local_10 = cursor. */
void processMeldVersionNumber(FILE *fp,char *buf)
{
  char *local_10;                    // -> version text
  local_10 = strstr(buf,"Firmware version = ");
  if (local_10 != (char *)0x0) {
    local_10 = local_10 + 0x13;                   // skip the label
  }
  fprintf((FILE *)fp,"NXP Version:%s\n",local_10);
  fflush((FILE *)fp);
  return;
}

/* getMeldVersionAndOtherInfo(fd, ifp): cmd "l\r" (DAT_0051a248). Feeds each
 * reply chunk to processMeldVersionNumber (which extracts the firmware line). */
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
  dumpAllSerialReceive(fd);
  memcpy(acStack_1098,&DAT_0051a248,3);           // "l\r"
  __n = strlen(acStack_1098);
  sVar1 = write(fd,acStack_1098,__n);
  if (sVar1 < 0) {
    fwrite("write() of 11 bytes for MeldVersionAndOtherInfo !\n",1,0x32,(FILE *)stderr);
    local_10 = 0;
  }
  else {
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

/* getMeldSerialNumber(fd, ifp, wsCtx): cmd "x" (DAT_0051a2a0). The final reply
 * goes to validateAndLoadFieldType(SERIAL), which stores wsCtx->serialNumber.
 * acStack_a8 = retString scratch (the parsed value). */
int getMeldSerialNumber(int fd,FILE *ifp,WhiteSpaceContext *wsCtx)
{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_10b4;
  char acStack_10a8 [4096];          // reply buffer
  char acStack_a8 [20];              // parsed-value scratch
  fd_set fStack_94;
  timeval local_14;
  dumpAllSerialReceive(fd);
  memcpy(acStack_10a8,&DAT_0051a2a0,2);           // "x"
  __n = strlen(acStack_10a8);
  sVar1 = write(fd,acStack_10a8,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
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
    if (-1 < sVar1) break;                        // got a reply
    fwrite("Read Serial Number failed!\n",1,0x1b,(FILE *)stderr);
  }
  if (ifp != (FILE *)0x0) {
    validateAndLoadFieldType(wsCtx,ifp,acStack_10a8,0x1000,SERIAL,acStack_a8);
    fflush((FILE *)ifp);
  }
  return 1;
}

/* getMeldFccIdInfo(fd, ifp, wsCtx): cmd "y" (DAT_0051a2d4). The reply goes to
 * validateAndLoadFieldType(FCCID), storing wsCtx->fccID. Shape matches the
 * serial-number reader above. */
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
  dumpAllSerialReceive(fd);
  memcpy(acStack_10a8,&DAT_0051a2d4,2);           // "y"
  __n = strlen(acStack_10a8);
  sVar1 = write(fd,acStack_10a8,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
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

/* getStreamingIpAddress(fd, ifp, wsCtx, buffer): cmd "b?" (DAT_0051a2ec). The
 * reply goes to validateAndLoadFieldType(STREAMING_IP), which writes the dotted
 * quad into `buffer` (used by the caller to compare/set the streaming IP). */
int getStreamingIpAddress(int fd,FILE *ifp,WhiteSpaceContext *wsCtx,char *buffer)
{
  size_t __n;
  ssize_t sVar1;
  int iVar2;
  uint local_109c;
  char acStack_1090 [4096];
  fd_set fStack_90;
  timeval local_10;
  dumpAllSerialReceive(fd);
  memcpy(acStack_1090,&DAT_0051a2ec,3);           // "b?"
  __n = strlen(acStack_1090);
  sVar1 = write(fd,acStack_1090,__n);
  if (sVar1 < 0) {
    fwrite("write() failed!\n",1,0x10,(FILE *)stderr);
  }
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

/* setStreamingIpAddress(fd, wsCtx, ipAddressString): if the radio's current
 * streaming IP already matches wsCtx->meldStreamingIpAddress, do nothing (return
 * 0); otherwise hex-encode the IP and send "b#[01<hex>]\r". acStack_2c = the
 * 8-hex-digit string; local_18 = return (1 on write success). */
int setStreamingIpAddress(int fd,WhiteSpaceContext *wsCtx,char *ipAddressString)
{
  size_t sVar1;
  int iVar2;
  ssize_t sVar3;
  char acStack_102c [4096];          // command buffer
  char acStack_2c [20];              // hex IP string
  int local_18;                      // return
  memset(acStack_2c,0,0x14);
  if (wsCtx->meldStreamingIpAddress[0] != '\0') {
    sVar1 = strlen(ipAddressString);
    iVar2 = strncmp(wsCtx->meldStreamingIpAddress,ipAddressString,sVar1);
    if (iVar2 == 0) {
      return 0;                                    // already set, nothing to do
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

/* serialProcessorThread(arg): thread B entry point. arg -> whitespaceThreadData;
 * wsCtx is read from offset +8. Does a one-time radio bring-up, signals thread A,
 * then loops every ~15 s choosing and asserting the operating channel.
 *
 * Locals:
 *   wsCtx       = the shared context (*(arg+8))
 *   local_1048  = the /dev/ttyS1 fd
 *   local_1088  = FILE* for /etc/config/fccidSerialInfo
 *   bVar1       = "radio has been initialized once" latch
 *   bVar2       = "transmitter is currently ON" flag
 *   local_1080  = last channel we set (to detect changes)
 *   local_1084  = seconds accumulator for the ~23 h ( >0x14370 = 82800 s )
 *                 periodic re-assert
 *   acStack_1044 = serial command scratch; acStack_44 = streaming-IP string
 */
void * serialProcessorThread(void *arg)
{
  bool bVar1;                        // initialized-once latch
  bool bVar2;                        // transmitter-on flag
  WhiteSpaceContext *wsCtx;
  size_t __n;                        // command length
  ssize_t sVar3;                     // write result
  int iVar4;                         // chosen channel / strcmp
  int iVar5;                         // strcmp(regDomain,"US")
  FILE *local_1088;                  // fccidSerialInfo file
  int local_1084;                    // seconds accumulator
  int local_1080;                    // last channel set
  int local_1048;                    // serial fd
  char acStack_1044 [4096];          // command scratch
  char acStack_44 [60];              // streaming IP string
  wsCtx = *(WhiteSpaceContext **)((int)arg + 8);
  bVar1 = false;
  bVar2 = false;
  local_1080 = 0;
  local_1084 = 0;
  local_1088 = fopen("/etc/config/fccidSerialInfo","w+");
  local_1048 = open_port();                        // /dev/ttyS1
  do {
    if (local_1048 == -1) {
      local_1048 = open_port();                    // keep retrying the open
    }
    else {
      if (!bVar1) {                                // ---- one-time radio bring-up ----
        memcpy(acStack_1044,"MELD_TECH_WS_ENABLE\r",0x15);
        __n = strlen(acStack_1044);
        sVar3 = write(local_1048,acStack_1044,__n);
        if (sVar3 < 0) {
          fwrite("write() of 11 bytes failed!\n",1,0x1c,(FILE *)stderr);
        }
        usleep(500000);
        queryInformationFromRadio(local_1048);     // "h"
        usleep(500000);
        disableRadio(local_1048);                  // "e0\r" (tx off)
        usleep(500000);
        bVar2 = false;
        getMeldVersionAndOtherInfo(local_1048,(FILE *)local_1088);   // "l\r"
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getMeldSerialNumber(local_1048,(FILE *)local_1088,wsCtx);    // "x" -> serialNumber
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getMeldFccIdInfo(local_1048,(FILE *)local_1088,wsCtx);       // "y" -> fccID
        usleep(500000);
        dumpAllSerialReceive(local_1048);
        usleep(500000);
        getStreamingIpAddress(local_1048,(FILE *)local_1088,wsCtx,acStack_44); // "b?"
        usleep(500000);
        iVar4 = setStreamingIpAddress(local_1048,wsCtx,acStack_44);  // "b#[01..]" if changed
        if (iVar4 != 0) {
          fclose(local_1088);
          local_1088 = fopen("/etc/config/fccidSerialInfo","w+");
          recreateFccSerialMeldFile
                    ((FILE *)local_1088,wsCtx->serialNumber,wsCtx->fccID,
                     wsCtx->meldStreamingIpAddress);
        }
        usleep(500000);
        if (wsCtx != (WhiteSpaceContext *)0x0) {   // initial power + modulation
          iVar4 = strcmp(wsCtx->regulatoryDomain,"US");
          if (iVar4 == 0) {                        // US path
            setPPowerValue(local_1048,wsCtx->enumeratedTransmitPower);   // "pNN\r"
            usleep(500000);
            changeNewModulation(local_1048,wsCtx->meldModulationMode);   // "m#[03..]"
            usleep(500000);
          }
          else {                                   // non-US path
            changeNewPowerLevel(local_1048,wsCtx->enumeratedTransmitPower); // "m#[02..]"
            usleep(500000);
            changeNewModulation(local_1048,wsCtx->meldModulationMode);
            usleep(500000);
          }
        }
        bVar1 = true;                              // bring-up done
        if ((wsCtx->fccID[0] != '\0') && (wsCtx->serialNumber[0] != '\0')) {
          signalAgentToProcess();                  // wake thread A (only if ids known)
        }
      }
      if (wsCtx != (WhiteSpaceContext *)0x0) {     // ---- periodic channel control ----
        iVar4 = getChannelToOperateOnFrom(wsCtx);  // 0 if no usable channel
        usleep(500000);
        if ((iVar4 == 0) && (local_1084 < 0x14371)) {        // no channel yet
          if ((bVar2) || (0x14370 < local_1084)) {           // if tx on, turn it off
            disableRadio(local_1048);
            usleep(500000);
            bVar2 = false;
            queryInformationFromRadio(local_1048);
            usleep(500000);
          }
        }
        else if ((local_1080 != iVar4) || (0x14370 < local_1084)) {  // new channel or 23h timer
          iVar5 = strcmp(wsCtx->regulatoryDomain,"US");
          if (iVar5 == 0) {
            changeChannelOnRadio(local_1048,iVar4);          // "c<ch>\r" (US)
            usleep(500000);
          }
          else {
            changeNewFrequency(local_1048,iVar4);            // "m#[04<freq>]" (non-US)
            usleep(500000);
          }
          if ((!bVar2) || (0x14370 < local_1084)) {          // ensure tx on
            enableRadio(local_1048);                         // "e1\r"
            usleep(500000);
            bVar2 = true;
          }
          queryInformationFromRadio(local_1048);
          usleep(500000);
          local_1084 = 0;                                    // reset the 23h timer
          setPPowerValue(local_1048,wsCtx->enumeratedTransmitPower);  // re-assert power
          usleep(500000);
          local_1080 = iVar4;                                // remember this channel
        }
      }
    }
    fflush((FILE *)stdout);
    tcflush(local_1048,1);                         // TCOFLUSH
    sleep(0xf);                                    // 15 s loop period
    local_1084 = local_1084 + 0xf;
  } while( true );                                 // never returns
}
