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


