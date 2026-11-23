
// ==== _init @ 004022e8 ====

int _init(EVP_PKEY_CTX *ctx)

{
  int iVar1;
  
  __gmon_start__();
  frame_dummy();
  iVar1 = __do_global_ctors_aux();
  return iVar1;
}



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



// ==== __do_global_dtors_aux @ 004023c0 ====

/* WARNING: Removing unreachable block (ram,0x00402414) */

void __do_global_dtors_aux(void)

{
  if (completed_5354 == '\0') {
    completed_5354 = '\x01';
  }
  return;
}



// ==== frame_dummy @ 0040245c ====

void frame_dummy(void)

{
  if (__JCR_END__ == 0) {
    return;
  }
  _Jv_RegisterClasses(&__JCR_END__);
  return;
}



// ==== term @ 00402490 ====

void term(int signum)

{
  SigTerm = 1;
  return;
}



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
  local_1c.flag = 6;
  encoder_param.audioIf = 0;
  adv7842_detect_audio_mode(adv7842_handle,&local_c);
  if (local_c == AUDIO_MODE_COMPRESSED) {
    encoder_param.audioIf = 1;
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
    adv7842_subscribe_event(adv7842_handle,9);
  }
  return local_10[0];
}



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
        local_30 = ADV7842_INPUT_TYPE_HDMI;
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
    case 1:
      if (evt->event_info == 0) {
        param1 = "unpluged";
      }
      else {
        param1 = "pluged";
      }
      printf("HDMI is %s\n",param1);
      if (evt->event_info == 0) {
        CurValidInput = CurValidInput & 0xfffffffb;
      }
      else {
        CurValidInput = CurValidInput | 4;
      }
      if ((CurInputType == ADV7842_INPUT_TYPE_NONE) && ((CurValidInput & 4U) != 0)) {
        xcode4_stop_encoder(encoderHandle);
        adv7842_unsubscribe_event(adv7842_handle,8);
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
          adv7842_subscribe_event(adv7842_handle,6);
        }
      }
      break;
    case 2:
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
    case 4:
      xcode4_stop_encoder(encoderHandle);
      adv7842_unsubscribe_event(adv7842_handle,6);
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
        uVar2 = CurValidInput & 0xfffffffd;
        uVar1 = CurValidInput & 1;
        CurValidInput = uVar2;
        if (uVar1 != 0) {
          CurInputType = ADV7842_INPUT_TYPE_CVBS;
          adv7842_program_input(adv7842_handle,ADV7842_INPUT_TYPE_CVBS,&local_c);
          if (local_c == VIDEO_STD_NONE) {
            CurValidInput = CurValidInput & 0xfffffffe;
          }
          else {
            adv7842_subscribe_event(adv7842_handle,6);
          }
        }
      }
      else {
        CurValidInput = CurValidInput | 2;
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
        adv7842_subscribe_event(adv7842_handle,9);
        CurInputType = ADV7842_INPUT_TYPE_NONE;
      }
      break;
    case 8:
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
    case 0x10:
      puts("HDMI signal is encrypted");
      adv7842_set_freerun_mode(adv7842_handle,ADV7842_FREERUN_MODE_FORCED_BLUESCREEN);
    }
  }
  return;
}



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



// ==== print_version @ 004038c4 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void print_version(void)

