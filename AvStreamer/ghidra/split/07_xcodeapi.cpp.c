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



