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
