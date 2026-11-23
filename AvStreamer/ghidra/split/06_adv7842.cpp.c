// ==== ADV7842_t::ADV7842_t @ 004138a0 ====

/* DWARF original prototype: void ADV7842_t(ADV7842_t * this) */

void __thiscall ADV7842_t::ADV7842_t(ADV7842_t *this)

{
  xcode_io_t *pxVar1;
  uchar local_10 [8];
  
                    /* Unresolved local var: uchar bTemp@[???] */
  this->m_I2CChannel = 1;
  pxVar1 = xcode_io_t::get_io();
  this->xcodeIO = pxVar1;
  if (adv7842_DebugDump == 1) {
    puts("//====== Start ADV7842_t ======");
  }
  read_register(this,0x20,0xfb,local_10);
  if (local_10[0] != 'h') {
    InitHDMI(this);
  }
  return;
}



// ==== ADV7842_t::ADV7842_t @ 00413974 ====

/* DWARF original prototype: void ADV7842_t(ADV7842_t * this, int i2cChannel) */

void __thiscall ADV7842_t::ADV7842_t(ADV7842_t *this,int i2cChannel)

{
  xcode_io_t *pxVar1;
  uchar local_10 [8];
  
                    /* Unresolved local var: uchar bTemp@[???] */
  this->m_I2CChannel = i2cChannel;
  pxVar1 = xcode_io_t::get_io();
  this->xcodeIO = pxVar1;
  this->m_InputSource = XC_INPUT_NONE;
  sprintf(log_buff,"adv7842.cpp : Select External Audio - Setting GPIO 5 to LOW, SigTerm = %d",
          SigTerm);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1100,0x20,0);
  xcode_io_t::write_reg_mask_bits(this->xcodeIO,0x1104,0x20,0x20);
  if (adv7842_DebugDump == 1) {
    puts("//====== Start ADV7842_t ======");
  }
  read_register(this,0x20,0xfb,local_10);
  if (local_10[0] != 'h') {
    InitHDMI(this);
  }
  return;
}



// ==== ADV7842_t::~ADV7842_t @ 00413b20 ====

/* DWARF original prototype: void ~ADV7842_t(ADV7842_t * this, int __in_chrg) */

void __thiscall ADV7842_t::~ADV7842_t(ADV7842_t *this,int __in_chrg)

{
  return;
}



// ==== ADV7842_t::reset_ADV7842 @ 00413b44 ====

/* DWARF original prototype: void reset_ADV7842(ADV7842_t * this) */

void __thiscall ADV7842_t::reset_ADV7842(ADV7842_t *this)

{
  return;
}



// ==== ADV7842_t::get_current_input_source @ 00413b68 ====

/* DWARF original prototype: enum_xc_input_type_t get_current_input_source(ADV7842_t * this) */

enum_xc_input_type_t __thiscall ADV7842_t::get_current_input_source(ADV7842_t *this)

{
  return this->m_InputSource;
}



// ==== ADV7842_t::set_free_run_mode @ 00413b94 ====

/* DWARF original prototype: int set_free_run_mode(ADV7842_t * this, enum_xc_free_run_mode_t
   FreeRunMode) */

int __thiscall ADV7842_t::set_free_run_mode(ADV7842_t *this,enum_xc_free_run_mode_t FreeRunMode)

{
  uint uVar1;
  uint local_10;
  uchar local_c;
  byte local_b [3];
  
                    /* Unresolved local var: uint i2cAddress@[???]
                       Unresolved local var: uchar regAddr@[???]
                       Unresolved local var: uchar regData@[???] */
  if (this->m_InputSource < XC_INPUT_HDMI_COMPONENT_576P) {
    uVar1 = 1 << (this->m_InputSource & XC_INPUT_HDMI_COMPONENT_720P);
    if ((uVar1 & 0x15400000) != 0) {
      local_10 = 0x48;
      local_c = 0xdd;
      read_register(this,0x48,0xdd,local_b);
      if (FreeRunMode == XC_FREE_RUN_BLUE_SCREEN) {
        local_b[0] = local_b[0] & 0xfc | 1;
      }
      else if (FreeRunMode != XC_FREE_RUN_COLOR_BAR) {
        local_b[0] = local_b[0] & 0xfc;
      }
      goto LAB_00413d0c;
    }
    if ((uVar1 & 1) != 0) goto LAB_00413d0c;
  }
  local_10 = 0x22;
  local_c = 0xbf;
  read_register(this,0x22,0xbf,local_b);
  if (FreeRunMode - XC_FREE_RUN_BLUE_SCREEN < 2) {
    local_b[0] = local_b[0] | 1;
  }
  else {
    local_b[0] = local_b[0] & 0xfe;
  }
LAB_00413d0c:
  write_register(this,local_10,local_c,local_b[0]);
  return 0;
}