{
  undefined1 *puVar1;
  undefined4 in_stack_fffffff4;
  
  puVar1 = &_gp;
  putchar(10);
  (**(code **)(puVar1 + -0x7ed0))(&DAT_0041ff7c);
  (**(code **)(puVar1 + -0x7ea0))
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
  local_8f8 = 0x138a;
  local_8f4 = 0x20;
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
  local_8cc = 5000;
  local_8c8 = 17000000;
  local_8c4 = 19000000;
  local_8c0 = 0;
  memset(&sStack_73c,0,0x90);
  sStack_73c.sa_mask.__val[0] = (ulong)term;
  sigaction(0xf,&sStack_73c,(sigaction *)0x0);
  memcpy(acStack_608,"main.c :",9);
  for (local_8bc = 0; local_8bc < argc; local_8bc = local_8bc + 1) {
    sVar11 = strlen(acStack_608);
    memcpy(acStack_608 + sVar11,&DAT_0042017c,2);
    strcat(acStack_608,argv[local_8bc]);
  }
  syslog(5,acStack_608);
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
    if ((char)iVar9 == -1) {
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
        memset(&encoder_param,0,0xc0);
        encoder_param.bitRate = local_8c8;
        encoder_param.dstPort = local_8f8;
        encoder_param.dstTTL = local_8f4;
        encoder_param.dstAddr = string2netaddr(local_844);
        encoder_param.dvoPort = DVO_PORT_1;
        encoder_param.outMode = local_8ec;
        encoder_param.videoCodec = local_8f0;
        if (local_8c4 != 0) {
          encoder_param.flag = encoder_param.flag | 4;
          encoder_param.rmxRate = local_8c4;
        }
        if (bVar2) {
          encoder_param.flag = encoder_param.flag | 2;
        }
        if (local_8c0 != 0) {
          encoder_param.flag = encoder_param.flag | 8;
          encoder_param.pcrPid = local_8c0;
        }
        if ((local_8ec != VIXS_ENCODER_OUT_MODE_FILE) || (bVar3)) {
          if (local_8ec == VIXS_ENCODER_OUT_MODE_FILE) {
            strcpy(encoder_param.captureFileName,local_8d8);
          }
          if (bVar4) {
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
            if (sStack_6a8.st_size._4_4_ == 1) {
              sVar11 = 0x814;
            }
            else {
              sVar11 = 0x2f0;
            }
            encoder_param.flag = encoder_param.flag | 1;
            encoder_param.data_insertion_param.data = malloc(sVar11);
            encoder_param.data_insertion_param.interval = local_8cc;
            bVar5 = false;
            encoder_param.data_insertion_param.data_size = sVar11;
          }
          else {
            printf("\nError! Can\'t open data file %s\n",local_8dc);
            puts("\nWarning: No data insertion");
          }
          if (bVar5) {
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
              sVar11 = ((int)sStack_6a8.st_size / 0xbc) * 0xbc;
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
            adv7842_subscribe_event(adv7842_handle,0x17);
          }
          if (iVar6 == 0) {
            while (SigTerm == 0) {
              memcpy(acStack_608,"main.c : Caught SIGTERM",0x18);
            }
            syslog(5,acStack_608);
          }
          else {
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
    case 'F':
      local_8d8 = _optarg;
      bVar3 = true;
      printf("\nDebug mode - Set Capture file : %s\n",_optarg);
      break;
    default:
      print_version();
      print_usage();
      return 0;
    case 'M':
      uVar7 = atoi(_optarg);
      setMinor((uint16_t)uVar7);
      printf("Set Minor Channel Number= %d\n",uVar7 & 0xffff);
      break;
    case 'P':
      local_8f8 = atoi(_optarg);
      printf("Set Port Number = %d\n",local_8f8);
      break;
    case 'R':
      local_8c8 = atoi(_optarg);
      if ((17000000 < local_8c8) || (local_8c8 < 1000000)) {
        printf("\nError! The Video Data Bitrate %d - is not within range %d to %d.\n\n",local_8c8,
               1000000,17000000);
        print_usage();
        return 1;
      }
      printf("Set Video Data Bitrate = %d bps\n",local_8c8);
      break;
    case 'T':
      local_8f4 = atoi(_optarg);
      if ((0xff < local_8f4) || (local_8f4 < 0)) {
        printf("\nError! The TTL Value %d - is not within range 0 to 255.\n\n",local_8f4);
        print_usage();
        return 1;
      }
      printf("Set TTL Value = %d\n",local_8f4);
      break;
    case 'c':
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
    case 'd':
      local_8dc = _optarg;
      printf("Set Data Insertion Filename = %s\n",_optarg);
      bVar5 = true;
      break;
    case 'e':
      local_894.edid_file_name = _optarg;
      printf("Set EDID file = %s\n",_optarg);
      break;
    case 'f':
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
      iVar9 = strncmp("/mnt/sdcard/",__s2,0xb);
      if (iVar9 != 0) {
        puts("\nError! The Capture file name must start with \"/mnt/sdcard\".\n");
        print_usage();
        return 1;
      }
      printf("Set Capture file = %s\n",_optarg);
      bVar3 = true;
      break;
    case 'g':
      bVar4 = true;
      puts("Enable SDT/NIT Table Generation");
      break;
    case 'i':
      puts("Enable NULL Packet Insertion");
      bVar2 = true;
      break;
    case 'l':
      g_llc_delay = atoi(_optarg);
      printf("Set LLC Phase = %d\n",g_llc_delay);
      break;
    case 'm':
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
    case 'n':
      strcpy(local_844,_optarg);
      printf("Set Destination IP Address = %s\n",local_844);
      break;
    case 'o':
      iVar9 = strcmp(_optarg,"480i");
      if (iVar9 != 0) {
        printf("\nError! The Video Output Scaling %s is not valid.\n\n",_optarg);
        print_usage();
        return 1;
      }
      scaler_o480i = 1;
      printf("Enable Video Output Scaling to %s\n",_optarg);
      break;
    case 'p':
      local_8c0 = atoi(_optarg);
      printf("Set PCR PID = %d\n",local_8c0);
      break;
    case 'r':
      local_8c4 = atoi(_optarg);
      if ((19000000 < local_8c4) || (local_8c4 < 1500000)) {
        printf("\nError! The RMX Bitrate %d - is not within range %d to %d.\n\n",local_8c4,1500000,
               19000000);
        print_usage();
        return 1;
      }
      printf("Set RMX Bitrate = %d bps\n",local_8c4);
      break;
    case 't':
      local_8cc = atoi(_optarg);
      printf("Set Data Insertion Interval = %dms\n",local_8cc);
      break;
    case 'v':
      print_version();
      return 0;
    }
  } while( true );
}



// ==== message @ 00405420 ====

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

void writePSI(uint8_t *p_packet,dvbpsi_psi_section_t *p_section)

{
  int iVar1;
  uint8_t *puVar2;
  dvbpsi_psi_section_t *local_res4;
  uint8_t *local_18;
  uint8_t *local_14;
  
  *p_packet = 0x47;
  for (local_res4 = p_section; local_res4 != (dvbpsi_psi_section_t *)0x0;
      local_res4 = local_res4->p_next) {
                    /* Unresolved local var: uint8_t * p_pos_in_ts@[???]
                       Unresolved local var: uint8_t * p_byte@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
    local_14 = local_res4->p_data;
    puVar2 = local_res4->p_payload_end;
    if (local_res4->b_syntax_indicator == false) {
      iVar1 = 0;
    }
    else {
      iVar1 = 4;
    }
    p_packet[1] = p_packet[1] | 0x40;
    p_packet[4] = 0;
    local_18 = p_packet + 5;
    for (; (local_18 < p_packet + 0xbc && (local_14 < puVar2 + iVar1)); local_14 = local_14 + 1) {
      *local_18 = *local_14;
      local_18 = local_18 + 1;
    }
    for (; local_18 < p_packet + 0xbc; local_18 = local_18 + 1) {
      *local_18 = 0xff;
    }
  }
  return;
}



// ==== bswap_32 @ 004056dc ====

uint32_t bswap_32(uint32_t x)

{
  uint uVar1;
  
  uVar1 = (x & 0xff00ff) << 8 | x >> 8 & 0xff00ff;
  return (uVar1 >> 0x10) + uVar1 * 0x10000;
}



// ==== crc32 @ 00405740 ====

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

int FormTSHeader(uint8_t *dat8,uint16_t pid,uint8_t counter)

{
  *dat8 = 0x47;
  dat8[1] = (byte)(pid >> 8) & 0x1f | 0x40;
  dat8[2] = (uint8_t)pid;
  dat8[3] = counter & 0xf | 0x10;
  dat8[4] = 0;
  return 5;
}



// ==== FormATSC_MGT @ 00405898 ====

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
  *data = 199;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)(local_30 >> 8);
  data[10] = (uint8_t)local_30;
  local_38 = 0xb;
  if (vctlen != 0) {
    data[0xb] = 0;
    data[0xc] = 0;
    data[0xd] = 0xff;
    data[0xe] = 0xfb;
    data[0xf] = 0xf7;
    data[0x10] = (uint8_t)(vctlen >> 0x18);
    data[0x11] = (uint8_t)(vctlen >> 0x10);
    data[0x12] = (uint8_t)(vctlen >> 8);
    data[0x13] = (uint8_t)vctlen;
    data[0x14] = 0xf0;
    data[0x15] = 0;
    local_38 = 0x16;
  }
  if (eitcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)eitcnt; local_34 = local_34 + 1) {
      uVar1 = eitpid[local_34];
      data[local_38] = 1;
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0;
      data[local_38 + 3] = (uint8_t)uVar1;
      data[local_38 + 4] = 0xf7;
      data[local_38 + 5] = (uint8_t)(eitlen[local_34] >> 0x18);
      data[local_38 + 6] = (uint8_t)(eitlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(eitlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)eitlen[local_34];
      data[local_38 + 9] = 0xf0;
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  if (ettcnt != 0) {
    for (local_34 = 0; local_34 < (int)(uint)ettcnt; local_34 = local_34 + 1) {
      uVar1 = ettpid[local_34];
      data[local_38] = 2;
      data[local_38 + 1] = (uint8_t)local_34;
      data[local_38 + 2] = (byte)(uVar1 >> 8) & 0x1f | 0xe0;
      data[local_38 + 3] = (uint8_t)uVar1;
      data[local_38 + 4] = 0xf7;
      data[local_38 + 5] = (uint8_t)(ettlen[local_34] >> 0x18);
      data[local_38 + 6] = (uint8_t)(ettlen[local_34] >> 0x10);
      data[local_38 + 7] = (uint8_t)(ettlen[local_34] >> 8);
      data[local_38 + 8] = (uint8_t)ettlen[local_34];
      data[local_38 + 9] = 0xf0;
      data[local_38 + 10] = 0;
      local_38 = local_38 + 0xb;
    }
  }
  data[local_38] = 0xf0;
  data[local_38 + 1] = 0;
  length = local_38 + 2;
  data[1] = (byte)((uint)(local_38 + 3) >> 8) | 0xf0;
  data[2] = (char)length + 1;
  uVar2 = crc32(0xffffffff,data,length);
  uVar2 = bswap_32(uVar2);
  data[length] = (uint8_t)(uVar2 >> 0x18);
  data[local_38 + 3] = (uint8_t)(uVar2 >> 0x10);
  data[local_38 + 4] = (uint8_t)(uVar2 >> 8);
  data[local_38 + 5] = (uint8_t)uVar2;
  return local_38 + 6;
}



// ==== setMinor @ 004063e4 ====

uint16_t setMinor(uint16_t minor)

{
  sti_minor = minor;
  return minor;
}



// ==== FormATSC_ETT @ 00406420 ====

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
    bVar1 = 0;
  }
  else {
    bVar1 = 2;
  }
  *data = 0xcc;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = (uint8_t)(event_id >> 8);
  data[4] = (uint8_t)event_id;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)(source_id >> 8);
  data[10] = (byte)source_id | (byte)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (byte)iVar2 | bVar1;
  data[0xd] = 1;
  local_30 = 0xe;
  for (local_2c = 0; local_2c < 1; local_2c = local_2c + 1) {
    data[local_30] = 0x65;
    data[local_30 + 1] = 0x6e;
    data[local_30 + 2] = 0x67;
    data[local_30 + 3] = 1;
    local_30 = local_30 + 4;
    for (local_28 = 0; local_28 < 1; local_28 = local_28 + 1) {
      data[local_30] = 0;
      data[local_30 + 1] = 0;
      data[local_30 + 2] = (uint8_t)addinfolen;
      local_30 = local_30 + 3;
      for (local_24 = 0; local_24 < addinfolen; local_24 = local_24 + 1) {
        data[local_30] = addinfo[local_24];
        local_30 = local_30 + 1;
      }
    }
  }
  data[1] = (byte)(local_30 + 1 >> 8) | 0xf0;
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
  iVar4 = (tVar3 / 0x15180) * 0x15180 + -0x12d53d70 + timeoffs;
  *data = 0xcb;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 3;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 1;
  local_50 = 10;
  bVar1 = false;
  while (!bVar1) {
    data[local_50] = (byte)(evt_id >> 8) | 0xc0;
    data[local_50 + 1] = (uint8_t)evt_id;
    data[local_50 + 2] = (uint8_t)((uint)iVar4 >> 0x18);
    data[local_50 + 3] = (uint8_t)((uint)iVar4 >> 0x10);
    data[local_50 + 4] = (uint8_t)((uint)iVar4 >> 8);
    data[local_50 + 5] = (uint8_t)iVar4;
    data[local_50 + 6] = ((byte)etm & 3) << 4 | 0xc0;
    data[local_50 + 7] = 0x54;
    data[local_50 + 8] = 0x60;
    uVar5 = local_50 + 9;
    data[uVar5] = 0;
    data[local_50 + 10] = 1;
    local_50 = local_50 + 0xb;
    for (local_4c = 0; local_4c < 1; local_4c = local_4c + 1) {
      data[local_50] = 0x65;
      data[local_50 + 1] = 0x6e;
      data[local_50 + 2] = 0x67;
      data[local_50 + 3] = 1;
      local_50 = local_50 + 4;
      for (local_48 = 0; local_48 < 1; local_48 = local_48 + 1) {
        data[local_50] = 0;
        data[local_50 + 1] = 0;
        data[local_50 + 2] = (uint8_t)title_len;
        local_50 = local_50 + 3;
        for (local_44 = 0; local_44 < title_len; local_44 = local_44 + 1) {
          data[local_50] = title[local_44];
          local_50 = local_50 + 1;
        }
      }
    }
    data[uVar5 & 0xff] = ((byte)local_50 - (char)uVar5) - 1;
    data[local_50] = 0;
    data[local_50 + 1] = 0;
    data[local_50 + 2] = 0x86;
    data[local_50 + 3] = 7;
    data[local_50 + 4] = 0xe1;
    data[local_50 + 5] = 0x65;
    data[local_50 + 6] = 0x6e;
    data[local_50 + 7] = 0x67;
    data[local_50 + 8] = 0xc1;
    data[local_50 + 9] = 0x3f;
    data[local_50 + 10] = 0xff;
    uVar2 = ((short)(local_50 + 0xb) - (ushort)(byte)local_50) - 2;
    data[local_50 & 0xff] = (byte)(uVar2 >> 8) & 0xf | 0xf0;
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
  *data = 200;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = 1;
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
    data[local_68] = 0;
    data[local_68 + 1] = 0x4d;
    data[local_68 + 2] = 0;
    data[local_68 + 3] = 0x45;
    data[local_68 + 4] = 0;
    data[local_68 + 5] = 0x4c;
    data[local_68 + 6] = 0;
    data[local_68 + 7] = 0x44;
    data[local_68 + 8] = 0;
    data[local_68 + 9] = 0x33;
    data[local_68 + 10] = 0;
    data[local_68 + 0xb] = 0x30;
    data[local_68 + 0xc] = 0;
    data[local_68 + 0xd] = 0x30;
    data[local_68 + 0xe] = (byte)(mjchannel >> 6) | 0xf0;
    data[local_68 + 0xf] = (char)mjchannel << 2 | (byte)(mnchannel >> 8) & 0xf;
    data[local_68 + 0x10] = (uint8_t)mnchannel;
    data[local_68 + 0x11] = 4;
    data[local_68 + 0x12] = 0;
    data[local_68 + 0x13] = 0;
    data[local_68 + 0x14] = 0;
    data[local_68 + 0x15] = 0;
    data[local_68 + 0x16] = 0;
    data[local_68 + 0x17] = 0;
    data[local_68 + 0x18] = 0;
    data[local_68 + 0x19] = 3;
    data[local_68 + 0x1a] = etm << 6 | 9;
    data[local_68 + 0x1b] = 0xc2;
    data[local_68 + 0x1c] = 0;
    data[local_68 + 0x1d] = 3;
    iVar2 = local_68 + 0x1e;
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[local_68 + 0x1f] = (uint8_t)local_5c;
    data[local_68 + 0x20] = 0xa0;
    iVar3 = local_68 + 0x21;
    data[iVar3] = 0;
    data[local_68 + 0x22] = 1;
    local_68 = local_68 + 0x23;
    for (local_58 = 0; local_58 < 1; local_58 = local_58 + 1) {
      data[local_68] = 0x65;
      data[local_68 + 1] = 0x6e;
      data[local_68 + 2] = 0x67;
      data[local_68 + 3] = 1;
      local_68 = local_68 + 4;
      for (local_54 = 0; local_54 < 1; local_54 = local_54 + 1) {
        data[local_68] = 0;
        data[local_68 + 1] = 0;
        data[local_68 + 2] = (uint8_t)ext_name_len;
        local_68 = local_68 + 3;
        for (local_50 = 0; local_50 < ext_name_len; local_50 = local_50 + 1) {
          data[local_68] = extended_name[local_50];
          local_68 = local_68 + 1;
        }
      }
    }
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1;
    data[local_68] = 0xa1;
    iVar3 = local_68 + 1;
    data[iVar3] = 0;
    data[local_68 + 2] = 0xe0;
    data[local_68 + 3] = 0x30;
    data[local_68 + 4] = 2;
    data[local_68 + 5] = 2;
    data[local_68 + 6] = 0xe0;
    data[local_68 + 7] = 0x30;
    data[local_68 + 8] = 0x65;
    data[local_68 + 9] = 0x6e;
    data[local_68 + 10] = 0x67;
    data[local_68 + 0xb] = 0x81;
    data[local_68 + 0xc] = 0xe0;
    data[local_68 + 0xd] = 0x31;
    data[local_68 + 0xe] = 0x65;
    data[local_68 + 0xf] = 0x6e;
    data[local_68 + 0x10] = 0x67;
    local_68 = local_68 + 0x11;
    data[iVar3] = ((char)local_68 - (char)iVar3) - 1;
    local_5c = ((short)local_68 - (short)iVar2) + -2;
    data[iVar2] = (byte)((ushort)local_5c >> 8) & 3 | 0xfc;
    data[sVar1 + 0x1f] = (uint8_t)local_5c;
  }
  data[local_68] = 0xfc;
  data[local_68 + 1] = 0;
  local_68 = local_68 + 2;
  for (local_60 = 0; local_60 < 0; local_60 = local_60 + 1) {
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
  iVar2 = tVar1 + -0x12d4db1b;
  *data = 0xcd;
  data[1] = 0xf0;
  data[2] = 0;
  data[3] = 0;
  data[4] = 0;
  data[5] = 0xef;
  data[6] = 0;
  data[7] = 0;
  data[8] = 0;
  data[9] = (uint8_t)((uint)iVar2 >> 0x18);
  data[10] = (uint8_t)((uint)iVar2 >> 0x10);
  data[0xb] = (uint8_t)((uint)iVar2 >> 8);
  data[0xc] = (uint8_t)iVar2;
  data[0xd] = 0xf;
  data[0xe] = 0;
  data[0xf] = 0;
  data[1] = 0xf0;
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
  
  local_4c[0] = 0x4d;
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
  local_4c[4] = 0x6c;
  local_4c[5] = 10;
  local_4c[6] = 0;
  local_4c[7] = 0;
  local_40 = 0;
  local_30 = 0xff;
  local_24 = 0;
  local_20 = 0;
  local_2c = 0x3000341;
  local_28 = 1;
  local_68 = '\0';
  iVar2 = freq * 100000;
  if (lngt < 0.0) {
    lngt = lngt + 360.0;
  }
  fVar5 = (lngt / 180.0) * 32768.0;
  if (fVar5 < 2.1474836e+09) {
    uVar3 = (undefined2)(int)fVar5;
  }
  else {
    uVar3 = (undefined2)(int)(fVar5 - 2.1474836e+09);
  }
  if (lttd < 0.0) {
    lttd = lttd + 360.0;
  }
  fVar5 = (lttd / 90.0) * 32768.0;
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
    local_68 = '\x02';
  }
  else if (bandwidth < 7) {
    if (bandwidth == 5) {
      local_68 = '\x03';
    }
  }
  else if (bandwidth == 7) {
    local_68 = '\x01';
  }
  else if (bandwidth == 8) {
    local_68 = '\0';
  }
  uVar1 = CONCAT12(local_68 << 5,local_38._0_2_);
  _local_38 = CONCAT13(0x40,uVar1 | 0x1f0000);
  local_34 = 0xffffff1c;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_nit_init(&dStack_1c,0x40,3,0x2114,0x17,true);
    dvbpsi_nit_descriptor_add(&dStack_1c,0x40,4,local_4c);
    p_ts = dvbpsi_nit_ts_add(&dStack_1c,0,0x2114);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x5a,0xb,&uStack_3a);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x6c,10,local_4c + 6);
    dvbpsi_nit_ts_descriptor_add(p_ts,0x41,3,(uint8_t *)((int)&local_2c + 2));
    dStack_1c.b_current_next = true;
    p_section = dvbpsi_nit_sections_generate(p_dvbpsi,&dStack_1c,0x40);
    *packet = 0x47;
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
  local_d8 = 0x48;
  local_d7 = 0xe;
  local_d6[0] = 1;
  local_d6[1] = 4;
  local_d6[2] = 0x4d;
  local_d6[3] = 0x45;
  local_d6[4] = 0x4c;
  local_d6[5] = 0x44;
  local_d6[6] = 8;
  local_d6[7] = 0x4d;
  local_d6[8] = 0x4b;
  local_d6[9] = 0x56;
  local_d6[10] = 0x4d;
  local_d6[0xb] = 0x54;
  local_d6[0xc] = 0x33;
  local_d6[0xd] = 0x30;
  local_d6[0xe] = 0x30;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_sdt_init(&dStack_58,0x42,0,0x10,false,0x2114);
    p_service = dvbpsi_sdt_service_add(&dStack_58,3,false,false,4,false);
    sprintf(acStack_4c,"%s,%s(%s,%3.6f,%3.6f)",company,service_name,sn,(double)lttd,(double)lngt);
    sVar1 = strlen(acStack_4c);
    local_d6[6] = (char)sVar1;
    memcpy(local_d6 + 7,acStack_4c,(size_t)sVar1);
    dvbpsi_sdt_service_descriptor_add(p_service,0x48,local_d6[6] + 7,local_d6);
    dStack_58.b_current_next = true;
    p_section = dvbpsi_sdt_sections_generate(p_dvbpsi,&dStack_58);
    *packet = 0x47;
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
          (int)((double)(ptVar1->tm_mon + 2 + iVar2 * 0xc) * 30.6001);
  uVar4 = ptVar1->tm_hour << 0x10 | ptVar1->tm_min << 8 | ptVar1->tm_sec;
  p_dvbpsi = dvbpsi_new(message,DVBPSI_MSG_DEBUG);
  if (p_dvbpsi != (dvbpsi_t *)0x0) {
    dvbpsi_tot_init(&dStack_30,0x70,0,0x10,false,
                    CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                             uVar4 | uVar3 * 0x1000000));
    p_section = dvbpsi_tot_sections_generate(p_dvbpsi,&dStack_30);
    *packet = 0x47;
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
    local_84 = 0x6500114e;
    local_80 = 0xb00756e;
    local_7c = 0x74736574;
    local_78 = 0x20666f20;
    local_74 = 0x544945;
    local_60 = 0;
    local_64 = 0x920254;
    memset(&local_5c,0,0x40);
    local_5c = 0x4d;
    local_5b = 0x1e;
    local_5a[0] = 0x65;
    local_5a[1] = 0x6e;
    local_5a[2] = 0x75;
    local_5a[3] = 0xf;
    local_5a[4] = 0x74;
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
    local_5a[0x13] = 10;
    local_5a[0x14] = 0x65;
    local_5a[0x15] = 0x78;
    local_5a[0x16] = 0x70;
    local_5a[0x17] = 0x65;
    local_5a[0x18] = 0x72;
    local_5a[0x19] = 0x69;
    local_5a[0x1a] = 0x6d;
    local_5a[0x1b] = 0x65;
    local_5a[0x1c] = 0x6e;
    local_5a[0x1d] = 0x74;
    dvbpsi_eit_init(&dStack_98,0x4e,3,0x10,false,0,0x2114,0,0);
    p_event = dvbpsi_eit_event_add
                        (&dStack_98,0,
                         CONCAT44((int)uVar4 >> 0x1f | uVar3 >> 8 | ((int)uVar3 >> 0x1f) << 0x18,
                                  uVar4 | uVar3 * 0x1000000),0x5dc,0,false,0x32);
    dvbpsi_eit_event_descriptor_add(p_event,0x4e,0x11,auStack_82);
    dvbpsi_eit_event_descriptor_add(p_event,0x54,2,(uint8_t *)((int)&local_64 + 2));
    dvbpsi_eit_event_descriptor_add(p_event,0x4d,0x1e,local_5a);
    p_section = dvbpsi_eit_sections_generate(p_dvbpsi,&dStack_98,0x4e);
    *packet = 0x47;
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
      if (((uint)vStack_30.result & 0x1000) != 0) {
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
        pEnCoder->stopRecv = 1;
      }
      else {
        pEnCoder->recvTimeoutCnt = pEnCoder->recvTimeoutCnt - 1;
        if (pEnCoder->recvTimeoutCnt == 0) {
          pEnCoder->stopRecv = 1;
        }
      }
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_recv_video_thread_entry");
  return (void *)0x0;
}



// ==== terminate_encoder_recv_thread @ 00409d30 ====

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
  conf->modulationmode = 1;
  conf->bandwidth = 0;
  conf->chan_freq = 0x1b;
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
              conf->bandwidth = local_14[0x19] + -0x30;
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
      __n = _Var2 - 1;
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
  readCfgFile("/etc/config/queryInfo",&fStack_e4);
  readSbiFile("/etc/config/sbi",&local_a0,&local_9c);
  readCompanyFile("/etc/config/company",acStack_98);
  sprintf(acStack_48,"%s,%s(%s,%3.6f, %3.6f)",acStack_98,fStack_e4.fcc,fStack_e4.sn,(double)local_a0
          ,(double)local_9c);
  ext_name_len = strlen(acStack_48);
  printf("%s:Enter Encoder %x send data thread Entry \n","encoder_send_data_thread_entry",
         pEnCoder->idx);
  while (mThread->toExit == 0) {
    local_e8 = pEnCoder->sendDataSize;
    if (fStack_e4.modulationmode == 0) {
      genNIT((uint8_t *)pEnCoder->pSendDataBuf,local_128,fStack_e4.chan_freq,fStack_e4.bandwidth,
             local_a0,local_9c);
      genSDT((uint8_t *)(pEnCoder->pSendDataBuf + 0xbc),local_128,acStack_98,fStack_e4.fcc,
             fStack_e4.sn,local_a0,local_9c);
      genTDT((uint8_t *)(pEnCoder->pSendDataBuf + 0x178),local_128);
      genEIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x234),local_128);
    }
    else if (fStack_e4.modulationmode == 1) {
      dat8 = (uint8_t *)pEnCoder->pSendDataBuf;
      FormTSHeader(dat8,0x1ffb,local_128);
      vctlen = FormATSC_TVCT((uint8_t *)(pEnCoder->pSendDataBuf + 5),(uint16_t)fStack_e4.chan_freq,
                             sti_minor,acStack_48,ext_name_len,0);
      for (local_124 = vctlen + 5; local_124 < 0xbc; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0xbc,local_78[0],local_126);
      local_70 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0xc1),acStack_48,ext_name_len,0,
                              10,1);
      for (local_124 = local_70 + 0xc1; local_124 < 0x178; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x178,local_78[1],local_126);
      local_6c = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x17d),acStack_48,ext_name_len,
                              0x5460,0x14,1);
      for (local_124 = local_6c + 0x17d; local_124 < 0x234; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x234,local_78[2],local_126);
      local_68 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x239),acStack_48,ext_name_len,
                              0xa8c0,0x1e,1);
      for (local_124 = local_68 + 0x239; local_124 < 0x2f0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x2f0,local_78[3],local_126);
      local_64 = FormATSC_EIT((uint8_t *)(pEnCoder->pSendDataBuf + 0x2f5),acStack_48,ext_name_len,
                              0xfd20,0x28,1);
      for (local_124 = local_64 + 0x2f5; local_124 < 0x3ac; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_126 = local_126 + 1;
      FormTSHeader(dat8 + 0x3ac,local_60[0],local_127);
      local_58 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x3b1),acStack_48,ext_name_len,3,
                              10);
      for (local_124 = local_58 + 0x3b1; local_124 < 0x468; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x468,local_60[1],local_127);
      local_54 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x46d),acStack_48,ext_name_len,3,
                              0x14);
      for (local_124 = local_54 + 0x46d; local_124 < 0x524; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x524,local_60[2],local_127);
      local_50 = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x529),acStack_48,ext_name_len,3,
                              0x1e);
      for (local_124 = local_50 + 0x529; local_124 < 0x5e0; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      FormTSHeader(dat8 + 0x5e0,local_60[3],local_127);
      local_4c = FormATSC_ETT((uint8_t *)(pEnCoder->pSendDataBuf + 0x5e5),acStack_48,ext_name_len,3,
                              0x28);
      for (local_124 = local_4c + 0x5e5; local_124 < 0x69c; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_127 = local_127 + 1;
      FormTSHeader(dat8 + 0x69c,0x1ffb,local_128 + 1);
      iVar1 = FormATSC_MGT((uint8_t *)(pEnCoder->pSendDataBuf + 0x6a1),vctlen,&local_70,local_78,4,
                           &local_58,local_60,4);
      for (local_124 = iVar1 + 0x6a1; local_124 < 0x758; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
      local_128 = local_128 + 2;
      FormTSHeader(dat8 + 0x758,0x1ffb,local_128);
      iVar1 = FormATSC_STT((uint8_t *)(pEnCoder->pSendDataBuf + 0x75d));
      for (local_124 = iVar1 + 0x75d; local_124 < 0x814; local_124 = local_124 + 1) {
        dat8[local_124] = 0xff;
      }
    }
    local_128 = local_128 + 1;
    rVar2 = vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8000000);
    if (rVar2 != RET_OK) {
      printf("%s:Xcoder %x Send data to AV driver error\n","encoder_send_data_thread_entry",
             pEnCoder->idx);
      mThread->toExit = 1;
    }
    if (pEnCoder->sendDataInterval == 0) {
      mThread->toExit = 1;
    }
    else {
      usleep(pEnCoder->sendDataInterval * 1000);
    }
  }
  printf("%s:Exit recv thread Entry \n","encoder_send_data_thread_entry");
  local_e8 = 0;
  vixs_encoder_send_stream(pEnCoder,(int *)&local_e8,0x8400000);
  return (void *)0x0;
}



// ==== terminate_encoder_send_data_thread @ 0040b57c ====

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

return_code_t xcode4_open_encoder(int *handle,vixs_encoder_init_param_t *param)

{
  vixs_encoder_t *pEnCoder;
  return_code_t rVar1;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  pEnCoder = malloc(0x1980);
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
      if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
        local_10 = launch_encoder_recv_thead((vixs_encoder_t *)handle);
        if (local_10 != RET_OK) {
          printf("%s:Launch Encode Thread failed\n","xcode4_start_encoder");
          vixs_encoder_close_stream((vixs_encoder_t *)handle);
        }
      }
      if (*(int *)(handle + 0xd4) != 0) {
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

return_code_t xcode4_stop_encoder(int handle)

{
  return_code_t local_10;
  
                    /* Unresolved local var: return_code_t rc@[???]
                       Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  printf("%s:E\n","xcode4_stop_encoder");
  if ((*(int *)(handle + 0x3c) == 1) || (*(int *)(handle + 0x3c) == 3)) {
    local_10 = terminate_encoder_recv_thread((vixs_encoder_t *)handle);
    if (local_10 != RET_OK) {
      printf("%s:Fail to terminate encode_thread, return value %x\n","xcode4_stop_encoder",local_10)
      ;
    }
  }
  else {
    vixs_encoder_flush_stream((vixs_encoder_t *)handle);
  }
  if (*(int *)(handle + 0xd4) != 0) {
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

return_code_t xcode4_close_encoder(int handle)

{
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  free((void *)handle);
  return RET_OK;
}



// ==== xcode4_update_encoder_param @ 0040bc58 ====

return_code_t xcode4_update_encoder_param(int handle,vixs_encoder_update_param_t *param)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???] */
  rVar1 = vixs_encoder_update_stream((vixs_encoder_t *)handle,param);
  return rVar1;
}



// ==== xcode4_get_encoder_ts_info @ 0040bcb8 ====

return_code_t xcode4_get_encoder_ts_info(int handle,vixs_encoder_ts_info_t *info)

{
  return_code_t rVar1;
  
                    /* Unresolved local var: vixs_encoder_t * pEnCoder@[???]
                       Unresolved local var: return_code_t rc@[???] */
  rVar1 = vixs_encoder_get_ts_info((vixs_encoder_t *)handle);
  if (rVar1 == RET_OK) {
    info->inputPsiLock = *(uint *)(handle + 0x1324) & 1;
    info->inputVideoLock = (uint)((*(uint *)(handle + 0x1324) & 2) != 0);
    info->inputAudioLock = (uint)((*(uint *)(handle + 0x1324) & 4) != 0);
    info->inputPCRLock = (uint)((*(uint *)(handle + 0x1324) & 8) != 0);
    info->inputVideoCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x10) != 0);
    info->inputAudioCCErr = (uint)((*(uint *)(handle + 0x1324) & 0x20) != 0);
    info->tsOutput = (uint)((*(uint *)(handle + 0x1324) & 0x40) == 0);
    info->videoOutput = (uint)((*(uint *)(handle + 0x1324) & 0x80) == 0);
    info->audioOutput = (uint)((*(uint *)(handle + 0x1324) & 0x100) == 0);
  }
  return rVar1;
}



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



