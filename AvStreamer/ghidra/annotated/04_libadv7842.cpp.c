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