// ==== ADV7842_t::program_input_source @ 00413d48 ====

/* DWARF original prototype: int program_input_source(ADV7842_t * this, enum_xc_input_type_t
   InputSource) */

int __thiscall ADV7842_t::program_input_source(ADV7842_t *this,enum_xc_input_type_t InputSource)

{
  uchar local_10 [8];
  
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
    write_register(this,0x32,'w','\x0f');
    usleep(1000000);
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



// ==== ADV7842_t::set_video_standard @ 004141b0 ====

/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */
/* DWARF original prototype: int set_video_standard(ADV7842_t * this,
   enum_analog_digital_video_standard_t VideoStandard) */

int __thiscall
ADV7842_t::set_video_standard(ADV7842_t *this,enum_analog_digital_video_standard_t VideoStandard)

{
  this->m_VideoStandard = VideoStandard;
  return 0;
}



// ==== ADV7842_t::InitHDMI @ 004141e8 ====

/* DWARF original prototype: int InitHDMI(ADV7842_t * this) */

int __thiscall ADV7842_t::InitHDMI(ADV7842_t *this)

{
  if (adv7842_DebugDump == 1) {
    puts("//=== Initial Reset. Ignore this error message!");
  }
  write_register(this,0x20,0xff,0x80);
  usleep(300000);
  if (adv7842_DebugDump == 1) {
    puts("//=== HDMI Init Mode Power On, Initial Mapping");
  }
  write_register(this,0x20,0xf9,'d');
  write_register(this,0x20,0xfa,'l');
  write_register(this,0x20,0xf1,0x90);
  write_register(this,0x20,0xf2,0x94);
  write_register(this,0x20,0xf3,0x84);
  write_register(this,0x20,0xf4,0x88);
  write_register(this,0x20,0xf5,'|');
  write_register(this,0x20,0xf8,'L');
  write_register(this,0x20,0xfb,'h');
  write_register(this,0x20,0xfc,'`');
  write_register(this,0x20,0xfd,'D');
  write_register(this,0x20,0xfe,'H');
  return 0;
}



// ==== ADV7842_t::InitMode @ 0041440c ====

/* DWARF original prototype: int InitMode(ADV7842_t * this) */

int __thiscall ADV7842_t::InitMode(ADV7842_t *this)

{
  if (adv7842_DebugDump == 1) {
    puts("//=== Initial Reset. Ignore this error message!");
  }
  write_register(this,0x20,0xff,0x80);
  usleep(300000);
  memcpy(log_buff,"adv7842.cpp : Analog Init Mode Power On, Initial Mapping",0x39);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  write_register(this,0x20,0xf1,0x90);
  write_register(this,0x20,0xf2,0x94);
  write_register(this,0x20,0xf3,0x84);
  write_register(this,0x20,0xf4,0x88);
  write_register(this,0x20,0xf5,'|');
  write_register(this,0x20,0xf8,'L');
  write_register(this,0x20,0xf9,'d');
  write_register(this,0x20,0xfa,'l');
  write_register(this,0x20,0xfb,'h');
  write_register(this,0x20,0xfd,'D');
  write_register(this,0x20,0xfe,'H');
  return 0;
}



// ==== ADV7842_t::LoadEDID @ 00414650 ====

/* DWARF original prototype: void LoadEDID(ADV7842_t * this) */

void __thiscall ADV7842_t::LoadEDID(ADV7842_t *this)

{
  byte local_10 [8];
  
                    /* Unresolved local var: uchar bTemp@[???] */
  read_register(this,0x34,'i',local_10);
  sprintf(log_buff,"adv7842.cpp : HPA_AUTO_INT_EDID is %x",(uint)local_10[0]);
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
  LoadHDMI(this,'\a');
  memcpy(log_buff,"adv7842.cpp : Check Loading EDID",0x21);
  puts(log_buff);
  syslog(5,"%s",log_buff);
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
    read_register(this,0x36,0xff,local_10);
    sprintf(log_buff,"adv7842.cpp : EDID checksum = 0x%x",(uint)local_10[0]);
    puts(log_buff);
    syslog(5,"%s",log_buff);
  }
  return;
}