// ==== vixs_encoder_init @ 0040d8d4 ====

/* WARNING: Enum "_GOP_TYPE": Some values do not have unique names */
/* WARNING: Enum "xcode_video_type": Some values do not have unique names */

return_code_t vixs_encoder_init(vixs_encoder_t *pEnCoder,vixs_encoder_init_param_t *param)

{
  int iVar1;
  return_code_t rVar2;
  uint uVar3;
  undefined4 uVar4;
  
                    /* Unresolved local var: PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture@[???] */
  memset(pEnCoder,0,0x1980);
  pEnCoder->bActive = 0;
  pEnCoder->stopEncode = 0;
  pEnCoder->stopRecv = 0;
  pEnCoder->recvTimeoutCnt = 10;
  pEnCoder->init_param = param;
  if (param->bitRate == 0) {
    return RET_OK;
  }
  if (param->inVideoStandard < param->outVideoStandard) {
    param->outVideoStandard = param->inVideoStandard;
  }
  if ((param->flag & 1) != 0) {
    pEnCoder->bSendData = 1;
    pEnCoder->sendDataSize = (param->data_insertion_param).data_size;
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
  memset(&pEnCoder->ioctl_sdev_open,0,0x9e8);
  (pEnCoder->ioctl_sdev_open).ioctl_header.command = 0x1f;
  (pEnCoder->ioctl_sdev_open).ioctl_header.size_of_buf = 0x9e8;
  (pEnCoder->ioctl_sdev_open).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_open).ioctl_header.return_value = 2;
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).stream_handle = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).stream_handle + 4) = 0;
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
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_pmt_pid = 0x20;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_src = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_pid = 0x30;
  if (param->videoCodec == VIXS_XCODE_VIDEO_MPEG2) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_type = MPEG2_CBR;
  }
  else if (param->videoCodec == VIXS_XCODE_VIDEO_H264) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_type = AVC_CBR;
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
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_brate = param->bitRate;
  switch(param->inVideoStandard) {
  default:
    printf("%s:Invalid input format\n","vixs_encoder_init");
    return RET_ERR;
  case VIDEO_STD_480I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
    break;
  case VIDEO_STD_576I:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
    break;
  case VIDEO_STD_480P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
    break;
  case VIDEO_STD_576P:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
    break;
  case VIDEO_STD_720P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
    break;
  case VIDEO_STD_720P_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
    break;
  case VIDEO_STD_1080I_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
    break;
  case VIDEO_STD_1080I_50HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
    break;
  case VIDEO_STD_1080P_60HZ:
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
    (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
  }
  switch(param->outVideoStandard) {
  default:
    printf("%s:Invalid input format\n","vixs_encoder_init");
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
  if (pEnCoder->bSendData == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_size = 0;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.soc_shared_data_input_buffer_size = 0x10000;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_program_number = 3;
  (pEnCoder->ioctl_sdev_open).ioctl_param.diff_psi_out_time = 500;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_src = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_out_pid = 0x31;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_type = MP2;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_brate = 0x2ee00;
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
  (pEnCoder->ioctl_sdev_open).ioctl_param.req_size = 0x10000;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_fixed_dma_size = 0x10000;
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).ioctl_param.init_time_stamp = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.init_time_stamp + 4) = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_capture_offset = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.progressiveValue = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_flags =
       (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_flags | 1;
  if (((pEnCoder->ioctl_sdev_open).ioctl_param.video_out_frate == 0x19) &&
     ((pEnCoder->ioctl_sdev_open).ioctl_param.disp_vert < 0x241)) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 7;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.video_src_type = 3;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.n_b = GOP_B_DEFAULT;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_data_flags = 0x400;
  (pEnCoder->ioctl_sdev_open).ioctl_param.audio_interface = param->audioIf;
  (pEnCoder->ioctl_sdev_open).ioctl_param.insert_vbi_data_flags = 0;
  if ((param->flag & 4) == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.stream_remux_bitrate = 19300000;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.stream_remux_bitrate = param->rmxRate;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.peak_video_bitrate = 0;
  if ((param->flag & 8) == 0) {
    (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_out_id =
         (pEnCoder->ioctl_sdev_open).ioctl_param.video_out_pid;
  }
  else {
    (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_out_id = param->pcrPid;
  }
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_flags = 0xd8;
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
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_ps_enum = PS_DEFAULT_PS;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_pid1 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_content1 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_pid2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.out_lang_desc_content2 = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_max_vpes_size = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.remux_fixed_apes_size = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.pcr_output_duration = 500;
  (pEnCoder->ioctl_sdev_open).ioctl_param.dts_delay_from_pcr = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.scaling_detail_level = DEFAULT_DETAIL;
  (pEnCoder->ioctl_sdev_open).ioctl_param.sharpness = SHARPNESS_DEFAULT;
  (pEnCoder->ioctl_sdev_open).ioctl_param.enableLowMode = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_cropped_horz_inclusive_region = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.video_cropped_vert_inclusive_region = 0;
  *(undefined4 *)&(pEnCoder->ioctl_sdev_open).ioctl_param.mux_handle = 0;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_open).ioctl_param.mux_handle + 4) = 0;
  (pEnCoder->ioctl_sdev_open).ioctl_param.output_packet_alignment_number = 0;
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
        pEnCoder->lastConsumedRecvBufIdx = 0xff;
        pEnCoder->totalOutputFrameNum = 0;
        pEnCoder->totalOutputSize = 0;
        pEnCoder->recvBufferSize = 0x10000;
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



// ==== vixs_encoder_open_stream @ 0040ec70 ====

return_code_t vixs_encoder_open_stream(vixs_encoder_t *pEnCoder)

{
  IOCTLSDEVOPENCAPTURE *pIVar1;
  return_code_t rVar2;
  undefined4 uVar3;
  int local_10;
  
                    /* Unresolved local var: PIOCTLSDEVOPENCAPTURE pioctl_open_strm_capture@[???]
                       Unresolved local var: int i@[???] */
  pIVar1 = &pEnCoder->ioctl_sdev_open;
  printf("%s:E\n","vixs_encoder_open_stream");
  ioctl(pEnCoder->hXcode,0x17241724,pIVar1);
  if ((pEnCoder->ioctl_sdev_open).ioctl_header.return_value == 1) {
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



// ==== vixs_encoder_close_stream @ 0040f0f8 ====

return_code_t vixs_encoder_close_stream(vixs_encoder_t *pEnCoder)

{
  bool bVar1;
  IOCTLSDEVCLOSE *__s;
  undefined4 uVar2;
  
                    /* Unresolved local var: PIOCTLSDEVCLOSE pioctl_close_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_close;
  printf("%s:E\n","vixs_encoder_close_stream");
  memset(__s,0,0xb4);
  (pEnCoder->ioctl_sdev_close).ioctl_header.command = 0x11;
  (__s->ioctl_header).size_of_buf = 0xb4;
  (pEnCoder->ioctl_sdev_close).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_close).ioctl_header.return_value = 2;
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



// ==== vixs_encoder_update_stream @ 0040f2a0 ====

return_code_t
vixs_encoder_update_stream(vixs_encoder_t *pEnCoder,vixs_encoder_update_param_t *param)

{
  IOCTLSDEVCTRL *__s;
  return_code_t rVar1;
  undefined4 uVar2;
  
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
      if ((param->flag & 2U) != 0) {
        printf("%s:Update Input Video Format %x\n","vixs_encoder_update_stream",
               param->newInVideoStandard);
        switch(param->newInVideoStandard) {
        default:
          printf("%s:Invalid input format\n","vixs_encoder_update_stream");
          return RET_ERR;
        case VIDEO_STD_480I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff;
          break;
        case VIDEO_STD_576I:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff;
          break;
        case VIDEO_STD_480P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x1e0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          break;
        case VIDEO_STD_576P:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x240;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
          break;
        case VIDEO_STD_720P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          break;
        case VIDEO_STD_720P_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x500;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x2d0;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x32;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
          break;
        case VIDEO_STD_1080I_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x1e;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff;
          break;
        case VIDEO_STD_1080I_50HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x19;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags & 0xfffffeff;
          break;
        case VIDEO_STD_1080P_60HZ:
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_horz = 0x780;
          (pEnCoder->ioctl_sdev_open).ioctl_param.in_disp_vert = 0x438;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_frate = 0x3c;
          (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags =
               (pEnCoder->ioctl_sdev_open).ioctl_param.video_in_flags | 0x100;
        }
      }
      if ((param->flag & 4U) != 0) {
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
      memset(__s,0,0x4a8);
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.command = 0x12;
      (__s->ioctl_header).size_of_buf = 0x4a8;
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.time_out_value = 5000;
      (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value = 2;
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
      if ((pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value == 1) {
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



// ==== vixs_encoder_flush_stream @ 0040fcc4 ====

return_code_t vixs_encoder_flush_stream(vixs_encoder_t *pEnCoder)

{
  bool bVar1;
  IOCTLSDEVCTRL *__s;
  undefined4 uVar2;
  
                    /* Unresolved local var: PIOCTLSDEVCTRL pioctl_dev_ctrl@[???] */
  __s = &pEnCoder->ioctl_sdev_ctrl;
  printf("%s:E\n","vixs_encoder_flush_stream");
  memset(__s,0,0x4a8);
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.command = 0x12;
  (__s->ioctl_header).size_of_buf = 0x4a8;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_header.return_value = 2;
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_ctrl).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_ctrl).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_sdev_ctrl).ioctl_param.stream_ctrl = 1;
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



// ==== vixs_encoder_recv_stream @ 0040fe94 ====

return_code_t
vixs_encoder_recv_stream
          (vixs_encoder_t *pEnCoder,int *pBufSize,int consumedIdx,
          vixs_xcode_recv_strm_info_t *pRecvInfo)

{
  bool bVar1;
  IOCTLSDEVRECV *__s;
  undefined4 uVar2;
  
                    /* Unresolved local var: PIOCTLSDEVRECV pioctl_recv_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_recv;
  memset(__s,0,0x240);
  (pEnCoder->ioctl_sdev_recv).ioctl_header.command = 0x14;
  (__s->ioctl_header).size_of_buf = 0x240;
  (pEnCoder->ioctl_sdev_recv).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_recv).ioctl_header.return_value = 2;
  *pBufSize = pEnCoder->recvBufferSize;
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



// ==== vixs_encoder_get_ts_info @ 00410168 ====

return_code_t vixs_encoder_get_ts_info(vixs_encoder_t *pEnCoder)

{
  bool bVar1;
  IOCTLSDEVTSCTRL *__s;
  undefined4 uVar2;
  
                    /* Unresolved local var: PIOCTLSDEVTSCTRL pioctl_ts_ctrl@[???] */
  __s = &pEnCoder->ioctl_ts_ctrl;
  memset(__s,0,0x450);
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.command = 0x102;
  (__s->ioctl_header).size_of_buf = 0x450;
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_ts_ctrl).ioctl_header.return_value = 2;
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_ts_ctrl).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_ts_ctrl).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_ts_ctrl).ioctl_param.flags = 0x20;
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



