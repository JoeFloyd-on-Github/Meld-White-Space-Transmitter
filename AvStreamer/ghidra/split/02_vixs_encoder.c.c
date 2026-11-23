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



