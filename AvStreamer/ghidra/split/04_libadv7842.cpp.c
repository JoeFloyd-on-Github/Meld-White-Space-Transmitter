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