// ==== vixs_encoder_send_stream @ 00410330 ====

return_code_t vixs_encoder_send_stream(vixs_encoder_t *pEnCoder,int *pbufsize,uint64 flags)

{
  bool bVar1;
  IOCTLSDEVSEND *__s;
  undefined4 uVar2;
  
                    /* Unresolved local var: PIOCTLSDEVSEND pioctl_send_strm@[???] */
  __s = &pEnCoder->ioctl_sdev_send;
  memset(__s,0,0x1d8);
  (pEnCoder->ioctl_sdev_send).ioctl_header.command = 0x13;
  (__s->ioctl_header).size_of_buf = 0x1d8;
  (pEnCoder->ioctl_sdev_send).ioctl_header.time_out_value = 5000;
  (pEnCoder->ioctl_sdev_send).ioctl_header.return_value = 2;
  uVar2 = *(undefined4 *)((int)&pEnCoder->strm_handle_encoder + 4);
  *(int *)&(pEnCoder->ioctl_sdev_send).stream_handle = (int)pEnCoder->strm_handle_encoder;
  *(undefined4 *)((int)&(pEnCoder->ioctl_sdev_send).stream_handle + 4) = uVar2;
  (pEnCoder->ioctl_sdev_send).flags = flags;
  (pEnCoder->ioctl_sdev_send).buffer = pEnCoder->pSendDataBufPhy;
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



// ==== adv7842_thread @ 00410da0 ====

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
    ADV7842_t::read_register(arg,0x20,0xd7,local_28);
    pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
    if ((my_subscribed_event & 0x11) != 0) {
      ADV7842_t::read_register(arg,0x20,'o',local_28);
      if (((((local_28[0] & 2) != 0) && (hdmi_available == 0)) ||
          (((local_28[0] & 2) == 0 && (hdmi_available == 1)))) && ((my_subscribed_event & 1) != 0))
      {
        local_34.eventID = 1;
        local_34.input = ADV7842_INPUT_TYPE_HDMI;
        hdmi_available = (uint)((local_28[0] & 2) != 0);
        local_34.event_info = hdmi_available;
        pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
        (*my_event_callback)(my_event_callback_data,&local_34);
        pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
      }
      ADV7842_t::read_register(arg,0x34,'\x18',local_28);
      printf("HDMI HDMI 0x18 is %x\n",(uint)local_28[0]);
      ADV7842_t::read_register(arg,0x34,'6',local_28);
      printf("HDMI HDMI 0x36 is %x\n",(uint)local_28[0]);
      if ((local_28[0] & 2) == 0) {
        puts("libadv7842.cpp : I2S Audio is PCM format");
      }
      else if ((local_28[0] & 2) == 2) {
        puts("libadv7842.cpp : I2S Audio is Compressed format");
      }
      ADV7842_t::read_register(arg,0x34,'9',local_28);
      if ((local_28[0] & 0xf) == 2) {
        if (!bVar1) {
          memcpy(msg_buff,"libadv7842.cpp : Selecting HDMI Audio - Setting GPIO 5 to HIGH",0x3f);
          puts(msg_buff);
          syslog(5,"libadv7842.cpp : Selecting HDMI Audio - Setting GPIO 5 to HIGH");
          xcode_io_t::write_reg_mask_bits(xcodeIO,0x1100,0x20,0x20);
          xcode_io_t::write_reg_mask_bits(xcodeIO,0x1104,0x20,0x20);
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
        xcode_io_t::write_reg_mask_bits(xcodeIO,0x1100,0x20,0);
        xcode_io_t::write_reg_mask_bits(xcodeIO,0x1104,0x20,0x20);
        bVar1 = false;
        memcpy(msg_buff,"libadv7842.cpp : GPIO 5 set to LOW - Using XC4208 Master Audio Clock",0x45)
        ;
        syslog(5,msg_buff);
      }
      if (hdmi_available == 1) {
        strcpy(last_sample_rate,msg_sample_rate);
        switch(local_28[0] & 0xf) {
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
        if (audio_msg_cntr == 0x10) {
          audio_msg_cntr = 0;
          sprintf(msg_buff,"libadv7842.cpp : HDMI Audio Sample Rate = %s",msg_sample_rate);
          syslog(5,msg_buff);
        }
        printf("%s - cntr = %d\n",msg_buff,audio_msg_cntr);
      }
      ADV7842_t::read_register(arg,0x34,'\x04',local_28);
      if (((local_28[0] & 0x10) != 0) && ((my_subscribed_event & 0x10) != 0)) {
        local_34.eventID = 0x10;
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
          local_c = VIDEO_STD_NONE;
          local_34.input = ADV7842_INPUT_TYPE_NONE;
          adv7842_enum_input((int)arg,aAStack_24,&local_c,1);
          if (local_c != VIDEO_STD_NONE) {
                    /* Unresolved local var: int inputMask@[???] */
            local_4c = ADV7842_INPUT_TYPE_NONE;
            local_34.eventID = 8;
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
            local_34.eventID = 4;
            local_34.input = local_38;
            (*my_event_callback)(my_event_callback_data,&local_34);
          }
        }
        else {
          local_34.eventID = 2;
          local_34.input = local_38;
          local_34.event_info = local_3c;
          curStd = local_3c;
          (*my_event_callback)(my_event_callback_data,&local_34);
        }
      }
    }
    usleep(200000);
  }
  return (void *)0x0;
}



// ==== map_video_std @ 00411a64 ====

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

return_code_t adv7842_init(ADV7842_init_param_t *pParam,uint *pHandle)

{
  ADV7842_t *this;
  return_code_t rVar1;
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???] */
  this = operator_new(0x10);
                    /* try { // try from 00411b88 to 00411b8f has its CatchHandler @ 00411c84 */
  ADV7842_t::ADV7842_t(this,0);
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

return_code_t adv7842_get_input(int handle,ADV7842_input_type_t *pInputType)

