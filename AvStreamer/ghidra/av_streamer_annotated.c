/*
 * av_streamer_annotated.c  --  fully annotated decompilation of av_streamer
 * =========================================================================
 *
 * Combined from AvStreamer/ghidra/annotated/*.c (19 files, one per DWARF
 * compilation unit + CRT stubs). Every file was verified with
 * scripts/check_annotated.py against its raw Ghidra split: the C code is
 * identical to Ghidra's output (only comments and whitespace differ).
 *
 * Binary: /disk/Meld/AV30/test_script/av_streamer (v2.14, Oct 9 2015,
 *         md5 65023c5fd5c3f99af36c8057e1acc75d, MIPS32r2-LE, DWARF intact).
 */


/* #####################################################################
 * FILE: 00_main.c.c
 * ##################################################################### */

/*
 * 00_main.c.c  --  annotated Ghidra decompilation of main.c (av_streamer)
 * =========================================================================
 *
 * Original source file: main.c (compiled as C++ / GNU C 4.5.3, ELF32 MIPS32r2 LE)
 * Build dir reported by print_version():
 *   /media/WD_passport/Meld_AV_Script/2p14_a/avstreamer/src/avstreamer
 *
 * Role in av_streamer:
 *   Application entry point, CLI option parsing (getopt), signal handling
 *   (SIGTERM), ADV7842 video front-end initialization and hotplug/resolution/
 *   signal-loss/HDCP event state machine, and ViXS XCode4 encoder lifecycle
 *   management (open/start/update/stop/close) plus interactive 'q'/'g' console
 *   or background SIGTERM wait loop.
 *
 * Main data structures (from DWARF):
 *   - ADV7842_init_param_t (12 bytes): callback data, event_callback, edid_file_name.
 *   - ADV7842_event_t (12 bytes): { int eventID; ADV7842_input_type_t input; int event_info; }.
 *   - ADV7842_input_info_t (8 bytes): { ADV7842_input_type_t input; video_std_t video_std; }.
 *   - vixs_encoder_init_param_t (192 / 0xc0 bytes): video stds, bitRate, rmxRate,
 *     pcrPid, audioIf, outMode, dvoPort, dstAddr, dstPort, dstTTL, captureFileName[128],
 *     data_insertion_param { data, data_size, interval }, flag, videoCodec.
 *   - vixs_encoder_update_param_t (16 bytes): { flag, newBitrate, newInVideoStandard,
 *     newOutVideoStandard }.
 *   - vixs_encoder_ts_info_t (36 bytes): lock/CC-error/output counters queried by 'g'.
 *   - fileconf (t_conf, 68 bytes): parsed from /etc/config/queryInfo by readCfgFile().
 *
 * Globals owned by this unit (addresses from DWARF):
 *   - SigTerm        (0x004376a0, volatile int): set to 1 by term() on SIGTERM.
 *   - adv7842_handle (0x004376a4, void*): opaque handle from adv7842_init().
 *   - curStd         (0x004376a8, video_std_t): currently active video standard.
 *   - CurInputType   (0x004376ac, ADV7842_input_type_t): active input (0=NONE,1=CVBS,2=COMP,4=HDMI).
 *   - CurValidInput  (0x004376b0, uint): bitmask of inputs with valid signal (bit0=CVBS, bit1=COMP, bit2=HDMI).
 *   - encoderHandle  (0x004376b8, int): handle from xcode4_open_encoder().
 *   - encoder_param  (0x004376c0, vixs_encoder_init_param_t): global encoder config (192 bytes).
 *   - scaler_o480i   (0x00437780, int): 1 if "-o 480i" output scaling is enabled.
 *   - g_llc_delay    (0x00436160, int): ADV7842 LLC clock phase delay set by hidden "-l" option.
 */


/*
 * term @ 00402490
 *
 * Purpose:
 *   POSIX signal handler for SIGTERM (signal 15 / 0xf), registered via sigaction()
 *   in main(). Sets the global volatile flag SigTerm = 1 to exit the background
 *   wait loop.
 *
 * Parameters:
 *   signum - signal number delivered by the kernel (unused).
 *
 * Return value: void.
 * Callers: kernel signal delivery (registered in main()).
 * Side effects: writes global SigTerm = 1.
 */
// ==== term @ 00402490 ====

void term(int signum)

{
  SigTerm = 1;
  return;
}



/*
 * string2netaddr @ 004024c0
 *
 * Purpose:
 *   Convert a dotted-quad IPv4 string or hostname into a host-byte-order
 *   32-bit IPv4 address for encoder_param.dstAddr (UDP output mode).
 *
 * Parameters:
 *   strAddr - NUL-terminated IPv4 string (e.g. "192.168.1.193") or hostname.
 *
 * Return value:
 *   Host-byte-order 32-bit IPv4 address on success, or -1 (0xffffffff) if
 *   inet_addr() fails and gethostbyname() returns NULL.
 *
 * Callers: main().
 * Callees: inet_addr(), gethostbyname(), ntohl().
 *
 * Locals:
 *   local_10 - network-byte-order 32-bit IPv4 address ("int addr").
 *   phVar1   - struct hostent* returned by gethostbyname() ("hostent * he").
 *   uVar2    - host-byte-order result from ntohl(local_10).
 */
// ==== string2netaddr @ 004024c0 ====

int string2netaddr(char *strAddr)

{
  hostent *phVar1;
  uint32_t uVar2;
  uint32_t local_10;
  
                    /* Unresolved local var: hostent * he@[???]
                       Unresolved local var: int addr@[???] */
  local_10 = inet_addr(strAddr);
  if (local_10 == 0xffffffff) {
    phVar1 = gethostbyname(strAddr);
    if (phVar1 == (hostent *)0x0) {
      return -1;
    }
    local_10 = *(uint32_t *)*phVar1->h_addr_list;
  }
  uVar2 = ntohl(local_10);
  return uVar2;
}



/*
 * start_encoder @ 0040258c
 *
 * Purpose:
 *   Configure audio interface and input/output video standards, then either
 *   open a new XCode4 encoder session (restart == 0) or update an existing one
 *   (restart != 0), and start encoding.
 *
 * Parameters:
 *   std     - detected input video_std_t (0=NONE, 1=480I, 2=576I, 3=480P,
 *             4=576P, 5=720P_60HZ, 6=720P_50HZ, 7=1080I_60HZ, 8=1080I_50HZ,
 *             9=1080P_60HZ).
 *   restart - 0 on initial startup (calls xcode4_open_encoder); non-zero when
 *             reconfiguring on hotplug/resolution change (calls
 *             xcode4_update_encoder_param).
 *
 * Return value:
 *   return_code_t from xcode4_start_encoder() (RET_OK=0, RET_ERR=1).
 *
 * Callers: main(), prepare_input_detect(), sample_event_callback().
 * Callees: adv7842_detect_audio_mode(), xcode4_open_encoder(),
 *          xcode4_update_encoder_param(), xcode4_start_encoder(), puts().
 *
 * Side effects:
 *   Updates global encoder_param.audioIf (0=PCM, 1=compressed),
 *   inVideoStandard, outVideoStandard, and encoderHandle (when restart == 0).
 *   NOTE: on restart != 0, updates local_1c (flag=6 -> update in/out video
 *   standards) but still mutates encoder_param.audioIf without a corresponding
 *   update flag in local_1c.
 *
 * Locals:
 *   local_c  - audio_mode_t audioMode (0=AUDIO_MODE_PCM, 1=AUDIO_MODE_COMPRESSED).
 *   local_1c - vixs_encoder_update_param_t update_param; flag=6 updates
 *              newInVideoStandard (bit 1) and newOutVideoStandard (bit 2).
 *   rVar1    - return_code_t rc from xcode4_start_encoder().
 */
// ==== start_encoder @ 0040258c ====

int start_encoder(video_std_t std,int restart)

{
  return_code_t rVar1;
  vixs_encoder_update_param_t local_1c;
  audio_mode_t local_c;
  
                    /* Unresolved local var: int rc@[???]
                       Unresolved local var: vixs_encoder_update_param_t update_param@[???]
                       Unresolved local var: audio_mode_t audioMode@[???] */
  local_c = AUDIO_MODE_PCM;
  local_1c.flag = 6;                            // bit 1 (newInVideoStandard) | bit 2 (newOutVideoStandard)
  encoder_param.audioIf = 0;                    // default to PCM
  adv7842_detect_audio_mode(adv7842_handle,&local_c);
  if (local_c == AUDIO_MODE_COMPRESSED) {
    encoder_param.audioIf = 1;                  // compressed (AC-3/passthrough) audio
  }
  if (restart == 0) {
    encoder_param.inVideoStandard = std;
    if (scaler_o480i == 1) {
      puts("Video output scaler set to 480i");
      encoder_param.outVideoStandard = VIDEO_STD_480I;
    }
    else {
      encoder_param.outVideoStandard = std;
    }
    xcode4_open_encoder(&encoderHandle,&encoder_param);
  }
  else {
    local_1c.newInVideoStandard = std;
    if (scaler_o480i == 1) {
      puts("Video output scaler set to 480i");
      local_1c.newOutVideoStandard = VIDEO_STD_480I;
    }
    else {
      local_1c.newOutVideoStandard = std;
    }
    xcode4_update_encoder_param(encoderHandle,&local_1c);
  }
  rVar1 = xcode4_start_encoder(encoderHandle);
  return rVar1;
}



/*
 * prepare_input_detect @ 00402770
 *
 * Purpose:
 *   Fallback when no active input signal is locked: switches the ADV7842 to
 *   COMPONENT (YPbPr) input to probe for a signal. If none is found
 *   (VIDEO_STD_NONE), starts the encoder anyway at 480i (freerun/blue screen)
 *   and subscribes to HDMI HPD (1) + INPUT_DETECTED (8) events (mask 9).
 *
 * Parameters:
 *   restart - passed through to start_encoder(VIDEO_STD_480I, restart).
 *
 * Return value:
 *   Detected video_std_t on COMPONENT input (VIDEO_STD_NONE if no signal).
 *
 * Callers: main(), sample_event_callback() (cases 1 and 4).
 * Callees: adv7842_program_input(), start_encoder(), adv7842_subscribe_event().
 *
 * Side effects:
 *   Resets curStd = VIDEO_STD_480I and CurInputType = ADV7842_INPUT_TYPE_NONE.
 *   If no component signal is present, starts the encoder in 480i and subscribes
 *   to event mask 9 (ADV7842_EVT_HDMI_HPD | ADV7842_EVT_INPUT_DETECTED).
 *
 * Locals:
 *   local_10[2] - video_std_t std (local_10[0] receives detected standard).
 */
// ==== prepare_input_detect @ 00402770 ====

video_std_t prepare_input_detect(int restart)

{
  video_std_t local_10 [2];
  
                    /* Unresolved local var: video_std_t std@[???] */
  curStd = VIDEO_STD_480I;
  local_10[0] = VIDEO_STD_480I;
  CurInputType = ADV7842_INPUT_TYPE_NONE;
  adv7842_program_input(adv7842_handle,ADV7842_INPUT_TYPE_COMPONENT,local_10);
  if (local_10[0] == VIDEO_STD_NONE) {
    start_encoder(VIDEO_STD_480I,restart);
    adv7842_subscribe_event(adv7842_handle,9);  // 0x1 (HDMI_HPD) | 0x8 (INPUT_DETECTED)
  }
  return local_10[0];
}



/*
 * detect_input_source @ 0040283c
 *
 * Purpose:
 *   Enumerate all 3 ADV7842 inputs (HDMI, Component, CVBS), update the global
 *   CurValidInput bitmask, and select the highest-priority input that has a
 *   valid video standard.
 *   Priority order: HDMI (4) > Component (2) > CVBS (1).
 *
 * Parameters: none.
 * Return value:
 *   Selected ADV7842_input_type_t (NONE=0, CVBS=1, COMPONENT=2, HDMI=4).
 *
 * Callers: main().
 * Callees: adv7842_enum_input(), printf(), puts().
 *
 * Side effects:
 *   Sets bits in global CurValidInput: bit 0 (1)=CVBS, bit 1 (2)=Component,
 *   bit 2 (4)=HDMI.
 *
 * Locals:
 *   local_10[2]  - uint32_t inputCount (initialized to 3, updated by enum_input).
 *   aAStack_28   - ADV7842_input_info_t inputInfo[3] (8 bytes each).
 *   local_2c     - uint32_t loop index i.
 *   local_30     - ADV7842_input_type_t ActiveInputType (best input found so far).
 *   AVar1        - temporary aAStack_28[i].input.
 */
// ==== detect_input_source @ 0040283c ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

ADV7842_input_type_t detect_input_source(void)

{
  ADV7842_input_type_t AVar1;
  ADV7842_input_type_t local_30;
  uint local_2c;
  ADV7842_input_info_t aAStack_28 [3];
  uint local_10 [2];
  
                    /* Unresolved local var: ADV7842_input_type_t ActiveInputType@[???]
                       Unresolved local var: ADV7842_input_info[3] inputInfo@[???]
                       Unresolved local var: uint32_t inputCount@[???]
                       Unresolved local var: uint32_t i@[???] */
  local_10[0] = 3;
  local_30 = ADV7842_INPUT_TYPE_NONE;
  adv7842_enum_input(adv7842_handle,aAStack_28,local_10,1);
  printf("Total input %d\n",local_10[0]);
  for (local_2c = 0; local_2c < local_10[0]; local_2c = local_2c + 1) {
    AVar1 = aAStack_28[local_2c].input;
    if (AVar1 == ADV7842_INPUT_TYPE_COMPONENT) {
      if (aAStack_28[local_2c].video_std != VIDEO_STD_NONE) {
        printf("Component signal detected with video standard %d\n",aAStack_28[local_2c].video_std);
        CurValidInput = CurValidInput | 2;
        if ((local_30 == ADV7842_INPUT_TYPE_NONE) || (local_30 == ADV7842_INPUT_TYPE_CVBS)) {
          local_30 = ADV7842_INPUT_TYPE_COMPONENT;
        }
      }
    }
    else if (AVar1 == ADV7842_INPUT_TYPE_HDMI) {
      if (aAStack_28[local_2c].video_std != VIDEO_STD_NONE) {
        printf("HDMI signal detected with video standard %d\n",aAStack_28[local_2c].video_std);
        CurValidInput = CurValidInput | 4;
        local_30 = ADV7842_INPUT_TYPE_HDMI;    // highest priority: unconditionally overrides
      }
    }
    else if (AVar1 == ADV7842_INPUT_TYPE_CVBS) {
      if (aAStack_28[local_2c].video_std != VIDEO_STD_NONE) {
        printf("CVBS signal detected with video standard %d\n",aAStack_28[local_2c].video_std);
        CurValidInput = CurValidInput | 1;
        if (local_30 == ADV7842_INPUT_TYPE_NONE) {
          local_30 = ADV7842_INPUT_TYPE_CVBS;
        }
      }
    }
    else {
      puts("Something wrong");
      local_30 = ADV7842_INPUT_TYPE_NONE;
    }
  }
  return local_30;
}



/*
 * sample_event_callback @ 00402b20
 *
 * Purpose:
 *   ADV7842 event callback invoked from the libadv7842 polling thread
 *   (adv7842_thread) whenever a subscribed event fires. Manages input switching,
 *   encoder stop/restart on resolution changes or signal loss, and HDCP
 *   encryption blue-screen forcing.
 *
 * Parameters:
 *   data - user context pointer passed in ADV7842_init_param_t.data (NULL).
 *   evt  - pointer to ADV7842_event_t:
 *            eventID:
 *              1    = ADV7842_EVT_HDMI_HPD (event_info: 0=unplugged, !=0=plugged)
 *              2    = ADV7842_EVT_INPUT_SIG_RES_CHG (event_info: new video_std_t)
 *              4    = ADV7842_EVT_INPUT_SIG_LOST
 *              8    = ADV7842_EVT_INPUT_DETECTED
 *              0x10 = ADV7842_EVT_HDMI_INPUT_SIG_ENC (HDCP encrypted input)
 *            input: ADV7842_input_type_t associated with the event.
 *
 * Return value: void.
 * Callers: adv7842_thread() in 04_libadv7842.cpp.c (via function pointer).
 * Callees: xcode4_stop_encoder(), adv7842_unsubscribe_event(),
 *          adv7842_program_input(), adv7842_detect_video_std(),
 *          adv7842_set_freerun_mode(), adv7842_subscribe_event(),
 *          start_encoder(), prepare_input_detect(), printf(), puts().
 *
 * Threading:
 *   Runs on the adv7842_thread context; mutates globals CurValidInput,
 *   CurInputType, curStd, and drives encoderHandle without an explicit mutex.
 *
 * Quirks / bugs:
 *   - In case 4 (SIG_LOST), if Component is lost and CVBS is valid & locks
 *     (lines 250-258), it subscribes to events (6) but never calls
 *     start_encoder()!
 *   - In case 4, if Component is detected by prepare_input_detect(0)
 *     (lines 261-270), xcode4_stop_encoder() is called a second time even
 *     though it was already stopped at line 226.
 *
 * Locals:
 *   local_c - video_std_t std (detected or new video standard).
 *   input   - evt->input cached before switch.
 *   param1  - "pluged" / "unpluged" status string for case 1 printf.
 *   uVar1   - temporary CurValidInput & 1 (CVBS valid bit).
 *   uVar2   - temporary CurValidInput & ~2 (Component valid bit cleared).
 */
// ==== sample_event_callback @ 00402b20 ====

void sample_event_callback(void *data,ADV7842_event_t *evt)

{
  ADV7842_input_type_t input;
  char *param1;
  uint uVar1;
  uint uVar2;
  video_std_t local_c;
  
                    /* Unresolved local var: video_std_t std@[???]
                       Unresolved local var: ADV7842_input_type_t input@[???] */
  local_c = VIDEO_STD_NONE;
  if (evt != (ADV7842_event_t *)0x0) {
    printf("Receive an Event ID %x  input %x info %x\n",evt->eventID,evt->input,evt->event_info);
    input = evt->input;
    switch(evt->eventID) {
    case 1:                                     // ADV7842_EVT_HDMI_HPD
      if (evt->event_info == 0) {
        param1 = "unpluged";
      }
      else {
        param1 = "pluged";
      }
      printf("HDMI is %s\n",param1);
      if (evt->event_info == 0) {
        CurValidInput = CurValidInput & 0xfffffffb; // clear HDMI bit (bit 2, ~4)
      }
      else {
        CurValidInput = CurValidInput | 4;      // set HDMI bit (bit 2)
      }
      if ((CurInputType == ADV7842_INPUT_TYPE_NONE) && ((CurValidInput & 4U) != 0)) {
        xcode4_stop_encoder(encoderHandle);
        adv7842_unsubscribe_event(adv7842_handle,8); // unsub INPUT_DETECTED
        adv7842_program_input(adv7842_handle,input,&local_c);
        adv7842_detect_video_std(adv7842_handle,&local_c);
        if (local_c == VIDEO_STD_NONE) {
          puts("No signal in HDMI");
          CurValidInput = CurValidInput & 0xfffffffb;
          prepare_input_detect(1);
        }
        else {
          puts("Start HDMI streaming");
          CurInputType = ADV7842_INPUT_TYPE_HDMI;
          adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
          start_encoder(local_c,1);
          adv7842_subscribe_event(adv7842_handle,6); // sub RES_CHG (2) | SIG_LOST (4)
        }
      }
      break;
    case 2:                                     // ADV7842_EVT_INPUT_SIG_RES_CHG
      local_c = evt->event_info;
      printf("Detected Video Standard change current %d new %d!\n",curStd,local_c);
      puts("Stop Encoder");
      xcode4_stop_encoder(encoderHandle);
      adv7842_program_input(adv7842_handle,CurInputType,&local_c);
      adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
      printf("Restart Encoder with std %d\n",local_c);
      start_encoder(local_c,1);
      curStd = local_c;
      break;
    case 4:                                     // ADV7842_EVT_INPUT_SIG_LOST
      xcode4_stop_encoder(encoderHandle);
      adv7842_unsubscribe_event(adv7842_handle,6); // unsub RES_CHG | SIG_LOST
      CurValidInput = ~input & CurValidInput;
      if ((input != ADV7842_INPUT_TYPE_HDMI) && ((CurValidInput & 4U) != 0)) {
        CurInputType = ADV7842_INPUT_TYPE_HDMI;
        adv7842_program_input(adv7842_handle,ADV7842_INPUT_TYPE_HDMI,&local_c);
        adv7842_detect_video_std(adv7842_handle,&local_c);
        if (local_c != VIDEO_STD_NONE) {
          puts("Start HDMI streaming");
          CurInputType = ADV7842_INPUT_TYPE_HDMI;
          start_encoder(local_c,1);
          adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
          adv7842_subscribe_event(adv7842_handle,6);
          return;
        }
        puts("HDMI has no signal");
        CurValidInput = CurValidInput & 0xfffffffb;
        CurInputType = ADV7842_INPUT_TYPE_NONE;
      }
      local_c = prepare_input_detect(0);
      if (local_c == VIDEO_STD_NONE) {
        uVar2 = CurValidInput & 0xfffffffd;     // clear Component bit (bit 1, ~2)
        uVar1 = CurValidInput & 1;              // check CVBS bit (bit 0)
        CurValidInput = uVar2;
        if (uVar1 != 0) {
          CurInputType = ADV7842_INPUT_TYPE_CVBS;
          adv7842_program_input(adv7842_handle,ADV7842_INPUT_TYPE_CVBS,&local_c);
          if (local_c == VIDEO_STD_NONE) {
            CurValidInput = CurValidInput & 0xfffffffe; // clear CVBS bit (bit 0, ~1)
          }
          else {
            adv7842_subscribe_event(adv7842_handle,6); // quirk: does not call start_encoder()
          }
        }
      }
      else {
        CurValidInput = CurValidInput | 2;      // set Component bit (bit 1)
        CurInputType = ADV7842_INPUT_TYPE_COMPONENT;
        xcode4_stop_encoder(encoderHandle);
        adv7842_program_input(adv7842_handle,CurInputType,&local_c);
        printf("Restart Encoder with std %d\n",local_c);
        start_encoder(local_c,1);
        adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
        adv7842_subscribe_event(adv7842_handle,6);
      }
      if (CurValidInput == 0) {
        puts("ADV7842_EVT_INPUT_DETECTED event registered");
        adv7842_subscribe_event(adv7842_handle,9); // sub HDMI_HPD (1) | INPUT_DETECTED (8)
        CurInputType = ADV7842_INPUT_TYPE_NONE;
      }
      break;
    case 8:                                     // ADV7842_EVT_INPUT_DETECTED
      adv7842_unsubscribe_event(adv7842_handle,8);
      CurValidInput = CurValidInput | input;
      if ((CurValidInput & ADV7842_INPUT_TYPE_HDMI) == ADV7842_INPUT_TYPE_NONE) {
        if ((CurValidInput & ADV7842_INPUT_TYPE_COMPONENT) == ADV7842_INPUT_TYPE_NONE) {
          if ((CurValidInput & ADV7842_INPUT_TYPE_CVBS) == ADV7842_INPUT_TYPE_NONE) {
            CurInputType = ADV7842_INPUT_TYPE_NONE;
          }
          else {
            CurInputType = ADV7842_INPUT_TYPE_CVBS;
          }
        }
        else {
          CurInputType = ADV7842_INPUT_TYPE_COMPONENT;
        }
      }
      else {
        CurInputType = ADV7842_INPUT_TYPE_HDMI;
      }
      if (CurInputType == ADV7842_INPUT_TYPE_NONE) {
        adv7842_subscribe_event(adv7842_handle,8);
      }
      else {
        printf("Select Input %d\n",CurInputType);
        xcode4_stop_encoder(encoderHandle);
        adv7842_program_input(adv7842_handle,CurInputType,&local_c);
        adv7842_detect_video_std(adv7842_handle,&local_c);
        printf("Detected Input Video Standard is %d\n",local_c);
        start_encoder(local_c,1);
        adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
        adv7842_subscribe_event(adv7842_handle,6);
      }
      curStd = local_c;
      break;
    case 0x10:                                  // ADV7842_EVT_HDMI_INPUT_SIG_ENC (HDCP)
      puts("HDMI signal is encrypted");
      adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_FORCED_BLUESCREEN);
    }
  }
  return;
}



/*
 * print_usage @ 004035f4
 *
 * Purpose:
 *   Print the command-line help text to stdout.
 *   Note: hidden options -l (LLC phase), -f (SD card capture), and -F (debug
 *   capture) are accepted by getopt() in main() but omitted from this listing.
 *
 * Parameters: none.
 * Return value: void.
 * Callers: main() (on -h, -H, -?, or invalid option/argument).
 */
// ==== print_usage @ 004035f4 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void print_usage(void)

{
  putchar(10);
  puts("Usage: av_streamer [options]");
  puts("Options: ");
  puts("\t-c [Video CODEC] (MPEG2, H264)");
  puts("\t-m [Video Output Mode] (UDP, DVO, FILE)");
  puts("\t-e [EDID Filename]");
  puts("\t-n [IP_ADDRESS] (default 192.168.1.193)");
  puts("\t-P [IP_PORT] (default 5002)");
  printf("\t-T [IP_TTL] (default 32, range %d to %d)\n",0,0xff);
  puts("\t-d [Data Insertion Filename]");
  puts("\t-t [Data Insertion Interval(ms)]");
  printf("\t-r [RMX Bitrate(bps)] (default %d, range %d to %d)\n",19000000,1500000,19000000);
  printf("\t-R [Bitrate(bps)] (default %d, range %d to %d)\n",17000000,1000000,17000000);
  puts("\t-i Enable Null Packet Insertion");
  puts("\t-o [Video Output Scaled To] (480i)");
  puts("\t-p [PCR PID]");
  puts("\t-M [Specify Minor Channel]");
  puts("\t-g Enable Table Generation");
  puts("\t-v Print Software Version");
  puts("\t-? Print Usage");
  puts("\t-h Print Usage");
  puts("\t-H Print Usage");
  return;
}



/*
 * print_version @ 004038c4
 *
 * Purpose:
 *   Print av_streamer version ("2.14", parsed by avs_start.sh via `-v`),
 *   changelog notes, library versions (libadv7842 0x108, libxcodeencoder 0x10a),
 *   and build metadata (time/host/dir) to stdout.
 *
 * Parameters: none.
 * Return value: void.
 * Callers: main() (on -v, -h, -H, -?, or unknown option).
 *
 * Decompiler notes:
 *   Ghidra could not resolve GOT-relative calls via $gp (puVar1 = &_gp):
 *     puVar1 - 0x7ed0 -> puts("2.14") (&DAT_0041ff7c = "2.14")
 *     puVar1 - 0x7ea0 -> printf(...)
 *     puVar1 - 0x7f4c -> puts(...)
 *   in_stack_fffffff4 is a Ghidra artifact from MIPS o32 double-precision
 *   argument passing in $a2/$a3 for "%4.2f" (2.14).
 */
// ==== print_version @ 004038c4 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void print_version(void)

{
  undefined1 *puVar1;
  undefined4 in_stack_fffffff4;
  
  puVar1 = &_gp;
  putchar(10);
  (**(code **)(puVar1 + -0x7ed0))(&DAT_0041ff7c); // puts("2.14")
  (**(code **)(puVar1 + -0x7ea0))                 // printf("av_streamer version %4.2f\n", 2.14)
            ("av_streamer version %4.2f\n",(double)CONCAT44(in_stack_fffffff4,puVar1));
  (**(code **)(puVar1 + -0x7f4c))("  Output optionally scaled to 480i");
  (**(code **)(puVar1 + -0x7f4c))("  Removed fec4xcode directory");
  (**(code **)(puVar1 + -0x7ea0))("  libadv7842 version 0x%x\n",0x108);
  (**(code **)(puVar1 + -0x7ea0))("  libxcodeencoder version 0x%x\n",0x10a);
  (**(code **)(puVar1 + -0x7f4c))("  vixs_encoder.c force avcSeiCtl while in H264 encoding");
  (**(code **)(puVar1 + -0x7f4c))("  TTL now optional");
  (**(code **)(puVar1 + -0x7ea0))("  Build Time = %s\n","Fri Oct  9 10:23:14 2015");
  (**(code **)(puVar1 + -0x7ea0))
            ("  Build Host = %s\n",
             "Linux Dell-5150 2.6.35.14-106.fc14.i686 #1 SMP Wed Nov 23 13:57:33 UTC 2011 i686 i686 i386 GNU/Linux"
            );
  (**(code **)(puVar1 + -0x7ea0))
            ("  Build Dir  = %s\n",
             "/media/WD_passport/Meld_AV_Script/2p14_a/avstreamer/src/avstreamer");
  return;
}



/*
 * main @ 00403a6c
 *
 * Purpose:
 *   Entry point for av_streamer. Parses CLI options, installs SIGTERM handler,
 *   logs invocation to syslog, initializes ADV7842 (including optional EDID file
 *   from -e), detects initial video input, builds global encoder_param, sets up
 *   PSIP/SI table generation (-g) or raw TS data insertion (-d), starts the
 *   XCode4 encoder, and runs either the interactive console loop ('q' quit,
 *   'g' print lock/output stats) or the background SIGTERM wait loop.
 *
 * Parameters:
 *   argc, argv - standard CLI arguments. On MT300-AV (avs_start.sh), launched
 *     with: -e Meld_Edid_r12_720p_pcm48.txt -c MPEG2 -m DVO -p 48 -g -t 1000
 *     -R 17000000 -r 19000000.
 *
 * Return value:
 *   0 (RET_OK) on clean exit, 1 (RET_ERR) on CLI/file error, ~RET_OK (-2) if
 *   adv7842_init() fails.
 *
 * Side effects:
 *   - Installs SIGTERM handler term().
 *   - Logs startup, stdin tty state, and exit to syslog (LOG_NOTICE = 5).
 *   - Reads /proc/self/mounts if -f is used; reads /etc/config/queryInfo if -g
 *     is used; reads data file if -d is used (without -g).
 *   - Initializes ADV7842 (/dev/vixs/xcodedrv I2C) and XCode4 encoder.
 *   - Calls fcloseall() on exit.
 *
 * Quirks / bugs:
 *   - Line 584-587: when -g (bVar4) is NOT set, the `else` branch unconditionally
 *     prints "Error! Can't open data file (null)" and "Warning: No data insertion"
 *     even when -d IS set (before line 588 actually opens the -d file); conversely,
 *     when -g IS set, line 581 forces bVar5 = false, silently overriding -d.
 *   - Line 627-629: when stdin is not a tty (backgrounded via avs_start.sh),
 *     `while (SigTerm == 0)` busy-spins at 100% CPU repeatedly calling memcpy()
 *     with no sleep/pause!
 *   - Line 757: `strncmp("/mnt/sdcard/", __s2, 0xb)` compares only 11 bytes
 *     ("/mnt/sdcard"), not 12 ("/mnt/sdcard/").
 *   - Line 494: Ghidra maps offset 4 of MIPS struct sigaction (sa_handler) onto
 *     sStack_73c.sa_mask.__val[0].
 *
 * Locals (mapping Ghidra names to original DWARF names):
 *   local_8f8   - int dstport (default 0x138a = 5002).
 *   local_8f4   - int dstttl (default 0x20 = 32).
 *   local_8f0   - vixs_xcode_video_codec_t videoCodec (default 1 = MPEG2).
 *   local_8ec   - vixs_encoder_output_mode_t mode (default 2 = DVO).
 *   local_8dc   - char *data_file (-d).
 *   local_8d8   - char *capture_file (-f / -F).
 *   local_8cc   - int data_interval in ms (default 5000).
 *   local_8c8   - uint32_t bitrate in bps (default 17000000).
 *   local_8c4   - uint32_t rmx_rate in bps (default 19000000).
 *   local_8c0   - uint32_t pcr_pid (default 0 = unset).
 *   local_8bc   - int n (argv loop index).
 *   local_898   - video_std_t std.
 *   local_894   - ADV7842_init_param_t adv7842_init_param.
 *   local_888   - char path[32] ("/etc/config/queryInfo").
 *   vStack_868  - vixs_encoder_ts_info_t encoder_ts_info.
 *   local_844   - char dstAddr[64] (default "192.168.1.193").
 *   local_804   - char dev_name[100] (joined with auStack_803).
 *   local_7a0   - char dev_mnt[100] (joined with auStack_79f).
 *   sStack_73c  - struct sigaction action (144 / 0x90 bytes).
 *   sStack_6a8  - unioned stack slot: fileconf conf (68 bytes, for readCfgFile)
 *                 and struct stat64 st (for -d data file size). st_size._4_4_
 *                 is offset 0x3c = fileconf.modulationmode (1 = ATSC).
 *   acStack_608 - char cmdLine[512] (syslog message buffer).
 *   local_408   - char tmp_line[1024] (joined with auStack_407).
 *   bVar1       - int sdcard_mounted.
 *   bVar2       - int insert_null_pkt (-i).
 *   bVar3       - int capture_filename_given (-f / -F).
 *   bVar4       - int sdtnit (-g, PSIP/SI table generation).
 *   bVar5       - int insert_data (-d).
 *   iVar6       - interactive = isatty(fileno(stdin)) (and getchar() temp).
 *   input       - ADV7842_input_type_t from detect_input_source().
 *   sVar11      - strlen() / malloc size (0x814=2068=11*188 ATSC; 0x2f0=752=4*188 DVB).
 *   pFVar12     - FILE *fp_proc (/proc/self/mounts) or dataFile (-d).
 *   __s2        - cached _optarg for -f prefix check.
 */
// ==== main @ 00403a6c ====

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int main(int argc,char **argv)

{
  bool bVar1;
  bool bVar2;
  bool bVar3;
  bool bVar4;
  bool bVar5;
  char *__s2;
  int iVar6;
  uint uVar7;
  char *pcVar8;
  int iVar9;
  return_code_t rVar10;
  ADV7842_input_type_t input;
  size_t sVar11;
  FILE *pFVar12;
  int local_8f8;
  int local_8f4;
  vixs_xcode_video_codec_t local_8f0;
  vixs_encoder_output_mode_t local_8ec;
  char *local_8dc;
  char *local_8d8;
  int local_8cc;
  uint32_t local_8c8;
  uint32_t local_8c4;
  uint32_t local_8c0;
  int local_8bc;
  video_std_t local_898;
  ADV7842_init_param_t local_894;
  char local_888 [32];
  vixs_encoder_ts_info_t vStack_868;
  char local_844 [64];
  char local_804;
  undefined1 auStack_803 [99];
  char local_7a0;
  undefined1 auStack_79f [99];
  sigaction sStack_73c;
  stat64 sStack_6a8;
  char acStack_608 [512];
  char local_408;
  undefined1 auStack_407 [1023];
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: char[64] dstAddr@[???]
                       Unresolved local var: int dstport@[???]
                       Unresolved local var: int dstttl@[???]
                       Unresolved local var: char ch@[???]
                       Unresolved local var: video_std_t std@[???]
                       Unresolved local var: vixs_xcode_video_codec_t videoCodec@[???]
                       Unresolved local var: ADV7842_init_param_t adv7842_init_param@[???]
                       Unresolved local var: vixs_encoder_output_mode_t mode@[???]
                       Unresolved local var: ADV7842_input_type_t input@[???]
                       Unresolved local var: vixs_encoder_ts_info_t encoder_ts_info@[???]
                       Unresolved local var: int insert_data@[???]
                       Unresolved local var: int sdtnit@[???]
                       Unresolved local var: int insert_null_pkt@[???]
                       Unresolved local var: char * data_file@[???]
                       Unresolved local var: char * capture_file@[???]
                       Unresolved local var: char[1024] tmp_line@[???]
                       Unresolved local var: char[100] dev_name@[???]
                       Unresolved local var: char[100] dev_mnt@[???]
                       Unresolved local var: int sdcard_mounted@[???]
                       Unresolved local var: int capture_filename_given@[???]
                       Unresolved local var: int data_size@[???]
                       Unresolved local var: int data_interval@[???]
                       Unresolved local var: FILE * dataFile@[???]
                       Unresolved local var: FILE * fp_proc@[???]
                       Unresolved local var: uint32_t bitrate@[???]
                       Unresolved local var: uint32_t rmx_rate@[???]
                       Unresolved local var: uint32_t pcr_pid@[???]
                       Unresolved local var: char[512] cmdLine@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: int interactive@[???]
                       Unresolved local var: uint16_t minor@[???]
                       Unresolved local var: sigaction action@[???] */
  builtin_strncpy(local_844,"192.168.1.193",0xe);
  memset(local_844 + 0xe,0,0x32);
  local_8f8 = 0x138a;                           // default UDP port 5002
  local_8f4 = 0x20;                             // default IP TTL 32
  local_898 = VIDEO_STD_NONE;
  local_8f0 = VIXS_XCODE_VIDEO_MPEG2;
  local_8ec = VIXS_ENCODER_OUT_MODE_DVO;
  bVar5 = false;
  bVar4 = false;
  bVar2 = false;
  local_8dc = (char *)0x0;
  local_8d8 = (char *)0x0;
  local_408 = '\0';
  memset(auStack_407,0,0x3ff);
  local_804 = '\0';
  memset(auStack_803,0,99);
  local_7a0 = '\0';
  memset(auStack_79f,0,99);
  bVar1 = false;
  bVar3 = false;
  local_8cc = 5000;                             // default data insertion interval: 5000 ms
  local_8c8 = 17000000;                         // default video bitrate: 17 Mbps
  local_8c4 = 19000000;                         // default RMX mux rate: 19 Mbps
  local_8c0 = 0;
  memset(&sStack_73c,0,0x90);
  sStack_73c.sa_mask.__val[0] = (ulong)term;    // MIPS sigaction.sa_handler at offset 4
  sigaction(0xf,&sStack_73c,(sigaction *)0x0);  // 0xf = SIGTERM
  memcpy(acStack_608,"main.c :",9);
  for (local_8bc = 0; local_8bc < argc; local_8bc = local_8bc + 1) {
    sVar11 = strlen(acStack_608);
    memcpy(acStack_608 + sVar11,&DAT_0042017c,2); // " " separator
    strcat(acStack_608,argv[local_8bc]);
  }
  syslog(5,acStack_608);                        // LOG_NOTICE = 5
  iVar6 = fileno(_stdin);
  iVar6 = isatty(iVar6);
  if (iVar6 == 0) {
    syslog(5,"main.c : STDIN is a file or a pipe");
  }
  else {
    syslog(5,"main.c : STDIN is a terminal");
  }
  memset(&local_894,0,0xc);
  local_894.data = (void *)0x0;
  local_894.event_callback = sample_event_callback;
  local_894.edid_file_name = (char *)0x0;
  do {
    iVar9 = getopt(argc,argv,"c:m:e:n:P:T:d:t:r:R:io:p:M:gl:f:F:v?hH");
    __s2 = _optarg;
    if ((char)iVar9 == -1) {                    // all CLI options parsed; start hardware init
      rVar10 = adv7842_init(&local_894,&adv7842_handle);
      if (rVar10 == RET_OK) {
        input = detect_input_source();
        if (input == ADV7842_INPUT_TYPE_NONE) {
          puts("\nNo Video Signals at any inputs\n");
        }
        else {
          printf("Select Input %d\n",input);
          adv7842_program_input(adv7842_handle,input,&local_898);
          adv7842_detect_video_std(adv7842_handle,&local_898);
        }
        curStd = local_898;
        CurInputType = input;
        printf("Video Input Detected : Standard = %d\n",local_898);
        memset(&encoder_param,0,0xc0);          // 0xc0 = 192 bytes = sizeof(vixs_encoder_init_param_t)
        encoder_param.bitRate = local_8c8;
        encoder_param.dstPort = local_8f8;
        encoder_param.dstTTL = local_8f4;
        encoder_param.dstAddr = string2netaddr(local_844);
        encoder_param.dvoPort = DVO_PORT_1;
        encoder_param.outMode = local_8ec;
        encoder_param.videoCodec = local_8f0;
        if (local_8c4 != 0) {
          encoder_param.flag = encoder_param.flag | 4; // bit 2: RMX rate valid
          encoder_param.rmxRate = local_8c4;
        }
        if (bVar2) {
          encoder_param.flag = encoder_param.flag | 2; // bit 1: NULL packet insertion
        }
        if (local_8c0 != 0) {
          encoder_param.flag = encoder_param.flag | 8; // bit 3: custom PCR PID
          encoder_param.pcrPid = local_8c0;
        }
        if ((local_8ec != VIXS_ENCODER_OUT_MODE_FILE) || (bVar3)) {
          if (local_8ec == VIXS_ENCODER_OUT_MODE_FILE) {
            strcpy(encoder_param.captureFileName,local_8d8);
          }
          if (bVar4) {                          // -g: PSIP/SI table generation
                    /* Unresolved local var: fileconf conf@[???]
                       Unresolved local var: char[32] path@[???]
                       Unresolved local var: int datasize@[???] */
            builtin_strncpy(local_888,"/etc/config/queryInfo",0x16);
            local_888[0x16] = '\0';
            local_888[0x17] = '\0';
            local_888[0x18] = '\0';
            local_888[0x19] = '\0';
            local_888[0x1a] = '\0';
            local_888[0x1b] = '\0';
            local_888[0x1c] = '\0';
            local_888[0x1d] = '\0';
            local_888[0x1e] = '\0';
            local_888[0x1f] = '\0';
            readCfgFile(local_888,(fileconf *)&sStack_6a8);
            if (sStack_6a8.st_size._4_4_ == 1) { // offset 0x3c = fileconf.modulationmode (1 = ATSC)
              sVar11 = 0x814;                   // 2068 = 11 * 188-byte TS packets (ATSC PSIP)
            }
            else {
              sVar11 = 0x2f0;                   // 752 = 4 * 188-byte TS packets (DVB-T/other SI)
            }
            encoder_param.flag = encoder_param.flag | 1; // bit 0: data insertion enabled
            encoder_param.data_insertion_param.data = malloc(sVar11);
            encoder_param.data_insertion_param.interval = local_8cc;
            bVar5 = false;                      // -g silently overrides -d
            encoder_param.data_insertion_param.data_size = sVar11;
          }
          else {
            // Bug: prints error even when -d IS set (or neither -g nor -d is set)
            printf("\nError! Can\'t open data file %s\n",local_8dc);
            puts("\nWarning: No data insertion");
          }
          if (bVar5) {                          // -d: raw TS data insertion from file
            pFVar12 = fopen64(local_8dc,"rb");
            if (pFVar12 == (FILE *)0x0) {
              printf("\nError! Can\'t open data file %s\n",local_8dc);
              puts("\nWarning: No data insertion");
            }
            else {
                    /* Unresolved local var: stat st@[???] */
              iVar9 = stat64(local_8dc,&sStack_6a8);
              if (iVar9 == -1) {
                perror("stat");
                    /* WARNING: Subroutine does not return */
                exit(1);
              }
              sVar11 = ((int)sStack_6a8.st_size / 0xbc) * 0xbc; // round down to multiple of 188 bytes
              if (sVar11 == 0) {
                puts("\nWarning: empty data file");
                puts("\nWarning: No data insertion");
              }
              else {
                printf("Insert data size %d interval %d\n",sVar11,local_8cc);
                encoder_param.flag = encoder_param.flag | 1;
                encoder_param.data_insertion_param.data = malloc(sVar11);
                encoder_param.data_insertion_param.data_size =
                     fread(encoder_param.data_insertion_param.data,1,sVar11,pFVar12);
                encoder_param.data_insertion_param.interval = local_8cc;
              }
              fclose(pFVar12);
            }
          }
          if ((curStd == VIDEO_STD_NONE) || (CurInputType == ADV7842_INPUT_TYPE_NONE)) {
            prepare_input_detect(0);
          }
          else {
            start_encoder(local_898,0);
            adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_AUTO);
            adv7842_subscribe_event(adv7842_handle,0x17); // 1(HPD) | 2(RES_CHG) | 4(SIG_LOST) | 16(ENC)
          }
          if (iVar6 == 0) {                     // non-interactive (daemonized): busy-wait on SigTerm
            while (SigTerm == 0) {
              memcpy(acStack_608,"main.c : Caught SIGTERM",0x18);
            }
            syslog(5,acStack_608);
          }
          else {                                // interactive terminal: 'q' to quit, 'g' for stats
            puts("\n  Press \'q\' to Exit");
            puts("  Press \'g\' to print encoder status\n");
            while( true ) {
              iVar6 = getchar();
              if ((char)iVar6 == 'q') break;
              if (((char)iVar6 == 'g') &&
                 (rVar10 = xcode4_get_encoder_ts_info(encoderHandle,&vStack_868), rVar10 == RET_OK))
              {
                printf("inputVideoLock: %x\n",vStack_868.inputVideoLock);
                printf("inputAudioLock: %x\n",vStack_868.inputAudioLock);
                printf("tsOutput: %x\n",vStack_868.tsOutput);
                printf("videoOutput: %x\n",vStack_868.videoOutput);
              }
            }
            puts("Caught keyboard \'q\' command");
          }
          rVar10 = xcode4_stop_encoder(encoderHandle);
          xcode4_close_encoder(encoderHandle);
          adv7842_deinit(adv7842_handle);
          if (encoder_param.data_insertion_param.data != (char *)0x0) {
            free(encoder_param.data_insertion_param.data);
          }
          memcpy(acStack_608,"main.c : Closed encoder handles. Exiting \"av_streamer\"",0x37);
          puts(acStack_608);
          syslog(5,acStack_608);
          fcloseall();
        }
        else {
          puts("\nError! No capture filename was provided.\n");
          print_usage();
          rVar10 = RET_ERR;
        }
      }
      else {
        puts("init adv7842 failed");
        rVar10 = ~RET_OK;
      }
      return rVar10;
    }
    switch((char)iVar9) {
    case 'F':                                   // hidden: debug capture file (no SD card check)
      local_8d8 = _optarg;
      bVar3 = true;
      printf("\nDebug mode - Set Capture file : %s\n",_optarg);
      break;
    default:
      print_version();
      print_usage();
      return 0;
    case 'M':                                   // PSIP TVCT minor channel number
      uVar7 = atoi(_optarg);
      setMinor((uint16_t)uVar7);
      printf("Set Minor Channel Number= %d\n",uVar7 & 0xffff);
      break;
    case 'P':                                   // UDP destination port
      local_8f8 = atoi(_optarg);
      printf("Set Port Number = %d\n",local_8f8);
      break;
    case 'R':                                   // video bitrate (1..17 Mbps)
      local_8c8 = atoi(_optarg);
      if ((17000000 < local_8c8) || (local_8c8 < 1000000)) {
        printf("\nError! The Video Data Bitrate %d - is not within range %d to %d.\n\n",local_8c8,
               1000000,17000000);
        print_usage();
        return 1;
      }
      printf("Set Video Data Bitrate = %d bps\n",local_8c8);
      break;
    case 'T':                                   // UDP IP TTL (0..255)
      local_8f4 = atoi(_optarg);
      if ((0xff < local_8f4) || (local_8f4 < 0)) {
        printf("\nError! The TTL Value %d - is not within range 0 to 255.\n\n",local_8f4);
        print_usage();
        return 1;
      }
      printf("Set TTL Value = %d\n",local_8f4);
      break;
    case 'c':                                   // video codec: H264 or MPEG2
      iVar9 = strcmp(_optarg,"H264");
      if (iVar9 == 0) {
        printf("Set Video CODEC = %s\n",_optarg);
        local_8f0 = VIXS_XCODE_VIDEO_H264;
      }
      else {
        iVar9 = strcmp(_optarg,"MPEG2");
        if (iVar9 != 0) {
          printf("\nError! The Video CODEC %s is not valid.\n\n",_optarg);
          print_usage();
          return 1;
        }
        printf("Set Video CODEC = %s\n",_optarg);
        local_8f0 = VIXS_XCODE_VIDEO_MPEG2;
      }
      break;
    case 'd':                                   // TS data insertion filename
      local_8dc = _optarg;
      printf("Set Data Insertion Filename = %s\n",_optarg);
      bVar5 = true;
      break;
    case 'e':                                   // ADV7842 EDID filename
      local_894.edid_file_name = _optarg;
      printf("Set EDID file = %s\n",_optarg);
      break;
    case 'f':                                   // hidden: capture file on /mnt/sdcard (/dev/mmcblk0p1)
      local_8d8 = _optarg;
      pFVar12 = fopen64("/proc/self/mounts","r");
      if (pFVar12 == (FILE *)0x0) {
        puts("\nError! Can\'t open /proc/self/mounts to check if an SDcard is mounted.\n");
        print_usage();
        return 1;
      }
      while (pcVar8 = fgets(&local_408,0x400,pFVar12), pcVar8 != (char *)0x0) {
        sscanf(&local_408,"%s %s",&local_804,&local_7a0);
        iVar9 = strncmp("/dev/mmcblk0p1",&local_804,0xe);
        if ((iVar9 == 0) && (iVar9 = strncmp("/mnt/sdcard",&local_7a0,0xb), iVar9 == 0)) {
          bVar1 = true;
          puts("\nAn SDcard is mounted.\n");
        }
      }
      if (!bVar1) {
        puts("\nError! An SDcard must be inserted.\n");
        print_usage();
        return 1;
      }
      iVar9 = strncmp("/mnt/sdcard/",__s2,0xb); // quirk: 0xb=11 compares "/mnt/sdcard" without trailing '/'
      if (iVar9 != 0) {
        puts("\nError! The Capture file name must start with \"/mnt/sdcard\".\n");
        print_usage();
        return 1;
      }
      printf("Set Capture file = %s\n",_optarg);
      bVar3 = true;
      break;
    case 'g':                                   // enable ATSC PSIP / DVB SI table generation
      bVar4 = true;
      puts("Enable SDT/NIT Table Generation");
      break;
    case 'i':                                   // enable NULL packet insertion
      puts("Enable NULL Packet Insertion");
      bVar2 = true;
      break;
    case 'l':                                   // hidden: ADV7842 LLC clock phase delay
      g_llc_delay = atoi(_optarg);
      printf("Set LLC Phase = %d\n",g_llc_delay);
      break;
    case 'm':                                   // output mode: UDP, DVO, FILE
      iVar9 = strcmp(_optarg,"UDP");
      if (iVar9 == 0) {
        printf("Set Encoder Output Mode = %s\n",_optarg);
        local_8ec = VIXS_ENCODER_OUT_MODE_UDP;
      }
      else {
        iVar9 = strcmp(_optarg,"DVO");
        if (iVar9 == 0) {
          printf("Set Encoder Output Mode = %s\n",_optarg);
          local_8ec = VIXS_ENCODER_OUT_MODE_DVO;
        }
        else {
          iVar9 = strcmp(_optarg,"FILE");
          if (iVar9 != 0) {
            printf("\nError! The Encoder Output Mode %s is not valid.\n\n",_optarg);
            print_usage();
            return 1;
          }
          printf("Set Encoder Output Mode = %s\n",_optarg);
          local_8ec = VIXS_ENCODER_OUT_MODE_FILE;
        }
      }
      break;
    case 'n':                                   // UDP destination IP/hostname
      strcpy(local_844,_optarg);
      printf("Set Destination IP Address = %s\n",local_844);
      break;
    case 'o':                                   // output scaler: only "480i" accepted
      iVar9 = strcmp(_optarg,"480i");
      if (iVar9 != 0) {
        printf("\nError! The Video Output Scaling %s is not valid.\n\n",_optarg);
        print_usage();
        return 1;
      }
      scaler_o480i = 1;
      printf("Enable Video Output Scaling to %s\n",_optarg);
      break;
    case 'p':                                   // custom PCR PID
      local_8c0 = atoi(_optarg);
      printf("Set PCR PID = %d\n",local_8c0);
      break;
    case 'r':                                   // RMX mux bitrate (1.5..19 Mbps)
      local_8c4 = atoi(_optarg);
      if ((19000000 < local_8c4) || (local_8c4 < 1500000)) {
        printf("\nError! The RMX Bitrate %d - is not within range %d to %d.\n\n",local_8c4,1500000,
               19000000);
        print_usage();
        return 1;
      }
      printf("Set RMX Bitrate = %d bps\n",local_8c4);
      break;
    case 't':                                   // data insertion interval in ms
      local_8cc = atoi(_optarg);
      printf("Set Data Insertion Interval = %dms\n",local_8cc);
      break;
    case 'v':                                   // print version and exit
      print_version();
      return 0;
    }
  } while( true );
}


/* #####################################################################
 * FILE: 01_libxcode4encoder.c.c
 * ##################################################################### */

/*
 * 01_libxcode4encoder.c.c  --  annotated decompilation of libxcode4encoder.c
 * ============================================================================
 *
 * Original source path (from DWARF):
 *   /media/WD_passport/Meld_AV_Script/2p13_a/avstreamer/src/libxcode4encoder/libxcode4encoder.c
 * Address range: 0x00405420 .. 0x0040bdfc (30 functions, 1735 raw lines)
 *
 * Role in av_streamer:
 *   High-level ViXS XCode4 encoder wrapper, thread manager, and ATSC PSIP /
 *   DVB-T SI table generator. Called by main.c (start_encoder(), main()) and
 *   delegates low-level /dev/vixs/xcodedrv ioctls to vixs_encoder.c.
 *
 * Main data structures (DWARF):
 *   - vixs_encoder_t (0x1980 = 6528 bytes): encoder instance allocated by
 *     xcode4_open_encoder(). Key offsets left raw by Ghidra:
 *       +0x003c  outputMode (1=UDP, 2=DVO, 3=FILE)
 *       +0x00d4  bSendData  (non-zero when -g PSIP/SI injection is enabled)
 *       +0x125c  ioctl_ts_ctrl (IOCTLSDEVTSCTRL)
 *       +0x1324  ioctl_ts_ctrl.ioctl_param.tsinput_lock (TS status bitmask)
 *   - fileconf (t_conf, 68 bytes): parsed from /etc/config/queryInfo
 *   - vixs_encoder_ts_info_t (36 bytes): unpacked TS lock/output status
 *
 * Globals owned by this unit:
 *   - crc32_tab[256] @ 0x00436170: byte-swapped MPEG-2 CRC-32 (0x04c11db7) LUT
 *   - sti_minor      @ 0x00436574: uint16_t PSIP TVCT minor channel (default 1)
 */

// ==== message @ 00405420 ====

/*
 * message(): libdvbpsi logging callback (dvbpsi_message_cb).
 *   Params: handle (unused), level (DVBPSI_MSG_ERROR/WARN/DEBUG), msg
 *   Returns: void
 *   Callers: passed to dvbpsi_new() in genNIT, genSDT, genTDT, genEIT.
 *   Side effects: writes formatted log line to stderr; ignores DVBPSI_MSG_NONE (-1).
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void message(dvbpsi_t *handle,dvbpsi_msg_level_t level,char *msg)

{
  if (level == DVBPSI_MSG_WARN) {
    fwrite("Warning: ",1,9,_stderr);
  }
  else if (level == DVBPSI_MSG_DEBUG) {
    fwrite("Debug: ",1,7,_stderr);
  }
  else {
    if (level != DVBPSI_MSG_ERROR) {
      return;
    }
    fwrite("Error: ",1,7,_stderr);
  }
  fprintf(_stderr,"%s\n",msg);
  return;
}



// ==== writePSI @ 00405574 ====

/*
 * writePSI(): packs a libdvbpsi PSI section list into a single 188-byte (0xbc)
 *             MPEG-2 TS packet whose 4-byte TS header (PID, CC) was already
 *             written by the caller.
 *   Params: p_packet (188-byte TS buffer), p_section (linked list of sections)
 *   Returns: void
 *   Callers: genNIT, genSDT, genTDT, genEIT
 *   Locals:
 *     local_res4 = p_section loop cursor
 *     local_14   = p_byte (source pointer into p_section->p_data)
 *     puVar2     = p_end  (p_section->p_payload_end)
 *     iVar1      = trailing CRC length (4 if b_syntax_indicator, else 0)
 *     local_18   = p_pos_in_ts (destination pointer starting at p_packet + 5)
 *   Quirk: if p_section->p_next != NULL, subsequent sections overwrite the same
 *          188-byte packet buffer from offset 5 instead of advancing packets.
 */
void writePSI(uint8_t *p_packet,dvbpsi_psi_section_t *p_section)

{
  int iVar1;
  uint8_t *puVar2;
  dvbpsi_psi_section_t *local_res4;
  uint8_t *local_18;
  uint8_t *local_14;
  
  *p_packet = 0x47;                                // TS sync byte
  for (local_res4 = p_section; local_res4 != (dvbpsi_psi_section_t *)0x0;
      local_res4 = local_res4->p_next) {
                    /* Unresolved local var: uint8_t * p_pos_in_ts@[???]
                       Unresolved local var: uint8_t * p_byte@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
    local_14 = local_res4->p_data;
    puVar2 = local_res4->p_payload_end;
    if (local_res4->b_syntax_indicator == false) {
      iVar1 = 0;                                   // short-syntax section (e.g. TDT): no CRC32
    }
    else {
      iVar1 = 4;                                   // long-syntax section: include 4-byte CRC32
    }
    p_packet[1] = p_packet[1] | 0x40;              // set Payload Unit Start Indicator (PUSI = 1)
    p_packet[4] = 0;                               // PSI pointer_field = 0
    local_18 = p_packet + 5;
    for (; (local_18 < p_packet + 0xbc && (local_14 < puVar2 + iVar1)); local_14 = local_14 + 1) {
      *local_18 = *local_14;
      local_18 = local_18 + 1;
    }
    for (; local_18 < p_packet + 0xbc; local_18 = local_18 + 1) {
      *local_18 = 0xff;                            // pad remainder of 188-byte TS packet with 0xff stuffing
    }
  }
  return;
}



// ==== bswap_32 @ 004056dc ====

/*
 * bswap_32(): byte-swaps a 32-bit unsigned integer (endian conversion).
 *   Params: x (32-bit value)
 *   Returns: byte-swapped uint32_t
 *   Callers: FormATSC_MGT, FormATSC_ETT, FormATSC_EIT, FormATSC_TVCT, FormATSC_STT
 *   Locals: uVar1 = intermediate value with adjacent bytes swapped within 16-bit halves
 */
uint32_t bswap_32(uint32_t x)

{
  uint uVar1;
  
  uVar1 = (x & 0xff00ff) << 8 | x >> 8 & 0xff00ff; // swap bytes within each 16-bit half
  return (uVar1 >> 0x10) + uVar1 * 0x10000;        // swap the two 16-bit halves
}



// ==== crc32 @ 00405740 ====

/*
 * crc32(): computes MPEG-2 PSI/PSIP CRC-32 (polynomial 0x04c11db7, MSB-first).
 *   Params: crc (initial seed, callers pass 0xffffffff), buffer, length
 *   Returns: byte-swapped 32-bit CRC (callers pass result to bswap_32())
 *   Callers: FormATSC_MGT, FormATSC_ETT, FormATSC_EIT, FormATSC_TVCT, FormATSC_STT
 *   Locals: local_res0 = running CRC; local_res4 = byte pointer into buffer
 *   Note: crc32_tab @ 0x00436170 stores byte-swapped table entries (e.g.
 *         crc32_tab[1] = 0xb71dc104 in LE = bswap_32(0x04c11db7)), allowing a
 *         right-shift loop on little-endian MIPS followed by bswap_32().
 */
uint32_t crc32(uint32_t crc,uint8_t *buffer,size_t length)

{
  uint local_res0;
  byte *local_res4;
  
                    /* Unresolved local var: uint8_t * end@[???] */
  local_res0 = crc;
  for (local_res4 = buffer; local_res4 < buffer + length; local_res4 = local_res4 + 1) {
    local_res0 = crc32_tab[local_res0 & 0xff ^ (uint)*local_res4] ^ local_res0 >> 8;
  }
  return local_res0;
}



// ==== FormTSHeader @ 004057e8 ====

/*
 * FormTSHeader(): writes a 4-byte MPEG-2 TS header + 1-byte PSI pointer_field.
 *   Params: dat8 (output buffer), pid (13-bit PID), counter (4-bit continuity counter)
 *   Returns: 5 (bytes written)
 *   Callers: encoder_send_data_thread_entry (for ATSC PSIP packets)
 */
int FormTSHeader(uint8_t *dat8,uint16_t pid,uint8_t counter)

{
  *dat8 = 0x47;                                    // TS sync byte
  dat8[1] = (byte)(pid >> 8) & 0x1f | 0x40;        // TEI=0, PUSI=1 (0x40), priority=0, PID[12:8]
  dat8[2] = (uint8_t)pid;                          // PID[7:0]
  dat8[3] = counter & 0xf | 0x10;                  // scrambling=00, adaptation=01 (payload only), CC
  dat8[4] = 0;                                     // PSI pointer_field = 0
  return 5;
}



// ==== FormATSC_MGT @ 00405898 ====

/*
 * FormATSC_MGT(): builds an ATSC A/65 Master Guide Table (MGT, table_id=0xC7)
 *                 section listing the TVCT, EIT-0..N, and ETT-0..M tables.
 *   Params: data (output buffer), vctlen (TVCT section length),
 *           eitlen/eitpid/eitcnt, ettlen/ettpid/ettcnt
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals:
 *     local_30 = tables_defined count
 *     local_38 = write offset i into data
 *     local_34 = loop index j over EIT / ETT arrays
 *     uVar1    = current eitpid[j] or ettpid[j]
 *     length   = byte length before the 4-byte CRC32
 *     uVar2    = computed CRC32
 */
int FormATSC_MGT(uint8_t *data,uint32_t vctlen,uint32_t *eitlen,uint16_t *eitpid,uint8_t eitcnt,
                uint32_t *ettlen,uint16_t *ettpid,uint8_t ettcnt)

{
  uint16_t uVar1;
  size_t length;
  uint32_t uVar2;
  int local_38;
  int local_34;
  ushort local_30;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t reserved_3@[???]
                       Unresolved local var: uint8_t reserved_5@[???]
                       Unresolved local var: uint8_t reserved_6@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t section_number@[???]
                       Unresolved local var: uint8_t last_section_number@[???]
                       Unresolved local var: uint16_t tables_defined@[???]
                       Unresolved local var: uint16_t table_type_PID@[???]
                       Unresolved local var: uint8_t table_type_version_number@[???]
                       Unresolved local var: uint16_t table_type_descriptors_length@[???]
                       Unresolved local var: uint16_t descriptors_length@[???] */
  local_30 = (ushort)(vctlen != 0);
  if (eitcnt != 0) {
    local_30 = eitcnt + local_30;
  }
  if (ettcnt != 0) {
    local_30 = ettcnt + local_30;
  }
  *data = 199;                                     // table_id = 0xC7 (MGT)
  data[1] = 0xf0;                                  // section_syntax=1, private=1, reserved=11 (length patched later)
  data[2] = 0;
  data[3] = 0;                                     // table_id_extension = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17 (23), current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)(local_30 >> 8);              // tables_defined MSB
  data[10] = (uint8_t)local_30;                    // tables_defined LSB
  local_38 = 0xb;
  if (vctlen != 0) {
    data[0xb] = 0;                                 // table_type = 0x0000 (Terrestrial VCT, current_next=1)
    data[0xc] = 0;
    data[0xd] = 0xff;                              // reserved=111, PID=0x1ffb (PSIP base PID)
    data[0xe] = 0xfb;
    data[0xf] = 0xf7;                              // reserved=111, version_number=0x17
    data[0x10] = (uint8_t)(vctlen >> 0x18);        // number_bytes (32-bit BE)
    data[0x11] = (uint8_t)(vctlen >> 0x10);
    data[0x12] = (uint8_t)(vctlen >> 8);
    data[0x13] = (uint8_t)vctlen;
    data[0x14] = 0xf0;                             // reserved=1111, table_type_descriptors_length=0
    data[0x15] = 0;
    local_38 = 0x16;
  }
  if (eitcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)eitcnt; local_34 = local_34 + 1) {
      uVar1 = eitpid[local_34];
      data[local_38] = 1;                          // table_type = 0x0100 + j (EIT-j)
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0; // reserved=111 | PID[12:8]
      data[local_38 + 3] = (uint8_t)uVar1;         // PID[7:0]
      data[local_38 + 4] = 0xf7;                   // reserved=111, version_number=0x17
      data[local_38 + 5] = (uint8_t)(eitlen[local_34] >> 0x18); // number_bytes (32-bit BE)
      data[local_38 + 6] = (uint8_t)(eitlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(eitlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)eitlen[local_34];
      data[local_38 + 9] = 0xf0;                   // descriptors_length = 0
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  if (ettcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)ettcnt; local_34 = local_34 + 1) {
      uVar1 = ettpid[local_34];
      data[local_38] = 2;                          // table_type = 0x0200 + j (Event ETT-j)
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0; // reserved=111 | PID[12:8]
      data[local_38 + 3] = (uint8_t)uVar1;         // PID[7:0]
      data[local_38 + 4] = 0xf7;                   // reserved=111, version_number=0x17
      data[local_38 + 5] = (uint8_t)(ettlen[local_34] >> 0x18); // number_bytes (32-bit BE)
      data[local_38 + 6] = (uint8_t)(ettlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(ettlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)ettlen[local_34];
      data[local_38 + 9] = 0xf0;                   // descriptors_length = 0
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  data[local_38] = 0xf0;                           // trailing MGT descriptors_length = 0
  data[local_38 + 1] = 0;
  length = local_38 + 2;
  data[1] = (byte)((uint)(local_38 + 3) >> 8) | 0xf0; // patch section_length = (length + 4) - 3 = local_38 + 3
  data[2] = (char)length + 1;
  uVar2 = crc32(0xffffffff,data,length);
  uVar2 = bswap_32(uVar2);
  data[length] = (uint8_t)(uVar2 >> 0x18);         // append 4-byte CRC32 (big-endian)
  data[local_38 + 3] = (uint8_t)(uVar2 >> 0x10);
  data[local_38 + 4] = (uint8_t)(uVar2 >> 8);
  data[local_38 + 5] = (uint8_t)uVar2;
  return local_38 + 6;
}



// ==== setMinor @ 004063e4 ====

/*
 * setMinor(): sets the global PSIP TVCT minor channel number (sti_minor).
 *   Params: minor (16-bit minor channel number; default sti_minor = 1)
 *   Returns: minor
 *   Callers: main() in 00_main.c.c on CLI option '-M <minor>'
 */
uint16_t setMinor(uint16_t minor)

{
  sti_minor = minor;
  return minor;
}



// ==== FormATSC_ETT @ 00406420 ====

/*
 * FormATSC_ETT(): builds an ATSC A/65 Extended Text Table (ETT, table_id=0xCC)
 *                 section with a single "eng" Multiple String Structure (MSS).
 *   Params: data (output buffer), addinfo (text), addinfolen, source_id, event_id
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (PIDs 0x1100..0x1103)
 *   Locals:
 *     iVar2    = event_id << 2 (ETM_id bits [15:2])
 *     bVar1    = ETM_id[1:0] (0 for channel ETM if event_id==0, 2 for event ETM)
 *     local_30 = write offset i into data
 *     local_2c = MSS string loop index l (0..0)
 *     local_28 = MSS segment loop index m (0..0)
 *     local_24 = byte copy loop index n (0..addinfolen-1)
 *     uVar3    = computed CRC32
 *   Quirk: data[10] ORs (iVar2 >> 16) into source_id LSB, which would corrupt
 *          source_id if event_id >= 0x4000 (safe for event_id 10..40 used here).
 */
int FormATSC_ETT(uint8_t *data,char *addinfo,int addinfolen,uint16_t source_id,uint16_t event_id)

{
  byte bVar1;
  int iVar2;
  uint32_t uVar3;
  size_t local_30;
  int local_2c;
  int local_28;
  int local_24;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t ETT_table_id_ext@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint32_t ETM_id@[???] */
  iVar2 = (uint)event_id << 2;
  if (event_id == 0) {
    bVar1 = 0;                                     // channel ETM (ETM_id[1:0] = 0b00)
  }
  else {
    bVar1 = 2;                                     // event ETM (ETM_id[1:0] = 0b10)
  }
  *data = 0xcc;                                    // table_id = 0xCC (ETT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = (uint8_t)(event_id >> 8);              // ETT_table_id_extension = event_id
  data[4] = (uint8_t)event_id;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)(source_id >> 8);             // ETM_id[31:16] = source_id
  data[10] = (byte)source_id | (byte)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);         // ETM_id[15:2] = event_id, [1:0] = bVar1
  data[0xc] = (byte)iVar2 | bVar1;
  data[0xd] = 1;                                   // MSS: number_strings = 1
  local_30 = 0xe;
  for (local_2c = 0; local_2c < 1; local_2c = local_2c + 1) {
    data[local_30] = 0x65;                         // ISO_639_language_code = "eng"
    data[local_30 + 1] = 0x6e;
    data[local_30 + 2] = 0x67;
    data[local_30 + 3] = 1;                        // number_segments = 1
    local_30 = local_30 + 4;
    for (local_28 = 0; local_28 < 1; local_28 = local_28 + 1) {
      data[local_30] = 0;                          // compression_type = 0 (uncompressed)
      data[local_30 + 1] = 0;                      // mode = 0 (ISO-8859-1 / ASCII)
      data[local_30 + 2] = (uint8_t)addinfolen;    // number_bytes
      local_30 = local_30 + 3;
      for (local_24 = 0; local_24 < addinfolen; local_24 = local_24 + 1) {
        data[local_30] = addinfo[local_24];
        local_30 = local_30 + 1;
      }
    }
  }
  data[1] = (byte)(local_30 + 1 >> 8) | 0xf0;      // patch section_length = (local_30 + 4) - 3
  data[2] = (char)local_30 + 1;
  uVar3 = crc32(0xffffffff,data,local_30);
  uVar3 = bswap_32(uVar3);
  data[local_30] = (uint8_t)(uVar3 >> 0x18);
  data[local_30 + 1] = (uint8_t)(uVar3 >> 0x10);
  data[local_30 + 2] = (uint8_t)(uVar3 >> 8);
  data[local_30 + 3] = (uint8_t)uVar3;
  return local_30 + 4;
}



// ==== FormATSC_EIT @ 00406980 ====

/*
 * FormATSC_EIT(): builds an ATSC A/65 Event Information Table (EIT, table_id=0xCB)
 *                 section containing 1 hardcoded 6-hour event for source_id=3.
 *   Params: data (output buffer), title, title_len, timeoffs (sec from UTC midnight),
 *           evt_id (event_id), etm (2-bit ETM_location)
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (4 EITs on PIDs 0x1000..0x1003)
 *   Locals:
 *     tVar3    = current Unix time
 *     iVar4    = GPS start_time: UTC midnight in GPS seconds + timeoffs
 *                (0x12d53d70 = 315,964,800 = Unix-to-GPS epoch 1970->1980)
 *     local_50 = write offset i into data
 *     uVar5    = title_len_pos (offset of event title_length byte)
 *     uVar2    = computed event descriptors_length
 *     bVar1    = single-iteration loop guard
 *     local_4c/48/44 = MSS loop indices l/m/n
 *     uVar6    = computed CRC32
 *   Quirks:
 *     - Masks uVar5 and local_50 with 0xff when patching lengths (breaks if >= 256).
 *     - Empty dead loop at end of descriptor block.
 *     - Hardcodes source_id=3, duration=0x5460 (21600s = 6h), and Caption Service
 *       Descriptor (0x86, "eng", digital_cc=1).
 */
/* WARNING: Heritage AFTER dead removal. Example location: v0xffffffe4 : 0x00406cc4 */
/* WARNING: Restarted to delay deadcode elimination for space: stack */

int FormATSC_EIT(uint8_t *data,char *title,int title_len,uint32_t timeoffs,uint16_t evt_id,int etm)

{
  bool bVar1;
  ushort uVar2;
  time_t tVar3;
  int iVar4;
  uint uVar5;
  uint32_t uVar6;
  size_t local_50;
  int local_4c;
  int local_48;
  int local_44;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint16_t j@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t source_id@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint16_t event_id@[???]
                       Unresolved local var: uint8_t reserved_1@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t ETM_location@[???]
                       Unresolved local var: uint8_t title_len_pos@[???]
                       Unresolved local var: uint8_t desc_len_pos@[???]
                       Unresolved local var: uint16_t descriptors_length@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: uint8_t num_events_in_section@[???]
                       Unresolved local var: uint32_t[1] gpstime@[???]
                       Unresolved local var: uint32_t[1] len_secs@[???] */
  tVar3 = time((time_t *)0x0);
  iVar4 = (tVar3 / 0x15180) * 0x15180 + -0x12d53d70 + timeoffs; // 0x15180=86400s/day; -0x12d53d70=-315964800s (GPS epoch)
  *data = 0xcb;                                    // table_id = 0xCB (EIT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // source_id = 0x0003
  data[4] = 3;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = 1;                                     // num_events_in_section = 1
  local_50 = 10;
  bVar1 = false;
  while (!bVar1) {
    data[local_50] = (byte)(evt_id >> 8) | 0xc0;   // reserved=11 | event_id[13:8]
    data[local_50 + 1] = (uint8_t)evt_id;          // event_id[7:0]
    data[local_50 + 2] = (uint8_t)((uint)iVar4 >> 0x18); // start_time (32-bit BE GPS seconds)
    data[local_50 + 3] = (uint8_t)((uint)iVar4 >> 0x10);
    data[local_50 + 4] = (uint8_t)((uint)iVar4 >> 8);
    data[local_50 + 5] = (uint8_t)iVar4;
    data[local_50 + 6] = ((byte)etm & 3) << 4 | 0xc0; // reserved=11, ETM_location=etm, duration[19:16]=0
    data[local_50 + 7] = 0x54;                     // length_in_seconds = 0x05460 (21600s = 6 hours)
    data[local_50 + 8] = 0x60;
    uVar5 = local_50 + 9;                          // title_len_pos
    data[uVar5] = 0;                               // placeholder for title_length
    data[local_50 + 10] = 1;                       // MSS: number_strings = 1
    local_50 = local_50 + 0xb;
    for (local_4c = 0; local_4c < 1; local_4c = local_4c + 1) {
      data[local_50] = 0x65;                       // "eng"
      data[local_50 + 1] = 0x6e;
      data[local_50 + 2] = 0x67;
      data[local_50 + 3] = 1;                      // number_segments = 1
      local_50 = local_50 + 4;
      for (local_48 = 0; local_48 < 1; local_48 = local_48 + 1) {
        data[local_50] = 0;                        // compression_type = 0
        data[local_50 + 1] = 0;                    // mode = 0
        data[local_50 + 2] = (uint8_t)title_len;   // number_bytes
        local_50 = local_50 + 3;
        for (local_44 = 0; local_44 < title_len; local_44 = local_44 + 1) {
          data[local_50] = title[local_44];
          local_50 = local_50 + 1;
        }
      }
    }
    data[uVar5 & 0xff] = ((byte)local_50 - (char)uVar5) - 1; // patch title_length
    data[local_50] = 0;                            // descriptors_length placeholder (2 bytes)
    data[local_50 + 1] = 0;
    data[local_50 + 2] = 0x86;                     // descriptor_tag = 0x86 (ATSC Caption Service Descriptor)
    data[local_50 + 3] = 7;                        // descriptor_length = 7
    data[local_50 + 4] = 0xe1;                     // reserved=111, number_of_services=1
    data[local_50 + 5] = 0x65;                     // language = "eng"
    data[local_50 + 6] = 0x6e;
    data[local_50 + 7] = 0x67;
    data[local_50 + 8] = 0xc1;                     // digital_cc=1, reserved=1, caption_service_number=1
    data[local_50 + 9] = 0x3f;                     // easy_reader=0, wide_aspect_ratio=0, reserved=111111
    data[local_50 + 10] = 0xff;                    // reserved=11111111
    uVar2 = ((short)(local_50 + 0xb) - (ushort)(byte)local_50) - 2; // = 9 bytes
    data[local_50 & 0xff] = (byte)(uVar2 >> 8) & 0xf | 0xf0; // patch descriptors_length
    data[(local_50 & 0xff) + 1] = (uint8_t)uVar2;
    for (local_4c = 0; local_4c < (int)(uint)uVar2; local_4c = local_4c + 1) {
    }
    local_50 = local_50 + 0xb;
    bVar1 = true;
  }
  data[1] = (byte)(local_50 + 1 >> 8) | 0xf0;
  data[2] = (char)local_50 + 1;
  uVar6 = crc32(0xffffffff,data,local_50);
  uVar6 = bswap_32(uVar6);
  data[local_50] = (uint8_t)(uVar6 >> 0x18);
  data[local_50 + 1] = (uint8_t)(uVar6 >> 0x10);
  data[local_50 + 2] = (uint8_t)(uVar6 >> 8);
  data[local_50 + 3] = (uint8_t)uVar6;
  return local_50 + 4;
}



// ==== FormATSC_TVCT @ 004073b8 ====

/*
 * FormATSC_TVCT(): builds an ATSC A/65 Terrestrial Virtual Channel Table (TVCT,
 *                  table_id=0xC8) section defining 1 virtual channel ("MELD300",
 *                  major=mjchannel, minor=mnchannel).
 *   Params: data (output buffer), mjchannel (from conf.chan_freq), mnchannel
 *           (from sti_minor), extended_name, ext_name_len, etm
 *   Returns: total section length in bytes (including 4-byte CRC32)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals:
 *     local_68 = write offset i into data
 *     sVar1    = saved channel-entry base offset (10)
 *     local_64 = channel loop index j (0..0)
 *     local_5c = channel descriptors_length accumulator
 *     iVar2    = desc_len_pos (offset of 10-bit channel descriptors_length)
 *     iVar3    = descriptor length byte offset (for 0xA0 and 0xA1 descriptors)
 *     local_58/54/50 = MSS loop indices l/m/n for Extended Channel Name (0xA0)
 *     local_60 = dead loop index k (0 < 0)
 *     uVar4    = computed CRC32
 *   Note:
 *     - Short name is hardcoded UTF-16BE "MELD300".
 *     - Service Location Descriptor (0xA1) hardcodes PCR_PID=0x0030, MPEG-2 video
 *       (stream_type=0x02, PID=0x0030), and AC-3 audio (stream_type=0x81, PID=0x0031).
 *     - Minor channel mask (& 0xf) at data[+0xf] is 2 bits wider than ATSC 10-bit minor.
 */
int FormATSC_TVCT(uint8_t *data,uint16_t mjchannel,uint16_t mnchannel,char *extended_name,
                 int ext_name_len,uint8_t etm)

{
  size_t sVar1;
  int iVar2;
  int iVar3;
  uint32_t uVar4;
  size_t local_68;
  int local_64;
  int local_60;
  short local_5c;
  int local_58;
  int local_54;
  int local_50;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: int k@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint16_t transport_stream_id@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t reserved_1@[???]
                       Unresolved local var: uint8_t reserved_2@[???]
                       Unresolved local var: uint8_t reserved_3@[???]
                       Unresolved local var: uint8_t reserved_4@[???]
                       Unresolved local var: uint8_t reserved_5@[???]
                       Unresolved local var: uint8_t modulation_mode@[???]
                       Unresolved local var: uint16_t channelTSID@[???]
                       Unresolved local var: uint16_t program_number@[???]
                       Unresolved local var: uint8_t ETM_location@[???]
                       Unresolved local var: uint8_t access_controlled@[???]
                       Unresolved local var: uint8_t hidden@[???]
                       Unresolved local var: uint8_t hide_guide@[???]
                       Unresolved local var: uint8_t service_type@[???]
                       Unresolved local var: uint8_t num_channels_in_section@[???]
                       Unresolved local var: uint16_t major_channel_number@[???]
                       Unresolved local var: uint16_t minor_channel_number@[???]
                       Unresolved local var: uint16_t source_id@[???]
                       Unresolved local var: uint16_t descriptors_length@[???]
                       Unresolved local var: uint16_t add_descriptors_length@[???] */
  local_5c = 0;
  *data = 200;                                     // table_id = 0xC8 (TVCT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // transport_stream_id = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = 1;                                     // num_channels_in_section = 1
  local_68 = 10;
  for (local_64 = 0; sVar1 = local_68, local_64 < 1; local_64 = local_64 + 1) {
                    /* Unresolved local var: int desc_len_pos@[???]
                       Unresolved local var: int ext_name_pos@[???]
                       Unresolved local var: int l@[???]
                       Unresolved local var: int m@[???]
                       Unresolved local var: int n@[???]
                       Unresolved local var: uint8_t reserved_6@[???]
                       Unresolved local var: uint8_t reserved_7@[???]
                       Unresolved local var: uint16_t PCR_PID@[???]
                       Unresolved local var: int serv_loc_pos@[???]
                       Unresolved local var: int EPID@[???] */
    data[local_68] = 0;                            // short_name[0..6] = UTF-16BE "MELD300" (14 bytes)
    data[local_68 + 1] = 0x4d;                     // 'M'
    data[local_68 + 2] = 0;
    data[local_68 + 3] = 0x45;                     // 'E'
    data[local_68 + 4] = 0;
    data[local_68 + 5] = 0x4c;                     // 'L'
    data[local_68 + 6] = 0;
    data[local_68 + 7] = 0x44;                     // 'D'
    data[local_68 + 8] = 0;
    data[local_68 + 9] = 0x33;                     // '3'
    data[local_68 + 10] = 0;
    data[local_68 + 0xb] = 0x30;                   // '0'
    data[local_68 + 0xc] = 0;
    data[local_68 + 0xd] = 0x30;                   // '0'
    data[local_68 + 0xe] = (byte)(mjchannel >> 6) | 0xf0; // reserved=1111, major_channel_number[9:6]
    data[local_68 + 0xf] = (char)mjchannel << 2 | (byte)(mnchannel >> 8) & 0xf; // major[5:0] | minor[9:8]
    data[local_68 + 0x10] = (uint8_t)mnchannel;    // minor_channel_number[7:0]
    data[local_68 + 0x11] = 4;                     // modulation_mode = 4 (8-VSB)
    data[local_68 + 0x12] = 0;                     // carrier_frequency = 0 (4 bytes)
    data[local_68 + 0x13] = 0;
    data[local_68 + 0x14] = 0;
    data[local_68 + 0x15] = 0;
    data[local_68 + 0x16] = 0;                     // channel_TSID = 0x0000
    data[local_68 + 0x17] = 0;
    data[local_68 + 0x18] = 0;                     // program_number = 0x0003
    data[local_68 + 0x19] = 3;
    data[local_68 + 0x1a] = etm << 6 | 9;          // ETM_location=etm, access_ctrl=0, hidden=0, hide_guide=1, reserved=001
    data[local_68 + 0x1b] = 0xc2;                  // reserved=11, service_type=2 (ATSC Digital TV)
    data[local_68 + 0x1c] = 0;                     // source_id = 0x0003
    data[local_68 + 0x1d] = 3;
    iVar2 = local_68 + 0x1e;                       // desc_len_pos
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[local_68 + 0x1f] = (uint8_t)local_5c;
    data[local_68 + 0x20] = 0xa0;                  // descriptor_tag = 0xA0 (ATSC Extended Channel Name Descriptor)
    iVar3 = local_68 + 0x21;                       // ext_name_pos (descriptor_length)
    data[iVar3] = 0;
    data[local_68 + 0x22] = 1;                     // MSS: number_strings = 1
    local_68 = local_68 + 0x23;
    for (local_58 = 0; local_58 < 1; local_58 = local_58 + 1) {
      data[local_68] = 0x65;                       // "eng"
      data[local_68 + 1] = 0x6e;
      data[local_68 + 2] = 0x67;
      data[local_68 + 3] = 1;                      // number_segments = 1
      local_68 = local_68 + 4;
      for (local_54 = 0; local_54 < 1; local_54 = local_54 + 1) {
        data[local_68] = 0;                        // compression_type = 0
        data[local_68 + 1] = 0;                    // mode = 0
        data[local_68 + 2] = (uint8_t)ext_name_len; // number_bytes
        local_68 = local_68 + 3;
        for (local_50 = 0; local_50 < ext_name_len; local_50 = local_50 + 1) {
          data[local_68] = extended_name[local_50];
          local_68 = local_68 + 1;
        }
      }
    }
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1; // patch 0xA0 descriptor_length
    data[local_68] = 0xa1;                         // descriptor_tag = 0xA1 (ATSC Service Location Descriptor)
    iVar3 = local_68 + 1;                          // serv_loc_pos (descriptor_length)
    data[iVar3] = 0;
    data[local_68 + 2] = 0xe0;                     // reserved=111, PCR_PID = 0x0030
    data[local_68 + 3] = 0x30;
    data[local_68 + 4] = 2;                        // number_elements = 2
    data[local_68 + 5] = 2;                        // element 0: stream_type = 0x02 (MPEG-2 video)
    data[local_68 + 6] = 0xe0;                     // elementary_PID = 0x0030
    data[local_68 + 7] = 0x30;
    data[local_68 + 8] = 0x65;                     // ISO_639_language_code = "eng"
    data[local_68 + 9] = 0x6e;
    data[local_68 + 10] = 0x67;
    data[local_68 + 0xb] = 0x81;                   // element 1: stream_type = 0x81 (ATSC AC-3 audio)
    data[local_68 + 0xc] = 0xe0;                   // elementary_PID = 0x0031
    data[local_68 + 0xd] = 0x31;
    data[local_68 + 0xe] = 0x65;                   // ISO_639_language_code = "eng"
    data[local_68 + 0xf] = 0x6e;
    data[local_68 + 0x10] = 0x67;
    local_68 = local_68 + 0x11;
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1; // patch 0xA1 descriptor_length (15 bytes)
    local_5c = ((short)local_68 - (short)iVar2) + -2; // total channel descriptors_length
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[sVar1 + 0x1f] = (uint8_t)local_5c;
  }
  data[local_68] = 0xfc;                           // reserved=111111, additional_descriptors_length = 0
  data[local_68 + 1] = 0;
  local_68 = local_68 + 2;
  for (local_60 = 0; local_60 < 0; local_60 = local_60 + 1) { // dead loop
    data[local_68] = 1;
    local_68 = local_68 + 1;
  }
  data[1] = (byte)(local_68 + 1 >> 8) | 0xf0;
  data[2] = (char)local_68 + 1;
  uVar4 = crc32(0xffffffff,data,local_68);
  uVar4 = bswap_32(uVar4);
  data[local_68] = (uint8_t)(uVar4 >> 0x18);
  data[local_68 + 1] = (uint8_t)(uVar4 >> 0x10);
  data[local_68 + 2] = (uint8_t)(uVar4 >> 8);
  data[local_68 + 3] = (uint8_t)uVar4;
  return local_68 + 4;
}



// ==== FormATSC_STT @ 004082c8 ====

/*
 * FormATSC_STT(): builds an ATSC A/65 System Time Table (STT, table_id=0xCD)
 *                 section (fixed 20 bytes = 0x14).
 *   Params: data (output buffer)
 *   Returns: 20 (0x14)
 *   Callers: encoder_send_data_thread_entry (ATSC mode, PID 0x1ffb)
 *   Locals: tVar1 = time(NULL); iVar2 = GPS system_time; uVar3 = CRC32
 *   Quirk: subtracts 0x12d4db1b (315,939,611) instead of 0x12d53d70 (315,964,800)
 *          used in FormATSC_EIT, making STT system_time ~7 hours (25,189s) ahead.
 *          GPS_UTC_offset is hardcoded to 15 (0x0f).
 */
int FormATSC_STT(uint8_t *data)

{
  time_t tVar1;
  int iVar2;
  uint32_t uVar3;
  
                    /* Unresolved local var: int i@[???]
                       Unresolved local var: uint32_t crc@[???]
                       Unresolved local var: uint8_t tableid@[???]
                       Unresolved local var: uint8_t reserved_0@[???]
                       Unresolved local var: uint8_t current_next_indicator@[???]
                       Unresolved local var: uint8_t version_number@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: uint32_t gpstime@[???] */
  tVar1 = time((time_t *)0x0);
  iVar2 = tVar1 + -0x12d4db1b;                     // Unix time -> GPS seconds (offset by ~7h vs 0x12d53d70)
  *data = 0xcd;                                    // table_id = 0xCD (STT)
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;                                     // table_id_extension = 0x0000
  data[4] = 0;
  data[5] = 0xef;                                  // reserved=11, version_number=0x17, current_next=1
  data[6] = 0;                                     // section_number = 0
  data[7] = 0;                                     // last_section_number = 0
  data[8] = 0;                                     // protocol_version = 0
  data[9] = (uint8_t)((uint)iVar2 >> 0x18);        // system_time (32-bit BE GPS seconds)
  data[10] = (uint8_t)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (uint8_t)iVar2;
  data[0xd] = 0xf;                                 // GPS_UTC_offset = 15 seconds
  data[0xe] = 0;                                   // daylight_savings = 0x0000
  data[0xf] = 0;
  data[1] = 0xf0;                                  // section_length = 0x011 (17 bytes after byte 2)
  data[2] = 0x11;
  uVar3 = crc32(0xffffffff,data,0x10);
  uVar3 = bswap_32(uVar3);
  data[0x10] = (uint8_t)(uVar3 >> 0x18);
  data[0x11] = (uint8_t)(uVar3 >> 0x10);
  data[0x12] = (uint8_t)(uVar3 >> 8);
  data[0x13] = (uint8_t)uVar3;
  return 0x14;
}



// ==== genNIT @ 004086b4 ====

/*
 * genNIT(): builds a DVB-T Network Information Table (NIT actual, table_id=0x40)
 *           on PID 0x0010 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC), freq (from conf.chan_freq),
 *           bandwidth (5/6/7/8 MHz), lngt (longitude), lttd (latitude)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_4c[0..3] = network_name ("MELD")
 *     local_4c[4..8], uStack_43..41, local_40 = cell_list_descriptor (0x6c) buffer
 *     local_3c, uStack_3a..39, local_38, uStack_35, local_34 = terrestrial_delivery_system_descriptor (0x5a)
 *     local_30 = cell_extent_of_longitude (0xff)
 *     local_2c..20 = service_list_descriptor (0x41) buffer (service_id=3, type=1)
 *     dStack_1c = dvbpsi_nit_t
 *     local_68 = DVB-T bandwidth bits (0=8MHz, 1=7MHz, 2=6MHz, 3=5MHz)
 *     iVar2 = hz_freq (freq * 100000)
 *     fVar5 = scaled lat/lon; uVar4 = lttd16, uVar3 = lngt16
 */
int genNIT(uint8_t *packet,uint8_t counter,uint32_t freq,int bandwidth,float lngt,float lttd)

{
  uint3 uVar1;
  int iVar2;
  dvbpsi_t *p_dvbpsi;
  dvbpsi_nit_ts_t *p_ts;
  dvbpsi_psi_section_t *p_section;
  undefined2 uVar3;
  undefined2 uVar4;
  float fVar5;
  char local_68;
  uint8_t local_4c [9];
  undefined1 uStack_43;
  undefined1 uStack_42;
  undefined1 uStack_41;
  undefined4 local_40;
  undefined2 local_3c;
  uint8_t uStack_3a;
  undefined1 uStack_39;
  undefined1 local_38 [3];
  undefined1 uStack_35;
  undefined4 local_34;
  undefined2 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined2 local_20;
  dvbpsi_nit_t dStack_1c;
  
  local_4c[0] = 0x4d;                              // "MELD" (network_name_descriptor 0x40)
  local_4c[1] = 0x45;
  local_4c[2] = 0x4c;
  local_4c[3] = 0x44;
                    /* Unresolved local var: uint8_t[4] name@[???]
                       Unresolved local var: uint8_t[12] cell@[???]
                       Unresolved local var: uint8_t[14] td5a@[???]
                       Unresolved local var: uint8_t[14] td41@[???]
                       Unresolved local var: dvbpsi_nit_t nit@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_es@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: uint8_t bw_mod@[???]
                       Unresolved local var: uint32_t hz_freq@[???]
                       Unresolved local var: float tmp@[???]
                       Unresolved local var: uint16_t lngt16@[???]
                       Unresolved local var: uint16_t lttd16@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  local_4c[4] = 0x6c;                              // descriptor 0x6c header (unused by dvbpsi_nit_ts_descriptor_add)
  local_4c[5] = 10;
  local_4c[6] = 0;                                 // cell_id = 0x0000
  local_4c[7] = 0;
  local_40 = 0;
  local_30 = 0xff;
  local_24 = 0;
  local_20 = 0;
  local_2c = 0x3000341;                            // LE: tag=0x41, len=3, service_id=0x0003..
  local_28 = 1;                                    // service_type=1 (digital TV)
  local_68 = '\0';
  iVar2 = freq * 100000;
  if (lngt < 0.0) {
    lngt = lngt + 360.0;                           // wrap negative longitude into [0, 360)
  }
  fVar5 = (lngt / 180.0) * 32768.0;                // EN 300 468 cell_longitude (2^15 / 180)
  if (fVar5 < 2.1474836e+09) {
    uVar3 = (undefined2)(int)fVar5;
  }
  else {
    uVar3 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  if (lttd < 0.0) {
    lttd = lttd + 360.0;
  }
  fVar5 = (lttd / 90.0) * 32768.0;                 // EN 300 468 cell_latitude (2^15 / 90)
  if (fVar5 < 2.1474836e+09) {
    uVar4 = (undefined2)(int)fVar5;
  }
  else {
    uVar4 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  stack0xffffffbc =
       CONCAT13((char)uVar3,
                CONCAT12((char)((ushort)uVar3 >> 8),CONCAT11((char)uVar4,(char)((ushort)uVar4 >> 8))
                        ));
  _local_3c = CONCAT13((char)((uint)iVar2 >> 0x10),CONCAT12((char)((uint)iVar2 >> 0x18),0xb5a));
  local_38._0_2_ = CONCAT11((char)iVar2,(char)((uint)iVar2 >> 8));
  if (bandwidth == 6) {
    local_68 = '\x02';                             // 6 MHz
  }
  else if (bandwidth < 7) {
    if (bandwidth == 5) {
      local_68 = '\x03';                           // 5 MHz
    }
  }
  else if (bandwidth == 7) {
    local_68 = '\x01';                             // 7 MHz
  }
  else if (bandwidth == 8) {
    local_68 = '\0';                               // 8 MHz
  }
  uVar1 = CONCAT12(local_68 << 5,local_38._0_2_);
  _local_38 = CONCAT13(0x40,uVar1 | 0x1f0000);     // bandwidth[7:5] | priority=1, Time_Slicing=1, MPE-FEC=1, res=11
  local_34 = 0xffffff1c;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_nit_init(&dStack_1c,0x40,3,0x2114,0x17,true); // table_id=0x40, network_id=3, version=0x17, current_next=1
    dvbpsi_nit_descriptor_add(&dStack_1c,0x40,4,local_4c); // 0x40: network_name_descriptor "MELD"
    p_ts = dvbpsi_nit_ts_add(&dStack_1c,0,0x2114); // ts_id=0, orig_network_id=0x2114
    dvbpsi_nit_ts_descriptor_add(p_ts,0x5a,0xb,&uStack_3a); // 0x5a: terrestrial_delivery_system_descriptor
    dvbpsi_nit_ts_descriptor_add(p_ts,0x6c,10,local_4c + 6); // 0x6c: cell_list_descriptor
    dvbpsi_nit_ts_descriptor_add(p_ts,0x41,3,(uint8_t *)((int)&local_2c + 2)); // 0x41: service_list_descriptor
    dStack_1c.b_current_next = true;
    p_section = dvbpsi_nit_sections_generate(p_dvbpsi,&dStack_1c,0x40);
    *packet = 0x47;                                // TS header: PID 0x0010 (NIT)
    packet[1] = 0;
    packet[2] = 0x10;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_nit_empty(&dStack_1c);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genSDT @ 00408cc4 ====

/*
 * genSDT(): builds a DVB Service Description Table (SDT actual, table_id=0x42)
 *           on PID 0x0011 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC), company, service_name (fcc),
 *           sn, lttd, lngt
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_d8, local_d7, local_d6 = td48[128] (service_descriptor 0x48 payload)
 *     dStack_58 = dvbpsi_sdt_t
 *     acStack_4c = formatted service_name string "<company>,<fcc>(<sn>,<lttd>,<lngt>)"
 *     p_service = dvbpsi_sdt_service_t* (service_id=3, running_status=4)
 *     sVar1 = strlen(acStack_4c)
 */
int genSDT(uint8_t *packet,uint8_t counter,char *company,char *service_name,char *sn,float lttd,
          float lngt)

{
  dvbpsi_t *p_dvbpsi;
  dvbpsi_sdt_service_t *p_service;
  size_t sVar1;
  dvbpsi_psi_section_t *p_section;
  undefined1 local_d8;
  undefined1 local_d7;
  uint8_t local_d6 [126];
  dvbpsi_sdt_t dStack_58;
  char acStack_4c [68];
  
                    /* Unresolved local var: uint8_t[128] td48@[???]
                       Unresolved local var: dvbpsi_sdt_t sdt@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_ss@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: char[64] fullname@[???]
                       Unresolved local var: uint8_t fullnamelen@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  memset(&local_d8,0,0x80);
  local_d8 = 0x48;                                 // tag 0x48 (service_descriptor)
  local_d7 = 0xe;
  local_d6[0] = 1;                                 // service_type = 1 (digital television service)
  local_d6[1] = 4;                                 // service_provider_name_length = 4
  local_d6[2] = 0x4d;                              // "MELD"
  local_d6[3] = 0x45;
  local_d6[4] = 0x4c;
  local_d6[5] = 0x44;
  local_d6[6] = 8;                                 // default service_name_length (overwritten below)
  local_d6[7] = 0x4d;                              // default "MKVMT300" (overwritten below)
  local_d6[8] = 0x4b;
  local_d6[9] = 0x56;
  local_d6[10] = 0x4d;
  local_d6[0xb] = 0x54;
  local_d6[0xc] = 0x33;
  local_d6[0xd] = 0x30;
  local_d6[0xe] = 0x30;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_sdt_init(&dStack_58,0x42,0,0x10,false,0x2114); // table_id=0x42, ts_id=0, ver=0x10, orig_net=0x2114
    p_service = dvbpsi_sdt_service_add(&dStack_58,3,false,false,4,false); // service_id=3, running_status=4
    sprintf(acStack_4c,"%s,%s(%s,%3.6f,%3.6f)",company,service_name,sn,(double)lttd,(double)lngt);
    sVar1 = strlen(acStack_4c);
    local_d6[6] = (char)sVar1;                     // service_name_length
    memcpy(local_d6 + 7,acStack_4c,(size_t)sVar1);
    dvbpsi_sdt_service_descriptor_add(p_service,0x48,local_d6[6] + 7,local_d6);
    dStack_58.b_current_next = true;
    p_section = dvbpsi_sdt_sections_generate(p_dvbpsi,&dStack_58);
    *packet = 0x47;                                // TS header: PID 0x0011 (SDT)
    packet[1] = 0;
    packet[2] = 0x11;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_sdt_empty(&dStack_58);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genTDT @ 00409024 ====

/*
 * genTDT(): builds a DVB Time and Date Table (TDT, table_id=0x70) on PID 0x0014
 *           into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     local_1c  = current time_t; ptVar1 = gmtime(&local_1c)
 *     iVar2     = L (1 for Jan/Feb, else 0) in EN 300 468 Annex C MJD formula
 *     uVar3     = 16-bit Modified Julian Date (MJD)
 *     uVar4     = 24-bit UTC time (HH<<16 | MM<<8 | SS)
 *     dStack_30 = dvbpsi_tot_t (used for TDT with table_id=0x70)
 *   Quirk: packs HH/MM/SS as raw binary bytes rather than BCD as required by EN 300 468.
 */
int genTDT(uint8_t *packet,uint8_t counter)

{
  tm *ptVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  dvbpsi_t *p_dvbpsi;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_tot_t dStack_30;
  time_t local_1c;
  
                    /* Unresolved local var: dvbpsi_tot_t tdt@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: tm * ttx@[???]
                       Unresolved local var: int L@[???]
                       Unresolved local var: uint64_t mjd@[???]
                       Unresolved local var: uint64_t utc@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???] */
  local_1c = time((time_t *)0x0);
  ptVar1 = gmtime(&local_1c);
  if ((ptVar1->tm_mon == 0) || (ptVar1->tm_mon == 1)) {
    iVar2 = 1;
  }
  else {
    iVar2 = 0;
  }
  uVar3 = ptVar1->tm_mday + 0x3a6c + (int)((double)(ptVar1->tm_year - iVar2) * 365.25) +
          (int)((double)(ptVar1->tm_mon + 2 + iVar2 * 0xc) * 30.6001); // 0x3a6c = 14956 (MJD formula)
  uVar4 = ptVar1->tm_hour << 0x10 | ptVar1->tm_min << 8 | ptVar1->tm_sec;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_tot_init(&dStack_30,0x70,0,0x10,false,  // table_id=0x70 (TDT), version=0x10
                    CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                             uVar4 | uVar3 * 0x1000000));
    p_section = dvbpsi_tot_sections_generate(p_dvbpsi,&dStack_30);
    *packet = 0x47;                                // TS header: PID 0x0014 (TDT/TOT)
    packet[1] = 0;
    packet[2] = 0x14;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_tot_empty(&dStack_30);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
  }
  return (uint)(p_dvbpsi == (dvbpsi_t *)0x0);
}



// ==== genEIT @ 00409330 ====

/*
 * genEIT(): builds a DVB Event Information Table (EIT present/following actual,
 *           table_id=0x4E) on PID 0x0012 into a 188-byte TS packet using libdvbpsi.
 *   Params: packet (188-byte output), counter (CC)
 *   Returns: 0 on success, 1 if dvbpsi_new() fails
 *   Callers: encoder_send_data_thread_entry (when conf.modulationmode == 0, DVB-T)
 *   Locals:
 *     dStack_98  = dvbpsi_eit_t
 *     local_88   = time(NULL); ptVar2 = gmtime(&local_88)
 *     uVar3/uVar4 = MJD / UTC start time (same EN 300 468 formula as genTDT)
 *     local_84..68, auStack_82 = td4e[32] (0x4e extended_event_descriptor, "test of EIT")
 *     local_64, local_60 = td54[8] (0x54 content_descriptor, 0x09 0x20)
 *     local_5c, local_5b, local_5a = td4d[64] (0x4d short_event_descriptor, "test event-name" / "experiment")
 */
int genEIT(uint8_t *packet,uint8_t counter)

{
  dvbpsi_t *p_dvbpsi;
  int iVar1;
  tm *ptVar2;
  uint uVar3;
  uint uVar4;
  dvbpsi_eit_event_t *p_event;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_eit_t dStack_98;
  time_t local_88;
  undefined4 local_84;
  uint8_t auStack_82 [2];
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  undefined4 local_74;
  undefined4 local_70;
  undefined4 local_6c;
  undefined4 local_68;
  undefined4 local_64;
  undefined4 local_60;
  undefined1 local_5c;
  undefined1 local_5b;
  uint8_t local_5a [66];
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_section1@[???]
                       Unresolved local var: dvbpsi_eit_t eit@[???]
                       Unresolved local var: dvbpsi_eit_event_t * et@[???]
                       Unresolved local var: dvbpsi_t * p_dvbpsi@[???]
                       Unresolved local var: time_t tt@[???]
                       Unresolved local var: tm * ttx@[???]
                       Unresolved local var: int L@[???]
                       Unresolved local var: uint64_t mjd@[???]
                       Unresolved local var: uint64_t utc@[???]
                       Unresolved local var: uint8_t[32] td4e@[???]
                       Unresolved local var: uint8_t[8] td54@[???]
                       Unresolved local var: uint8_t[64] td4d@[???] */
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
    iVar1 = 1;
  }
  else {
    local_88 = time((time_t *)0x0);
    ptVar2 = gmtime(&local_88);
    if ((ptVar2->tm_mon == 0) || (ptVar2->tm_mon == 1)) {
      iVar1 = 1;
    }
    else {
      iVar1 = 0;
    }
    uVar3 = ptVar2->tm_mday + 0x3a6c + (int)((double)(ptVar2->tm_year - iVar1) * 365.25) +
            (int)((double)(ptVar2->tm_mon + 2 + iVar1 * 0xc) * 30.6001);
    uVar4 = ptVar2->tm_hour << 0x10 | ptVar2->tm_min << 8 | ptVar2->tm_sec;
    local_70 = 0;
    local_6c = 0;
    local_68 = 0;
    local_84 = 0x6500114e;                         // LE: tag=0x4e, len=0x11, desc_num/last=0x00, "e.."
    local_80 = 0xb00756e;                          // "nu", items_len=0, text_len=11
    local_7c = 0x74736574;                         // "test"
    local_78 = 0x20666f20;                         // " of "
    local_74 = 0x544945;                           // "EIT\0"
    local_60 = 0;
    local_64 = 0x920254;                           // LE: tag=0x54, len=2, content_nibble=0x09, user=0x20
    memset(&local_5c,0,0x40);
    local_5c = 0x4d;                               // tag 0x4d (short_event_descriptor), len=0x1e
    local_5b = 0x1e;
    local_5a[0] = 0x65;                            // "enu"
    local_5a[1] = 0x6e;
    local_5a[2] = 0x75;
    local_5a[3] = 0xf;                             // event_name_length = 15
    local_5a[4] = 0x74;                            // "test event-name"
    local_5a[5] = 0x65;
    local_5a[6] = 0x73;
    local_5a[7] = 0x74;
    local_5a[8] = 0x20;
    local_5a[9] = 0x65;
    local_5a[10] = 0x76;
    local_5a[0xb] = 0x65;
    local_5a[0xc] = 0x6e;
    local_5a[0xd] = 0x74;
    local_5a[0xe] = 0x2d;
    local_5a[0xf] = 0x6e;
    local_5a[0x10] = 0x61;
    local_5a[0x11] = 0x6d;
    local_5a[0x12] = 0x65;
    local_5a[0x13] = 10;                           // text_length = 10
    local_5a[0x14] = 0x65;                         // "experiment"
    local_5a[0x15] = 0x78;
    local_5a[0x16] = 0x70;
    local_5a[0x17] = 0x65;
    local_5a[0x18] = 0x72;
    local_5a[0x19] = 0x69;
    local_5a[0x1a] = 0x6d;
    local_5a[0x1b] = 0x65;
    local_5a[0x1c] = 0x6e;
    local_5a[0x1d] = 0x74;
    dvbpsi_eit_init(&dStack_98,0x4e,3,0x10,false,0,0x2114,0,0); // table_id=0x4e, service_id=3, ver=0x10
    p_event = dvbpsi_eit_event_add
                        (&dStack_98,0,
                         CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                                  uVar4 | uVar3 * 0x1000000),0x5dc,0,false,0x32);
    dvbpsi_eit_event_descriptor_add(p_event,0x4e,0x11,auStack_82);
    dvbpsi_eit_event_descriptor_add(p_event,0x54,2,(uint8_t *)((int)&local_64 + 2));
    dvbpsi_eit_event_descriptor_add(p_event,0x4d,0x1e,local_5a);
    p_section = dvbpsi_eit_sections_generate(p_dvbpsi,&dStack_98,0x4e);
    *packet = 0x47;                                // TS header: PID 0x0012 (EIT)
    packet[1] = 0;
    packet[2] = 0x12;
    packet[3] = counter & 0xf | 0x10;
    writePSI(packet,p_section);
    dvbpsi_eit_empty(&dStack_98);
    dvbpsi_DeletePSISections(p_section);
    dvbpsi_delete(p_dvbpsi);
    iVar1 = 0;
  }
  return iVar1;
}



// ==== encoder_recv_video_thread_entry @ 00409900 ====

/*
 * encoder_recv_video_thread_entry(): pthread entry for receiving encoded MPEG-TS
 *                                    DMA buffers from /dev/vixs/xcodedrv and
 *                                    forwarding them to UDP (-m UDP) or capture
 *                                    file (-m FILE / -f / -F). Not started in
 *                                    DVO RF-modulator mode (-m DVO).
 *   Params: arg (util_thread_arg_t* -> {pThread, pEnCoder})
 *   Returns: NULL
 *   Callers: launched by launch_encoder_recv_thead()
 *   Callees: util_thread_notify_ready, vixs_encoder_recv_stream, util_sendto, write
 *   Locals:
 *     pEnCoder = (vixs_encoder_t*)((util_thread_arg_t*)arg)->arg
 *     mThread  = ((util_thread_arg_t*)arg)->pThread
 *     local_50 = txLen (bytes sent over UDP on this iteration)
 *     local_34 = bufSize (bytes received from driver)
 *     vStack_30 = vixs_xcode_recv_strm_info_t (driver recv metadata/status)
 *     uVar2    = curBufIdx (ioctl_sdev_recv.dmabuffer_idx)
 *     rVar1    = return_code_t scratch
 *     piVar3, param3 = errno / strerror() on UDP send failure
 */
void * encoder_recv_video_thread_entry(void *arg)

{
  size_t param2;
  vixs_encoder_t *pEnCoder;
  util_thread_t *mThread;
  return_code_t rVar1;
  uint32 uVar2;
  int *piVar3;
  char *param3;
  uint local_50;
  size_t local_34;
  vixs_xcode_recv_strm_info_t vStack_30;
  
                    /* Unresolved local var: util_thread_arg_t * tArg@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: util_thread_t * mThread@[???]
                       Unresolved local var: int bufSize@[???]
                       Unresolved local var: int consumedIdx@[???]
                       Unresolved local var: int curBufIdx@[???]
                       Unresolved local var: vixs_xcode_recv_strm_info_t recvInfo@[???]
                       Unresolved local var: int txLen@[???]
                       Unresolved local var: return_code_t rc@[???] */
  pEnCoder = *(vixs_encoder_t **)((int)arg + 4);
  mThread = *(util_thread_t **)arg;
  local_50 = 0;
  util_thread_notify_ready(mThread,RET_OK);
  printf("%s:Enter Encoder %x recv thread Entry \n","encoder_recv_video_thread_entry",pEnCoder->idx)
  ;
  while ((pEnCoder->stopRecv == 0 && (mThread->toExit == 0))) {
    rVar1 = vixs_encoder_recv_stream
                      (pEnCoder,(int *)&local_34,pEnCoder->lastConsumedRecvBufIdx,&vStack_30);
    if (rVar1 == RET_OK) {
      if (((uint)vStack_30.result & 0x1000) != 0) { // bit 12 = EOS / last frame flag
        printf("%s:Received last frame\n","encoder_recv_video_thread_entry");
        pEnCoder->stopRecv = 1;
        pEnCoder->stopEncode = 1;
      }
      uVar2 = (pEnCoder->ioctl_sdev_recv).dmabuffer_idx;
      if (pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_UDP) {
        if (pEnCoder->dstSock != 0) {
          rVar1 = util_sendto(pEnCoder->dstSock,pEnCoder->pRecvBuf[uVar2],local_34,0,
                              (sockaddr *)&pEnCoder->dstSockAddr,0x10);
          param2 = local_34;
          if (rVar1 == RET_OK) {
            local_50 = local_34;
          }
          else {
            piVar3 = __errno_location();
            param3 = strerror(*piVar3);
            printf("%s:Send error Payload %x: %s\n","encoder_recv_video_thread_entry",param2,param3)
            ;
            local_50 = 0;
            pEnCoder->totalUdpOutError = pEnCoder->totalUdpOutError + 1;
          }
        }
      }
      else if (pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_FILE) {
        write(pEnCoder->captureOutFileFd,pEnCoder->pRecvBuf[uVar2],local_34);
      }
      pEnCoder->totalOutputSize = pEnCoder->totalOutputSize + local_50;
      pEnCoder->lastConsumedRecvBufIdx = uVar2;
    }
    else {
      printf("%s:Xcoder %x Receive from AV driver error\n","encoder_recv_video_thread_entry",
             pEnCoder->idx);
      if ((((vStack_30.returnValue == 0x1b) || (vStack_30.returnValue == 0x1e)) ||
          (vStack_30.returnValue == 5)) || (vStack_30.returnValue == 0x24)) {
        pEnCoder->stopRecv = 1;                    // fatal driver error codes: stop immediately
      }
      else {
        pEnCoder->recvTimeoutCnt = pEnCoder->recvTimeoutCnt - 1;
        if (pEnCoder->recvTimeoutCnt == 0) {
          pEnCoder->stopRecv = 1;                  // exhausted retry/timeout budget (initially 10)
        }
      }
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_recv_video_thread_entry");
  return (void *)0x0;
}



// ==== terminate_encoder_recv_thread @ 00409d30 ====

/*
 * terminate_encoder_recv_thread(): signals and joins recvVideoThread.
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (error rollback), xcode4_stop_encoder
 */
return_code_t terminate_encoder_recv_thread(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_destroy_thread(&pEnCoder->recvVideoThread);
  if (param2 != RET_OK) {
    printf("%s:Fail to terminate encode recv thread, return value %x\n",
           "terminate_encoder_recv_thread",param2);
  }
  return param2;
}



// ==== launch_encoder_recv_thead @ 00409dc8 ====

/*
 * launch_encoder_recv_thead(): spawns recvVideoThread (note original "thead" typo).
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (when outputMode is UDP or FILE)
 */
return_code_t launch_encoder_recv_thead(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_create_thread(&pEnCoder->recvVideoThread,encoder_recv_video_thread_entry,pEnCoder);
  if (param2 != RET_OK) {
    printf("%s:Fail to create encode recv thread, return value %x\n","launch_encoder_recv_thead",
           param2);
  }
  return param2;
}



// ==== readCfgFile @ 00409e6c ====

/*
 * readCfgFile(): parses /etc/config/queryInfo key=value lines into fileconf *conf.
 *   Params: pth (path, "/etc/config/queryInfo"), conf (output fileconf struct)
 *   Returns: 0 on success, -1 if fopen64() fails (defaults still populated)
 *   Callers: encoder_send_data_thread_entry
 *   Defaults: modulationmode=1 (ATSC), bandwidth=0, chan_freq=27 (0x1b),
 *             txpower=0x35, dacgain=0xab, fwversion=" v0.034 Tx v2",
 *             fcc="OKVMT300", sn="00000467".
 *   Locals:
 *     __stream = FILE*; local_14 = getline() buffer (not freed); local_10[0] = linesize
 *     local_1c = result (0 or -1); sVar2 = string length; lVar3 = strtol() result
 *   Quirk: DVB-T bandwidth check tests local_10[0] (allocated getline capacity)
 *          rather than _Var4 (line length) before reading local_14[0x19].
 */
int readCfgFile(char *pth,fileconf *conf)

{
  FILE *__stream;
  int iVar1;
  size_t sVar2;
  long lVar3;
  __ssize_t _Var4;
  int local_1c;
  char *local_14;
  size_t local_10 [2];
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: size_t size@[???]
                       Unresolved local var: int result@[???] */
  local_10[0] = 0;
  local_1c = 0;
  __stream = fopen64(pth,"r");
  conf->modulationmode = 1;                        // default: ATSC
  conf->bandwidth = 0;
  conf->chan_freq = 0x1b;                          // default channel: 27
  conf->txpower = 0x35;
  conf->dacgain = 0xab;
  memcpy(conf," v0.034 Tx v2",0xe);
  memcpy(conf->fcc,"OKVMT300",9);
  memcpy(conf->sn,"00000467",9);
  if (__stream == (FILE *)0x0) {
    local_1c = -1;
  }
  else {
    while( true ) {
      _Var4 = getline(&local_14,local_10,__stream);
      if (_Var4 == -1) break;
      iVar1 = strncmp(local_14,"Modulation mode = ",0x12);
      if (iVar1 == 0) {
        conf->bandwidth = 0;
        iVar1 = strncmp(local_14 + 0x12,"ATSC",4);
        if (iVar1 == 0) {
          conf->modulationmode = 1;
        }
        else {
          iVar1 = strncmp(local_14 + 0x12,"DVB-T",5);
          if (iVar1 == 0) {
            conf->modulationmode = 0;
            if (local_10[0] < 0x1a) {
              conf->bandwidth = 8;
            }
            else {
              conf->bandwidth = local_14[0x19] + -0x30; // e.g. "DVB-T 8MHz" -> 8
            }
          }
          else {
            conf->modulationmode = 2;
          }
        }
      }
      iVar1 = strncmp(local_14,"MT300-AV Firmware version = ",0x1c);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 0x1d;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf,local_14 + 0x1c,sVar2);
      }
      iVar1 = strncmp(local_14,"S/N = ",6);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 7;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf->sn,local_14 + 6,sVar2);
      }
      iVar1 = strncmp(local_14,"FCC ID = ",9);
      if (iVar1 == 0) {
        sVar2 = _Var4 - 10;
        if (0x10 < sVar2) {
          sVar2 = 0x10;
        }
        memcpy(conf->fcc,local_14 + 9,sVar2);
      }
      iVar1 = strncmp(local_14,"Channel = ",10);
      if (iVar1 == 0) {
        iVar1 = atoi(local_14 + 10);
        conf->chan_freq = iVar1;
      }
      iVar1 = strncmp(local_14,"Frequency = ",0xc);
      if (iVar1 == 0) {
        iVar1 = atoi(local_14 + 0xc);
        conf->chan_freq = iVar1;
      }
      iVar1 = strncmp(local_14,"TX power = ",0xb);
      if (iVar1 == 0) {
        lVar3 = strtol(local_14 + 0xb,(char **)0x0,0x10);
        conf->txpower = lVar3;
      }
      iVar1 = strncmp(local_14,"DAC gain = ",0xb);
      if (iVar1 == 0) {
        lVar3 = strtol(local_14 + 0xb,(char **)0x0,0x10);
        conf->dacgain = lVar3;
      }
    }
    fclose(__stream);
  }
  return local_1c;
}



// ==== readSbiFile @ 0040a3f8 ====

/*
 * readSbiFile(): parses /etc/config/sbi (UCI config) for GPS coordinates.
 *   Params: pth ("/etc/config/sbi"), lngt (out longitude), lttd (out latitude)
 *   Returns: 0 on success, -1 on open failure (outputs default to 0.0)
 *   Callers: encoder_send_data_thread_entry
 *   Locals: __stream = FILE*; local_10 = getline() buffer; local_c = linesize;
 *           local_18 = result (0 or -1)
 */
int readSbiFile(char *pth,float *lngt,float *lttd)

{
  FILE *__stream;
  int iVar1;
  __ssize_t _Var2;
  float fVar3;
  int local_18;
  char *local_10;
  size_t local_c;
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: int result@[???] */
  local_c = 0;
  local_18 = 0;
  __stream = fopen64(pth,"r");
  *lngt = 0.0;
  *lttd = 0.0;
  if (__stream == (FILE *)0x0) {
    local_18 = -1;
  }
  else {
    while( true ) {
      _Var2 = getline(&local_10,&local_c,__stream);
      if (_Var2 == -1) break;
      iVar1 = strncmp(local_10,"    option \'Longitude\' \'",0x18);
      if (iVar1 == 0) {
        fVar3 = strtof(local_10 + 0x18,(char **)0x0);
        *lngt = fVar3;
      }
      iVar1 = strncmp(local_10,"    option \'Latitude\' \'",0x17);
      if (iVar1 == 0) {
        fVar3 = strtof(local_10 + 0x17,(char **)0x0);
        *lttd = fVar3;
      }
    }
    fclose(__stream);
  }
  return local_18;
}



// ==== readCompanyFile @ 0040a5ac ====

/*
 * readCompanyFile(): reads the first line of /etc/config/company (max 32 chars).
 *   Params: pth ("/etc/config/company"), company_name (out buffer >= 32 bytes)
 *   Returns: 32 (0x20) on success, -1 on open failure (writes "Meldtech")
 *   Callers: encoder_send_data_thread_entry
 *   Locals: __stream = FILE*; local_10 = getline() buffer; local_c = linesize; __n = copy len
 */
int readCompanyFile(char *pth,char *company_name)

{
  FILE *__stream;
  int iVar1;
  __ssize_t _Var2;
  size_t __n;
  char *local_10;
  size_t local_c;
  
                    /* Unresolved local var: FILE * config@[???]
                       Unresolved local var: char * line@[???]
                       Unresolved local var: size_t linesize@[???]
                       Unresolved local var: size_t size@[???] */
  local_c = 0;
  __stream = fopen64(pth,"r");
  if (__stream == (FILE *)0x0) {
    memcpy(company_name,"Meldtech",9);
    iVar1 = -1;
  }
  else {
    _Var2 = getline(&local_10,&local_c,__stream);
    if (_Var2 != 0) {
      __n = _Var2 - 1;                             // strip trailing newline
      if (0x20 < __n) {
        __n = 0x20;
      }
      memcpy(company_name,local_10,__n);
    }
    fclose(__stream);
    iVar1 = 0x20;
  }
  return iVar1;
}



// ==== encoder_send_data_thread_entry @ 0040a6ec ====

/*
 * encoder_send_data_thread_entry(): pthread entry that periodically builds and
 *                                   injects ATSC PSIP (11 TS packets, 2068 B) or
 *                                   DVB-T SI (4 TS packets, 752 B) tables into
 *                                   the XCode4 TS muxer via hXcodeSendData.
 *   Params: arg (util_thread_arg_t* -> {pThread, pEnCoder})
 *   Returns: NULL
 *   Callers: launched by launch_encoder_send_data_thead() when -g is set
 *   MT300 behaviour (PSIP virtual-channel latch):
 *     /etc/config/queryInfo, /etc/config/sbi, and /etc/config/company are read
 *     ONCE at thread startup before the while loop. Consequently, conf.chan_freq
 *     (used as the PSIP TVCT major_channel_number) latches the channel present
 *     when av_streamer starts and never updates if whitespaceagent later changes
 *     the RF channel.
 *   Locals:
 *     pEnCoder   = (vixs_encoder_t*)((util_thread_arg_t*)arg)->arg
 *     mThread    = ((util_thread_arg_t*)arg)->pThread
 *     local_128  = psicounter (CC for 0x1ffb / DVB SI)
 *     local_127  = ettcounter (CC for ETT PIDs 0x1100..0x1103)
 *     local_126  = eitcounter (CC for EIT PIDs 0x1000..0x1003)
 *     local_124  = kk (0xff TS-packet stuffing loop index)
 *     local_e8   = bufSize (pEnCoder->sendDataSize)
 *     fStack_e4  = fileconf parsed from /etc/config/queryInfo
 *     local_a0   = lngt (longitude); local_9c = lttd (latitude)
 *     acStack_98 = company[32]
 *     local_78   = eitpid[4] = {0x1000, 0x1001, 0x1002, 0x1003}
 *     local_70..64 = eitlen[4]
 *     local_60   = ettpid[4] = {0x1100, 0x1101, 0x1102, 0x1103}
 *     local_58..4c = ettlen[4]
 *     acStack_48 = extended_name[64] ("<company>,<fcc>(<sn>,<lngt>, <lttd>)")
 *     ext_name_len = strlen(acStack_48)
 *     dat8       = (uint8_t*)pEnCoder->pSendDataBuf
 *     vctlen     = TVCT section length
 */
void * encoder_send_data_thread_entry(void *arg)

{
  vixs_encoder_t *pEnCoder;
  util_thread_t *mThread;
  size_t ext_name_len;
  uint8_t *dat8;
  uint32_t vctlen;
  int iVar1;
  return_code_t rVar2;
  uint8_t local_128;
  uint8_t local_127;
  uint8_t local_126;
  uint local_124;
  uint32 local_e8;
  fileconf fStack_e4;
  float local_a0;
  float local_9c;
  char acStack_98 [32];
  uint16_t local_78 [4];
  uint32_t local_70;
  int local_6c;
  int local_68;
  int local_64;
  uint16_t local_60 [4];
  uint32_t local_58;
  int local_54;
  int local_50;
  int local_4c;
  char acStack_48 [64];
  
                    /* Unresolved local var: util_thread_arg_t * tArg@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: util_thread_t * mThread@[???]
                       Unresolved local var: int bufSize@[???]
                       Unresolved local var: uint64 flag@[???]
                       Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: uint8_t psicounter@[???]
                       Unresolved local var: uint8_t ettcounter@[???]
                       Unresolved local var: uint8_t eitcounter@[???]
                       Unresolved local var: fileconf conf@[???]
                       Unresolved local var: float lngt@[???]
                       Unresolved local var: float lttd@[???]
                       Unresolved local var: char[32] company@[???]
                       Unresolved local var: uint32_t timeoffset@[???]
                       Unresolved local var: uint16_t pid@[???]
                       Unresolved local var: uint16_t[4] eitpid@[???]
                       Unresolved local var: uint32_t[4] eitlen@[???]
                       Unresolved local var: uint16_t[4] ettpid@[???]
                       Unresolved local var: uint32_t[4] ettlen@[???]
                       Unresolved local var: uint8_t eitcnt@[???]
                       Unresolved local var: uint8_t ettcnt@[???]
                       Unresolved local var: uint32_t vctlen@[???]
                       Unresolved local var: uint8_t * dat8@[???]
                       Unresolved local var: uint32_t start_pos@[???]
                       Unresolved local var: int len@[???]
                       Unresolved local var: int kk@[???]
                       Unresolved local var: char[64] extended_name@[???]
                       Unresolved local var: int ext_name_len@[???] */
  pEnCoder = *(vixs_encoder_t **)((int)arg + 4);
  mThread = *(util_thread_t **)arg;
  local_128 = 0;
  local_127 = 0;
  local_126 = 0;
  local_78[0] = 0x1000;
  local_78[1] = 0x1001;
  local_78[2] = 0x1002;
  local_78[3] = 0x1003;
  local_70 = 0;
  local_6c = 0;
  local_68 = 0;
  local_64 = 0;
  local_60[0] = 0x1100;
  local_60[1] = 0x1101;
  local_60[2] = 0x1102;
  local_60[3] = 0x1103;
  local_58 = 0;
  local_54 = 0;
  local_50 = 0;
  local_4c = 0;
  util_thread_notify_ready(mThread,RET_OK);
  memset(&fStack_e4,0,0x44);
  memset(acStack_98,0,0x20);
  readCfgFile("/etc/config/queryInfo",&fStack_e4); // latched once at thread start!
  readSbiFile("/etc/config/sbi",&local_a0,&local_9c);
  readCompanyFile("/etc/config/company",acStack_98);
  sprintf(acStack_48,"%s,%s(%s,%3.6f, %3.6f)",acStack_98,fStack_e4.fcc,fStack_e4.sn,(double)local_a0
          ,(double)local_9c);
  ext_name_len = strlen(acStack_48);
  printf("%s:Enter Encoder %x send data thread Entry \n","encoder_send_data_thread_entry",
         pEnCoder->idx);
  while (mThread->toExit == 0) {
    local_e8 = pEnCoder->sendDataSize;
    if (fStack_e4.modulationmode == 0) {           // DVB-T SI: 4 TS packets (NIT, SDT, TDT, EIT)
      genNIT((uint8_t *)pEnCoder->pSendDataBuf,local_128,fStack_e4.chan_freq,fStack_e4.bandwidth,
             local_a0,local_9c);
      genSDT((uint8_t *)(pEnCoder->pSendDataBuf + 0xbc),local_128,acStack_98,fStack_e4.fcc,
             fStack_e4.sn,local_a0,local_9c);
      genTDT((uint8_t *)(pEnCoder->pSendDataBuf + 0x178),local_128);
      genEIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x234),local_128);
    }
    else if (fStack_e4.modulationmode == 1) {      // ATSC PSIP: 11 TS packets (TVCT, 4xEIT, 4xETT, MGT, STT)
      dat8 = (uint8_t *)pEnCoder->pSendDataBuf;
      FormTSHeader(dat8,0x1ffb,local_128);         // Packet 0 (+0x000): TVCT on PID 0x1ffb
      vctlen = FormATSC_TVCT((uint8_t *)(pEnCoder->pSendDataBuf + 5),(uint16_t)fStack_e4.chan_freq,
                             sti_minor,acStack_48,ext_name_len,0);
      for (local_124 = vctlen + 5; local_124 < 0xbc; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0xbc,local_78[0],local_126); // Packet 1 (+0x0bc): EIT-0 on PID 0x1000
      local_70 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0xc1),acStack_48,ext_name_len,0,
                              10,1);
      for (local_124 = local_70 + 0xc1; local_124 < 0x178; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x178,local_78[1],local_126); // Packet 2 (+0x178): EIT-1 on PID 0x1001 (+6h)
      local_6c = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x17d),acStack_48,ext_name_len,
                              0x5460,0x14,1);
      for (local_124 = local_6c + 0x17d; local_124 < 0x234; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x234,local_78[2],local_126); // Packet 3 (+0x234): EIT-2 on PID 0x1002 (+12h)
      local_68 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x239),acStack_48,ext_name_len,
                              0xa8c0,0x1e,1);
      for (local_124 = local_68 + 0x239; local_124 < 0x2f0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x2f0,local_78[3],local_126); // Packet 4 (+0x2f0): EIT-3 on PID 0x1003 (+18h)
      local_64 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x2f5),acStack_48,ext_name_len,
                              0xfd20,0x28,1);
      for (local_124 = local_64 + 0x2f5; local_124 < 0x3ac; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_126 = local_126 + 1;
      FormTSHeader(dat8 + 0x3ac,local_60[0],local_127); // Packet 5 (+0x3ac): ETT-0 on PID 0x1100
      local_58 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x3b1),acStack_48,ext_name_len,3,
                              10);
      for (local_124 = local_58 + 0x3b1; local_124 < 0x468; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x468,local_60[1],local_127); // Packet 6 (+0x468): ETT-1 on PID 0x1101
      local_54 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x46d),acStack_48,ext_name_len,3,
                              0x14);
      for (local_124 = local_54 + 0x46d; local_124 < 0x524; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x524,local_60[2],local_127); // Packet 7 (+0x524): ETT-2 on PID 0x1102
      local_50 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x529),acStack_48,ext_name_len,3,
                              0x1e);
      for (local_124 = local_50 + 0x529; local_124 < 0x5e0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x5e0,local_60[3],local_127); // Packet 8 (+0x5e0): ETT-3 on PID 0x1103
      local_4c = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x5e5),acStack_48,ext_name_len,3,
                              0x28);
      for (local_124 = local_4c + 0x5e5; local_124 < 0x69c; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_127 = local_127 + 1;
      FormTSHeader(dat8 + 0x69c,0x1ffb,local_128 + 1); // Packet 9 (+0x69c): MGT on PID 0x1ffb
      iVar1 = FormATSC_MGT((uint8_t *)(pEnCoder->pSendDataBuf + 0x6a1),vctlen,&local_70,local_78,4,
                           &local_58,local_60,4);
      for (local_124 = iVar1 + 0x6a1; local_124 < 0x758; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_128 = local_128 + 2;
      FormTSHeader(dat8 + 0x758,0x1ffb,local_128); // Packet 10 (+0x758): STT on PID 0x1ffb
      iVar1 = FormATSC_STT((uint8_t *)(pEnCoder->pSendDataBuf + 0x75d));
      for (local_124 = iVar1 + 0x75d; local_124 < 0x814; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
    }
    local_128 = local_128 + 1;
    rVar2 = vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8000000); // IOCTLSDEVSEND (flags=0x8000000)
    if (rVar2 != RET_OK) {
      printf("%s:Xcoder %x Send data to AV driver error\n","encoder_send_data_thread_entry",
             pEnCoder->idx);
      mThread->toExit = 1;
    }
    if (pEnCoder->sendDataInterval == 0) {
      mThread->toExit = 1;
    }
    else {
      usleep(pEnCoder->sendDataInterval * 1000);   // -t <ms> (e.g. 1000 ms in avs_start.sh)
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_send_data_thread_entry");
  local_e8 = 0;
  vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8400000); // final 0-byte EOS/flush send
  return (void *)0x0;
}



// ==== terminate_encoder_send_data_thread @ 0040b57c ====

/*
 * terminate_encoder_send_data_thread(): signals and joins sendDataThread.
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (error rollback), xcode4_stop_encoder
 */
return_code_t terminate_encoder_send_data_thread(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_destroy_thread(&pEnCoder->sendDataThread);
  if (param2 != RET_OK) {
    printf("%s:Fail to terminate encoder send data thread, return value %x\n",
           "terminate_encoder_send_data_thread",param2);
  }
  return param2;
}



// ==== launch_encoder_send_data_thead @ 0040b614 ====

/*
 * launch_encoder_send_data_thead(): spawns sendDataThread (note original "thead" typo).
 *   Params: pEnCoder
 *   Returns: RET_OK or RET_ERR
 *   Callers: xcode4_start_encoder (when pEnCoder->bSendData != 0)
 */
return_code_t launch_encoder_send_data_thead(vixs_encoder_t *pEnCoder)

{
  return_code_t param2;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  param2 = util_create_thread(&pEnCoder->sendDataThread,encoder_send_data_thread_entry,pEnCoder);
  if (param2 != RET_OK) {
    printf("%s:Fail to create encoder send data thread, return value %x\n",
           "launch_encoder_send_data_thead",param2);
  }
  return param2;
}



// ==== xcode4_open_encoder @ 0040b6b8 ====

/*
 * xcode4_open_encoder(): allocates a vixs_encoder_t (0x1980 = 6528 bytes) and
 *                        initializes it via vixs_encoder_init().
 *   Params: handle (out int* receiving vixs_encoder_t*), param (init params)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c
 */
return_code_t xcode4_open_encoder(int *handle,vixs_encoder_init_param_t *param)

{
  vixs_encoder_t *pEnCoder;
  return_code_t rVar1;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  pEnCoder = malloc(0x1980);                       // sizeof(vixs_encoder_t) = 6528
  if (pEnCoder == (vixs_encoder_t *)0x0) {
    printf("%s:No memory\n","xcode4_open_encoder");
    rVar1 = RET_ERR;
  }
  else {
    rVar1 = vixs_encoder_init(pEnCoder,param);
    if (rVar1 == RET_OK) {
      printf("%s:Init Encoder Sucess\n","xcode4_open_encoder");
      *handle = (int)pEnCoder;
    }
    else {
      printf("%s:Init Encoder Failed\n","xcode4_open_encoder");
      free(pEnCoder);
      *handle = 0;
    }
  }
  return rVar1;
}



// ==== xcode4_start_encoder @ 0040b804 ====

/*
 * xcode4_start_encoder(): opens the XCode4 encoder stream and launches the
 *                         recv (for UDP/FILE) and send-data (for -g) threads.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c
 *   Locals: local_10 = rc; handle+0x3c = outputMode; handle+0xd4 = bSendData
 */
return_code_t xcode4_start_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_start_encoder");
  if (handle == 0) {
    printf("%s:Invalid Handle\n","xcode4_start_encoder");
    local_10 = RET_ERR;
  }
  else {
    local_10 = vixs_encoder_open_stream((vixs_encoder_t *)handle);
    if (local_10 == RET_OK) {
      if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) { // outputMode: UDP(1) or FILE(3)
        local_10 = launch_encoder_recv_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Encode Thread failed\n","xcode4_start_encoder");
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
      if (*(int *)(handle + 0xd4) != 0) {          // bSendData != 0 (-g table generation)
        local_10 = launch_encoder_send_data_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Send Thread failed\n","xcode4_start_encoder");
          if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
            terminate_encoder_recv_thread((vixs_encoder_t *)handle);
          }
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
    }
    else {
      printf("%s:Open Encoder stream failed\n","xcode4_start_encoder");
    }
    printf("%s:X\n","xcode4_start_encoder");
  }
  return local_10;
}



// ==== xcode4_stop_encoder @ 0040ba60 ====

/*
 * xcode4_stop_encoder(): stops recvVideoThread (or flushes DVO stream), stops
 *                        sendDataThread if active, and closes the XCode4 stream.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK or RET_ERR (Bug: uninitialized local_10 if outputMode==DVO and bSendData==0)
 *   Callers: sample_event_callback() and main() in 00_main.c.c
 */
return_code_t xcode4_stop_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_stop_encoder");
  if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) { // outputMode: UDP(1) or FILE(3)
    local_10 = terminate_encoder_recv_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encode_thread, return value %x\n","xcode4_stop_encoder",local_10)
      ;
    }
  }
  else {
    vixs_encoder_flush_stream((vixs_encoder_t *)handle); // DVO(2): flush instead of recv thread
  }
  if (*(int *)(handle + 0xd4) != 0) {              // bSendData != 0
    local_10 = terminate_encoder_send_data_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encoder_send_data_thread, return value %x\n",
             "xcode4_stop_encoder",local_10);
    }
  }
  vixs_encoder_close_stream((vixs_encoder_t *)handle);
  printf("%s:X\n","xcode4_stop_encoder");
  return local_10;
}



// ==== xcode4_close_encoder @ 0040bbfc ====

/*
 * xcode4_close_encoder(): frees the vixs_encoder_t handle.
 *   Params: handle (vixs_encoder_t*)
 *   Returns: RET_OK
 *   Callers: main() in 00_main.c.c
 *   Note: does not invoke vixs_encoder_deinit() (defined in 02_vixs_encoder.c.c).
 */
return_code_t xcode4_close_encoder(int handle)

{
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  free((void *)handle);
  return RET_OK;
}



// ==== xcode4_update_encoder_param @ 0040bc58 ====

/*
 * xcode4_update_encoder_param(): updates encoder input/output video standard or
 *                                bitrate via vixs_encoder_update_stream().
 *   Params: handle (vixs_encoder_t*), param (vixs_encoder_update_param_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: start_encoder() in 00_main.c.c (on restart != 0)
 */
return_code_t xcode4_update_encoder_param(int handle,vixs_encoder_update_param_t *param)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  rVar1 = vixs_encoder_update_stream((vixs_encoder_t *)handle,param);
  return rVar1;
}



// ==== xcode4_get_encoder_ts_info @ 0040bcb8 ====

/*
 * xcode4_get_encoder_ts_info(): queries TS lock and output status from the
 *                               XCode4 driver and unpacks the status bitmask at
 *                               handle+0x1324 (ioctl_ts_ctrl.ioctl_param.tsinput_lock).
 *   Params: handle (vixs_encoder_t*), info (out vixs_encoder_ts_info_t*)
 *   Returns: RET_OK or RET_ERR
 *   Callers: main() in 00_main.c.c (periodic status poll)
 */
return_code_t xcode4_get_encoder_ts_info(int handle,vixs_encoder_ts_info_t *info)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: return_code_t rc@[???] */
  rVar1 = vixs_encoder_get_ts_info((vixs_encoder_t *)handle);
  if (rVar1 == RET_OK) {
    info->inputPsiLock = *(uint *)(handle + 0x1324) & 1;           // bit 0: PSI lock
    info->inputVideoLock = (uint)((*(uint *)(handle + 0x1324) & 2) != 0); // bit 1: video lock
    info->inputAudioLock = (uint)((*(uint *)(handle + 0x1324) & 4) != 0); // bit 2: audio lock
    info->inputPCRLock = (uint)((*(uint *)(handle + 0x1324) & 8) != 0);   // bit 3: PCR lock
    info->inputVideoCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x10) != 0); // bit 4: video CC error
    info->inputAudioCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x20) != 0); // bit 5: audio CC error
    info->tsOutput = (uint)((*(uint *)(handle + 0x1324) & 0x40) == 0);    // bit 6 (inverted): TS output active
    info->videoOutput = (uint)((*(uint *)(handle + 0x1324) & 0x80) == 0); // bit 7 (inverted): video output active
    info->audioOutput = (uint)((*(uint *)(handle + 0x1324) & 0x100) == 0); // bit 8 (inverted): audio output active
  }
  return rVar1;
}




/* #####################################################################
 * FILE: 02_vixs_encoder.c.c
 * ##################################################################### */

/*
 * 02_vixs_encoder.c.c  --  annotated decompilation of vixs_encoder.c
 * =====================================================================
 *
 * Original source file:
 *   /media/WD_passport/Meld_AV_Script/2p12/avstreamer/src/libxcode4encoder/vixs_encoder.c
 * Compiler: GNU C 4.5.3 (ELF32 MIPS32r2 little-endian, full DWARF)
 *
 * Role in av_streamer:
 *   Low-level ViXS XCode4 hardware encoder driver interface. Unlike xcodeapi.cpp
 *   (07_xcodeapi.cpp.c), this unit does NOT call the xcode_* helper wrappers;
 *   instead it populates IOCTLSDEV* structures embedded directly inside
 *   vixs_encoder_t and issues ioctl(fd, 0x17241724, ...) directly against
 *   /dev/vixs/xcodedrv.
 *
 *   All functions in this unit are called exclusively from 01_libxcode4encoder.c.c
 *   (xcode4_open_encoder, xcode4_start_encoder, xcode4_stop_encoder,
 *   xcode4_update_encoder_param, xcode4_get_encoder_ts_info,
 *   encoder_recv_video_thread_entry, encoder_send_data_thread_entry), except
 *   debug_ioctl_encoder and vixs_encoder_deinit which are unused (dead code).
 *
 * Main data structures (DWARF):
 *   - vixs_encoder_t (size 0x1980 = 6528 bytes):
 *       +0x0000 idx, bActive, stopEncode, stopRecv, recvTimeoutCnt, recvBufferSize
 *       +0x0018 lastConsumedRecvBufIdx, totalOutputFrameNum, totalOutputSize
 *       +0x0024 startTime (timeval), +0x0030 strm_handle_encoder (uint64)
 *       +0x0038 vidCodec, +0x003c outputMode, +0x0040 recvBufNum
 *       +0x0044 pRecvBuf[16], +0x0084 pRecvBufPhy[16]
 *       +0x00c4 pSendDataBuf, +0x00c8 pSendDataBufPhy
 *       +0x00cc sendDataInterval, +0x00d0 sendDataSize, +0x00d4 bSendData
 *       +0x00d8 ioctl_sdev_open  (IOCTLSDEVOPENCAPTURE, size 0x9e8 = 2536)
 *       +0x0ac0 ioctl_sdev_close (IOCTLSDEVCLOSE,       size 0x0b4 = 180)
 *       +0x0b74 ioctl_sdev_ctrl  (IOCTLSDEVCTRL,        size 0x4a8 = 1192)
 *       +0x101c ioctl_sdev_recv  (IOCTLSDEVRECV,        size 0x240 = 576)
 *       +0x125c ioctl_ts_ctrl    (IOCTLSDEVTSCTRL,      size 0x450 = 1104)
 *       +0x16ac ioctl_sdev_send  (IOCTLSDEVSEND,        size 0x1d8 = 472)
 *       +0x1884 hXcode (fd), +0x1888 hXcodeRecvVideo (fd), +0x188c hXcodeSendData (fd)
 *       +0x1890 recvVideoThread (util_thread_t), +0x18f0 sendDataThread (util_thread_t)
 *       +0x1950 dstAddr, dstPort, dstTTL, dstSock, totalUdpOutError, dstSockAddr
 *       +0x1974 init_param (vixs_encoder_init_param_t *), +0x1978 captureOutFileFd
 *   - vixs_encoder_init_param_t (size 192 bytes)
 *   - vixs_encoder_update_param_t (size 16 bytes)
 *   - vixs_xcode_recv_strm_info_t (size 24 bytes)
 *
 * Globals owned: none (all state lives in caller-allocated vixs_encoder_t).
 */

/* =====================================================================
 * debug_ioctl_encoder @ 0040be00
 * =====================================================================
 * Purpose:
 *   Diagnostic helper that dumps every field of an IOCTLSDEVOPENCAPTURE
 *   ioctl_param struct to stdout.
 * Parameters:
 *   pioctl_open_strm_capture - pointer to populated IOCTLSDEVOPENCAPTURE struct.
 * Return value: void.
 * Callers: NONE (dead code; never called in av_streamer).
 * Callees: printf.
 * Threading/locking: none.
 * Side effects: writes to stdout.
 */
// ==== debug_ioctl_encoder @ 0040be00 ====

/* WARNING: Enum "xcode_video_type": Some values do not have unique names */
/* WARNING: Enum "_GOP_TYPE": Some values do not have unique names */
/* WARNING: Enum "_xcode_mpeg4_profile": Some values do not have unique names */
/* WARNING: Enum "_disp_aspect_ratio": Some values do not have unique names */
/* WARNING: Enum "_xcode_mpeg4_level": Some values do not have unique names */

void debug_ioctl_encoder(PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture)

{
  printf("%s:\nCalling Strm open CAPTURE !\n","debug_ioctl_encoder");
  printf("%s:=======  CMD_SDEV_OPEN_CAPTURE arguments  =======\n","debug_ioctl_encoder");
  printf("%s:\tmode = 0x%llX\n","debug_ioctl_encoder",(pioctl_open_strm_capture->ioctl_param).mode);
  printf("%s:\tvideo_src = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_src);
  printf("%s:\tvideo_type = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_type);
  printf("%s:\tvideo_brate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_brate);
  printf("%s:\tvideo_frate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_frate);
  printf("%s:\tvideo_in_frate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_in_frate);
  printf("%s:\tvideo_out_frate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_out_frate);
  printf("%s:\tin_disp_horz = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).in_disp_horz);
  printf("%s:\tin_disp_vert = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).in_disp_vert);
  printf("%s:\tdisp_horz = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).disp_horz);
  printf("%s:\tdisp_vert = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).disp_vert);
  printf("%s:\taudio_src = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_src);
  printf("%s:\taudio_type = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_type);
  printf("%s:\taudio_brate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_brate);
  printf("%s:\taudio_srate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_srate);
  printf("%s:\taudio_in_srate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_in_srate);
  printf("%s:\taudio_channels = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_channels);
  printf("%s:\tinit_time_stamp = 0x%llX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).init_time_stamp);
  printf("%s:\tvideo_out_pid = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_out_pid);
  printf("%s:\taudio_out_pid = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_out_pid);
  printf("%s:\tn_b = 0x%X\n","debug_ioctl_encoder",(pioctl_open_strm_capture->ioctl_param).n_b);
  printf("%s:\tvideo_src_type = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_src_type);
  printf("%s:\tstream_remux_bitrate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).stream_remux_bitrate);
  printf("%s:\tpeak_video_bitrate = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).peak_video_bitrate);
  printf("%s:\tRateContinuity = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).RateContinuity);
  printf("%s:\tinsert_vbi_data_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).insert_vbi_data_flags);
  printf("%s:\tprivate_data_size = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).private_data_size);
  printf("%s:\taudio_data_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_data_flags);
  printf("%s:\tremux_ps_enum = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_ps_enum);
  printf("%s:\tremux_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_flags);
  printf("%s:\tremux_pack_size = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_pack_size);
  printf("%s:\tprofile = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).profile);
  printf("%s:\taspect_ratio = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).aspect_ratio);
  printf("%s:\tsharpness = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).sharpness);
  printf("%s:\tvideo_in_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_in_flags);
  printf("%s:\tvideo_out_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_out_flags);
  printf("%s:\tthreshold = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).threshold);
  printf("%s:\tcgms_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).cgms_flags);
  printf("%s:\tapstb_flags = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).apstb_flags);
  printf("%s:\tepn = 0x%lX\n","debug_ioctl_encoder",(pioctl_open_strm_capture->ioctl_param).epn);
  printf("%s:\trdi_cci_ss = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).rdi_cci_ss);
  printf("%s:\tlevel = 0x%X\n","debug_ioctl_encoder",(pioctl_open_strm_capture->ioctl_param).level);
  printf("%s:\tquanttype = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).quanttype);
  printf("%s:\tmpeg4_insert_seq_header_freq = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).mpeg4_insert_seq_header_freq);
  printf("%s:\tvideo_capture_offset = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_capture_offset);
  printf("%s:\tpcr_out_id = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).pcr_out_id);
  printf("%s:\tenc_quadrant = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).enc_quadrant);
  printf("%s:\tout_next_atype = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_next_atype);
  printf("%s:\tframe_accuracy_gpio_pin = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).frame_accuracy_gpio_pin);
  printf("%s:\tout_pmt_pid = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_pmt_pid);
  printf("%s:\tfilters_enable = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).filters_enable);
  printf("%s:\taudio_src2 = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_src2);
  printf("%s:\taudio_type2 = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_type2);
  printf("%s:\taudio_brate2 = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_brate2);
  printf("%s:\taudio_srate2 = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_srate2);
  printf("%s:\taudio_in_srate2 = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_in_srate2);
  printf("%s:\taudio_channels2 = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_channels2);
  printf("%s:\taudio_out_pid2 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_out_pid2);
  printf("%s:\taudio_data_flags2 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).audio_data_flags2);
  printf("%s:\tout_next_atype2 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_next_atype2);
  printf("%s:\tdiff_psi_out_time = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).diff_psi_out_time);
  printf("%s:\tout_program_number = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_program_number);
  printf("%s:\tvfr_mode = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).vfr_mode);
  printf("%s:\tvfr_threshold = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).vfr_threshold);
  printf("%s:\tmp4_version = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).mp4_version);
  printf("%s:\tpcr_output_duration = %lu\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).pcr_output_duration);
  printf("%s:\tIEC61937_init_ts = 0x%llX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).IEC61937_init_ts);
  printf("%s:\tout_maxvbvbufferlevel = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_maxvbvbufferlevel);
  printf("%s:\tout_lang_desc_pid1 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_lang_desc_pid1);
  printf("%s:\tout_lang_desc_content1 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_lang_desc_content1);
  printf("%s:\tout_lang_desc_pid2 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_lang_desc_pid2);
  printf("%s:\tout_lang_desc_content2 = 0x%lX\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).out_lang_desc_content2);
  printf("%s:\tblizzard_syncbyte = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).blizzard_syncbyte);
  printf("%s:\tremux_max_vpes_size = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_max_vpes_size);
  printf("%s:\tremux_fixed_apes_size = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_fixed_apes_size);
  printf("%s:\tremux_fixed_dma_size = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_fixed_dma_size);
  printf("%s:\tcomb_dot_crawl_detect = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).comb_dot_crawl_detect);
  printf("%s:\tcomb_luma_motion = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).comb_luma_motion);
  printf("%s:\tcomb_chroma_motion = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).comb_chroma_motion);
  printf("%s:\tcomb_response = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).comb_response);
  printf("%s:\ttemporal_luma_motion = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).temporal_luma_motion);
  printf("%s:\ttemporal_chroma_motion = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).temporal_chroma_motion);
  printf("%s:\tspatial_luma_edge = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).spatial_luma_edge);
  printf("%s:\tspatial_chroma_edge = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).spatial_chroma_edge);
  printf("%s:\tspatial_strength = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).spatial_strength);
  printf("%s:\tscaling_detail_level = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).scaling_detail_level);
  printf("%s:\tenableLowMode = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).enableLowMode);
  printf("%s:\tcontent_key_slot_id = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).content_key_slot_id);
  printf("%s:\tavc_out_flags = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avc_out_flags);
  printf("%s:\tavcSeiCtrl = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcSeiCtrl);
  printf("%s:\tavcPVRConvergeTime = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcPVRConvergeTime);
  printf("%s:\tavcDTSDelayOffset = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcDTSDelayOffset);
  printf("%s:\tavcUserDataSeiInputSize = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcUserDataSeiInputSize);
  printf("%s:\tavcCustFormat = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcCustFormat);
  printf("%s:\tavcHrdBitRate = %d\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcHrdBitRate);
  printf("%s:\tavcCpbSize = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).avcCpbSize);
  printf("%s:\tpmt_descriptor_size = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).pmt_descriptor_size);
  printf("%s:\tpmt_descriptor_ctrl = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).pmt_descriptor_ctrl);
  printf("%s:\tHDCPoutEnabled = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).HDCPoutEnabled);
  printf("%s:\tremux_vr_flags = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).remux_vr_flags);
  printf("%s:\trdi_cci_source = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).rdi_cci_source);
  printf("%s:\tprogressiveValue = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).progressiveValue);
  printf("%s:\tvideo_cropped_horz_inclusive_region = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_cropped_horz_inclusive_region);
  printf("%s:\tvideo_cropped_vert_inclusive_region = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).video_cropped_vert_inclusive_region);
  printf("%s:\tminimum_ats_delta = 0x%X\n","debug_ioctl_encoder",
         (pioctl_open_strm_capture->ioctl_param).minimum_ats_delta);
  return;
}



/* =====================================================================
 * vixs_encoder_init @ 0040d8d4
 * =====================================================================
 * Purpose:
 *   Zero-initializes a vixs_encoder_t instance, populates the embedded
 *   IOCTLSDEVOPENCAPTURE request from `param`, opens three independent file
 *   descriptors to `/dev/vixs/xcodedrv` (control, video recv, data send),
 *   and initializes either the UDP TX socket or capture output file.
 * Parameters:
 *   pEnCoder - caller-allocated vixs_encoder_t (0x1980 = 6528 bytes)
 *   param    - initialization parameters (codecs, standards, bitrates, I/O)
 * Return value:
 *   RET_OK (0) on success (or if param->bitRate == 0, early no-op return),
 *   RET_ERR (1) on invalid video standard, driver open failure, or UDP/file
 *   setup failure (closing any already-opened driver fds on error).
 * Callers:
 *   xcode4_open_encoder (01_libxcode4encoder.c.c)
 * Callees:
 *   memset, printf, puts, open64, close, util_init_tx_udp_socket (03_util_common.c.c)
 * Threading/locking:
 *   Called single-threaded before worker threads are launched. Opens 3 fds to
 *   /dev/vixs/xcodedrv so control, recv, and send threads can issue blocking
 *   ioctls concurrently without contending on one fd.
 * Side effects:
 *   Opens /dev/vixs/xcodedrv x3 (O_RDWR); optionally creates UDP socket or
 *   opens param->captureFileName with O_CREAT|O_TRUNC|O_RDWR.
 * Quirks / bugs:
 *   - If param->bitRate == 0, returns RET_OK immediately without opening driver.
 *   - `n_b` is set to GOP_B_DEFAULT (0) and later overwritten with
 *     `GOP_4_P | GOP_2_B`.
 *   - `open64(param->captureFileName, 0x302)` uses O_CREAT without a mode arg,
 *     leaving file permissions to whatever happens to be in $a2.
 */
// ==== vixs_encoder_init @ 0040d8d4 ====

/* WARNING: Enum "_GOP_TYPE": Some values do not have unique names */
/* WARNING: Enum "xcode_video_type": Some values do not have unique names */

return_code_t vixs_encoder_init(vixs_encoder_t *pEnCoder,vixs_encoder_init_param_t *param)

{
  int iVar1;                       // scratch return value from open64()
  return_code_t rVar2;             // return value from util_init_tx_udp_socket()
  uint uVar3;                      // upper 32 bits of 64-bit ioctl_param.mode
  undefined4 uVar4;                // upper 32 bits of 64-bit ioctl_param.mode (DVO branch)
  
                    /* Unresolved local var: PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture@[???] */
  memset(pEnCoder,0,0x1980);       // sizeof(vixs_encoder_t) = 6528 bytes
  pEnCoder->bActive = 0;
  pEnCoder->stopEncode = 0;
  pEnCoder->stopRecv = 0;
  pEnCoder->recvTimeoutCnt = 10;   // allow up to 10 consecutive recv errors before stopping
  pEnCoder->init_param = param;
  if (param->bitRate == 0) {
    return RET_OK;
  }
  // Never upscale: clamp outVideoStandard <= inVideoStandard (enum values increase with resolution)
  if (param->inVideoStandard < param->outVideoStandard) {
    param->outVideoStandard = param->inVideoStandard;
  }
  // param->flag bit 0: enable PSIP/SI table or user data insertion via shared DMA buffer
  if ((param->flag & 1) != 0) {
    pEnCoder->bSendData = 1;
    pEnCoder->sendDataSize = (param->data_insertion_param).data_size;
    // Align down to whole 188-byte (0xbc) MPEG-TS packets, capped at 64 KiB (0x10000)
    pEnCoder->sendDataSize = (pEnCoder->sendDataSize / 0xbc) * 0xbc;
    if (pEnCoder->sendDataSize == 0) {
      pEnCoder->bSendData = 0;
    }
    else {
      pEnCoder->sendDataInterval = (param->data_insertion_param).interval;
      if (0x10000 < pEnCoder->sendDataSize) {
        pEnCoder->sendDataSize = 0x10000;
      }
    }
  }
  memset(&pEnCoder->ioctl_sdev_open,0,0x9e8); // sizeof(IOCTLSDEVOPENCAPTURE) = 2536 bytes
  (pEnCoder->ioctl_sdev_open).ioctl_header.command = 0x1f; // CMD_SDEV_OPEN_CAPTURE
  (pEnCoder->ioctl_sdev_open).ioctl_header.size_of_buf = 0x9e8;
  (pEnCoder->ioctl_sdev_open).ioctl_header.time_out_value = 5000; // 5000 ms
  (pEnCoder->ioctl_sdev_open).ioctl_header.return_value = 2;    // IOCTL_RET_CMD_FAILED (sentinel)
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).stream_handle = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).stream_handle + 4) = 0;
  // Base capture mode flags (64-bit); bit 54 (0x400000 in high word) enables data insertion;
  // bits 1 and 9 (0x202 in low word) enable DVO output mode
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode = 0x10004141;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode + 4) = 0;
  if (pEnCoder->bSendData != 0) {
    uVar3 = *(uint *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode + 4);
    *(int *)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode =
         (int)(pEnCoder->ioctl_sdev_open).ioctl_param.mode;
    *(uint *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode + 4) = uVar3 | 0x400000;
  }
  if (param->outMode == VIXS_ENCODER_OUT_MODE_DVO) {
    uVar4 = *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode + 4);
    *(uint *)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode =
         (uint)(pEnCoder->ioctl_sdev_open).ioctl_param.mode | 0x202;
    *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mode + 4) = uVar4;
  }
  pEnCoder->outputMode = param->outMode;
  (pEnCoder->ioctl_sdev_open).ioctl_param.output_port = param->dvoPort;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_pmt_pid = 0x20;   // PMT PID = 0x0020 (32)
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_src = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_pid = 0x30; // Video PID = 0x0030 (48)
  if (param->videoCodec == VIXS_XCODE_VIDEO_MPEG2) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_type = MPEG2_CBR; // 4
  }
  else if (param->videoCodec == VIXS_XCODE_VIDEO_H264) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_type = AVC_CBR;   // 9 (MPEG4_AVC)
    puts("pja - force avcSeiCtl |= AVC_SEICTRL_RP_IDR");
    (pEnCoder->ioctl_sdev_open).ioctl_param.avcSeiCtrl =
         (pEnCoder->ioctl_sdev_open).ioctl_param.avcSeiCtrl | 0x20000;
    puts("pja - force avc_out_flags |= AVC_OUT_FLAGS_FORCEIDR");
    (pEnCoder->ioctl_sdev_open).ioctl_param.avc_out_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.avc_out_flags | 1;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_type = MPEG2_CBR;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_brate = param->bitRate; // bps (e.g. 17000000)
  // Map input video_std_t to width x height, frame rate, and progressive flag (bit 8 = 0x100)
  switch(param->inVideoStandard) {
  default:
    printf("%s:Invalid input format\n","vixs_encoder_init");
    return RET_ERR;
  case VIDEO_STD_480I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0; // 480
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e; // 30 fps
    break;
  case VIDEO_STD_576I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240; // 576
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19; // 25 fps
    break;
  case VIDEO_STD_480P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0; // 480
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c; // 60 fps
    break;
  case VIDEO_STD_576P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240; // 576
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32; // 50 fps
    break;
  case VIDEO_STD_720P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500; // 1280
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c; // 60 fps
    break;
  case VIDEO_STD_720P_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500; // 1280
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32; // 50 fps
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
    break;
  case VIDEO_STD_1080I_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e; // 30 fps
    break;
  case VIDEO_STD_1080I_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19; // 25 fps
    break;
  case VIDEO_STD_1080P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c; // 60 fps
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
  }
  // Map output video_std_t to encoded resolution and frame rate
  switch(param->outVideoStandard) {
  default:
    printf("%s:Invalid input format\n","vixs_encoder_init");
    return RET_ERR;
  case VIDEO_STD_480I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x1e0; // 480
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x1e; // 30 fps
    break;
  case VIDEO_STD_576I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x240; // 576
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x19; // 25 fps
    break;
  case VIDEO_STD_480P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x1e0; // 480
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c; // 60 fps
    break;
  case VIDEO_STD_576P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x240; // 576
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x32; // 50 fps
    break;
  case VIDEO_STD_720P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x500; // 1280
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c; // 60 fps
    break;
  case VIDEO_STD_720P_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x500; // 1280
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x2d0; // 720
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x32; // 50 fps
    break;
  case VIDEO_STD_1080I_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x1e; // 30 fps
    break;
  case VIDEO_STD_1080I_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x19; // 25 fps
    break;
  case VIDEO_STD_1080P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780; // 1920
    (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438; // 1080
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c; // 60 fps
  }
  if (pEnCoder->bSendData == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_size = 0;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_size = 0x10000; // 64 KiB
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_program_number = 3; // MPEG program #3
  (pEnCoder->ioctl_sdev_open).ioctl_param.diff_psi_out_time = 500; // 500 ms PSI interval
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_src = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_out_pid = 0x31; // Audio PID = 0x0031 (49)
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_type = MP2;     // MPEG-1 Layer II (2)
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_brate = 0x2ee00; // 192000 bps
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_in_srate = 48000;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_srate = 48000;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_channels = 2;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_src2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_type2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_channels2 = 2;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_in_srate2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_srate2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_brate2 = 0x2ee00;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_data_flags2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_out_pid2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.req_size = 0x10000;   // 64 KiB DMA request
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_fixed_dma_size = 0x10000; // 64 KiB
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).ioctl_param.init_time_stamp = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.init_time_stamp + 4) = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_capture_offset = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.progressiveValue = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_flags =
       (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_flags | 1;
  // video_src_type: 7 for 25 fps PAL SD (576i/p), 3 otherwise (NTSC / HD)
  if (((pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate == 0x19) &&
     ((pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert < 0x241)) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 7;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 3;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.n_b = GOP_B_DEFAULT; // overwritten below at line 492
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_data_flags = 0x400;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_interface = param->audioIf; // 0=PCM, 1=compressed
  (pEnCoder->ioctl_sdev_open).ioctl_param.insert_vbi_data_flags = 0;
  // param->flag bit 2 (4): custom remux bitrate; otherwise default 19.3 Mbps (ATSC 8-VSB)
  if ((param->flag & 4) == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.stream_remux_bitrate = 19300000;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.stream_remux_bitrate = param->rmxRate;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.peak_video_bitrate = 0;
  // param->flag bit 3 (8): custom PCR PID; otherwise share video_out_pid (0x30)
  if ((param->flag & 8) == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_out_id =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_pid;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_out_id = param->pcrPid;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_flags = 0xd8;
  // param->flag bit 1 (2): insert null packets on DVO (-i CLI option)
  if ((param->outMode == VIXS_ENCODER_OUT_MODE_DVO) && ((param->flag & 2) != 0)) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.remux_flags2 = 1;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.remux_flags2 = 0;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_seg_duration = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.output_flow_ctrl_throttle = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.low_latency = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.filters_enable = 0xc;
  (pEnCoder->ioctl_sdev_open).ioctl_param.comb_dot_crawl_detect = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.comb_luma_motion = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.comb_chroma_motion = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.comb_response = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.temporal_luma_motion = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.temporal_chroma_motion = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.spatial_luma_edge = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.spatial_chroma_edge = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.spatial_strength = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.RateContinuity = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.n_b = GOP_4_P|GOP_2_B;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_ps_enum = PS_DEFAULT_PS; // 0
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_pid1 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_content1 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_pid2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_content2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_max_vpes_size = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_fixed_apes_size = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_output_duration = 500;
  (pEnCoder->ioctl_sdev_open).ioctl_param.dts_delay_from_pcr = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.scaling_detail_level = DEFAULT_DETAIL; // 0
  (pEnCoder->ioctl_sdev_open).ioctl_param.sharpness = SHARPNESS_DEFAULT;       // 0
  (pEnCoder->ioctl_sdev_open).ioctl_param.enableLowMode = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_cropped_horz_inclusive_region = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_cropped_vert_inclusive_region = 0;
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).ioctl_param.mux_handle = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mux_handle + 4) = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.output_packet_alignment_number = 0;
  // Open 3 independent handles to /dev/vixs/xcodedrv (O_RDWR = 2)
  iVar1 = open64("/dev/vixs/xcodedrv",2);
  pEnCoder->hXcode = iVar1;
  if (pEnCoder->hXcode < 1) {
    printf("%s:Failed to open XCode driver - %d %s\n","vixs_encoder_init",pEnCoder->hXcode,
           "/dev/vixs/xcodedrv");
  }
  else {
    iVar1 = open64("/dev/vixs/xcodedrv",2);
    pEnCoder->hXcodeRecvVideo = iVar1;
    if (pEnCoder->hXcodeRecvVideo < 1) {
      printf("%s:Failed to open XCode driver - %d %s\n","vixs_encoder_init",
             pEnCoder->hXcodeRecvVideo,"/dev/vixs/xcodedrv");
    }
    else {
      iVar1 = open64("/dev/vixs/xcodedrv",2);
      pEnCoder->hXcodeSendData = iVar1;
      if (0 < pEnCoder->hXcodeSendData) {
        pEnCoder->lastConsumedRecvBufIdx = 0xff; // 0xff = no buffer consumed yet
        pEnCoder->totalOutputFrameNum = 0;
        pEnCoder->totalOutputSize = 0;
        pEnCoder->recvBufferSize = 0x10000;      // 64 KiB
        (pEnCoder->recvVideoThread).isReady = 0;
        (pEnCoder->sendDataThread).isReady = 0;
        if (param->outMode == VIXS_ENCODER_OUT_MODE_UDP) {
          pEnCoder->dstAddr = param->dstAddr;
          pEnCoder->dstPort = param->dstPort;
          pEnCoder->dstTTL = param->dstTTL;
          rVar2 = util_init_tx_udp_socket
                            (pEnCoder->dstAddr,pEnCoder->dstPort,pEnCoder->dstTTL,&pEnCoder->dstSock
                             ,&pEnCoder->dstSockAddr);
          if (rVar2 != RET_OK) {
            printf("%s:Failed to UDP TX socket\n","vixs_encoder_init");
            goto LAB_0040ea30;
          }
        }
        if (param->outMode == VIXS_ENCODER_OUT_MODE_FILE) {
          // MIPS Linux: 0x302 = O_CREAT (0x100) | O_TRUNC (0x200) | O_RDWR (0x2); missing mode arg
          iVar1 = open64(param->captureFileName,0x302);
          pEnCoder->captureOutFileFd = iVar1;
          if (pEnCoder->captureOutFileFd < 1) {
            printf("%s:Failed to open capture file %s\n","vixs_encoder_init",param->captureFileName)
            ;
            goto LAB_0040ea30;
          }
        }
        return RET_OK;
      }
      printf("%s:Failed to open XCode driver - %d %s\n","vixs_encoder_init",pEnCoder->hXcodeSendData
             ,"/dev/vixs/xcodedrv");
    }
  }
LAB_0040ea30:
  if (pEnCoder->hXcode != 0) {
    close(pEnCoder->hXcode);
    pEnCoder->hXcode = 0;
  }
  if (pEnCoder->hXcodeRecvVideo != 0) {
    close(pEnCoder->hXcodeRecvVideo);
    pEnCoder->hXcodeRecvVideo = 0;
  }
  if (pEnCoder->hXcodeSendData != 0) {
    close(pEnCoder->hXcodeSendData);
    pEnCoder->hXcodeSendData = 0;
  }
  return RET_ERR;
}



/* =====================================================================
 * vixs_encoder_deinit @ 0040eb04
 * =====================================================================
 * Purpose:
 *   Closes all open file descriptors and sockets held by `pEnCoder` and
 *   resets active/stop state flags.
 * Parameters:
 *   pEnCoder - encoder instance
 * Return value: RET_OK (0) always.
 * Callers: NONE (dead code; xcode4_close_encoder only calls free(handle),
 *   leaking these fds if called before process exit).
 * Callees: close.
 * Threading/locking: none.
 * Side effects: closes hXcode, hXcodeRecvVideo, hXcodeSendData, dstSock,
 *   captureOutFileFd if non-zero.
 */
// ==== vixs_encoder_deinit @ 0040eb04 ====

return_code_t vixs_encoder_deinit(vixs_encoder_t *pEnCoder)

{
  if (pEnCoder->hXcode != 0) {
    close(pEnCoder->hXcode);
    pEnCoder->hXcode = 0;
  }
  if (pEnCoder->hXcodeRecvVideo != 0) {
    close(pEnCoder->hXcodeRecvVideo);
    pEnCoder->hXcodeRecvVideo = 0;
  }
  if (pEnCoder->hXcodeSendData != 0) {
    close(pEnCoder->hXcodeSendData);
    pEnCoder->hXcodeSendData = 0;
  }
  if (pEnCoder->dstSock != 0) {
    close(pEnCoder->dstSock);
    pEnCoder->dstSock = 0;
  }
  if (pEnCoder->captureOutFileFd != 0) {
    close(pEnCoder->captureOutFileFd);
    pEnCoder->captureOutFileFd = 0;
  }
  pEnCoder->bActive = 0;
  pEnCoder->stopEncode = 0;
  pEnCoder->stopRecv = 0;
  return RET_OK;
}



/* =====================================================================
 * vixs_encoder_open_stream @ 0040ec70
 * =====================================================================
 * Purpose:
 *   Issues CMD_SDEV_OPEN_CAPTURE (0x1f) via ioctl(hXcode, 0x17241724) to start
 *   the XCode4 capture/encode stream. On success, caches the returned DMA
 *   receive buffer pointers (for UDP/FILE modes), copies initial PSIP/user data
 *   into the shared data-insertion buffer (if bSendData != 0), records
 *   strm_handle_encoder and startTime, and sets bActive = 1.
 * Parameters:
 *   pEnCoder - initialized encoder instance
 * Return value:
 *   RET_OK (0) if ioctl_header.return_value == 1 (IOCTL_RET_CMD_COMPLETE),
 *   RET_ERR (1) otherwise.
 * Callers:
 *   xcode4_start_encoder (01_libxcode4encoder.c.c)
 * Callees:
 *   ioctl, memcpy, gettimeofday, printf
 * Threading/locking: called before recv/send threads are started.
 * Side effects:
 *   Sends ioctl 0x17241724 (CMD_SDEV_OPEN_CAPTURE) on hXcode; writes to
 *   kernel-mapped pSendDataBuf if bSendData != 0; sets bActive = 1.
 */
// ==== vixs_encoder_open_stream @ 0040ec70 ====

return_code_t vixs_encoder_open_stream(vixs_encoder_t *pEnCoder)

{
  IOCTLSDEVOPENCAPTURE *pIVar1;    // alias for &pEnCoder->ioctl_sdev_open
  return_code_t rVar2;             // return code (RET_OK or RET_ERR)
  undefined4 uVar3;                // upper 32 bits of 64-bit stream_handle
  int local_10;                    // loop index over soc_output_buffer_num
  
                    /* Unresolved local var: PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture@[???]
                       Unresolved local var: int i@[???] */
  pIVar1 = &pEnCoder->ioctl_sdev_open;
  printf("%s:E\n","vixs_encoder_open_stream");
  ioctl(pEnCoder->hXcode,0x17241724,pIVar1);
  if ((pEnCoder->ioctl_sdev_open).ioctl_header.return_value == 1) { // IOCTL_RET_CMD_COMPLETE
    printf("%s:OPEN STREAM LOCAL IOCTL_RET_CMD_COMPLETE : %x\n","vixs_encoder_open_stream",
           (uint)(pEnCoder->ioctl_sdev_open).stream_handle);
    if ((pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_UDP) ||
       (pEnCoder->outputMode == VIXS_ENCODER_OUT_MODE_FILE)) {
      pEnCoder->recvBufNum =
           (pEnCoder->ioctl_sdev_open).ioctl_param.soc_output_video_buf_info.soc_output_buffer_num;
      printf("%s:Get %d recv video buffer from driver\n","vixs_encoder_open_stream",
             pEnCoder->recvBufNum);
      for (local_10 = 0; local_10 < pEnCoder->recvBufNum; local_10 = local_10 + 1) {
        pEnCoder->pRecvBuf[local_10] =
             (char *)(pIVar1->ioctl_param).soc_output_video_buf_info.soc_output_buffer_addr
                     [local_10];
        pEnCoder->pRecvBufPhy[local_10] =
             (char *)(pIVar1->ioctl_param).soc_output_video_buf_info.soc_output_buffer_addr_phy
                     [local_10];
        printf("%s:Video buffer %d: addr %x phy addr %x\n","vixs_encoder_open_stream",local_10,
               (uint)pEnCoder->pRecvBuf[local_10],(uint)pEnCoder->pRecvBufPhy[local_10]);
      }
    }
    if (pEnCoder->bSendData != 0) {
      pEnCoder->pSendDataBuf =
           (char *)(pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_addr;
      pEnCoder->pSendDataBufPhy =
           (char *)(pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_addr_phy;
      printf("%s:Open Send buf %x\n","vixs_encoder_open_stream",
             (pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_addr);
      memcpy(pEnCoder->pSendDataBuf,(pEnCoder->init_param->data_insertion_param).data,
             (pEnCoder->init_param->data_insertion_param).data_size);
      printf("%s:User Data Copied size 0x%x bytes\n","vixs_encoder_open_stream",
             (pEnCoder->init_param->data_insertion_param).data_size);
    }
    uVar3 = *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).stream_handle + 4);
    *(int *)&pEnCoder->strm_handle_encoder = (int)(pEnCoder->ioctl_sdev_open).stream_handle;
    *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4) = uVar3;
    gettimeofday((timeval *)&pEnCoder->startTime,(__timezone_ptr_t)0x0);
    pEnCoder->bActive = 1;
    printf("%s:X OK\n","vixs_encoder_open_stream");
    rVar2 = RET_OK;
  }
  else {
    printf("%s:X fail\n","vixs_encoder_open_stream");
    printf("%s:OPEN STREAM LOCAL Failed, return code = 0x%lx\n","vixs_encoder_open_stream",
           (pEnCoder->ioctl_sdev_open).ioctl_header.return_value);
    rVar2 = RET_ERR;
  }
  return rVar2;
}



/* =====================================================================
 * vixs_encoder_close_stream @ 0040f0f8
 * =====================================================================
 * Purpose:
 *   Issues CMD_SDEV_CLOSE (0x11) via ioctl(hXcode, 0x17241724) to close
 *   the active XCode4 stream handle.
 * Parameters:
 *   pEnCoder - encoder instance with valid strm_handle_encoder
 * Return value:
 *   RET_OK (0) if return_value == 1, RET_ERR (1) otherwise.
 * Callers:
 *   xcode4_start_encoder (error path), xcode4_stop_encoder (01_libxcode4encoder.c.c)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: called after worker threads are terminated.
 * Side effects: sends CMD_SDEV_CLOSE ioctl on hXcode.
 */
// ==== vixs_encoder_close_stream @ 0040f0f8 ====

return_code_t vixs_encoder_close_stream(vixs_encoder_t *pEnCoder)

{
  bool bVar1;                      // true (1 = RET_ERR) if ioctl return_value != 1
  IOCTLSDEVCLOSE *__s;             // &pEnCoder->ioctl_sdev_close
  undefined4 uVar2;                // upper 32 bits of 64-bit strm_handle_encoder
  
                    /* Unresolved local var: PIOCTLSDEVCLOSE pioctl_close_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_close;
  printf("%s:E\n","vixs_encoder_close_stream");
  memset(__s,0,0xb4);              // sizeof(IOCTLSDEVCLOSE) = 180 bytes
  (pEnCoder->ioctl_sdev_close).ioctl_header.command = 0x11; // CMD_SDEV_CLOSE
  (__s->ioctl_header).size_of_buf = 0xb4;
  (pEnCoder->ioctl_sdev_close).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_close).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_close).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_close).stream_handle + 4) = uVar2;
  ioctl(pEnCoder->hXcode,0x17241724,__s);
  bVar1 = (pEnCoder->ioctl_sdev_close).ioctl_header.return_value != 1;
  if (bVar1) {
    printf("%s:X fail %x\n","vixs_encoder_close_stream",
           (pEnCoder->ioctl_sdev_close).ioctl_header.return_value);
  }
  else {
    printf("%s:X OK\n","vixs_encoder_close_stream");
  }
  return (uint)bVar1;
}



/* =====================================================================
 * vixs_encoder_update_stream @ 0040f2a0
 * =====================================================================
 * Purpose:
 *   Updates encoder bitrate and/or input/output video standards.
 *   - If bActive == 0 OR bit 0 of param->flag (VIXS_ENCODER_UPDATE_BITRATE)
 *     is clear: mutates `pEnCoder->ioctl_sdev_open.ioctl_param` in place so
 *     the new settings take effect on the next vixs_encoder_open_stream().
 *   - If bActive != 0 AND bit 0 of param->flag is set: issues CMD_SDEV_CTRL
 *     (0x12) on `hXcode` to update `video_brate` live.
 * Parameters:
 *   pEnCoder - encoder instance
 *   param    - update flags (1=bitrate, 2=in_v_fmt, 4=out_v_fmt) and values
 * Return value:
 *   RET_OK (0) on success, RET_ERR (1) if (param->flag & 7) == 0 or invalid
 *   video standard.
 * Callers:
 *   xcode4_update_encoder_param (01_libxcode4encoder.c.c) <- start_encoder (00_main.c.c)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: none.
 * Side effects:
 *   Updates cached ioctl_sdev_open.ioctl_param or sends CMD_SDEV_CTRL ioctl.
 * Quirks / bugs:
 *   - Misleading log: prints "Encoder is NOT Active!" even when bActive == 1
 *     if VIXS_ENCODER_UPDATE_BITRATE (1) is not set (which is the normal
 *     restart path from start_encoder with flag=6).
 *   - Live bitrate update multiplies `param->newBitrate * 1000` (assuming kbps),
 *     whereas inactive update assigns `param->newBitrate` directly (bps).
 *   - Copy-paste log string "Invalid input format" in outVideoStandard switch.
 */
// ==== vixs_encoder_update_stream @ 0040f2a0 ====

return_code_t
vixs_encoder_update_stream(vixs_encoder_t *pEnCoder,vixs_encoder_update_param_t *param)

{
  IOCTLSDEVCTRL *__s;              // &pEnCoder->ioctl_sdev_ctrl
  return_code_t rVar1;             // return code (RET_OK or RET_ERR)
  undefined4 uVar2;                // upper 32 bits of 64-bit handles/flags
  
                    /* Unresolved local var: PIOCTLSDEVCTRL pioctl_dev_ctrl@[???]
                       Unresolved local var: PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture@[???] */
  __s = &pEnCoder->ioctl_sdev_ctrl;
  printf("%s:E\n","vixs_encoder_update_stream");
  if ((param->flag & 7U) == 0) {
    rVar1 = RET_ERR;
  }
  else {
    if ((pEnCoder->bActive == 0) || ((param->flag & 1U) == 0)) {
      printf("%s:Encoder is NOT Active!\n","vixs_encoder_update_stream");
      if ((param->flag & 1U) != 0) {
        printf("%s:update bitrate to %d\n","vixs_encoder_update_stream",param->newBitrate);
        (pEnCoder->ioctl_sdev_open).ioctl_param.video_brate = param->newBitrate;
      }
      if ((param->flag & 2U) != 0) { // VIXS_ENCODER_UPDATE_IN_V_FMT
        printf("%s:Update Input Video Format %x\n","vixs_encoder_update_stream",
               param->newInVideoStandard);
        // Set resolution, input framerate, and progressive bit (0x100) in video_in_flags
        switch(param->newInVideoStandard) {
        default:
          printf("%s:Invalid input format\n","vixs_encoder_update_stream");
          return RET_ERR;
        case VIDEO_STD_480I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff; // interlaced
          break;
        case VIDEO_STD_576I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff; // interlaced
          break;
        case VIDEO_STD_480P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          break;
        case VIDEO_STD_576P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
          break;
        case VIDEO_STD_720P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          break;
        case VIDEO_STD_720P_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
          break;
        case VIDEO_STD_1080I_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff; // interlaced
          break;
        case VIDEO_STD_1080I_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff; // interlaced
          break;
        case VIDEO_STD_1080P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100; // progressive
        }
      }
      if ((param->flag & 4U) != 0) { // VIXS_ENCODER_UPDATE_OUT_V_FMT
        printf("%s:update output video format to %d\n","vixs_encoder_update_stream",
               param->newOutVideoStandard);
        switch(param->newOutVideoStandard) {
        default:
          printf("%s:Invalid input format\n","vixs_encoder_update_stream");
          return RET_ERR;
        case VIDEO_STD_480I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x1e;
          break;
        case VIDEO_STD_576I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x19;
          break;
        case VIDEO_STD_480P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c;
          break;
        case VIDEO_STD_576P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x32;
          break;
        case VIDEO_STD_720P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c;
          break;
        case VIDEO_STD_720P_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x32;
          break;
        case VIDEO_STD_1080I_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x1e;
          break;
        case VIDEO_STD_1080I_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x19;
          break;
        case VIDEO_STD_1080P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate = 0x3c;
        }
      }
    }
    else {
      printf("%s:Encoder is Active!\n","vixs_encoder_update_stream");
      memset(__s,0,0x4a8);          // sizeof(IOCTLSDEVCTRL) = 1192 bytes
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.command = 0x12; // CMD_SDEV_CTRL
      (__s->ioctl_header).size_of_buf = 0x4a8;
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.time_out_value = 5000;
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
      uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
      *(int *)&(pEnCoder->ioctl_sdev_ctrl).stream_handle = (int)pEnCoder->strm_handle_encoder;
      *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).stream_handle + 4) = uVar2;
      if ((param->flag & 1U) != 0) {
        uVar2 = *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags + 4);
        *(uint *)&(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags =
             (uint)(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags | 1;
        *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags + 4) = uVar2;
        (pEnCoder->ioctl_sdev_ctrl).ioctl_param.video_brate = param->newBitrate * 1000;
        printf("%s:update bitrateto %d\n","vixs_encoder_update_stream",
               (pEnCoder->ioctl_sdev_ctrl).ioctl_param.video_brate);
      }
      ioctl(pEnCoder->hXcode,0x17241724,__s);
      if ((pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value == 1) { // IOCTL_RET_CMD_COMPLETE
        printf("%s:X success\n","vixs_encoder_update_stream");
        return RET_OK;
      }
    }
    if (((pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate == 0x19) &&
       ((pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert < 0x241)) {
      (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 7;
    }
    else {
      (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 3;
    }
    rVar1 = RET_OK;
  }
  return rVar1;
}



/* =====================================================================
 * vixs_encoder_flush_stream @ 0040fcc4
 * =====================================================================
 * Purpose:
 *   Issues CMD_SDEV_CTRL (0x12) with stream_ctrl = 1 and flags = 0x4000
 *   (flush/stop stream) on `hXcodeRecvVideo`. Used when stopping DVO mode
 *   (which has no recv thread).
 * Parameters:
 *   pEnCoder - encoder instance
 * Return value:
 *   RET_OK (0) if return_value == 1, RET_ERR (1) otherwise.
 * Callers:
 *   xcode4_stop_encoder (01_libxcode4encoder.c.c, when outputMode != UDP/FILE)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: none.
 * Side effects: sends CMD_SDEV_CTRL (flush) ioctl on hXcodeRecvVideo.
 */
// ==== vixs_encoder_flush_stream @ 0040fcc4 ====

return_code_t vixs_encoder_flush_stream(vixs_encoder_t *pEnCoder)

{
  bool bVar1;                      // true (1 = RET_ERR) if ioctl return_value != 1
  IOCTLSDEVCTRL *__s;              // &pEnCoder->ioctl_sdev_ctrl
  undefined4 uVar2;                // upper 32 bits of 64-bit strm_handle_encoder
  
                    /* Unresolved local var: PIOCTLSDEVCTRL pioctl_dev_ctrl@[???] */
  __s = &pEnCoder->ioctl_sdev_ctrl;
  printf("%s:E\n","vixs_encoder_flush_stream");
  memset(__s,0,0x4a8);              // sizeof(IOCTLSDEVCTRL) = 1192 bytes
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.command = 0x12; // CMD_SDEV_CTRL
  (__s->ioctl_header).size_of_buf = 0x4a8;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_ctrl).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_param.stream_ctrl = 1; // flush stream
  *(undefined4 *)&(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags = 0x4000;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).ioctl_param.flags + 4) = 0;
  ioctl(pEnCoder->hXcodeRecvVideo,0x17241724,__s);
  bVar1 = (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value != 1;
  if (bVar1) {
    printf("%s:X fail %x\n","vixs_encoder_flush_stream",
           (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value);
  }
  else {
    printf("%s:X success\n","vixs_encoder_flush_stream");
  }
  return (uint)bVar1;
}



/* =====================================================================
 * vixs_encoder_recv_stream @ 0040fe94
 * =====================================================================
 * Purpose:
 *   Issues CMD_SDEV_RECV (0x14) on `hXcodeRecvVideo` to receive a DMA buffer
 *   of encoded transport stream data from the XCode4 driver (UDP/FILE modes),
 *   returning the previous buffer index (`consumedIdx`) to the driver.
 * Parameters:
 *   pEnCoder    - encoder instance
 *   pBufSize    - [out] receives actual byte count returned by driver
 *                 (overwritten on entry with pEnCoder->recvBufferSize = 64 KiB)
 *   consumedIdx - DMA buffer index previously consumed (0xff on first call)
 *   pRecvInfo   - [out] populated with more, result, returnCode, returnValue
 * Return value:
 *   RET_OK (0) if return_value == 1, RET_ERR (1) otherwise.
 * Callers:
 *   encoder_recv_video_thread_entry (01_libxcode4encoder.c.c)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: runs in recvVideoThread using dedicated fd hXcodeRecvVideo.
 * Side effects:
 *   Sends CMD_SDEV_RECV ioctl; updates pEnCoder->totalOutputSize,
 *   totalOutputFrameNum, and resets recvTimeoutCnt = 10 on success.
 */
// ==== vixs_encoder_recv_stream @ 0040fe94 ====

return_code_t
vixs_encoder_recv_stream
          (vixs_encoder_t *pEnCoder,int *pBufSize,int consumedIdx,
          vixs_xcode_recv_strm_info_t *pRecvInfo)

{
  bool bVar1;                      // true (1 = RET_ERR) if ioctl return_value != 1
  IOCTLSDEVRECV *__s;              // &pEnCoder->ioctl_sdev_recv
  undefined4 uVar2;                // upper 32 bits of 64-bit stream_handle / result
  
                    /* Unresolved local var: PIOCTLSDEVRECV pioctl_recv_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_recv;
  memset(__s,0,0x240);              // sizeof(IOCTLSDEVRECV) = 576 bytes
  (pEnCoder->ioctl_sdev_recv).ioctl_header.command = 0x14; // CMD_SDEV_RECV
  (__s->ioctl_header).size_of_buf = 0x240;
  (pEnCoder->ioctl_sdev_recv).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
  *pBufSize = pEnCoder->recvBufferSize; // 0x10000 (64 KiB)
  *(undefined4 *)&(pEnCoder->ioctl_sdev_recv).flags = 0x2000;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_recv).flags + 4) = 0;
  (pEnCoder->ioctl_sdev_recv).size = *pBufSize;
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_recv).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_recv).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_sdev_recv).dmabuffer_consumed_idx = consumedIdx;
  ioctl(pEnCoder->hXcodeRecvVideo,0x17241724,__s);
  bVar1 = (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value != 1;
  if (bVar1) {
    printf("%s:RECEIVE STREAM Error, reason %x : result = 0x%x\n","vixs_encoder_recv_stream",
           (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value,
           (uint)(pEnCoder->ioctl_sdev_recv).result);
    pRecvInfo->returnValue = (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value;
  }
  else {
    *pBufSize = (pEnCoder->ioctl_sdev_recv).size;
    pRecvInfo->more = (pEnCoder->ioctl_sdev_recv).more;
    uVar2 = *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_recv).result + 4);
    *(int *)&pRecvInfo->result = (int)(pEnCoder->ioctl_sdev_recv).result;
    *(undefined4 *)((int)&pRecvInfo->result + 4) = uVar2;
    pRecvInfo->returnCode = (pEnCoder->ioctl_sdev_recv).returncode;
    pRecvInfo->returnValue = (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value;
    pEnCoder->totalOutputSize = pEnCoder->totalOutputSize + (pEnCoder->ioctl_sdev_recv).size;
    pEnCoder->totalOutputFrameNum = (uint32)(pEnCoder->ioctl_sdev_recv).picturenumber;
    pEnCoder->recvTimeoutCnt = 10;
  }
  return (uint)bVar1;
}



/* =====================================================================
 * vixs_encoder_get_ts_info @ 00410168
 * =====================================================================
 * Purpose:
 *   Issues TS control command 0x102 with flags = 0x20 (SDEV_TSCTRL_GET_INFO)
 *   on `hXcodeRecvVideo` to query transport stream lock/error status into
 *   `pEnCoder->ioctl_ts_ctrl`.
 * Parameters:
 *   pEnCoder - encoder instance
 * Return value:
 *   RET_OK (0) if return_value == 1, RET_ERR (1) otherwise.
 * Callers:
 *   xcode4_get_encoder_ts_info (01_libxcode4encoder.c.c, which unpacks the
 *   bitfield at handle+0x1324 into vixs_encoder_ts_info_t)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: none.
 * Side effects: sends ioctl 0x17241724 (cmd 0x102) on hXcodeRecvVideo.
 */
// ==== vixs_encoder_get_ts_info @ 00410168 ====

return_code_t vixs_encoder_get_ts_info(vixs_encoder_t *pEnCoder)

{
  bool bVar1;                      // true (1 = RET_ERR) if ioctl return_value != 1
  IOCTLSDEVTSCTRL *__s;            // &pEnCoder->ioctl_ts_ctrl
  undefined4 uVar2;                // upper 32 bits of 64-bit strm_handle_encoder
  
                    /* Unresolved local var: PIOCTLSDEVTSCTRL pioctl_ts_ctrl@[???] */
  __s = &pEnCoder->ioctl_ts_ctrl;
  memset(__s,0,0x450);              // sizeof(IOCTLSDEVTSCTRL) = 1104 bytes
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.command = 0x102; // SDEV_TSCTRL
  (__s->ioctl_header).size_of_buf = 0x450;
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_ts_ctrl).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_ts_ctrl).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.flags = 0x20; // SDEV_TSCTRL_GET_INFO
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.src = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.pid = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.pid_remap = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.insert_pid = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.psi_check_table_id = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.psi_check_version_id = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.num_ts_passthrough_pid = 0;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.num_ts_passthrough_table = 0;
  ioctl(pEnCoder->hXcodeRecvVideo,0x17241724,__s);
  bVar1 = (pEnCoder->ioctl_ts_ctrl).ioctl_header.return_value != 1;
  if (bVar1) {
    printf("%s:SDEV_TSCTRL_GET_INFO Error, reason %x\n","vixs_encoder_get_ts_info",
           (pEnCoder->ioctl_ts_ctrl).ioctl_header.return_value);
  }
  return (uint)bVar1;
}



/* =====================================================================
 * vixs_encoder_send_stream @ 00410330
 * =====================================================================
 * Purpose:
 *   Issues CMD_SDEV_SEND (0x13) on `hXcodeSendData` to submit PSIP/SI or
 *   user TS packets from `pEnCoder->pSendDataBufPhy` into the XCode4 muxer.
 * Parameters:
 *   pEnCoder - encoder instance with valid pSendDataBufPhy
 *   pbufsize - [in/out] byte size to send (188-byte aligned); updated on
 *              success with bytes actually accepted by driver
 *   flags    - driver send flags (caller passes 0x8000000 for normal send,
 *              0x8400000 with size=0 on thread exit to signal end-of-stream)
 * Return value:
 *   RET_OK (0) if return_value == 1, RET_ERR (1) otherwise.
 * Callers:
 *   encoder_send_data_thread_entry (01_libxcode4encoder.c.c)
 * Callees:
 *   memset, ioctl, printf
 * Threading/locking: runs in sendDataThread using dedicated fd hXcodeSendData.
 * Side effects: sends CMD_SDEV_SEND ioctl on hXcodeSendData.
 */
// ==== vixs_encoder_send_stream @ 00410330 ====

return_code_t vixs_encoder_send_stream(vixs_encoder_t *pEnCoder,int *pbufsize,uint64 flags)

{
  bool bVar1;                      // true (1 = RET_ERR) if ioctl return_value != 1
  IOCTLSDEVSEND *__s;              // &pEnCoder->ioctl_sdev_send
  undefined4 uVar2;                // upper 32 bits of 64-bit strm_handle_encoder
  
                    /* Unresolved local var: PIOCTLSDEVSEND pioctl_send_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_send;
  memset(__s,0,0x1d8);              // sizeof(IOCTLSDEVSEND) = 472 bytes
  (pEnCoder->ioctl_sdev_send).ioctl_header.command = 0x13; // CMD_SDEV_SEND
  (__s->ioctl_header).size_of_buf = 0x1d8;
  (pEnCoder->ioctl_sdev_send).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_send).ioctl_header.return_value = 2; // IOCTL_RET_CMD_FAILED
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_send).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_send).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_sdev_send).flags = flags;
  (pEnCoder->ioctl_sdev_send).buffer = pEnCoder->pSendDataBufPhy; // physical DMA address
  (pEnCoder->ioctl_sdev_send).size = *pbufsize;
  ioctl(pEnCoder->hXcodeSendData,0x17241724,__s);
  bVar1 = (pEnCoder->ioctl_sdev_send).ioctl_header.return_value != 1;
  if (bVar1) {
    printf("%s:IOCTLSDEVSEND Error, reason %x\n","vixs_encoder_send_stream",
           (pEnCoder->ioctl_sdev_send).ioctl_header.return_value);
  }
  else {
    *pbufsize = (pEnCoder->ioctl_sdev_send).size;
  }
  return (uint)bVar1;
}


/* #####################################################################
 * FILE: 03_util_common.c.c
 * ##################################################################### */

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




/* #####################################################################
 * FILE: 04_libadv7842.cpp.c
 * ##################################################################### */

/*
 * 04_libadv7842.cpp.c  --  annotated Ghidra decompilation of libadv7842.cpp
 * =========================================================================
 *
 * Original source : /media/WD_passport/Meld_AV_Script/2p12/avstreamer/src/libadv7842/libadv7842.cpp
 * Compiler        : GNU C++ 4.5.3 (MIPS32r2 little-endian, with DWARF)
 * Code range      : 0x00410da0 - 0x00412a88
 *
 * Role in av_streamer:
 *   C-style public API wrapper (`adv7842_*`) and background polling thread
 *   (`adv7842_thread`) around the `ADV7842_t` C++ hardware driver class
 *   (implemented in adv7842.cpp). Manages input selection (CVBS, Component,
 *   HDMI), video standard detection, audio mode/sample-rate detection,
 *   XCode4 GPIO 5 audio clock muxing, free-run mode, and event callbacks
 *   consumed by main.c (`sample_event_callback`).
 *
 * Main data structures (from DWARF):
 *   ADV7842_input_type_t:
 *     NONE=0, CVBS=1, COMPONENT=2, HDMI=4
 *   ADV7842_event_id:
 *     ADV7842_EVT_HDMI_HPD           = 1   (0x01) - HDMI 5V/cable plug/unplug
 *     ADV7842_EVT_INPUT_SIG_RES_CHG  = 2   (0x02) - active input video std changed
 *     ADV7842_EVT_INPUT_SIG_LOST     = 4   (0x04) - active input signal lost
 *     ADV7842_EVT_INPUT_DETECTED     = 8   (0x08) - any input signal detected
 *     ADV7842_EVT_HDMI_INPUT_SIG_ENC = 16  (0x10) - HDMI signal encrypted (HDCP)
 *     ADV7842_EVT_MASK               = 31  (0x1f)
 *   ADV7842_freerun_mode_t:
 *     AUTO=0, FORCED_BLUESCREEN=1, FORCED_COLORBAR=2
 *   video_std_t:
 *     NONE=0, 480I=1, 576I=2, 480P=3, 576P=4, 720P_60HZ=5, 720P_50HZ=6,
 *     1080I_60HZ=7, 1080I_50HZ=8, 1080P_60HZ=9
 *   audio_mode_t:
 *     AUDIO_MODE_PCM=0, AUDIO_MODE_COMPRESSED=1
 *   ADV7842_t (16 bytes):
 *     +0x00 m_InputSource (enum_xc_input_type_t)
 *     +0x04 m_VideoStandard (enum_analog_digital_video_standard_t)
 *     +0x08 m_I2CChannel (uint)
 *     +0x0c xcodeIO (xcode_io_t *)
 *
 * Globals owned (all internal/static to libadv7842.cpp):
 *   video_standard_array[10] @ 0x00436640 (.data) - maps enum_analog_digital_video_standard_t -> video_std_t
 *   exitMyThread             @ 0x00437790 (.bss)  - set to 1 by adv7842_deinit() to stop adv7842_thread
 *   myThreadId               @ 0x00437794 (.bss)  - pthread_t for adv7842_thread
 *   adv7842Mutex             @ 0x00437798 (.bss)  - pthread_mutex_t guarding state & ADV7842 I2C access
 *   my_event_callback        @ 0x004377b0 (.bss)  - user callback fn(data, ADV7842_event_t *)
 *   my_event_callback_data   @ 0x004377b4 (.bss)  - opaque user pointer passed to callback
 *   curStd                   @ 0x004377b8 (.bss)  - static curStd (distinct from main.c's global curStd @ 0x004376a8)
 *   my_subscribed_event      @ 0x004377bc (.bss)  - bitmask of subscribed ADV7842_event_id flags
 *   hdmi_available           @ 0x004377c0 (.bss)  - 1 if HDMI 5V cable detect asserted, else 0
 *   msg_buff[256]            @ 0x004377c4 (.bss)  - scratch buffer for syslog/printf audio messages
 *   msg_sample_rate[128]     @ 0x004378c4 (.bss)  - current HDMI audio sample rate string
 *   last_sample_rate[128]    @ 0x00436690 (.data) - previous HDMI audio sample rate string
 *   audio_msg_cntr           @ 0x00437944 (.bss)  - periodic sample-rate syslog counter (logs every 16 ticks)
 *   xcodeIO                  @ 0x00437948 (.bss)  - cached xcode_io_t* singleton for GPIO 5 audio mux
 */

// ==== adv7842_thread @ 00410da0 ====
/*
 * adv7842_thread():
 *   Background polling thread started by adv7842_subscribe_event(). Polls the
 *   ADV7842 every 200 ms for HDMI HPD, HDMI audio format/sample rate, HDCP
 *   encryption, newly connected inputs, video standard changes, and signal loss.
 *
 * Parameters:
 *   arg - Cast to ADV7842_t* (the `handle` returned by adv7842_init()).
 *
 * Returns:
 *   NULL (0x0) when exitMyThread != 0.
 *
 * I2C / MMIO accesses:
 *   - ADV7842 IO Map (7-bit 0x20 -> 8-bit 0x40):
 *       reg 0xd7: polled at top of loop (value discarded)
 *       reg 0x6f ('o'): HDMI_LVL_RAW_STATUS_1; bit 1 (0x02) = CABLE_DET_A_RAW (HDMI 5V detect)
 *   - ADV7842 HDMI Map (7-bit 0x34 -> 8-bit 0x68):
 *       reg 0x18: audio packet status (printed)
 *       reg 0x36 ('6'): IEC 60958 CS byte 0; bit 1 (0x02): 0=PCM, 1=Compressed
 *       reg 0x39 ('9'): IEC 60958 CS byte 3; bits [3:0] = audio sample rate
 *       reg 0x04: bit 4 (0x10) triggers ADV7842_EVT_HDMI_INPUT_SIG_ENC (HDCP/encrypted)
 *   - XCode4 MMIO GPIO 5 (mask 0x20):
 *       reg 0x1100 (GPIO output data), reg 0x1104 (GPIO output enable):
 *       Driven HIGH when HDMI audio sample rate == 48.0 kHz (0x2) -> HDMI I2S Audio Clock;
 *       driven LOW otherwise -> External PCM DAC / XC4208 Master Audio Clock.
 *
 * Bugs / quirks:
 *   - `local_c` is typed by Ghidra as `video_std_t` due to stack-slot reuse, but
 *     is actually `uint count` passed to adv7842_enum_input().
 *   - When firing eventID 0x10 (encrypted), `local_34.event_info` is left uninitialized/stale.
 *   - Releases `adv7842Mutex` around `my_event_callback()` invocations so callbacks
 *     can safely call `adv7842_program_input()` / `adv7842_subscribe_event()`.
 *
 * Locals:
 *   bVar1      - samplingFreq48KHz: tracks whether GPIO 5 is currently HIGH (HDMI 48 kHz)
 *   iVar2      - strcmp() result comparing last_sample_rate vs msg_sample_rate
 *   local_4c   - inputMask: bitmask of detected inputs when firing ADV7842_EVT_INPUT_DETECTED
 *   local_48   - i: loop index over aAStack_24[0..count-1]
 *   local_3c   - std: detected video_std_t on current input
 *   local_38   - input: current active ADV7842_input_type_t
 *   local_34   - event: ADV7842_event_t passed to my_event_callback
 *   local_28   - regVal[4]: 1-byte I2C register read buffer
 *   aAStack_24 - info[3]: ADV7842_input_info_t array for adv7842_enum_input()
 *   local_c    - count: uint count of detected inputs returned by adv7842_enum_input()
 */
void * adv7842_thread(void *arg)

{
  bool bVar1;
  int iVar2;
  ADV7842_input_type_t local_4c;
  video_std_t local_48;
  video_std_t local_3c;
  ADV7842_input_type_t local_38;
  ADV7842_event_t local_34;
  byte local_28 [4];
  ADV7842_input_info_t aAStack_24 [3];
  video_std_t local_c;
  
                    /* Unresolved local var: video_std_t std@[???]
                       Unresolved local var: int adv7842_handle@[???]
                       Unresolved local var: ADV7842_input_type_t input@[???]
                       Unresolved local var: ADV7842_event_t event@[???]
                       Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: uchar regVal@[???]
                       Unresolved local var: uchar samplingFreq48KHz@[???] */
  local_3c = VIDEO_STD_NONE;
  bVar1 = false;
  xcodeIO = xcode_io_t::get_io();
  while (exitMyThread == 0) {
    ADV7842_t::read_register(arg,0x20,0xd7,local_28); // IO Map (0x40) reg 0xd7
    pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
    if ((my_subscribed_event & 0x11) != 0) {   // HPD (0x01) or HDMI_INPUT_SIG_ENC (0x10)
      ADV7842_t::read_register(arg,0x20,'o',local_28); // IO Map 0x6f: bit 1 = HDMI 5V cable detect
      if (((((local_28[0] & 2) != 0) && (hdmi_available == 0)) ||
          (((local_28[0] & 2) == 0 && (hdmi_available == 1)))) && ((my_subscribed_event & 1) != 0))
      {
        local_34.eventID = 1;                  // ADV7842_EVT_HDMI_HPD
        local_34.input = ADV7842_INPUT_TYPE_HDMI;
        hdmi_available = (uint)((local_28[0] & 2) != 0);
        local_34.event_info = hdmi_available;
        pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
        (*my_event_callback)(my_event_callback_data,&local_34);
        pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
      }
      ADV7842_t::read_register(arg,0x34,'\x18',local_28); // HDMI Map (0x68) reg 0x18
      printf("HDMI HDMI 0x18 is %x\n",(uint)local_28[0]);
      ADV7842_t::read_register(arg,0x34,'6',local_28); // HDMI Map reg 0x36 (CS byte 0)
      printf("HDMI HDMI 0x36 is %x\n",(uint)local_28[0]);
      if ((local_28[0] & 2) == 0) {
        puts("libadv7842.cpp : I2S Audio is PCM format");
      }
      else if ((local_28[0] & 2) == 2) {
        puts("libadv7842.cpp : I2S Audio is Compressed format");
      }
      ADV7842_t::read_register(arg,0x34,'9',local_28); // HDMI Map reg 0x39 (CS byte 3: Fs)
      if ((local_28[0] & 0xf) == 2) {          // 0x2 == 48.0 kHz
        if (!bVar1) {
          memcpy(msg_buff,"libadv7842.cpp : Selecting HDMI Audio - Setting GPIO 5 to HIGH",0x3f);
          puts(msg_buff);
          syslog(5,"libadv7842.cpp : Selecting HDMI Audio - Setting GPIO 5 to HIGH");
          xcode_io_t::write_reg_mask_bits(xcodeIO,0x1100,0x20,0x20); // GPIO5 data = HIGH
          xcode_io_t::write_reg_mask_bits(xcodeIO,0x1104,0x20,0x20); // GPIO5 output enable
          bVar1 = true;
          memcpy(msg_buff,"libadv7842.cpp : GPIO 5 set to HIGH - Using HDMI I2S Audio Clock",0x41);
          puts(msg_buff);
          syslog(5,msg_buff);
        }
      }
      else if (bVar1) {
        memcpy(msg_buff,"libadv7842.cpp : Selecting External PCM DAC Audio - Setting GPIO 5 to LOW",
               0x4a);
        puts(msg_buff);
        syslog(5,msg_buff);
        xcode_io_t::write_reg_mask_bits(xcodeIO,0x1100,0x20,0); // GPIO5 data = LOW
        xcode_io_t::write_reg_mask_bits(xcodeIO,0x1104,0x20,0x20); // GPIO5 output enable
        bVar1 = false;
        memcpy(msg_buff,"libadv7842.cpp : GPIO 5 set to LOW - Using XC4208 Master Audio Clock",0x45)
        ;
        syslog(5,msg_buff);
      }
      if (hdmi_available == 1) {
        strcpy(last_sample_rate,msg_sample_rate);
        switch(local_28[0] & 0xf) {            // IEC 60958-3 Channel Status Byte 3 [3:0]
        case 0:
          memcpy(msg_sample_rate,"44.1KHz",8);
          break;
        default:
          memcpy(msg_sample_rate,"UNKNOWN!",9);
          break;
        case 2:
          memcpy(msg_sample_rate,"48.0KHz",8);
          break;
        case 3:
          memcpy(msg_sample_rate,"32.0KHz",8);
          break;
        case 8:
          memcpy(msg_sample_rate,"88.2KHz",8);
          break;
        case 10:
          memcpy(msg_sample_rate,"96.0KHz",8);
          break;
        case 0xc:
          memcpy(msg_sample_rate,"176KHz",7);
          break;
        case 0xe:
          memcpy(msg_sample_rate,"192KHz",7);
        }
        iVar2 = strcmp(last_sample_rate,msg_sample_rate);
        if (iVar2 != 0) {
          audio_msg_cntr = 0;
          sprintf(msg_buff,
                  "libadv7842.cpp : HDMI Audio Sample Rate Changed : last = %s / current = %s",
                  last_sample_rate,msg_sample_rate);
          puts(msg_buff);
          syslog(5,msg_buff);
        }
        audio_msg_cntr = audio_msg_cntr + 1;
        if (audio_msg_cntr == 0x10) {          // log every 16 * 200ms = 3.2s
          audio_msg_cntr = 0;
          sprintf(msg_buff,"libadv7842.cpp : HDMI Audio Sample Rate = %s",msg_sample_rate);
          syslog(5,msg_buff);
        }
        printf("%s - cntr = %d\n",msg_buff,audio_msg_cntr);
      }
      ADV7842_t::read_register(arg,0x34,'\x04',local_28); // HDMI Map reg 0x04
      if (((local_28[0] & 0x10) != 0) && ((my_subscribed_event & 0x10) != 0)) {
        local_34.eventID = 0x10;               // ADV7842_EVT_HDMI_INPUT_SIG_ENC
        local_34.input = ADV7842_INPUT_TYPE_HDMI;
        pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
        (*my_event_callback)(my_event_callback_data,&local_34);
        pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
      }
    }
    if ((my_subscribed_event & 0x1f) == 0) {
      pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
    }
    else {
      adv7842_get_input((int)arg,&local_38);
      printf("Current Input is %x\n",local_38);
      if ((local_38 == ADV7842_INPUT_TYPE_NONE) || ((my_subscribed_event & 8) != 0)) {
        if ((my_subscribed_event & 8) == 0) {
          pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
        }
        else {
                    /* Unresolved local var: ADV7842_input_info[3] info@[???]
                       Unresolved local var: uint count@[???] */
          pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
          local_c = VIDEO_STD_NONE;            // count = 0
          local_34.input = ADV7842_INPUT_TYPE_NONE;
          adv7842_enum_input((int)arg,aAStack_24,&local_c,1);
          if (local_c != VIDEO_STD_NONE) {     // count != 0
                    /* Unresolved local var: int inputMask@[???] */
            local_4c = ADV7842_INPUT_TYPE_NONE;
            local_34.eventID = 8;              // ADV7842_EVT_INPUT_DETECTED
                    /* Unresolved local var: uint i@[???] */
            for (local_48 = VIDEO_STD_NONE; local_48 < local_c; local_48 = local_48 + VIDEO_STD_480I
                ) {
              local_4c = local_4c | aAStack_24[local_48].input;
            }
            local_34.input = local_4c;
            local_34.event_info = local_c;
            (*my_event_callback)(my_event_callback_data,&local_34);
          }
        }
      }
      else {
        adv7842_detect_video_std_internal((int)arg,&local_3c);
        pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
        if (((local_3c == curStd) || (local_3c == VIDEO_STD_NONE)) ||
           ((my_subscribed_event & 2) == 0)) {
          if ((local_3c == VIDEO_STD_NONE) && ((my_subscribed_event & 4) != 0)) {
            local_34.eventID = 4;              // ADV7842_EVT_INPUT_SIG_LOST
            local_34.input = local_38;
            (*my_event_callback)(my_event_callback_data,&local_34);
          }
        }
        else {
          local_34.eventID = 2;                // ADV7842_EVT_INPUT_SIG_RES_CHG
          local_34.input = local_38;
          local_34.event_info = local_3c;
          curStd = local_3c;
          (*my_event_callback)(my_event_callback_data,&local_34);
        }
      }
    }
    usleep(200000);                            // 200 ms poll interval
  }
  return (void *)0x0;
}



// ==== map_video_std @ 00411a64 ====
/*
 * map_video_std():
 *   Linearly searches `video_standard_array[0..9]` to translate an ADV7842
 *   internal `enum_analog_digital_video_standard_t` bitmask into the public
 *   `video_std_t` enum.
 *
 * Table contents (from .data @ 0x00436640):
 *   [0] 0x00000000 -> VIDEO_STD_NONE (0)
 *   [1] 0x00000001 -> VIDEO_STD_480I (1)
 *   [2] 0x00000010 -> VIDEO_STD_576I (2)
 *   [3] 0x00000200 -> VIDEO_STD_480P (3)
 *   [4] 0x00000800 -> VIDEO_STD_576P (4)
 *   [5] 0x08001000 -> VIDEO_STD_720P_60HZ (5)
 *   [6] 0x40001000 -> VIDEO_STD_720P_50HZ (6)
 *   [7] 0x01002000 -> VIDEO_STD_1080I_60HZ (7)
 *   [8] 0x02002000 -> VIDEO_STD_1080I_50HZ (8)
 *   [9] 0x08004000 -> VIDEO_STD_1080P_60HZ (9)
 *
 * Returns:
 *   Matching video_std_t, or VIDEO_STD_NONE (0) if not found.
 *
 * Callers:
 *   adv7842_program_input_internal(), adv7842_detect_video_std_internal().
 *
 * Locals:
 *   local_18 - i: loop index (0..9)
 *   local_14 - index: matched entry index
 *   local_10 - std: return value
 */

/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */

video_std_t map_video_std(enum_analog_digital_video_standard_t VideoStandard)

{
  uint local_18;
  uint local_14;
  video_std_t local_10;
  
                    /* Unresolved local var: uint i@[???]
                       Unresolved local var: uint index@[???]
                       Unresolved local var: video_std_t std@[???] */
  local_18 = 0;
  do {
    if (9 < local_18) {
LAB_00411ad8:
      if (local_18 == 10) {
        local_10 = VIDEO_STD_NONE;
      }
      else {
        local_10 = video_standard_array[local_14].std;
      }
      return local_10;
    }
    if (video_standard_array[local_18].VideoStandard == VideoStandard) {
      local_14 = local_18;
      goto LAB_00411ad8;
    }
    local_18 = local_18 + 1;
  } while( true );
}



// ==== adv7842_init @ 00411b28 ====
/*
 * adv7842_init():
 *   Allocates and constructs an ADV7842_t instance (I2C channel 0), loads the
 *   EDID (from pParam->edid_file_name if non-NULL, else default internal EDID),
 *   initializes adv7842Mutex, and stores the event callback and user data.
 *
 * Parameters:
 *   pParam  - Init parameters (data, event_callback, edid_file_name).
 *   pHandle - [out] Receives (uint)this pointer on success.
 *
 * Returns:
 *   RET_OK (0) on success, RET_ERR (1) if operator_new returns NULL.
 *
 * Callers:
 *   main() in main.c.
 *
 * Side effects:
 *   - ADV7842_t::ADV7842_t() sets GPIO 5 LOW, reads IO Map 0xfb, and calls
 *     InitHDMI() if not already initialized.
 *   - ADV7842_t::LoadEDID() writes EDID to ADV7842 and toggles HPA_AUTO_INT_EDID.
 *   - Quirk: ADV7842_t::ADV7842_t(this, 0) is invoked before checking `this == NULL`.
 */
return_code_t adv7842_init(ADV7842_init_param_t *pParam,uint *pHandle)

{
  ADV7842_t *this;
  return_code_t rVar1;
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???] */
  this = operator_new(0x10);                   // sizeof(ADV7842_t) == 16
                    /* try { // try from 00411b88 to 00411b8f has its CatchHandler @ 00411c84 */
  ADV7842_t::ADV7842_t(this,0);                // i2cChannel = 0
  if (this == (ADV7842_t *)0x0) {
    rVar1 = RET_ERR;
  }
  else {
    if (pParam->edid_file_name == (char *)0x0) {
      ADV7842_t::LoadEDID(this);
    }
    else {
      ADV7842_t::LoadEDID(this,pParam->edid_file_name);
    }
    exitMyThread = 0;
    pthread_mutex_init((pthread_mutex_t *)&adv7842Mutex,(pthread_mutexattr_t *)0x0);
    my_event_callback = pParam->event_callback;
    my_event_callback_data = pParam->data;
    *pHandle = (uint)this;
    rVar1 = RET_OK;
  }
  return rVar1;
}



// ==== adv7842_deinit @ 00411cbc ====
/*
 * adv7842_deinit():
 *   Stops adv7842_thread if running (sets exitMyThread = 1 and pthread_join),
 *   destroys adv7842Mutex, and deletes the ADV7842_t instance.
 *
 * Parameters:
 *   handle - ADV7842_t* cast to int.
 *
 * Returns:
 *   RET_OK (0).
 *
 * Callers:
 *   main() in main.c (on exit).
 *
 * Locals:
 *   iVar1      - pthread_join() return code
 *   in_a1      - Ghidra-recovered $a1 scratch passed to ~ADV7842_t
 *   apvStack_14 - res: void* return value from pthread_join()
 */
return_code_t adv7842_deinit(int handle)

{
  int iVar1;
  void **in_a1;
  void *apvStack_14 [2];
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: void * res@[???] */
  if (myThreadId != 0) {
    exitMyThread = 1;
    in_a1 = apvStack_14;
    iVar1 = pthread_join(myThreadId,in_a1);
    if (iVar1 != 0) {
      printf("Error to terminate thread %x\n",myThreadId);
      in_a1 = (void **)myThreadId;
    }
    myThreadId = 0;
  }
  pthread_mutex_destroy((pthread_mutex_t *)&adv7842Mutex);
  if (handle != 0) {
    ADV7842_t::~ADV7842_t((ADV7842_t *)handle,(int)in_a1);
    operator_delete((void *)handle);
  }
  return RET_OK;
}



// ==== adv7842_program_input_internal @ 00411de4 ====
/*
 * adv7842_program_input_internal():
 *   Programs the ADV7842 for the requested input type (NONE, CVBS, COMPONENT,
 *   or HDMI), then detects the active video standard. When `*result_std ==
 *   VIDEO_STD_NONE` on entry, auto-reprograms Component (480i..1080p), CVBS 576i,
 *   or HDMI 1080p60 to match the detected resolution. Updates `hdmi_available`.
 *   Caller must hold `adv7842Mutex`.
 *
 * Parameters:
 *   handle     - ADV7842_t* cast to int.
 *   input      - Desired ADV7842_input_type_t (NONE/CVBS/COMPONENT/HDMI).
 *   result_std - [in/out] On entry: expected std (e.g. 576I for CVBS, or NONE
 *                to trigger auto-reprogramming to detected std). On exit: detected std.
 *
 * Returns:
 *   RET_OK (0) on success, RET_ERR (1) on unsupported input or Component format.
 *
 * Callers:
 *   adv7842_program_input(), adv7842_enum_input().
 *
 * Locals:
 *   param2   - std: mapped video_std_t from detect_video_standard()
 *   local_20 - InputType: enum_xc_input_type_t passed to program_input_source()
 *   local_10 - VideoStandard[2]: enum_analog_digital_video_standard_t output
 */

/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */

return_code_t
adv7842_program_input_internal(int handle,ADV7842_input_type_t input,video_std_t *result_std)

{
  video_std_t param2;
  enum_xc_input_type_t local_20;
  enum_analog_digital_video_standard_t local_10 [2];
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: enum_xc_input_type_t InputType@[???]
                       Unresolved local var: video_std_t expectedStd@[???]
                       Unresolved local var: video_std_t std@[???]
                       Unresolved local var: enum_analog_digital_video_standard_t
                       VideoStandard@[???] */
  local_10[0] = ANALOG_DIGITAL_VIDEO_STANDARD_NONE;
  if (input == ADV7842_INPUT_TYPE_CVBS) {
    if (*result_std == VIDEO_STD_576I) {
      local_20 = XC_INPUT_HDMI_COMPOSITE_576I;
    }
    else {
      local_20 = XC_INPUT_HDMI_COMPOSITE_480I;
    }
  }
  else if ((int)input < 2) {
    if (input != ADV7842_INPUT_TYPE_NONE) {
LAB_00411ed4:
      printf("%s: unsupported input type %d\n","adv7842_program_input_internal",input);
      return RET_ERR;
    }
    local_20 = XC_INPUT_NONE;
  }
  else if (input == ADV7842_INPUT_TYPE_COMPONENT) {
    local_20 = XC_INPUT_HDMI_COMPONENT_480I;
  }
  else {
    if (input != ADV7842_INPUT_TYPE_HDMI) goto LAB_00411ed4;
    local_20 = XC_INPUT_HDMI_TO_HDMI;
  }
  ADV7842_t::program_input_source((ADV7842_t *)handle,local_20);
  if (local_20 == XC_INPUT_NONE) {
    *result_std = VIDEO_STD_NONE;
  }
  else {
    ADV7842_t::detect_video_standard((ADV7842_t *)handle,local_10);
    param2 = map_video_std(local_10[0]);
    if (*result_std == VIDEO_STD_NONE) {
      if (((param2 == VIDEO_STD_480I) || (param2 == VIDEO_STD_NONE)) ||
         (input != ADV7842_INPUT_TYPE_COMPONENT)) {
        if ((input == ADV7842_INPUT_TYPE_HDMI) && (param2 == VIDEO_STD_1080P_60HZ)) {
          ADV7842_t::program_input_source((ADV7842_t *)handle,XC_INPUT_HDMI_TO_HDMI_1080P);
        }
        else if ((param2 != VIDEO_STD_480I) &&
                ((param2 == VIDEO_STD_576I && (input == ADV7842_INPUT_TYPE_CVBS)))) {
          ADV7842_t::program_input_source((ADV7842_t *)handle,XC_INPUT_HDMI_COMPOSITE_576I);
        }
      }
      else {
        switch(param2) {
        default:
          printf("%s: unsupported Component format %d\n","adv7842_program_input_internal",param2);
          return RET_ERR;
        case VIDEO_STD_480I:
          local_20 = XC_INPUT_HDMI_COMPONENT_480I;
          break;
        case VIDEO_STD_576I:
          local_20 = XC_INPUT_HDMI_COMPONENT_576I;
          break;
        case VIDEO_STD_480P:
          local_20 = XC_INPUT_HDMI_COMPONENT_480P;
          break;
        case VIDEO_STD_576P:
          local_20 = XC_INPUT_HDMI_COMPONENT_576P;
          break;
        case VIDEO_STD_720P_60HZ:
          local_20 = XC_INPUT_HDMI_COMPONENT_720P;
          break;
        case VIDEO_STD_720P_50HZ:
          local_20 = XC_INPUT_HDMI_COMPONENT_720P_50;
          break;
        case VIDEO_STD_1080I_60HZ:
          local_20 = XC_INPUT_HDMI_COMPONENT_1080I;
          break;
        case VIDEO_STD_1080I_50HZ:
          local_20 = XC_INPUT_HDMI_COMPONENT_1080I_50;
          break;
        case VIDEO_STD_1080P_60HZ:
          local_20 = XC_INPUT_HDMI_COMPONENT_1080P;
        }
        ADV7842_t::program_input_source((ADV7842_t *)handle,local_20);
      }
    }
    if (input == ADV7842_INPUT_TYPE_HDMI) {
      if (param2 == VIDEO_STD_NONE) {
        hdmi_available = 0;
      }
      else {
        hdmi_available = 1;
      }
    }
    *result_std = param2;
  }
  return RET_OK;
}



// ==== adv7842_program_input @ 004121d4 ====
/*
 * adv7842_program_input():
 *   Thread-safe wrapper around adv7842_program_input_internal(). Updates
 *   libadv7842's static `curStd` to `*result_std`.
 *
 * Parameters:
 *   handle     - ADV7842_t* cast to int.
 *   input      - Desired ADV7842_input_type_t.
 *   result_std - [in/out] Expected/detected video_std_t.
 *
 * Returns:
 *   return_code_t from adv7842_program_input_internal().
 *
 * Callers:
 *   prepare_input_detect(), sample_event_callback(), main() in main.c.
 */
return_code_t adv7842_program_input(int handle,ADV7842_input_type_t input,video_std_t *result_std)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: return_code_t rc@[???] */
  pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
  rVar1 = adv7842_program_input_internal(handle,input,result_std);
  curStd = *result_std;
  pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
  return rVar1;
}



// ==== adv7842_get_input @ 00412284 ====
/*
 * adv7842_get_input():
 *   Maps ADV7842_t::get_current_input_source() (enum_xc_input_type_t) back to
 *   ADV7842_input_type_t using bitmasks over values 22..45.
 *
 * Parameters:
 *   handle     - ADV7842_t* cast to int.
 *   pInputType - [out] Receives ADV7842_INPUT_TYPE_{COMPONENT,HDMI,CVBS,NONE}.
 *
 * Returns:
 *   RET_OK (0) always (writes ADV7842_INPUT_TYPE_NONE on unknown source).
 *
 * Callers:
 *   adv7842_thread().
 *
 * BUG:
 *   Component mask 0x5af0 (bits 4,5,6,7,9,11,12,14 relative to 22) omits bit 13
 *   (XC_INPUT_HDMI_COMPONENT_1080P = 35), so 1080p60 component is misreported
 *   as ADV7842_INPUT_TYPE_NONE.
 *
 * Locals:
 *   param2   - xc_input: current enum_xc_input_type_t from ADV7842_t
 *   uVar1    - 1 << (xc_input - XC_INPUT_HDMI_COMPOSITE_480I)
 *   local_18 - input_type: resolved ADV7842_input_type_t
 */
return_code_t adv7842_get_input(int handle,ADV7842_input_type_t *pInputType)

{
  enum_xc_input_type_t param2;
  uint uVar1;
  ADV7842_input_type_t local_18;
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: enum_xc_input_type_t xc_input@[???]
                       Unresolved local var: ADV7842_input_type_t input_type@[???] */
  param2 = ADV7842_t::get_current_input_source((ADV7842_t *)handle);
  if (param2 - XC_INPUT_HDMI_COMPOSITE_480I < 0x18) { // range 22..45
    uVar1 = 1 << (param2 - XC_INPUT_HDMI_COMPOSITE_480I & 0x1f);
    if ((uVar1 & 0x5af0) != 0) {               // Component 480i/480p/576i/576p/720p/720p50/1080i/1080i50
      local_18 = ADV7842_INPUT_TYPE_COMPONENT;
      goto LAB_0041237c;
    }
    if ((uVar1 & 0x840000) != 0) {             // HDMI_TO_HDMI (40) or HDMI_TO_HDMI_1080P (45)
      local_18 = ADV7842_INPUT_TYPE_HDMI;
      goto LAB_0041237c;
    }
    if ((uVar1 & 5) != 0) {                    // COMPOSITE_480I (22) or COMPOSITE_576I (24)
      local_18 = ADV7842_INPUT_TYPE_CVBS;
      goto LAB_0041237c;
    }
  }
  printf("%s: Unknown input type %d\n","adv7842_get_input",param2);
  local_18 = ADV7842_INPUT_TYPE_NONE;
LAB_0041237c:
  *pInputType = local_18;
  return RET_OK;
}



// ==== adv7842_detect_video_std_internal @ 004123a4 ====
/*
 * adv7842_detect_video_std_internal():
 *   Queries ADV7842_t::detect_video_standard() and maps the result via
 *   map_video_std(). Caller must hold `adv7842Mutex`.
 *
 * Parameters:
 *   handle    - ADV7842_t* cast to int.
 *   pInputStd - [out] Receives detected video_std_t.
 *
 * Returns:
 *   RET_OK (0).
 *
 * Callers:
 *   adv7842_thread(), adv7842_detect_video_std().
 */

/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */

return_code_t adv7842_detect_video_std_internal(int handle,video_std_t *pInputStd)

{
  video_std_t vVar1;
  enum_analog_digital_video_standard_t local_10 [2];
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: enum_analog_digital_video_standard_t
                       VideoStandard@[???]
                       Unresolved local var: video_std_t std@[???] */
  local_10[0] = ANALOG_DIGITAL_VIDEO_STANDARD_NONE;
  ADV7842_t::detect_video_standard((ADV7842_t *)handle,local_10);
  vVar1 = map_video_std(local_10[0]);
  *pInputStd = vVar1;
  return RET_OK;
}



// ==== adv7842_detect_video_std @ 00412438 ====
/*
 * adv7842_detect_video_std():
 *   Thread-safe wrapper around adv7842_detect_video_std_internal().
 *
 * Callers:
 *   sample_event_callback(), main() in main.c.
 */
return_code_t adv7842_detect_video_std(int handle,video_std_t *pInputStd)

{
  pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
  adv7842_detect_video_std_internal(handle,pInputStd);
  pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
  return RET_OK;
}



// ==== adv7842_detect_audio_mode @ 004124c4 ====
/*
 * adv7842_detect_audio_mode():
 *   Queries ADV7842_t::detect_audio_mode() (which reads HDMI Map 0x18 & 0x36)
 *   and returns AUDIO_MODE_PCM (0) or AUDIO_MODE_COMPRESSED (1).
 *
 * Parameters:
 *   handle          - ADV7842_t* cast to int.
 *   pInputAudioMode - [out] Receives audio_mode_t.
 *
 * Returns:
 *   RET_OK (0).
 *
 * Callers:
 *   start_encoder() in main.c.
 */
return_code_t adv7842_detect_audio_mode(int handle,audio_mode_t *pInputAudioMode)

{
  uint local_c;
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: uint bCompressed@[???] */
  local_c = 0;
  ADV7842_t::detect_audio_mode((ADV7842_t *)handle,&local_c);
  if (local_c == 0) {
    *pInputAudioMode = AUDIO_MODE_PCM;
  }
  else {
    *pInputAudioMode = AUDIO_MODE_COMPRESSED;
  }
  return RET_OK;
}



// ==== adv7842_enum_input @ 00412558 ====
/*
 * adv7842_enum_input():
 *   Probes CVBS, Component, and (optionally) HDMI inputs in sequence under
 *   adv7842Mutex, recording each input with a non-NONE video standard into
 *   pInputInfo[], then programs ADV7842_INPUT_TYPE_NONE and resets curStd.
 *
 * Parameters:
 *   handle      - ADV7842_t* cast to int.
 *   pInputInfo  - [out] Caller-allocated array (must hold up to 3 entries).
 *   pInputCount - [out] Number of active inputs found (0..3).
 *   detectHdmi  - Non-zero to also probe HDMI.
 *
 * Returns:
 *   RET_OK (0).
 *
 * Callers:
 *   detect_input_source() in main.c, adv7842_thread().
 *
 * Bugs / quirks:
 *   - `local_c` is initialized to VIDEO_STD_480I before CVBS and COMPONENT
 *     probes, but is NOT reset before the HDMI probe (carries over whatever
 *     COMPONENT returned).
 *   - Does not check input `*pInputCount` capacity before writing `pInputInfo`.
 *
 * Locals:
 *   bVar1    - true if probe succeeded and detected a non-NONE video_std_t
 *   rVar2    - return_code_t from adv7842_program_input_internal()
 *   local_10 - count: number of active inputs recorded
 *   local_c  - std: video_std_t in/out for adv7842_program_input_internal()
 */
return_code_t
adv7842_enum_input(int handle,ADV7842_input_info_t *pInputInfo,uint *pInputCount,int detectHdmi)

{
  bool bVar1;
  return_code_t rVar2;
  uint local_10;
  video_std_t local_c;
  
                    /* Unresolved local var: int count@[???]
                       Unresolved local var: video_std_t std@[???] */
  local_c = VIDEO_STD_NONE;
  pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
  local_c = VIDEO_STD_480I;
  rVar2 = adv7842_program_input_internal(handle,ADV7842_INPUT_TYPE_CVBS,&local_c);
  if ((rVar2 == RET_OK) && (local_c != VIDEO_STD_NONE)) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    pInputInfo->input = ADV7842_INPUT_TYPE_CVBS;
    pInputInfo->video_std = local_c;
  }
  local_10 = (uint)bVar1;
  local_c = VIDEO_STD_480I;
  rVar2 = adv7842_program_input_internal(handle,ADV7842_INPUT_TYPE_COMPONENT,&local_c);
  if ((rVar2 == RET_OK) && (local_c != VIDEO_STD_NONE)) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  if (bVar1) {
    pInputInfo[local_10].input = ADV7842_INPUT_TYPE_COMPONENT;
    pInputInfo[local_10].video_std = local_c;
    local_10 = local_10 + 1;
  }
  if (detectHdmi != 0) {
    rVar2 = adv7842_program_input_internal(handle,ADV7842_INPUT_TYPE_HDMI,&local_c);
    if ((rVar2 == RET_OK) && (local_c != VIDEO_STD_NONE)) {
      bVar1 = true;
    }
    else {
      bVar1 = false;
    }
    if (bVar1) {
      pInputInfo[local_10].input = ADV7842_INPUT_TYPE_HDMI;
      pInputInfo[local_10].video_std = local_c;
      local_10 = local_10 + 1;
    }
  }
  adv7842_program_input_internal(handle,ADV7842_INPUT_TYPE_NONE,&local_c);
  curStd = VIDEO_STD_NONE;
  pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
  *pInputCount = local_10;
  return RET_OK;
}



// ==== adv7842_subscribe_event @ 004127b8 ====
/*
 * adv7842_subscribe_event():
 *   Subscribes to one or more ADV7842_event_id bits (masked by 0x1f). Lazily
 *   spawns `adv7842_thread` on the first subscription.
 *
 * Parameters:
 *   handle    - ADV7842_t* cast to int (passed as arg to adv7842_thread).
 *   eventMask - Bitmask of ADV7842_event_id flags (0x01..0x1f).
 *
 * Returns:
 *   RET_OK (0) on success, RET_ERR (1) if (eventMask & 0x1f) == 0 or pthread_create fails.
 *
 * Callers:
 *   prepare_input_detect(), sample_event_callback(), main() in main.c.
 *
 * BUG:
 *   If pthread_create() fails (iVar2 != 0), returns RET_ERR without unlocking
 *   `adv7842Mutex`.
 */
return_code_t adv7842_subscribe_event(int handle,uint eventMask)

{
  return_code_t rVar1;
  int iVar2;
  
  if ((eventMask & 0x1f) == 0) {
    rVar1 = RET_ERR;
  }
  else {
    pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
    if ((my_subscribed_event | eventMask & 0x1f) != my_subscribed_event) {
      if ((my_subscribed_event == 0) && (myThreadId == 0)) {
        exitMyThread = 0;
        iVar2 = pthread_create(&myThreadId,(pthread_attr_t *)0x0,adv7842_thread,(void *)handle);
        if (iVar2 != 0) {
          puts("Fail to create thread ");
          return RET_ERR;                      // BUG: adv7842Mutex left locked
        }
      }
      my_subscribed_event = my_subscribed_event | eventMask & 0x1f;
    }
    pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
    rVar1 = RET_OK;
  }
  return rVar1;
}



// ==== adv7842_unsubscribe_event @ 00412920 ====
/*
 * adv7842_unsubscribe_event():
 *   Clears the specified ADV7842_event_id bits (masked by 0x1f) from
 *   `my_subscribed_event` under `adv7842Mutex`. Does not stop `adv7842_thread`
 *   even if `my_subscribed_event` becomes 0.
 *
 * Parameters:
 *   handle    - Unused (ADV7842_t* cast to int).
 *   eventMask - Bitmask of ADV7842_event_id flags to clear.
 *
 * Returns:
 *   RET_OK (0), or RET_ERR (1) if (eventMask & 0x1f) == 0.
 *
 * Callers:
 *   sample_event_callback() in main.c.
 */
return_code_t adv7842_unsubscribe_event(int handle,uint eventMask)

{
  return_code_t rVar1;
  
  if ((eventMask & 0x1f) == 0) {
    rVar1 = RET_ERR;
  }
  else {
    pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
    if ((~(eventMask & 0x1f) & my_subscribed_event) == my_subscribed_event) {
      pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
    }
    else {
      my_subscribed_event = ~(eventMask & 0x1f) & my_subscribed_event;
      pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
    }
    rVar1 = RET_OK;
  }
  return rVar1;
}



// ==== adv7842_set_freerun_mode @ 00412a24 ====
/*
 * adv7842_set_freerun_mode():
 *   Delegates directly to ADV7842_t::set_free_run_mode(mode) (AUTO=0,
 *   FORCED_BLUESCREEN=1, FORCED_COLORBAR=2). Note: does not lock adv7842Mutex.
 *
 * Parameters:
 *   handle - ADV7842_t* cast to int.
 *   mode   - ADV7842_freerun_mode_t.
 *
 * Returns:
 *   RET_OK (0).
 *
 * Callers:
 *   sample_event_callback(), main() in main.c.
 */
return_code_t adv7842_set_freerun_mode(int handle,ADV7842_freerun_mode_t mode)

{
                    /* Unresolved local var: ADV7842_t * adv7842@[???] */
  ADV7842_t::set_free_run_mode((ADV7842_t *)handle,mode);
  return RET_OK;
}




/* #####################################################################
 * FILE: 05_xcodeio.cpp.c
 * ##################################################################### */

/*
 * 05_xcodeio.cpp.c  --  annotated decompilation of src/libadv7842/xcodeio.cpp
 * ============================================================================
 *
 * Original source: /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libadv7842/xcodeio.cpp
 *
 * Role in av_streamer:
 *   Singleton hardware-I/O wrapper (`xcode_io_t`) used by the ADV7842 front-end
 *   driver (`ADV7842_t` in 06_adv7842.cpp.c and `libadv7842.cpp` in 04_libadv7842.cpp.c)
 *   to talk to `/dev/vixs/xcodedrv` via the ViXS SDK C API (`xcodeapi.cpp` /
 *   07_xcodeapi.cpp.c). It provides:
 *     1. I2C master transactions (`i2c_read`, `i2c_write`, `i2c_write_then_read`)
 *        used to program and poll the ADV7842 HDMI/analog video receiver.
 *     2. Memory-mapped register (MMR) access (`mmreg_read`, `mmreg_write`,
 *        `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits`) used to toggle
 *        ViXS XCode4 GPIO 5 (MMR 0x1100/0x1104 bit 5 = 0x20, external audio mux).
 *     3. Frame-buffer memory helpers (`fb_write`, `fb_read`, `fb_move`, `fb_set`,
 *        `fb_addr_to_ptr`) wrapping the mapped XCode4 frame-buffer base pointer.
 *
 *   Note: The ViXS encoder path (`02_vixs_encoder.c.c`) does NOT use `xcode_io_t`;
 *   it opens `/dev/vixs/xcodedrv` and issues ioctls directly.
 *
 * Main data structure (`xcode_io_t`, 60 bytes from DWARF):
 *   +0x00  _vptr.xcode_io_t    (vtable @ 0x00436138, virtual destructor)
 *   +0x04  DebugDumpPrint      (int, uninitialized -> 0 in .bss; enables I2C error printf)
 *   +0x08  DebugPrint          (int, unused)
 *   +0x0c  FPGADebugDumpPrint  (int, unused)
 *   +0x10  HDMIDebugDumpPrint  (int, unused)
 *   +0x14  m_FRBase            (uchar*, mapped XCode4 frame-buffer base)
 *   +0x18  m_MMRBase           (uint*, mapped XCode4 MMR base)
 *   +0x1c  endian_flag         (int, 0 = native little-endian; 1 = byteswap 32-bit MMR)
 *   +0x20  m_Mutex             (pthread_mutex_t, 24 bytes; initialized/destroyed, NEVER locked)
 *   +0x38  m_XcodeIOHandle     (xcode_handle_t / int fd for /dev/vixs/xcodedrv)
 *
 * Globals owned:
 *   - `get_io()::XocdeIO` (guard variable) and `get_io::lexical_block_0::XocdeIO`
 *     (the singleton `xcode_io_t` instance in .bss; note original typo "XocdeIO").
 */

/*
 * xcode_io_t::xcode_io_t @ 00412a90
 *
 * Purpose:
 *   Constructor for the `xcode_io_t` singleton. Initializes `m_Mutex`, opens
 *   `/dev/vixs/xcodedrv` via `xcode_open_handle()`, maps both the frame buffer
 *   (`m_FRBase`) and memory-mapped registers (`m_MMRBase`), and clears `endian_flag`.
 * Parameters:
 *   this - pointer to `xcode_io_t` instance
 * Return value:
 *   void
 * Callers / Callees:
 *   Called by: `xcode_io_t::get_io()` (00412da4) on first access.
 *   Calls: `pthread_mutex_init`, `xcode_open_handle`, `xcode_get_frame_buffer_map_addr`,
 *          `xcode_get_mmr_map_addr`, `puts`, `printf`.
 * Side effects:
 *   Opens `/dev/vixs/xcodedrv`, maps FB & MMR into process address space, prints
 *   base addresses or error messages to stdout.
 * Quirks:
 *   - `DebugDumpPrint` and sibling debug flags are not initialized here (relying
 *     on `.bss` zero-initialization for the static singleton).
 *   - `xVar1` (`m_XcodeIOHandle`) is not checked for `<= 0` / `< 0` before calling
 *     the map functions.
 */
// ==== xcode_io_t::xcode_io_t @ 00412a90 ====

/* WARNING: Removing unreachable block (ram,0x00412c5c) */
/* WARNING: Restarted to delay deadcode elimination for space: stack */
/* DWARF original prototype: void xcode_io_t(xcode_io_t * this) */

void __thiscall xcode_io_t::xcode_io_t(xcode_io_t *this)

{
  xcode_handle_t xVar1;  // fd returned by xcode_open_handle()
  uchar *puVar2;         // mapped frame-buffer base address
  uint *puVar3;          // mapped MMR base address
  
  this->_vptr_xcode_io_t = (_func_int_varargs **)&PTR__xcode_io_t_00436138;
                    /* Unresolved local var: char[5] tststr@[???]
                       Unresolved local var: int * ip@[???] */
  pthread_mutex_init((pthread_mutex_t *)&this->m_Mutex,(pthread_mutexattr_t *)0x0);
  xVar1 = xcode_open_handle();
  this->m_XcodeIOHandle = xVar1;
  puVar2 = xcode_get_frame_buffer_map_addr(this->m_XcodeIOHandle);
  this->m_FRBase = puVar2;
  if (this->m_FRBase == (uchar *)0x0) {
    puts("Get FrameBuffer Base Error!");
  }
  else {
    printf("FrameBuffer Base = %X\n",(uint)this->m_FRBase);
  }
  puVar3 = xcode_get_mmr_map_addr(this->m_XcodeIOHandle);
  this->m_MMRBase = puVar3;
  if (this->m_MMRBase == (uint *)0x0) {
    puts("Get MMR Base Error!");
  }
  else {
    printf("MMR Base = %X\n",(uint)this->m_MMRBase);
  }
  this->endian_flag = 0;  // MIPS32-LE host matches XCode4 LE MMR layout
  return;
}



/*
 * xcode_io_t::~xcode_io_t @ 00412c80 (complete object destructor D1)
 *
 * Purpose:
 *   Closes `m_XcodeIOHandle` if non-zero and destroys `m_Mutex`.
 * Parameters:
 *   this      - pointer to `xcode_io_t`
 *   __in_chrg - GCC C++ ABI in-charge flag (unused here)
 * Callers / Callees:
 *   Called by: deleting destructor `~xcode_io_t` (00412d44) and `__cxa_atexit`
 *              cleanup registered in `get_io()`.
 *   Calls: `xcode_close_handle`, `pthread_mutex_destroy`.
 * Quirk:
 *   Checks `m_XcodeIOHandle != 0` instead of `>= 0` (if `open64` failed and
 *   returned `-1`, this would call `close(-1)`).
 */
// ==== xcode_io_t::~xcode_io_t @ 00412c80 ====

/* WARNING: Removing unreachable block (ram,0x00412d14) */
/* DWARF original prototype: void ~xcode_io_t(xcode_io_t * this, int __in_chrg) */

void __thiscall xcode_io_t::~xcode_io_t(xcode_io_t *this,int __in_chrg)

{
  this->_vptr_xcode_io_t = (_func_int_varargs **)&PTR__xcode_io_t_00436138;
  if (this->m_XcodeIOHandle != 0) {
    xcode_close_handle(this->m_XcodeIOHandle);
  }
  pthread_mutex_destroy((pthread_mutex_t *)&this->m_Mutex);
  return;
}



/*
 * xcode_io_t::~xcode_io_t @ 00412d44 (deleting destructor D0)
 *
 * Purpose:
 *   Virtual deleting destructor: runs `~xcode_io_t(this, __in_chrg)` then frees
 *   the object with `operator delete(this)`.
 * Callers:
 *   Vtable slot for deleting destructor (not reached for static singleton).
 */
// ==== xcode_io_t::~xcode_io_t @ 00412d44 ====

/* DWARF original prototype: void ~xcode_io_t(xcode_io_t * this, int __in_chrg) */

void __thiscall xcode_io_t::~xcode_io_t(xcode_io_t *this,int __in_chrg)

{
  ~xcode_io_t(this,__in_chrg);
  operator_delete(this);
  return;
}



/*
 * xcode_io_t::get_io @ 00412da4
 *
 * Purpose:
 *   Thread-safe Meyers singleton accessor returning `&XocdeIO`.
 * Return value:
 *   Pointer to the single static `xcode_io_t` instance.
 * Callers / Callees:
 *   Called by: `ADV7842_t::ADV7842_t` (both constructors in 06_adv7842.cpp.c).
 *   Calls: `__cxa_guard_acquire`, `xcode_io_t::xcode_io_t`, `__cxa_guard_release`,
 *          `__cxa_atexit`.
 * Threading:
 *   Static initialization is guarded by GCC's `__cxa_guard_acquire` / `release`.
 */
// ==== xcode_io_t::get_io @ 00412da4 ====

xcode_io_t * xcode_io_t::get_io(void)

{
  int iVar1;  // __cxa_guard_acquire result (1 = this thread must construct)
  
  if ((get_io()::XocdeIO == '\0') && (iVar1 = __cxa_guard_acquire(&get_io()::XocdeIO), iVar1 != 0))
  {
                    /* try { // try from 00412e14 to 00412e1b has its CatchHandler @ 00412e90 */
    xcode_io_t(&get_io::lexical_block_0::XocdeIO);
    __cxa_guard_release(&get_io()::XocdeIO);
    __cxa_atexit(~xcode_io_t,&get_io::lexical_block_0::XocdeIO,&__dso_handle);
  }
  return &get_io::lexical_block_0::XocdeIO;
}



/*
 * xcode_io_t::i2c_read @ 00412ed4
 *
 * Purpose:
 *   Performs an I2C read of `n` bytes from `Address` on `I2CChannel` via
 *   `xcode_i2c_read` with hard-coded `atomic_period = 0x43` (67) and
 *   `timeout = 300` ms.
 * Parameters:
 *   this       - `xcode_io_t` instance
 *   I2CChannel - ViXS XCode4 I2C bus/block number (1 for ADV7842)
 *   Address    - 8-bit shifted I2C slave address (e.g. `ADV7842_I2CAddress << 1`)
 *   data       - destination buffer for read bytes
 *   n          - byte count (max 64 in driver struct, unchecked)
 *   start      - 1 to emit I2C START condition
 *   stop       - 1 to emit I2C STOP condition
 * Return value:
 *   0 on success (`IOCTL_RET_CMD_COMPLETE`), otherwise driver `return_value`.
 * Callers / Callees:
 *   Called by: `xcode_io_t::i2c_write_then_read`.
 *   Calls: `xcode_i2c_read`, `printf` (only if `DebugDumpPrint == 1`).
 */
// ==== xcode_io_t::i2c_read @ 00412ed4 ====

/* DWARF original prototype: int i2c_read(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_read
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;  // return code from xcode_i2c_read (0 = success)
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_read(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if ((iVar1 != 0) && (this->DebugDumpPrint == 1)) {
    printf("I2C_READ failed @ %2.2X\n",Address);
  }
  return iVar1;
}



/*
 * xcode_io_t::i2c_write @ 00412fbc
 *
 * Purpose:
 *   Performs an I2C write of `n` bytes to `Address` on `I2CChannel` via
 *   `xcode_i2c_write` with `atomic_period = 0x43` (67) and `timeout = 300` ms.
 * Parameters:
 *   this, I2CChannel, Address, data, n, start, stop - same as `i2c_read`.
 * Return value:
 *   0 on success, otherwise driver `return_value`.
 * Callers / Callees:
 *   Called by: `ADV7842_t::write_register` (06_adv7842.cpp.c) and
 *              `xcode_io_t::i2c_write_then_read`.
 *   Calls: `xcode_i2c_write`, `printf`.
 * Quirk:
 *   Suppresses the `I2C_WRITE failed` message when `Address == 0x40` (ADV7842 IO
 *   map 7-bit 0x20 << 1, likely because software reset at 0x40 reg 0xFF drops
 *   the I2C ACK).
 */
// ==== xcode_io_t::i2c_write @ 00412fbc ====

/* DWARF original prototype: int i2c_write(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;  // return code from xcode_i2c_write (0 = success)
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_write(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if (((iVar1 != 0) && (Address != 0x40)) && (this->DebugDumpPrint == 1)) {
    printf("I2C_WRITE failed @ %2.2X\n",Address);
  }
  return iVar1;
}



/*
 * xcode_io_t::i2c_write_then_read @ 004130b4
 *
 * Purpose:
 *   Combined I2C register-read transaction: writes `wn` bytes (typically 1
 *   register offset) with `stop = 0` (no STOP), then if that succeeds, issues
 *   a repeated-START (`start = 1`) read of `rn` bytes with `stop`.
 * Parameters:
 *   this       - `xcode_io_t` instance
 *   I2CChannel - I2C bus/block number
 *   Address    - 8-bit I2C slave address
 *   wdata      - write buffer (register subaddress)
 *   rdata      - read destination buffer
 *   wn, rn     - write and read byte counts
 *   start      - START flag for the initial write phase
 *   stop       - STOP flag for the trailing read phase
 * Return value:
 *   0 on success, or non-zero error code from `i2c_write` or `i2c_read`.
 * Callers / Callees:
 *   Called by: `ADV7842_t::read_register` (06_adv7842.cpp.c).
 *   Calls: `xcode_io_t::i2c_write`, `xcode_io_t::i2c_read`.
 */
// ==== xcode_io_t::i2c_write_then_read @ 004130b4 ====

/* DWARF original prototype: int i2c_write_then_read(xcode_io_t * this, int I2CChannel, int Address,
   void * wdata, void * rdata, size_t wn, size_t rn, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write_then_read
          (xcode_io_t *this,int I2CChannel,int Address,void *wdata,void *rdata,size_t wn,size_t rn,
          int start,int stop)

{
  int iVar1;  // error status from i2c_write / i2c_read
  
                    /* Unresolved local var: int error@[???] */
  iVar1 = i2c_write(this,I2CChannel,Address,wdata,wn,start,0);
  if (iVar1 == 0) {
    iVar1 = i2c_read(this,I2CChannel,Address,rdata,rn,1,stop);
  }
  return iVar1;
}



/*
 * byteswap @ 0041316c
 *
 * Purpose:
 *   In-place 32-bit endianness swap of the 4 bytes pointed to by `ptr`.
 * Parameters:
 *   ptr - pointer to 4-byte buffer
 * Callers:
 *   `xcode_io_t::mmreg_write`, `xcode_io_t::mmreg_read` (when `endian_flag != 0`).
 */
// ==== byteswap @ 0041316c ====

void byteswap(char *ptr)

{
  char cVar1;  // temp byte for swap
  
                    /* Unresolved local var: char c@[???] */
  cVar1 = *ptr;
  *ptr = ptr[3];
  ptr[3] = cVar1;
  cVar1 = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = cVar1;
  return;
}



/*
 * xcode_io_t::mmreg_write @ 004131f8
 *
 * Purpose:
 *   Writes a 32-bit `value` to XCode4 MMR offset `reg_addr` (4-byte aligned via
 *   `& 0xfffffffc`), byte-swapping first if `endian_flag != 0`.
 * Parameters:
 *   this     - `xcode_io_t` instance
 *   reg_addr - byte offset from `m_MMRBase`
 *   value    - 32-bit value to write
 * Callers:
 *   `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits` (when `endian_flag != 0`).
 *   Unused externally in `av_streamer`.
 */
// ==== xcode_io_t::mmreg_write @ 004131f8 ====

/* DWARF original prototype: void mmreg_write(xcode_io_t * this, uint reg_addr, uint value) */

void __thiscall xcode_io_t::mmreg_write(xcode_io_t *this,uint reg_addr,uint value)

{
  uint local_res8 [2];  // stack copy of value for optional in-place byteswap
  
  if (this->m_MMRBase != (uint *)0x0) {
    local_res8[0] = value;
    if (this->endian_flag != 0) {
      byteswap((char *)local_res8);
    }
    *(uint *)((int)this->m_MMRBase + (reg_addr & 0xfffffffc)) = local_res8[0];
  }
  return;
}



/*
 * xcode_io_t::mmreg_read @ 0041327c
 *
 * Purpose:
 *   Reads a 32-bit value from XCode4 MMR offset `reg_addr` (4-byte aligned),
 *   byte-swapping if `endian_flag != 0`. Returns 0 if `m_MMRBase == NULL`.
 * Callers:
 *   `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits` (when `endian_flag != 0`).
 *   Unused externally in `av_streamer`.
 */
// ==== xcode_io_t::mmreg_read @ 0041327c ====

/* DWARF original prototype: uint mmreg_read(xcode_io_t * this, uint reg_addr) */

uint __thiscall xcode_io_t::mmreg_read(xcode_io_t *this,uint reg_addr)

{
  uint local_10 [2];  // read-back register value (swapped in-place if endian_flag != 0)
  
  if (this->m_MMRBase == (uint *)0x0) {
    local_10[0] = 0;
  }
  else if (this->endian_flag == 0) {
    local_10[0] = *(uint *)((int)this->m_MMRBase + (reg_addr & 0xfffffffc));
  }
  else {
                    /* Unresolved local var: int tmp@[???] */
    local_10[0] = *(uint *)((int)this->m_MMRBase + (reg_addr & 0xfffffffc));
    byteswap((char *)local_10);
  }
  return local_10[0];
}



/*
 * xcode_io_t::clear_reg_mask @ 00413330
 *
 * Purpose:
 *   Clears bits specified by `mask` in MMR `reg_addr` (`reg &= ~mask`).
 *   Unused in `av_streamer`.
 * Threading:
 *   Non-atomic read-modify-write; `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::clear_reg_mask @ 00413330 ====

/* DWARF original prototype: void clear_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::clear_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;  // temp register value in byteswap path
  
  if (this->m_MMRBase != (uint *)0x0) {
    if (this->endian_flag == 0) {
      this->m_MMRBase[reg_addr >> 2] = this->m_MMRBase[reg_addr >> 2] & ~mask;
    }
    else {
                    /* Unresolved local var: uint tmpval@[???] */
      uVar1 = mmreg_read(this,reg_addr);
      mmreg_write(this,reg_addr,uVar1 & ~mask);
    }
  }
  return;
}



/*
 * xcode_io_t::set_reg_mask @ 00413404
 *
 * Purpose:
 *   Sets bits specified by `mask` in MMR `reg_addr` (`reg |= mask`).
 *   Unused in `av_streamer`.
 * Threading:
 *   Non-atomic read-modify-write; `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::set_reg_mask @ 00413404 ====

/* DWARF original prototype: void set_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::set_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;  // temp register value in byteswap path
  
  if (this->m_MMRBase != (uint *)0x0) {
    if (this->endian_flag == 0) {
      this->m_MMRBase[reg_addr >> 2] = this->m_MMRBase[reg_addr >> 2] | mask;
    }
    else {
                    /* Unresolved local var: uint tmpval@[???] */
      uVar1 = mmreg_read(this,reg_addr);
      mmreg_write(this,reg_addr,uVar1 | mask);
    }
  }
  return;
}



/*
 * xcode_io_t::write_reg_mask_bits @ 004134d0
 *
 * Purpose:
 *   Clears `mask` bits and ORs in `value` in MMR `reg_addr`.
 * Callers:
 *   - `04_libadv7842.cpp.c`: `adv7842_program_input_internal` sets MMR `0x1100`
 *     mask `0x20` to `0x20` (HDMI audio) or `0` (external audio), and `0x1104`
 *     mask `0x20` to `0x20` (GPIO 5 output enable).
 *   - `06_adv7842.cpp.c`: `ADV7842_t::ADV7842_t(int)` and `program_input_source`
 *     similarly drive GPIO 5 via `0x1100`/`0x1104`.
 * Quirks:
 *   - Does NOT mask `value` with `mask` (`value` is ORed directly).
 *   - In the `endian_flag == 0` path, performs two separate writes to `m_MMRBase`
 *     (first clearing `mask`, then ORing `value`), creating a brief glitch window
 *     where the masked bits are 0.
 *   - `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::write_reg_mask_bits @ 004134d0 ====

/* DWARF original prototype: void write_reg_mask_bits(xcode_io_t * this, uint reg_addr, uint mask,
   uint value) */

void __thiscall xcode_io_t::write_reg_mask_bits(xcode_io_t *this,uint reg_addr,uint mask,uint value)

{
  uint uVar1;  // temp register value in byteswap path
  
  if (this->m_MMRBase != (uint *)0x0) {
    if (this->endian_flag == 0) {
      this->m_MMRBase[reg_addr >> 2] = this->m_MMRBase[reg_addr >> 2] & ~mask;
      this->m_MMRBase[reg_addr >> 2] = this->m_MMRBase[reg_addr >> 2] | value;
    }
    else {
                    /* Unresolved local var: uint tmpval@[???] */
      uVar1 = mmreg_read(this,reg_addr);
      mmreg_write(this,reg_addr,uVar1 & ~mask | value);
    }
  }
  return;
}



/*
 * xcode_io_t::fb_write @ 004135f0
 *
 * Purpose:
 *   Copies `size` bytes from host `buf` into XCode4 frame-buffer memory at
 *   offset `fb_addr`. Returns destination pointer or NULL if `m_FRBase == NULL`.
 *   Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_write @ 004135f0 ====

/* DWARF original prototype: void * fb_write(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_write(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;  // memcpy return value (destination pointer) or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(this->m_FRBase + fb_addr,buf,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_read @ 00413680
 *
 * Purpose:
 *   Copies `size` bytes from XCode4 frame-buffer offset `fb_addr` to host `buf`.
 *   Returns `buf` or NULL if `m_FRBase == NULL`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_read @ 00413680 ====

/* DWARF original prototype: void * fb_read(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_read(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;  // memcpy return value (buf) or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(buf,this->m_FRBase + fb_addr,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_move @ 00413710
 *
 * Purpose:
 *   `memmove` within XCode4 frame-buffer memory from `src_fb_addr` to
 *   `dst_fb_addr`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_move @ 00413710 ====

/* DWARF original prototype: void * fb_move(xcode_io_t * this, uint dst_fb_addr, uint src_fb_addr,
   size_t size) */

void * __thiscall
xcode_io_t::fb_move(xcode_io_t *this,uint dst_fb_addr,uint src_fb_addr,size_t size)

{
  void *pvVar1;  // memmove return value or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memmove(this->m_FRBase + dst_fb_addr,this->m_FRBase + src_fb_addr,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_set @ 004137b0
 *
 * Purpose:
 *   `memset` `size` bytes at XCode4 frame-buffer offset `fb_addr` to `value`.
 *   Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_set @ 004137b0 ====

/* DWARF original prototype: void * fb_set(xcode_io_t * this, uint fb_addr, int value, size_t size)
    */

void * __thiscall xcode_io_t::fb_set(xcode_io_t *this,uint fb_addr,int value,size_t size)

{
  void *pvVar1;  // memset return value or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memset(this->m_FRBase + fb_addr,value,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_addr_to_ptr @ 00413840
 *
 * Purpose:
 *   Converts a frame-buffer offset `fb_addr` to a user-space virtual pointer
 *   (`m_FRBase + fb_addr`), or NULL if `m_FRBase == NULL`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_addr_to_ptr @ 00413840 ====

/* DWARF original prototype: void * fb_addr_to_ptr(xcode_io_t * this, uint fb_addr) */

void * __thiscall xcode_io_t::fb_addr_to_ptr(xcode_io_t *this,uint fb_addr)

{
  uchar *puVar1;  // virtual pointer into mapped frame buffer, or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    puVar1 = (uchar *)0x0;
  }
  else {
    puVar1 = this->m_FRBase + fb_addr;
  }
  return puVar1;
}




/* #####################################################################
 * FILE: 06_adv7842.cpp.c
 * ##################################################################### */

/*
 * 06_adv7842.cpp.c  --  annotated decompilation of adv7842.cpp
 * ============================================================================
 *
 * Original source file:
 *   /media/WD_passport/Meld_AV_Script/2p14_a/avstreamer/src/libadv7842/adv7842.cpp
 *
 * Role in av_streamer:
 *   Implements the C++ class ADV7842_t, which drives the Analog Devices ADV7842
 *   HDMI/component/composite/S-Video front-end receiver over I2C via the
 *   xcode_io_t singleton (05_xcodeio.cpp.c -> /dev/vixs/xcodedrv ioctls).
 *   All methods are called from the libadv7842 C wrapper and event-polling
 *   thread in 04_libadv7842.cpp.c (adv7842_init, adv7842_program_input,
 *   adv7842_detect_video_std, adv7842_detect_audio_mode, adv7842_set_freerun_mode,
 *   adv7842_get_input).
 *
 * Main data structures (from DWARF):
 *   ADV7842_t (16 bytes):
 *     +0x00  enum_xc_input_type_t                 m_InputSource
 *     +0x04  enum_analog_digital_video_standard_t m_VideoStandard
 *     +0x08  uint                                 m_I2CChannel  (0 in MT300-AV)
 *     +0x0c  xcode_io_t *                         xcodeIO
 *
 *   ADV7842 I2C slave map (7-bit address -> 8-bit write address, programmed in
 *   InitHDMI / InitMode on IO map 0x20 / 0x40):
 *     0x20 (8-bit 0x40): IO Map (fixed default)
 *     0x22 (8-bit 0x44): CP Map (Component Processor, reg 0xFD = 0x44)
 *     0x24 (8-bit 0x48): VDP Map (reg 0xFE = 0x48)
 *     0x26 (8-bit 0x4C): AFE Map (reg 0xF8 = 0x4C)
 *     0x32 (8-bit 0x64): KSV / Repeater Map (reg 0xF9 = 0x64)
 *     0x34 (8-bit 0x68): HDMI Map (reg 0xFB = 0x68)
 *     0x36 (8-bit 0x6C): EDID Map (reg 0xFA = 0x6C)
 *     0x3E (8-bit 0x7C): INFOFRAME Map (reg 0xF5 = 0x7C)
 *     0x42 (8-bit 0x84): AVLINK Map (reg 0xF3 = 0x84)
 *     0x44 (8-bit 0x88): CEC Map (reg 0xF4 = 0x88)
 *     0x48 (8-bit 0x90): SDP Map (Standard Definition Processor, reg 0xF1 = 0x90)
 *     0x4A (8-bit 0x94): SDPIO Map (reg 0xF2 = 0x94)
 *
 * Globals owned by adv7842.cpp (from DWARF):
 *   adv7842_DebugDump       (int @ 0x4379a0)     - enables verbose stdout prints when == 1
 *   curr_vidmode_buff       (char[256] @ 0x437178) - scratch buffer for video-detect logs
 *   log_buff                (char[256] @ 0x437278) - scratch buffer for general logs
 *   adv7842load_EDID_tab    (uchar[840] @ 0x436e30) - default/loaded EDID I2C script (3-byte tuples)
 *   adv7842mode_*_tab       (uchar[] @ 0x436xxx)    - per-input register tables (3-byte tuples)
 *
 * External globals referenced:
 *   SigTerm                 (volatile sig_atomic_t, defined in main.c)
 *   g_llc_delay             (int, defined in main.c, set via -l <n> CLI flag)
 */

/*
 * ADV7842_t::ADV7842_t (default constructor) @ 004138a0
 *
 * Purpose:
 *   Default-constructs ADV7842_t on I2C channel 1 and initializes the ADV7842
 *   I2C register map if not already configured.
 *   NOTE: DEAD CODE in this binary; adv7842_init() invokes the 2-arg
 *   constructor ADV7842_t(this, 0) instead. Unlike the 2-arg constructor, this
 *   overload does NOT initialize m_InputSource, m_VideoStandard, or GPIO 5.
 *
 * Parameters:
 *   this - pointer to uninitialized 16-byte ADV7842_t object
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: none observed in split units
 *   Callees: xcode_io_t::get_io, read_register, InitHDMI
 *
 * Side effects:
 *   Reads IO map 0x20 reg 0xFB; if != 0x68 ('h'), calls InitHDMI() which resets
 *   the ADV7842 and programs the I2C slave map.
 */
void __thiscall ADV7842_t::ADV7842_t(ADV7842_t *this)

{
  xcode_io_t *pxVar1;                // singleton xcode_io_t*
  uchar local_10 [8];                // bTemp: 1-byte register read buffer
  
                    /* Unresolved local var: uchar bTemp@[???] */
  this->m_I2CChannel = 1;
  pxVar1 = xcode_io_t::get_io();
  this->xcodeIO = pxVar1;
  if (adv7842_DebugDump == 1) {
    puts("//====== Start ADV7842_t ======");
  }
  // Check if HDMI_SLAVE_ADDR (IO map 0x20, reg 0xFB) is already mapped to 0x68 ('h')
  read_register(this,0x20,0xfb,local_10);
  if (local_10[0] != 'h') {
    InitHDMI(this);
  }
  return;
}



/*
 * ADV7842_t::ADV7842_t (channel constructor) @ 00413974
 *
 * Purpose:
 *   Constructs ADV7842_t bound to a specific XCode4 I2C bus channel, forces
 *   external audio clock selection via GPIO 5, and initializes the ADV7842 I2C
 *   slave map if not already mapped.
 *   NOTE: m_VideoStandard is left uninitialized.
 *
 * Parameters:
 *   this       - pointer to uninitialized 16-byte ADV7842_t object
 *   i2cChannel - XCode4 I2C channel index (0 passed by adv7842_init)
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: adv7842_init (04_libadv7842.cpp.c)
 *   Callees: xcode_io_t::get_io, xcode_io_t::write_reg_mask_bits,
 *            read_register, InitHDMI
 *
 * Side effects:
 *   Writes XCode4 MMIO GPIO registers 0x1100 (data) and 0x1104 (direction)
 *   bit 5 (mask 0x20) to drive GPIO 5 LOW (external PCM DAC / XC4208 master
 *   audio clock). Logs to stdout and syslog(LOG_NOTICE). May reset ADV7842 via
 *   InitHDMI().
 */
void __thiscall ADV7842_t::ADV7842_t(ADV7842_t *this,int i2cChannel)

{
  xcode_io_t *pxVar1;                // singleton xcode_io_t*
  uchar local_10 [8];                // bTemp: 1-byte register read buffer
  
                    /* Unresolved local var: uchar bTemp@[???] */
  this->m_I2CChannel = i2cChannel;
  pxVar1 = xcode_io_t::get_io();
  this->xcodeIO = pxVar1;
  this->m_InputSource = XC_INPUT_NONE;
  sprintf(log_buff,"adv7842.cpp : Select External Audio - Setting GPIO 5 to LOW, SigTerm = %d",
          SigTerm);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  // Drive XCode4 GPIO 5 LOW (0x1100 = data reg, 0x1104 = output-enable reg, inferred)
  xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1100,0x20,0);
  xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1104,0x20,0x20);
  if (adv7842_DebugDump == 1) {
    puts("//====== Start ADV7842_t ======");
  }
  // Check if HDMI_SLAVE_ADDR (IO map 0x20, reg 0xFB) is already 0x68 ('h')
  read_register(this,0x20,0xfb,local_10);
  if (local_10[0] != 'h') {
    InitHDMI(this);
  }
  return;
}



/*
 * ADV7842_t::~ADV7842_t @ 00413b20
 *
 * Purpose:
 *   Destructor for ADV7842_t (no-op; does not free xcodeIO singleton).
 *
 * Parameters:
 *   this      - ADV7842_t instance
 *   __in_chrg - GCC ABI in-charge flag (unused)
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: adv7842_deinit (04_libadv7842.cpp.c)
 *   Callees: none
 */
void __thiscall ADV7842_t::~ADV7842_t(ADV7842_t *this,int __in_chrg)

{
  return;
}



/*
 * ADV7842_t::reset_ADV7842 @ 00413b44
 *
 * Purpose:
 *   Empty stub (no-op). Actual reset is performed in InitHDMI() / InitMode()
 *   via IO map 0x20 reg 0xFF = 0x80.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: none
 *   Callees: none
 */
void __thiscall ADV7842_t::reset_ADV7842(ADV7842_t *this)

{
  return;
}



/*
 * ADV7842_t::get_current_input_source @ 00413b68
 *
 * Purpose:
 *   Accessor returning the currently configured input source.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   this->m_InputSource (enum_xc_input_type_t)
 *
 * Callers / Callees:
 *   Callers: adv7842_get_input (04_libadv7842.cpp.c)
 *   Callees: none
 */
enum_xc_input_type_t __thiscall ADV7842_t::get_current_input_source(ADV7842_t *this)

{
  return this->m_InputSource;
}



/*
 * ADV7842_t::set_free_run_mode @ 00413b94
 *
 * Purpose:
 *   Configures the ADV7842 free-run output mode (auto, forced blue screen,
 *   or color bar) on either the SDP map (for SD analog/component inputs) or
 *   the CP map (for HD component / HDMI inputs).
 *
 * Parameters:
 *   this        - ADV7842_t instance
 *   FreeRunMode - XC_FREE_RUN_AUTO (0), XC_FREE_RUN_BLUE_SCREEN (1), or
 *                 XC_FREE_RUN_COLOR_BAR (2)
 *
 * Returns:
 *   0 (always)
 *
 * Callers / Callees:
 *   Callers: adv7842_set_freerun_mode (04_libadv7842.cpp.c)
 *   Callees: read_register, write_register
 *
 * Side effects:
 *   Modifies SDP reg 0x48:0xDD or CP reg 0x22:0xBF via I2C.
 *
 * Bugs / Quirks:
 *   1. If m_InputSource == XC_INPUT_NONE (0), (uVar1 & 1) != 0 jumps directly
 *      to write_register with UNINITIALIZED local_10, local_c, and local_b[0].
 *   2. XC_INPUT_HDMI_S_VIDEO_480I (0x12 = 18) is NOT in bitmask 0x15400000,
 *      so it falls through to CP map 0x22:0xBF instead of SDP map 0x48:0xDD.
 *   3. On the SDP path, XC_FREE_RUN_COLOR_BAR (2) leaves local_b[0] unchanged
 *      instead of setting color-bar bits.
 */
int __thiscall ADV7842_t::set_free_run_mode(ADV7842_t *this,enum_xc_free_run_mode_t FreeRunMode)

{
  uint uVar1;                        // bitmask (1 << m_InputSource) for input-class check
  uint local_10;                     // i2cAddress (7-bit: 0x48 SDP or 0x22 CP)
  uchar local_c;                     // regAddr (0xDD SDP or 0xBF CP)
  byte local_b [3];                  // regData (read-modify-write buffer)
  
                    /* Unresolved local var: uint i2cAddress@[???]
                       Unresolved local var: uchar regAddr@[???]
                       Unresolved local var: uchar regData@[???] */
  // Check inputs < 29 (XC_INPUT_HDMI_COMPONENT_576P); Ghidra renders 0x1f mask as enum 31
  if (this->m_InputSource < XC_INPUT_HDMI_COMPONENT_576P) {
    uVar1 = 1 << (this->m_InputSource & XC_INPUT_HDMI_COMPONENT_720P);
    // Bitmask 0x15400000 = bits 22 (COMPOSITE_480I), 24 (COMPOSITE_576I),
    //                      26 (COMPONENT_480I), 28 (COMPONENT_576I) -> SDP path
    if ((uVar1 & 0x15400000) != 0) {
      local_10 = 0x48;               // 7-bit SDP map (8-bit 0x90)
      local_c = 0xdd;                // SDP free-run control register
      read_register(this,0x48,0xdd,local_b);
      if (FreeRunMode == XC_FREE_RUN_BLUE_SCREEN) {
        local_b[0] = local_b[0] & 0xfc | 1;
      }
      else if (FreeRunMode != XC_FREE_RUN_COLOR_BAR) {
        local_b[0] = local_b[0] & 0xfc;
      }
      goto LAB_00413d0c;
    }
    // BUG: if m_InputSource == XC_INPUT_NONE (0), jumps with uninitialized locals!
    if ((uVar1 & 1) != 0) goto LAB_00413d0c;
  }
  // CP / HDMI / HD component path: CP map 0x22 (8-bit 0x44), reg 0xBF (CP free-run)
  local_10 = 0x22;
  local_c = 0xbf;
  read_register(this,0x22,0xbf,local_b);
  if (FreeRunMode - XC_FREE_RUN_BLUE_SCREEN < 2) {
    local_b[0] = local_b[0] | 1;     // force free-run (blue screen / color bar)
  }
  else {
    local_b[0] = local_b[0] & 0xfe;  // auto free-run
  }
LAB_00413d0c:
  write_register(this,local_10,local_c,local_b[0]);
  return 0;
}



/*
 * ADV7842_t::program_input_source @ 00413d48
 *
 * Purpose:
 *   Switches the ADV7842 to the requested input source. For HDMI inputs,
 *   resets GPIO 5 LOW (external audio default), enables the internal EDID on
 *   all 4 HDMI ports (KSV map 0x32 reg 0x77 = 0x0F), waits 1s, and verifies
 *   reg 0x7D == 0x0F before calling LoadHDMI(). For analog inputs, sets GPIO 5
 *   LOW and calls LoadHDMI().
 *
 * Parameters:
 *   this        - ADV7842_t instance
 *   InputSource - target enum_xc_input_type_t
 *
 * Returns:
 *   0 on success (or if InputSource == XC_INPUT_NONE), -1 if internal EDID
 *   failed to enable on HDMI input.
 *
 * Callers / Callees:
 *   Callers: adv7842_program_input_internal (04_libadv7842.cpp.c)
 *   Callees: xcode_io_t::write_reg_mask_bits, write_register, read_register,
 *            LoadHDMI, usleep
 *
 * Side effects:
 *   Updates this->m_InputSource, sets GPIO 5 LOW, writes KSV 0x32:0x77 = 0x0F,
 *   sleeps 1s on HDMI, and programs full mode register table via LoadHDMI().
 */
int __thiscall ADV7842_t::program_input_source(ADV7842_t *this,enum_xc_input_type_t InputSource)

{
  uchar local_10 [8];                // bTemp: readback of KSV reg 0x7D
  
                    /* Unresolved local var: uchar bTemp@[???] */
  this->m_InputSource = InputSource;
  if (adv7842_DebugDump == 1) {
    printf("//====== Start program_input_source = %X ======\n",InputSource);
  }
  if ((((InputSource == XC_INPUT_HDMI_TO_HDMI) || (InputSource == XC_INPUT_HDMI_TO_HDMI_480I)) ||
      (InputSource == XC_INPUT_HDMI_TO_HDMI_576I)) ||
     (((InputSource == XC_INPUT_HDMI_TO_HDMI_480P || (InputSource == XC_INPUT_HDMI_TO_HDMI_576P)) ||
      (InputSource == XC_INPUT_HDMI_TO_HDMI_1080P)))) {
    memcpy(log_buff,"adv7842.cpp : Select External Audio - Setting GPIO 5 to LOW",0x3c);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1100,0x20,0);
    xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1104,0x20,0x20);
    memcpy(log_buff,"adv7842.cpp : Enable internal EDID",0x23);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    // KSV/Repeater map 0x32 (8-bit 0x64), reg 0x77 ('w'): enable internal EDID for ports A-D (0x0F)
    write_register(this,0x32,'w','\x0f');
    usleep(1000000);
    // Read back KSV map 0x32 reg 0x7D ('}'): internal EDID enable status
    read_register(this,0x32,'}',local_10);
    if (local_10[0] != '\x0f') {
      memcpy(log_buff,"adv7842.cpp : ERROR : Internal EDID not enabled",0x30);
      puts(log_buff);
      syslog(5,"%s",log_buff);
      return -1;
    }
    memcpy(log_buff,"adv7842.cpp : Internal EDID has been enabled for all the ports",0x3f);
    puts(log_buff);
    syslog(5,"%s",log_buff);
  }
  else {
    if (InputSource == XC_INPUT_NONE) {
      return 0;
    }
    memcpy(log_buff,"adv7842.cpp : Select External Audio - Setting GPIO 5 to LOW",0x3c);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1100,0x20,0);
    xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1104,0x20,0x20);
  }
  LoadHDMI(this,(uchar)InputSource);
  return 0;
}



/*
 * ADV7842_t::set_video_standard @ 004141b0
 *
 * Purpose:
 *   Setter for m_VideoStandard. NOTE: No callers observed in the split units.
 *
 * Parameters:
 *   this          - ADV7842_t instance
 *   VideoStandard - enum_analog_digital_video_standard_t value to store
 *
 * Returns:
 *   0 (always)
 *
 * Callers / Callees:
 *   Callers: none observed
 *   Callees: none
 */
/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */
/* DWARF original prototype: int set_video_standard(ADV7842_t * this,
   enum_analog_digital_video_standard_t VideoStandard) */

int __thiscall
ADV7842_t::set_video_standard(ADV7842_t *this,enum_analog_digital_video_standard_t VideoStandard)

{
  this->m_VideoStandard = VideoStandard;
  return 0;
}



/*
 * ADV7842_t::InitHDMI @ 004141e8
 *
 * Purpose:
 *   Resets the ADV7842 (IO map 0x20 reg 0xFF = 0x80, 300 ms delay) and
 *   programs all sub-map I2C slave addresses for HDMI mode.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   0 (always)
 *
 * Callers / Callees:
 *   Callers: ADV7842_t constructors, LoadHDMI
 *   Callees: write_register, usleep
 *
 * Side effects:
 *   Issues software reset to ADV7842 and configures I2C slave address map on
 *   IO map 0x20 (regs 0xF1..0xF5, 0xF8..0xFE).
 */
int __thiscall ADV7842_t::InitHDMI(ADV7842_t *this)

{
  if (adv7842_DebugDump == 1) {
    puts("//=== Initial Reset. Ignore this error message!");
  }
  write_register(this,0x20,0xff,0x80); // IO map MAIN_RESET
  usleep(300000);
  if (adv7842_DebugDump == 1) {
    puts("//=== HDMI Init Mode Power On, Initial Mapping");
  }
  write_register(this,0x20,0xf9,'d');  // KSV / Repeater map = 0x64 (7-bit 0x32)
  write_register(this,0x20,0xfa,'l');  // EDID map = 0x6C (7-bit 0x36)
  write_register(this,0x20,0xf1,0x90); // SDP map = 0x90 (7-bit 0x48)
  write_register(this,0x20,0xf2,0x94); // SDPIO map = 0x94 (7-bit 0x4A)
  write_register(this,0x20,0xf3,0x84); // AVLINK map = 0x84 (7-bit 0x42)
  write_register(this,0x20,0xf4,0x88); // CEC map = 0x88 (7-bit 0x44)
  write_register(this,0x20,0xf5,'|');  // INFOFRAME map = 0x7C (7-bit 0x3E)
  write_register(this,0x20,0xf8,'L');  // AFE map = 0x4C (7-bit 0x26)
  write_register(this,0x20,0xfb,'h');  // HDMI map = 0x68 (7-bit 0x34)
  write_register(this,0x20,0xfc,'`');  // IO reg 0xFC = 0x60 (HDMI-only init)
  write_register(this,0x20,0xfd,'D');  // CP map = 0x44 (7-bit 0x22)
  write_register(this,0x20,0xfe,'H');  // VDP map = 0x48 (7-bit 0x24)
  return 0;
}



/*
 * ADV7842_t::InitMode @ 0041440c
 *
 * Purpose:
 *   Resets the ADV7842 (IO map 0x20 reg 0xFF = 0x80, 300 ms delay) and
 *   programs all sub-map I2C slave addresses for analog/component mode.
 *   Identical to InitHDMI() except for log string and omitting IO reg 0xFC.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   0 (always)
 *
 * Callers / Callees:
 *   Callers: LoadHDMI
 *   Callees: write_register, usleep
 */
int __thiscall ADV7842_t::InitMode(ADV7842_t *this)

{
  if (adv7842_DebugDump == 1) {
    puts("//=== Initial Reset. Ignore this error message!");
  }
  write_register(this,0x20,0xff,0x80); // IO map MAIN_RESET
  usleep(300000);
  memcpy(log_buff,"adv7842.cpp : Analog Init Mode Power On, Initial Mapping",0x39);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  write_register(this,0x20,0xf1,0x90); // SDP map = 0x90 (7-bit 0x48)
  write_register(this,0x20,0xf2,0x94); // SDPIO map = 0x94 (7-bit 0x4A)
  write_register(this,0x20,0xf3,0x84); // AVLINK map = 0x84 (7-bit 0x42)
  write_register(this,0x20,0xf4,0x88); // CEC map = 0x88 (7-bit 0x44)
  write_register(this,0x20,0xf5,'|');  // INFOFRAME map = 0x7C (7-bit 0x3E)
  write_register(this,0x20,0xf8,'L');  // AFE map = 0x4C (7-bit 0x26)
  write_register(this,0x20,0xf9,'d');  // KSV / Repeater map = 0x64 (7-bit 0x32)
  write_register(this,0x20,0xfa,'l');  // EDID map = 0x6C (7-bit 0x36)
  write_register(this,0x20,0xfb,'h');  // HDMI map = 0x68 (7-bit 0x34)
  write_register(this,0x20,0xfd,'D');  // CP map = 0x44 (7-bit 0x22)
  write_register(this,0x20,0xfe,'H');  // VDP map = 0x48 (7-bit 0x24)
  return 0;
}



/*
 * ADV7842_t::LoadEDID (built-in table) @ 00414650
 *
 * Purpose:
 *   Loads the compiled-in adv7842load_EDID_tab into the ADV7842 EDID RAM and
 *   commits it to the SPI EEPROM. Used when no -e <file> is provided.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: adv7842_init (04_libadv7842.cpp.c)
 *   Callees: read_register, write_register, LoadHDMI, usleep
 *
 * Side effects:
 *   1. Sets HDMI map 0x34 reg 0x69 ('i', HPA_AUTO_INT_EDID) = 0xA4 (arms auto
 *      Hot-Plug-Assert on internal EDID), sleeps 1s.
 *   2. Calls LoadHDMI(this, 7 [XC_INPUT_EDID]) to write adv7842load_EDID_tab.
 *   3. Triggers SPI EEPROM store via KSV map 0x32: reg 0x7F = 0x04,
 *      reg 0x7E = 0x01, sleeps 1s, checks reg 0x7E bit 0 == 0.
 *   4. Reads EDID checksum from EDID map 0x36 reg 0xFF.
 *   NOTE: On SPI EEPROM write error, logs to syslog only (no puts(), unlike
 *   LoadEDID(char*)).
 */
void __thiscall ADV7842_t::LoadEDID(ADV7842_t *this)

{
  byte local_10 [8];                 // bTemp: register readback buffer
  
                    /* Unresolved local var: uchar bTemp@[???] */
  // HDMI map 0x34 (8-bit 0x68), reg 0x69 ('i'): HPA_AUTO_INT_EDID
  read_register(this,0x34,'i',local_10);
  sprintf(log_buff,"adv7842.cpp : HPA_AUTO_INT_EDID is %x",(uint)local_10[0]);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  // Arm auto-HPA on internal EDID (0xA4)
  write_register(this,0x34,'i',0xa4);
  memcpy(log_buff,"adv7842.cpp : Changed HPA_AUTO_INT_EDID to 0xA4",0x30);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  usleep(1000000);
  memcpy(log_buff,"adv7842.cpp : Start Loading EDID",0x21);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  LoadHDMI(this,'\a');               // 7 == XC_INPUT_EDID -> adv7842load_EDID_tab
  memcpy(log_buff,"adv7842.cpp : Check Loading EDID",0x21);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  // Trigger SPI EEPROM write via KSV map 0x32 regs 0x7F/0x7E
  write_register(this,0x32,'\x7f','\x04');
  write_register(this,0x32,'~','\x01');
  usleep(1000000);
  read_register(this,0x32,'~',local_10);
  if ((local_10[0] & 1) == 1) {
    memcpy(log_buff,"adv7842.cpp : ERROR : Writing EDID to SPI EEPROM",0x31);
    syslog(5,"%s",log_buff);
  }
  else {
    memcpy(log_buff,"adv7842.cpp : Internal EDID has been written to SPI EEPROM",0x3b);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    // EDID map 0x36 (8-bit 0x6C), reg 0xFF: last byte of 256-byte EDID (checksum)
    read_register(this,0x36,0xff,local_10);
    sprintf(log_buff,"adv7842.cpp : EDID checksum = 0x%x",(uint)local_10[0]);
    puts(log_buff);
    syslog(5,"%s",log_buff);
  }
  return;
}



/*
 * ADV7842_t::LoadEDID (from text file) @ 00414a8c
 *
 * Purpose:
 *   Parses an EDID register-script text file (lines of "<i2c8> <reg> <val>"
 *   in hex, e.g. Meld_Edid_r12_720p_pcm48.txt) into adv7842load_EDID_tab,
 *   writes it to the ADV7842, and stores it into the SPI EEPROM.
 *
 * Parameters:
 *   this     - ADV7842_t instance
 *   fileName - path to EDID script file (from -e CLI flag)
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: adv7842_init (04_libadv7842.cpp.c)
 *   Callees: fopen64, fgets, sscanf, fclose, read_register, write_register,
 *            LoadHDMI, usleep
 *
 * Side effects:
 *   Overwrites global adv7842load_EDID_tab[0..0x116*3+2], sets
 *   HPA_AUTO_INT_EDID = 0xA4, programs EDID RAM + SPI EEPROM, reads checksum.
 *
 * Quirks / MT300 relevance:
 *   - Reads up to 0x117 (279) lines (leaving entry 279 as the 00 00 00 sentinel).
 *     If the file has fewer than 279 lines (e.g. Meld_Edid_r12_720p_pcm48.txt
 *     has 273 lines), trailing entries in adv7842load_EDID_tab keep their
 *     compiled-in values.
 *   - Sets HPA_AUTO_INT_EDID = 0xA4 here; see docs/radio-and-video.md §5.1 for
 *     the cold-boot HDMI lock issue when av_streamer starts late after boot.
 */
void __thiscall ADV7842_t::LoadEDID(ADV7842_t *this,char *fileName)

{
  FILE *__stream;                    // edidFile
  char *pcVar1;                      // fgets() return value
  int local_128;                     // i: loop index (0..0x116 for read, 0..0x117 for dump)
  byte local_11c [4];                // bTemp: register readback buffer
  uint local_118;                    // val[0]: 8-bit I2C address parsed from line
  uint local_114;                    // val[1]: register offset parsed from line
  uint local_110;                    // val[2]: register value parsed from line
  char acStack_10c [260];            // lineBuf: 256-byte line buffer (+4 pad)
  
                    /* Unresolved local var: uchar bTemp@[???]
                       Unresolved local var: uchar * mTbl@[???]
                       Unresolved local var: int i@[???]
                       Unresolved local var: uint[3] val@[???]
                       Unresolved local var: FILE * edidFile@[???]
                       Unresolved local var: char[256] lineBuf@[???] */
  __stream = fopen64(fileName,"r");
  if (__stream == (FILE *)0x0) {
    sprintf(log_buff,"adv7842.cpp : ERROR : Can\'t open EDID file %s",fileName);
    puts(log_buff);
    syslog(5,"%s",log_buff);
  }
  else {
    for (local_128 = 0; local_128 < 0x117; local_128 = local_128 + 1) {
      pcVar1 = fgets(acStack_10c,0x100,__stream);
      if (pcVar1 != (char *)0x0) {
        sscanf(acStack_10c,"%x %x %x",&local_118,&local_114,&local_110);
        adv7842load_EDID_tab[local_128 * 3] = (uchar)local_118;
        adv7842load_EDID_tab[local_128 * 3 + 1] = (uchar)local_114;
        adv7842load_EDID_tab[local_128 * 3 + 2] = (uchar)local_110;
      }
    }
    sprintf(log_buff,"adv7842.cpp : Loading the EDID table from : %s",fileName);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    for (local_128 = 0; local_128 < 0x118; local_128 = local_128 + 1) {
      sprintf(log_buff,"   - %02x %02x %02x",(uint)adv7842load_EDID_tab[local_128 * 3],
              (uint)adv7842load_EDID_tab[local_128 * 3 + 1],
              (uint)adv7842load_EDID_tab[local_128 * 3 + 2]);
      puts(log_buff);
    }
    fclose(__stream);
    // HDMI map 0x34 (8-bit 0x68), reg 0x69 ('i'): HPA_AUTO_INT_EDID
    read_register(this,0x34,'i',local_11c);
    sprintf(log_buff,"adv7842.cpp : HPA_AUTO_INT_EDID is %x",(uint)local_11c[0]);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    write_register(this,0x34,'i',0xa4);
    memcpy(log_buff,"adv7842.cpp : Changed HPA_AUTO_INT_EDID to 0xA4",0x30);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    usleep(1000000);
    memcpy(log_buff,"adv7842.cpp : Start Loading EDID",0x21);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    LoadHDMI(this,'\a');             // 7 == XC_INPUT_EDID -> adv7842load_EDID_tab
    memcpy(log_buff,"adv7842.cpp : Check Loading EDID",0x21);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    // Trigger SPI EEPROM write via KSV map 0x32 regs 0x7F/0x7E
    write_register(this,0x32,'\x7f','\x04');
    write_register(this,0x32,'~','\x01');
    usleep(1000000);
    read_register(this,0x32,'~',local_11c);
    if ((local_11c[0] & 1) == 1) {
      memcpy(log_buff,"adv7842.cpp : ERROR : Writing EDID to SPI",0x2a);
      puts(log_buff);
      syslog(5,"%s",log_buff);
    }
    else {
      memcpy(log_buff,"adv7842.cpp : Internal EDID has been stored into SPI EEPROM",0x3c);
      puts(log_buff);
      syslog(5,"%s",log_buff);
      read_register(this,0x36,0xff,local_11c);
      sprintf(log_buff,"adv7842.cpp : EDID checksum = 0x%x",(uint)local_11c[0]);
      puts(log_buff);
      syslog(5,"%s",log_buff);
    }
  }
  return;
}



/*
 * ADV7842_t::CheckEDID @ 00415214
 *
 * Purpose:
 *   Reads and logs the EDID checksum byte (EDID map 0x36 reg 0xFF).
 *   NOTE: No callers observed in the split units.
 *
 * Parameters:
 *   this - ADV7842_t instance
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: none observed
 *   Callees: read_register
 */
void __thiscall ADV7842_t::CheckEDID(ADV7842_t *this)

{
  byte local_10 [8];                 // bTemp: EDID checksum byte
  
                    /* Unresolved local var: uchar bTemp@[???] */
  read_register(this,0x36,0xff,local_10);
  sprintf(log_buff,"adv7842.cpp : EDID checksum = 0x%x",(uint)local_10[0]);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  return;
}



/*
 * ADV7842_t::write_register @ 004152dc
 *
 * Purpose:
 *   Writes a single 8-bit register on the ADV7842 over I2C via xcode_io_t.
 *
 * Parameters:
 *   this               - ADV7842_t instance
 *   ADV7842_I2CAddress - 7-bit I2C slave address (shifted left by 1 for 8-bit)
 *   Register           - 8-bit register sub-address
 *   Data               - 8-bit value to write
 *
 * Returns:
 *   int status from xcode_io_t::i2c_write (0 = success)
 *
 * Callers / Callees:
 *   Callers: internal ADV7842_t methods
 *   Callees: xcode_io_t::i2c_write (05_xcodeio.cpp.c)
 */
int __thiscall
ADV7842_t::write_register(ADV7842_t *this,uint ADV7842_I2CAddress,uchar Register,uchar Data)

{
  int iVar1;                         // retcode from xcode_io_t::i2c_write
  uchar local_c;                     // TempData[0]: register sub-address
  uchar uStack_b;                    // TempData[1]: data byte
  
                    /* Unresolved local var: ADV7842_command_t TempData@[???]
                       Unresolved local var: int retcode@[???] */
  // Little-endian pack: byte 0 = Register, byte 1 = Data
  _local_c = CONCAT11(Data,Register);
  iVar1 = xcode_io_t::i2c_write
                    (this->xcodeIO,this->m_I2CChannel,ADV7842_I2CAddress << 1,&local_c,2,1,1);
  if (adv7842_DebugDump == 1) {
    printf("//=== write_register i2c 0x%02x reg 0x%02x data 0x%02x   m_I2CChannel=0x%x\n",
           ADV7842_I2CAddress << 1,(uint)Register,(uint)Data,this->m_I2CChannel);
  }
  return iVar1;
}



/*
 * ADV7842_t::read_register @ 004153f0
 *
 * Purpose:
 *   Reads a single 8-bit register from the ADV7842 over I2C via xcode_io_t
 *   (repeated-start write of 1-byte register address, then 1-byte read).
 *
 * Parameters:
 *   this               - ADV7842_t instance
 *   ADV7842_I2CAddress - 7-bit I2C slave address (shifted left by 1 for 8-bit)
 *   Register           - 8-bit register sub-address
 *   Data               - output pointer for the 1-byte read value
 *
 * Returns:
 *   int status from xcode_io_t::i2c_write_then_read (0 = success)
 *
 * Callers / Callees:
 *   Callers: internal ADV7842_t methods, adv7842_thread (04_libadv7842.cpp.c)
 *   Callees: xcode_io_t::i2c_write_then_read (05_xcodeio.cpp.c)
 */
int __thiscall
ADV7842_t::read_register(ADV7842_t *this,uint ADV7842_I2CAddress,uchar Register,uchar *Data)

{
  int iVar1;                         // retcode from xcode_io_t::i2c_write_then_read
  ushort local_c [2];                // TempData: 1-byte register address at byte 0
  
                    /* Unresolved local var: ADV7842_command_t TempData@[???]
                       Unresolved local var: int retcode@[???] */
  local_c[0] = (ushort)Register;
  iVar1 = xcode_io_t::i2c_write_then_read
                    (this->xcodeIO,this->m_I2CChannel,ADV7842_I2CAddress << 1,local_c,Data,1,1,1,1);
  if (adv7842_DebugDump == 1) {
    printf("//=== read_register i2c 0x%02x reg 0x%02x data 0x%02x\n",ADV7842_I2CAddress << 1,
           (uint)Register,(uint)*Data);
  }
  return iVar1;
}



/*
 * ADV7842_t::LoadHDMI @ 00415500
 *
 * Purpose:
 *   Selects and replays a 3-byte-per-entry I2C register table for the given
 *   input mode (or EDID load). Calls InitHDMI() for HDMI modes, InitMode() for
 *   analog/component modes, or neither for EDID load (input == 7).
 *   For general HDMI modes (0x28/0x2B/0x2C), patches IO reg 0x19 (LLC_DLL)
 *   bits [4:0] (LLC_DLL_PHASE) at adv7842mode_HDMI_YPbPr_HDMI_tab[0x17] using
 *   g_llc_delay (set via -l <n> CLI flag).
 *
 * Parameters:
 *   this  - ADV7842_t instance
 *   input - enum_xc_input_type_t cast to uchar
 *
 * Returns:
 *   void
 *
 * Callers / Callees:
 *   Callers: program_input_source, LoadEDID (both overloads)
 *   Callees: InitHDMI, InitMode, write_register, read_register, usleep
 *
 * Table format:
 *   Sequence of 3-byte tuples (addr8, reg, val) terminated by (0, 0, 0):
 *     - (addr8 & 1) == 0: write val to (addr8 >> 1, reg).
 *       Special reset delays: 300 ms after (0x40, 0xFF, 0x80) [ADV7842 reset]
 *       or (0x54, 0x17, 0x02) [external TX/encoder reset, inferred].
 *     - (addr8 & 1) == 1: read (addr8 >> 1, reg) and rewind index by 1
 *       (read entries occupy 2 bytes instead of 3).
 *
 * Bugs / Quirks:
 *   If input does not match any switch case, local_1c (mTbl) is uninitialized.
 */
void __thiscall ADV7842_t::LoadHDMI(ADV7842_t *this,uchar input)

{
  byte bVar1;                        // bI2cDeviceAddr: 8-bit I2C address (bit 0 = R/W)
  uchar Register;                    // bI2CIntAddr: register offset
  uchar uVar2;                       // sentinel check byte 0 (addr8)
  uchar uVar3;                       // sentinel check byte 1 (reg)
  uchar uVar4;                       // sentinel check byte 2 (val)
  bool bVar5;                        // hdmi_select: true -> call InitHDMI()
  bool bVar6;                        // edid_select: true -> skip InitHDMI()/InitMode()
  uint uVar7;                        // previous value of adv7842mode_HDMI_YPbPr_HDMI_tab[0x17]
  int local_28;                      // i: byte offset into register table
  uchar *local_1c;                   // mTbl: pointer to selected register table
  uchar local_c [4];                 // bNewValue: value to write / readback buffer
  
                    /* Unresolved local var: uint i@[???]
                       Unresolved local var: uint hdmi_select@[???]
                       Unresolved local var: uint edid_select@[???]
                       Unresolved local var: uint inpsrc@[???]
                       Unresolved local var: uchar * mTbl@[???]
                       Unresolved local var: uchar endTbldat@[???]
                       Unresolved local var: uchar bNewValue@[???]
                       Unresolved local var: uchar bI2CIntAddr@[???]
                       Unresolved local var: uchar bI2cDeviceAddr@[???]
                       Unresolved local var: uchar bI2cDeviceAddr_7bit@[???]
                       Unresolved local var: int tmp_g_llc_delay@[???] */
  local_28 = 0;
  bVar5 = false;
  bVar6 = false;
  switch(input) {
  case '\a':                         // 0x07: XC_INPUT_EDID
    local_1c = adv7842load_EDID_tab;
    bVar6 = true;
    memcpy(log_buff,"adv7842.cpp : Load EDID into SPI EEPROM",0x28);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    break;
  case '\x12':                       // 0x12: XC_INPUT_HDMI_S_VIDEO_480I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_S_VIDEO_480I",0x24);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_S_Video_tab;
    break;
  case '\x16':                       // 0x16: XC_INPUT_HDMI_COMPOSITE_480I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPOSITE_480I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_Composite_tab;
    break;
  case '\x18':                       // 0x18: XC_INPUT_HDMI_COMPOSITE_576I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPOSITE_576I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_Composite_576i_tab;
    break;
  case '\x1a':                       // 0x1a: XC_INPUT_HDMI_COMPONENT_480I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_480I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_480i_10b_HDMI_tab;
    break;
  case '\x1b':                       // 0x1b: XC_INPUT_HDMI_COMPONENT_480P
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_480P_HD",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_480p_24b_422_HDMI_tab;
    break;
  case '\x1c':                       // 0x1c: XC_INPUT_HDMI_COMPONENT_576I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_576I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_PAL_576i_10b_HDMI_tab;
    break;
  case '\x1d':                       // 0x1d: XC_INPUT_HDMI_COMPONENT_576P
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_576P_HD",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_576p_24b_422_HDMI_tab;
    break;
  case '\x1f':                       // 0x1f: XC_INPUT_HDMI_COMPONENT_720P
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_720P",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_720p_24b_422_HDMI_tab;
    break;
  case '!':                          // 0x21: XC_INPUT_HDMI_COMPONENT_720P_50
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_720P_50",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_720p_50HZ_24b_422_HDMI_tab;
    break;
  case '\"':                         // 0x22: XC_INPUT_HDMI_COMPONENT_1080I
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080I",0x27);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080i_24b_422_HDMI_tab;
    break;
  case '#':                          // 0x23: XC_INPUT_HDMI_COMPONENT_1080P
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080P",0x27);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080p_24b_422_HDMI_tab;
    break;
  case '$':                          // 0x24: XC_INPUT_HDMI_COMPONENT_1080I_50
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080I_50",0x2a);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080i_50HZ_24b_422_HDMI_tab;
    break;
  case '(':                          // 0x28: XC_INPUT_HDMI_TO_HDMI
  case '+':                          // 0x2b: XC_INPUT_HDMI_TO_HDMI_480P
  case ',':                          // 0x2c: XC_INPUT_HDMI_TO_HDMI_576P
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_tab;
    // Offset 0x15..0x17 is {0x40, 0x19, 0x83} = IO map reg 0x19 (LLC_DLL);
    // bits [4:0] set LLC_DLL_PHASE from g_llc_delay (-l CLI option).
    uVar7 = (uint)adv7842mode_HDMI_YPbPr_HDMI_tab[0x17];
    adv7842mode_HDMI_YPbPr_HDMI_tab[0x17] =
         adv7842mode_HDMI_YPbPr_HDMI_tab[0x17] & 0xe0 | (byte)g_llc_delay & 0x1f;
    sprintf(log_buff,"adv7842.cpp : Changing LLC DLL PHASE from %x to %x",uVar7 & 0x1f,
            (uint)adv7842mode_HDMI_YPbPr_HDMI_tab[0x17]);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI",0x24);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
    break;
  case ')':                          // 0x29: XC_INPUT_HDMI_TO_HDMI_480I
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_480i_tab;
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI_480I",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
    break;
  case '*':                          // 0x2a: XC_INPUT_HDMI_TO_HDMI_576I
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_576i_tab;
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI_576I",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
    break;
  case '-':                          // 0x2d: XC_INPUT_HDMI_TO_HDMI_1080P
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_1080p_tab;
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI_1080P",0x2a);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
  }
  if (bVar5) {
    InitHDMI(this);
  }
  else if (!bVar6) {
    InitMode(this);
  }
  usleep(3000);
  do {
    bVar1 = local_1c[local_28];
    Register = local_1c[local_28 + 1];
    local_c[0] = local_1c[local_28 + 2];
    uVar2 = local_1c[local_28];
    uVar3 = local_1c[local_28 + 1];
    uVar4 = local_1c[local_28 + 2];
    if ((uVar2 != '\0' || uVar3 != '\0') || uVar4 != '\0') {
      if ((bVar1 & 1) == 0) {
        write_register(this,(int)(uint)bVar1 >> 1,Register,local_c[0]);
        if (((bVar1 == 0x40) && (Register == 0xff)) && (local_c[0] == 0x80)) {
          if (adv7842_DebugDump == 1) {
            puts("//=== Has detected reset command has to wait for 300 Milisecond. ");
          }
          usleep(300000);
        }
        if (((bVar1 == 0x54) && (Register == '\x17')) && (local_c[0] == '\x02')) {
          if (adv7842_DebugDump == 1) {
            puts("//=== Has detected reset command for encoder has to wait for 300 Milisecond. ");
          }
          usleep(300000);
        }
      }
      else {
        read_register(this,(int)(uint)bVar1 >> 1,Register,local_c);
        local_28 = local_28 + -1;
      }
    }
    local_28 = local_28 + 3;
  } while ((uVar2 != '\0' || uVar3 != '\0') || uVar4 != '\0');
  return;
}



/*
 * ADV7842_t::detect_audio_mode @ 00416224
 *
 * Purpose:
 *   Detects whether incoming HDMI audio is compressed (non-PCM, e.g. AC-3)
 *   or linear PCM by inspecting HDMI map 0x34 registers. For non-HDMI inputs,
 *   always reports uncompressed (*bCompressed = 0).
 *
 * Parameters:
 *   this        - ADV7842_t instance
 *   bCompressed - output pointer: set to 1 if compressed audio, 0 otherwise
 *
 * Returns:
 *   0 (always)
 *
 * Callers / Callees:
 *   Callers: adv7842_detect_audio_mode (04_libadv7842.cpp.c)
 *   Callees: read_register
 */
int __thiscall ADV7842_t::detect_audio_mode(ADV7842_t *this,uint *bCompressed)

{
  byte local_10;                     // value: HDMI reg 0x18, then reg 0x36
  byte local_f [7];                  // value39: HDMI reg 0x39 (IEC 60958 CS byte 3, sample rate)
  
                    /* Unresolved local var: uchar value@[???]
                       Unresolved local var: uchar value39@[???] */
  *bCompressed = 0;
  if ((((this->m_InputSource == XC_INPUT_HDMI_TO_HDMI) ||
       (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_480I)) ||
      (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_576I)) ||
     (((this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_480P ||
       (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_576P)) ||
      (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_1080P)))) {
    // HDMI map 0x34 (8-bit 0x68), reg 0x18: audio packet present (bit 0)
    read_register(this,0x34,'\x18',&local_10);
    sprintf(log_buff,"adv7842.cpp : HDMI HDMI 0x18 is %x",(uint)local_10);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    if ((local_10 & 1) != 0) {
      // Reg 0x36: IEC 60958 Channel Status byte 0; bit 1 (0x02) = compressed / non-PCM
      read_register(this,0x34,'6',&local_10);
      sprintf(log_buff,"adv7842.cpp : HDMI HDMI 0x36 is %x",(uint)local_10);
      puts(log_buff);
      syslog(5,"%s",log_buff);
      // Reg 0x39: IEC 60958 Channel Status byte 3 (sampling frequency)
      read_register(this,0x34,'9',local_f);
      sprintf(log_buff,"adv7842.cpp : HDMI HDMI 0x39 is %x",(uint)local_f[0]);
      puts(log_buff);
      syslog(5,"%s",log_buff);
      if ((local_10 & 2) != 0) {
        *bCompressed = 1;
      }
    }
  }
  else {
    *bCompressed = 0;
  }
  return 0;
}



/*
 * ADV7842_t::detect_video_standard @ 004164e0
 *
 * Purpose:
 *   Detects the active video standard (resolution + frame rate) on the
 *   currently selected input:
 *     - Analog CVBS/S-Video path (SDP map 0x48): polls lock (0x5A bit 0),
 *       autodetect valid (0x57 bit 3), standard (0x52[3:0]), 50/60Hz (0x59 bit 3),
 *       and CVBS vs S-Video (0x54 bit 5).
 *     - Digital/Component path (CP map 0x22 STDI): triggers STDI measurement
 *       (0x86 |= 0x04), polls DVALID (0xB1 bit 7) up to 9x100ms, then reads
 *       BL (0xB1[5:0]:0xB2), SCVS (0xB3[7:3]), and SCF (0xB3[2:0]:0xB4) up to
 *       12x500ms to classify 480i/576i/480p/576p/720p/1080i/1080p @ 50/60Hz.
 *
 * Parameters:
 *   this          - ADV7842_t instance
 *   VideoStandard - output pointer for detected standard (+ frame-rate flag bits:
 *                   0x8000000 = 60Hz prog, 0x40000000 = 50Hz prog,
 *                   0x1000000 = 60Hz 1080i, 0x2000000 = 50Hz 1080i)
 *
 * Returns:
 *   0 (always, even if no signal is detected; caller checks *VideoStandard)
 *
 * Callers / Callees:
 *   Callers: adv7842_detect_video_std_internal (04_libadv7842.cpp.c)
 *   Callees: read_register, write_register, usleep
 *
 * Bugs / Quirks:
 *   1. Analog branch checks XC_INPUT_SVIDEO (1) instead of
 *      XC_INPUT_HDMI_S_VIDEO_480I (0x12), which LoadHDMI() uses.
 *   2. Digital retry loop (j = 0..11) does not re-read 0xB1, reusing the
 *      pre-loop 0xB1 value for BL[13:8].
 */
/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */
/* DWARF original prototype: int detect_video_standard(ADV7842_t * this,
   enum_analog_digital_video_standard_t * VideoStandard) */

int __thiscall
ADV7842_t::detect_video_standard
          (ADV7842_t *this,enum_analog_digital_video_standard_t *VideoStandard)

{
  byte bVar1;                        // BL[7:0] (CP reg 0xB2) for 50/60Hz discrimination
  byte bVar2;                        // SCVS = reg 0xB3 >> 3 (syncs per Vsync)
  ushort uVar3;                      // scratch for 14-bit BL and 11-bit SCF
  int local_20;                      // counter: DVALID poll iterations (0..8)
  int local_1c;                      // j: STDI classification retry count (0..11)
  byte local_10 [8];                 // value: 1-byte register readback buffer
  
                    /* Unresolved local var: uchar value@[???]
                       Unresolved local var: uchar SCVS@[???]
                       Unresolved local var: ushort BL@[???]
                       Unresolved local var: ushort SCF@[???]
                       Unresolved local var: int counter@[???]
                       Unresolved local var: int j@[???]
                       Unresolved local var: ushort BLTime@[???] */
  local_20 = 0;
  *VideoStandard = ANALOG_DIGITAL_VIDEO_STANDARD_NONE;
  if (((this->m_InputSource == XC_INPUT_HDMI_COMPOSITE_480I) ||
      (this->m_InputSource == XC_INPUT_SVIDEO)) ||
     (this->m_InputSource == XC_INPUT_HDMI_COMPOSITE_576I)) {
    // Analog SDP path (SDP map 0x48 / 8-bit 0x90)
    if (adv7842_DebugDump == 1) {
      puts("wait 1 second ");
    }
    usleep(500000);
    usleep(1000000);
    *VideoStandard = ANALOG_DIGITAL_VIDEO_STANDARD_NONE;
    read_register(this,0x48,'\0',local_10);
    read_register(this,0x48,'Z',local_10); // 0x5A: SDP lock status (bit 0)
    sprintf(curr_vidmode_buff,"adv7842.cpp : HDMI_SDP, 0x5A = 0x%x",(uint)local_10[0]);
    puts(curr_vidmode_buff);
    syslog(5,"%s",curr_vidmode_buff);
    if ((local_10[0] & 1) == 1) {
      read_register(this,0x48,'W',local_10); // 0x57: bit 3 = standard autodetect valid
      if ((local_10[0] & 8) == 0) {
        memcpy(curr_vidmode_buff,"adv7842.cpp : ERROR : The video standard cannot be determined",
               0x3e);
        puts(curr_vidmode_buff);
        syslog(5,"%s",curr_vidmode_buff);
      }
      else {
        sprintf(curr_vidmode_buff,"adv7842.cpp : HDMI_SDP, 0x57 = 0x%x",(uint)local_10[0]);
        puts(curr_vidmode_buff);
        syslog(5,"%s",curr_vidmode_buff);
        read_register(this,0x48,'R',local_10); // 0x52[3:0]: detected SD standard
        local_10[0] = local_10[0] & 0xf;
        sprintf(curr_vidmode_buff,"adv7842.cpp : HDMI_SDP, 0x52 = 0x%x",(uint)local_10[0]);
        puts(curr_vidmode_buff);
        syslog(5,"%s",curr_vidmode_buff);
        if ((local_10[0] == 0) || (local_10[0] == 2)) {
          *VideoStandard = ANALOG_VIDEO_STANDARD_NTSC;
          memcpy(curr_vidmode_buff,"adv7842.cpp : NTSC signal detected",0x23);
          puts(curr_vidmode_buff);
          syslog(5,"%s",curr_vidmode_buff);
        }
        else if (((local_10[0] == 4) || (local_10[0] == 6)) ||
                ((local_10[0] == 0xc || (local_10[0] == 0xe)))) {
          *VideoStandard = ANALOG_VIDEO_STANDARD_PAL;
          memcpy(curr_vidmode_buff,"adv7842.cpp : PAL signal detected",0x22);
          syslog(5,"%s",curr_vidmode_buff);
        }
        read_register(this,0x48,'Y',local_10); // 0x59 bit 3: 0=60Hz, 1=50Hz
        if ((local_10[0] & 8) == 0) {
          memcpy(curr_vidmode_buff,"adv7842.cpp : 60HZ detected",0x1c);
          puts(curr_vidmode_buff);
          syslog(5,"%s",curr_vidmode_buff);
        }
        else {
          memcpy(curr_vidmode_buff,"adv7842.cpp : 50HZ detected",0x1c);
          puts(curr_vidmode_buff);
          syslog(5,"%s",curr_vidmode_buff);
        }
        read_register(this,0x48,'T',local_10); // 0x54 bit 5: 0=Composite, 1=S-Video
        if ((local_10[0] & 0x20) == 0) {
          memcpy(curr_vidmode_buff,"adv7842.cpp : COMPOSITE detected",0x21);
          puts(curr_vidmode_buff);
          syslog(5,"%s",curr_vidmode_buff);
        }
        else {
          memcpy(curr_vidmode_buff,"adv7842.cpp : SVIDEO detected",0x1e);
          puts(curr_vidmode_buff);
          syslog(5,"%s",curr_vidmode_buff);
        }
      }
    }
    else {
      memcpy(curr_vidmode_buff,"adv7842.cpp : ERROR : Cannot detect signal",0x2b);
      puts(curr_vidmode_buff);
      syslog(5,"%s",curr_vidmode_buff);
    }
  }
  else {
    // Digital / Component CP STDI path (CP map 0x22 / 8-bit 0x44)
    usleep(500000);
    if (*VideoStandard == ANALOG_DIGITAL_VIDEO_STANDARD_NONE) {
      read_register(this,0x22,0x86,local_10); // STDI_CH1_CTRL
      if (adv7842_DebugDump == 1) {
        printf("ADV7842_STDI_REGISTER = %x\n",(uint)local_10[0]);
      }
      write_register(this,0x22,0x86,local_10[0] | 4); // trigger STDI measurement
      while( true ) {
        read_register(this,0x22,0xb1,local_10); // CH1_STDI_DVALID (bit 7) + BL[13:8]
        if (adv7842_DebugDump == 1) {
          printf("ADV7844_DVALID_REGISTER = %x\n",(uint)local_10[0]);
        }
        local_20 = local_20 + 1;
        if ((char)local_10[0] < '\0') { // bit 7 set -> DVALID == 1
          local_1c = 0;
          goto LAB_00416f1c;
        }
        if (8 < local_20) break;
        usleep(100000);
      }
      memcpy(curr_vidmode_buff,"adv7842.cpp : NO digital video standard detected",0x31);
      puts(curr_vidmode_buff);
      syslog(5,"%s",curr_vidmode_buff);
    }
  }
  return 0;
LAB_00416f1c:
  if (0xb < local_1c) {
LAB_00416f30:
    puts(curr_vidmode_buff);
    syslog(5,"%s",curr_vidmode_buff);
    return 0;
  }
  usleep(500000);
  uVar3 = (ushort)local_10[0];
  read_register(this,0x22,0xb2,local_10); // BL[7:0]
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_BL_REGISTER = %x\n",(uint)local_10[0]);
  }
  bVar1 = local_10[0];
  if (adv7842_DebugDump == 1) {
    printf("BL = %d\n",(uint)(ushort)((uVar3 & 0xff3f) << 8 | (ushort)local_10[0]));
  }
  read_register(this,0x22,0xb3,local_10); // SCVS[4:0] = [7:3], SCF[10:8] = [2:0]
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_SCVS_LCVS_REGISTER = %x\n",(uint)local_10[0]);
  }
  bVar2 = (byte)((int)(uint)local_10[0] >> 3);
  if (adv7842_DebugDump == 1) {
    printf("SCVS = %d, value = 0x%x\n",(int)(uint)local_10[0] >> 3,(uint)local_10[0]);
  }
  uVar3 = (ushort)local_10[0];
  read_register(this,0x22,0xb4,local_10); // SCF[7:0]
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_SCF_LCF_REGISTER = %x, SCF = %d\n",(uint)local_10[0],(uint)(uVar3 & 0xff07));
  }
  uVar3 = (uVar3 & 0xff07) << 8 | (ushort)local_10[0]; // 11-bit SCF (lines/frame or field)
  if (adv7842_DebugDump == 1) {
    printf("SCF = %d\n",(uint)uVar3);
  }
  if ((((1 < bVar2) && (bVar2 < 6)) && (0x100 < uVar3)) && (uVar3 < 0x10c)) {
    *VideoStandard = ANALOG_VIDEO_STANDARD_NTSC; // 480i@60Hz (SCF 257..267)
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 480i@60Hz",0x27);
    goto LAB_00416f30;
  }
  if (((1 < bVar2) && (bVar2 < 6)) && ((0x132 < uVar3 && (uVar3 < 0x13e)))) {
    *VideoStandard = ANALOG_VIDEO_STANDARD_PAL; // 576i@50Hz (SCF 307..317)
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 576i@50Hz",0x27);
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x201 < uVar3 && (uVar3 < 0x217)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_480p; // 480p@60Hz (SCF 514..534)
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 480p@60Hz",0x27);
    goto LAB_00416f30;
  }
  if ((((3 < bVar2) && (bVar2 < 7)) && (0x265 < uVar3)) && (uVar3 < 0x27b)) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_576p; // 576p@50Hz (SCF 614..634)
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 576p@50Hz",0x27);
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x2e8 < uVar3 && (uVar3 < 0x2f4)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_720p; // 720p (SCF 745..755)
    if ((bVar1 < 0xd0) || (0xd8 < bVar1)) {
      *VideoStandard = *VideoStandard | 0x8000000; // 60Hz flag
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 720p@60",0x25);
    }
    else {
      *VideoStandard = *VideoStandard | 0x40000000; // 50Hz flag (BL[7:0] 208..216)
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 720p@50",0x25);
    }
    goto LAB_00416f30;
  }
  if ((((3 < bVar2) && (bVar2 < 7)) && (0x22c < uVar3)) && (uVar3 < 0x238)) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_1080i; // 1080i (SCF 557..567)
    if ((bVar1 < 0xc5) || (0xcd < bVar1)) {
      *VideoStandard = *VideoStandard | 0x1000000; // 60Hz flag
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080i@60",0x26);
    }
    else {
      *VideoStandard = *VideoStandard | 0x2000000; // 50Hz flag (BL[7:0] 197..205)
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080i@50",0x26);
    }
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x459 < uVar3 && (uVar3 < 0x46f)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_1080p; // 1080p (SCF 1114..1134)
    if ((bVar1 < 0xdc) || (0xe4 < bVar1)) {
      *VideoStandard = *VideoStandard | 0x8000000; // 60Hz flag
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080p@60",0x26);
    }
    else {
      *VideoStandard = *VideoStandard | 0x40000000; // 50Hz flag (BL[7:0] 220..228)
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080p@50",0x26);
    }
    goto LAB_00416f30;
  }
  local_1c = local_1c + 1;
  goto LAB_00416f1c;
}



/*
 * ADV7842_t::set_channel @ 004175d4
 *
 * Purpose:
 *   Setter for m_I2CChannel. NOTE: No callers observed in the split units.
 *
 * Parameters:
 *   this    - ADV7842_t instance
 *   channel - XCode4 I2C bus channel index
 *
 * Returns:
 *   void
 */
void __thiscall ADV7842_t::set_channel(ADV7842_t *this,uint channel)

{
  this->m_I2CChannel = channel;
  return;
}




/* #####################################################################
 * FILE: 07_xcodeapi.cpp.c
 * ##################################################################### */

/*
 * 07_xcodeapi.cpp.c  --  annotated decompilation of src/libadv7842/xcodeapi.cpp
 * ============================================================================
 *
 * Original source: /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libadv7842/xcodeapi.cpp
 *
 * Role in av_streamer:
 *   Low-level ViXS XCode4 SDK C wrapper over `/dev/vixs/xcodedrv` (`ioctl` code
 *   `0x17241724`). Only the first 7 functions are actually used in `av_streamer`
 *   (called by `xcode_io_t` in `05_xcodeio.cpp.c` for ADV7842 I2C and GPIO MMR
 *   access):
 *     - `xcode_open_handle`, `xcode_close_handle`
 *     - `xcode_print_error_code`
 *     - `xcode_get_frame_buffer_map_addr` (cmd 8), `xcode_get_mmr_map_addr` (cmd 9)
 *     - `xcode_i2c_read`, `xcode_i2c_write` (cmd 0x1b)
 *
 *   All remaining functions (`xcode_recv_stream`, `xcode_send_stream`,
 *   `xcode_update_stream`, `xcode_update_stream2`, `xcode_close_stream`,
 *   `xcode_open_stream_*`, `xcode_tsd_dvo_config`, `xcode_dma_*`,
 *   `xcode_ts_control`) are dead/unused code in `av_streamer` linked from the
 *   ViXS SDK; `02_vixs_encoder.c.c` opens `/dev/vixs/xcodedrv` and issues ioctl
 *   `0x17241724` directly instead of calling `xcodeapi.cpp`.
 *
 * Common IOCTL ABI (`/dev/vixs/xcodedrv`, request `0x17241724`):
 *   Every command passes a struct beginning with `IOCTLBUFFERHEADER` (164 = 0xa4 bytes):
 *     +0x00  size_of_buf      (uint)  total struct size in bytes
 *     +0x04  command          (uint)  ViXS driver command ID
 *     +0x08  asynchronous     (uint)  0 = synchronous
 *     +0x0c  return_value     (uint)  caller pre-sets to 2 (IOCTL_RET_CMD_FAILED);
 *                                     driver sets 1 (IOCTL_RET_CMD_COMPLETE) on success
 *     +0x10  driver_reserved  (IOCTLDRIVERDATA, 132 bytes)
 *     +0x94  time_out_value   (uint)  timeout in ms (500, 2000, or -1)
 *     +0x98  sub_command      (uint)  used by TSD_CONFIG (1)
 *     +0x9c  reserved2        (uint)
 *     +0xa0  reserved3        (uint)
 */

/*
 * xcode_open_handle @ 00417610
 *
 * Purpose:
 *   Opens `/dev/vixs/xcodedrv` read-write (`O_RDWR = 2`) via `open64`.
 * Return value:
 *   File descriptor (`xcode_handle_t`, `int`/`uint`).
 * Callers:
 *   `xcode_io_t::xcode_io_t` (05_xcodeio.cpp.c).
 * Bug:
 *   Checks `if (xVar1 == 0)` instead of `if (xVar1 < 0)`! On `open64` failure,
 *   `-1` (`0xffffffff`) is returned silently without printing the error message,
 *   and `0` (stdin) would falsely be treated as an error.
 */
// ==== xcode_open_handle @ 00417610 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

xcode_handle_t xcode_open_handle(void)

{
  xcode_handle_t xVar1;  // fd returned by open64("/dev/vixs/xcodedrv", O_RDWR)
  
                    /* Unresolved local var: xcode_handle_t handle@[???] */
  xVar1 = open64("/dev/vixs/xcodedrv",2);  // O_RDWR = 2
  if (xVar1 == 0) {                        // BUG: should be (int)xVar1 < 0
    printf("ERROR: Failed to open XCode driver - %d\n",0);
    xVar1 = 0;
  }
  return xVar1;
}



/*
 * xcode_close_handle @ 004176a4
 *
 * Purpose:
 *   Closes a `/dev/vixs/xcodedrv` file descriptor if non-zero.
 * Parameters:
 *   handle - fd from `xcode_open_handle()`
 * Return value:
 *   `close()` return value, or `-1` if `handle == 0`.
 * Callers:
 *   `xcode_io_t::~xcode_io_t` (05_xcodeio.cpp.c).
 * Quirk:
 *   Matches `xcode_open_handle`'s bug by treating `0` rather than `< 0` as invalid.
 */
// ==== xcode_close_handle @ 004176a4 ====

int xcode_close_handle(xcode_handle_t handle)

{
  int iVar1;  // close() return value, or -1 if handle == 0
  
  if (handle == 0) {
    iVar1 = -1;
  }
  else {
    iVar1 = close(handle);
  }
  return iVar1;
}



/*
 * xcode_print_error_code @ 00417710
 *
 * Purpose:
 *   Decodes ViXS `IOCTL_RET_*` status codes (`ioctl_header.return_value`) and
 *   prints a human-readable diagnostic prefixed by `[caller]`.
 * Parameters:
 *   error  - `IOCTL_RET_*` code (0x00..0x30 or 0xffffffff)
 *   caller - tag string (e.g. "GET_FBADDR", "GET_MMRADDR", "RECV", "SEND")
 * Callers:
 *   `xcode_get_frame_buffer_map_addr`, `xcode_get_mmr_map_addr`, and the unused
 *   stream/DMA/TSD functions in this file.
 * Notes:
 *   DWARF `IOCTL_RET_*` enum mapping:
 *     0x00 IOCTL_RET_NOT_USED
 *     0x01 IOCTL_RET_CMD_COMPLETE
 *     0x02 IOCTL_RET_CMD_FAILED
 *     0x03 IOCTL_RET_CMD_INVALID
 *     0x04 IOCTL_RET_PARAM_INVALID
 *     0x05 IOCTL_RET_HANDLE_INVALID
 *     0x06 IOCTL_RET_TIMEDOUT
 *     0x07 IOCTL_RET_SUBMIT_BUFFER_FULL
 *     0x08 IOCTL_RET_TIMEOUT_QUEUE_FULL
 *     0x09 IOCTL_RET_USER_REQUEST_CANCEL
 *     0x0a IOCTL_RET_CMD_DMA_FAIL_BTB
 *     0x0b IOCTL_RET_CMD_DMA_FAIL_SCATTER_LIST
 *     0x0c IOCTL_RET_CMD_DMA_HW_FAILURE
 *     0x0d IOCTL_RET_I2C_TRANSFER_ERROR
 *     0x0e IOCTL_RET_CMD_FAIL_HW_ERROR
 *     0x0f IOCTL_RET_CMD_HW_TIMED_OUT_ERROR
 *     0x10 IOCTL_RET_CMD_TSD_NO_CHANNELS_AVAIL_ERROR
 *     0x11 IOCTL_RET_CMD_TSD_ALLOC_FAILED_ERROR
 *     0x12 IOCTL_RET_CMD_TSD_ALLOC_PAT_REG_FAILED_ERROR
 *     0x13 IOCTL_RET_PSI_SLOT_CLOSED
 *     0x14 IOCTL_RET_PSI_SLOT_BUSY
 *     0x15 IOCTL_RET_ALIGNMENT_ERROR
 *     0x16 IOCTL_RET_STREAM_BUFFER_FULL
 *     0x17 IOCTL_RET_STREAM_BUFFER_EMPTY
 *     0x18 IOCTL_RET_USER_BUFFER_TOO_SMALL
 *     0x19 IOCTL_RET_DEVICE_CAP_ERROR
 *     0x1a IOCTL_RET_NO_KERNEL_MEMORY
 *     0x1b IOCTL_RET_DEVICE_CLOSED
 *     0x1c IOCTL_RET_DEVICE_IS_SHARED
 *     0x1d IOCTL_RET_NO_TUNERS_AVAIL
 *     0x1e IOCTL_RET_DEVICE_NOT_READY
 *     0x1f IOCTL_RET_AGAIN
 *     0x20 IOCTL_RET_TSD_TS_OUT_RESOURCE_ALLOC_FAILURE
 *     0x21 IOCTL_RET_SD_STREAM_IS_HD_STREAM
 *     0x22 IOCTL_RET_OFFSET_OUT_OF_RANGE
 *     0x23 IOCTL_RET_STREAM_IS_BEING_FLUSHED
 *     0x24 IOCTL_RET_STREAM_HAS_BEEN_FLUSHED
 *     0x25 IOCTL_RET_BUFFER_SIZE_INVALID
 *     0x26 IOCTL_RET_DATA_LESS_THAN_16LINES
 *     0x27 IOCTL_RET_SPI_MASTER_BUSY
 *     0x28 IOCTL_RET_ERROR_STREAM_CONTEXT
 *     0x29 IOCTL_RET_INVALID_NUM_OF_DMA_BYTES
 *     0x2a IOCTL_RET_NO_MORE_USB_FIFO
 *     0x2f IOCTL_RET_MIPS_DEAD (0x2b..0x2e internal errors fall through to Unknown)
 *     0x30 IOCTL_RET_NODEVICE
 *     0xffffffff IOCTL_RET_CMD_PROCESSING (still being serviced)
 */
// ==== xcode_print_error_code @ 00417710 ====

void xcode_print_error_code(uint error,char *caller)

{
  if (error == 0x16) {
    printf("[%s] Destination buffer is full\n",caller);
    return;
  }
  if (error < 0x17) {
    if (error == 10) {
      printf("[%s] DMA failed because input/output request buffer exceeded the max  size\n",caller);
      return;
    }
    if (error < 0xb) {
      if (error == 4) {
        printf("[%s] Command parameter INVALID\n",caller);
        return;
      }
      if (error < 5) {
        if (error == 1) {
          printf("[%s] Commad request completed\n",caller);
          return;
        }
        if (error == 0) {
          printf("[%s] Buffer is not used\n",caller);
          return;
        }
        if (error == 2) {
          printf("[%s] Commad request FAILED\n",caller);
          return;
        }
        if (error == 3) {
          printf("[%s] Commad request id INVALID\n",caller);
          return;
        }
      }
      else {
        if (error == 7) {
          printf("[%s] The driver buffer was full\n",caller);
          return;
        }
        if (error < 8) {
          if (error == 5) {
            printf("[%s] Command contains invalid device handle\n",caller);
            return;
          }
          if (error == 6) {
            printf("[%s] Command timedout\n",caller);
            return;
          }
        }
        else {
          if (error == 8) {
            printf("[%s] Resources are full...no space left in timer queue;\n",caller);
            return;
          }
          if (error == 9) {
            printf("[%s] User requested that associated File handle requests be cancelled;\n",caller
                  );
            return;
          }
        }
      }
    }
    else {
      if (error == 0x10) {
        printf("[%s] TSD failed because no channels were available or Channel was invalid...depending on IOCTL\n"
               ,caller);
        return;
      }
      if (error < 0x11) {
        if (error == 0xd) {
          printf("[%s] I2C transfer failed at electrical level\n",caller);
          return;
        }
        if (error < 0xe) {
          if (error == 0xb) {
            printf("[%s] DMA failed because there was an error in the scatter list\n",caller);
            return;
          }
          if (error == 0xc) {
            printf("[%s] DMA Status states that the DMA Engine has encountered an error\n",caller);
            return;
          }
        }
        else {
          if (error == 0xe) {
            printf("[%s] failed because of hardware error\n",caller);
            return;
          }
          if (error == 0xf) {
            printf("[%s] failed because of hardware error\n",caller);
            return;
          }
        }
      }
      else {
        if (error == 0x13) {
          printf("[%s] TSD -  an application as closed the PSI slot.\n",caller);
          return;
        }
        if (error < 0x14) {
          if (error == 0x11) {
            printf("[%s] TSD failed to allocate Frame Buffer Memory.\n",caller);
            return;
          }
          if (error == 0x12) {
            printf("[%s] TSD failed to allocate a Pattern Register.\n",caller);
            return;
          }
        }
        else {
          if (error == 0x14) {
            printf("[%s] TSD -  The specified TSD slot is busy...try again.\n",caller);
            return;
          }
          if (error == 0x15) {
            printf("[%s] Pointer or size needs to be aligned\n",caller);
            return;
          }
        }
      }
    }
  }
  else {
    if (error == 0x22) {
      printf("[%s] Requested data is out of Memory Range\n",caller);
      return;
    }
    if (error < 0x23) {
      if (error == 0x1c) {
        printf("[%s] Input of this device is being shared\n",caller);
        return;
      }
      if (error < 0x1d) {
        if (error == 0x19) {
          printf("[%s] Device is not capable of requested operation\n",caller);
          return;
        }
        if (error < 0x1a) {
          if (error == 0x17) {
            printf("[%s] Source buffer is empty\n",caller);
            return;
          }
          if (error == 0x18) {
            printf("[%s] Buffer provided by user is too small to receive a pes\n",caller);
            return;
          }
        }
        else {
          if (error == 0x1a) {
            printf("[%s] No enough kernel memory\n",caller);
            return;
          }
          if (error == 0x1b) {
            printf("[%s] Device has been closed\n",caller);
            return;
          }
        }
      }
      else {
        if (error == 0x1f) {
          printf("[%s] Failed but may succeed on retry\n",caller);
          return;
        }
        if (error < 0x20) {
          if (error == 0x1d) {
            printf("[%s] There were no tuners available for request\n",caller);
            return;
          }
          if (error == 0x1e) {
            printf("[%s] The device is not initialized (MIPS not enabled)\n",caller);
            return;
          }
        }
        else {
          if (error == 0x20) {
            printf("[%s] Failure was due to runing out of TS Out resource allocation\n",caller);
            return;
          }
          if (error == 0x21) {
            printf("[%s] Stream processing has stopped for this stream, lack of HD resources\n",
                   caller);
            return;
          }
        }
      }
    }
    else {
      if (error == 0x28) {
        printf("[%s] The Stream Context is no longer valid because the firmware is disabled\n",
               caller);
        return;
      }
      if (error < 0x29) {
        if (error == 0x25) {
          printf("[%s] One of the buffer size requested is either below or above range.\n",caller);
          return;
        }
        if (error < 0x26) {
          if (error == 0x23) {
            printf("[%s] Stream is being flushed of all input data..sends are no longer valid\n",
                   caller);
            return;
          }
          if (error == 0x24) {
            printf("[%s] Stream has been flushed of all input data..sends are no longer valid\n",
                   caller);
            return;
          }
        }
        else {
          if (error == 0x26) {
            printf("[%s] Multiple of 16 lines are required when sending YUV Data to XCode for encoding\n"
                   ,caller);
            return;
          }
          if (error == 0x27) {
            printf("[%s] SPI Master is busy and can not successfully complete the request\n",caller)
            ;
            return;
          }
        }
      }
      else {
        if (error == 0x2f) {
          printf("[%s] The firmware has a fatal error. Driver must be reloaded to restart the operation.\n"
                 ,caller);
          return;
        }
        if (error < 0x30) {
          if (error == 0x29) {
            printf("[%s] For USB DMA, must be multi. of 512 bytes\n",caller);
            return;
          }
          if (error == 0x2a) {
            printf("[%s] For USB, indicate no FIFO avaliable\n",caller);
            return;
          }
        }
        else {
          if (error == 0x30) {
            printf("[%s] No device.\n",caller);
            return;
          }
          if (error == 0xffffffff) {
            printf("[%s] Command is still being serviced\n",caller);
            return;
          }
        }
      }
    }
  }
  printf("[%s] Unknown Error\n",caller);
  return;
}



/*
 * xcode_get_frame_buffer_map_addr @ 004181ec
 *
 * Purpose:
 *   Issues `IOCTLDEVFRAMEBUFFER` command 8 (GET_FBADDR) on `/dev/vixs/xcodedrv`
 *   to obtain the user-space mapped base address of XCode4 frame-buffer memory.
 * Parameters:
 *   XCodeHandle - open fd for `/dev/vixs/xcodedrv`
 * Return value:
 *   Mapped FB virtual address (`pbuffer`), or NULL on error.
 * Callers / Callees:
 *   Called by: `xcode_io_t::xcode_io_t` (05_xcodeio.cpp.c).
 *   Calls: `memset`, `ioctl(..., 0x17241724, ...)`, `xcode_print_error_code`.
 * Stack layout (`IOCTLDEVFRAMEBUFFER`, 172 = 0xac bytes):
 *   local_b8 (+0x00): ioctl_header.size_of_buf = 0xac
 *   local_b4 (+0x04): ioctl_header.command     = 8
 *   local_ac (+0x0c): ioctl_header.return_value = 2 (IOCTL_RET_CMD_FAILED)
 *   local_24 (+0x94): ioctl_header.time_out_value = 500 ms
 *   local_14 (+0xa4): pbuffer (output mapped address)
 */
// ==== xcode_get_frame_buffer_map_addr @ 004181ec ====

void * xcode_get_frame_buffer_map_addr(xcode_handle_t XCodeHandle)

{
  undefined4 local_b8;  // ioctl_header.size_of_buf
  undefined4 local_b4;  // ioctl_header.command
  uint local_ac;        // ioctl_header.return_value
  undefined4 local_24;  // ioctl_header.time_out_value
  void *local_14;       // IOCTLDEVFRAMEBUFFER.pbuffer
  
                    /* Unresolved local var: IOCTLDEVFRAMEBUFFER ioctl_devframebuffer@[???] */
  memset(&local_b8,0,0xac);
  local_b4 = 8;
  local_b8 = 0xac;
  local_24 = 500;
  local_ac = 2;
  ioctl(XCodeHandle,0x17241724,&local_b8);
  if (local_ac != 1) {
    xcode_print_error_code(local_ac,"GET_FBADDR");
    local_14 = (void *)0x0;
  }
  return local_14;
}



/*
 * xcode_get_mmr_map_addr @ 004182d0
 *
 * Purpose:
 *   Issues `IOCTLDEVFRAMEBUFFER` command 9 (GET_MMRADDR) on `/dev/vixs/xcodedrv`
 *   to obtain the user-space mapped base address of XCode4 MMRs (used for GPIO 5).
 * Parameters:
 *   XCodeHandle - open fd for `/dev/vixs/xcodedrv`
 * Return value:
 *   Mapped MMR virtual address (`pbuffer`), or NULL on error.
 * Callers / Callees:
 *   Called by: `xcode_io_t::xcode_io_t` (05_xcodeio.cpp.c).
 *   Calls: `memset`, `ioctl(..., 0x17241724, ...)`, `xcode_print_error_code`.
 * Stack layout: same `IOCTLDEVFRAMEBUFFER` (0xac bytes) as `xcode_get_frame_buffer_map_addr`.
 */
// ==== xcode_get_mmr_map_addr @ 004182d0 ====

void * xcode_get_mmr_map_addr(xcode_handle_t XCodeHandle)

{
  undefined4 local_b8;  // ioctl_header.size_of_buf
  undefined4 local_b4;  // ioctl_header.command
  uint local_ac;        // ioctl_header.return_value
  undefined4 local_24;  // ioctl_header.time_out_value
  void *local_14;       // IOCTLDEVFRAMEBUFFER.pbuffer
  
                    /* Unresolved local var: IOCTLDEVFRAMEBUFFER ioctl_devframebuffer@[???] */
  memset(&local_b8,0,0xac);
  local_b4 = 9;
  local_b8 = 0xac;
  local_24 = 500;
  local_ac = 2;
  ioctl(XCodeHandle,0x17241724,&local_b8);
  if (local_ac != 1) {
    xcode_print_error_code(local_ac,"GET_MMRADDR");
    local_14 = (void *)0x0;
  }
  return local_14;
}



/*
 * xcode_i2c_read @ 004183b4
 *
 * Purpose:
 *   Executes an I2C master read via `IOCTLI2CCOMMAND` (command 0x1b, 272 bytes).
 * Parameters:
 *   XCodeHandle   - open fd for `/dev/vixs/xcodedrv`
 *   I2CChannel    - I2C controller block number (`i2c_block_number`)
 *   Address       - 8-bit I2C slave address
 *   data          - destination buffer for `n` read bytes
 *   n             - byte count (max 64 bytes; NOT bounds-checked!)
 *   atomic_period - bus atomic period parameter
 *   timeout       - I2C transaction timeout (ms)
 *   start, stop   - whether to emit START / STOP conditions
 * Return value:
 *   0 on success (`return_value == 1`), otherwise driver `return_value`.
 * Callers:
 *   `xcode_io_t::i2c_read` (05_xcodeio.cpp.c).
 * Stack layout (`IOCTLI2CCOMMAND`, 272 = 0x110 bytes):
 *   local_118 (+0x000): ioctl_header.size_of_buf = 0x110
 *   local_114 (+0x004): ioctl_header.command     = 0x1b
 *   local_10c (+0x00c): ioctl_header.return_value
 *   auStack_74(+0x0a4): data_array[64]
 *   local_34  (+0x0e4): num_bytes
 *   local_30  (+0x0e8): rop = 0 (read)
 *   local_2c  (+0x0ec): slave_address
 *   local_28  (+0x0f0): atomic_period
 *   local_24  (+0x0f4): address_mode = 0
 *   local_20  (+0x0f8): timeout
 *   local_1c  (+0x0fc): i2c_block_number
 *   local_18  (+0x100): startbit
 *   local_14  (+0x104): stopbit
 * Quirks:
 *   - Unlike `xcode_i2c_write`, `return_value` (`local_10c`) is left at 0
 *     (`IOCTL_RET_NOT_USED`) rather than pre-set to 2, and `time_out_value` is 0.
 *   - No check that `n <= 64`.
 */
// ==== xcode_i2c_read @ 004183b4 ====

int xcode_i2c_read(xcode_handle_t XCodeHandle,int I2CChannel,int Address,void *data,size_t n,
                  int atomic_period,int timeout,int start,int stop)

{
  undefined4 local_118;     // ioctl_header.size_of_buf (0x110)
  undefined4 local_114;     // ioctl_header.command (0x1b)
  int local_10c;            // ioctl_header.return_value
  undefined1 auStack_74 [64]; // data_array[64] (+0xa4)
  size_t local_34;          // num_bytes (+0xe4)
  undefined4 local_30;      // rop = 0 (read) (+0xe8)
  int local_2c;             // slave_address (+0xec)
  int local_28;             // atomic_period (+0xf0)
  undefined4 local_24;      // address_mode = 0 (+0xf4)
  int local_20;             // timeout (+0xf8)
  int local_1c;             // i2c_block_number (+0xfc)
  int local_18;             // startbit (+0x100)
  int local_14;             // stopbit (+0x104)
  
                    /* Unresolved local var: IOCTLI2CCOMMAND I2CCommand@[???] */
  memset(&local_118,0,0x110);
  local_114 = 0x1b;
  local_118 = 0x110;
  local_34 = n;
  local_30 = 0;
  local_28 = atomic_period;
  local_24 = 0;
  local_20 = timeout;
  local_18 = start;
  local_14 = stop;
  local_2c = Address;
  local_1c = I2CChannel;
  ioctl(XCodeHandle,0x17241724,&local_118);
  if (local_10c == 1) {
    memcpy(data,auStack_74,n);
    local_10c = 0;
  }
  return local_10c;
}



/*
 * xcode_i2c_write @ 004184dc
 *
 * Purpose:
 *   Executes an I2C master write via `IOCTLI2CCOMMAND` (command 0x1b, 272 bytes).
 * Parameters:
 *   Same as `xcode_i2c_read`, with `data` as source buffer.
 * Return value:
 *   0 on success (`return_value == 1`), otherwise driver `return_value`.
 * Callers:
 *   `xcode_io_t::i2c_write` (05_xcodeio.cpp.c).
 * Stack layout:
 *   Same as `xcode_i2c_read`, plus:
 *   local_84 (+0x94): ioctl_header.time_out_value = 0xffffffff (-1)
 *   local_30 (+0xe8): rop = 1 (write)
 *   local_10c (+0x0c): pre-set to 2 (IOCTL_RET_CMD_FAILED)
 * Bug/Quirk:
 *   `memcpy(auStack_74, data, n)` has no bounds check for `n > 64`.
 */
// ==== xcode_i2c_write @ 004184dc ====

int xcode_i2c_write(xcode_handle_t XCodeHandle,int I2CChannel,int Address,void *data,size_t n,
                   int atomic_period,int timeout,int start,int stop)

{
  undefined4 local_118;     // ioctl_header.size_of_buf (0x110)
  undefined4 local_114;     // ioctl_header.command (0x1b)
  int local_10c;            // ioctl_header.return_value
  undefined4 local_84;      // ioctl_header.time_out_value (0xffffffff)
  undefined1 auStack_74 [64]; // data_array[64] (+0xa4)
  size_t local_34;          // num_bytes (+0xe4)
  undefined4 local_30;      // rop = 1 (write) (+0xe8)
  int local_2c;             // slave_address (+0xec)
  int local_28;             // atomic_period (+0xf0)
  undefined4 local_24;      // address_mode = 0 (+0xf4)
  int local_20;             // timeout (+0xf8)
  int local_1c;             // i2c_block_number (+0xfc)
  int local_18;             // startbit (+0x100)
  int local_14;             // stopbit (+0x104)
  
                    /* Unresolved local var: IOCTLI2CCOMMAND I2CCommand@[???] */
  memset(&local_118,0,0x110);
  memcpy(auStack_74,data,n);
  local_114 = 0x1b;
  local_118 = 0x110;
  local_34 = n;
  local_30 = 1;
  local_28 = atomic_period;
  local_24 = 0;
  local_20 = timeout;
  local_18 = start;
  local_14 = stop;
  local_10c = 2;
  local_84 = 0xffffffff;
  local_2c = Address;
  local_1c = I2CChannel;
  ioctl(XCodeHandle,0x17241724,&local_118);
  if (local_10c == 1) {
    local_10c = 0;
  }
  return local_10c;
}



/*
 * xcode_recv_stream @ 00418618
 *
 * Purpose:
 *   Receives encoded stream data from `/dev/vixs/xcodedrv` using
 *   `IOCTLSDEVRECV` (command 0x14, size 0x240 = 576 bytes, timeout 2000 ms).
 *   Unused in `av_streamer` (`02_vixs_encoder.c.c` issues cmd 0x14 directly).
 * Return value:
 *   0 on success, otherwise `return_value` (after calling `xcode_print_error_code`).
 */
// ==== xcode_recv_stream @ 00418618 ====

uint xcode_recv_stream(xcode_handle_t XCodeHandle,IOCTLSDEVRECV_conflict *pioctl_recv_strm,
                      void *pbuffer,uint *pbufsize,uint64 hStream,uint *pflags,uint *pmore,
                      uint *presult)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_recv_strm->ioctl_header).command = 0x14;
  (pioctl_recv_strm->ioctl_header).size_of_buf = 0x240;
  (pioctl_recv_strm->ioctl_header).time_out_value = 2000;
  (pioctl_recv_strm->ioctl_header).return_value = 2;
  pioctl_recv_strm->buffer = pbuffer;
  *(uint *)&pioctl_recv_strm->flags = *pflags;
  *(undefined4 *)((int)&pioctl_recv_strm->flags + 4) = 0;
  pioctl_recv_strm->size = *pbufsize;
  pioctl_recv_strm->stream_handle = hStream;
  ioctl(XCodeHandle,0x17241724,pioctl_recv_strm);
  if ((pioctl_recv_strm->ioctl_header).return_value == 1) {
    *pbufsize = pioctl_recv_strm->size;
    *pmore = pioctl_recv_strm->more;
    *presult = (uint)pioctl_recv_strm->result;
    *pflags = (uint)pioctl_recv_strm->flags;
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_recv_strm->ioctl_header).return_value,"RECV");
    uVar1 = (pioctl_recv_strm->ioctl_header).return_value;
  }
  return uVar1;
}



/*
 * xcode_send_stream @ 00418804
 *
 * Purpose:
 *   Sends elementary/TS data to `/dev/vixs/xcodedrv` using `IOCTLSDEVSEND`
 *   (command 0x13, size 0x1d8 = 472 bytes, timeout 2000 ms).
 *   Unused in `av_streamer` (`02_vixs_encoder.c.c` issues cmd 0x13 directly).
 * Return value:
 *   0 on success, otherwise `return_value`.
 */
// ==== xcode_send_stream @ 00418804 ====

uint xcode_send_stream(xcode_handle_t XCodeHandle,IOCTLSDEVSEND_conflict *pioctl_send_strm,
                      void *pbuffer,uint *pbufsize,uint64 hStream,uint flags,uint more,uint pts,
                      uint dts)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_send_strm->ioctl_header).command = 0x13;
  (pioctl_send_strm->ioctl_header).size_of_buf = 0x1d8;
  (pioctl_send_strm->ioctl_header).time_out_value = 2000;
  (pioctl_send_strm->ioctl_header).return_value = 2;
  pioctl_send_strm->stream_handle = hStream;
  *(uint *)&pioctl_send_strm->flags = flags;
  *(undefined4 *)((int)&pioctl_send_strm->flags + 4) = 0;
  *(uint *)&pioctl_send_strm->pts = pts;
  *(undefined4 *)((int)&pioctl_send_strm->pts + 4) = 0;
  *(uint *)&pioctl_send_strm->dts = dts;
  *(undefined4 *)((int)&pioctl_send_strm->dts + 4) = 0;
  pioctl_send_strm->buffer = pbuffer;
  pioctl_send_strm->size = *pbufsize;
  ioctl(XCodeHandle,0x17241724,pioctl_send_strm);
  if ((pioctl_send_strm->ioctl_header).return_value == 1) {
    *pbufsize = pioctl_send_strm->size;
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_send_strm->ioctl_header).return_value,"SEND");
    uVar1 = (pioctl_send_strm->ioctl_header).return_value;
  }
  return uVar1;
}



/*
 * xcode_update_stream @ 004189bc
 *
 * Purpose:
 *   Updates stream parameters via `IOCTLSDEVCTRL` (command 0x12, size 0x4a8 =
 *   1192 bytes, timeout 2000 ms). Sets `stream_ctrl = 1` if `parmsChanged & 0x4000`,
 *   else `0`. Unused in `av_streamer`.
 */
// ==== xcode_update_stream @ 004189bc ====

uint xcode_update_stream(xcode_handle_t XCodeHandle,IOCTLSDEVCTRL *pioctl_dev_ctrl,uint64 hStream,
                        uint parmsChanged)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_dev_ctrl->ioctl_header).command = 0x12;
  (pioctl_dev_ctrl->ioctl_header).size_of_buf = 0x4a8;
  (pioctl_dev_ctrl->ioctl_header).time_out_value = 2000;
  (pioctl_dev_ctrl->ioctl_header).return_value = 2;
  pioctl_dev_ctrl->stream_handle = hStream;
  *(uint *)&(pioctl_dev_ctrl->ioctl_param).flags = parmsChanged;
  *(undefined4 *)((int)&(pioctl_dev_ctrl->ioctl_param).flags + 4) = 0;
  if ((parmsChanged & 0x4000) == 0) {
    (pioctl_dev_ctrl->ioctl_param).stream_ctrl = 0;
  }
  else {
    (pioctl_dev_ctrl->ioctl_param).stream_ctrl = 1;
  }
  ioctl(XCodeHandle,0x17241724,pioctl_dev_ctrl);
  if ((pioctl_dev_ctrl->ioctl_header).return_value == 1) {
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_dev_ctrl->ioctl_header).return_value,"UPDATE");
    uVar1 = (pioctl_dev_ctrl->ioctl_header).return_value;
  }
  return uVar1;
}



/*
 * xcode_update_stream2 @ 00418b38
 *
 * Purpose:
 *   Extended stream control via `IOCTLSDEVCTRL2` (command 0x26, size 0x1a8 =
 *   424 bytes, timeout 2000 ms). Unused in `av_streamer`.
 */
// ==== xcode_update_stream2 @ 00418b38 ====

uint xcode_update_stream2
               (xcode_handle_t XCodeHandle,IOCTLSDEVCTRL2 *pioctl_dev_ctrl,uint64 hStream,
               uint parmsChanged)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_dev_ctrl->ioctl_header).command = 0x26;
  (pioctl_dev_ctrl->ioctl_header).size_of_buf = 0x1a8;
  (pioctl_dev_ctrl->ioctl_header).time_out_value = 2000;
  (pioctl_dev_ctrl->ioctl_header).return_value = 2;
  pioctl_dev_ctrl->stream_handle = hStream;
  (pioctl_dev_ctrl->ioctl_param).flags = parmsChanged;
  ioctl(XCodeHandle,0x17241724,pioctl_dev_ctrl);
  if ((pioctl_dev_ctrl->ioctl_header).return_value == 1) {
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_dev_ctrl->ioctl_header).return_value,"UPDATE");
    uVar1 = (pioctl_dev_ctrl->ioctl_header).return_value;
  }
  return uVar1;
}



/*
 * xcode_close_stream @ 00418c74
 *
 * Purpose:
 *   Closes an XCode4 stream handle via `IOCTLSDEVCLOSE` (command 0x11, size
 *   0xb4 = 180 bytes, timeout 2000 ms). Unused in `av_streamer`.
 * Stack layout (`IOCTLSDEVCLOSE`):
 *   local_c0 (+0x00): ioctl_header.size_of_buf = 0xb4
 *   local_bc (+0x04): ioctl_header.command     = 0x11
 *   local_b4 (+0x0c): ioctl_header.return_value = 2
 *   local_2c (+0x94): ioctl_header.time_out_value = 2000
 *   local_1c (+0xa4): stream_handle (uint64)
 */
// ==== xcode_close_stream @ 00418c74 ====

uint xcode_close_stream(xcode_handle_t XCodeHandle,uint64 hStream)

{
  undefined4 local_c0;  // ioctl_header.size_of_buf (0xb4)
  undefined4 local_bc;  // ioctl_header.command (0x11)
  uint local_b4;        // ioctl_header.return_value
  undefined4 local_2c;  // ioctl_header.time_out_value (2000)
  uint64 local_1c;      // stream_handle (+0xa4)
  
                    /* Unresolved local var: IOCTLSDEVCLOSE ioctl_close_strm@[???] */
  memset(&local_c0,0,0xb4);
  local_bc = 0x11;
  local_c0 = 0xb4;
  local_2c = 2000;
  local_b4 = 2;
  local_1c = hStream;
  ioctl(XCodeHandle,0x17241724,&local_c0);
  if (local_b4 == 1) {
    local_b4 = 0;
  }
  else {
    xcode_print_error_code(local_b4,"CLOSE");
  }
  return local_b4;
}



/*
 * xcode_open_stream_capture @ 00418d70
 *
 * Purpose:
 *   Opens a capture stream via `IOCTLSDEVOPENCAPTURE` (command 0x1f, size
 *   0x9e8 = 2536 bytes, timeout 2000 ms). Returns 64-bit `stream_handle` or 0.
 *   Unused in `av_streamer`.
 */
// ==== xcode_open_stream_capture @ 00418d70 ====

uint64 xcode_open_stream_capture
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENCAPTURE *pioctl_open_strm_capture)

{
  undefined4 uVar1;  // low 32 bits of returned stream_handle
  undefined4 uVar2;  // high 32 bits of returned stream_handle
  
  (pioctl_open_strm_capture->ioctl_header).command = 0x1f;
  (pioctl_open_strm_capture->ioctl_header).size_of_buf = 0x9e8;
  (pioctl_open_strm_capture->ioctl_header).time_out_value = 2000;
  (pioctl_open_strm_capture->ioctl_header).return_value = 2;
  *(undefined4 *)&pioctl_open_strm_capture->stream_handle = 0;
  *(undefined4 *)((int)&pioctl_open_strm_capture->stream_handle + 4) = 0;
  ioctl(XCodeHandle,0x17241724,pioctl_open_strm_capture);
  if ((pioctl_open_strm_capture->ioctl_header).return_value == 1) {
    uVar1 = (undefined4)pioctl_open_strm_capture->stream_handle;
    uVar2 = *(undefined4 *)((int)&pioctl_open_strm_capture->stream_handle + 4);
  }
  else {
    xcode_print_error_code((pioctl_open_strm_capture->ioctl_header).return_value,"OPEN_CAPTURE");
    uVar1 = 0;
    uVar2 = 0;
  }
  return CONCAT44(uVar2,uVar1);
}



/*
 * xcode_open_stream_local @ 00418ea0
 *
 * Purpose:
 *   Opens a local stream via `IOCTLSDEVOPENLOCAL` (command 0x10, size 0xd44 =
 *   3396 bytes, timeout 2000 ms). Returns 64-bit `stream_handle` or 0.
 *   Unused in `av_streamer`.
 */
// ==== xcode_open_stream_local @ 00418ea0 ====

uint64 xcode_open_stream_local
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENLOCAL *pioctl_open_strm_local)

{
  undefined4 uVar1;  // low 32 bits of returned stream_handle
  undefined4 uVar2;  // high 32 bits of returned stream_handle
  
  (pioctl_open_strm_local->ioctl_header).command = 0x10;
  (pioctl_open_strm_local->ioctl_header).size_of_buf = 0xd44;
  (pioctl_open_strm_local->ioctl_header).time_out_value = 2000;
  (pioctl_open_strm_local->ioctl_header).return_value = 2;
  *(undefined4 *)&pioctl_open_strm_local->stream_handle = 0;
  *(undefined4 *)((int)&pioctl_open_strm_local->stream_handle + 4) = 0;
  ioctl(XCodeHandle,0x17241724,pioctl_open_strm_local);
  if ((pioctl_open_strm_local->ioctl_header).return_value == 1) {
    uVar1 = (undefined4)pioctl_open_strm_local->stream_handle;
    uVar2 = *(undefined4 *)((int)&pioctl_open_strm_local->stream_handle + 4);
  }
  else {
    xcode_print_error_code((pioctl_open_strm_local->ioctl_header).return_value,"OPEN_LOCAL");
    uVar1 = 0;
    uVar2 = 0;
  }
  return CONCAT44(uVar2,uVar1);
}



/*
 * xcode_open_stream_ts @ 00418fd0
 *
 * Purpose:
 *   Opens a transport-stream encode/transcode stream via `IOCTLSDEVOPENTS`
 *   (command 0xf, size 0xa04 = 2564 bytes, timeout 2000 ms). Returns 64-bit
 *   `stream_handle` or 0. Unused in `av_streamer`.
 */
// ==== xcode_open_stream_ts @ 00418fd0 ====

uint64 xcode_open_stream_ts(xcode_handle_t XCodeHandle,IOCTLSDEVOPENTS *pioctl_open_strm_ts)

{
  undefined4 uVar1;  // low 32 bits of returned stream_handle
  undefined4 uVar2;  // high 32 bits of returned stream_handle
  
  (pioctl_open_strm_ts->ioctl_header).command = 0xf;
  (pioctl_open_strm_ts->ioctl_header).size_of_buf = 0xa04;
  (pioctl_open_strm_ts->ioctl_header).time_out_value = 2000;
  (pioctl_open_strm_ts->ioctl_header).return_value = 2;
  *(undefined4 *)&pioctl_open_strm_ts->stream_handle = 0;
  *(undefined4 *)((int)&pioctl_open_strm_ts->stream_handle + 4) = 0;
  ioctl(XCodeHandle,0x17241724,pioctl_open_strm_ts);
  if ((pioctl_open_strm_ts->ioctl_header).return_value == 1) {
    uVar1 = (undefined4)pioctl_open_strm_ts->stream_handle;
    uVar2 = *(undefined4 *)((int)&pioctl_open_strm_ts->stream_handle + 4);
  }
  else {
    xcode_print_error_code((pioctl_open_strm_ts->ioctl_header).return_value,"OPEN_TS");
    uVar1 = 0;
    uVar2 = 0;
  }
  return CONCAT44(uVar2,uVar1);
}



/*
 * xcode_open_stream_ts_passthrough @ 00419100
 *
 * Purpose:
 *   Opens a TS passthrough stream via `IOCTLSDEVOPENTSPASSTHROUGH` (command
 *   0x2d, size 0x564 = 1380 bytes, timeout 2000 ms). Unused in `av_streamer`.
 * Quirk:
 *   Does not initialize `ioctl_header.return_value = 2` or zero `stream_handle`
 *   before calling `ioctl()`.
 */
// ==== xcode_open_stream_ts_passthrough @ 00419100 ====

uint64 xcode_open_stream_ts_passthrough
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENTSPASSTHROUGH *pioctl_open_passthrough)

{
  undefined4 uVar1;  // low 32 bits of returned stream_handle
  undefined4 uVar2;  // high 32 bits of returned stream_handle
  
  (pioctl_open_passthrough->ioctl_header).command = 0x2d;
  (pioctl_open_passthrough->ioctl_header).size_of_buf = 0x564;
  (pioctl_open_passthrough->ioctl_header).time_out_value = 2000;
  ioctl(XCodeHandle,0x17241724,pioctl_open_passthrough);
  if ((pioctl_open_passthrough->ioctl_header).return_value == 1) {
    uVar1 = (undefined4)pioctl_open_passthrough->stream_handle;
    uVar2 = *(undefined4 *)((int)&pioctl_open_passthrough->stream_handle + 4);
  }
  else {
    xcode_print_error_code((pioctl_open_passthrough->ioctl_header).return_value,"OPEN_PASSTHROUGH");
    uVar1 = 0;
    uVar2 = 0;
  }
  return CONCAT44(uVar2,uVar1);
}



/*
 * xcode_tsd_dvo_config @ 0041920c
 *
 * Purpose:
 *   Configures Transport Stream Demux (TSD) DVO output via `IOCTLTSDCONFIG`
 *   (command 0x101, sub_command 1, size 0xf4 = 244 bytes, timeout 2000 ms).
 *   Unused in `av_streamer`.
 */
// ==== xcode_tsd_dvo_config @ 0041920c ====

uint xcode_tsd_dvo_config(xcode_handle_t XCodeHandle,IOCTLTSDCONFIG *pioctl_tsdconfig)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_tsdconfig->ioctl_header).command = 0x101;
  (pioctl_tsdconfig->ioctl_header).size_of_buf = 0xf4;
  (pioctl_tsdconfig->ioctl_header).time_out_value = 2000;
  (pioctl_tsdconfig->ioctl_header).sub_command = 1;
  (pioctl_tsdconfig->ioctl_header).return_value = 2;
  ioctl(XCodeHandle,0x17241724,pioctl_tsdconfig);
  if ((pioctl_tsdconfig->ioctl_header).return_value == 1) {
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_tsdconfig->ioctl_header).return_value,"TSD_CONFIG");
    uVar1 = (pioctl_tsdconfig->ioctl_header).return_value;
  }
  return uVar1;
}



/*
 * xcode_dma_host_to_fb @ 004192f4
 *
 * Purpose:
 *   DMAs `size` bytes from host buffer `host_buf` to XCode4 frame-buffer
 *   offset `fb_addr` via `IOCTLDMAHOSTFB` (command 0xb, size 0xb4 = 180 bytes,
 *   timeout 2000 ms). Unused in `av_streamer`.
 * Stack layout (`IOCTLDMAHOSTFB`):
 *   local_c0 (+0x00): size_of_buf = 0xb4
 *   local_bc (+0x04): command = 0xb
 *   local_b4 (+0x0c): return_value = 2
 *   local_2c (+0x94): time_out_value = 2000
 *   local_1c (+0xa4): size_in_bytes_of_mem
 *   local_18 (+0xa8): palloc_mem (host_buf)
 *   local_10 (+0xb0): fb_dma_address (fb_addr)
 */
// ==== xcode_dma_host_to_fb @ 004192f4 ====

uint xcode_dma_host_to_fb(xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  undefined4 local_c0;  // ioctl_header.size_of_buf (0xb4)
  undefined4 local_bc;  // ioctl_header.command (0xb)
  uint local_b4;        // ioctl_header.return_value
  undefined4 local_2c;  // ioctl_header.time_out_value (2000)
  uint local_1c;        // size_in_bytes_of_mem (+0xa4)
  void *local_18;       // palloc_mem (+0xa8)
  uint local_10;        // fb_dma_address (+0xb0)
  
                    /* Unresolved local var: IOCTLDMAHOSTFB ioctl_dma_host_fb@[???] */
  memset(&local_c0,0,0xb4);
  local_bc = 0xb;
  local_c0 = 0xb4;
  local_2c = 2000;
  local_b4 = 2;
  local_1c = size;
  local_18 = host_buf;
  local_10 = fb_addr;
  ioctl(XCodeHandle,0x17241724,&local_c0);
  if (local_b4 == 1) {
    local_b4 = 0;
  }
  else {
    xcode_print_error_code(local_b4,"HOST_DMA_FB");
  }
  return local_b4;
}



/*
 * xcode_dma_host_to_fb_large_chunk @ 004193fc
 *
 * Purpose:
 *   Splits host-to-FB DMA transfers larger than 512 KiB (`0x80000`) into
 *   512 KiB chunks calling `xcode_dma_host_to_fb`. Unused in `av_streamer`.
 * Bug:
 *   Return value of intermediate 512 KiB chunk calls is ignored.
 */
// ==== xcode_dma_host_to_fb_large_chunk @ 004193fc ====

uint xcode_dma_host_to_fb_large_chunk
               (xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  uint uVar1;       // return code from final chunk (or 0 if exact multiple of 512 KiB)
  void *local_res4; // advancing host_buf pointer
  uint local_res8;  // advancing fb_addr offset
  uint local_resc;  // remaining byte count
  
  local_res4 = host_buf;
  local_res8 = fb_addr;
  for (local_resc = size; 0x80000 < local_resc; local_resc = local_resc - 0x80000) {
    xcode_dma_host_to_fb(XCodeHandle,local_res4,local_res8,0x80000);
    local_res4 = (void *)((int)local_res4 + 0x80000);
    local_res8 = local_res8 + 0x80000;
  }
  if (local_resc == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = xcode_dma_host_to_fb(XCodeHandle,local_res4,local_res8,local_resc);
  }
  return uVar1;
}



/*
 * xcode_dma_fb_to_host @ 004194d4
 *
 * Purpose:
 *   DMAs `size` bytes from XCode4 frame-buffer offset `fb_addr` to `host_buf`
 *   via `IOCTLDMAHOSTFB` (command 0xc, size 0xb4 = 180 bytes, timeout 2000 ms).
 *   Unused in `av_streamer`.
 */
// ==== xcode_dma_fb_to_host @ 004194d4 ====

uint xcode_dma_fb_to_host(xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  undefined4 local_c0;  // ioctl_header.size_of_buf (0xb4)
  undefined4 local_bc;  // ioctl_header.command (0xc)
  uint local_b4;        // ioctl_header.return_value
  undefined4 local_2c;  // ioctl_header.time_out_value (2000)
  uint local_1c;        // size_in_bytes_of_mem (+0xa4)
  void *local_18;       // palloc_mem (+0xa8)
  uint local_10;        // fb_dma_address (+0xb0)
  
                    /* Unresolved local var: IOCTLDMAHOSTFB ioctl_dma_host_fb@[???] */
  memset(&local_c0,0,0xb4);
  local_bc = 0xc;
  local_c0 = 0xb4;
  local_2c = 2000;
  local_b4 = 2;
  local_1c = size;
  local_18 = host_buf;
  local_10 = fb_addr;
  ioctl(XCodeHandle,0x17241724,&local_c0);
  if (local_b4 == 1) {
    local_b4 = 0;
  }
  else {
    xcode_print_error_code(local_b4,"FB_DMA_HOST");
  }
  return local_b4;
}



/*
 * xcode_dma_fb_to_fb @ 004195dc
 *
 * Purpose:
 *   DMAs `size` bytes within XCode4 frame-buffer memory from `src_addr` to
 *   `dst_addr` via `IOCTLDMAFBFB` (command 0xd, size 0xb0 = 176 bytes, timeout
 *   2000 ms). Unused in `av_streamer`.
 */
// ==== xcode_dma_fb_to_fb @ 004195dc ====

uint xcode_dma_fb_to_fb(xcode_handle_t XCodeHandle,uint src_addr,uint dst_addr,uint size)

{
  undefined4 local_b8;  // ioctl_header.size_of_buf (0xb0)
  undefined4 local_b4;  // ioctl_header.command (0xd)
  uint local_ac;        // ioctl_header.return_value
  undefined4 local_24;  // ioctl_header.time_out_value (2000)
  uint local_14;        // size_in_bytes_of_mem (+0xa4)
  uint local_10;        // fb_dma_address_dst (+0xa8)
  uint local_c;         // fb_dma_address_src (+0xac)
  
                    /* Unresolved local var: IOCTLDMAFBFB ioctl_dma_fb_fb@[???] */
  memset(&local_b8,0,0xb0);
  local_b4 = 0xd;
  local_b8 = 0xb0;
  local_24 = 2000;
  local_ac = 2;
  local_14 = size;
  local_10 = dst_addr;
  local_c = src_addr;
  ioctl(XCodeHandle,0x17241724,&local_b8);
  if (local_ac == 1) {
    local_ac = 0;
  }
  else {
    xcode_print_error_code(local_ac,"FB_DMA_FB");
  }
  return local_ac;
}



/*
 * xcode_dma_pattern_to_fb @ 004196e4
 *
 * Purpose:
 *   Fills `size` bytes at XCode4 frame-buffer offset `fb_addr` with 32-bit
 *   `pattern` via `IOCTLDMAPATTERNFB` (command 0x17, size 0xb0 = 176 bytes,
 *   timeout 2000 ms). Unused in `av_streamer`.
 */
// ==== xcode_dma_pattern_to_fb @ 004196e4 ====

uint xcode_dma_pattern_to_fb(xcode_handle_t XCodeHandle,uint fb_addr,uint pattern,uint size)

{
  undefined4 local_b8;  // ioctl_header.size_of_buf (0xb0)
  undefined4 local_b4;  // ioctl_header.command (0x17)
  uint local_ac;        // ioctl_header.return_value
  undefined4 local_24;  // ioctl_header.time_out_value (2000)
  uint local_14;        // size_in_bytes_of_mem (+0xa4)
  uint local_10;        // fb_dma_address_dst (+0xa8)
  uint local_c;         // pattern (+0xac)
  
                    /* Unresolved local var: IOCTLDMAPATTERNFB ioctl_dma_pattern_fb@[???] */
  memset(&local_b8,0,0xb0);
  local_b4 = 0x17;
  local_b8 = 0xb0;
  local_24 = 2000;
  local_ac = 2;
  local_14 = size;
  local_10 = fb_addr;
  local_c = pattern;
  ioctl(XCodeHandle,0x17241724,&local_b8);
  if (local_ac == 1) {
    local_ac = 0;
  }
  else {
    xcode_print_error_code(local_ac,"PATTERN_DMA_FB");
  }
  return local_ac;
}



/*
 * xcode_dma_pattern_to_fb_large_chunk @ 004197ec
 *
 * Purpose:
 *   Splits pattern-fill DMA transfers larger than 512 KiB (`0x80000`) into
 *   512 KiB chunks calling `xcode_dma_pattern_to_fb`. Unused in `av_streamer`.
 * Bug:
 *   Return value of intermediate 512 KiB chunk calls is ignored.
 */
// ==== xcode_dma_pattern_to_fb_large_chunk @ 004197ec ====

uint xcode_dma_pattern_to_fb_large_chunk
               (xcode_handle_t XCodeHandle,uint fb_addr,uint pattern,uint size)

{
  uint uVar1;       // return code from final chunk (or 0 if exact multiple of 512 KiB)
  uint local_res4;  // advancing fb_addr offset
  uint local_resc;  // remaining byte count
  
  local_res4 = fb_addr;
  for (local_resc = size; 0x80000 < local_resc; local_resc = local_resc - 0x80000) {
    xcode_dma_pattern_to_fb(XCodeHandle,local_res4,pattern,0x80000);
    local_res4 = local_res4 + 0x80000;
  }
  if (local_resc == 0) {
    uVar1 = 0;
  }
  else {
    uVar1 = xcode_dma_pattern_to_fb(XCodeHandle,local_res4,pattern,local_resc);
  }
  return uVar1;
}



/*
 * xcode_ts_control @ 004198b4
 *
 * Purpose:
 *   Controls transport-stream mux/PID parameters via `IOCTLSDEVTSCTRL` (command
 *   0x102, size 0x450 = 1104 bytes, timeout 2000 ms). Unused in `av_streamer`.
 * Quirk:
 *   Takes `int hStream` (32-bit) and sign-extends it (`hStream >> 0x1f`) into
 *   the 64-bit `stream_handle` field.
 */
// ==== xcode_ts_control @ 004198b4 ====

uint xcode_ts_control(xcode_handle_t XCodeHandle,IOCTLSDEVTSCTRL *pioctl_ts_ctrl,int hStream,
                     uint parmsChanged)

{
  uint uVar1;  // 0 on success, else ioctl_header.return_value
  
  (pioctl_ts_ctrl->ioctl_header).command = 0x102;
  (pioctl_ts_ctrl->ioctl_header).size_of_buf = 0x450;
  (pioctl_ts_ctrl->ioctl_header).time_out_value = 2000;
  (pioctl_ts_ctrl->ioctl_header).return_value = 2;
  *(int *)&pioctl_ts_ctrl->stream_handle = hStream;
  *(int *)((int)&pioctl_ts_ctrl->stream_handle + 4) = hStream >> 0x1f;
  (pioctl_ts_ctrl->ioctl_param).flags = parmsChanged;
  ioctl(XCodeHandle,0x17241724,pioctl_ts_ctrl);
  if ((pioctl_ts_ctrl->ioctl_header).return_value == 1) {
    uVar1 = 0;
  }
  else {
    xcode_print_error_code((pioctl_ts_ctrl->ioctl_header).return_value,"TS_CTL");
    uVar1 = (pioctl_ts_ctrl->ioctl_header).return_value;
  }
  return uVar1;
}




/* #####################################################################
 * FILE: 08_dvbpsi.c.c
 * ##################################################################### */

/*
 * 08_dvbpsi.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/dvbpsi.c
 * =========================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/dvbpsi.c
 *                   (DWARF CU @ 0x419a00..0x41a76c, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x handle-based API (dvbpsi_new /
 *                   dvbpsi_delete / dvbpsi_decoder_new / dvbpsi_packet_push),
 *                   as opposed to the legacy 0.x API without dvbpsi_t handles.
 *
 * Role in av_streamer:
 *   Core handle lifecycle, generic PSI section-reassembly state machine
 *   (dvbpsi_packet_push), and logging helpers (dvbpsi_error/warning/debug).
 *   In av_streamer, 01_libxcode4encoder.c.c calls dvbpsi_new(message,
 *   DVBPSI_MSG_DEBUG) and dvbpsi_delete() when generating DVB SI tables
 *   (genNIT, genSDT, genTDT, genEIT); the decoder/packet_push path is linked
 *   in from libdvbpsi.a (not called by the encoder-only SI generators).
 *
 * Main data structures (from DWARF):
 *   dvbpsi_t (struct dvbpsi_s, 16 bytes):
 *     +0x00  dvbpsi_decoder_t *p_decoder   (attached PSI/SI decoder, or NULL)
 *     +0x04  dvbpsi_message_cb pf_message  (logging callback)
 *     +0x08  dvbpsi_msg_level_t i_msg_level (-1=NONE, 0=ERROR, 1=WARN, 2=DEBUG)
 *     +0x0c  void *p_sys                   (user/system private pointer)
 *
 *   dvbpsi_decoder_t (struct dvbpsi_decoder_s, 28 bytes = 0x1c):
 *     +0x00  uint8_t i_magic[3]            ("psi" = 0x70, 0x73, 0x69)
 *     +0x03  _Bool b_complete_header       (true once 3-byte PSI header read)
 *     +0x04  _Bool b_discontinuity         (TS continuity discontinuity flag)
 *     +0x05  _Bool b_current_valid         (true once a valid table is current)
 *     +0x06  uint8_t i_continuity_counter  (last TS CC, 0xff = uninitialized)
 *     +0x07  uint8_t i_last_section_number (expected last section_number)
 *     +0x08  dvbpsi_psi_section_t *p_current_section (in-progress section)
 *     +0x0c  dvbpsi_psi_section_t *p_sections        (sorted completed list)
 *     +0x10  dvbpsi_callback_gather_t pf_gather      (section callback)
 *     +0x14  int i_section_max_size        (max section buffer size)
 *     +0x18  int i_need                    (bytes remaining for header/payload)
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 dvbpsi.c.
 */

/* dvbpsi_new @ 0x00419a00
 * Purpose  : Allocate and initialize a top-level libdvbpsi handle.
 * Params   : callback - user logging callback (dvbpsi_message_cb), or NULL
 *            level    - verbosity threshold (dvbpsi_msg_level)
 * Returns  : Newly allocated dvbpsi_t * on success, or NULL on OOM.
 * Callers  : genNIT, genSDT, genTDT, genEIT in 01_libxcode4encoder.c.c
 * Callees  : calloc
 */
// ==== dvbpsi_new @ 00419a00 ====

dvbpsi_t * dvbpsi_new(dvbpsi_message_cb callback,dvbpsi_msg_level level)

{
  dvbpsi_t *pdVar1;          // allocated dvbpsi_t handle
  
                    /* Unresolved local var: dvbpsi_t * p_dvbpsi@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);   // sizeof(dvbpsi_s) == 16 bytes
  if (pdVar1 != (dvbpsi_t *)0x0) {
    pdVar1->p_decoder = (dvbpsi_decoder_t *)0x0;
    pdVar1->pf_message = callback;
    pdVar1->i_msg_level = level;
  }
  return pdVar1;
}



/* dvbpsi_delete @ 0x00419a5c
 * Purpose  : Free a libdvbpsi handle after its decoder has been detached.
 *            Asserts p_dvbpsi->p_decoder == NULL before freeing.
 * Params   : p_dvbpsi - handle previously created by dvbpsi_new()
 * Returns  : void
 * Callers  : genNIT, genSDT, genTDT, genEIT in 01_libxcode4encoder.c.c
 * Callees  : __assert_fail, free
 */
// ==== dvbpsi_delete @ 00419a5c ====

void dvbpsi_delete(dvbpsi_t *p_dvbpsi)

{
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","dvbpsi.c",0x44,"dvbpsi_delete");
  }
  p_dvbpsi->pf_message = (dvbpsi_message_cb)0x0;
  free(p_dvbpsi);
  return;
}



/* dvbpsi_decoder_new @ 0x00419ab0
 * Purpose  : Allocate and initialize a PSI/SI decoder base struct (or a larger
 *            table-specific decoder struct whose first member is
 *            dvbpsi_decoder_t, e.g. dvbpsi_tot_t / dvbpsi_demux_t).
 * Params   : pf_gather          - callback invoked per completed PSI section
 *            i_section_max_size - max section size (e.g. 1024 or 4096)
 *            b_discontinuity      - initial discontinuity state
 *            psi_size           - total allocation size (>= sizeof(dvbpsi_decoder_t) = 0x1c)
 * Returns  : Pointer to zero-initialized decoder struct, or NULL on OOM.
 * Callers  : dvbpsi_tot_attach (13_tables_tot.c.c), dvbpsi_AttachDemux (14_demux.c.c), etc.
 * Callees  : __assert_fail, calloc
 */
// ==== dvbpsi_decoder_new @ 00419ab0 ====

void * dvbpsi_decoder_new(dvbpsi_callback_gather_t pf_gather,int i_section_max_size,
                         _Bool b_discontinuity,size_t psi_size)

{
  undefined2 *puVar1;        // raw pointer to allocated dvbpsi_decoder_t (viewed as uint16_t*)
  undefined2 *puVar2;        // return value (same as puVar1, or NULL on OOM)
  
                    /* Unresolved local var: dvbpsi_decoder_t * p_decoder@[???] */
  if (psi_size < 0x1c) {     // 0x1c == sizeof(dvbpsi_decoder_s)
                    /* WARNING: Subroutine does not return */
    __assert_fail("psi_size >= sizeof(dvbpsi_decoder_t)","dvbpsi.c",0x51,"dvbpsi_decoder_new");
  }
  puVar1 = calloc(1,psi_size);
  puVar2 = (undefined2 *)0x0;
  if (puVar1 != (undefined2 *)0x0) {
    *(dvbpsi_callback_gather_t *)(puVar1 + 8) = pf_gather;   // +0x10: pf_gather
    *puVar1 = 0x7370;                                        // +0x00: i_magic[0..1] = 'p','s' (LE)
    *(undefined1 *)(puVar1 + 1) = 0x69;                      // +0x02: i_magic[2] = 'i' ("psi")
    *(undefined1 *)(puVar1 + 3) = 0xff;                      // +0x06: i_continuity_counter = 0xff (unset)
    *(undefined4 *)(puVar1 + 4) = 0;                         // +0x08: p_current_section = NULL
    *(int *)(puVar1 + 10) = i_section_max_size;              // +0x14: i_section_max_size
    *(_Bool *)(puVar1 + 2) = b_discontinuity;                // +0x04: b_discontinuity
    *(undefined1 *)((int)puVar1 + 5) = 0;                    // +0x05: b_current_valid = false
    *(undefined1 *)((int)puVar1 + 7) = 0;                    // +0x07: i_last_section_number = 0
    *(undefined4 *)(puVar1 + 6) = 0;                         // +0x0c: p_sections = NULL
    *(undefined1 *)((int)puVar1 + 3) = 0;                    // +0x03: b_complete_header = false
    puVar2 = puVar1;
  }
  return puVar2;
}



/* dvbpsi_decoder_reset @ 0x00419b80
 * Purpose  : Discard all queued completed PSI sections on a decoder and
 *            optionally invalidate the current table state.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 *            b_force   - if true, clears b_current_valid
 * Returns  : void
 * Callers  : table gather callbacks (e.g. dvbpsi_tot_sections_gather, etc.)
 * Callees  : __assert_fail, dvbpsi_DeletePSISections
 */
// ==== dvbpsi_decoder_reset @ 00419b80 ====

void dvbpsi_decoder_reset(dvbpsi_decoder_t *p_decoder,_Bool b_force)

{
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x6c,"dvbpsi_decoder_reset");
  }
  if (b_force) {
    p_decoder->b_current_valid = false;
  }
  dvbpsi_DeletePSISections(p_decoder->p_sections);
  p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  return;
}



/* dvbpsi_decoder_psi_sections_completed @ 0x00419bf0
 * Purpose  : Check whether the decoder's sorted p_sections list contains a
 *            contiguous sequence of sections 0 .. i_last_section_number.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 * Returns  : true if all sections 0..i_last_section_number are present, else false.
 * Callers  : table gather callbacks (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_decoder_psi_sections_completed @ 00419bf0 ====

_Bool dvbpsi_decoder_psi_sections_completed(dvbpsi_decoder_t *p_decoder)

{
  _Bool _Var1;               // b_complete result
  dvbpsi_psi_section_t *pdVar2; // cursor walking p_decoder->p_sections
  uint uVar3;                // prev_nr: section number of current node
  uint uVar4;                // expected next section number (prev_nr + 1)
  
                    /* Unresolved local var: _Bool b_complete@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p@[???]
                       Unresolved local var: uint32_t prev_nr@[???] */
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x7c,"dvbpsi_decoder_psi_sections_completed");
  }
  pdVar2 = p_decoder->p_sections;
  _Var1 = false;
  if ((pdVar2 != (dvbpsi_psi_section_t *)0x0) && (pdVar2->i_number == 0)) {
    uVar3 = 0;
    do {
      pdVar2 = pdVar2->p_next;
      if (p_decoder->i_last_section_number == uVar3) {
        _Var1 = true;        // reached expected last_section_number contiguously
      }
      if (pdVar2 == (dvbpsi_psi_section_s *)0x0) {
        return _Var1;
      }
      uVar4 = uVar3 + 1;
      if (uVar4 == 0x100) {  // section_number is 8-bit (0..255)
                    /* WARNING: Subroutine does not return */
        __assert_fail(s_prev_nr_<_256_00424318,"dvbpsi.c",0x84,
                      "dvbpsi_decoder_psi_sections_completed");
      }
      uVar3 = (uint)pdVar2->i_number;
    } while (uVar3 == uVar4);
  }
  return _Var1;
}



/* dvbpsi_decoder_psi_section_add @ 0x00419cac
 * Purpose  : Insert a newly gathered PSI section into p_decoder->p_sections
 *            sorted by ascending i_number. If a section with the same i_number
 *            already exists, it is replaced and freed.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 *            p_section - detached section (asserted non-NULL, p_next == NULL)
 * Returns  : true if an existing section with the same i_number was overwritten,
 *            false if inserted as a new section number.
 * Callers  : table gather callbacks
 * Callees  : __assert_fail, dvbpsi_DeletePSISections
 */
// ==== dvbpsi_decoder_psi_section_add @ 00419cac ====

_Bool dvbpsi_decoder_psi_section_add(dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                // p_section->i_number
  _Bool _Var2;               // b_overwrite return flag
  dvbpsi_psi_section_t *p_section_00; // p: current node during traversal
  dvbpsi_psi_section_t *p_section_01; // p_prev: previous node (initially head)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: _Bool b_overwrite@[???] */
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x95,"dvbpsi_decoder_psi_section_add");
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section","dvbpsi.c",0x96,"dvbpsi_decoder_psi_section_add");
  }
  if (p_section->p_next != (dvbpsi_psi_section_s *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section->p_next == ((void *)0)","dvbpsi.c",0x97,
                  "dvbpsi_decoder_psi_section_add");
  }
  p_section_01 = p_decoder->p_sections;
  if (p_section_01 == (dvbpsi_psi_section_t *)0x0) {
    // Empty list: insert at head
    p_decoder->p_sections = p_section;
    p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    return false;
  }
  bVar1 = p_section->i_number;
  if (p_section_01->i_number == bVar1) {
    // Replace head node with matching section number
    p_section->p_next = p_section_01->p_next;
    p_section_01->p_next = (dvbpsi_psi_section_s *)0x0;
    dvbpsi_DeletePSISections(p_section_01);
    _Var2 = true;
    p_decoder->p_sections = p_section;
  }
  else if (bVar1 < p_section_01->i_number) {
    // Insert before head
    p_section->p_next = p_section_01;
    _Var2 = false;
    p_decoder->p_sections = p_section;
  }
  else {
    // Walk list to find insertion point or matching i_number
    while (p_section_00 = p_section_01->p_next, p_section_00 != (dvbpsi_psi_section_t *)0x0) {
      if (p_section_00->i_number == bVar1) {
        p_section_01->p_next = p_section;
        p_section->p_next = p_section_00->p_next;
        p_section_00->p_next = (dvbpsi_psi_section_s *)0x0;
        dvbpsi_DeletePSISections(p_section_00);
        return true;
      }
      if (bVar1 < p_section_00->i_number) {
        p_section_01->p_next = p_section;
        p_section->p_next = p_section_00;
        return false;
      }
      p_section_01 = p_section_00;
    }
    // Append at tail
    _Var2 = false;
    if (p_section_01->i_number < bVar1) {
      p_section_01->p_next = p_section;
      p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return _Var2;
}



/* dvbpsi_decoder_delete @ 0x00419e54
 * Purpose  : Free all queued sections, any in-progress current section, and
 *            the decoder struct itself.
 * Params   : p_decoder - decoder instance (asserted non-NULL)
 * Returns  : void
 * Callers  : dvbpsi_tot_detach (13_tables_tot.c.c), dvbpsi_DetachDemux (14_demux.c.c), etc.
 * Callees  : __assert_fail, dvbpsi_DeletePSISections, free
 */
// ==== dvbpsi_decoder_delete @ 00419e54 ====

void dvbpsi_decoder_delete(dvbpsi_decoder_t *p_decoder)

{
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0xe2,"dvbpsi_decoder_delete");
  }
  if (p_decoder->p_sections != (dvbpsi_psi_section_t *)0x0) {
    dvbpsi_DeletePSISections(p_decoder->p_sections);
    p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  }
  dvbpsi_DeletePSISections(p_decoder->p_current_section);
  free(p_decoder);
  return;
}



/* dvbpsi_decoder_present @ 0x00419edc
 * Purpose  : Check whether a dvbpsi_t handle is non-NULL and has a decoder attached.
 * Params   : p_dvbpsi - handle to inspect
 * Returns  : true if p_dvbpsi != NULL && p_dvbpsi->p_decoder != NULL
 */
// ==== dvbpsi_decoder_present @ 00419edc ====

_Bool dvbpsi_decoder_present(dvbpsi_t *p_dvbpsi)

{
  bool bVar1;
  
  bVar1 = false;
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    bVar1 = p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0;
  }
  return bVar1;
}



/* dvbpsi_error @ 0x00419ef4
 * Purpose  : Format and emit a DVBPSI_MSG_ERROR message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_NONE (-1).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_error @ 00419ef4 ====

void dvbpsi_error(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
  if (DVBPSI_MSG_NONE < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_ERROR,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}



/* dvbpsi_packet_push @ 0x00419fe4
 * Purpose  : Feed one 188-byte (0xbc) MPEG-2 TS packet into the attached PSI
 *            decoder, checking sync byte (0x47), continuity counter, adaptation
 *            field, and pointer_field (PUSI), reassembling PSI sections and
 *            validating CRC-32 before invoking p_decoder->pf_gather.
 * Params   : p_dvbpsi - handle with attached decoder (asserted non-NULL)
 *            p_data   - pointer to 188-byte TS packet
 * Returns  : true on normal consumption (or end of packet), false on error
 *            (bad sync, duplicate CC, no payload, waiting for PUSI, or OOM).
 * Quirks   : Upstream libdvbpsi 1.1.0 bug at lines 391-405: dvbpsi_has_CRC32()
 *            reads p_section->i_table_id and p_section->b_syntax_indicator
 *            BEFORE they are populated from p_section->p_data[0..1].
 * Callees  : __assert_fail, dvbpsi_error, dvbpsi_DeletePSISections,
 *            dvbpsi_NewPSISection, dvbpsi_ValidPSISection, memcpy, pf_gather
 */
// ==== dvbpsi_packet_push @ 00419fe4 ====

_Bool dvbpsi_packet_push(dvbpsi_t *p_dvbpsi,uint8_t *p_data)

{
  uint8_t uVar1;             // scratch for i_table_id / i_last_number
  bool bVar2;                // result of dvbpsi_has_CRC32(p_section) check
  _Bool _Var3;               // scratch bool (b_complete_header / b_syntax_indicator / ValidPSI)
  uint uVar4;                // i_expected_counter, then section_length
  int iVar5;                 // i_need / i_section_max_size scratch
  uint8_t *puVar6;           // p_section->p_data
  undefined3 extraout_var;   // MIPS ABI upper 24 bits of _Bool return
  undefined3 extraout_var_00;// MIPS ABI upper 24 bits of _Bool return
  uint8_t *puVar7;           // updated p_payload_end / p_section->p_data
  char cVar8;                // cached b_syntax_indicator
  uint uVar9;                // received TS continuity_counter (p_data[3] & 0xf)
  dvbpsi_psi_section_t *p_section; // active section being assembled
  dvbpsi_decoder_t *pdVar10; // p_decoder = p_dvbpsi->p_decoder
  uint8_t *__n;              // i_available: bytes remaining in 188-byte TS packet
  byte *__src;               // p_payload_pos: current read pointer inside p_data
  byte *pbVar11;             // p_new_pos: start of new section if PUSI is set
  byte *pbVar12;             // pending start of next section in same TS packet
  
                    /* Unresolved local var: uint8_t i_expected_counter@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_section@[???]
                       Unresolved local var: uint8_t * p_payload_pos@[???]
                       Unresolved local var: uint8_t * p_new_pos@[???]
                       Unresolved local var: int i_available@[???]
                       Unresolved local var: dvbpsi_decoder_t * p_decoder@[???]
                       Unresolved local var: _Bool b_first@[???] */
  pdVar10 = p_dvbpsi->p_decoder;
  if (pdVar10 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","dvbpsi.c",0x10a,"dvbpsi_packet_push");
  }
  if (*p_data != 0x47) {     // MPEG-2 TS sync byte 0x47 ('G')
    dvbpsi_error(p_dvbpsi,"PSI decoder","not a TS packet");
    return false;
  }
  // Continuity counter (CC) check: p_data[3] bits 3..0
  if (pdVar10->i_continuity_counter == 0xff) {
    pdVar10->i_continuity_counter = p_data[3] & 0xf;
  }
  else {
    uVar4 = pdVar10->i_continuity_counter + 1 & 0xf;
    uVar9 = p_data[3] & 0xf;
    pdVar10->i_continuity_counter = (uint8_t)uVar9;
    if ((uVar4 == (uVar9 + 1 & 0xf)) && (pdVar10->b_discontinuity == false)) {
      // Same CC twice in a row -> duplicate packet; PID = ((p_data[1] & 0x1f) << 8) | p_data[2]
      dvbpsi_error(p_dvbpsi,"PSI decoder","TS duplicate (received %d, expected %d) for PID %d",uVar9
                   ,uVar4,(p_data[1] & 0x1f) << 8 | (uint)p_data[2]);
      return false;
    }
    if (uVar4 != uVar9) {
      dvbpsi_error(p_dvbpsi,"PSI decoder","TS discontinuity (received %d, expected %d) for PID %d",
                   uVar9,uVar4,(p_data[1] & 0x1f) << 8 | (uint)p_data[2]);
      pdVar10->b_discontinuity = true;
      if (pdVar10->p_current_section != (dvbpsi_psi_section_t *)0x0) {
        dvbpsi_DeletePSISections(pdVar10->p_current_section);
        pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      }
    }
  }
  // Adaptation field control (p_data[3] & 0x30): bit 4 = payload present, bit 5 = adaptation field
  if ((p_data[3] & 0x10) == 0) {
    return false;            // no payload in TS packet
  }
  __src = p_data + 4;
  if ((p_data[3] & 0x20) != 0) {
    __src = p_data + p_data[4] + 5; // skip adaptation_field_length + adaptation_field
  }
  // Payload Unit Start Indicator (PUSI = p_data[1] & 0x40): pointer_field follows
  pbVar11 = (byte *)0x0;
  if ((p_data[1] & 0x40) != 0) {
    pbVar11 = __src + *__src + 1; // start of new section after pointer_field bytes
    __src = __src + 1;
  }
  p_section = pdVar10->p_current_section;
  pbVar12 = pbVar11;
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
    if (pbVar11 == (byte *)0x0) {
      return false;          // no section in progress and no PUSI -> wait for start
    }
    p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
    pdVar10->p_current_section = p_section;
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return false;
    }
    pdVar10->i_need = 3;     // need 3-byte common PSI header first
    pdVar10->b_complete_header = false;
    pbVar12 = (byte *)0x0;
    __src = pbVar11;
  }
  __n = p_data + (0xbc - (int)__src); // bytes left in 188-byte (0xbc) TS packet
  if ((int)__n < 1) {
    return true;
  }
  do {
    while( true ) {
      if ((int)__n < pdVar10->i_need) {
        // Partial copy; more bytes needed from subsequent TS packet(s)
        memcpy(p_section->p_payload_end,__src,(size_t)__n);
        iVar5 = pdVar10->i_need;
        p_section->p_payload_end = p_section->p_payload_end + (int)__n;
        pdVar10->i_need = iVar5 - (int)__n;
        return true;
      }
      memcpy(p_section->p_payload_end,__src,pdVar10->i_need);
      iVar5 = pdVar10->i_need;
      _Var3 = pdVar10->b_complete_header;
      puVar7 = p_section->p_payload_end + iVar5;
      __src = __src + iVar5;
      p_section->p_payload_end = puVar7;
      __n = __n + -iVar5;
      if (_Var3 == false) break;
      // Full section body received. Check dvbpsi_has_CRC32():
      // true unless table_id in {0x70(TDT), 0x71(RST), 0x72(ST), 0x7e(DIT)},
      // or if b_syntax_indicator==false and table_id != 0x73(TOT).
      // NOTE: upstream bug in libdvbpsi 1.1.0 reads i_table_id/b_syntax_indicator
      // before parsing them from p_section->p_data below.
      uVar1 = p_section->i_table_id;
      bVar2 = false;
      if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
         (bVar2 = true, p_section->b_syntax_indicator == false)) {
        bVar2 = uVar1 == 0x73;
      }
      puVar6 = p_section->p_data;
      _Var3 = (_Bool)-((char)puVar6[1] >> 7); // bit 7 of byte 1: section_syntax_indicator
      p_section->b_syntax_indicator = _Var3;
      p_section->b_private_indicator = SUB41((puVar6[1] & 0x7f) >> 6,0); // bit 6: private_indicator
      if (_Var3 == false) {
        cVar8 = '\0';
        if (!bVar2) {
LAB_0041a39c:
          p_section->i_table_id = *puVar6;
          if (cVar8 != '\0') goto LAB_0041a3a8;
LAB_0041a424:
          // Short section (syntax_indicator == 0): 3-byte header only
          p_section->i_extension = 0;
          p_section->i_version = 0;
          p_section->b_current_next = true;
          p_section->i_number = 0;
          p_section->i_last_number = 0;
          p_section->p_payload_start = puVar6 + 3;
          goto LAB_0041a3ec;
        }
        p_section->p_payload_end = puVar7 + -4; // strip 4-byte CRC-32 from payload
LAB_0041a314:
        _Var3 = dvbpsi_ValidPSISection(p_section);
        if (CONCAT31(extraout_var,_Var3) != 0) {
          puVar6 = p_section->p_data;
          cVar8 = p_section->b_syntax_indicator;
          goto LAB_0041a39c;
        }
        _Var3 = dvbpsi_ValidPSISection(p_section);
        if (CONCAT31(extraout_var_00,_Var3) == 0) {
          dvbpsi_error(p_dvbpsi,"misc PSI","Bad CRC_32 table 0x%x !!!",(uint)*p_section->p_data);
        }
        else {
          dvbpsi_error(p_dvbpsi,"misc PSI","table 0x%x",(uint)*p_section->p_data);
        }
        dvbpsi_DeletePSISections(p_section);
      }
      else {
        p_section->p_payload_end = puVar7 + -4; // strip 4-byte CRC-32 from payload
        if (bVar2) goto LAB_0041a314;
        p_section->i_table_id = *puVar6;
        if (_Var3 == false) goto LAB_0041a424;
LAB_0041a3a8:
        // Long section (syntax_indicator == 1): 8-byte extended PSI header
        p_section->i_extension = CONCAT11(puVar6[3],puVar6[4]);
        p_section->i_version = (uint8_t)((puVar6[5] & 0x3f) >> 1);
        p_section->b_current_next = (_Bool)(puVar6[5] & 1);
        p_section->i_number = puVar6[6];
        uVar1 = puVar6[7];
        p_section->p_payload_start = puVar6 + 8;
        p_section->i_last_number = uVar1;
LAB_0041a3ec:
        if (pdVar10->pf_gather != (dvbpsi_callback_gather_t)0x0) {
          (*pdVar10->pf_gather)(p_dvbpsi,p_section);
        }
      }
      pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      if (pbVar12 == (byte *)0x0) {
        if (__n == (uint8_t *)0x0) {
          return true;
        }
        pbVar12 = __src;
        if (*__src == 0xff) { // TS stuffing byte 0xff marks end of sections in packet
          return true;
        }
      }
LAB_0041a218:
      // Start next section in remaining bytes of this TS packet
      __src = pbVar12;
      p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
      pdVar10->p_current_section = p_section;
      if (p_section == (dvbpsi_psi_section_t *)0x0) {
        return false;
      }
      pdVar10->i_need = 3;
      __n = p_data + (0xbc - (int)__src);
      pdVar10->b_complete_header = false;
      pbVar12 = (byte *)0x0;
      if ((int)__n < 1) {
        return true;
      }
    }
    // Just finished 3-byte common PSI header: extract 12-bit section_length
    puVar7 = p_section->p_data;
    pdVar10->b_complete_header = true;
    iVar5 = pdVar10->i_section_max_size;
    uVar4 = (puVar7[1] & 0xf) << 8 | (uint)puVar7[2];
    p_section->i_length = (uint16_t)uVar4;
    pdVar10->i_need = uVar4;
    if (iVar5 + -2 <= (int)uVar4) { // section_length + 3 > i_section_max_size
      dvbpsi_error(p_dvbpsi,"PSI decoder","PSI section too long");
      dvbpsi_DeletePSISections(p_section);
      pdVar10->p_current_section = (dvbpsi_psi_section_t *)0x0;
      if (pbVar12 == (byte *)0x0) {
        return true;
      }
      goto LAB_0041a218;
    }
    if ((int)__n < 1) {
      return true;
    }
  } while( true );
}



/* dvbpsi_warning @ 0x0041a588
 * Purpose  : Format and emit a DVBPSI_MSG_WARN message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_ERROR (0).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_warning @ 0041a588 ====

void dvbpsi_warning(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
  if (DVBPSI_MSG_ERROR < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_WARN,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}



/* dvbpsi_debug @ 0x0041a678
 * Purpose  : Format and emit a DVBPSI_MSG_DEBUG message via dvbpsi->pf_message
 *            if dvbpsi->i_msg_level > DVBPSI_MSG_WARN (1).
 * Params   : dvbpsi - handle, src - component label, fmt... - printf format & args
 * Returns  : void
 * Callees  : vasprintf, asprintf, free, (*dvbpsi->pf_message)
 */
// ==== dvbpsi_debug @ 0041a678 ====

void dvbpsi_debug(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;                 // vasprintf return (length of formatted body)
  int iVar2;                 // asprintf return
  undefined4 in_a3;          // 4th MIPS arg register ($a3) spilled for va_list
  undefined4 local_resc;     // stack home for $a3 / start of va_list
  char *local_1c;            // tmp: formatted message body
  char *local_18 [2];        // msg[0]: "libdvbpsi (%s): %s"
  
  if (DVBPSI_MSG_WARN < dvbpsi->i_msg_level) {
                    /* Unresolved local var: va_list ap@[???]
                       Unresolved local var: char * tmp@[???]
                       Unresolved local var: int err@[???]
                       Unresolved local var: char * msg@[???] */
    local_1c = (char *)0x0;
    local_resc = in_a3;
    iVar1 = vasprintf(&local_1c,fmt,&local_resc);
    if (-1 < iVar1) {
      local_18[0] = (char *)0x0;
      iVar2 = asprintf(local_18,"libdvbpsi (%s): %s",src,local_1c);
      if (iVar2 < 0) {
        free(local_1c);
      }
      else {
        free(local_1c);
        if ((iVar1 != 0) && (dvbpsi->pf_message != (dvbpsi_message_cb)0x0)) {
          (*dvbpsi->pf_message)(dvbpsi,DVBPSI_MSG_DEBUG,local_18[0]);
        }
        free(local_18[0]);
      }
    }
  }
  return;
}




/* #####################################################################
 * FILE: 09_psi.c.c
 * ##################################################################### */

/*
 * 09_psi.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/psi.c
 * ======================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/psi.c
 *                   (DWARF CU @ 0x41a770..0x41aba4, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x API.
 *
 * Role in av_streamer:
 *   Allocates, frees, validates, CRC-32s, and serializes raw PSI/SI section
 *   buffers (dvbpsi_psi_section_t). Called by table generators (NIT, SDT,
 *   EIT, TOT/TDT) via dvbpsi_NewPSISection, dvbpsi_BuildPSISection, and
 *   dvbpsi_DeletePSISections (in 01_libxcode4encoder.c.c and tables/*.c).
 *
 * Main data structures (from DWARF):
 *   dvbpsi_psi_section_t (struct dvbpsi_psi_section_s, 32 bytes = 0x20):
 *     +0x00  uint8_t  i_table_id
 *     +0x01  _Bool    b_syntax_indicator
 *     +0x02  _Bool    b_private_indicator
 *     +0x04  uint16_t i_length
 *     +0x06  uint16_t i_extension        (TS ID / service ID / etc.)
 *     +0x08  uint8_t  i_version          (5-bit version_number)
 *     +0x09  _Bool    b_current_next
 *     +0x0a  uint8_t  i_number           (section_number)
 *     +0x0b  uint8_t  i_last_number      (last_section_number)
 *     +0x0c  uint8_t *p_data             (raw section buffer)
 *     +0x10  uint8_t *p_payload_start    (start of table-specific payload)
 *     +0x14  uint8_t *p_payload_end      (end of payload, before 4-byte CRC32)
 *     +0x18  uint32_t i_crc              (computed MPEG-2 CRC-32)
 *     +0x1c  dvbpsi_psi_section_s *p_next (linked list next pointer)
 *
 * Globals referenced: dvbpsi_crc32_table (rodata, 256-entry MPEG-2 CRC-32 LUT).
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 psi.c.
 */

/* dvbpsi_NewPSISection @ 0x0041a770
 * Purpose  : Allocate a dvbpsi_psi_section_t (0x20 bytes) and its backing
 *            raw data buffer of i_max_size bytes.
 * Params   : i_max_size - maximum section buffer size in bytes (e.g. 1024/4096)
 * Returns  : Newly allocated section with p_payload_end == p_data, or NULL on OOM.
 * Callers  : dvbpsi_packet_push (08_dvbpsi.c.c), table *_sections_generate()
 * Callees  : calloc, free
 */
// ==== dvbpsi_NewPSISection @ 0041a770 ====

dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size)

{
  dvbpsi_psi_section_t *__ptr; // p_section (0x20 bytes)
  uint8_t *puVar1;             // allocated p_data buffer of size i_max_size
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_section@[???] */
  __ptr = calloc(1,0x20);
  if (__ptr != (dvbpsi_psi_section_t *)0x0) {
    puVar1 = calloc(1,i_max_size);
    __ptr->p_data = puVar1;
    if (puVar1 == (uint8_t *)0x0) {
      free(__ptr);
      __ptr = (dvbpsi_psi_section_t *)0x0;
    }
    else {
      __ptr->p_payload_end = puVar1;
      __ptr->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return __ptr;
}



/* dvbpsi_DeletePSISections @ 0x0041a800
 * Purpose  : Walk a linked list of dvbpsi_psi_section_t and free each
 *            section's p_data buffer and struct.
 * Params   : p_section - head of section list (may be NULL)
 * Returns  : void
 * Callers  : genNIT, genSDT, genTDT, genEIT (01_libxcode4encoder.c.c),
 *            dvbpsi_decoder_reset, dvbpsi_decoder_delete, etc.
 * Callees  : free
 */
// ==== dvbpsi_DeletePSISections @ 0041a800 ====

void dvbpsi_DeletePSISections(dvbpsi_psi_section_t *p_section)

{
  dvbpsi_psi_section_s *pdVar1; // p_next
  
  while (p_section != (dvbpsi_psi_section_t *)0x0) {
                    /* Unresolved local var: dvbpsi_psi_section_t * p_next@[???] */
    pdVar1 = p_section->p_next;
    if (p_section->p_data != (uint8_t *)0x0) {
      free(p_section->p_data);
    }
    free(p_section);
    p_section = pdVar1;
  }
  return;
}



/* dvbpsi_CheckPSISection @ 0x0041a86c
 * Purpose  : Validate that a gathered PSI section has the expected table_id
 *            and (except for TOT 0x73) has section_syntax_indicator == 1,
 *            then log section metadata at debug level.
 * Params   : p_dvbpsi       - handle (asserted non-NULL; DAT_004244f0="p_dvbpsi")
 *            p_section      - section to check (asserted non-NULL)
 *            table_id       - expected MPEG/DVB table_id
 *            psz_table_name - table name string for error/debug messages
 * Returns  : true if valid, false on table_id or syntax_indicator mismatch.
 * Callers  : table *_sections_gather() (SDT, EIT, NIT)
 * Callees  : __assert_fail, dvbpsi_error, dvbpsi_debug
 */
// ==== dvbpsi_CheckPSISection @ 0041a86c ====

_Bool dvbpsi_CheckPSISection
                (dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section,uint8_t table_id,
                char *psz_table_name)

{
  uint uVar1;                // p_section->i_table_id
  
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,(char *)((int)&DAT_004244f9 + 3),0xc4,
                  "dvbpsi_CheckPSISection"); // "p_dvbpsi", "psi.c" (inferred)
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section",(char *)((int)&DAT_004244f9 + 3),0xc5,"dvbpsi_CheckPSISection");
  }
  uVar1 = (uint)p_section->i_table_id;
  if (uVar1 != table_id) {
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (table_id == 0x%02x expected 0x%02x)",
                 uVar1,(uint)table_id);
    return false;
  }
  if ((p_section->b_syntax_indicator == false) && (uVar1 != 0x73)) { // 0x73 == DVB TOT
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (section_syntax_indicator == 0)");
    return false;
  }
  dvbpsi_debug(p_dvbpsi,psz_table_name,
               "Table version %2d, i_extension %5d, section %3d up to %3d, current %1d",
               (uint)p_section->i_version,(uint)p_section->i_extension,(uint)p_section->i_number,
               (uint)p_section->i_last_number,(uint)p_section->b_current_next);
  return true;
}



/* dvbpsi_ValidPSISection @ 0x0041a98c
 * Purpose  : Verify the MPEG-2 CRC-32 over [p_data, p_payload_end + 4).
 *            Uses standard MPEG-2 CRC-32 (poly 0x04C11DB7, MSB-first, init
 *            0xFFFFFFFF, no final XOR), so a valid section + CRC sums to 0.
 * Params   : p_section - section whose p_data..p_payload_end+4 holds section+CRC
 * Returns  : true if CRC-32 residue is 0, false otherwise.
 * Callers  : dvbpsi_packet_push (08_dvbpsi.c.c), dvbpsi_BuildPSISection
 */
// ==== dvbpsi_ValidPSISection @ 0041a98c ====

_Bool dvbpsi_ValidPSISection(dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                // current byte
  bool bVar2;                // true if residue == 0
  uint uVar3;                // i_crc running accumulator (init 0xffffffff)
  byte *pbVar4;              // p_byte cursor
  
                    /* Unresolved local var: uint32_t i_crc@[???]
                       Unresolved local var: uint8_t * p_byte@[???] */
  pbVar4 = p_section->p_data;
  bVar2 = false;
  if (pbVar4 < p_section->p_payload_end + 4) {
    uVar3 = 0xffffffff;
    do {
      bVar1 = *pbVar4;
      pbVar4 = pbVar4 + 1;
      uVar3 = dvbpsi_crc32_table[uVar3 >> 0x18 ^ (uint)bVar1] ^ uVar3 << 8;
    } while (pbVar4 < p_section->p_payload_end + 4);
    bVar2 = uVar3 == 0;
  }
  return bVar2;
}



/* dvbpsi_CalculateCRC32 @ 0x0041a9e8
 * Purpose  : Compute the MPEG-2 CRC-32 over [p_data, p_payload_end), store it
 *            in p_section->i_crc, and write it big-endian to p_payload_end[0..3].
 * Params   : p_section - section with populated header/payload up to p_payload_end
 * Returns  : void
 * Callers  : dvbpsi_BuildPSISection
 */
// ==== dvbpsi_CalculateCRC32 @ 0041a9e8 ====

void dvbpsi_CalculateCRC32(dvbpsi_psi_section_t *p_section)

{
  byte *pbVar1;              // p_byte cursor
  byte bVar2;                // current byte, then MSB of final CRC
  uint uVar3;                // running CRC-32 accumulator
  byte *pbVar4;              // p_section->p_payload_end (4-byte CRC destination)
  
                    /* Unresolved local var: uint8_t * p_byte@[???] */
  pbVar1 = p_section->p_data;
  pbVar4 = p_section->p_payload_end;
  p_section->i_crc = 0xffffffff;
  bVar2 = 0xff;
  if (pbVar1 < pbVar4) {
    uVar3 = 0xffffffff;
    do {
      bVar2 = *pbVar1;
      pbVar1 = pbVar1 + 1;
      uVar3 = dvbpsi_crc32_table[uVar3 >> 0x18 ^ (uint)bVar2] ^ uVar3 << 8;
      p_section->i_crc = uVar3;
    } while (pbVar1 < pbVar4);
    bVar2 = (byte)(uVar3 >> 0x18);
  }
  // Write CRC-32 big-endian at p_payload_end[0..3] (note MIPS32-LE +2 offset = bits 23..16)
  *pbVar4 = bVar2;
  p_section->p_payload_end[1] = (uint8_t)*(undefined2 *)((int)&p_section->i_crc + 2);
  p_section->p_payload_end[2] = (uint8_t)(p_section->i_crc >> 8);
  p_section->p_payload_end[3] = (uint8_t)p_section->i_crc;
  return;
}



/* dvbpsi_BuildPSISection @ 0x0041aa74
 * Purpose  : Serialize the PSI section header fields (table_id, syntax/private
 *            indicators, reserved bits 0x30, 12-bit section_length, and for
 *            long sections the 5-byte extension/version/current_next/number/
 *            last_number) into p_section->p_data, then compute and verify the
 *            trailing CRC-32 when required by dvbpsi_has_CRC32().
 * Params   : p_dvbpsi  - handle for error reporting
 *            p_section - section to serialize
 * Returns  : void
 * Callers  : table *_sections_generate() (NIT, SDT, EIT, TOT/TDT)
 * Callees  : dvbpsi_CalculateCRC32, dvbpsi_ValidPSISection, dvbpsi_error
 */
// ==== dvbpsi_BuildPSISection @ 0041aa74 ====

void dvbpsi_BuildPSISection(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint8_t uVar1;             // p_section->i_table_id
  byte bVar2;                // private_indicator bit (0x40 or 0)
  _Bool _Var3;               // result of dvbpsi_ValidPSISection()
  undefined3 extraout_var;   // MIPS ABI upper 24 bits of _Bool return
  byte bVar4;                // section_syntax_indicator bit (0x80 or 0)
  
  bVar4 = 0x80;
  bVar2 = 0x40;
  *p_section->p_data = p_section->i_table_id;
  if (p_section->b_syntax_indicator == false) {
    bVar4 = 0;
  }
  if (p_section->b_private_indicator == false) {
    bVar2 = 0;
  }
  // Byte 1: syntax(1) | private(1) | reserved('11' = 0x30) | section_length[11:8]
  p_section->p_data[1] = bVar2 | bVar4 | (byte)(p_section->i_length >> 8) & 0xf | 0x30;
  p_section->p_data[2] = (uint8_t)p_section->i_length;
  if (p_section->b_syntax_indicator != false) {
    // Bytes 3..7: extended PSI header
    p_section->p_data[3] = (uint8_t)(p_section->i_extension >> 8);
    p_section->p_data[4] = (uint8_t)p_section->i_extension;
    p_section->p_data[5] = p_section->b_current_next | 0xc0U | (p_section->i_version & 0x1f) << 1;
    p_section->p_data[6] = p_section->i_number;
    p_section->p_data[7] = p_section->i_last_number;
  }
  uVar1 = p_section->i_table_id;
  // dvbpsi_has_CRC32(): skip CRC for TDT(0x70), RST(0x71), ST(0x72), DIT(0x7e);
  // include CRC for long sections or TOT(0x73).
  if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
     ((p_section->b_syntax_indicator != false || (uVar1 == 0x73)))) {
    dvbpsi_CalculateCRC32(p_section);
    _Var3 = dvbpsi_ValidPSISection(p_section);
    if (CONCAT31(extraout_var,_Var3) == 0) {
      dvbpsi_error(p_dvbpsi,"misc PSI","********************************************");
      dvbpsi_error(p_dvbpsi,"misc PSI","* Generated PSI section has a bad CRC_32.  *");
      dvbpsi_error(p_dvbpsi,"misc PSI","* THIS IS A BUG, PLEASE REPORT TO THE LIST *");
      dvbpsi_error(p_dvbpsi,"misc PSI","*  ---  libdvbpsi-devel@videolan.org  ---  *");
      dvbpsi_error(p_dvbpsi,"misc PSI","********************************************");
      return;
    }
  }
  return;
}




/* #####################################################################
 * FILE: 10_tables_sdt.c.c
 * ##################################################################### */

/*
 * 10_tables_sdt.c.c  --  annotated decompilation of libdvbpsi tables/sdt.c
 * =========================================================================
 *
 * Original source file: tables/sdt.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_sdt_*` public API
 * (`dvbpsi_sdt_attach`, `dvbpsi_sdt_init`, `dvbpsi_sdt_sections_generate`,
 * `dvbpsi_sdt_service_add`, `dvbpsi_sdt_service_descriptor_add`), and the
 * `dvbpsi_demux_t` sub-decoder attachment model. Note that internal helper
 * `dvbpsi_ReInitSDT` retains the older CamelCase naming convention.
 *
 * Purpose in av_streamer:
 *   Provides DVB Service Description Table (SDT, ETSI EN 300 468, PID 0x0011,
 *   table_id 0x42 actual TS / 0x46 other TS) construction, section
 *   serialization, and section parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genSDT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_sdt_init`, `dvbpsi_sdt_service_add`,
 *   `dvbpsi_sdt_service_descriptor_add`, `dvbpsi_sdt_sections_generate`,
 *   `dvbpsi_sdt_empty`), which emits an SDT packet on PID 0x0011 with
 *   service_id=3 and a DVB service_descriptor (tag 0x48) encoding
 *   "<company>,<service_name>(<sn>,<lat>,<lng>)".
 *   The decoder/demux functions (`dvbpsi_sdt_attach`, `dvbpsi_sdt_detach`,
 *   `dvbpsi_sdt_new`, `dvbpsi_sdt_delete`, `dvbpsi_ReInitSDT`,
 *   `dvbpsi_sdt_sections_decode`, `dvbpsi_sdt_sections_gather`) are linked
 *   in from libdvbpsi but are dead code in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_sdt_t (12 bytes):
 *     +0x00 uint8_t  i_table_id       (0x42 = SDT actual TS, 0x46 = other TS)
 *     +0x02 uint16_t i_extension      (transport_stream_id)
 *     +0x04 uint8_t  i_version        (5-bit version_number)
 *     +0x05 _Bool    b_current_next
 *     +0x06 uint16_t i_network_id     (original_network_id)
 *     +0x08 dvbpsi_sdt_service_t *p_first_service
 *   dvbpsi_sdt_service_t (16 bytes):
 *     +0x00 uint16_t i_service_id
 *     +0x02 _Bool    b_eit_schedule
 *     +0x03 _Bool    b_eit_present
 *     +0x04 uint8_t  i_running_status (3-bit: 4 = running)
 *     +0x05 _Bool    b_free_ca
 *     +0x06 uint16_t i_descriptors_length
 *     +0x08 dvbpsi_descriptor_t *p_first_descriptor
 *     +0x0c dvbpsi_sdt_service_s *p_next
 *   dvbpsi_sdt_decoder_t (52 / 0x34 bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t   (28 bytes = sizeof(dvbpsi_decoder_t))
 *     +0x1c dvbpsi_sdt_callback pf_sdt_callback  (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                   (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_sdt_t current_sdt             (+0x28: {i_version, b_current_next})
 *     +0x30 dvbpsi_sdt_t *p_building_sdt         (Ghidra: p_decoder[1].i_section_max_size)
 *
 * Globals owned: none.
 */

/* dvbpsi_sdt_attach @ 0041ac60
 * Role (upstream libdvbpsi 1.x): Attaches an SDT sub-decoder for (i_table_id,
 *   i_extension) to the demux attached to p_dvbpsi.
 * Parameters:
 *   p_dvbpsi    - libdvbpsi handle (must have a dvbpsi_demux_t attached)
 *   i_table_id  - PSI table ID (typically 0x42 for SDT actual TS)
 *   i_extension - transport_stream_id
 *   pf_callback - callback invoked when a complete SDT is decoded
 *   p_cb_data   - opaque user pointer passed to pf_callback
 * Returns: true on success, false if sub-decoder already exists or alloc fails.
 * Callers/callees: dead code in av_streamer; calls dvbpsi_demuxGetSubDec,
 *   dvbpsi_decoder_new, dvbpsi_NewDemuxSubDecoder, dvbpsi_AttachDemuxSubDecoder,
 *   dvbpsi_decoder_delete, dvbpsi_error.
 * Threading/side effects: allocates 0x34-byte decoder + demux subdec; asserts
 *   on NULL p_dvbpsi or NULL p_dvbpsi->p_decoder. */
_Bool dvbpsi_sdt_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_sdt_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_sdt_decoder_t (0x34 bytes)
  dvbpsi_demux_t *p_demux;           // demuxer cast from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x3b,"dvbpsi_sdt_attach"); // "p_dvbpsi" (inferred)
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x3c,"dvbpsi_sdt_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x34 (52) bytes for dvbpsi_sdt_decoder_t; pf_gather=NULL (subdec handles it)
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x34);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_sdt_detach,dvbpsi_sdt_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_sdt_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_sdt_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_section_max_size = 0;                   // +0x30: p_building_sdt = NULL
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"SDT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



/* dvbpsi_sdt_init @ 0041ae10
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_sdt_t
 *   structure (typically on the stack) before adding services and generating sections.
 * Parameters:
 *   p_sdt          - SDT struct to initialize (asserted non-NULL)
 *   i_table_id     - 0x42 (SDT actual TS) or 0x46 (SDT other TS)
 *   i_extension    - transport_stream_id
 *   i_version      - 5-bit version_number
 *   b_current_next - current_next_indicator
 *   i_network_id   - original_network_id (in av_streamer: 0x2114)
 * Returns: void.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (table_id=0x42, ext=0, ver=0x10,
 *   b_current_next=false [overwritten to true before generate], net_id=0x2114). */
void dvbpsi_sdt_init(dvbpsi_sdt_t *p_sdt,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint16_t i_network_id)

{
  if (p_sdt == (dvbpsi_sdt_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt","tables/sdt.c",0x90,"dvbpsi_sdt_init");
  }
  p_sdt->i_table_id = i_table_id;
  p_sdt->i_version = i_version;
  p_sdt->i_extension = i_extension;
  p_sdt->b_current_next = b_current_next;
  p_sdt->i_network_id = i_network_id;
  p_sdt->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  return;
}



/* dvbpsi_sdt_new @ 0041ae84
 * Role (upstream libdvbpsi 1.x): Heap-allocates (12 bytes) and initializes a
 *   dvbpsi_sdt_t structure.
 * Parameters: same fields as dvbpsi_sdt_init.
 * Returns: newly allocated dvbpsi_sdt_t *, or NULL on malloc failure.
 * Callers: dvbpsi_sdt_sections_gather (via inlined dvbpsi_AddSectionSDT). */
dvbpsi_sdt_t *
dvbpsi_sdt_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_network_id)

{
  dvbpsi_sdt_t *pdVar1;              // newly allocated dvbpsi_sdt_t *
  
                    /* Unresolved local var: dvbpsi_sdt_t * p_sdt@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);              // sizeof(dvbpsi_sdt_t) == 12
  if (pdVar1 != (dvbpsi_sdt_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_network_id = i_network_id;
    pdVar1->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_sdt_empty @ 0041af0c
 * Role (upstream libdvbpsi 1.x): Frees all services in p_sdt->p_first_service
 *   and their attached descriptor lists, resetting p_first_service to NULL.
 *   Does not free p_sdt itself (safe for stack-allocated structs).
 * Parameters: p_sdt - SDT struct to empty.
 * Callers: genSDT() in 01_libxcode4encoder.c.c, and dvbpsi_sdt_delete(). */
void dvbpsi_sdt_empty(dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_sdt_service_t *__ptr;       // current service node being freed (p_service)
  dvbpsi_sdt_service_s *pdVar1;      // saved next pointer (p_tmp)
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  __ptr = p_sdt->p_first_service;
  while (__ptr != (dvbpsi_sdt_service_t *)0x0) {
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_sdt->p_first_service = (dvbpsi_sdt_service_t *)0x0;
  return;
}



/* dvbpsi_sdt_delete @ 0041af80
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_sdt_t.
 * Parameters: p_sdt - SDT struct to free (NULL-safe).
 * Callers: dvbpsi_ReInitSDT, dvbpsi_sdt_detach. */
void dvbpsi_sdt_delete(dvbpsi_sdt_t *p_sdt)

{
  if (p_sdt != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_empty(p_sdt);
  }
  free(p_sdt);
  return;
}



/* dvbpsi_ReInitSDT @ 0041afc4
 * Role (upstream libdvbpsi 1.x internal): Resets the SDT decoder state after a
 *   completed table or on discontinuity/error. Retains CamelCase name from
 *   earlier libdvbpsi generations.
 * Parameters:
 *   p_decoder - SDT decoder instance (asserted non-NULL)
 *   b_force   - if true, also deletes p_building_sdt (not yet handed to callback)
 * Callers: dvbpsi_sdt_sections_gather. */
void dvbpsi_ReInitSDT(dvbpsi_sdt_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;   // upper 24 bits of MIPS $a1 register (b_force)
  
  if (p_decoder == (dvbpsi_sdt_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/sdt.c",0x10a,"dvbpsi_ReInitSDT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_sdt != (dvbpsi_sdt_t *)0x0)) {
    dvbpsi_sdt_delete(p_decoder->p_building_sdt);
  }
  p_decoder->p_building_sdt = (dvbpsi_sdt_t *)0x0;
  return;
}



/* dvbpsi_sdt_detach @ 0041b050
 * Role (upstream libdvbpsi 1.x): Detaches and frees the SDT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Parameters: p_dvbpsi, i_table_id, i_extension.
 * Callers: dead code in av_streamer (registered as pf_detach in dvbpsi_sdt_attach). */
void dvbpsi_sdt_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_sdt_decoder_t *)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x6b,"dvbpsi_sdt_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x6c,"dvbpsi_sdt_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"SDT Decoder",
                 "No such SDT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  if (pdVar1 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec->p_decoder","tables/sdt.c",0x7b,"dvbpsi_sdt_detach");
  }
  // Free any partially built SDT (+0x30 = p_building_sdt)
  if ((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_delete((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size);
  }
  pdVar1[1].i_section_max_size = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_sdt_service_add @ 0041b1c0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_sdt_service_t (16 bytes)
 *   and appends it to the end of p_sdt->p_first_service.
 * Parameters:
 *   p_sdt            - target SDT struct
 *   i_service_id     - 16-bit MPEG/DVB service_id (program_number)
 *   b_eit_schedule   - EIT_schedule_flag
 *   b_eit_present    - EIT_present_following_flag
 *   i_running_status - 3-bit running_status (e.g. 4 = running)
 *   b_free_ca        - free_CA_mode (true = scrambled)
 * Returns: pointer to the new service node, or NULL on calloc failure.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (service_id=3, running_status=4),
 *   and dvbpsi_sdt_sections_decode(). */
dvbpsi_sdt_service_t *
dvbpsi_sdt_service_add
          (dvbpsi_sdt_t *p_sdt,uint16_t i_service_id,_Bool b_eit_schedule,_Bool b_eit_present,
          uint8_t i_running_status,_Bool b_free_ca)

{
  dvbpsi_sdt_service_t *pdVar1;      // newly allocated service node (p_service)
  dvbpsi_sdt_service_t *pdVar2;      // list traversal cursor (p_last_service)
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);           // sizeof(dvbpsi_sdt_service_t) == 16
  if (pdVar1 != (dvbpsi_sdt_service_t *)0x0) {
    pdVar2 = p_sdt->p_first_service;
    pdVar1->i_service_id = i_service_id;
    pdVar1->b_eit_schedule = b_eit_schedule;
    pdVar1->b_eit_present = b_eit_present;
    pdVar1->i_running_status = i_running_status;
    pdVar1->b_free_ca = b_free_ca;
    pdVar1->p_next = (dvbpsi_sdt_service_s *)0x0;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    if (pdVar2 == (dvbpsi_sdt_service_t *)0x0) {
      p_sdt->p_first_service = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_sdt_service_s *)0x0) {
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_last_service@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_sdt_service_descriptor_add @ 0041b288
 * Role (upstream libdvbpsi 1.x): Creates a descriptor via dvbpsi_NewDescriptor
 *   and appends it to p_service->p_first_descriptor.
 * Parameters: p_service, i_tag (e.g. 0x48 service_descriptor), i_length, p_data.
 * Returns: newly allocated dvbpsi_descriptor_t *, or NULL on allocation failure.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (tag 0x48), and
 *   dvbpsi_sdt_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_sdt_service_descriptor_add
          (dvbpsi_sdt_service_t *p_service,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_service->p_first_descriptor,p_descriptor);
    p_service->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_service->p_first_descriptor","tables/sdt.c",0x100,
                    "dvbpsi_sdt_service_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_sdt_sections_decode @ 0041b320
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_sdt with services and their descriptors.
 * Parameters: p_sdt - target SDT struct; p_section - linked list of PSI sections.
 * SDT section payload layout (after 8-byte PSI header):
 *   +0..+1: original_network_id (16 bits)
 *   +2:     reserved_future_use (0xff)
 *   +3..:   loop of service entries, each 5 bytes + descriptors_loop_length:
 *             +0..+1: service_id
 *             +2:     reserved_future_use[7:2], EIT_schedule_flag[1], EIT_p/f_flag[0]
 *             +3..+4: running_status[15:13], free_CA_mode[12], descriptors_loop_length[11:0]
 * Callers: dvbpsi_sdt_sections_gather (dead code in av_streamer). */
void dvbpsi_sdt_sections_decode(dvbpsi_sdt_t *p_sdt,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (byte 3 of service entry / desc length)
  byte bVar2;                        // byte 4 of service entry (low 8 bits of descriptors_loop_length)
  uint8_t *puVar3;                   // lookahead pointer (p_byte + 4 or p_byte + 2) for bounds check
  dvbpsi_sdt_service_t *p_service;   // newly added service entry
  uint8_t *puVar4;                   // p_section->p_payload_end
  uint8_t *p_data;                   // descriptor payload pointer (p_byte + 2)
  uint8_t *puVar5;                   // current byte cursor (p_byte)
  uint8_t *puVar6;                   // start of current service entry
  uint8_t *puVar7;                   // end of current service's descriptor loop
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar4 = p_section->p_payload_end;
    puVar5 = p_section->p_payload_start + 3; // skip original_network_id (2B) + reserved (1B)
    puVar3 = p_section->p_payload_start + 7; // need at least 5 bytes (puVar5 + 4 < p_payload_end)
    while (puVar6 = puVar5, puVar3 < puVar4) {
      while( true ) {
                    /* Unresolved local var: uint16_t i_service_id@[???]
                       Unresolved local var: _Bool b_eit_schedule@[???]
                       Unresolved local var: _Bool b_eit_present@[???]
                       Unresolved local var: uint8_t i_running_status@[???]
                       Unresolved local var: _Bool b_free_ca@[???]
                       Unresolved local var: uint16_t i_srv_length@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
        bVar1 = puVar6[3];
        bVar2 = puVar6[4];
        p_service = dvbpsi_sdt_service_add
                              (p_sdt,CONCAT11(*puVar6,puVar6[1]),SUB41((puVar6[2] & 3) >> 1,0),
                               (_Bool)(puVar6[2] & 1),bVar1 >> 5,SUB41((bVar1 & 0x1f) >> 4,0));
        puVar4 = p_section->p_payload_end;
        puVar5 = puVar6 + 5;
        puVar7 = puVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2); // end of service descriptors
        if (puVar4 < puVar7) goto LAB_0041b418;
        p_data = puVar6 + 7;
        puVar3 = puVar6 + 9;
        if (puVar7 < p_data) break;
        do {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
          bVar1 = puVar5[1];
          if ((int)(bVar1 + 1) < (int)puVar7 - (int)puVar5) {
            dvbpsi_sdt_service_descriptor_add(p_service,*puVar5,bVar1,p_data);
          }
          puVar5 = puVar5 + bVar1 + 2;
          p_data = puVar5 + 2;
        } while (p_data <= puVar7);
        puVar6 = puVar5;
        if (p_section->p_payload_end <= puVar5 + 4) goto LAB_0041b418;
      }
    }
LAB_0041b418:
    p_section = p_section->p_next;
  } while( true );
}



/* dvbpsi_sdt_sections_gather @ 0041b45c
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for SDT
 *   sections. Validates table_id == 0x42 (note: SDT other TS 0x46 is rejected),
 *   checks TS discontinuity / extension / version / last_section_number
 *   (inlined dvbpsi_CheckSDT), buffers sections until complete (inlined
 *   dvbpsi_AddSectionSDT), decodes them, and invokes pf_sdt_callback.
 * Parameters: p_dvbpsi, p_private_decoder (dvbpsi_sdt_decoder_t *), p_section.
 * Callers: registered in dvbpsi_sdt_attach (dead code in av_streamer). */
void dvbpsi_sdt_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var2;                       // boolean return from PSI helper calls
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  int iVar3;                         // scratch: p_building_sdt or pf_sdt_callback
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  undefined3 extraout_var_01;        // MIPS ABI upper 3 bytes
  dvbpsi_psi_section_t *pdVar4;      // copy of current_sdt bytes +0x04..+0x07
  dvbpsi_sdt_t *pdVar5;              // p_building_sdt (dvbpsi_sdt_t *)
  undefined4 uVar6;                  // p_sdt_decoder->p_cb_data (+0x20)
  dvbpsi_psi_section_t *p_section_00; // p_private_decoder->p_sections
  dvbpsi_psi_section_t *pdVar7;      // copy of current_sdt bytes +0x00..+0x03
  dvbpsi_decoder_t *pdVar8;          // p_demux (p_dvbpsi->p_decoder)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x161,"dvbpsi_sdt_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x162,"dvbpsi_sdt_sections_gather");
  }
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x42,"SDT decoder"); // expects table_id 0x42
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041b694;
  pdVar8 = p_dvbpsi->p_decoder;
  if (pdVar8->b_discontinuity == false) {
    iVar3 = p_private_decoder[1].i_section_max_size; // +0x30: p_building_sdt
    if (iVar3 == 0) {
      // No table in progress; ignore if version+current_next match current_sdt (+0x28)
      if ((p_private_decoder->b_current_valid != false) &&
         (sVar1._0_1_ = p_section->i_version, sVar1._1_1_ = p_section->b_current_next,
         *(short *)&p_private_decoder[1].p_sections == sVar1)) {
        dvbpsi_debug(p_dvbpsi,"SDT decoder","ignoring already decoded section %d",
                     (uint)p_section->i_number);
        goto LAB_0041b694;
      }
    }
    else {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      // Verify consistency with p_building_sdt (+0x02 i_extension, +0x04 i_version)
      if (*(uint16_t *)(iVar3 + 2) == p_section->i_extension) {
        if (*(uint8_t *)(iVar3 + 4) == p_section->i_version) {
          if (p_private_decoder->i_last_section_number == p_section->i_last_number)
          goto LAB_0041b50c;
          dvbpsi_error(p_dvbpsi,"SDT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          dvbpsi_error(p_dvbpsi,"SDT decoder",
                       "\'version_number\' differs whereas no discontinuity has occured");
        }
      }
      else {
        dvbpsi_error(p_dvbpsi,"SDT decoder",
                     "\'transport_stream_id\' differs whereas no TS discontinuity has occured");
      }
      dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,true);
    }
  }
  else {
    dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,true);
    p_private_decoder->b_discontinuity = false;
    pdVar8->b_discontinuity = false;
  }
LAB_0041b50c:
  if (p_private_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt_decoder","tables/sdt.c",0x13f,"dvbpsi_AddSectionSDT");
  }
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_section","tables/sdt.c",0x140,"dvbpsi_AddSectionSDT");
  }
  if (p_private_decoder[1].i_section_max_size == 0) {
    // Allocate p_building_sdt with original_network_id from p_payload_start[0..1]
    pdVar5 = dvbpsi_sdt_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                            p_section->b_current_next,
                            CONCAT11(*p_section->p_payload_start,p_section->p_payload_start[1]));
    p_private_decoder[1].i_section_max_size = (int)pdVar5;
    if (pdVar5 == (dvbpsi_sdt_t *)0x0) {
      dvbpsi_error(p_dvbpsi,"SDT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041b694:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    p_private_decoder->i_last_section_number = p_section->i_last_number;
  }
  _Var2 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var2) != 0) {
    dvbpsi_debug(p_dvbpsi,"SDT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var2 = dvbpsi_decoder_psi_sections_completed(p_private_decoder);
  if (CONCAT31(extraout_var_01,_Var2) == 0) {
    return;
  }
  iVar3._0_1_ = p_private_decoder[1].i_magic[0];
  iVar3._1_1_ = p_private_decoder[1].i_magic[1];
  iVar3._2_1_ = p_private_decoder[1].i_magic[2];
  iVar3._3_1_ = p_private_decoder[1].b_complete_header;
  if (iVar3 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_sdt_decoder->pf_sdt_callback","tables/sdt.c",0x19a,"dvbpsi_sdt_sections_gather"
                 );
  }
  // Copy *p_building_sdt into p_sdt_decoder->current_sdt (+0x24..+0x2f)
  pdVar5 = (dvbpsi_sdt_t *)p_private_decoder[1].i_section_max_size;
  p_section_00 = p_private_decoder->p_sections;
  pdVar7 = *(dvbpsi_psi_section_t **)pdVar5;
  pdVar4 = *(dvbpsi_psi_section_t **)&pdVar5->i_version;
  p_private_decoder[1].pf_gather = (dvbpsi_callback_gather_t)pdVar5->p_first_service;
  p_private_decoder[1].p_current_section = pdVar7;
  p_private_decoder[1].p_sections = pdVar4;
  p_private_decoder->b_current_valid = true;
  dvbpsi_sdt_sections_decode(pdVar5,p_section_00);
  dvbpsi_DeletePSISections(p_private_decoder->p_sections);
  uVar6._0_1_ = p_private_decoder[1].b_discontinuity;
  uVar6._1_1_ = p_private_decoder[1].b_current_valid;
  uVar6._2_1_ = p_private_decoder[1].i_continuity_counter;
  uVar6._3_1_ = p_private_decoder[1].i_last_section_number;
  iVar3 = p_private_decoder[1].i_section_max_size;
  p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_private_decoder + 1))(uVar6,iVar3); // pf_sdt_callback(p_cb_data, p_building_sdt)
  dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,false);
  return;
}



/* dvbpsi_sdt_sections_generate @ 0041b7f4
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_sdt_t and its service/
 *   descriptor lists into a linked list of 1024-byte max PSI sections
 *   (dvbpsi_NewPSISection(0x400)) and finalizes CRC32 via dvbpsi_BuildPSISection.
 * Parameters: p_dvbpsi - libdvbpsi handle; p_sdt - SDT struct to serialize.
 * Returns: head of linked dvbpsi_psi_section_t * list.
 * Quirk: hardcodes i_table_id = 0x42 (ignoring p_sdt->i_table_id) and
 *   b_private_indicator = true. Max section size = 1024 (0x400), max data
 *   offset before CRC = 0x3fb (1019; + 4B CRC = 1023 <= 1024).
 * Callers: genSDT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t * dvbpsi_sdt_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_psi_section_s **ppdVar1;    // &p_section->p_next for finalization loop
  _Bool _Var2;                       // scratch b_current_next
  uint16_t uVar3;                    // scratch i_extension / i_network_id
  uint8_t uVar4;                     // i_last_number (copied from final section)
  dvbpsi_psi_section_t *pdVar5;      // head of section list (p_result)
  dvbpsi_descriptor_t *pdVar6;       // descriptor list cursor
  uint8_t *puVar7;                   // write cursor in current section
  dvbpsi_psi_section_t *pdVar8;      // current section being populated (p_current)
  uint uVar9;                        // accumulated byte size of next service + descriptors
  uint8_t *puVar10;                  // projected end offset for overflow check (< 0x3fb)
  byte bVar11;                       // scratch byte (flags / descriptor length)
  int iVar12;                        // current byte count in section (p_payload_end - p_data)
  dvbpsi_psi_section_t *p_section;   // section cursor in finalization loop
  dvbpsi_psi_section_t *pdVar13;     // previous/current section pointer (p_prev)
  dvbpsi_sdt_service_t *pdVar14;     // service list cursor (p_service)
  uint8_t *puVar15;                  // start of current service entry in section buffer
  byte *pbVar16;                     // pointer to byte 3 of service entry (descriptors_loop_length hi)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar14 = p_sdt->p_first_service;
  pdVar5->i_table_id = 0x42;         // hardcoded SDT actual TS (0x42)
  uVar3 = p_sdt->i_extension;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = true;
  pdVar5->i_length = 0xc;            // 5B after section_length + 3B SDT hdr + 4B CRC = 12
  pdVar5->i_extension = uVar3;
  pdVar5->i_version = p_sdt->i_version;
  _Var2 = p_sdt->b_current_next;
  pdVar5->i_number = 0;
  uVar3 = p_sdt->i_network_id;
  pdVar5->b_current_next = _Var2;
  pdVar5->p_payload_end = pdVar5->p_payload_end + 0xb; // 8B PSI hdr + 3B SDT hdr = 11
  pdVar5->p_payload_start = pdVar5->p_data + 8;
  pdVar5->p_data[8] = (uint8_t)(uVar3 >> 8);
  pdVar5->p_data[9] = (uint8_t)p_sdt->i_network_id;
  pdVar5->p_data[10] = 0xff;         // reserved_future_use
  pdVar13 = pdVar5;
joined_r0x0041b8c0:
  if (pdVar14 == (dvbpsi_sdt_service_t *)0x0) {
    p_section = pdVar5;
    pdVar8 = pdVar13;
    do {
      uVar4 = pdVar8->i_number;
LAB_0041ba64:
      p_section->i_last_number = uVar4;
      dvbpsi_BuildPSISection(p_dvbpsi,p_section);
      ppdVar1 = &p_section->p_next;
      p_section = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_t *)0x0);
    return pdVar5;
  }
  do {
    pdVar6 = pdVar14->p_first_descriptor;
    puVar15 = pdVar13->p_payload_end;
    pdVar8 = pdVar13;
    if (pdVar6 != (dvbpsi_descriptor_t *)0x0) {
      iVar12 = (int)puVar15 - (int)pdVar13->p_data;
      uVar9 = 5;                     // 5-byte service header
      if (iVar12 < 0x3f8) {
        bVar11 = pdVar6->i_length;
        while( true ) {
          pdVar6 = pdVar6->p_next;
          uVar9 = uVar9 + bVar11 + 2 & 0xffff;
          if (pdVar6 == (dvbpsi_descriptor_s *)0x0) break;
          if (0x3fc < (int)(uVar9 + iVar12)) {
            if ((iVar12 != 0xb) && (uVar9 < 0x3f2)) goto LAB_0041bb20;
            break;
          }
          bVar11 = pdVar6->i_length;
        }
      }
      else {
LAB_0041bb20:
        dvbpsi_debug(p_dvbpsi,"SDT generator",
                     "create a new section to carry more Service descriptors");
        pdVar8 = dvbpsi_NewPSISection(0x400);
        pdVar8->i_table_id = 0x42;
        uVar3 = p_sdt->i_extension;
        pdVar8->b_syntax_indicator = true;
        pdVar8->b_private_indicator = true;
        pdVar13->p_next = pdVar8;
        pdVar8->i_extension = uVar3;
        pdVar8->i_length = 0xc;
        uVar4 = pdVar13->i_number;
        pdVar8->i_version = p_sdt->i_version;
        _Var2 = p_sdt->b_current_next;
        pdVar8->i_number = uVar4 + 1;
        uVar3 = p_sdt->i_network_id;
        pdVar8->p_payload_end = pdVar8->p_payload_end + 0xb;
        pdVar8->b_current_next = _Var2;
        pdVar8->p_payload_start = pdVar8->p_data + 8;
        pdVar8->p_data[8] = (uint8_t)(uVar3 >> 8);
        pdVar8->p_data[9] = (uint8_t)p_sdt->i_network_id;
        pdVar8->p_data[10] = 0xff;
        puVar15 = pdVar8->p_payload_end;
      }
    }
    bVar11 = 0xfc;                   // reserved_future_use[7:2] = 0b111111
    pbVar16 = puVar15 + 3;
    *puVar15 = (uint8_t)(pdVar14->i_service_id >> 8);
    puVar15[1] = (uint8_t)pdVar14->i_service_id;
    if (pdVar14->b_eit_schedule != false) {
      bVar11 = 0xfe;                 // set EIT_schedule_flag (bit 1)
    }
    puVar15[2] = bVar11 | pdVar14->b_eit_present; // bit 0 = EIT_present_following_flag
    puVar15[3] = (pdVar14->b_free_ca & 1U) << 4 | pdVar14->i_running_status << 5;
    pdVar6 = pdVar14->p_first_descriptor;
    puVar7 = pdVar8->p_payload_end + 5;
    pdVar8->p_payload_end = puVar7;
    pdVar8->i_length = pdVar8->i_length + 5;
    pdVar13 = pdVar8;
    if (pdVar6 == (dvbpsi_descriptor_t *)0x0) break;
    puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    while ((int)puVar10 < 0x3fb) {   // ensure room for tag+len+data + 4B CRC within 1024B
      *puVar7 = pdVar6->i_tag;
      pdVar8->p_payload_end[1] = pdVar6->i_length;
      memcpy(pdVar8->p_payload_end + 2,pdVar6->p_data,(size_t)pdVar6->i_length);
      puVar7 = pdVar8->p_payload_end + pdVar6->i_length + 2;
      pdVar8->p_payload_end = puVar7;
      puVar10 = &pdVar6->i_length;
      pdVar6 = pdVar6->p_next;
      pdVar8->i_length = pdVar8->i_length + 2 + (ushort)*puVar10;
      if (pdVar6 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041ba30;
      puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    }
    dvbpsi_error(p_dvbpsi,"SDT generator","unable to carry all the descriptors");
    puVar7 = pdVar8->p_payload_end;
    *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
    puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
    pdVar14 = pdVar14->p_next;
    if (pdVar14 == (dvbpsi_sdt_service_s *)0x0) {
      uVar4 = pdVar8->i_number;
      p_section = pdVar5;
      goto LAB_0041ba64;
    }
  } while( true );
LAB_0041ba30:
  // Backfill 12-bit descriptors_loop_length in puVar15[3..4]
  *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
  puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
  pdVar14 = pdVar14->p_next;
  goto joined_r0x0041b8c0;
}


/* #####################################################################
 * FILE: 11_tables_eit.c.c
 * ##################################################################### */

/*
 * 11_tables_eit.c.c  --  annotated decompilation of libdvbpsi tables/eit.c
 * =========================================================================
 *
 * Original source file: tables/eit.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_eit_*` public API
 * (`dvbpsi_eit_attach`, `dvbpsi_eit_init`, `dvbpsi_eit_sections_generate`,
 * `dvbpsi_eit_event_add`, `dvbpsi_eit_nvod_event_add`,
 * `dvbpsi_eit_event_descriptor_add`), and `dvbpsi_demux_t` sub-decoder model.
 * Internal helper `dvbpsi_ReInitEIT` retains the older CamelCase naming, and
 * `NewEITSection` @ 0041bbe0 is a non-static helper not present as a public
 * symbol in upstream libdvbpsi 1.x (likely added or exposed in this fork).
 *
 * Purpose in av_streamer:
 *   Provides DVB Event Information Table (EIT, ETSI EN 300 468, PID 0x0012,
 *   table_id 0x4e..0x6f) construction, section serialization, and section
 *   parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genEIT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_eit_init`, `dvbpsi_eit_event_add`,
 *   `dvbpsi_eit_event_descriptor_add`, `dvbpsi_eit_sections_generate`,
 *   `dvbpsi_eit_empty`), which emits a present/following EIT on PID 0x0012
 *   (table_id=0x4e, service_id=3, ts_id=0, orig_network_id=0x2114) with a
 *   single event starting at current UTC/MJD time, duration 0x05dc (1500 BCD?),
 *   and descriptors: extended_event (0x4e), content (0x54), short_event (0x4d,
 *   "test event-name" / "experiment").
 *   The decoder/demux functions (`dvbpsi_eit_attach`, `dvbpsi_eit_detach`,
 *   `dvbpsi_eit_new`, `dvbpsi_eit_delete`, `dvbpsi_ReInitEIT`,
 *   `dvbpsi_eit_nvod_event_add`, `dvbpsi_eit_sections_decode`,
 *   `dvbpsi_eit_sections_gather`) are dead code in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_eit_t (16 bytes):
 *     +0x00 uint8_t  i_table_id                    (0x4e..0x6f)
 *     +0x02 uint16_t i_extension                   (service_id)
 *     +0x04 uint8_t  i_version                     (5-bit version_number)
 *     +0x05 _Bool    b_current_next
 *     +0x06 uint16_t i_ts_id                       (transport_stream_id)
 *     +0x08 uint16_t i_network_id                  (original_network_id)
 *     +0x0a uint8_t  i_segment_last_section_number
 *     +0x0b uint8_t  i_last_table_id
 *     +0x0c dvbpsi_eit_event_t *p_first_event
 *   dvbpsi_eit_event_t (40 / 0x28 bytes):
 *     +0x00 uint16_t i_event_id
 *     +0x08 uint64_t i_start_time                  (40-bit MJD + UTC BCD)
 *     +0x10 uint32_t i_duration                    (24-bit HHMMSS BCD)
 *     +0x14 uint8_t  i_running_status
 *     +0x15 _Bool    b_free_ca
 *     +0x16 _Bool    b_nvod                        (true if start_time==0xfffff000 && status==0)
 *     +0x18 uint16_t i_descriptors_length
 *     +0x1c dvbpsi_descriptor_t *p_first_descriptor
 *     +0x20 dvbpsi_eit_event_s *p_next
 *   dvbpsi_eit_decoder_t (60 / 0x3c bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t                (28 bytes)
 *     +0x1c dvbpsi_eit_callback pf_eit_callback    (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                     (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_eit_t current_eit               (+0x28: {i_version, b_current_next})
 *     +0x34 dvbpsi_eit_t *p_building_eit           (Ghidra: p_decoder[1].i_need)
 *     +0x38 uint8_t  i_first_received_section_number (Ghidra: p_decoder[2].i_magic[0])
 *
 * Globals owned: none.
 */

/* NewEITSection @ 0041bbe0
 * Role (non-upstream / fork helper): Allocates a 4094-byte (0xffe) PSI section
 *   and initializes the 8-byte PSI header plus the 6-byte EIT-specific header
 *   (transport_stream_id, original_network_id, last_table_id; note that
 *   p_data[0xc] segment_last_section_number is filled in later by the caller).
 * Parameters:
 *   p_eit            - source EIT metadata
 *   i_table_id       - table_id (0x4e..0x6f)
 *   i_section_number - section_number (0..255)
 * Returns: newly allocated dvbpsi_psi_section_t * (NULL not checked!).
 * Callers: dvbpsi_eit_sections_generate. */
dvbpsi_psi_section_t * NewEITSection(dvbpsi_eit_t *p_eit,int i_table_id,int i_section_number)

{
  _Bool _Var1;                       // scratch p_eit->b_current_next
  uint16_t uVar2;                    // scratch i_extension / i_ts_id
  dvbpsi_psi_section_t *pdVar3;      // allocated PSI section (p_result)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[DW_OP_reg2(v0)] */
  pdVar3 = dvbpsi_NewPSISection(0xffe); // EIT max section size = 4096 (0xffe = 4094)
  pdVar3->i_table_id = (uint8_t)i_table_id;
  uVar2 = p_eit->i_extension;
  pdVar3->b_syntax_indicator = true;
  pdVar3->b_private_indicator = true;
  pdVar3->i_length = 0xf;            // 5B PSI ext + 6B EIT hdr + 4B CRC = 15 (0xf)
  pdVar3->i_extension = uVar2;
  pdVar3->i_version = p_eit->i_version;
  _Var1 = p_eit->b_current_next;
  pdVar3->i_number = (uint8_t)i_section_number;
  uVar2 = p_eit->i_ts_id;
  pdVar3->b_current_next = _Var1;
  pdVar3->p_payload_end = pdVar3->p_payload_end + 0xe; // 8B PSI hdr + 6B EIT hdr = 14 (0xe)
  pdVar3->p_payload_start = pdVar3->p_data + 8;
  pdVar3->p_data[8] = (uint8_t)(uVar2 >> 8);           // transport_stream_id[15:8]
  pdVar3->p_data[9] = (uint8_t)p_eit->i_ts_id;         // transport_stream_id[7:0]
  pdVar3->p_data[10] = (uint8_t)(p_eit->i_network_id >> 8); // original_network_id[15:8]
  pdVar3->p_data[0xb] = (uint8_t)p_eit->i_network_id;       // original_network_id[7:0]
  pdVar3->p_data[0xd] = p_eit->i_last_table_id;        // last_table_id (p_data[0xc] set later)
  return pdVar3;
}



/* dvbpsi_eit_attach @ 0041bcb4
 * Role (upstream libdvbpsi 1.x): Attaches an EIT sub-decoder for (i_table_id,
 *   i_extension) to p_dvbpsi's demuxer.
 * Parameters: p_dvbpsi, i_table_id (0x4e..0x6f), i_extension (service_id),
 *   pf_callback, p_cb_data.
 * Returns: true on success, false if already attached or allocation fails.
 * Callers: dead code in av_streamer. */
_Bool dvbpsi_eit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_eit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_eit_decoder_t (0x3c bytes)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x3c,"dvbpsi_eit_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x3d,"dvbpsi_eit_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x3c (60) bytes for dvbpsi_eit_decoder_t
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_eit_detach,dvbpsi_eit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_eit_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_eit_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_need = 0;                               // +0x34: p_building_eit = NULL
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"EIT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



/* dvbpsi_eit_init @ 0041be64
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_eit_t.
 * Parameters: p_eit, i_table_id (0x4e), i_extension (service_id), i_version,
 *   b_current_next, i_ts_id, i_network_id, i_segment_last_section_number,
 *   i_last_table_id.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (0x4e, ext=3, ver=0x10,
 *   b_current_next=false, ts_id=0, net_id=0x2114, seg_last=0, last_tid=0). */
void dvbpsi_eit_init(dvbpsi_eit_t *p_eit,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint16_t i_ts_id,uint16_t i_network_id,
                    uint8_t i_segment_last_section_number,uint8_t i_last_table_id)

{
  p_eit->i_table_id = i_table_id;
  p_eit->i_extension = i_extension;
  p_eit->i_version = i_version;
  p_eit->b_current_next = b_current_next;
  p_eit->i_ts_id = i_ts_id;
  p_eit->i_network_id = i_network_id;
  p_eit->i_segment_last_section_number = i_segment_last_section_number;
  p_eit->i_last_table_id = i_last_table_id;
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



/* dvbpsi_eit_new @ 0041bea0
 * Role (upstream libdvbpsi 1.x): Heap-allocates (16 bytes) and initializes a
 *   dvbpsi_eit_t structure.
 * Returns: newly allocated dvbpsi_eit_t *, or NULL on malloc failure.
 * Callers: dvbpsi_eit_sections_gather (via inlined dvbpsi_AddSectionEIT). */
dvbpsi_eit_t *
dvbpsi_eit_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_ts_id,uint16_t i_network_id,uint8_t i_segment_last_section_number,
              uint8_t i_last_table_id)

{
  dvbpsi_eit_t *pdVar1;              // newly allocated dvbpsi_eit_t *
  
                    /* Unresolved local var: dvbpsi_eit_t * p_eit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);             // sizeof(dvbpsi_eit_t) == 16
  if (pdVar1 != (dvbpsi_eit_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_ts_id = i_ts_id;
    pdVar1->i_network_id = i_network_id;
    pdVar1->i_segment_last_section_number = i_segment_last_section_number;
    pdVar1->i_last_table_id = i_last_table_id;
    pdVar1->p_first_event = (dvbpsi_eit_event_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_eit_empty @ 0041bf58
 * Role (upstream libdvbpsi 1.x): Frees all events in p_eit->p_first_event and
 *   their descriptor lists, resetting p_first_event to NULL.
 * Callers: genEIT() in 01_libxcode4encoder.c.c, and dvbpsi_eit_delete(). */
void dvbpsi_eit_empty(dvbpsi_eit_t *p_eit)

{
  dvbpsi_eit_event_t *__ptr;         // current event node being freed (p_event)
  dvbpsi_eit_event_s *pdVar1;        // saved next pointer (p_tmp)
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  __ptr = p_eit->p_first_event;
  while (__ptr != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



/* dvbpsi_eit_delete @ 0041bfcc
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_eit_t.
 * Callers: dvbpsi_ReInitEIT, dvbpsi_eit_detach. */
void dvbpsi_eit_delete(dvbpsi_eit_t *p_eit)

{
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_empty(p_eit);
  }
  free(p_eit);
  return;
}



/* dvbpsi_eit_detach @ 0041c010
 * Role (upstream libdvbpsi 1.x): Detaches and frees the EIT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Quirk: unlike dvbpsi_sdt_detach, does not assert p_subdec->p_decoder != NULL.
 * Callers: dead code in av_streamer. */
void dvbpsi_eit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_eit_t *p_eit;               // p_eit_decoder->p_building_eit (+0x34)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_eit_decoder_t *)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x6d,"dvbpsi_eit_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x6e,"dvbpsi_eit_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"EIT Decoder",
                 "No such EIT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  p_eit = (dvbpsi_eit_t *)pdVar1[1].i_need; // +0x34: p_building_eit
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_eit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_ReInitEIT @ 0041c158
 * Role (upstream libdvbpsi 1.x internal): Resets the EIT decoder state.
 * Quirk: ignores its `b_force` parameter — unconditionally passes `true` to
 *   dvbpsi_decoder_reset() and unconditionally deletes p_building_eit (unlike
 *   dvbpsi_ReInitSDT and dvbpsi_ReInitNIT which guard on b_force).
 * Callers: dvbpsi_eit_sections_gather. */
void dvbpsi_ReInitEIT(dvbpsi_eit_decoder_t *p_decoder,_Bool b_force)

{
  if (p_decoder == (dvbpsi_eit_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/eit.c",0x119,"dvbpsi_ReInitEIT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,true);
  if (p_decoder->p_building_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_decoder->p_building_eit);
  }
  p_decoder->p_building_eit = (dvbpsi_eit_t *)0x0;
  return;
}



/* dvbpsi_eit_event_add @ 0041c1d0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_eit_event_t (40 bytes)
 *   and appends it to p_eit->p_first_event. Sets b_nvod if the lower 32 bits
 *   of i_start_time have 0xfffff000 set and i_running_status == 0 (NVOD
 *   reference events have undefined start_time = 0xffffffffff).
 * Parameters: p_eit, i_event_id, i_start_time (40-bit MJD+UTC), i_duration
 *   (24-bit BCD), i_running_status, b_free_ca, i_event_descriptor_length.
 * Returns: newly allocated event pointer, or NULL on calloc failure.
 * Callers: genEIT() in 01_libxcode4encoder.c.c, dvbpsi_eit_nvod_event_add(),
 *   and dvbpsi_eit_sections_decode(). */
dvbpsi_eit_event_t *
dvbpsi_eit_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint64_t i_start_time,uint32_t i_duration,
          uint8_t i_running_status,_Bool b_free_ca,uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;        // newly allocated event node (p_event)
  dvbpsi_eit_event_t *pdVar2;        // list traversal cursor (p_last_event)
  uint local_20;                     // low 32 bits of i_start_time for NVOD check
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x28);           // sizeof(dvbpsi_eit_event_t) == 40
  local_20 = (uint)i_start_time;
  if (pdVar1 != (dvbpsi_eit_event_t *)0x0) {
    pdVar2 = p_eit->p_first_event;
    pdVar1->i_event_id = i_event_id;
    pdVar1->i_start_time = i_start_time;
    pdVar1->i_duration = i_duration;
    pdVar1->i_running_status = i_running_status;
    pdVar1->b_free_ca = b_free_ca;
    pdVar1->b_nvod = (local_20 & 0xfffff000) == 0xfffff000 && i_running_status == 0;
    pdVar1->p_next = (dvbpsi_eit_event_s *)0x0;
    pdVar1->i_descriptors_length = i_event_descriptor_length;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    if (pdVar2 == (dvbpsi_eit_event_t *)0x0) {
      p_eit->p_first_event = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_eit_event_s *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_last_event@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_eit_nvod_event_add @ 0041c2c0
 * Role (upstream libdvbpsi 1.x): Convenience wrapper around
 *   dvbpsi_eit_event_add for Near Video On Demand (NVOD) reference events
 *   (passes start_time = 0xfffff000, running_status = 0).
 * Callers: dead code in av_streamer. */
dvbpsi_eit_event_t *
dvbpsi_eit_nvod_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint32_t i_duration,_Bool b_free_ca,
          uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;        // newly added NVOD event node
  
  pdVar1 = dvbpsi_eit_event_add
                     (p_eit,i_event_id,0xfffff000,i_duration,0,b_free_ca,i_event_descriptor_length);
  return pdVar1;
}



/* dvbpsi_eit_event_descriptor_add @ 0041c2fc
 * Role (upstream libdvbpsi 1.x): Creates a descriptor via dvbpsi_NewDescriptor
 *   and appends it to p_event->p_first_descriptor.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (tags 0x4e, 0x54, 0x4d), and
 *   dvbpsi_eit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_eit_event_descriptor_add
          (dvbpsi_eit_event_t *p_event,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_event->p_first_descriptor,p_descriptor);
    p_event->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_event->p_first_descriptor","tables/eit.c",0x10f,
                    "dvbpsi_eit_event_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_eit_sections_decode @ 0041c394
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_eit with events and their descriptors.
 * EIT section payload layout (after 8-byte PSI header):
 *   +0..+1: transport_stream_id
 *   +2..+3: original_network_id
 *   +4:     segment_last_section_number
 *   +5:     last_table_id
 *   +6..:   loop of event entries, each 12 bytes + descriptors_loop_length:
 *             +0..+1:  event_id
 *             +2..+6:  start_time (40-bit MJD+UTC)
 *             +7..+9:  duration (24-bit BCD)
 *             +10..+11: running_status[15:13], free_CA_mode[12], descriptors_loop_length[11:0]
 * Quirk: `(bVar1 & 8) == 0` is passed as `b_free_ca` — wait: bit 4 (0x10) is
 *   free_CA_mode in EN 300 468, whereas bit 3 (0x08) is reserved, and `== 0`
 *   inverts it! (Upstream libdvbpsi bug in this version.)
 * Callers: dvbpsi_eit_sections_gather (dead code in av_streamer). */
void dvbpsi_eit_sections_decode(dvbpsi_eit_t *p_eit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (byte 10 of event / descriptor length)
  dvbpsi_eit_event_t *p_event;       // newly added event node
  uint8_t *puVar2;                   // p_ev_end (clamped to p_payload_end)
  uint8_t *puVar3;                   // current byte cursor (event start, then descriptor cursor)
  uint uVar4;                        // i_ev_length (12-bit descriptors_loop_length)
  uint8_t *puVar5;                   // p_section->p_payload_end
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar5 = p_section->p_payload_end;
    puVar3 = p_section->p_payload_start + 6; // skip 6-byte EIT header
    while (puVar3 < puVar5) {
      while( true ) {
                    /* Unresolved local var: uint16_t i_event_id@[???]
                       Unresolved local var: uint64_t i_start_time@[???]
                       Unresolved local var: uint32_t i_duration@[???]
                       Unresolved local var: uint8_t i_running_status@[???]
                       Unresolved local var: _Bool b_free_ca@[???]
                       Unresolved local var: uint16_t i_ev_length@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???]
                       Unresolved local var: uint8_t * p_ev_end@[???] */
        bVar1 = puVar3[10];
        uVar4 = (bVar1 & 0xf) << 8 | (uint)puVar3[0xb];
        p_event = dvbpsi_eit_event_add
                            (p_eit,CONCAT11(*puVar3,puVar3[1]),
                             (ulonglong)
                             CONCAT14(puVar3[2],
                                      (uint)puVar3[3] << 0x18 | (uint)puVar3[6] |
                                      (uint)puVar3[4] << 0x10 | (uint)puVar3[5] << 8),
                             (uint)puVar3[7] << 0x10 | (uint)puVar3[8] << 8 | (uint)puVar3[9],
                             bVar1 >> 5,(bVar1 & 8) == 0,(uint16_t)uVar4);
        if (p_event == (dvbpsi_eit_event_t *)0x0) goto LAB_0041c4cc;
        puVar3 = puVar3 + 0xc;       // advance past 12-byte event header
        puVar2 = puVar3 + uVar4;
        if (p_section->p_payload_end <= puVar3 + uVar4) {
          puVar2 = p_section->p_payload_end;
        }
        if (puVar2 <= puVar3) break;
        do {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
          bVar1 = puVar3[1];
          if ((int)(bVar1 + 1) < (int)puVar2 - (int)puVar3) {
            dvbpsi_eit_event_descriptor_add(p_event,*puVar3,bVar1,puVar3 + 2);
          }
          puVar3 = puVar3 + bVar1 + 2;
        } while (puVar3 < puVar2);
        if (puVar5 <= puVar3) goto LAB_0041c4cc;
      }
    }
LAB_0041c4cc:
    p_section = p_section->p_next;
  } while( true );
}



/* dvbpsi_eit_sections_gather @ 0041c518
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for EIT
 *   sections. Accepts table_id in [0x4e..0x6f] (actual/other p/f and schedule),
 *   checks TS discontinuity / service_id / version / last_section_number
 *   (inlined dvbpsi_CheckEIT), buffers sections (inlined dvbpsi_AddSectionEIT),
 *   checks EIT segment completion (inlined dvbpsi_IsCompleteEIT using
 *   i_first_received_section_number and segment_last_section_number at
 *   p_payload_start[4]), decodes, and invokes pf_eit_callback.
 * Callers: registered in dvbpsi_eit_attach (dead code in av_streamer). */
void dvbpsi_eit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // current section's i_number during completion scan
  short sVar2;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var3;                       // boolean return from PSI helpers
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  int iVar4;                         // scratch: p_building_eit or pf_eit_callback
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  uint uVar5;                        // i_first_received_section_number (+0x38)
  dvbpsi_psi_section_t *pdVar6;      // section list cursor in completion check
  uint8_t *puVar7;                   // p_section->p_payload_start
  dvbpsi_callback_gather_t p_Var8;   // copy of current_eit bytes +0x08..+0x0b
  dvbpsi_eit_t *pdVar9;              // p_building_eit (dvbpsi_eit_t *)
  undefined4 uVar10;                 // p_eit_decoder->p_cb_data (+0x20)
  undefined1 b_force;                // scratch arg passed to dvbpsi_ReInitEIT (ignored by it)
  dvbpsi_psi_section_t *pdVar11;     // p_section / p_private_decoder->p_sections
  uint8_t table_id;                  // expected table_id passed to dvbpsi_CheckPSISection
  dvbpsi_psi_section_t *pdVar12;     // copy of current_eit bytes +0x00..+0x03
  dvbpsi_decoder_t *pdVar13;         // p_demux (p_dvbpsi->p_decoder)
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x1ab,"dvbpsi_eit_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x1ac,"dvbpsi_eit_sections_gather");
  }
  // Accept table_id in [0x4e, 0x6f]: (uint8_t)(i_table_id - 0x4e) < 0x22
  table_id = 0x4e;
  if ((byte)(p_section->i_table_id + 0xb2) < 0x22) {
    table_id = p_section->i_table_id;
  }
  pdVar11 = p_section;
  _Var3 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"EIT decoder");
  if (CONCAT31(extraout_var,_Var3) == 0) goto LAB_0041c7e8;
  pdVar13 = p_dvbpsi->p_decoder;
  if (pdVar13->b_discontinuity == false) {
    iVar4 = p_private_decoder[1].i_need; // +0x34: p_building_eit
    if (iVar4 == 0) {
      if ((p_private_decoder->b_current_valid != false) &&
         (sVar2._0_1_ = p_section->i_version, sVar2._1_1_ = p_section->b_current_next,
         *(short *)&p_private_decoder[1].p_sections == sVar2)) {
        dvbpsi_debug(p_dvbpsi,"EIT decoder","ignoring already decoded section %d",
                     (uint)p_section->i_number);
        goto LAB_0041c7e8;
      }
    }
    else {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint16_t *)(iVar4 + 2) == p_section->i_extension) {
        if (*(uint8_t *)(iVar4 + 4) == p_section->i_version) {
          if (p_private_decoder->i_last_section_number == p_section->i_last_number)
          goto LAB_0041c5d8;
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'version_number\' differs whereas no discontinuity has occurred");
        }
      }
      else {
        b_force = 0x20;
        dvbpsi_error(p_dvbpsi,"EIT decoder",
                     "\'service_id\' differs whereas no TS discontinuity has occurred");
      }
      dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,(_Bool)b_force);
    }
  }
  else {
    dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,SUB41(pdVar11,0));
    p_private_decoder->b_discontinuity = false;
    pdVar13->b_discontinuity = false;
  }
LAB_0041c5d8:
  if (p_private_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_eit_decoder","tables/eit.c",0x181,"dvbpsi_AddSectionEIT");
  }
  if (p_private_decoder[1].i_need == 0) {
    puVar7 = p_section->p_payload_start;
    pdVar9 = dvbpsi_eit_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                            p_section->b_current_next,CONCAT11(*puVar7,puVar7[1]),
                            CONCAT11(puVar7[2],puVar7[3]),puVar7[4],puVar7[5]);
    p_private_decoder[1].i_need = (int)pdVar9;
    p_private_decoder->i_last_section_number = p_section->i_last_number;
    p_private_decoder[2].i_magic[0] = p_section->i_number; // +0x38: i_first_received_section_number
    if (pdVar9 == (dvbpsi_eit_t *)0x0) {
      dvbpsi_error(p_dvbpsi,"EIT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041c7e8:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    p_private_decoder->i_last_section_number = p_section->i_last_number;
  }
  _Var3 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var3) != 0) {
    dvbpsi_debug(p_dvbpsi,"EIT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
                    /* Unresolved local var: _Bool b_complete@[???]
                       Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
  // Fast check: table can only be complete when we hit last_section_number (if started at 0)
  // or wrapped back to i_first_received_section_number - 1
  uVar5 = (uint)p_private_decoder[2].i_magic[0];
  if (uVar5 == 0) {
    if (p_section->i_number != p_private_decoder->i_last_section_number) {
      return;
    }
  }
  else if ((uVar5 != p_section->i_number) && ((uint)p_section->i_number != uVar5 - 1)) {
    return;
  }
                    /* Unresolved local var: dvbpsi_psi_section_t * p@[???] */
  // Walk p_sections verifying EIT segment continuity (p_payload_start[4] = segment_last_section_number)
  pdVar11 = p_private_decoder->p_sections;
  if (pdVar11 != (dvbpsi_psi_section_t *)0x0) {
    bVar1 = pdVar11->i_number;
    pdVar6 = pdVar11;
    while( true ) {
      while( true ) {
        if (bVar1 == p_private_decoder->i_last_section_number) {
          iVar4._0_1_ = p_private_decoder[1].i_magic[0];
          iVar4._1_1_ = p_private_decoder[1].i_magic[1];
          iVar4._2_1_ = p_private_decoder[1].i_magic[2];
          iVar4._3_1_ = p_private_decoder[1].b_complete_header;
          if (iVar4 == 0) {
                    /* WARNING: Subroutine does not return */
            __assert_fail("p_eit_decoder->pf_eit_callback","tables/eit.c",0x1eb,
                          "dvbpsi_eit_sections_gather");
          }
          // Copy *p_building_eit into current_eit (+0x24..+0x33)
          pdVar9 = (dvbpsi_eit_t *)p_private_decoder[1].i_need;
          pdVar12 = *(dvbpsi_psi_section_t **)pdVar9;
          pdVar6 = *(dvbpsi_psi_section_t **)&pdVar9->i_version;
          p_Var8 = *(dvbpsi_callback_gather_t *)&pdVar9->i_network_id;
          p_private_decoder[1].i_section_max_size = (int)pdVar9->p_first_event;
          p_private_decoder[1].p_current_section = pdVar12;
          p_private_decoder[1].p_sections = pdVar6;
          p_private_decoder[1].pf_gather = p_Var8;
          p_private_decoder->b_current_valid = true;
          dvbpsi_eit_sections_decode(pdVar9,pdVar11);
          dvbpsi_DeletePSISections(p_private_decoder->p_sections);
          uVar10._0_1_ = p_private_decoder[1].b_discontinuity;
          uVar10._1_1_ = p_private_decoder[1].b_current_valid;
          uVar10._2_1_ = p_private_decoder[1].i_continuity_counter;
          uVar10._3_1_ = p_private_decoder[1].i_last_section_number;
          iVar4 = p_private_decoder[1].i_need;
          p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
          (**(code **)(p_private_decoder + 1))(uVar10,iVar4); // pf_eit_callback(p_cb_data, p_building_eit)
          p_private_decoder[1].i_need = 0; // caller owns p_building_eit; no ReInitEIT needed
          return;
        }
        if (pdVar6->p_payload_start[4] != bVar1) break;
        do {
          pdVar6 = pdVar6->p_next;
          if (pdVar6 == (dvbpsi_psi_section_s *)0x0) {
            return;
          }
          bVar1 = pdVar6->i_number;
        } while (bVar1 < p_private_decoder->i_last_section_number);
      }
      pdVar6 = pdVar6->p_next;
      if (pdVar6 == (dvbpsi_psi_section_s *)0x0) break;
      bVar1 = pdVar6->i_number;
    }
  }
  return;
}



/* dvbpsi_eit_sections_generate @ 0041c960
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_eit_t and its event/
 *   descriptor lists into a linked list of 4094-byte (0xffe) PSI sections via
 *   NewEITSection() and finalizes CRC32 via dvbpsi_BuildPSISection().
 * Parameters: p_dvbpsi, p_eit, i_table_id (e.g. 0x4e).
 * Returns: head of linked dvbpsi_psi_section_t * list, or NULL on alloc failure.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t *
dvbpsi_eit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_eit_t *p_eit,uint8_t i_table_id)

{
  dvbpsi_psi_section_s **ppdVar1;    // &p_current->p_next in finalization loop
  int iVar2;                         // projected byte offset in section
  dvbpsi_psi_section_t *pdVar3;      // head of section list (p_result)
  dvbpsi_descriptor_t *pdVar4;       // descriptor list cursor
  uint8_t *puVar5;                   // write cursor in current section
  dvbpsi_psi_section_t *pdVar6;      // newly allocated overflow section
  uint uVar7;                        // accumulated byte size of event + descriptors
  uint8_t *puVar8;                   // projected end offset for overflow check (< 0xffb)
  dvbpsi_psi_section_t *pdVar9;      // current section being populated (p_current)
  dvbpsi_eit_event_t *pdVar10;       // event list cursor (p_event)
  uint8_t *puVar11;                  // start of current event entry (p_event_start)
  uint i_section_number;             // running section_number (becomes last_section_number)
  byte *pbVar12;                     // pointer to byte 10 of event entry (descriptors_loop_length hi)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: uint8_t i_last_section_number@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  pdVar3 = NewEITSection(p_eit,(uint)i_table_id,0);
  if (pdVar3 != (dvbpsi_psi_section_t *)0x0) {
    pdVar10 = p_eit->p_first_event;
    i_section_number = 0;
    if (pdVar10 != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: uint8_t * p_event_start@[???]
                       Unresolved local var: uint16_t i_event_length@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
      pdVar9 = pdVar3;
LAB_0041c9d8:
      do {
        pdVar4 = pdVar10->p_first_descriptor;
        puVar11 = pdVar9->p_payload_end;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          // Estimate space needed for 12B event header + 2B desc hdr + first desc payload
          uVar7 = pdVar4->i_length + 0xe;
          iVar2 = uVar7 - (int)pdVar9->p_data;
          while ((int)(puVar11 + iVar2) < 0xffb) { // 0xffb = 4091 (+ 4B CRC = 4095 <= 4096)
            pdVar4 = pdVar4->p_next;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041ca38;
            uVar7 = uVar7 + pdVar4->i_length + 2 & 0xffff;
            iVar2 = uVar7 - (int)pdVar9->p_data;
          }
          i_section_number = i_section_number + 1 & 0xff;
          pdVar6 = NewEITSection(p_eit,(uint)i_table_id,i_section_number);
          puVar11 = pdVar6->p_payload_end;
          pdVar9->p_next = pdVar6;
          pdVar9 = pdVar6;
        }
LAB_0041ca38:
        pbVar12 = puVar11 + 10;
        *puVar11 = (uint8_t)(pdVar10->i_event_id >> 8);
        puVar11[1] = (uint8_t)pdVar10->i_event_id;
        // Serialize 40-bit start_time (bytes +3..+7 of little-endian uint64_t? Note MIPS-LE layout)
        puVar11[2] = (uint8_t)*(undefined4 *)((int)&pdVar10->i_start_time + 4);
        puVar11[3] = *(uint8_t *)((int)&pdVar10->i_start_time + 3);
        puVar11[4] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_start_time + 2);
        puVar11[5] = (uint8_t)(pdVar10->i_start_time >> 8);
        puVar11[6] = (uint8_t)pdVar10->i_start_time;
        // Serialize 24-bit duration
        puVar11[7] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_duration + 2);
        puVar11[8] = (uint8_t)(pdVar10->i_duration >> 8);
        puVar11[9] = (uint8_t)pdVar10->i_duration;
        puVar11[10] = (pdVar10->b_free_ca & 1U) << 4 | pdVar10->i_running_status << 5;
        pdVar4 = pdVar10->p_first_descriptor;
        puVar5 = pdVar9->p_payload_end + 0xc;
        pdVar9->p_payload_end = puVar5;
        pdVar9->i_length = pdVar9->i_length + 0xc;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          while ((int)puVar8 < 0xffb) {
            *puVar5 = pdVar4->i_tag;
            pdVar9->p_payload_end[1] = pdVar4->i_length;
            memcpy(pdVar9->p_payload_end + 2,pdVar4->p_data,(size_t)pdVar4->i_length);
            puVar5 = pdVar9->p_payload_end + pdVar4->i_length + 2;
            pdVar9->p_payload_end = puVar5;
            puVar8 = &pdVar4->i_length;
            pdVar4 = pdVar4->p_next;
            pdVar9->i_length = pdVar9->i_length + 2 + (ushort)*puVar8;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041cb74;
            puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          }
          dvbpsi_error(p_dvbpsi,"EIT generator",
                       "too many descriptors in event, unable to carry all the descriptors");
          puVar5 = pdVar9->p_payload_end;
          *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
          puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
          pdVar10 = pdVar10->p_next;
          if (pdVar10 == (dvbpsi_eit_event_s *)0x0) {
            pdVar9 = pdVar3;
            goto LAB_0041cba4;
          }
          goto LAB_0041c9d8;
        }
LAB_0041cb74:
        // Backfill 12-bit descriptors_loop_length in puVar11[10..11]
        *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
        puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
        pdVar10 = pdVar10->p_next;
      } while (pdVar10 != (dvbpsi_eit_event_s *)0x0);
    }
    pdVar9 = pdVar3;
LAB_0041cba4:
    do {
      pdVar9->p_data[0xc] = (uint8_t)i_section_number; // segment_last_section_number
      pdVar9->i_last_number = (uint8_t)i_section_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar9);
      ppdVar1 = &pdVar9->p_next;
      pdVar9 = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_s *)0x0);
  }
  return pdVar3;
}


/* #####################################################################
 * FILE: 12_tables_nit.c.c
 * ##################################################################### */

/*
 * 12_tables_nit.c.c  --  annotated decompilation of libdvbpsi tables/nit.c
 * =========================================================================
 *
 * Original source file: tables/nit.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_nit_*` public API
 * (`dvbpsi_nit_attach`, `dvbpsi_nit_init`, `dvbpsi_nit_sections_generate`,
 * `dvbpsi_nit_descriptor_add`, `dvbpsi_nit_ts_add`,
 * `dvbpsi_nit_ts_descriptor_add`), and `dvbpsi_demux_t` sub-decoder model.
 * Internal helper `dvbpsi_ReInitNIT` retains the older CamelCase naming.
 *
 * Purpose in av_streamer:
 *   Provides DVB Network Information Table (NIT, ETSI EN 300 468, PID 0x0010,
 *   table_id 0x40 actual network / 0x41 other network) construction, section
 *   serialization, and section parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genNIT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_nit_init`, `dvbpsi_nit_descriptor_add`,
 *   `dvbpsi_nit_ts_add`, `dvbpsi_nit_ts_descriptor_add`,
 *   `dvbpsi_nit_sections_generate`, `dvbpsi_nit_empty`), which emits an NIT on
 *   PID 0x0010 (table_id=0x40, network_id=0x2114, version=0x17) with a
 *   network_name_descriptor (0x40, "MELD") and one TS entry (ts_id=0,
 *   orig_network_id=0x2114) carrying terrestrial_delivery_system (0x5a),
 *   cell_list (0x6c, lat/lon), and service_list (0x41) descriptors.
 *   The decoder/demux functions (`dvbpsi_nit_attach`, `dvbpsi_nit_detach`,
 *   `dvbpsi_nit_new`, `dvbpsi_nit_delete`, `dvbpsi_ReInitNIT`,
 *   `dvbpsi_nit_sections_decode`, `dvbpsi_nit_sections_gather`) are dead code
 *   in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_nit_t (16 bytes):
 *     +0x00 uint8_t  i_table_id          (0x40 = NIT actual, 0x41 = NIT other)
 *     +0x02 uint16_t i_extension         (sub-decoder extension)
 *     +0x04 uint16_t i_network_id        (written to PSI section_syntax_id / i_extension)
 *     +0x06 uint8_t  i_version           (5-bit version_number)
 *     +0x07 _Bool    b_current_next
 *     +0x08 dvbpsi_descriptor_t *p_first_descriptor  (network descriptors)
 *     +0x0c dvbpsi_nit_ts_t     *p_first_ts          (transport stream loop)
 *   dvbpsi_nit_ts_t (12 bytes):
 *     +0x00 uint16_t i_ts_id
 *     +0x02 uint16_t i_orig_network_id
 *     +0x04 dvbpsi_descriptor_t *p_first_descriptor  (TS descriptors)
 *     +0x08 dvbpsi_nit_ts_s     *p_next
 *   dvbpsi_nit_decoder_t (60 / 0x3c bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t      (28 bytes)
 *     +0x1c dvbpsi_nit_callback pf_nit_callback      (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                       (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_nit_t current_nit                 (+0x2a: {i_version, b_current_next})
 *     +0x34 dvbpsi_nit_t *p_building_nit             (Ghidra: p_decoder[1].i_need)
 *     +0x38 uint16_t i_network_id                    (Ghidra: p_decoder[2].i_magic[0..1])
 *
 * Globals owned: none.
 */

/* dvbpsi_nit_attach @ 0041cc80
 * Role (upstream libdvbpsi 1.x): Attaches an NIT sub-decoder for (i_table_id,
 *   i_extension) to p_dvbpsi's demuxer. Stores i_extension as expected
 *   i_network_id at +0x38.
 * Parameters: p_dvbpsi, i_table_id (0x40 or 0x41), i_extension (network_id),
 *   pf_callback, p_cb_data.
 * Returns: true on success, false if already attached or allocation fails.
 * Callers: dead code in av_streamer. */
_Bool dvbpsi_nit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_nit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_nit_decoder_t (0x3c bytes)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/nit.c",0x3e,"dvbpsi_nit_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/nit.c",0x3f,"dvbpsi_nit_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x3c (60) bytes for dvbpsi_nit_decoder_t
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_nit_detach,dvbpsi_nit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(uint16_t *)p_decoder[2].i_magic = i_extension;       // +0x38: i_network_id
        *(dvbpsi_nit_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_nit_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_need = 0;                               // +0x34: p_building_nit = NULL
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"NIT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



/* dvbpsi_nit_init @ 0041ce34
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_nit_t.
 * Parameters: p_nit, i_table_id (0x40), i_extension, i_network_id, i_version,
 *   b_current_next.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (0x40, ext=3, net_id=0x2114,
 *   ver=0x17, b_current_next=true). */
void dvbpsi_nit_init(dvbpsi_nit_t *p_nit,uint8_t i_table_id,uint16_t i_extension,
                    uint16_t i_network_id,uint8_t i_version,_Bool b_current_next)

{
  p_nit->i_table_id = i_table_id;
  p_nit->i_extension = i_extension;
  p_nit->i_network_id = i_network_id;
  p_nit->i_version = i_version;
  p_nit->b_current_next = b_current_next;
  p_nit->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  p_nit->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  return;
}



/* dvbpsi_nit_new @ 0041ce5c
 * Role (upstream libdvbpsi 1.x): Heap-allocates (16 bytes) and initializes a
 *   dvbpsi_nit_t structure.
 * Returns: newly allocated dvbpsi_nit_t *, or NULL on malloc failure.
 * Callers: dvbpsi_nit_sections_gather (via inlined dvbpsi_AddSectionNIT). */
dvbpsi_nit_t *
dvbpsi_nit_new(uint8_t i_table_id,uint16_t i_extension,uint16_t i_network_id,uint8_t i_version,
              _Bool b_current_next)

{
  dvbpsi_nit_t *pdVar1;              // newly allocated dvbpsi_nit_t *
  
                    /* Unresolved local var: dvbpsi_nit_t * p_nit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);             // sizeof(dvbpsi_nit_t) == 16
  if (pdVar1 != (dvbpsi_nit_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_network_id = i_network_id;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    pdVar1->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_nit_empty @ 0041cee8
 * Role (upstream libdvbpsi 1.x): Frees network-level descriptors, all TS
 *   entries in p_nit->p_first_ts, and each TS entry's descriptor list.
 * Callers: genNIT() in 01_libxcode4encoder.c.c, and dvbpsi_nit_delete(). */
void dvbpsi_nit_empty(dvbpsi_nit_t *p_nit)

{
  dvbpsi_nit_ts_t *__ptr;            // current TS node being freed (p_ts)
  dvbpsi_nit_ts_s *pdVar1;           // saved next pointer (p_tmp)
  
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???] */
  __ptr = p_nit->p_first_ts;
  dvbpsi_DeleteDescriptors(p_nit->p_first_descriptor);
  while (__ptr != (dvbpsi_nit_ts_t *)0x0) {
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_nit->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  p_nit->p_first_ts = (dvbpsi_nit_ts_t *)0x0;
  return;
}



/* dvbpsi_nit_delete @ 0041cf70
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_nit_t.
 * Callers: dvbpsi_ReInitNIT, dvbpsi_nit_detach. */
void dvbpsi_nit_delete(dvbpsi_nit_t *p_nit)

{
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_empty(p_nit);
  }
  free(p_nit);
  return;
}



/* dvbpsi_ReInitNIT @ 0041cfb4
 * Role (upstream libdvbpsi 1.x internal): Resets the NIT decoder state.
 * Parameters: p_decoder (asserted non-NULL), b_force (if true, also deletes
 *   p_building_nit).
 * Callers: dvbpsi_nit_sections_gather. */
void dvbpsi_ReInitNIT(dvbpsi_nit_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;   // upper 24 bits of MIPS $a1 register (b_force)
  
  if (p_decoder == (dvbpsi_nit_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/nit.c",0x11d,"dvbpsi_ReInitNIT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_nit != (dvbpsi_nit_t *)0x0)) {
    dvbpsi_nit_delete(p_decoder->p_building_nit);
  }
  p_decoder->p_building_nit = (dvbpsi_nit_t *)0x0;
  return;
}



/* dvbpsi_nit_detach @ 0041d040
 * Role (upstream libdvbpsi 1.x): Detaches and frees the NIT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Quirk: omits NULL-pointer asserts on p_dvbpsi and p_dvbpsi->p_decoder that
 *   dvbpsi_sdt_detach and dvbpsi_eit_detach include.
 * Callers: dead code in av_streamer. */
void dvbpsi_nit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_nit_t *p_nit;               // p_nit_decoder->p_building_nit (+0x34)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_nit_decoder_t *)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???] */
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"NIT Decoder",
                 "No such NIT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  p_nit = (dvbpsi_nit_t *)pdVar1[1].i_need; // +0x34: p_building_nit
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_delete(p_nit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_nit_descriptor_add @ 0041d138
 * Role (upstream libdvbpsi 1.x): Creates a network-level descriptor and appends
 *   it to p_nit->p_first_descriptor (inline list append rather than calling
 *   dvbpsi_AddDescriptor).
 * Callers: genNIT() in 01_libxcode4encoder.c.c (tag 0x40 network_name "MELD"),
 *   and dvbpsi_nit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_nit_descriptor_add(dvbpsi_nit_t *p_nit,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *pdVar1;       // newly allocated descriptor (p_descriptor)
  dvbpsi_descriptor_t *pdVar2;       // list traversal cursor (p_last_descriptor)
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[DW_OP_reg2(v0)] */
  pdVar1 = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (pdVar1 != (dvbpsi_descriptor_t *)0x0) {
    pdVar2 = p_nit->p_first_descriptor;
    if (p_nit->p_first_descriptor == (dvbpsi_descriptor_t *)0x0) {
      p_nit->p_first_descriptor = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_descriptor_s *)0x0) {
                    /* Unresolved local var: dvbpsi_descriptor_t * p_last_descriptor@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_nit_ts_add @ 0041d1b0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_nit_ts_t (12 bytes) and
 *   appends it to p_nit->p_first_ts.
 * Parameters: p_nit, i_ts_id, i_orig_network_id.
 * Returns: newly allocated TS entry, or NULL on malloc failure.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (ts_id=0, orig_net_id=0x2114),
 *   and dvbpsi_nit_sections_decode(). */
dvbpsi_nit_ts_t * dvbpsi_nit_ts_add(dvbpsi_nit_t *p_nit,uint16_t i_ts_id,uint16_t i_orig_network_id)

{
  dvbpsi_nit_ts_t *pdVar1;           // newly allocated TS entry (p_ts)
  dvbpsi_nit_ts_t *pdVar2;           // list traversal cursor (p_last_ts)
  
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_ts@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);              // sizeof(dvbpsi_nit_ts_t) == 12
  if (pdVar1 != (dvbpsi_nit_ts_t *)0x0) {
    pdVar2 = p_nit->p_first_ts;
    pdVar1->i_ts_id = i_ts_id;
    pdVar1->i_orig_network_id = i_orig_network_id;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    pdVar1->p_next = (dvbpsi_nit_ts_s *)0x0;
    if (pdVar2 == (dvbpsi_nit_ts_t *)0x0) {
      p_nit->p_first_ts = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_nit_ts_s *)0x0) {
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_last_ts@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_nit_ts_descriptor_add @ 0041d244
 * Role (upstream libdvbpsi 1.x): Creates a TS-level descriptor and appends it
 *   to p_ts->p_first_descriptor via dvbpsi_AddDescriptor.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (tags 0x5a, 0x6c, 0x41), and
 *   dvbpsi_nit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_nit_ts_descriptor_add(dvbpsi_nit_ts_t *p_ts,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_ts->p_first_descriptor,p_descriptor);
    p_ts->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_ts->p_first_descriptor","tables/nit.c",0x113,"dvbpsi_nit_ts_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_nit_sections_decode @ 0041d2dc
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_nit with network descriptors and TS entries (+ their
 *   descriptors).
 * NIT section payload layout (after 8-byte PSI header):
 *   +0..+1: reserved_future_use[15:12], network_descriptors_length[11:0]
 *   +2..:   network descriptor loop
 *   next 2B: reserved_future_use[15:12], transport_stream_loop_length[11:0]
 *   loop of TS entries, each 6 bytes + transport_descriptors_length:
 *     +0..+1: transport_stream_id
 *     +2..+3: original_network_id
 *     +4..+5: reserved_future_use[15:12], transport_descriptors_length[11:0]
 * Callers: dvbpsi_nit_sections_gather (dead code in av_streamer). */
void dvbpsi_nit_sections_decode(dvbpsi_nit_t *p_nit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (length hi / descriptor length)
  uint8_t i_tag;                     // descriptor tag
  byte bVar2;                        // low byte of transport_descriptors_length
  dvbpsi_nit_ts_t *p_ts;             // newly added TS entry
  uint8_t *p_data;                   // descriptor payload pointer
  byte *pbVar3;                      // current byte cursor / lookahead pointer
  uint uVar4;                        // descriptor length
  byte *pbVar5;                      // end of current descriptor loop / TS loop
  byte *pbVar6;                      // descriptor payload or TS entry cursor
  byte *pbVar7;                      // next descriptor payload pointer
  byte *pbVar8;                      // scratch: &length byte or end of TS descriptors
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    pbVar6 = p_section->p_payload_start;
    pbVar3 = pbVar6 + 2;
    pbVar5 = pbVar3 + ((*pbVar6 & 0xf) << 8 | (uint)pbVar6[1]); // end of network descriptors
    pbVar6 = pbVar6 + 4;
    if (pbVar5 < pbVar6) {
      bVar1 = *pbVar3;
    }
    else {
      do {
        while( true ) {
          pbVar8 = pbVar3 + 1;
          uVar4 = (uint)*pbVar8;
          i_tag = *pbVar3;
          if ((int)pbVar5 - (int)pbVar3 <= (int)(uVar4 + 1)) break;
          pbVar3 = pbVar3 + uVar4 + 2;
          pbVar7 = pbVar3 + 2;
          dvbpsi_nit_descriptor_add(p_nit,i_tag,*pbVar8,pbVar6);
          pbVar6 = pbVar7;
          if (pbVar5 < pbVar7) goto LAB_0041d3a0;
        }
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        pbVar3 = pbVar3 + uVar4 + 2;
        pbVar6 = pbVar3 + 2;
      } while (pbVar6 <= pbVar5);
LAB_0041d3a0:
      bVar1 = *pbVar3;               // high byte of transport_stream_loop_length
    }
    pbVar5 = pbVar3 + ((bVar1 & 0xf) << 8 | (uint)pbVar3[1]) + 2;
    if (p_section->p_payload_end <= pbVar3 + ((bVar1 & 0xf) << 8 | (uint)pbVar3[1]) + 2) {
      pbVar5 = p_section->p_payload_end;
    }
    while (pbVar3 = pbVar6 + 6, pbVar3 <= pbVar5) {
                    /* Unresolved local var: uint8_t * p_end2@[???]
                       Unresolved local var: uint16_t i_ts_id@[???]
                       Unresolved local var: uint16_t i_orig_network_id@[???]
                       Unresolved local var: uint16_t i_ts_length@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???] */
      bVar1 = pbVar6[4];
      bVar2 = pbVar6[5];
      p_ts = dvbpsi_nit_ts_add(p_nit,CONCAT11(*pbVar6,pbVar6[1]),CONCAT11(pbVar6[2],pbVar6[3]));
      if (p_ts == (dvbpsi_nit_ts_t *)0x0) break;
      pbVar8 = pbVar3 + ((bVar1 & 0xf) << 8 | (uint)bVar2);
      if (p_section->p_payload_end <= pbVar3 + ((bVar1 & 0xf) << 8 | (uint)bVar2)) {
        pbVar8 = p_section->p_payload_end;
      }
      p_data = pbVar6 + 8;
      pbVar6 = pbVar3;
      while (p_data <= pbVar8) {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        bVar1 = pbVar6[1];
        if ((int)(bVar1 + 1) < (int)pbVar8 - (int)pbVar6) {
          dvbpsi_nit_ts_descriptor_add(p_ts,*pbVar6,bVar1,p_data);
        }
        pbVar6 = pbVar6 + bVar1 + 2;
        p_data = pbVar6 + 2;
      }
    }
    p_section = p_section->p_next;
  } while( true );
}



/* dvbpsi_nit_sections_gather @ 0041d4d4
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for NIT
 *   sections. Accepts table_id 0x40 (actual) or 0x41 (other), verifies
 *   section->i_extension == p_nit_decoder->i_network_id, checks discontinuity /
 *   version / last_section_number (inlined dvbpsi_CheckNIT), buffers sections
 *   (inlined dvbpsi_AddSectionNIT), decodes when complete, and invokes
 *   pf_nit_callback.
 * Callers: registered in dvbpsi_nit_attach (dead code in av_streamer). */
void dvbpsi_nit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var2;                       // boolean return from PSI helpers
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  undefined3 extraout_var_01;        // MIPS ABI upper 3 bytes
  dvbpsi_descriptor_t *pdVar3;       // copy of current_nit.p_first_descriptor
  dvbpsi_nit_t *pdVar4;              // p_building_nit (dvbpsi_nit_t *)
  undefined4 uVar5;                  // p_nit_decoder->p_cb_data (+0x20)
  uint16_t i_extension;              // expected i_network_id (+0x38) / p_section->i_extension
  dvbpsi_psi_section_t *p_section_00; // p_private_decoder->p_sections
  int iVar6;                         // scratch: pf_nit_callback / p_building_nit
  uint8_t table_id;                  // expected table_id (0x40 or 0x41)
  uint16_t i_network_id;             // p_section->i_extension / expected i_network_id
  dvbpsi_psi_section_t *pdVar7;      // copy of current_nit bytes +0x04..+0x07
  dvbpsi_psi_section_t *pdVar8;      // copy of current_nit bytes +0x00..+0x03
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/nit.c",0x16b,"dvbpsi_nit_sections_gather");
  }
  // Accept table_id 0x40 (NIT actual) or 0x41 (NIT other)
  table_id = 0x40;
  if ((byte)(p_section->i_table_id - 0x40) < 2) {
    table_id = p_section->i_table_id;
  }
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"NIT decoder");
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041d6b0;
  i_extension = *(uint16_t *)p_private_decoder[2].i_magic; // +0x38: i_network_id
  i_network_id = p_section->i_extension;
  if (i_network_id != i_extension) {
    dvbpsi_error(p_dvbpsi,"NIT decoder","\'network_id\' don\'t match");
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  if (p_private_decoder->b_discontinuity == false) {
    if (p_private_decoder[1].i_need != 0) { // +0x34: p_building_nit
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint8_t *)(p_private_decoder[1].i_need + 6) == p_section->i_version) {
        if (p_private_decoder->i_last_section_number == p_section->i_last_number) goto LAB_0041d5a0;
        dvbpsi_error(p_dvbpsi,"NIT decoder",
                     "\'last_section_number\' differs whereas no discontinuity has occured");
      }
      else {
        dvbpsi_error(p_dvbpsi,"NIT decoder",
                     "\'version_number\' differs whereas no discontinuity has occured");
      }
      dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,true);
      goto LAB_0041d594;
    }
    // Check {i_version, b_current_next} at current_nit+0x06 (+0x2a in decoder)
    if ((p_private_decoder->b_current_valid != false) &&
       (sVar1._0_1_ = p_section->i_version, sVar1._1_1_ = p_section->b_current_next,
       *(short *)((int)&p_private_decoder[1].p_sections + 2) == sVar1)) {
      dvbpsi_debug(p_dvbpsi,"NIT decoder","ignoring already decoded section %d",
                   (uint)p_section->i_number);
      goto LAB_0041d6b0;
    }
  }
  else {
    dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,true);
    p_private_decoder->b_discontinuity = false;
LAB_0041d594:
                    /* Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
    if (p_private_decoder[1].i_need != 0) goto LAB_0041d5a0;
    i_extension = p_section->i_extension;
    i_network_id = *(uint16_t *)p_private_decoder[2].i_magic;
  }
  pdVar4 = dvbpsi_nit_new(p_section->i_table_id,i_extension,i_network_id,p_section->i_version,
                          p_section->b_current_next);
  p_private_decoder[1].i_need = (int)pdVar4;
  if (pdVar4 == (dvbpsi_nit_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"NIT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041d6b0:
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  p_private_decoder->i_last_section_number = p_section->i_last_number;
LAB_0041d5a0:
  _Var2 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var2) != 0) {
    dvbpsi_debug(p_dvbpsi,"NIT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var2 = dvbpsi_decoder_psi_sections_completed(p_private_decoder);
  if (CONCAT31(extraout_var_01,_Var2) == 0) {
    return;
  }
  iVar6._0_1_ = p_private_decoder[1].i_magic[0];
  iVar6._1_1_ = p_private_decoder[1].i_magic[1];
  iVar6._2_1_ = p_private_decoder[1].i_magic[2];
  iVar6._3_1_ = p_private_decoder[1].b_complete_header;
  if (iVar6 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_nit_decoder->pf_nit_callback","tables/nit.c",0x1ae,"dvbpsi_nit_sections_gather"
                 );
  }
  // Copy *p_building_nit into current_nit (+0x24..+0x33)
  pdVar4 = (dvbpsi_nit_t *)p_private_decoder[1].i_need;
  p_section_00 = p_private_decoder->p_sections;
  pdVar8 = *(dvbpsi_psi_section_t **)pdVar4;
  pdVar7 = *(dvbpsi_psi_section_t **)&pdVar4->i_network_id;
  pdVar3 = pdVar4->p_first_descriptor;
  p_private_decoder[1].i_section_max_size = (int)pdVar4->p_first_ts;
  p_private_decoder[1].p_current_section = pdVar8;
  p_private_decoder[1].p_sections = pdVar7;
  p_private_decoder[1].pf_gather = (dvbpsi_callback_gather_t)pdVar3;
  p_private_decoder->b_current_valid = true;
  dvbpsi_nit_sections_decode(pdVar4,p_section_00);
  dvbpsi_DeletePSISections(p_private_decoder->p_sections);
  uVar5._0_1_ = p_private_decoder[1].b_discontinuity;
  uVar5._1_1_ = p_private_decoder[1].b_current_valid;
  uVar5._2_1_ = p_private_decoder[1].i_continuity_counter;
  uVar5._3_1_ = p_private_decoder[1].i_last_section_number;
  iVar6 = p_private_decoder[1].i_need;
  p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_private_decoder + 1))(uVar5,iVar6); // pf_nit_callback(p_cb_data, p_building_nit)
  dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,false);
  return;
}



/* dvbpsi_nit_sections_generate @ 0041d808
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_nit_t (network-level
 *   descriptors + transport_stream loop + TS descriptors) into a linked list
 *   of 1024-byte (0x400) PSI sections and finalizes CRC32 via
 *   dvbpsi_BuildPSISection().
 * Parameters: p_dvbpsi, p_nit, i_table_id (0x40 or 0x41).
 * Returns: head of linked dvbpsi_psi_section_t * list.
 * Note: sets b_private_indicator = false (unlike SDT/EIT which set true), and
 *   writes p_nit->i_network_id into section i_extension.
 * Callers: genNIT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t *
dvbpsi_nit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_nit_t *p_nit,uint8_t i_table_id)

{
  uint8_t uVar1;                     // previous section i_number
  _Bool _Var2;                       // scratch p_nit->b_current_next
  byte bVar3;                        // scratch descriptor length
  uint16_t uVar4;                    // scratch p_nit->i_network_id
  dvbpsi_psi_section_t *pdVar5;      // head of section list (p_result)
  uint8_t *puVar6;                   // scratch: p_data / p_payload_start / TS entry start
  dvbpsi_psi_section_t *pdVar7;      // current section being populated (p_current)
  uint8_t *puVar8;                   // write cursor / p_transport_stream_loop_length
  uint uVar9;                        // accumulated byte size / loop length
  uint8_t *puVar10;                  // projected end offset for overflow check (< 0x3fb)
  uint8_t *puVar11;                  // write cursor inside TS descriptor loop
  int iVar12;                        // current byte count in section (p_payload_end - p_data)
  dvbpsi_descriptor_t *pdVar13;      // descriptor list cursor (p_descriptor)
  dvbpsi_psi_section_t *pdVar14;     // previous section pointer (p_prev) / finalization cursor
  dvbpsi_nit_ts_s *pdVar15;          // TS list cursor (p_ts)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???]
                       Unresolved local var: dvbpsi_nit_ts_t * p_ts@[???]
                       Unresolved local var: uint16_t i_network_descriptors_length@[???]
                       Unresolved local var: uint16_t i_transport_stream_loop_length@[???]
                       Unresolved local var: uint8_t * p_transport_stream_loop_length@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar13 = p_nit->p_first_descriptor;
  pdVar15 = p_nit->p_first_ts;
  pdVar5->i_table_id = i_table_id;
  uVar4 = p_nit->i_network_id;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = false;
  pdVar5->i_length = 0xd;            // 5B PSI ext + 2B net_desc_len + 2B ts_loop_len + 4B CRC = 13
  pdVar5->i_extension = uVar4;
  puVar6 = pdVar5->p_data;
  pdVar5->i_version = p_nit->i_version;
  puVar8 = pdVar5->p_payload_end + 10; // 8B PSI hdr + 2B network_descriptors_length = 10
  pdVar5->b_current_next = p_nit->b_current_next;
  pdVar5->i_number = 0;
  pdVar5->p_payload_end = puVar8;
  pdVar5->p_payload_start = puVar6 + 8;
  pdVar7 = pdVar5;
  pdVar14 = pdVar5;
  // Phase 1: serialize network-level descriptors
  if (pdVar13 != (dvbpsi_descriptor_t *)0x0) {
    while( true ) {
      pdVar7 = pdVar14;
      if (0x3fa < (int)(puVar8 + ((uint)pdVar13->i_length - (int)puVar6))) {
        puVar11 = pdVar14->p_payload_start;
        puVar6[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff) >> 8) | 0xf0;
        pdVar14->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff);
        *pdVar14->p_payload_end = 0; // empty transport_stream_loop_length in overflowed section
        pdVar14->p_payload_end[1] = 0;
        pdVar14->p_payload_end = pdVar14->p_payload_end + 2;
        pdVar7 = dvbpsi_NewPSISection(0x400);
        uVar1 = pdVar14->i_number;
        pdVar7->i_table_id = i_table_id;
        uVar4 = p_nit->i_network_id;
        pdVar14->p_next = pdVar7;
        pdVar7->b_syntax_indicator = true;
        pdVar7->b_private_indicator = false;
        pdVar7->i_extension = uVar4;
        pdVar7->i_length = 0xd;
        pdVar7->i_version = p_nit->i_version;
        _Var2 = p_nit->b_current_next;
        puVar8 = pdVar7->p_payload_end + 10;
        pdVar7->p_payload_end = puVar8;
        pdVar7->i_number = uVar1 + 1;
        pdVar7->b_current_next = _Var2;
        pdVar7->p_payload_start = pdVar7->p_data + 8;
      }
      *puVar8 = pdVar13->i_tag;
      pdVar7->p_payload_end[1] = pdVar13->i_length;
      memcpy(pdVar7->p_payload_end + 2,pdVar13->p_data,(size_t)pdVar13->i_length);
      puVar8 = pdVar7->p_payload_end + pdVar13->i_length + 2;
      pdVar7->p_payload_end = puVar8;
      puVar6 = &pdVar13->i_length;
      pdVar13 = pdVar13->p_next;
      pdVar7->i_length = pdVar7->i_length + 2 + (ushort)*puVar6;
      if (pdVar13 == (dvbpsi_descriptor_s *)0x0) break;
      puVar6 = pdVar7->p_data;
      pdVar14 = pdVar7;
    }
  }
  // Backfill network_descriptors_length (12 bits, reserved bits [15:12] = 0xf)
  puVar6 = pdVar7->p_payload_start;
  pdVar7->p_data[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff) >> 8) | 0xf0;
  pdVar7->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff);
  puVar8 = pdVar7->p_payload_end;    // p_transport_stream_loop_length (2 bytes)
  puVar6 = puVar8 + 2;
  pdVar7->p_payload_end = puVar6;
  pdVar14 = pdVar7;
  // Phase 2: serialize transport stream loop
  if (pdVar15 == (dvbpsi_nit_ts_t *)0x0) {
LAB_0041dbf4:
    uVar9 = (uint)(pdVar7->p_payload_end + (-2 - (int)puVar8)) & 0xffff;
    *puVar8 = (byte)(uVar9 >> 8) | 0xf0;
    puVar8[1] = (uint8_t)uVar9;
    pdVar14 = pdVar5;
    do {
      pdVar14->i_last_number = pdVar7->i_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar14);
      pdVar14 = pdVar14->p_next;
    } while (pdVar14 != (dvbpsi_psi_section_s *)0x0);
    return pdVar5;
  }
  do {
    pdVar13 = pdVar15->p_first_descriptor;
    pdVar7 = pdVar14;
    if (pdVar13 != (dvbpsi_descriptor_t *)0x0) {
      iVar12 = (int)puVar6 - (int)pdVar14->p_data;
      uVar9 = 5;
      if (iVar12 < 0x3f8) {
        bVar3 = pdVar13->i_length;
        while( true ) {
          pdVar13 = pdVar13->p_next;
          uVar9 = uVar9 + bVar3 + 2 & 0xffff;
          if (pdVar13 == (dvbpsi_descriptor_s *)0x0) break;
          if (0x3fc < (int)(uVar9 + iVar12)) {
            if ((iVar12 != 0xc) && (uVar9 < 0x3f1)) goto LAB_0041dc84;
            break;
          }
          bVar3 = pdVar13->i_length;
        }
      }
      else {
LAB_0041dc84:
        // Finalize transport_stream_loop_length on current section and open a new section
        *puVar8 = (byte)(((uint)(puVar6 + (-2 - (int)puVar8)) & 0xffff) >> 8) | 0xf0;
        puVar8[1] = (uint8_t)((uint)(puVar6 + (-2 - (int)puVar8)) & 0xffff);
        dvbpsi_debug(p_dvbpsi,"NIT generator","create a new section to carry more TS descriptors");
        pdVar7 = dvbpsi_NewPSISection(0x400);
        pdVar7->i_table_id = i_table_id;
        uVar4 = p_nit->i_network_id;
        pdVar14->p_next = pdVar7;
        pdVar7->b_syntax_indicator = true;
        pdVar7->b_private_indicator = false;
        pdVar7->i_length = 0xd;
        pdVar7->i_extension = uVar4;
        uVar1 = pdVar14->i_number;
        pdVar7->i_version = p_nit->i_version;
        _Var2 = p_nit->b_current_next;
        pdVar7->p_payload_end = pdVar7->p_payload_end + 10;
        pdVar7->b_current_next = _Var2;
        pdVar7->i_number = uVar1 + 1;
        pdVar7->p_payload_start = pdVar7->p_data + 8;
        pdVar7->p_data[8] = 0xf0;    // network_descriptors_length = 0 in continuation section
        pdVar7->p_data[9] = 0;
        puVar8 = pdVar7->p_payload_end;
        puVar6 = puVar8 + 2;
        pdVar7->p_payload_end = puVar6;
      }
    }
    *puVar6 = (uint8_t)(pdVar15->i_ts_id >> 8);
    puVar6[1] = (uint8_t)pdVar15->i_ts_id;
    puVar6[2] = (uint8_t)(pdVar15->i_orig_network_id >> 8);
    puVar6[3] = (uint8_t)pdVar15->i_orig_network_id;
    pdVar13 = pdVar15->p_first_descriptor;
    puVar11 = pdVar7->p_payload_end + 6;
    pdVar7->p_payload_end = puVar11;
    pdVar7->i_length = pdVar7->i_length + 6;
    if (pdVar13 == (dvbpsi_descriptor_t *)0x0) {
LAB_0041db7c:
      puVar6[4] = (byte)(((uint)(puVar11 + (-6 - (int)puVar6)) & 0xffff) >> 8) | 0xf0;
      puVar6[5] = (uint8_t)((uint)(puVar11 + (-6 - (int)puVar6)) & 0xffff);
      pdVar15 = pdVar15->p_next;
    }
    else {
      puVar10 = puVar11 + ((uint)pdVar13->i_length - (int)pdVar7->p_data);
      while ((int)puVar10 < 0x3fb) { // ensure room for descriptor + 4B CRC within 1024B
        *puVar11 = pdVar13->i_tag;
        pdVar7->p_payload_end[1] = pdVar13->i_length;
        memcpy(pdVar7->p_payload_end + 2,pdVar13->p_data,(size_t)pdVar13->i_length);
        puVar11 = pdVar7->p_payload_end + pdVar13->i_length + 2;
        pdVar7->p_payload_end = puVar11;
        puVar10 = &pdVar13->i_length;
        pdVar13 = pdVar13->p_next;
        pdVar7->i_length = pdVar7->i_length + 2 + (ushort)*puVar10;
        if (pdVar13 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041db7c;
        puVar10 = puVar11 + ((uint)pdVar13->i_length - (int)pdVar7->p_data);
      }
      dvbpsi_error(p_dvbpsi,"NIT generator","unable to carry all the TS descriptors");
      uVar9 = (uint)(pdVar7->p_payload_end + (-6 - (int)puVar6)) & 0xffff;
      puVar6[4] = (byte)(uVar9 >> 8) | 0xf0;
      puVar6[5] = (uint8_t)uVar9;
      pdVar15 = pdVar15->p_next;
    }
    if (pdVar15 == (dvbpsi_nit_ts_s *)0x0) goto LAB_0041dbf4;
    puVar6 = pdVar7->p_payload_end;
    pdVar14 = pdVar7;
  } while( true );
}


/* #####################################################################
 * FILE: 13_tables_tot.c.c
 * ##################################################################### */

/*
 * 13_tables_tot.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/tables/tot.c
 * ===================================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/tables/tot.c
 *                   (DWARF CU @ 0x41dd50..0x41eb8c, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x TDT/TOT API (dvbpsi_tot_*).
 *                   Note: in libdvbpsi 1.1.0, both DVB TDT (table_id 0x70, no
 *                   descriptors, no CRC-32) and TOT (table_id 0x73, descriptors
 *                   + CRC-32) are handled by tables/tot.c on PID 0x0014.
 *
 * Role in av_streamer:
 *   01_libxcode4encoder.c.c::genTDT() calls dvbpsi_tot_init(&tdt, 0x70, 0,
 *   0x10, false, utc_mjd), dvbpsi_tot_sections_generate(), and
 *   dvbpsi_tot_empty() to build a DVB TDT (table_id 0x70) section carried in
 *   TS PID 0x0014. The decoder side (attach/detach/gather/decode) is linked
 *   from libdvbpsi.a but unused by av_streamer.
 *
 * Main data structures (from DWARF):
 *   dvbpsi_tot_t (struct dvbpsi_tot_s, 17 bytes = 0x11, packed/unaligned):
 *     +0x00  uint8_t  i_table_id           (0x70 = TDT, 0x73 = TOT)
 *     +0x01  uint16_t i_extension          (unused for TDT/TOT, typically 0)
 *     +0x03  uint8_t  i_version
 *     +0x04  _Bool    b_current_next
 *     +0x05  uint64_t i_utc_time           (40-bit MJD + BCD HH:MM:SS in low 5 bytes)
 *     +0x0d  dvbpsi_descriptor_t *p_first_descriptor (unaligned at +13!)
 *
 *   dvbpsi_tot_decoder_t (60 bytes = 0x3c, inferred from dvbpsi_decoder_new(..., 0x3c)):
 *     +0x00..+0x1b  dvbpsi_decoder_t base (28 bytes)
 *     +0x1c         dvbpsi_tot_callback pf_tot_callback
 *     +0x20         void *p_cb_data
 *     +0x24..+0x34  dvbpsi_tot_t current_tot (17 bytes, padded to 20)
 *     +0x38         dvbpsi_tot_t *p_building_tot
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 tables/tot.c.
 */

/* dvbpsi_tot_attach @ 0x0041dd50
 * Purpose  : Attach a TDT/TOT subdecoder to an existing dvbpsi_demux_t on
 *            (i_table_id, 0). Allocates dvbpsi_tot_decoder_t (0x3c bytes) and
 *            registers dvbpsi_tot_sections_gather / dvbpsi_tot_detach.
 * Params   : p_dvbpsi    - handle with attached demuxer (asserted non-NULL)
 *            i_table_id  - 0x70 (TDT) or 0x73 (TOT)
 *            i_extension - ignored (0 is used for subdecoder key)
 *            pf_callback - user callback invoked when TDT/TOT is decoded
 *            p_cb_data   - opaque user data passed to pf_callback
 * Returns  : true on success, false if already attached or on OOM.
 * Callees  : __assert_fail, dvbpsi_demuxGetSubDec, dvbpsi_decoder_new,
 *            dvbpsi_NewDemuxSubDecoder, dvbpsi_AttachDemuxSubDecoder,
 *            dvbpsi_decoder_delete, dvbpsi_error
 */
// ==== dvbpsi_tot_attach @ 0041dd50 ====

_Bool dvbpsi_tot_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_tot_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1; // p_subdec
  dvbpsi_decoder_t *p_decoder;   // p_tot_decoder (dvbpsi_tot_decoder_t*, 0x3c bytes)
  dvbpsi_demux_t *p_demux;       // p_dvbpsi->p_decoder cast to dvbpsi_demux_t*
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x3d,"dvbpsi_tot_attach"); // "p_dvbpsi" (inferred)
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x3e,"dvbpsi_tot_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,0);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,0,dvbpsi_tot_detach,dvbpsi_tot_sections_gather,p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_tot_callback *)(p_decoder + 1) = pf_callback;             // +0x1c: pf_tot_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;               // +0x20: p_cb_data
        // Zero +0x38: p_building_tot = NULL (via overlaid dvbpsi_decoder_t[2] fields)
        p_decoder[2].i_magic[0] = 0;
        p_decoder[2].i_magic[1] = 0;
        p_decoder[2].i_magic[2] = 0;
        p_decoder[2].b_complete_header = false;
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,0);
  }
  return false;
}



/* dvbpsi_tot_init @ 0x0041def0
 * Purpose  : Initialize a caller-allocated dvbpsi_tot_t struct.
 * Params   : p_tot          - struct to initialize (asserted non-NULL)
 *            i_table_id     - 0x70 (TDT) or 0x73 (TOT)
 *            i_extension    - table_id_extension (0 for TDT/TOT)
 *            i_version      - version_number
 *            b_current_next - current_next_indicator
 *            i_utc_time     - 40-bit UTC time (16-bit MJD << 24 | 24-bit BCD HHMMSS)
 * Returns  : void
 * Callers  : genTDT (01_libxcode4encoder.c.c)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_tot_init @ 0041def0 ====

void dvbpsi_tot_init(dvbpsi_tot_t *p_tot,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint64_t i_utc_time)

{
  if (p_tot == (dvbpsi_tot_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_tot","tables/tot.c",0x94,"dvbpsi_tot_init");
  }
  p_tot->i_table_id = i_table_id;
  p_tot->i_extension = i_extension;
  p_tot->i_version = i_version;
  p_tot->b_current_next = b_current_next;
  p_tot->i_utc_time = i_utc_time;
  p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  return;
}



/* dvbpsi_tot_new @ 0x0041df80
 * Purpose  : Heap-allocate and initialize a dvbpsi_tot_t (0x11 = 17 bytes).
 * Params   : same as dvbpsi_tot_init()
 * Returns  : Newly allocated dvbpsi_tot_t *, or NULL on OOM.
 * Callers  : dvbpsi_tot_sections_gather
 * Callees  : malloc
 */
// ==== dvbpsi_tot_new @ 0041df80 ====

dvbpsi_tot_t *
dvbpsi_tot_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint64_t i_utc_time)

{
  dvbpsi_tot_t *pdVar1;      // allocated p_tot (0x11 = 17 bytes)
  
                    /* Unresolved local var: dvbpsi_tot_t * p_tot@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x11);
  if (pdVar1 != (dvbpsi_tot_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_utc_time = i_utc_time;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_tot_empty @ 0x0041e028
 * Purpose  : Free all descriptors attached to p_tot and reset
 *            p_tot->p_first_descriptor to NULL (without freeing p_tot itself).
 * Params   : p_tot - struct to empty
 * Returns  : void
 * Callers  : genTDT (01_libxcode4encoder.c.c)
 * Callees  : dvbpsi_DeleteDescriptors
 * Note     : Unaligned 32-bit load (lwl/lwr) because p_first_descriptor is at
 *            offset +0x0d in the 17-byte dvbpsi_tot_s struct.
 */
// ==== dvbpsi_tot_empty @ 0041e028 ====

void dvbpsi_tot_empty(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;        // end byte of p_tot->p_first_descriptor (+0x10)
  uint uVar2;                // byte alignment of end address
  uint uVar3;                // byte alignment of start address (&p_first_descriptor)
  
  puVar1 = (undefined1 *)((int)&p_tot->p_first_descriptor + 3);
  uVar2 = (uint)puVar1 & 3;
  uVar3 = (uint)&p_tot->p_first_descriptor & 3;
  // Reassemble unaligned 32-bit pointer p_tot->p_first_descriptor (+0x0d)
  dvbpsi_DeleteDescriptors
            ((dvbpsi_descriptor_t *)
             ((*(int *)(puVar1 + -uVar2) << (3 - uVar2) * 8 |
              (uint)p_tot & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
             *(uint *)((int)&p_tot->p_first_descriptor - uVar3) >> uVar3 * 8));
  p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  return;
}



/* dvbpsi_tot_delete @ 0x0041e06c
 * Purpose  : Empty p_tot's descriptor list (if p_tot != NULL) and free(p_tot).
 * Params   : p_tot - heap-allocated dvbpsi_tot_t * (may be NULL)
 * Returns  : void
 * Callers  : dvbpsi_ReInitTOT, dvbpsi_tot_detach
 * Callees  : dvbpsi_DeleteDescriptors, free
 */
// ==== dvbpsi_tot_delete @ 0041e06c ====

void dvbpsi_tot_delete(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;        // end byte of p_tot->p_first_descriptor
  uint uVar2;                // byte alignment of end address
  uint uVar3;                // byte alignment of start address
  
  if (p_tot != (dvbpsi_tot_t *)0x0) {
    puVar1 = (undefined1 *)((int)&p_tot->p_first_descriptor + 3);
    uVar2 = (uint)puVar1 & 3;
    uVar3 = (uint)&p_tot->p_first_descriptor & 3;
    dvbpsi_DeleteDescriptors
              ((dvbpsi_descriptor_t *)
               ((*(int *)(puVar1 + -uVar2) << (3 - uVar2) * 8 |
                (uint)p_tot & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
               *(uint *)((int)&p_tot->p_first_descriptor - uVar3) >> uVar3 * 8));
    p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  }
  free(p_tot);
  return;
}



/* dvbpsi_ReInitTOT @ 0x0041e0c0
 * Purpose  : Reset the TOT/TDT decoder's section list and clear p_building_tot
 *            (freeing p_building_tot first if b_force is true).
 * Params   : p_decoder - TOT decoder (asserted non-NULL)
 *            b_force   - true to invalidate current table and free p_building_tot
 * Returns  : void
 * Callers  : dvbpsi_tot_sections_gather
 * Callees  : __assert_fail, dvbpsi_decoder_reset, dvbpsi_tot_delete
 */
// ==== dvbpsi_ReInitTOT @ 0041e0c0 ====

void dvbpsi_ReInitTOT(dvbpsi_tot_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015; // MIPS ABI upper 24 bits of _Bool arg $a1
  
  if (p_decoder == (dvbpsi_tot_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/tot.c",0xe0,"dvbpsi_ReInitTOT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_tot != (dvbpsi_tot_t *)0x0)) {
    dvbpsi_tot_delete(p_decoder->p_building_tot);
  }
  p_decoder->p_building_tot = (dvbpsi_tot_t *)0x0;
  return;
}



/* dvbpsi_tot_detach @ 0x0041e14c
 * Purpose  : Detach and free the TDT/TOT subdecoder for (i_table_id, 0).
 * Params   : p_dvbpsi    - handle with attached demuxer (asserted non-NULL)
 *            i_table_id  - 0x70 (TDT) or 0x73 (TOT)
 *            i_extension - ignored (0 is used for subdecoder key)
 * Returns  : void
 * Callers  : dvbpsi_DetachDemux (via p_subdec->pf_detach)
 * Callees  : __assert_fail, dvbpsi_demuxGetSubDec, dvbpsi_tot_delete,
 *            dvbpsi_DetachDemuxSubDecoder, dvbpsi_DeleteDemuxSubDecoder
 */
// ==== dvbpsi_tot_detach @ 0041e14c ====

void dvbpsi_tot_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec; // subdecoder node for (i_table_id, 0)
  dvbpsi_demux_t *p_demux;         // p_dvbpsi->p_decoder cast to dvbpsi_demux_t*
  dvbpsi_decoder_t *pdVar1;        // p_tot_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x6f,"dvbpsi_tot_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x70,"dvbpsi_tot_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,0);
  pdVar1 = p_subdec->p_decoder;
  if (pdVar1 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec->p_decoder","tables/tot.c",0x80,"dvbpsi_tot_detach");
  }
  if (*(dvbpsi_tot_t **)(pdVar1 + 2) != (dvbpsi_tot_t *)0x0) { // p_building_tot (+0x38)
    dvbpsi_tot_delete(*(dvbpsi_tot_t **)(pdVar1 + 2));
  }
  pdVar1[2].i_magic[0] = 0;
  pdVar1[2].i_magic[1] = 0;
  pdVar1[2].i_magic[2] = 0;
  pdVar1[2].b_complete_header = false;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_tot_descriptor_add @ 0x0041e258
 * Purpose  : Create a descriptor and append it to p_tot->p_first_descriptor.
 * Params   : p_tot    - target TOT struct
 *            i_tag    - descriptor_tag (e.g. 0x58 local_time_offset_descriptor)
 *            i_length - descriptor_length
 *            p_data   - descriptor payload bytes
 * Returns  : Newly allocated dvbpsi_descriptor_t *, or NULL on OOM.
 * Callers  : dvbpsi_tot_sections_decode
 * Callees  : dvbpsi_NewDescriptor, dvbpsi_AddDescriptor, __assert_fail
 */
// ==== dvbpsi_tot_descriptor_add @ 0041e258 ====

dvbpsi_descriptor_t *
dvbpsi_tot_descriptor_add(dvbpsi_tot_t *p_tot,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly created descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated p_tot->p_first_descriptor head
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_tot->p_first_descriptor,p_descriptor);
    p_tot->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_tot->p_first_descriptor","tables/tot.c",0xd6,"dvbpsi_tot_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_tot_sections_decode @ 0x0041e2f8
 * Purpose  : Decode a TDT (0x70) or TOT (0x73) PSI section into p_tot:
 *            validates TDT length == 5, extracts 40-bit UTC_time (MJD+BCD),
 *            and for TOT parses the 12-bit descriptors_loop_length and
 *            attaches each descriptor via dvbpsi_tot_descriptor_add().
 * Params   : p_dvbpsi  - handle for error reporting
 *            p_tot     - destination TOT/TDT struct
 *            p_section - raw PSI section
 * Returns  : void
 * Quirks   : Upstream libdvbpsi 1.1.0 bug at line 252: dvbpsi_error() format
 *            string has "%d" but p_section->i_length argument is missing.
 * Callers  : dvbpsi_tot_sections_gather
 * Callees  : dvbpsi_error, dvbpsi_tot_descriptor_add
 */
// ==== dvbpsi_tot_sections_decode @ 0041e2f8 ====

void dvbpsi_tot_sections_decode
               (dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                // scratch byte (UTC MSB / loop_length MSB)
  byte bVar2;                // loop_length LSB
  uint8_t uVar3;             // p_section->i_table_id
  byte *pbVar4;              // p_section->p_payload_end
  byte i_tag;                // descriptor_tag
  uint8_t *p_data;           // descriptor payload pointer
  byte *pbVar5;              // p_byte cursor
  byte *pbVar6;              // descriptor loop cursor
  uint uVar7;                // descriptor_length
  
  if (p_section != (dvbpsi_psi_section_t *)0x0) {
    uVar3 = p_section->i_table_id;
                    /* Unresolved local var: uint8_t * p_byte@[???] */
    if ((uVar3 == 0x70) && (p_section->i_length != 5)) { // TDT payload is strictly 5 bytes (UTC_time)
      dvbpsi_error(p_dvbpsi,"TDT decoder","TDT has an invalid payload size (%d bytes) !!!");
      return;
    }
    pbVar5 = p_section->p_payload_start;
    pbVar4 = p_section->p_payload_end;
    if (pbVar5 + 5 <= pbVar4) {
      // Unpack 40-bit big-endian UTC_time into 64-bit LE p_tot->i_utc_time
      bVar1 = *pbVar5;
      *(uint *)&p_tot->i_utc_time =
           (uint)pbVar5[4] | (uint)pbVar5[1] << 0x18 | (uint)pbVar5[2] << 0x10 |
           (uint)pbVar5[3] << 8;
      *(uint *)((int)&p_tot->i_utc_time + 4) = (uint)bVar1;
      uVar3 = p_section->i_table_id;
      pbVar5 = pbVar5 + 5;
    }
    p_data = pbVar5 + 4;
    if (uVar3 == 0x73) {     // TOT: 12-bit descriptors_loop_length + descriptors
                    /* Unresolved local var: uint16_t i_loop_length@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
      bVar1 = *pbVar5;
      bVar2 = pbVar5[1];
      pbVar6 = pbVar5 + 2;
      if (p_data <= pbVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2)) {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        uVar7 = (uint)pbVar5[3];
        i_tag = *pbVar6;
        if ((int)(uVar7 + 1) < (int)pbVar4 - (int)pbVar6) goto LAB_0041e434;
        while( true ) {
          pbVar6 = pbVar6 + uVar7 + 2;
          p_data = pbVar6 + 2;
          if (pbVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2) < p_data) break;
          uVar7 = (uint)pbVar6[1];
          i_tag = *pbVar6;
          if ((int)(uVar7 + 1) < (int)p_section->p_payload_end - (int)pbVar6) {
LAB_0041e434:
            dvbpsi_tot_descriptor_add(p_tot,i_tag,(uint8_t)uVar7,p_data);
          }
        }
      }
    }
  }
  return;
}



/* dvbpsi_tot_sections_gather @ 0x0041e484
 * Purpose  : Demuxer gather callback for TDT (0x70) and TOT (0x73) sections.
 *            Validates section, handles continuity/version/extension changes,
 *            queues section, and when complete decodes and invokes
 *            pf_tot_callback(p_cb_data, p_building_tot).
 * Params   : p_dvbpsi  - handle with attached demuxer (asserted non-NULL)
 *            p_decoder - dvbpsi_tot_decoder_t* cast to dvbpsi_decoder_t*
 *            p_section - completed PSI section
 * Returns  : void
 * Callees  : __assert_fail, dvbpsi_CheckPSISection, dvbpsi_error,
 *            dvbpsi_ReInitTOT, dvbpsi_tot_new, dvbpsi_DeletePSISections,
 *            dvbpsi_decoder_psi_section_add, dvbpsi_debug,
 *            dvbpsi_decoder_psi_sections_completed, dvbpsi_tot_sections_decode,
 *            (*pf_tot_callback)
 */
// ==== dvbpsi_tot_sections_gather @ 0041e484 ====

void dvbpsi_tot_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  uint8_t table_id;          // p_section->i_table_id
  _Bool _Var1;               // scratch bool
  undefined3 extraout_var;   // MIPS ABI upper 24 bits of _Bool return
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;                 // pf_tot_callback check
  int iVar3;                 // CheckPSISection result / p_building_tot
  undefined3 extraout_var_02;
  uint8_t *puVar4;           // p_section->p_payload_start (for 40-bit UTC_time)
  dvbpsi_callback_gather_t p_Var5; // copy temp for current_tot
  dvbpsi_psi_section_t *pdVar6;    // copy temp for current_tot
  undefined4 uVar7;          // p_cb_data (+0x20)
  dvbpsi_tot_t *pdVar8;      // p_building_tot (+0x38)
  undefined4 uVar9;          // p_building_tot passed to pf_tot_callback
  dvbpsi_psi_section_t *p_section_00; // p_decoder->p_sections
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x139,"dvbpsi_tot_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x13a,"dvbpsi_tot_sections_gather");
  }
  table_id = p_section->i_table_id;
  if ((table_id == 0x70) || (table_id == 0x73)) {
    _Var1 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"TDT/TOT decoder");
    iVar3 = CONCAT31(extraout_var_02,_Var1);
  }
  else {
    _Var1 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x70,"TDT/TOT decoder");
    iVar3 = CONCAT31(extraout_var,_Var1);
  }
  if (iVar3 == 0) goto LAB_0041e69c;
  if (p_decoder->b_discontinuity == false) {
    iVar3 = *(int *)(p_decoder + 2); // p_building_tot (+0x38)
    if (iVar3 != 0) {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (p_section->i_extension == *(uint16_t *)(iVar3 + 1)) {
        if (*(uint8_t *)(iVar3 + 3) == p_section->i_version) {
          if (p_decoder->i_last_section_number == p_section->i_last_number) goto LAB_0041e514;
          dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                       "\'version_number\' differs whereas no discontinuity has occured");
        }
      }
      else {
        dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                     "\'transport_stream_id\' differs whereas no TS discontinuity has occured");
      }
      dvbpsi_ReInitTOT((dvbpsi_tot_decoder_t *)p_decoder,true);
      goto LAB_0041e508;
    }
  }
  else {
    p_decoder->b_discontinuity = false;
LAB_0041e508:
                    /* Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
    iVar3._0_1_ = p_decoder[2].i_magic[0];
    iVar3._1_1_ = p_decoder[2].i_magic[1];
    iVar3._2_1_ = p_decoder[2].i_magic[2];
    iVar3._3_1_ = p_decoder[2].b_complete_header;
    if (iVar3 != 0) goto LAB_0041e514;
  }
  puVar4 = p_section->p_payload_start;
  pdVar8 = dvbpsi_tot_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                          p_section->b_current_next,
                          (ulonglong)
                          CONCAT14(*puVar4,(uint)puVar4[4] | (uint)puVar4[1] << 0x18 |
                                           (uint)puVar4[2] << 0x10 | (uint)puVar4[3] << 8));
  *(dvbpsi_tot_t **)(p_decoder + 2) = pdVar8;
  if (pdVar8 == (dvbpsi_tot_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"TOT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041e69c:
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  p_decoder->i_last_section_number = p_section->i_last_number;
LAB_0041e514:
  _Var1 = dvbpsi_decoder_psi_section_add(p_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var1) != 0) {
    dvbpsi_debug(p_dvbpsi,"TOT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var1 = dvbpsi_decoder_psi_sections_completed(p_decoder);
  if (CONCAT31(extraout_var_01,_Var1) == 0) {
    return;
  }
  iVar2._0_1_ = p_decoder[1].i_magic[0];
  iVar2._1_1_ = p_decoder[1].i_magic[1];
  iVar2._2_1_ = p_decoder[1].i_magic[2];
  iVar2._3_1_ = p_decoder[1].b_complete_header;
  if (iVar2 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_tot_decoder->pf_tot_callback","tables/tot.c",0x178,"dvbpsi_tot_sections_gather"
                 );
  }
  // Copy *p_building_tot (17 bytes) into p_tot_decoder->current_tot (+0x24..+0x34)
  pdVar8 = *(dvbpsi_tot_t **)(p_decoder + 2);
  p_section_00 = p_decoder->p_sections;
  pdVar6 = *(dvbpsi_psi_section_t **)&pdVar8->b_current_next;
  p_Var5 = *(dvbpsi_callback_gather_t *)((int)&pdVar8->i_utc_time + 3);
  iVar3 = *(int *)((int)&pdVar8->i_utc_time + 7);
  p_decoder[1].p_current_section = *(dvbpsi_psi_section_t **)pdVar8;
  p_decoder[1].pf_gather = p_Var5;
  p_decoder[1].p_sections = pdVar6;
  p_decoder[1].i_section_max_size = iVar3;
  *(undefined1 *)&p_decoder[1].i_need = *(undefined1 *)((int)&pdVar8->p_first_descriptor + 3);
  p_decoder->b_current_valid = true;
  dvbpsi_tot_sections_decode(p_dvbpsi,pdVar8,p_section_00);
  dvbpsi_DeletePSISections(p_decoder->p_sections);
  uVar7._0_1_ = p_decoder[1].b_discontinuity;        // p_cb_data (+0x20)
  uVar7._1_1_ = p_decoder[1].b_current_valid;
  uVar7._2_1_ = p_decoder[1].i_continuity_counter;
  uVar7._3_1_ = p_decoder[1].i_last_section_number;
  uVar9._0_1_ = p_decoder[2].i_magic[0];             // p_building_tot (+0x38)
  uVar9._1_1_ = p_decoder[2].i_magic[1];
  uVar9._2_1_ = p_decoder[2].i_magic[2];
  uVar9._3_1_ = p_decoder[2].b_complete_header;
  p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_decoder + 1))(uVar7,uVar9);         // pf_tot_callback(p_cb_data, p_building_tot)
  dvbpsi_ReInitTOT((dvbpsi_tot_decoder_t *)p_decoder,false);
  return;
}



/* dvbpsi_tot_sections_generate @ 0x0041e808
 * Purpose  : Serialize a dvbpsi_tot_t into a single PSI section:
 *              - If p_first_descriptor == NULL: generates TDT (table_id 0x70,
 *                8-byte section: 3-byte header + 5-byte UTC_time, no CRC-32).
 *              - If p_first_descriptor != NULL: generates TOT (table_id 0x73,
 *                3-byte header + 5-byte UTC_time + 2-byte descriptors_loop_length
 *                + descriptors + 4-byte CRC-32).
 * Params   : p_dvbpsi - handle for error reporting
 *            p_tot    - source TDT/TOT struct
 * Returns  : Newly allocated dvbpsi_psi_section_t * (caller frees with
 *            dvbpsi_DeletePSISections).
 * Callers  : genTDT (01_libxcode4encoder.c.c)
 * Callees  : dvbpsi_NewPSISection, memcpy, dvbpsi_error, dvbpsi_BuildPSISection
 * Quirks   : Upstream libdvbpsi 1.1.0:
 *            - Line 476/495 writes descriptors_loop_length as 0xf0 | 8-bit
 *              (i_length - 7), truncating if descriptor loop > 255 bytes.
 *            - Line 505 dvbpsi_error() is missing the %d argument (i_length).
 */
// ==== dvbpsi_tot_sections_generate @ 0041e808 ====

dvbpsi_psi_section_t * dvbpsi_tot_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot)

{
  dvbpsi_psi_section_t *p_section; // p_result
  uint8_t *puVar1;                 // p_data / p_payload_end cursor
  uint8_t uVar2;                   // 0x70 (TDT) or 0x73 (TOT)
  uint8_t *puVar3;                 // section size check / &pdVar4->i_length
  dvbpsi_descriptor_t *pdVar4;     // p_descriptor cursor
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  pdVar4 = p_tot->p_first_descriptor;
  if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
    p_section = dvbpsi_NewPSISection(8);     // TDT: 3-byte header + 5-byte UTC_time
    uVar2 = 0x70;
  }
  else {
    p_section = dvbpsi_NewPSISection(0x1000); // TOT: up to 4096 bytes
    uVar2 = 0x73;
  }
  puVar1 = p_section->p_data;
  p_section->i_table_id = uVar2;
  p_section->b_syntax_indicator = false;     // short section syntax for both TDT and TOT
  p_section->b_private_indicator = false;
  p_section->i_length = 5;
  p_section->p_payload_start = puVar1 + 3;
  p_section->p_payload_end = puVar1 + 8;
  // Pack 40-bit UTC_time big-endian into p_data[3..7]
  puVar1[3] = (uint8_t)*(undefined4 *)((int)&p_tot->i_utc_time + 4);
  p_section->p_data[4] = (uint8_t)(p_tot->i_utc_time >> 0x18);
  p_section->p_data[5] = (uint8_t)(p_tot->i_utc_time >> 0x10);
  p_section->p_data[6] = (uint8_t)(p_tot->i_utc_time >> 8);
  p_section->p_data[7] = (uint8_t)p_tot->i_utc_time;
  if (p_section->i_table_id == 0x73) {
    // Reserve 2 bytes for descriptors_loop_length at p_payload_start[5..6]
    puVar1 = p_section->p_payload_end + 2;
    p_section->p_payload_end = puVar1;
    p_section->i_length = p_section->i_length + 2;
    if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
LAB_0041eb04:
      p_section->p_payload_start[5] = 0xf0;  // reserved(4)='1111' | descriptors_loop_length[11:8]=0
      p_section->p_payload_start[6] = (char)p_section->i_length - 7;
    }
    else {
      puVar3 = puVar1 + ((uint)pdVar4->i_length - (int)p_section->p_data);
      while ((int)puVar3 < 0xffb) {          // keep total <= 4091 (4095 - 4-byte CRC-32)
        *puVar1 = pdVar4->i_tag;
        p_section->p_payload_end[1] = pdVar4->i_length;
        memcpy(p_section->p_payload_end + 2,pdVar4->p_data,(size_t)pdVar4->i_length);
        puVar1 = p_section->p_payload_end + pdVar4->i_length + 2;
        p_section->p_payload_end = puVar1;
        puVar3 = &pdVar4->i_length;
        pdVar4 = pdVar4->p_next;
        p_section->i_length = p_section->i_length + 2 + (ushort)*puVar3;
        if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041eb04;
        puVar3 = puVar1 + ((uint)pdVar4->i_length - (int)p_section->p_data);
      }
      dvbpsi_error(p_dvbpsi,"TDT/TOT generator",
                   "TOT does not fit into one section as it ought to be !!!");
      p_section->p_payload_start[5] = 0xf0;
      p_section->p_payload_start[6] = (char)p_section->i_length - 7;
    }
  }
  dvbpsi_BuildPSISection(p_dvbpsi,p_section);
  if (p_section->i_table_id == 0x73) {
    // Account for 4-byte CRC-32 appended by dvbpsi_BuildPSISection()
    p_section->i_length = p_section->i_length + 4;
    p_section->p_payload_end = p_section->p_payload_end + 4;
    return p_section;
  }
  if ((p_section->i_table_id == 0x70) && (p_section->i_length != 5)) {
    dvbpsi_error(p_dvbpsi,"TDT decoder","TDT has an invalid payload size (%d bytes) !!!");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","********************************************");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","*  Generated TDT/TOT section is invalid.   *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","* THIS IS A BUG, PLEASE REPORT TO THE LIST *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","*  ---  libdvbpsi-devel@videolan.org  ---  *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","********************************************");
    return p_section;
  }
  return p_section;
}




/* #####################################################################
 * FILE: 14_demux.c.c
 * ##################################################################### */

/*
 * 14_demux.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/demux.c
 * ========================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/demux.c
 *                   (DWARF CU @ 0x41eb90..0x41f16c, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x subtable demuxer API
 *                   (dvbpsi_AttachDemux / dvbpsi_DetachDemux / subdecoder helpers).
 *
 * Role in av_streamer:
 *   Dispatches PSI sections arriving on a shared PID (e.g. DVB SDT/BAT PID
 *   0x0011, EIT PID 0x0012, TDT/TOT PID 0x0014) to registered subdecoders
 *   keyed by (table_id, table_id_extension). In av_streamer, table attach/
 *   detach functions (SDT, EIT, TOT) reference dvbpsi_demuxGetSubDec,
 *   dvbpsi_NewDemuxSubDecoder, dvbpsi_AttachDemuxSubDecoder, etc., though
 *   av_streamer itself only generates SI tables and does not run the demuxer.
 *
 * Main data structures (from DWARF):
 *   dvbpsi_demux_t (struct dvbpsi_demux_s, 40 bytes = 0x28):
 *     +0x00..+0x1b  dvbpsi_decoder_t base (28 bytes = 1 * sizeof(dvbpsi_decoder_t))
 *     +0x1c         dvbpsi_demux_subdec_t *p_first_subdec (Ghidra: *(pdVar3 + 1))
 *     +0x20         dvbpsi_demux_new_cb_t pf_new_callback (Ghidra: &pdVar3[1].b_discontinuity)
 *     +0x24         void *p_new_cb_data                   (Ghidra: pdVar3[1].p_current_section)
 *
 *   dvbpsi_demux_subdec_t (struct dvbpsi_demux_subdec_s, 20 bytes = 0x14):
 *     +0x00  uint32_t i_id                     (((uint32_t)i_table_id << 16) | i_extension)
 *     +0x04  dvbpsi_demux_gather_cb_t pf_gather
 *     +0x08  dvbpsi_decoder_t *p_decoder
 *     +0x0c  dvbpsi_demux_detach_cb_t pf_detach
 *     +0x10  dvbpsi_demux_subdec_s *p_next
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 demux.c.
 */

/* dvbpsi_Demux @ 0x0041eb90
 * Purpose  : Demuxer gather callback registered with dvbpsi_decoder_new().
 *            Looks up a subdecoder matching (p_section->i_table_id,
 *            p_section->i_extension). If none is found, invokes
 *            p_demux->pf_new_callback so the application can attach one, then
 *            retries the lookup; if still absent, frees p_section.
 * Params   : p_dvbpsi  - handle with attached dvbpsi_demux_t (asserted non-NULL)
 *            p_section - completed PSI section from dvbpsi_packet_push()
 * Returns  : void
 * Callers  : dvbpsi_packet_push (via p_decoder->pf_gather)
 * Callees  : __assert_fail, (*pf_new_callback), (*p_subdec->pf_gather),
 *            dvbpsi_DeletePSISections
 */
// ==== dvbpsi_Demux @ 0041eb90 ====

void dvbpsi_Demux(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint *puVar1;              // p_subdec (dvbpsi_demux_subdec_t* viewed as uint32_t*)
  uint uVar2;                // i_id = ((uint)i_table_id << 16) | i_extension
  dvbpsi_decoder_t *pdVar3;  // p_demux (dvbpsi_demux_t*, base at +0x00)
  code *UNRECOVERED_JUMPTABLE; // p_subdec->pf_gather
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x68,"dvbpsi_Demux"); // "p_dvbpsi" (inferred)
  }
  pdVar3 = p_dvbpsi->p_decoder;
  if (pdVar3 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x69,"dvbpsi_Demux");
  }
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  puVar1 = *(uint **)(pdVar3 + 1); // p_demux->p_first_subdec (+0x1c)
  if (puVar1 == (uint *)0x0) {
LAB_0041ec0c:
    // No matching subdecoder yet: notify user callback (+0x20 pf_new_callback, +0x24 p_new_cb_data)
    (**(code **)&pdVar3[1].b_discontinuity)
              (p_dvbpsi,p_section->i_table_id,p_section->i_extension,pdVar3[1].p_current_section);
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
    puVar1 = *(uint **)(pdVar3 + 1); // re-read p_first_subdec after callback may have attached one
    if (puVar1 == (uint *)0x0) {
LAB_0041ec64:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1]; // p_subdec->pf_gather (+0x04)
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4]; // p_subdec->p_next (+0x10)
      if (puVar1 == (uint *)0x0) goto LAB_0041ec64;
    } while (uVar2 != *puVar1);
  }
  else {
    // Search p_first_subdec list for matching (table_id, extension)
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4];
      if (puVar1 == (uint *)0x0) goto LAB_0041ec0c;
    } while (uVar2 != *puVar1);
  }
  UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
LAB_0041ec88:
                    /* WARNING: Could not recover jumptable at 0x0041eca4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  // Dispatch: p_subdec->pf_gather(p_dvbpsi, p_subdec->p_decoder, p_section)
  (*UNRECOVERED_JUMPTABLE)(p_dvbpsi,puVar1[2],p_section);
  return;
}



/* dvbpsi_AttachDemux @ 0x0041ecf0
 * Purpose  : Allocate a dvbpsi_demux_t (40 bytes, max section 4096 = 0x1000,
 *            discontinuity=true) and attach it to p_dvbpsi->p_decoder.
 * Params   : p_dvbpsi      - handle with no existing decoder (asserted)
 *            pf_new_cb     - callback invoked when an unknown subtable arrives
 *            p_new_cb_data - opaque user data passed to pf_new_cb
 * Returns  : true on success, false on OOM.
 * Callees  : __assert_fail, dvbpsi_decoder_new
 */
// ==== dvbpsi_AttachDemux @ 0041ecf0 ====

_Bool dvbpsi_AttachDemux(dvbpsi_t *p_dvbpsi,dvbpsi_demux_new_cb_t pf_new_cb,void *p_new_cb_data)

{
  dvbpsi_decoder_t *pdVar1;  // p_demux (dvbpsi_demux_t*)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x38,"dvbpsi_AttachDemux");
  }
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","demux.c",0x39,"dvbpsi_AttachDemux");
  }
  pdVar1 = dvbpsi_decoder_new(dvbpsi_Demux,0x1000,true,0x28); // 0x28 = sizeof(dvbpsi_demux_s)
  if (pdVar1 != (dvbpsi_decoder_t *)0x0) {
    // Zero p_demux->p_first_subdec (+0x1c, 4 bytes) via overlaid dvbpsi_decoder_t fields
    pdVar1[1].i_magic[0] = 0;
    pdVar1[1].i_magic[1] = 0;
    pdVar1[1].i_magic[2] = 0;
    pdVar1[1].b_complete_header = false;
    *(dvbpsi_demux_new_cb_t *)&pdVar1[1].b_discontinuity = pf_new_cb; // +0x20: pf_new_callback
    pdVar1[1].p_current_section = p_new_cb_data;                      // +0x24: p_new_cb_data
    p_dvbpsi->p_decoder = pdVar1;
  }
  return pdVar1 != (dvbpsi_decoder_t *)0x0;
}



/* dvbpsi_demuxGetSubDec @ 0x0041edc4
 * Purpose  : Linearly search p_demux->p_first_subdec for a subdecoder
 *            matching (i_table_id, i_extension).
 * Params   : p_demux     - demux instance
 *            i_table_id  - MPEG/DVB table_id
 *            i_extension - 16-bit table_id_extension
 * Returns  : Matching dvbpsi_demux_subdec_t *, or NULL if not found.
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 */
// ==== dvbpsi_demuxGetSubDec @ 0041edc4 ====

dvbpsi_demux_subdec_t *
dvbpsi_demuxGetSubDec(dvbpsi_demux_t *p_demux,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *pdVar1; // p_subdec cursor
  uint32_t uVar2;                // current p_subdec->i_id
  
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  pdVar1 = p_demux->p_first_subdec;
  if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
    uVar2 = pdVar1->i_id;
    while ((uVar2 != CONCAT12(i_table_id,i_extension) &&
           (pdVar1 = pdVar1->p_next, pdVar1 != (dvbpsi_demux_subdec_s *)0x0))) {
      uVar2 = pdVar1->i_id;
    }
  }
  return pdVar1;
}



/* dvbpsi_DetachDemux @ 0x0041ee10
 * Purpose  : Detach all attached subdecoders (calling pf_detach if non-NULL,
 *            otherwise freeing the subdecoder node), then delete the demux
 *            decoder and clear p_dvbpsi->p_decoder.
 * Params   : p_dvbpsi - handle with attached demuxer (asserted non-NULL)
 * Returns  : void
 * Callees  : __assert_fail, (*p_subdec->pf_detach), free, dvbpsi_decoder_delete
 */
// ==== dvbpsi_DetachDemux @ 0041ee10 ====

void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)

{
  dvbpsi_decoder_t *p_decoder; // p_demux (dvbpsi_demux_t*)
  uint *__ptr;                 // p_subdec (dvbpsi_demux_subdec_t* as uint*)
  uint *puVar1;                // p_subdec_temp (saved p_next before detach/free)
  code *pcVar2;                // p_subdec->pf_detach (+0x0c)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x87,"dvbpsi_DetachDemux");
  }
  p_decoder = p_dvbpsi->p_decoder;
  if (p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x88,"dvbpsi_DetachDemux");
  }
  __ptr = *(uint **)(p_decoder + 1); // p_demux->p_first_subdec (+0x1c)
  if (__ptr != (uint *)0x0) {
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec_temp@[???] */
    pcVar2 = (code *)__ptr[3]; // pf_detach
    puVar1 = (uint *)__ptr[4]; // p_next
    if (pcVar2 == (code *)0x0) goto LAB_0041ee84;
    // Call pf_detach(p_dvbpsi, i_table_id, i_extension) for each subdecoder
    while ((*pcVar2)(p_dvbpsi,(*__ptr & 0xffffff) >> 0x10,*__ptr & 0xffff), __ptr = puVar1,
          puVar1 != (uint *)0x0) {
      while( true ) {
        pcVar2 = (code *)__ptr[3];
        puVar1 = (uint *)__ptr[4];
        if (pcVar2 != (code *)0x0) break;
LAB_0041ee84:
        free(__ptr);
        __ptr = puVar1;
        if (puVar1 == (uint *)0x0) goto LAB_0041ee98;
      }
    }
LAB_0041ee98:
    p_decoder = p_dvbpsi->p_decoder;
  }
  dvbpsi_decoder_delete(p_decoder);
  p_dvbpsi->p_decoder = (dvbpsi_decoder_t *)0x0;
  return;
}



/* dvbpsi_NewDemuxSubDecoder @ 0x0041ef04
 * Purpose  : Allocate and populate a dvbpsi_demux_subdec_t (0x14 bytes) for
 *            a specific (i_table_id, i_extension) pair.
 * Params   : i_table_id  - table_id to match
 *            i_extension - table_id_extension to match
 *            pf_detach   - detach callback (asserted non-NULL)
 *            pf_gather   - section gather callback (asserted non-NULL)
 *            p_decoder   - table-specific decoder struct
 * Returns  : Newly allocated dvbpsi_demux_subdec_t *, or NULL on OOM.
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail, calloc
 */
// ==== dvbpsi_NewDemuxSubDecoder @ 0041ef04 ====

dvbpsi_demux_subdec_t *
dvbpsi_NewDemuxSubDecoder
          (uint8_t i_table_id,uint16_t i_extension,dvbpsi_demux_detach_cb_t pf_detach,
          dvbpsi_demux_gather_cb_t pf_gather,dvbpsi_decoder_t *p_decoder)

{
  dvbpsi_demux_subdec_t *pdVar1; // allocated subdec (0x14 = 20 bytes)
  
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: uint32_t i_id@[???] */
  if (pf_gather == (dvbpsi_demux_gather_cb_t)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("pf_gather","demux.c",0xa7,"dvbpsi_NewDemuxSubDecoder");
  }
  if (pf_detach == (dvbpsi_demux_detach_cb_t)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("pf_detach","demux.c",0xa8,"dvbpsi_NewDemuxSubDecoder");
  }
  pdVar1 = calloc(1,0x14);
  if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
    pdVar1->i_id = (uint)CONCAT12(i_table_id,i_extension);
    pdVar1->p_decoder = p_decoder;
    pdVar1->pf_gather = pf_gather;
    pdVar1->pf_detach = pf_detach;
  }
  return pdVar1;
}



/* dvbpsi_DeleteDemuxSubDecoder @ 0x0041efcc
 * Purpose  : Delete a subdecoder's underlying table decoder and free the
 *            dvbpsi_demux_subdec_t struct.
 * Params   : p_subdec - subdecoder to free (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_detach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail, dvbpsi_decoder_delete, free
 */
// ==== dvbpsi_DeleteDemuxSubDecoder @ 0041efcc ====

void dvbpsi_DeleteDemuxSubDecoder(dvbpsi_demux_subdec_t *p_subdec)

{
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xc0,"dvbpsi_DeleteDemuxSubDecoder");
  }
  dvbpsi_decoder_delete(p_subdec->p_decoder);
  free(p_subdec);
  return;
}



/* dvbpsi_AttachDemuxSubDecoder @ 0x0041f038
 * Purpose  : Prepend a subdecoder to p_demux->p_first_subdec.
 * Params   : p_demux  - demux instance (asserted non-NULL)
 *            p_subdec - subdecoder to attach (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_attach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_AttachDemuxSubDecoder @ 0041f038 ====

void dvbpsi_AttachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux","demux.c",0xcf,"dvbpsi_AttachDemuxSubDecoder");
  }
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xd0,"dvbpsi_AttachDemuxSubDecoder");
  }
  p_subdec->p_next = p_demux->p_first_subdec;
  p_demux->p_first_subdec = p_subdec;
  return;
}



/* dvbpsi_DetachDemuxSubDecoder @ 0x0041f0b4
 * Purpose  : Unlink p_subdec from p_demux->p_first_subdec (without freeing it).
 * Params   : p_demux  - demux instance (asserted non-NULL, list non-empty)
 *            p_subdec - subdecoder to unlink (asserted non-NULL)
 * Returns  : void
 * Callers  : table *_detach() functions (SDT, EIT, NIT, TOT)
 * Callees  : __assert_fail
 */
// ==== dvbpsi_DetachDemuxSubDecoder @ 0041f0b4 ====

void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  dvbpsi_demux_subdec_t *pdVar1;   // current node in list walk
  dvbpsi_demux_subdec_s **ppdVar2; // pp_prev_subdec: pointer to previous p_next (or head)
  
                    /* Unresolved local var: dvbpsi_demux_subdec_t * * pp_prev_subdec@[???] */
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux","demux.c",0xde,"dvbpsi_DetachDemuxSubDecoder");
  }
  pdVar1 = p_demux->p_first_subdec;
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_demux->p_first_subdec","demux.c",0xdf,"dvbpsi_DetachDemuxSubDecoder");
  }
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec","demux.c",0xe1,"dvbpsi_DetachDemuxSubDecoder");
  }
  ppdVar2 = &p_demux->p_first_subdec;
  for (; pdVar1 != p_subdec; pdVar1 = pdVar1->p_next) {
    ppdVar2 = &pdVar1->p_next;
  }
  *ppdVar2 = p_subdec->p_next;
  return;
}




/* #####################################################################
 * FILE: 15_descriptor.c.c
 * ##################################################################### */

/*
 * 15_descriptor.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/descriptor.c
 * ==================================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/descriptor.c
 *                   (DWARF CU @ 0x41f170..0x41f3dc, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x descriptor list API.
 *
 * Role in av_streamer:
 *   Allocates, appends, frees, and inspects MPEG/DVB PSI/SI descriptors
 *   (dvbpsi_descriptor_t). Used by all DVB SI table generators in av_streamer
 *   (NIT, SDT, EIT, TOT) via dvbpsi_NewDescriptor, dvbpsi_AddDescriptor,
 *   and dvbpsi_DeleteDescriptors.
 *
 * Main data structures (from DWARF):
 *   dvbpsi_descriptor_t (struct dvbpsi_descriptor_s, 16 bytes = 0x10):
 *     +0x00  uint8_t i_tag                  (descriptor_tag)
 *     +0x01  uint8_t i_length               (descriptor_length)
 *     +0x04  uint8_t *p_data                (raw payload bytes, length i_length)
 *     +0x08  dvbpsi_descriptor_s *p_next    (singly linked list next)
 *     +0x0c  void *p_decoded                (optional decoded descriptor struct)
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 descriptor.c.
 */

/* dvbpsi_IsDescriptorDecoded @ 0x0041f170
 * Purpose  : Check whether a descriptor has already been decoded into p_decoded.
 * Params   : p_descriptor - descriptor to test (assumed non-NULL)
 * Returns  : true if p_descriptor->p_decoded != NULL.
 */
// ==== dvbpsi_IsDescriptorDecoded @ 0041f170 ====

_Bool dvbpsi_IsDescriptorDecoded(dvbpsi_descriptor_t *p_descriptor)

{
  return p_descriptor->p_decoded != (void *)0x0;
}



/* dvbpsi_CanDecodeAsDescriptor @ 0x0041f17c
 * Purpose  : Check whether a descriptor is non-NULL and has the expected tag.
 * Params   : p_descriptor - descriptor to test (may be NULL)
 *            i_tag        - expected descriptor_tag
 * Returns  : true if p_descriptor != NULL && p_descriptor->i_tag == i_tag.
 */
// ==== dvbpsi_CanDecodeAsDescriptor @ 0041f17c ====

_Bool dvbpsi_CanDecodeAsDescriptor(dvbpsi_descriptor_t *p_descriptor,uint8_t i_tag)

{
  bool bVar1;
  
  bVar1 = false;
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    bVar1 = p_descriptor->i_tag == i_tag;
  }
  return bVar1;
}



/* dvbpsi_NewDescriptor @ 0x0041f19c
 * Purpose  : Allocate a dvbpsi_descriptor_t (0x10 bytes) and a backing p_data
 *            buffer of i_length bytes, copying p_data if non-NULL.
 * Params   : i_tag    - MPEG/DVB descriptor_tag (e.g. 0x40, 0x41, 0x48, 0x4d, 0x4e, 0x54, 0x5a, 0x6c)
 *            i_length - payload length in bytes (0..255)
 *            p_data   - source payload bytes to copy, or NULL
 * Returns  : Newly allocated dvbpsi_descriptor_t *, or NULL on OOM.
 * Callers  : dvbpsi_tot_descriptor_add (13_tables_tot.c.c), NIT/SDT/EIT descriptor_add
 * Callees  : malloc, free, memcpy
 */
// ==== dvbpsi_NewDescriptor @ 0041f19c ====

dvbpsi_descriptor_t * dvbpsi_NewDescriptor(uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *__ptr; // p_descriptor (0x10 bytes)
  uint8_t *__dest;            // allocated p_data buffer of i_length bytes
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  __ptr = malloc(0x10);
  if (__ptr != (dvbpsi_descriptor_t *)0x0) {
    __dest = malloc((size_t)i_length); // Note: if i_length==0, relies on libc malloc(0) != NULL
    __ptr->p_data = __dest;
    if (__dest == (uint8_t *)0x0) {
      free(__ptr);
      __ptr = (dvbpsi_descriptor_t *)0x0;
    }
    else {
      __ptr->i_tag = i_tag;
      __ptr->i_length = i_length;
      if (p_data != (uint8_t *)0x0) {
        memcpy(__dest,p_data,(size_t)i_length);
      }
      __ptr->p_decoded = (void *)0x0;
      __ptr->p_next = (dvbpsi_descriptor_s *)0x0;
    }
  }
  return __ptr;
}



/* dvbpsi_AddDescriptor @ 0x0041f268
 * Purpose  : Append p_descriptor to the tail of singly linked list p_list.
 * Params   : p_list       - head of existing descriptor list (may be NULL)
 *            p_descriptor - descriptor to append (asserted non-NULL)
 * Returns  : Head of the updated list (p_descriptor if p_list was NULL, else p_list).
 * Callers  : dvbpsi_tot_descriptor_add (13_tables_tot.c.c), NIT/SDT/EIT helpers
 * Callees  : __assert_fail
 */
// ==== dvbpsi_AddDescriptor @ 0041f268 ====

dvbpsi_descriptor_t *
dvbpsi_AddDescriptor(dvbpsi_descriptor_t *p_list,dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_t *pdVar1; // p_last cursor
  
  if (p_descriptor == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_descriptor","descriptor.c",0x79,"dvbpsi_AddDescriptor");
  }
  pdVar1 = p_list;
  if (p_list == (dvbpsi_descriptor_t *)0x0) {
    return p_descriptor;
  }
  while (pdVar1->p_next != (dvbpsi_descriptor_s *)0x0) {
                    /* Unresolved local var: dvbpsi_descriptor_t * p_last@[???] */
    pdVar1 = pdVar1->p_next;
  }
  pdVar1->p_next = p_descriptor;
  return p_list;
}



/* dvbpsi_DeleteDescriptors @ 0x0041f2e0
 * Purpose  : Free an entire singly linked list of dvbpsi_descriptor_t,
 *            including each node's p_data and p_decoded buffers.
 * Params   : p_descriptor - head of descriptor list (may be NULL)
 * Returns  : void
 * Callers  : dvbpsi_tot_empty (13_tables_tot.c.c), NIT/SDT/EIT empty/decode
 * Callees  : free
 */
// ==== dvbpsi_DeleteDescriptors @ 0041f2e0 ====

void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_s *pdVar1; // p_next
  
  while (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
                    /* Unresolved local var: dvbpsi_descriptor_t * p_next@[???] */
    pdVar1 = p_descriptor->p_next;
    if (p_descriptor->p_data != (uint8_t *)0x0) {
      free(p_descriptor->p_data);
    }
    if (p_descriptor->p_decoded != (void *)0x0) {
      free(p_descriptor->p_decoded);
    }
    free(p_descriptor);
    p_descriptor = pdVar1;
  }
  return;
}



/* dvbpsi_DuplicateDecodedDescriptor @ 0x0041f364
 * Purpose  : Allocate a zero-initialized copy of a decoded descriptor struct
 *            and memcpy i_size bytes from p_decoded.
 * Params   : p_decoded - source decoded struct (may be NULL)
 *            i_size    - byte size of decoded struct
 * Returns  : Newly allocated copy, or NULL if p_decoded == NULL or on OOM.
 * Callees  : calloc, memcpy
 */
// ==== dvbpsi_DuplicateDecodedDescriptor @ 0041f364 ====

void * dvbpsi_DuplicateDecodedDescriptor(void *p_decoded,ssize_t i_size)

{
  void *__dest;              // p_duplicate
  
                    /* Unresolved local var: void * p_duplicate@[???] */
  __dest = (void *)0x0;
  if (p_decoded != (void *)0x0) {
    __dest = calloc(1,i_size);
    if (__dest != (void *)0x0) {
      memcpy(__dest,p_decoded,i_size);
    }
  }
  return __dest;
}




/* #####################################################################
 * FILE: 16_elf-init.c.c
 * ##################################################################### */

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




/* #####################################################################
 * FILE: 17_stat64.c.c
 * ##################################################################### */

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




/* #####################################################################
 * FILE: 18_nocu.c
 * ##################################################################### */

/*
 * 18_nocu.c  --  CRT startup/teardown & ctor/dtor glue (outside DWARF CUs)
 * ========================================================================
 *
 * Functions outside any DWARF compilation unit range: ELF entry (__start),
 * .init/_init, .fini/_fini, and GCC crtstuff (__do_global_ctors_aux,
 * __do_global_dtors_aux, frame_dummy).
 */

/*
 * _init @ 004022e8
 *
 * ELF .init entry point. Calls __gmon_start__ (if profiling), frame_dummy
 * (Java/EH frame registration), and __do_global_ctors_aux (C++ static ctors).
 */
// ==== _init @ 004022e8 ====

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  __gmon_start__();
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



/*
 * __start @ 00402360
 *
 * ELF e_entry. Sets up stack frame and calls __libc_start_main(main, argc,
 * argv, __libc_csu_init, ...). Never returns.
 */
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



/*
 * __do_global_dtors_aux @ 004023c0
 *
 * GCC crtstuff destructor runner, guarded by completed_5354 so it runs once.
 */
// ==== __do_global_dtors_aux @ 004023c0 ====

/* WARNING: Removing unreachable block (ram,0x00402414) */

void __do_global_dtors_aux(void)

{
  if (completed_5354 == '\0') {
    completed_5354 = '\x01';
  }
  return;
}



/*
 * frame_dummy @ 0040245c
 *
 * Registers JCR (Java Class Registration) table if __JCR_END__ != 0.
 */
// ==== frame_dummy @ 0040245c ====

void frame_dummy(void)

{
  if (__JCR_END__ == 0) {
    return;
  }
  _Jv_RegisterClasses(&__JCR_END__);
  return;
}



/*
 * __do_global_ctors_aux @ 0041f4c0
 *
 * Walks backwards through __CTOR_LIST__ (sentinel 0xffffffff) invoking each
 * global C++ constructor (e.g. std::ios_base::Init or static objects).
 */
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



/*
 * _fini @ 0041fa00
 *
 * ELF .fini entry point. Runs __do_global_dtors_aux().
 */
// ==== _fini @ 0041fa00 ====

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}