// ==== ADV7842_t::LoadEDID @ 00414a8c ====

/* DWARF original prototype: void LoadEDID(ADV7842_t * this, char * fileName) */

void __thiscall ADV7842_t::LoadEDID(ADV7842_t *this,char *fileName)

{
  FILE *__stream;
  char *pcVar1;
  int local_128;
  byte local_11c [4];
  uint local_118;
  uint local_114;
  uint local_110;
  char acStack_10c [260];
  
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
    LoadHDMI(this,'\a');
    memcpy(log_buff,"adv7842.cpp : Check Loading EDID",0x21);
    puts(log_buff);
    syslog(5,"%s",log_buff);
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



// ==== ADV7842_t::CheckEDID @ 00415214 ====

/* DWARF original prototype: void CheckEDID(ADV7842_t * this) */

void __thiscall ADV7842_t::CheckEDID(ADV7842_t *this)

{
  byte local_10 [8];
  
                    /* Unresolved local var: uchar bTemp@[???] */
  read_register(this,0x36,0xff,local_10);
  sprintf(log_buff,"adv7842.cpp : EDID checksum = 0x%x",(uint)local_10[0]);
  puts(log_buff);
  syslog(5,"%s",log_buff);
  return;
}



// ==== ADV7842_t::write_register @ 004152dc ====

/* DWARF original prototype: int write_register(ADV7842_t * this, uint ADV7842_I2CAddress, uchar
   Register, uchar Data) */

int __thiscall
ADV7842_t::write_register(ADV7842_t *this,uint ADV7842_I2CAddress,uchar Register,uchar Data)

{
  int iVar1;
  uchar local_c;
  uchar uStack_b;
  
                    /* Unresolved local var: ADV7842_command_t TempData@[???]
                       Unresolved local var: int retcode@[???] */
  _local_c = CONCAT11(Data,Register);
  iVar1 = xcode_io_t::i2c_write
                    (this->xcodeIO,this->m_I2CChannel,ADV7842_I2CAddress << 1,&local_c,2,1,1);
  if (adv7842_DebugDump == 1) {
    printf("//=== write_register i2c 0x%02x reg 0x%02x data 0x%02x   m_I2CChannel=0x%x\n",
           ADV7842_I2CAddress << 1,(uint)Register,(uint)Data,this->m_I2CChannel);
  }
  return iVar1;
}



// ==== ADV7842_t::read_register @ 004153f0 ====

/* DWARF original prototype: int read_register(ADV7842_t * this, uint ADV7842_I2CAddress, uchar
   Register, uchar * Data) */

int __thiscall
ADV7842_t::read_register(ADV7842_t *this,uint ADV7842_I2CAddress,uchar Register,uchar *Data)

{
  int iVar1;
  ushort local_c [2];
  
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



// ==== ADV7842_t::LoadHDMI @ 00415500 ====

/* DWARF original prototype: void LoadHDMI(ADV7842_t * this, uchar input) */

void __thiscall ADV7842_t::LoadHDMI(ADV7842_t *this,uchar input)

{
  byte bVar1;
  uchar Register;
  uchar uVar2;
  uchar uVar3;
  uchar uVar4;
  bool bVar5;
  bool bVar6;
  uint uVar7;
  int local_28;
  uchar *local_1c;
  uchar local_c [4];
  
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
  case '\a':
    local_1c = adv7842load_EDID_tab;
    bVar6 = true;
    memcpy(log_buff,"adv7842.cpp : Load EDID into SPI EEPROM",0x28);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    break;
  case '\x12':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_S_VIDEO_480I",0x24);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_S_Video_tab;
    break;
  case '\x16':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPOSITE_480I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_Composite_tab;
    break;
  case '\x18':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPOSITE_576I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_Composite_576i_tab;
    break;
  case '\x1a':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_480I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_480i_10b_HDMI_tab;
    break;
  case '\x1b':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_480P_HD",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_480p_24b_422_HDMI_tab;
    break;
  case '\x1c':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_576I",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_PAL_576i_10b_HDMI_tab;
    break;
  case '\x1d':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_576P_HD",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_576p_24b_422_HDMI_tab;
    break;
  case '\x1f':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_720P",0x26);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_720p_24b_422_HDMI_tab;
    break;
  case '!':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_720P_50",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_720p_50HZ_24b_422_HDMI_tab;
    break;
  case '\"':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080I",0x27);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080i_24b_422_HDMI_tab;
    break;
  case '#':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080P",0x27);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080p_24b_422_HDMI_tab;
    break;
  case '$':
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_COMPONENT_1080I_50",0x2a);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    local_1c = adv7842mode_YPbPr_1080i_50HZ_24b_422_HDMI_tab;
    break;
  case '(':
  case '+':
  case ',':
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_tab;
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
  case ')':
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_480i_tab;
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI_480I",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
    break;
  case '*':
    local_1c = adv7842mode_HDMI_YPbPr_HDMI_576i_tab;
    memcpy(log_buff,"adv7842.cpp : XC_INPUT_HDMI_TO_HDMI_576I",0x29);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    bVar5 = true;
    break;
  case '-':
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



// ==== ADV7842_t::detect_audio_mode @ 00416224 ====

/* DWARF original prototype: int detect_audio_mode(ADV7842_t * this, uint * bCompressed) */

int __thiscall ADV7842_t::detect_audio_mode(ADV7842_t *this,uint *bCompressed)

{
  byte local_10;
  byte local_f [7];
  
                    /* Unresolved local var: uchar value@[???]
                       Unresolved local var: uchar value39@[???] */
  *bCompressed = 0;
  if ((((this->m_InputSource == XC_INPUT_HDMI_TO_HDMI) ||
       (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_480I)) ||
      (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_576I)) ||
     (((this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_480P ||
       (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_576P)) ||
      (this->m_InputSource == XC_INPUT_HDMI_TO_HDMI_1080P)))) {
    read_register(this,0x34,'\x18',&local_10);
    sprintf(log_buff,"adv7842.cpp : HDMI HDMI 0x18 is %x",(uint)local_10);
    puts(log_buff);
    syslog(5,"%s",log_buff);
    if ((local_10 & 1) != 0) {
      read_register(this,0x34,'6',&local_10);
      sprintf(log_buff,"adv7842.cpp : HDMI HDMI 0x36 is %x",(uint)local_10);
      puts(log_buff);
      syslog(5,"%s",log_buff);
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



// ==== ADV7842_t::detect_video_standard @ 004164e0 ====

/* WARNING: Enum "enum_analog_digital_video_standard_t": Some values do not have unique names */
/* DWARF original prototype: int detect_video_standard(ADV7842_t * this,
   enum_analog_digital_video_standard_t * VideoStandard) */

int __thiscall
ADV7842_t::detect_video_standard
          (ADV7842_t *this,enum_analog_digital_video_standard_t *VideoStandard)

{
  byte bVar1;
  byte bVar2;
  ushort uVar3;
  int local_20;
  int local_1c;
  byte local_10 [8];
  
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
    if (adv7842_DebugDump == 1) {
      puts("wait 1 second ");
    }
    usleep(500000);
    usleep(1000000);
    *VideoStandard = ANALOG_DIGITAL_VIDEO_STANDARD_NONE;
    read_register(this,0x48,'\0',local_10);
    read_register(this,0x48,'Z',local_10);
    sprintf(curr_vidmode_buff,"adv7842.cpp : HDMI_SDP, 0x5A = 0x%x",(uint)local_10[0]);
    puts(curr_vidmode_buff);
    syslog(5,"%s",curr_vidmode_buff);
    if ((local_10[0] & 1) == 1) {
      read_register(this,0x48,'W',local_10);
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
        read_register(this,0x48,'R',local_10);
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
        read_register(this,0x48,'Y',local_10);
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
        read_register(this,0x48,'T',local_10);
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
    usleep(500000);
    if (*VideoStandard == ANALOG_DIGITAL_VIDEO_STANDARD_NONE) {
      read_register(this,0x22,0x86,local_10);
      if (adv7842_DebugDump == 1) {
        printf("ADV7842_STDI_REGISTER = %x\n",(uint)local_10[0]);
      }
      write_register(this,0x22,0x86,local_10[0] | 4);
      while( true ) {
        read_register(this,0x22,0xb1,local_10);
        if (adv7842_DebugDump == 1) {
          printf("ADV7844_DVALID_REGISTER = %x\n",(uint)local_10[0]);
        }
        local_20 = local_20 + 1;
        if ((char)local_10[0] < '\0') {
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
  read_register(this,0x22,0xb2,local_10);
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_BL_REGISTER = %x\n",(uint)local_10[0]);
  }
  bVar1 = local_10[0];
  if (adv7842_DebugDump == 1) {
    printf("BL = %d\n",(uint)(ushort)((uVar3 & 0xff3f) << 8 | (ushort)local_10[0]));
  }
  read_register(this,0x22,0xb3,local_10);
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_SCVS_LCVS_REGISTER = %x\n",(uint)local_10[0]);
  }
  bVar2 = (byte)((int)(uint)local_10[0] >> 3);
  if (adv7842_DebugDump == 1) {
    printf("SCVS = %d, value = 0x%x\n",(int)(uint)local_10[0] >> 3,(uint)local_10[0]);
  }
  uVar3 = (ushort)local_10[0];
  read_register(this,0x22,0xb4,local_10);
  if (adv7842_DebugDump == 1) {
    printf("ADV7842_SCF_LCF_REGISTER = %x, SCF = %d\n",(uint)local_10[0],(uint)(uVar3 & 0xff07));
  }
  uVar3 = (uVar3 & 0xff07) << 8 | (ushort)local_10[0];
  if (adv7842_DebugDump == 1) {
    printf("SCF = %d\n",(uint)uVar3);
  }
  if ((((1 < bVar2) && (bVar2 < 6)) && (0x100 < uVar3)) && (uVar3 < 0x10c)) {
    *VideoStandard = ANALOG_VIDEO_STANDARD_NTSC;
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 480i@60Hz",0x27);
    goto LAB_00416f30;
  }
  if (((1 < bVar2) && (bVar2 < 6)) && ((0x132 < uVar3 && (uVar3 < 0x13e)))) {
    *VideoStandard = ANALOG_VIDEO_STANDARD_PAL;
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 576i@50Hz",0x27);
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x201 < uVar3 && (uVar3 < 0x217)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_480p;
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 480p@60Hz",0x27);
    goto LAB_00416f30;
  }
  if ((((3 < bVar2) && (bVar2 < 7)) && (0x265 < uVar3)) && (uVar3 < 0x27b)) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_576p;
    memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 576p@50Hz",0x27);
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x2e8 < uVar3 && (uVar3 < 0x2f4)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_720p;
    if ((bVar1 < 0xd0) || (0xd8 < bVar1)) {
      *VideoStandard = *VideoStandard | 0x8000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 720p@60",0x25);
    }
    else {
      *VideoStandard = *VideoStandard | 0x40000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 720p@50",0x25);
    }
    goto LAB_00416f30;
  }
  if ((((3 < bVar2) && (bVar2 < 7)) && (0x22c < uVar3)) && (uVar3 < 0x238)) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_1080i;
    if ((bVar1 < 0xc5) || (0xcd < bVar1)) {
      *VideoStandard = *VideoStandard | 0x1000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080i@60",0x26);
    }
    else {
      *VideoStandard = *VideoStandard | 0x2000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080i@50",0x26);
    }
    goto LAB_00416f30;
  }
  if (((3 < bVar2) && (bVar2 < 7)) && ((0x459 < uVar3 && (uVar3 < 0x46f)))) {
    *VideoStandard = DIGITAL_VIDEO_STANDARD_1080p;
    if ((bVar1 < 0xdc) || (0xe4 < bVar1)) {
      *VideoStandard = *VideoStandard | 0x8000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080p@60",0x26);
    }
    else {
      *VideoStandard = *VideoStandard | 0x40000000;
      memcpy(curr_vidmode_buff,"adv7842.cpp : Input video is 1080p@50",0x26);
    }
    goto LAB_00416f30;
  }
  local_1c = local_1c + 1;
  goto LAB_00416f1c;
}



// ==== ADV7842_t::set_channel @ 004175d4 ====

/* DWARF original prototype: void set_channel(ADV7842_t * this, uint channel) */

void __thiscall ADV7842_t::set_channel(ADV7842_t *this,uint channel)

{
  this->m_I2CChannel = channel;
  return;
}