{
  enum_xc_input_type_t param2;
  uint uVar1;
  ADV7842_input_type_t local_18;
  
                    /* Unresolved local var: ADV7842_t * adv7842@[???]
                       Unresolved local var: enum_xc_input_type_t xc_input@[???]
                       Unresolved local var: ADV7842_input_type_t input_type@[???] */
  param2 = ADV7842_t::get_current_input_source((ADV7842_t *)handle);
  if (param2 - XC_INPUT_HDMI_COMPOSITE_480I < 0x18) {
    uVar1 = 1 << (param2 - XC_INPUT_HDMI_COMPOSITE_480I & 0x1f);
    if ((uVar1 & 0x5af0) != 0) {
      local_18 = ADV7842_INPUT_TYPE_COMPONENT;
      goto LAB_0041237c;
    }
    if ((uVar1 & 0x840000) != 0) {
      local_18 = ADV7842_INPUT_TYPE_HDMI;
      goto LAB_0041237c;
    }
    if ((uVar1 & 5) != 0) {
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

return_code_t adv7842_detect_video_std(int handle,video_std_t *pInputStd)

{
  pthread_mutex_lock((pthread_mutex_t *)&adv7842Mutex);
  adv7842_detect_video_std_internal(handle,pInputStd);
  pthread_mutex_unlock((pthread_mutex_t *)&adv7842Mutex);
  return RET_OK;
}



// ==== adv7842_detect_audio_mode @ 004124c4 ====

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
          return RET_ERR;
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

return_code_t adv7842_set_freerun_mode(int handle,ADV7842_freerun_mode_t mode)

{
                    /* Unresolved local var: ADV7842_t * adv7842@[???] */
  ADV7842_t::set_free_run_mode((ADV7842_t *)handle,mode);
  return RET_OK;
}



// ==== xcode_io_t::xcode_io_t @ 00412a90 ====

/* WARNING: Removing unreachable block (ram,0x00412c5c) */
/* WARNING: Restarted to delay deadcode elimination for space: stack */
/* DWARF original prototype: void xcode_io_t(xcode_io_t * this) */

void __thiscall xcode_io_t::xcode_io_t(xcode_io_t *this)

{
  xcode_handle_t xVar1;
  uchar *puVar2;
  uint *puVar3;
  
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
  this->endian_flag = 0;
  return;
}



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



// ==== xcode_io_t::~xcode_io_t @ 00412d44 ====

/* DWARF original prototype: void ~xcode_io_t(xcode_io_t * this, int __in_chrg) */

void __thiscall xcode_io_t::~xcode_io_t(xcode_io_t *this,int __in_chrg)

{
  ~xcode_io_t(this,__in_chrg);
  operator_delete(this);
  return;
}



// ==== xcode_io_t::get_io @ 00412da4 ====

xcode_io_t * xcode_io_t::get_io(void)

{
  int iVar1;
  
  if ((get_io()::XocdeIO == '\0') && (iVar1 = __cxa_guard_acquire(&get_io()::XocdeIO), iVar1 != 0))
  {
                    /* try { // try from 00412e14 to 00412e1b has its CatchHandler @ 00412e90 */
    xcode_io_t(&get_io::lexical_block_0::XocdeIO);
    __cxa_guard_release(&get_io()::XocdeIO);
    __cxa_atexit(~xcode_io_t,&get_io::lexical_block_0::XocdeIO,&__dso_handle);
  }
  return &get_io::lexical_block_0::XocdeIO;
}



// ==== xcode_io_t::i2c_read @ 00412ed4 ====

/* DWARF original prototype: int i2c_read(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_read
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_read(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if ((iVar1 != 0) && (this->DebugDumpPrint == 1)) {
    printf("I2C_READ failed @ %2.2X\n",Address);
  }
  return iVar1;
}



// ==== xcode_io_t::i2c_write @ 00412fbc ====

/* DWARF original prototype: int i2c_write(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_write(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if (((iVar1 != 0) && (Address != 0x40)) && (this->DebugDumpPrint == 1)) {
    printf("I2C_WRITE failed @ %2.2X\n",Address);
  }
  return iVar1;
}



// ==== xcode_io_t::i2c_write_then_read @ 004130b4 ====

/* DWARF original prototype: int i2c_write_then_read(xcode_io_t * this, int I2CChannel, int Address,
   void * wdata, void * rdata, size_t wn, size_t rn, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write_then_read
          (xcode_io_t *this,int I2CChannel,int Address,void *wdata,void *rdata,size_t wn,size_t rn,
          int start,int stop)

{
  int iVar1;
  
                    /* Unresolved local var: int error@[???] */
  iVar1 = i2c_write(this,I2CChannel,Address,wdata,wn,start,0);
  if (iVar1 == 0) {
    iVar1 = i2c_read(this,I2CChannel,Address,rdata,rn,1,stop);
  }
  return iVar1;
}



// ==== byteswap @ 0041316c ====

void byteswap(char *ptr)

{
  char cVar1;
  
                    /* Unresolved local var: char c@[???] */
  cVar1 = *ptr;
  *ptr = ptr[3];
  ptr[3] = cVar1;
  cVar1 = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = cVar1;
  return;
}



// ==== xcode_io_t::mmreg_write @ 004131f8 ====

/* DWARF original prototype: void mmreg_write(xcode_io_t * this, uint reg_addr, uint value) */

void __thiscall xcode_io_t::mmreg_write(xcode_io_t *this,uint reg_addr,uint value)

{
  uint local_res8 [2];
  
  if (this->m_MMRBase != (uint *)0x0) {
    local_res8[0] = value;
    if (this->endian_flag != 0) {
      byteswap((char *)local_res8);
    }
    *(uint *)((int)this->m_MMRBase + (reg_addr & 0xfffffffc)) = local_res8[0];
  }
  return;
}



// ==== xcode_io_t::mmreg_read @ 0041327c ====

/* DWARF original prototype: uint mmreg_read(xcode_io_t * this, uint reg_addr) */

uint __thiscall xcode_io_t::mmreg_read(xcode_io_t *this,uint reg_addr)

{
  uint local_10 [2];
  
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



// ==== xcode_io_t::clear_reg_mask @ 00413330 ====

/* DWARF original prototype: void clear_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::clear_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;
  
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



// ==== xcode_io_t::set_reg_mask @ 00413404 ====

/* DWARF original prototype: void set_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::set_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;
  
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



// ==== xcode_io_t::write_reg_mask_bits @ 004134d0 ====

/* DWARF original prototype: void write_reg_mask_bits(xcode_io_t * this, uint reg_addr, uint mask,
   uint value) */

void __thiscall xcode_io_t::write_reg_mask_bits(xcode_io_t *this,uint reg_addr,uint mask,uint value)

{
  uint uVar1;
  
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



// ==== xcode_io_t::fb_write @ 004135f0 ====

/* DWARF original prototype: void * fb_write(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_write(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(this->m_FRBase + fb_addr,buf,size);
  }
  return pvVar1;
}



// ==== xcode_io_t::fb_read @ 00413680 ====

/* DWARF original prototype: void * fb_read(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_read(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(buf,this->m_FRBase + fb_addr,size);
  }
  return pvVar1;
}



// ==== xcode_io_t::fb_move @ 00413710 ====

/* DWARF original prototype: void * fb_move(xcode_io_t * this, uint dst_fb_addr, uint src_fb_addr,
   size_t size) */

void * __thiscall
xcode_io_t::fb_move(xcode_io_t *this,uint dst_fb_addr,uint src_fb_addr,size_t size)

{
  void *pvVar1;
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memmove(this->m_FRBase + dst_fb_addr,this->m_FRBase + src_fb_addr,size);
  }
  return pvVar1;
}



// ==== xcode_io_t::fb_set @ 004137b0 ====

/* DWARF original prototype: void * fb_set(xcode_io_t * this, uint fb_addr, int value, size_t size)
    */

void * __thiscall xcode_io_t::fb_set(xcode_io_t *this,uint fb_addr,int value,size_t size)

{
  void *pvVar1;
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memset(this->m_FRBase + fb_addr,value,size);
  }
  return pvVar1;
}



// ==== xcode_io_t::fb_addr_to_ptr @ 00413840 ====

/* DWARF original prototype: void * fb_addr_to_ptr(xcode_io_t * this, uint fb_addr) */

void * __thiscall xcode_io_t::fb_addr_to_ptr(xcode_io_t *this,uint fb_addr)

{
  uchar *puVar1;
  
  if (this->m_FRBase == (uchar *)0x0) {
    puVar1 = (uchar *)0x0;
  }
  else {
    puVar1 = this->m_FRBase + fb_addr;
  }
  return puVar1;
}



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



// ==== xcode_open_handle @ 00417610 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

xcode_handle_t xcode_open_handle(void)

{
  xcode_handle_t xVar1;
  
                    /* Unresolved local var: xcode_handle_t handle@[???] */
  xVar1 = open64("/dev/vixs/xcodedrv",2);
  if (xVar1 == 0) {
    printf("ERROR: Failed to open XCode driver - %d\n",0);
    xVar1 = 0;
  }
  return xVar1;
}



// ==== xcode_close_handle @ 004176a4 ====

int xcode_close_handle(xcode_handle_t handle)

{
  int iVar1;
  
  if (handle == 0) {
    iVar1 = -1;
  }
  else {
    iVar1 = close(handle);
  }
  return iVar1;
}



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



// ==== xcode_get_frame_buffer_map_addr @ 004181ec ====

void * xcode_get_frame_buffer_map_addr(xcode_handle_t XCodeHandle)

{
  undefined4 local_b8;
  undefined4 local_b4;
  uint local_ac;
  undefined4 local_24;
  void *local_14;
  
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



// ==== xcode_get_mmr_map_addr @ 004182d0 ====

void * xcode_get_mmr_map_addr(xcode_handle_t XCodeHandle)

{
  undefined4 local_b8;
  undefined4 local_b4;
  uint local_ac;
  undefined4 local_24;
  void *local_14;
  
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



// ==== xcode_i2c_read @ 004183b4 ====

int xcode_i2c_read(xcode_handle_t XCodeHandle,int I2CChannel,int Address,void *data,size_t n,
                  int atomic_period,int timeout,int start,int stop)

{
  undefined4 local_118;
  undefined4 local_114;
  int local_10c;
  undefined1 auStack_74 [64];
  size_t local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
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



// ==== xcode_i2c_write @ 004184dc ====

int xcode_i2c_write(xcode_handle_t XCodeHandle,int I2CChannel,int Address,void *data,size_t n,
                   int atomic_period,int timeout,int start,int stop)

{
  undefined4 local_118;
  undefined4 local_114;
  int local_10c;
  undefined4 local_84;
  undefined1 auStack_74 [64];
  size_t local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  
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



// ==== xcode_recv_stream @ 00418618 ====

uint xcode_recv_stream(xcode_handle_t XCodeHandle,IOCTLSDEVRECV_conflict *pioctl_recv_strm,
                      void *pbuffer,uint *pbufsize,uint64 hStream,uint *pflags,uint *pmore,
                      uint *presult)

{
  uint uVar1;
  
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



// ==== xcode_send_stream @ 00418804 ====

uint xcode_send_stream(xcode_handle_t XCodeHandle,IOCTLSDEVSEND_conflict *pioctl_send_strm,
                      void *pbuffer,uint *pbufsize,uint64 hStream,uint flags,uint more,uint pts,
                      uint dts)

{
  uint uVar1;
  
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



// ==== xcode_update_stream @ 004189bc ====

uint xcode_update_stream(xcode_handle_t XCodeHandle,IOCTLSDEVCTRL *pioctl_dev_ctrl,uint64 hStream,
                        uint parmsChanged)

{
  uint uVar1;
  
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



// ==== xcode_update_stream2 @ 00418b38 ====

uint xcode_update_stream2
               (xcode_handle_t XCodeHandle,IOCTLSDEVCTRL2 *pioctl_dev_ctrl,uint64 hStream,
               uint parmsChanged)

{
  uint uVar1;
  
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



// ==== xcode_close_stream @ 00418c74 ====

uint xcode_close_stream(xcode_handle_t XCodeHandle,uint64 hStream)

{
  undefined4 local_c0;
  undefined4 local_bc;
  uint local_b4;
  undefined4 local_2c;
  uint64 local_1c;
  
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



// ==== xcode_open_stream_capture @ 00418d70 ====

uint64 xcode_open_stream_capture
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENCAPTURE *pioctl_open_strm_capture)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
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



// ==== xcode_open_stream_local @ 00418ea0 ====

uint64 xcode_open_stream_local
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENLOCAL *pioctl_open_strm_local)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
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



// ==== xcode_open_stream_ts @ 00418fd0 ====

uint64 xcode_open_stream_ts(xcode_handle_t XCodeHandle,IOCTLSDEVOPENTS *pioctl_open_strm_ts)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
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



// ==== xcode_open_stream_ts_passthrough @ 00419100 ====

uint64 xcode_open_stream_ts_passthrough
                 (xcode_handle_t XCodeHandle,IOCTLSDEVOPENTSPASSTHROUGH *pioctl_open_passthrough)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
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



// ==== xcode_tsd_dvo_config @ 0041920c ====

uint xcode_tsd_dvo_config(xcode_handle_t XCodeHandle,IOCTLTSDCONFIG *pioctl_tsdconfig)

{
  uint uVar1;
  
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



// ==== xcode_dma_host_to_fb @ 004192f4 ====

uint xcode_dma_host_to_fb(xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  undefined4 local_c0;
  undefined4 local_bc;
  uint local_b4;
  undefined4 local_2c;
  uint local_1c;
  void *local_18;
  uint local_10;
  
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



// ==== xcode_dma_host_to_fb_large_chunk @ 004193fc ====

uint xcode_dma_host_to_fb_large_chunk
               (xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  uint uVar1;
  void *local_res4;
  uint local_res8;
  uint local_resc;
  
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



// ==== xcode_dma_fb_to_host @ 004194d4 ====

uint xcode_dma_fb_to_host(xcode_handle_t XCodeHandle,void *host_buf,uint fb_addr,uint size)

{
  undefined4 local_c0;
  undefined4 local_bc;
  uint local_b4;
  undefined4 local_2c;
  uint local_1c;
  void *local_18;
  uint local_10;
  
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



// ==== xcode_dma_fb_to_fb @ 004195dc ====

uint xcode_dma_fb_to_fb(xcode_handle_t XCodeHandle,uint src_addr,uint dst_addr,uint size)

{
  undefined4 local_b8;
  undefined4 local_b4;
  uint local_ac;
  undefined4 local_24;
  uint local_14;
  uint local_10;
  uint local_c;
  
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



// ==== xcode_dma_pattern_to_fb @ 004196e4 ====

uint xcode_dma_pattern_to_fb(xcode_handle_t XCodeHandle,uint fb_addr,uint pattern,uint size)

{
  undefined4 local_b8;
  undefined4 local_b4;
  uint local_ac;
  undefined4 local_24;
  uint local_14;
  uint local_10;
  uint local_c;
  
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



// ==== xcode_dma_pattern_to_fb_large_chunk @ 004197ec ====

uint xcode_dma_pattern_to_fb_large_chunk
               (xcode_handle_t XCodeHandle,uint fb_addr,uint pattern,uint size)

{
  uint uVar1;
  uint local_res4;
  uint local_resc;
  
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



// ==== xcode_ts_control @ 004198b4 ====

uint xcode_ts_control(xcode_handle_t XCodeHandle,IOCTLSDEVTSCTRL *pioctl_ts_ctrl,int hStream,
                     uint parmsChanged)

{
  uint uVar1;
  
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



// ==== dvbpsi_new @ 00419a00 ====

dvbpsi_t * dvbpsi_new(dvbpsi_message_cb callback,dvbpsi_msg_level level)

{
  dvbpsi_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_t * p_dvbpsi@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);
  if (pdVar1 != (dvbpsi_t *)0x0) {
    pdVar1->p_decoder = (dvbpsi_decoder_t *)0x0;
    pdVar1->pf_message = callback;
    pdVar1->i_msg_level = level;
  }
  return pdVar1;
}



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



// ==== dvbpsi_decoder_new @ 00419ab0 ====

void * dvbpsi_decoder_new(dvbpsi_callback_gather_t pf_gather,int i_section_max_size,
                         _Bool b_discontinuity,size_t psi_size)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  
                    /* Unresolved local var: dvbpsi_decoder_t * p_decoder@[???] */
  if (psi_size < 0x1c) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("psi_size >= sizeof(dvbpsi_decoder_t)","dvbpsi.c",0x51,"dvbpsi_decoder_new");
  }
  puVar1 = calloc(1,psi_size);
  puVar2 = (undefined2 *)0x0;
  if (puVar1 != (undefined2 *)0x0) {
    *(dvbpsi_callback_gather_t *)(puVar1 + 8) = pf_gather;
    *puVar1 = 0x7370;
    *(undefined1 *)(puVar1 + 1) = 0x69;
    *(undefined1 *)(puVar1 + 3) = 0xff;
    *(undefined4 *)(puVar1 + 4) = 0;
    *(int *)(puVar1 + 10) = i_section_max_size;
    *(_Bool *)(puVar1 + 2) = b_discontinuity;
    *(undefined1 *)((int)puVar1 + 5) = 0;
    *(undefined1 *)((int)puVar1 + 7) = 0;
    *(undefined4 *)(puVar1 + 6) = 0;
    *(undefined1 *)((int)puVar1 + 3) = 0;
    puVar2 = puVar1;
  }
  return puVar2;
}



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



// ==== dvbpsi_decoder_psi_sections_completed @ 00419bf0 ====

_Bool dvbpsi_decoder_psi_sections_completed(dvbpsi_decoder_t *p_decoder)

{
  _Bool _Var1;
  dvbpsi_psi_section_t *pdVar2;
  uint uVar3;
  uint uVar4;
  
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
        _Var1 = true;
      }
      if (pdVar2 == (dvbpsi_psi_section_s *)0x0) {
        return _Var1;
      }
      uVar4 = uVar3 + 1;
      if (uVar4 == 0x100) {
                    /* WARNING: Subroutine does not return */
        __assert_fail(s_prev_nr_<_256_00424318,"dvbpsi.c",0x84,
                      "dvbpsi_decoder_psi_sections_completed");
      }
      uVar3 = (uint)pdVar2->i_number;
    } while (uVar3 == uVar4);
  }
  return _Var1;
}



// ==== dvbpsi_decoder_psi_section_add @ 00419cac ====

_Bool dvbpsi_decoder_psi_section_add(dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  _Bool _Var2;
  dvbpsi_psi_section_t *p_section_00;
  dvbpsi_psi_section_t *p_section_01;
  
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
    p_decoder->p_sections = p_section;
    p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    return false;
  }
  bVar1 = p_section->i_number;
  if (p_section_01->i_number == bVar1) {
    p_section->p_next = p_section_01->p_next;
    p_section_01->p_next = (dvbpsi_psi_section_s *)0x0;
    dvbpsi_DeletePSISections(p_section_01);
    _Var2 = true;
    p_decoder->p_sections = p_section;
  }
  else if (bVar1 < p_section_01->i_number) {
    p_section->p_next = p_section_01;
    _Var2 = false;
    p_decoder->p_sections = p_section;
  }
  else {
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
    _Var2 = false;
    if (p_section_01->i_number < bVar1) {
      p_section_01->p_next = p_section;
      p_section->p_next = (dvbpsi_psi_section_s *)0x0;
    }
  }
  return _Var2;
}



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



// ==== dvbpsi_error @ 00419ef4 ====

void dvbpsi_error(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
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



// ==== dvbpsi_packet_push @ 00419fe4 ====

_Bool dvbpsi_packet_push(dvbpsi_t *p_dvbpsi,uint8_t *p_data)

{
  uint8_t uVar1;
  bool bVar2;
  _Bool _Var3;
  uint uVar4;
  int iVar5;
  uint8_t *puVar6;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  uint8_t *puVar7;
  char cVar8;
  uint uVar9;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_decoder_t *pdVar10;
  uint8_t *__n;
  byte *__src;
  byte *pbVar11;
  byte *pbVar12;
  
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
  if (*p_data != 0x47) {
    dvbpsi_error(p_dvbpsi,"PSI decoder","not a TS packet");
    return false;
  }
  if (pdVar10->i_continuity_counter == 0xff) {
    pdVar10->i_continuity_counter = p_data[3] & 0xf;
  }
  else {
    uVar4 = pdVar10->i_continuity_counter + 1 & 0xf;
    uVar9 = p_data[3] & 0xf;
    pdVar10->i_continuity_counter = (uint8_t)uVar9;
    if ((uVar4 == (uVar9 + 1 & 0xf)) && (pdVar10->b_discontinuity == false)) {
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
  if ((p_data[3] & 0x10) == 0) {
    return false;
  }
  __src = p_data + 4;
  if ((p_data[3] & 0x20) != 0) {
    __src = p_data + p_data[4] + 5;
  }
  pbVar11 = (byte *)0x0;
  if ((p_data[1] & 0x40) != 0) {
    pbVar11 = __src + *__src + 1;
    __src = __src + 1;
  }
  p_section = pdVar10->p_current_section;
  pbVar12 = pbVar11;
  if (p_section == (dvbpsi_psi_section_t *)0x0) {
    if (pbVar11 == (byte *)0x0) {
      return false;
    }
    p_section = dvbpsi_NewPSISection(pdVar10->i_section_max_size);
    pdVar10->p_current_section = p_section;
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return false;
    }
    pdVar10->i_need = 3;
    pdVar10->b_complete_header = false;
    pbVar12 = (byte *)0x0;
    __src = pbVar11;
  }
  __n = p_data + (0xbc - (int)__src);
  if ((int)__n < 1) {
    return true;
  }
  do {
    while( true ) {
      if ((int)__n < pdVar10->i_need) {
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
      uVar1 = p_section->i_table_id;
      bVar2 = false;
      if (((2 < (byte)(uVar1 + 0x90)) && (uVar1 != 0x7e)) &&
         (bVar2 = true, p_section->b_syntax_indicator == false)) {
        bVar2 = uVar1 == 0x73;
      }
      puVar6 = p_section->p_data;
      _Var3 = (_Bool)-((char)puVar6[1] >> 7);
      p_section->b_syntax_indicator = _Var3;
      p_section->b_private_indicator = SUB41((puVar6[1] & 0x7f) >> 6,0);
      if (_Var3 == false) {
        cVar8 = '\0';
        if (!bVar2) {
LAB_0041a39c:
          p_section->i_table_id = *puVar6;
          if (cVar8 != '\0') goto LAB_0041a3a8;
LAB_0041a424:
          p_section->i_extension = 0;
          p_section->i_version = 0;
          p_section->b_current_next = true;
          p_section->i_number = 0;
          p_section->i_last_number = 0;
          p_section->p_payload_start = puVar6 + 3;
          goto LAB_0041a3ec;
        }
        p_section->p_payload_end = puVar7 + -4;
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
        p_section->p_payload_end = puVar7 + -4;
        if (bVar2) goto LAB_0041a314;
        p_section->i_table_id = *puVar6;
        if (_Var3 == false) goto LAB_0041a424;
LAB_0041a3a8:
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
        if (*__src == 0xff) {
          return true;
        }
      }
LAB_0041a218:
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
    puVar7 = p_section->p_data;
    pdVar10->b_complete_header = true;
    iVar5 = pdVar10->i_section_max_size;
    uVar4 = (puVar7[1] & 0xf) << 8 | (uint)puVar7[2];
    p_section->i_length = (uint16_t)uVar4;
    pdVar10->i_need = uVar4;
    if (iVar5 + -2 <= (int)uVar4) {
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



// ==== dvbpsi_warning @ 0041a588 ====

void dvbpsi_warning(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
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



// ==== dvbpsi_debug @ 0041a678 ====

void dvbpsi_debug(dvbpsi_t *dvbpsi,char *src,char *fmt,...)

{
  int iVar1;
  int iVar2;
  undefined4 in_a3;
  undefined4 local_resc;
  char *local_1c;
  char *local_18 [2];
  
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



// ==== dvbpsi_NewPSISection @ 0041a770 ====

dvbpsi_psi_section_t * dvbpsi_NewPSISection(int i_max_size)

{
  dvbpsi_psi_section_t *__ptr;
  uint8_t *puVar1;
  
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



// ==== dvbpsi_DeletePSISections @ 0041a800 ====

void dvbpsi_DeletePSISections(dvbpsi_psi_section_t *p_section)

{
  dvbpsi_psi_section_s *pdVar1;
  
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



// ==== dvbpsi_CheckPSISection @ 0041a86c ====

_Bool dvbpsi_CheckPSISection
                (dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section,uint8_t table_id,
                char *psz_table_name)

{
  uint uVar1;
  
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,(char *)((int)&DAT_004244f9 + 3),0xc4,
                  "dvbpsi_CheckPSISection");
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
  if ((p_section->b_syntax_indicator == false) && (uVar1 != 0x73)) {
    dvbpsi_error(p_dvbpsi,psz_table_name,"invalid section (section_syntax_indicator == 0)");
    return false;
  }
  dvbpsi_debug(p_dvbpsi,psz_table_name,
               "Table version %2d, i_extension %5d, section %3d up to %3d, current %1d",
               (uint)p_section->i_version,(uint)p_section->i_extension,(uint)p_section->i_number,
               (uint)p_section->i_last_number,(uint)p_section->b_current_next);
  return true;
}



// ==== dvbpsi_ValidPSISection @ 0041a98c ====

_Bool dvbpsi_ValidPSISection(dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  bool bVar2;
  uint uVar3;
  byte *pbVar4;
  
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



// ==== dvbpsi_CalculateCRC32 @ 0041a9e8 ====

void dvbpsi_CalculateCRC32(dvbpsi_psi_section_t *p_section)

{
  byte *pbVar1;
  byte bVar2;
  uint uVar3;
  byte *pbVar4;
  
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
  *pbVar4 = bVar2;
  p_section->p_payload_end[1] = (uint8_t)*(undefined2 *)((int)&p_section->i_crc + 2);
  p_section->p_payload_end[2] = (uint8_t)(p_section->i_crc >> 8);
  p_section->p_payload_end[3] = (uint8_t)p_section->i_crc;
  return;
}



// ==== dvbpsi_BuildPSISection @ 0041aa74 ====

void dvbpsi_BuildPSISection(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint8_t uVar1;
  byte bVar2;
  _Bool _Var3;
  undefined3 extraout_var;
  byte bVar4;
  
  bVar4 = 0x80;
  bVar2 = 0x40;
  *p_section->p_data = p_section->i_table_id;
  if (p_section->b_syntax_indicator == false) {
    bVar4 = 0;
  }
  if (p_section->b_private_indicator == false) {
    bVar2 = 0;
  }
  p_section->p_data[1] = bVar2 | bVar4 | (byte)(p_section->i_length >> 8) & 0xf | 0x30;
  p_section->p_data[2] = (uint8_t)p_section->i_length;
  if (p_section->b_syntax_indicator != false) {
    p_section->p_data[3] = (uint8_t)(p_section->i_extension >> 8);
    p_section->p_data[4] = (uint8_t)p_section->i_extension;
    p_section->p_data[5] = p_section->b_current_next | 0xc0U | (p_section->i_version & 0x1f) << 1;
    p_section->p_data[6] = p_section->i_number;
    p_section->p_data[7] = p_section->i_last_number;
  }
  uVar1 = p_section->i_table_id;
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



// ==== dvbpsi_sdt_attach @ 0041ac60 ====

_Bool dvbpsi_sdt_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_sdt_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x3b,"dvbpsi_sdt_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x3c,"dvbpsi_sdt_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x34);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_sdt_detach,dvbpsi_sdt_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_sdt_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[1].i_section_max_size = 0;
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



// ==== dvbpsi_sdt_init @ 0041ae10 ====

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



// ==== dvbpsi_sdt_new @ 0041ae84 ====

dvbpsi_sdt_t *
dvbpsi_sdt_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_network_id)

{
  dvbpsi_sdt_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_sdt_t * p_sdt@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);
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



// ==== dvbpsi_sdt_empty @ 0041af0c ====

void dvbpsi_sdt_empty(dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_sdt_service_t *__ptr;
  dvbpsi_sdt_service_s *pdVar1;
  
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



// ==== dvbpsi_sdt_delete @ 0041af80 ====

void dvbpsi_sdt_delete(dvbpsi_sdt_t *p_sdt)

{
  if (p_sdt != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_empty(p_sdt);
  }
  free(p_sdt);
  return;
}



// ==== dvbpsi_ReInitSDT @ 0041afc4 ====

void dvbpsi_ReInitSDT(dvbpsi_sdt_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;
  
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



// ==== dvbpsi_sdt_detach @ 0041b050 ====

void dvbpsi_sdt_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
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
  if ((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_delete((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size);
  }
  pdVar1[1].i_section_max_size = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_sdt_service_add @ 0041b1c0 ====

dvbpsi_sdt_service_t *
dvbpsi_sdt_service_add
          (dvbpsi_sdt_t *p_sdt,uint16_t i_service_id,_Bool b_eit_schedule,_Bool b_eit_present,
          uint8_t i_running_status,_Bool b_free_ca)

{
  dvbpsi_sdt_service_t *pdVar1;
  dvbpsi_sdt_service_t *pdVar2;
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);
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



// ==== dvbpsi_sdt_service_descriptor_add @ 0041b288 ====

dvbpsi_descriptor_t *
dvbpsi_sdt_service_descriptor_add
          (dvbpsi_sdt_service_t *p_service,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_sdt_sections_decode @ 0041b320 ====

void dvbpsi_sdt_sections_decode(dvbpsi_sdt_t *p_sdt,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  byte bVar2;
  uint8_t *puVar3;
  dvbpsi_sdt_service_t *p_service;
  uint8_t *puVar4;
  uint8_t *p_data;
  uint8_t *puVar5;
  uint8_t *puVar6;
  uint8_t *puVar7;
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar4 = p_section->p_payload_end;
    puVar5 = p_section->p_payload_start + 3;
    puVar3 = p_section->p_payload_start + 7;
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
        puVar7 = puVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2);
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



// ==== dvbpsi_sdt_sections_gather @ 0041b45c ====

void dvbpsi_sdt_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;
  _Bool _Var2;
  undefined3 extraout_var;
  int iVar3;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  dvbpsi_psi_section_t *pdVar4;
  dvbpsi_sdt_t *pdVar5;
  undefined4 uVar6;
  dvbpsi_psi_section_t *p_section_00;
  dvbpsi_psi_section_t *pdVar7;
  dvbpsi_decoder_t *pdVar8;
  
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
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x42,"SDT decoder");
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041b694;
  pdVar8 = p_dvbpsi->p_decoder;
  if (pdVar8->b_discontinuity == false) {
    iVar3 = p_private_decoder[1].i_section_max_size;
    if (iVar3 == 0) {
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
  (**(code **)(p_private_decoder + 1))(uVar6,iVar3);
  dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,false);
  return;
}



// ==== dvbpsi_sdt_sections_generate @ 0041b7f4 ====

dvbpsi_psi_section_t * dvbpsi_sdt_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_psi_section_s **ppdVar1;
  _Bool _Var2;
  uint16_t uVar3;
  uint8_t uVar4;
  dvbpsi_psi_section_t *pdVar5;
  dvbpsi_descriptor_t *pdVar6;
  uint8_t *puVar7;
  dvbpsi_psi_section_t *pdVar8;
  uint uVar9;
  uint8_t *puVar10;
  byte bVar11;
  int iVar12;
  dvbpsi_psi_section_t *p_section;
  dvbpsi_psi_section_t *pdVar13;
  dvbpsi_sdt_service_t *pdVar14;
  uint8_t *puVar15;
  byte *pbVar16;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar14 = p_sdt->p_first_service;
  pdVar5->i_table_id = 0x42;
  uVar3 = p_sdt->i_extension;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = true;
  pdVar5->i_length = 0xc;
  pdVar5->i_extension = uVar3;
  pdVar5->i_version = p_sdt->i_version;
  _Var2 = p_sdt->b_current_next;
  pdVar5->i_number = 0;
  uVar3 = p_sdt->i_network_id;
  pdVar5->b_current_next = _Var2;
  pdVar5->p_payload_end = pdVar5->p_payload_end + 0xb;
  pdVar5->p_payload_start = pdVar5->p_data + 8;
  pdVar5->p_data[8] = (uint8_t)(uVar3 >> 8);
  pdVar5->p_data[9] = (uint8_t)p_sdt->i_network_id;
  pdVar5->p_data[10] = 0xff;
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
      uVar9 = 5;
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
    bVar11 = 0xfc;
    pbVar16 = puVar15 + 3;
    *puVar15 = (uint8_t)(pdVar14->i_service_id >> 8);
    puVar15[1] = (uint8_t)pdVar14->i_service_id;
    if (pdVar14->b_eit_schedule != false) {
      bVar11 = 0xfe;
    }
    puVar15[2] = bVar11 | pdVar14->b_eit_present;
    puVar15[3] = (pdVar14->b_free_ca & 1U) << 4 | pdVar14->i_running_status << 5;
    pdVar6 = pdVar14->p_first_descriptor;
    puVar7 = pdVar8->p_payload_end + 5;
    pdVar8->p_payload_end = puVar7;
    pdVar8->i_length = pdVar8->i_length + 5;
    pdVar13 = pdVar8;
    if (pdVar6 == (dvbpsi_descriptor_t *)0x0) break;
    puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    while ((int)puVar10 < 0x3fb) {
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
  *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
  puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
  pdVar14 = pdVar14->p_next;
  goto joined_r0x0041b8c0;
}



// ==== NewEITSection @ 0041bbe0 ====

dvbpsi_psi_section_t * NewEITSection(dvbpsi_eit_t *p_eit,int i_table_id,int i_section_number)

{
  _Bool _Var1;
  uint16_t uVar2;
  dvbpsi_psi_section_t *pdVar3;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[DW_OP_reg2(v0)] */
  pdVar3 = dvbpsi_NewPSISection(0xffe);
  pdVar3->i_table_id = (uint8_t)i_table_id;
  uVar2 = p_eit->i_extension;
  pdVar3->b_syntax_indicator = true;
  pdVar3->b_private_indicator = true;
  pdVar3->i_length = 0xf;
  pdVar3->i_extension = uVar2;
  pdVar3->i_version = p_eit->i_version;
  _Var1 = p_eit->b_current_next;
  pdVar3->i_number = (uint8_t)i_section_number;
  uVar2 = p_eit->i_ts_id;
  pdVar3->b_current_next = _Var1;
  pdVar3->p_payload_end = pdVar3->p_payload_end + 0xe;
  pdVar3->p_payload_start = pdVar3->p_data + 8;
  pdVar3->p_data[8] = (uint8_t)(uVar2 >> 8);
  pdVar3->p_data[9] = (uint8_t)p_eit->i_ts_id;
  pdVar3->p_data[10] = (uint8_t)(p_eit->i_network_id >> 8);
  pdVar3->p_data[0xb] = (uint8_t)p_eit->i_network_id;
  pdVar3->p_data[0xd] = p_eit->i_last_table_id;
  return pdVar3;
}



// ==== dvbpsi_eit_attach @ 0041bcb4 ====

_Bool dvbpsi_eit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_eit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
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
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_eit_detach,dvbpsi_eit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_eit_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[1].i_need = 0;
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



// ==== dvbpsi_eit_init @ 0041be64 ====

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



// ==== dvbpsi_eit_new @ 0041bea0 ====

dvbpsi_eit_t *
dvbpsi_eit_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_ts_id,uint16_t i_network_id,uint8_t i_segment_last_section_number,
              uint8_t i_last_table_id)

{
  dvbpsi_eit_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_eit_t * p_eit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);
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



// ==== dvbpsi_eit_empty @ 0041bf58 ====

void dvbpsi_eit_empty(dvbpsi_eit_t *p_eit)

{
  dvbpsi_eit_event_t *__ptr;
  dvbpsi_eit_event_s *pdVar1;
  
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



// ==== dvbpsi_eit_delete @ 0041bfcc ====

void dvbpsi_eit_delete(dvbpsi_eit_t *p_eit)

{
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_empty(p_eit);
  }
  free(p_eit);
  return;
}



// ==== dvbpsi_eit_detach @ 0041c010 ====

void dvbpsi_eit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_eit_t *p_eit;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
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
  p_eit = (dvbpsi_eit_t *)pdVar1[1].i_need;
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_eit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_ReInitEIT @ 0041c158 ====

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



// ==== dvbpsi_eit_event_add @ 0041c1d0 ====

dvbpsi_eit_event_t *
dvbpsi_eit_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint64_t i_start_time,uint32_t i_duration,
          uint8_t i_running_status,_Bool b_free_ca,uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;
  dvbpsi_eit_event_t *pdVar2;
  uint local_20;
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x28);
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



// ==== dvbpsi_eit_nvod_event_add @ 0041c2c0 ====

dvbpsi_eit_event_t *
dvbpsi_eit_nvod_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint32_t i_duration,_Bool b_free_ca,
          uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;
  
  pdVar1 = dvbpsi_eit_event_add
                     (p_eit,i_event_id,0xfffff000,i_duration,0,b_free_ca,i_event_descriptor_length);
  return pdVar1;
}



// ==== dvbpsi_eit_event_descriptor_add @ 0041c2fc ====

dvbpsi_descriptor_t *
dvbpsi_eit_event_descriptor_add
          (dvbpsi_eit_event_t *p_event,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_eit_sections_decode @ 0041c394 ====

void dvbpsi_eit_sections_decode(dvbpsi_eit_t *p_eit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  dvbpsi_eit_event_t *p_event;
  uint8_t *puVar2;
  uint8_t *puVar3;
  uint uVar4;
  uint8_t *puVar5;
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar5 = p_section->p_payload_end;
    puVar3 = p_section->p_payload_start + 6;
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
        puVar3 = puVar3 + 0xc;
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



// ==== dvbpsi_eit_sections_gather @ 0041c518 ====

void dvbpsi_eit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  short sVar2;
  _Bool _Var3;
  undefined3 extraout_var;
  int iVar4;
  undefined3 extraout_var_00;
  uint uVar5;
  dvbpsi_psi_section_t *pdVar6;
  uint8_t *puVar7;
  dvbpsi_callback_gather_t p_Var8;
  dvbpsi_eit_t *pdVar9;
  undefined4 uVar10;
  undefined1 b_force;
  dvbpsi_psi_section_t *pdVar11;
  uint8_t table_id;
  dvbpsi_psi_section_t *pdVar12;
  dvbpsi_decoder_t *pdVar13;
  
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
  table_id = 0x4e;
  if ((byte)(p_section->i_table_id + 0xb2) < 0x22) {
    table_id = p_section->i_table_id;
  }
  pdVar11 = p_section;
  _Var3 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"EIT decoder");
  if (CONCAT31(extraout_var,_Var3) == 0) goto LAB_0041c7e8;
  pdVar13 = p_dvbpsi->p_decoder;
  if (pdVar13->b_discontinuity == false) {
    iVar4 = p_private_decoder[1].i_need;
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
    p_private_decoder[2].i_magic[0] = p_section->i_number;
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
          (**(code **)(p_private_decoder + 1))(uVar10,iVar4);
          p_private_decoder[1].i_need = 0;
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



// ==== dvbpsi_eit_sections_generate @ 0041c960 ====

dvbpsi_psi_section_t *
dvbpsi_eit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_eit_t *p_eit,uint8_t i_table_id)

{
  dvbpsi_psi_section_s **ppdVar1;
  int iVar2;
  dvbpsi_psi_section_t *pdVar3;
  dvbpsi_descriptor_t *pdVar4;
  uint8_t *puVar5;
  dvbpsi_psi_section_t *pdVar6;
  uint uVar7;
  uint8_t *puVar8;
  dvbpsi_psi_section_t *pdVar9;
  dvbpsi_eit_event_t *pdVar10;
  uint8_t *puVar11;
  uint i_section_number;
  byte *pbVar12;
  
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
          uVar7 = pdVar4->i_length + 0xe;
          iVar2 = uVar7 - (int)pdVar9->p_data;
          while ((int)(puVar11 + iVar2) < 0xffb) {
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
        puVar11[2] = (uint8_t)*(undefined4 *)((int)&pdVar10->i_start_time + 4);
        puVar11[3] = *(uint8_t *)((int)&pdVar10->i_start_time + 3);
        puVar11[4] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_start_time + 2);
        puVar11[5] = (uint8_t)(pdVar10->i_start_time >> 8);
        puVar11[6] = (uint8_t)pdVar10->i_start_time;
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
        *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
        puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
        pdVar10 = pdVar10->p_next;
      } while (pdVar10 != (dvbpsi_eit_event_s *)0x0);
    }
    pdVar9 = pdVar3;
LAB_0041cba4:
    do {
      pdVar9->p_data[0xc] = (uint8_t)i_section_number;
      pdVar9->i_last_number = (uint8_t)i_section_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar9);
      ppdVar1 = &pdVar9->p_next;
      pdVar9 = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_s *)0x0);
  }
  return pdVar3;
}



// ==== dvbpsi_nit_attach @ 0041cc80 ====

_Bool dvbpsi_nit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_nit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
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
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_nit_detach,dvbpsi_nit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(uint16_t *)p_decoder[2].i_magic = i_extension;
        *(dvbpsi_nit_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[1].i_need = 0;
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



// ==== dvbpsi_nit_init @ 0041ce34 ====

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



// ==== dvbpsi_nit_new @ 0041ce5c ====

dvbpsi_nit_t *
dvbpsi_nit_new(uint8_t i_table_id,uint16_t i_extension,uint16_t i_network_id,uint8_t i_version,
              _Bool b_current_next)

{
  dvbpsi_nit_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_nit_t * p_nit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);
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



// ==== dvbpsi_nit_empty @ 0041cee8 ====

void dvbpsi_nit_empty(dvbpsi_nit_t *p_nit)

{
  dvbpsi_nit_ts_t *__ptr;
  dvbpsi_nit_ts_s *pdVar1;
  
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



// ==== dvbpsi_nit_delete @ 0041cf70 ====

void dvbpsi_nit_delete(dvbpsi_nit_t *p_nit)

{
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_empty(p_nit);
  }
  free(p_nit);
  return;
}



// ==== dvbpsi_ReInitNIT @ 0041cfb4 ====

void dvbpsi_ReInitNIT(dvbpsi_nit_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;
  
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



// ==== dvbpsi_nit_detach @ 0041d040 ====

void dvbpsi_nit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_nit_t *p_nit;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
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
  p_nit = (dvbpsi_nit_t *)pdVar1[1].i_need;
  if (p_nit != (dvbpsi_nit_t *)0x0) {
    dvbpsi_nit_delete(p_nit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_nit_descriptor_add @ 0041d138 ====

dvbpsi_descriptor_t *
dvbpsi_nit_descriptor_add(dvbpsi_nit_t *p_nit,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *pdVar1;
  dvbpsi_descriptor_t *pdVar2;
  
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



// ==== dvbpsi_nit_ts_add @ 0041d1b0 ====

dvbpsi_nit_ts_t * dvbpsi_nit_ts_add(dvbpsi_nit_t *p_nit,uint16_t i_ts_id,uint16_t i_orig_network_id)

{
  dvbpsi_nit_ts_t *pdVar1;
  dvbpsi_nit_ts_t *pdVar2;
  
                    /* Unresolved local var: dvbpsi_nit_ts_t * p_ts@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);
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



// ==== dvbpsi_nit_ts_descriptor_add @ 0041d244 ====

dvbpsi_descriptor_t *
dvbpsi_nit_ts_descriptor_add(dvbpsi_nit_ts_t *p_ts,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_nit_sections_decode @ 0041d2dc ====

void dvbpsi_nit_sections_decode(dvbpsi_nit_t *p_nit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  uint8_t i_tag;
  byte bVar2;
  dvbpsi_nit_ts_t *p_ts;
  uint8_t *p_data;
  byte *pbVar3;
  uint uVar4;
  byte *pbVar5;
  byte *pbVar6;
  byte *pbVar7;
  byte *pbVar8;
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    pbVar6 = p_section->p_payload_start;
    pbVar3 = pbVar6 + 2;
    pbVar5 = pbVar3 + ((*pbVar6 & 0xf) << 8 | (uint)pbVar6[1]);
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
      bVar1 = *pbVar3;
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



// ==== dvbpsi_nit_sections_gather @ 0041d4d4 ====

void dvbpsi_nit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;
  _Bool _Var2;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  dvbpsi_descriptor_t *pdVar3;
  dvbpsi_nit_t *pdVar4;
  undefined4 uVar5;
  uint16_t i_extension;
  dvbpsi_psi_section_t *p_section_00;
  int iVar6;
  uint8_t table_id;
  uint16_t i_network_id;
  dvbpsi_psi_section_t *pdVar7;
  dvbpsi_psi_section_t *pdVar8;
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_nit_decoder_t * p_nit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/nit.c",0x16b,"dvbpsi_nit_sections_gather");
  }
  table_id = 0x40;
  if ((byte)(p_section->i_table_id - 0x40) < 2) {
    table_id = p_section->i_table_id;
  }
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"NIT decoder");
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041d6b0;
  i_extension = *(uint16_t *)p_private_decoder[2].i_magic;
  i_network_id = p_section->i_extension;
  if (i_network_id != i_extension) {
    dvbpsi_error(p_dvbpsi,"NIT decoder","\'network_id\' don\'t match");
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  if (p_private_decoder->b_discontinuity == false) {
    if (p_private_decoder[1].i_need != 0) {
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
  (**(code **)(p_private_decoder + 1))(uVar5,iVar6);
  dvbpsi_ReInitNIT((dvbpsi_nit_decoder_t *)p_private_decoder,false);
  return;
}



// ==== dvbpsi_nit_sections_generate @ 0041d808 ====

dvbpsi_psi_section_t *
dvbpsi_nit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_nit_t *p_nit,uint8_t i_table_id)

{
  uint8_t uVar1;
  _Bool _Var2;
  byte bVar3;
  uint16_t uVar4;
  dvbpsi_psi_section_t *pdVar5;
  uint8_t *puVar6;
  dvbpsi_psi_section_t *pdVar7;
  uint8_t *puVar8;
  uint uVar9;
  uint8_t *puVar10;
  uint8_t *puVar11;
  int iVar12;
  dvbpsi_descriptor_t *pdVar13;
  dvbpsi_psi_section_t *pdVar14;
  dvbpsi_nit_ts_s *pdVar15;
  
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
  pdVar5->i_length = 0xd;
  pdVar5->i_extension = uVar4;
  puVar6 = pdVar5->p_data;
  pdVar5->i_version = p_nit->i_version;
  puVar8 = pdVar5->p_payload_end + 10;
  pdVar5->b_current_next = p_nit->b_current_next;
  pdVar5->i_number = 0;
  pdVar5->p_payload_end = puVar8;
  pdVar5->p_payload_start = puVar6 + 8;
  pdVar7 = pdVar5;
  pdVar14 = pdVar5;
  if (pdVar13 != (dvbpsi_descriptor_t *)0x0) {
    while( true ) {
      pdVar7 = pdVar14;
      if (0x3fa < (int)(puVar8 + ((uint)pdVar13->i_length - (int)puVar6))) {
        puVar11 = pdVar14->p_payload_start;
        puVar6[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff) >> 8) | 0xf0;
        pdVar14->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar11)) & 0xffff);
        *pdVar14->p_payload_end = 0;
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
  puVar6 = pdVar7->p_payload_start;
  pdVar7->p_data[8] = (byte)(((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff) >> 8) | 0xf0;
  pdVar7->p_data[9] = (uint8_t)((uint)(puVar8 + (-2 - (int)puVar6)) & 0xffff);
  puVar8 = pdVar7->p_payload_end;
  puVar6 = puVar8 + 2;
  pdVar7->p_payload_end = puVar6;
  pdVar14 = pdVar7;
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
        pdVar7->p_data[8] = 0xf0;
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
      while ((int)puVar10 < 0x3fb) {
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



// ==== dvbpsi_tot_attach @ 0041dd50 ====

_Bool dvbpsi_tot_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_tot_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x3d,"dvbpsi_tot_attach");
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
        *(dvbpsi_tot_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
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



// ==== dvbpsi_tot_new @ 0041df80 ====

dvbpsi_tot_t *
dvbpsi_tot_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint64_t i_utc_time)

{
  dvbpsi_tot_t *pdVar1;
  
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



// ==== dvbpsi_tot_empty @ 0041e028 ====

void dvbpsi_tot_empty(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  
  puVar1 = (undefined1 *)((int)&p_tot->p_first_descriptor + 3);
  uVar2 = (uint)puVar1 & 3;
  uVar3 = (uint)&p_tot->p_first_descriptor & 3;
  dvbpsi_DeleteDescriptors
            ((dvbpsi_descriptor_t *)
             ((*(int *)(puVar1 + -uVar2) << (3 - uVar2) * 8 |
              (uint)p_tot & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
             *(uint *)((int)&p_tot->p_first_descriptor - uVar3) >> uVar3 * 8));
  p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  return;
}



// ==== dvbpsi_tot_delete @ 0041e06c ====

void dvbpsi_tot_delete(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  
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



// ==== dvbpsi_ReInitTOT @ 0041e0c0 ====

void dvbpsi_ReInitTOT(dvbpsi_tot_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;
  
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



// ==== dvbpsi_tot_detach @ 0041e14c ====

void dvbpsi_tot_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
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
  if (*(dvbpsi_tot_t **)(pdVar1 + 2) != (dvbpsi_tot_t *)0x0) {
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



// ==== dvbpsi_tot_descriptor_add @ 0041e258 ====

dvbpsi_descriptor_t *
dvbpsi_tot_descriptor_add(dvbpsi_tot_t *p_tot,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_tot_sections_decode @ 0041e2f8 ====

void dvbpsi_tot_sections_decode
               (dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  byte bVar2;
  uint8_t uVar3;
  byte *pbVar4;
  byte i_tag;
  uint8_t *p_data;
  byte *pbVar5;
  byte *pbVar6;
  uint uVar7;
  
  if (p_section != (dvbpsi_psi_section_t *)0x0) {
    uVar3 = p_section->i_table_id;
                    /* Unresolved local var: uint8_t * p_byte@[???] */
    if ((uVar3 == 0x70) && (p_section->i_length != 5)) {
      dvbpsi_error(p_dvbpsi,"TDT decoder","TDT has an invalid payload size (%d bytes) !!!");
      return;
    }
    pbVar5 = p_section->p_payload_start;
    pbVar4 = p_section->p_payload_end;
    if (pbVar5 + 5 <= pbVar4) {
      bVar1 = *pbVar5;
      *(uint *)&p_tot->i_utc_time =
           (uint)pbVar5[4] | (uint)pbVar5[1] << 0x18 | (uint)pbVar5[2] << 0x10 |
           (uint)pbVar5[3] << 8;
      *(uint *)((int)&p_tot->i_utc_time + 4) = (uint)bVar1;
      uVar3 = p_section->i_table_id;
      pbVar5 = pbVar5 + 5;
    }
    p_data = pbVar5 + 4;
    if (uVar3 == 0x73) {
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



// ==== dvbpsi_tot_sections_gather @ 0041e484 ====

void dvbpsi_tot_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  uint8_t table_id;
  _Bool _Var1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;
  int iVar3;
  undefined3 extraout_var_02;
  uint8_t *puVar4;
  dvbpsi_callback_gather_t p_Var5;
  dvbpsi_psi_section_t *pdVar6;
  undefined4 uVar7;
  dvbpsi_tot_t *pdVar8;
  undefined4 uVar9;
  dvbpsi_psi_section_t *p_section_00;
  
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
    iVar3 = *(int *)(p_decoder + 2);
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
  uVar7._0_1_ = p_decoder[1].b_discontinuity;
  uVar7._1_1_ = p_decoder[1].b_current_valid;
  uVar7._2_1_ = p_decoder[1].i_continuity_counter;
  uVar7._3_1_ = p_decoder[1].i_last_section_number;
  uVar9._0_1_ = p_decoder[2].i_magic[0];
  uVar9._1_1_ = p_decoder[2].i_magic[1];
  uVar9._2_1_ = p_decoder[2].i_magic[2];
  uVar9._3_1_ = p_decoder[2].b_complete_header;
  p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_decoder + 1))(uVar7,uVar9);
  dvbpsi_ReInitTOT((dvbpsi_tot_decoder_t *)p_decoder,false);
  return;
}



// ==== dvbpsi_tot_sections_generate @ 0041e808 ====

dvbpsi_psi_section_t * dvbpsi_tot_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot)

{
  dvbpsi_psi_section_t *p_section;
  uint8_t *puVar1;
  uint8_t uVar2;
  uint8_t *puVar3;
  dvbpsi_descriptor_t *pdVar4;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  pdVar4 = p_tot->p_first_descriptor;
  if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
    p_section = dvbpsi_NewPSISection(8);
    uVar2 = 0x70;
  }
  else {
    p_section = dvbpsi_NewPSISection(0x1000);
    uVar2 = 0x73;
  }
  puVar1 = p_section->p_data;
  p_section->i_table_id = uVar2;
  p_section->b_syntax_indicator = false;
  p_section->b_private_indicator = false;
  p_section->i_length = 5;
  p_section->p_payload_start = puVar1 + 3;
  p_section->p_payload_end = puVar1 + 8;
  puVar1[3] = (uint8_t)*(undefined4 *)((int)&p_tot->i_utc_time + 4);
  p_section->p_data[4] = (uint8_t)(p_tot->i_utc_time >> 0x18);
  p_section->p_data[5] = (uint8_t)(p_tot->i_utc_time >> 0x10);
  p_section->p_data[6] = (uint8_t)(p_tot->i_utc_time >> 8);
  p_section->p_data[7] = (uint8_t)p_tot->i_utc_time;
  if (p_section->i_table_id == 0x73) {
    puVar1 = p_section->p_payload_end + 2;
    p_section->p_payload_end = puVar1;
    p_section->i_length = p_section->i_length + 2;
    if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
LAB_0041eb04:
      p_section->p_payload_start[5] = 0xf0;
      p_section->p_payload_start[6] = (char)p_section->i_length - 7;
    }
    else {
      puVar3 = puVar1 + ((uint)pdVar4->i_length - (int)p_section->p_data);
      while ((int)puVar3 < 0xffb) {
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



// ==== dvbpsi_Demux @ 0041eb90 ====

void dvbpsi_Demux(dvbpsi_t *p_dvbpsi,dvbpsi_psi_section_t *p_section)

{
  uint *puVar1;
  uint uVar2;
  dvbpsi_decoder_t *pdVar3;
  code *UNRECOVERED_JUMPTABLE;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x68,"dvbpsi_Demux");
  }
  pdVar3 = p_dvbpsi->p_decoder;
  if (pdVar3 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","demux.c",0x69,"dvbpsi_Demux");
  }
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  puVar1 = *(uint **)(pdVar3 + 1);
  if (puVar1 == (uint *)0x0) {
LAB_0041ec0c:
    (**(code **)&pdVar3[1].b_discontinuity)
              (p_dvbpsi,p_section->i_table_id,p_section->i_extension,pdVar3[1].p_current_section);
                    /* Unresolved local var: uint32_t i_id@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
    puVar1 = *(uint **)(pdVar3 + 1);
    if (puVar1 == (uint *)0x0) {
LAB_0041ec64:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    uVar2 = (uint)CONCAT12(p_section->i_table_id,p_section->i_extension);
    if (uVar2 == *puVar1) {
      UNRECOVERED_JUMPTABLE = (code *)puVar1[1];
      goto LAB_0041ec88;
    }
    do {
      puVar1 = (uint *)puVar1[4];
      if (puVar1 == (uint *)0x0) goto LAB_0041ec64;
    } while (uVar2 != *puVar1);
  }
  else {
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
  (*UNRECOVERED_JUMPTABLE)(p_dvbpsi,puVar1[2],p_section);
  return;
}



// ==== dvbpsi_AttachDemux @ 0041ecf0 ====

_Bool dvbpsi_AttachDemux(dvbpsi_t *p_dvbpsi,dvbpsi_demux_new_cb_t pf_new_cb,void *p_new_cb_data)

{
  dvbpsi_decoder_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"demux.c",0x38,"dvbpsi_AttachDemux");
  }
  if (p_dvbpsi->p_decoder != (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder == ((void *)0)","demux.c",0x39,"dvbpsi_AttachDemux");
  }
  pdVar1 = dvbpsi_decoder_new(dvbpsi_Demux,0x1000,true,0x28);
  if (pdVar1 != (dvbpsi_decoder_t *)0x0) {
    pdVar1[1].i_magic[0] = 0;
    pdVar1[1].i_magic[1] = 0;
    pdVar1[1].i_magic[2] = 0;
    pdVar1[1].b_complete_header = false;
    *(dvbpsi_demux_new_cb_t *)&pdVar1[1].b_discontinuity = pf_new_cb;
    pdVar1[1].p_current_section = p_new_cb_data;
    p_dvbpsi->p_decoder = pdVar1;
  }
  return pdVar1 != (dvbpsi_decoder_t *)0x0;
}



// ==== dvbpsi_demuxGetSubDec @ 0041edc4 ====

dvbpsi_demux_subdec_t *
dvbpsi_demuxGetSubDec(dvbpsi_demux_t *p_demux,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *pdVar1;
  uint32_t uVar2;
  
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



// ==== dvbpsi_DetachDemux @ 0041ee10 ====

void dvbpsi_DetachDemux(dvbpsi_t *p_dvbpsi)

{
  dvbpsi_decoder_t *p_decoder;
  uint *__ptr;
  uint *puVar1;
  code *pcVar2;
  
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
  __ptr = *(uint **)(p_decoder + 1);
  if (__ptr != (uint *)0x0) {
                    /* Unresolved local var: dvbpsi_demux_subdec_t * p_subdec_temp@[???] */
    pcVar2 = (code *)__ptr[3];
    puVar1 = (uint *)__ptr[4];
    if (pcVar2 == (code *)0x0) goto LAB_0041ee84;
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



// ==== dvbpsi_NewDemuxSubDecoder @ 0041ef04 ====

dvbpsi_demux_subdec_t *
dvbpsi_NewDemuxSubDecoder
          (uint8_t i_table_id,uint16_t i_extension,dvbpsi_demux_detach_cb_t pf_detach,
          dvbpsi_demux_gather_cb_t pf_gather,dvbpsi_decoder_t *p_decoder)

{
  dvbpsi_demux_subdec_t *pdVar1;
  
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



// ==== dvbpsi_DetachDemuxSubDecoder @ 0041f0b4 ====

void dvbpsi_DetachDemuxSubDecoder(dvbpsi_demux_t *p_demux,dvbpsi_demux_subdec_t *p_subdec)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_demux_subdec_s **ppdVar2;
  
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



// ==== dvbpsi_IsDescriptorDecoded @ 0041f170 ====

_Bool dvbpsi_IsDescriptorDecoded(dvbpsi_descriptor_t *p_descriptor)

{
  return p_descriptor->p_decoded != (void *)0x0;
}



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



// ==== dvbpsi_NewDescriptor @ 0041f19c ====

dvbpsi_descriptor_t * dvbpsi_NewDescriptor(uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *__ptr;
  uint8_t *__dest;
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  __ptr = malloc(0x10);
  if (__ptr != (dvbpsi_descriptor_t *)0x0) {
    __dest = malloc((size_t)i_length);
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



// ==== dvbpsi_AddDescriptor @ 0041f268 ====

dvbpsi_descriptor_t *
dvbpsi_AddDescriptor(dvbpsi_descriptor_t *p_list,dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_DeleteDescriptors @ 0041f2e0 ====

void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_s *pdVar1;
  
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



// ==== dvbpsi_DuplicateDecodedDescriptor @ 0041f364 ====

void * dvbpsi_DuplicateDecodedDescriptor(void *p_decoded,ssize_t i_size)

{
  void *__dest;
  
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



// ==== __libc_csu_init @ 0041f3e0 ====

/* WARNING: Removing unreachable block (ram,0x0041f440) */

void __libc_csu_init(int argc,char **argv,char **envp)

{
                    /* Unresolved local var: size_t size@[???] */
  _init((EVP_PKEY_CTX *)argc);
                    /* Unresolved local var: size_t i@[???] */
  return;
}



// ==== __libc_csu_fini @ 0041f488 ====

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void __libc_csu_fini(void)

{
  return;
}



// ==== stat64 @ 0041f490 ====

int stat64(char *file,stat64 *buf)

{
  int iVar1;
  
  iVar1 = __xstat64(3,file,(stat64 *)buf);
  return iVar1;
}



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



// ==== _fini @ 0041fa00 ====

void _fini(void)

{
  __do_global_dtors_aux();
  return;
}


