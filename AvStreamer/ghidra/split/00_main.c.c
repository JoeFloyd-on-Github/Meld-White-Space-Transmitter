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



