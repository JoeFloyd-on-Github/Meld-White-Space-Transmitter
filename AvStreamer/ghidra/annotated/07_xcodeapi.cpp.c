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


