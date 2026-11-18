/*
 * load_keys_good.app.annotated.c  --  annotated Ghidra decompile of the
 *   APPLICATION functions of root/bin/load_keys_good
 * ============================================================================
 * load_keys_good provisions the device's content-protection / secure keys into
 * the ViXS XCode SoC's hardware KEYSTORE, through /dev/vixs/xcodedrv. It opens a
 * secure ("scramble") command stream and writes key material into keystore
 * slots via a register sequence (0x1824/0x1828 = key words, 0x182c = control).
 * See docs/boot-process.md section 6.3.
 *
 * SCOPE: load_keys_good is ~2.7 MB, STATICALLY linked, 849 decompiled functions
 * of which the vast majority are standard glibc (string/stdio/locale/dynamic-
 * loader/unwind). This file contains ONLY the program's own functions (17),
 * the OEM hardware-control code; the glibc routines are omitted exactly as the
 * WhiteSpaceApp/ghidra analysis omits libc. Function names are original symbols
 * from the (non-stripped) symbol table; a few internal helpers remain FUN_<addr>.
 * Nothing here was produced by running the binary.
 */

/* ---- dma_hdmi_4210 @ 004002e0 ----
 * HDMI DMA helper for the 4210 part.
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

int dma_hdmi_4210(void)

{
  int iVar1;
  undefined1 auStack_180 [376];
  
                    /* Unresolved local var: int idx@[???]
                       Unresolved local var: pint8 pSrcBuffer@[???]
                       Unresolved local var: pint8 pTem@[???]
                       Unresolved local var: uint i@[???]
                       Unresolved local var: ulong[84] hdmi_encrypted@[???]
                       Unresolved local var: DWord cmd@[???]
                       Unresolved local var: DWord cw@[???]
                       Unresolved local var: DWord[8] hdmi_DMA@[???] */
  memcpy(auStack_180,C_0_7348,0x150);
  _IO_puts("Programming dpk ram ...");
  RegWrite(0x1a04,1);
  memcpy(scrambleStream.sendMem.pBuffer,auStack_180,0x150);
  scrambleStream.controlWord[0] = '\0';
  scrambleStream.scrambleMode = 5;
  iVar1 = AES_FB2FB(&scrambleStream,0x150);
  if (iVar1 == 1) {
    putchar(10);
    RegWrite(0x1a04,0);
    _IO_puts("DPK ram has been programmed.");
  }
  else {
    PerformCleanUp();
  }
  return 0;
}


/* ---- REG_POLL @ 00400450 ----
 * Lower-level register poll primitive used by RegPoll.
 */

int REG_POLL(int reg,int bit_mask,int value)

{
  bool bVar1;
  uint32 uVar2;
  int local_18;
  
                    /* Unresolved local var: int timeout@[???] */
  local_18 = 1000000;
  do {
    bVar1 = local_18 == 0;
    local_18 = local_18 + -1;
    if (bVar1) {
      return 1;
    }
    uVar2 = RegRead(reg);
  } while ((uVar2 & bit_mask) != value);
  return 0;
}


/* ---- DISP_HDMI_REG_WRITE @ 00400500 ----
 * Write an HDMI transmitter display register.
 */

void DISP_HDMI_REG_WRITE(int reg,int value)

{
  RegWrite(0xdb40,value);
  RegWrite(0xdb30,reg | 0x80000000);
  usleep(1);
  REG_POLL(0xdb3c,0xd,0xd);
  return;
}


/* ---- DISP_HDMI_REG_READ @ 00400598 ----
 * Read an HDMI transmitter display register (via the display/HDMI block).
 */

uint32 DISP_HDMI_REG_READ(int reg)

{
  uint32 uVar1;
  
  RegWrite(0xdb30,reg);
  usleep(1);
  REG_POLL(0xdb3c,0xd,0xd);
  uVar1 = RegRead(0xdb44);
  return uVar1;
}


/* ---- main @ 0040061c ----
 * Load the device's secure keys into the ViXS keystore. createXcodeAccess()
 * opens the xcode engine; xcode_open_generic() opens a 'scramble' (encrypted)
 * command stream; then it writes key material into the keystore by a fixed
 * RegWrite/RegPoll sequence on registers 0x1824/0x1828 (key data words) and
 * 0x182c (control: poll-ready, then trigger 0/1 per slot). The 64-bit values
 * written (e.g. 0x1603858c:0x464e4c8a and 0x7dd92308:0x4673d3fd) ARE the keys
 * loaded into keystore slots. PerformCleanUp() closes the handles.
 */

int main(int argc,char **argv)

{
  uint32 uVar1;
  uint64 uVar2;
  
                    /* Unresolved local var: int idx@[???]
                       Unresolved local var: pint8 pSrcBuffer@[???]
                       Unresolved local var: pint8 pTem@[???]
                       Unresolved local var: uint i@[???]
                       Unresolved local var: uchar[16] in_last_AES@[???]
                       Unresolved local var: int data@[???] */
  uVar1 = createXcodeAccess();
  if (uVar1 == 0) {
    PerformCleanUp();
                    /* WARNING: Subroutine does not return */
    exit(0);
  }
  uVar2 = xcode_open_generic(hXCode,&ioctl_opengeneric,0x4000,&scrambleStream);
  scrambleStream.strm_handle_generic._0_4_ = (int)uVar2;
  scrambleStream.strm_handle_generic._4_4_ = (int)scrambleStream.strm_handle_generic >> 0x1f;
  if ((int)scrambleStream.strm_handle_generic == 0) {
    __printf("\nFailed to open a generic stream");
  }
  else {
    RegPoll(0x182c,0x80000000,0x80000000,100,100);
    RegWrite(0x1824,0x1603858c);
    RegWrite(0x1828,0x464e4c8a);
    RegWrite(0x182c,0);
    RegPoll(0x182c,0x80000000,0x80000000,100,100);
    RegWrite(0x1824,0x7dd92308);
    RegWrite(0x1828,0x4673d3fd);
    RegWrite(0x182c,1);
    dma_hdmi_4210();
  }
  PerformCleanUp();
  return 0;
}


/* ---- AES_FB2FB @ 00400860 ----
 * Run the hardware AES engine frame-buffer -> frame-buffer (content path).
 */

int AES_FB2FB(pscramble_stream pscrambleStream,int NumOfBytes)

{
  uint32 uVar1;
  
                    /* Unresolved local var: uint32 flags@[???]
                       Unresolved local var: puint8 pControlWord@[???]
                       Unresolved local var: pint8 pSrcBuffer_phy@[???]
                       Unresolved local var: pint8 pDstBuffer_phy@[???]
                       Unresolved local var: int ret@[???] */
  uVar1 = xcode_scramble_stream
                    (hXCode,pscrambleStream->strm_handle_generic,&ioctl_scrambler,
                     (pscrambleStream->sendMem).pBuffer_phy,
                     (pscrambleStream->recvMem).pBufferGeneric_phy[0],1,NumOfBytes,
                     pscrambleStream->scrambleMode,0x120,pscrambleStream->controlWord,(puint8)0x0,
                     (puint8)0x0,(puint8)0x0,0);
  return uVar1;
}


/* ---- AES_FB2KS @ 00400944 ----
 * Run the hardware AES engine frame-buffer -> KEYSTORE: decrypt/transfer a
 * key into a protected keystore slot.
 */

int AES_FB2KS(pscramble_stream pscrambleStream,uint32 dst_KS)

{
  uint32 uVar1;
  pvoid local_1c;
  
                    /* Unresolved local var: uint32 flags@[???]
                       Unresolved local var: puint8 pControlWord@[???]
                       Unresolved local var: puint8 pIVSeed@[???]
                       Unresolved local var: pint8 pSrcBuffer_phy@[???]
                       Unresolved local var: pint8 pDstBuffer_phy@[???]
                       Unresolved local var: int ret@[???] */
  uVar1 = xcode_scramble_stream
                    (hXCode,pscrambleStream->strm_handle_generic,&ioctl_scrambler,
                     (pscrambleStream->sendMem).pBuffer_phy,local_1c,1,0x10,
                     pscrambleStream->scrambleMode,0x160,pscrambleStream->controlWord,(puint8)0x0,
                     pscrambleStream->IVSeed,(puint8)0x0,dst_KS);
  return uVar1;
}


/* ---- createXcodeAccess @ 00400ab0 ----
 * Open /dev/vixs/xcodedrv and initialize the global xcode access handle
 * (hXCode) used by all the Reg/DMA/keystore helpers.
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

uint32 createXcodeAccess(void)

{
  bool bVar1;
  int iVar2;
  int local_c8;
  undefined4 local_c0;
  undefined4 local_bc;
  undefined4 local_2c;
  uint32_ptr local_1c;
  uint32 local_10;
  
                    /* Unresolved local var: uint32 error@[???]
                       Unresolved local var: IOCTLDEVFRAMEBUFFER ioctl_addr_config@[???]
                       Unresolved local var: uint32 rc@[???] */
  hXCode = __libc_open64("/dev/vixs/xcodedrv",2);
  bVar1 = hXCode < 1;
  if (bVar1) {
    __printf("\nFailed to open XCode driver - %d %s\n",hXCode,"/dev/vixs/xcodedrv");
    hXCode = 0;
  }
  if (hXCode != 0) {
    memset(&local_c0,0,0xac);
    local_bc = 9;
    local_c0 = 0xac;
    local_2c = 0xffffffff;
    iVar2 = ioctl(hXCode,0x17241724);
    if (iVar2 == -1) {
      _IO_puts("GET MMR ADDRESS FAILED ");
    }
    else {
      RegBaseAddr = local_1c;
      local_bc = 8;
      local_c0 = 0xac;
      iVar2 = ioctl(hXCode,0x17241724);
      if (iVar2 == -1) {
        _IO_puts("GET FB ADDRESS FAILED ");
      }
      else {
        FrameBufferBaseAddr = local_1c;
        local_c8 = 1;
      }
    }
    if (local_c8 == 0) {
      if (hXCode != 0) {
        close(hXCode);
      }
      hXCode = 0;
    }
  }
  if (bVar1) {
    if (hXCode != 0) {
      close(hXCode);
    }
    local_10 = 0;
  }
  else {
    local_10 = 1;
  }
  return local_10;
}


/* ---- xcode_open_generic @ 00400d18 ----
 * Open a 'generic stream' (a DMA/command channel) on the xcode device via
 * ioctl; used here as the secure 'scramble' stream for key loading.
 */

uint64 xcode_open_generic(int XCodeHandle,PIOCTLSDEVOPENGENERIC pioctl_opengeneric,uint32 mode,
                         pscramble_stream pscrambleStream)

{
  uint local_18;
  undefined4 local_10;
  undefined4 local_c;
  
                    /* Unresolved local var: uint i@[???] */
  memset(pioctl_opengeneric,0,0x204);
  (pioctl_opengeneric->ioctl_header).command = 0x2e;
  (pioctl_opengeneric->ioctl_header).size_of_buf = 0x204;
  (pioctl_opengeneric->ioctl_header).time_out_value = 5000;
  (pioctl_opengeneric->ioctl_header).return_value = 2;
  *(undefined4 *)&pioctl_opengeneric->stream_handle = 0;
  *(undefined4 *)((int)&pioctl_opengeneric->stream_handle + 4) = 0;
  (pioctl_opengeneric->ioctl_param).generic_mode = mode;
  (pioctl_opengeneric->ioctl_param).req_size = 0x6000;
  (pioctl_opengeneric->ioctl_param).soc_shared_input_buffer_size = 0x6000;
  ioctl(XCodeHandle,0x17241724);
  __printf("\nOpen Generic return_value = %u\n",(pioctl_opengeneric->ioctl_header).return_value);
  if ((pioctl_opengeneric->ioctl_header).return_value == 1) {
    scrambleStream.sendMem.pBuffer =
         (pint8)(pioctl_opengeneric->ioctl_param).soc_shared_input_buffer_addr;
    scrambleStream.sendMem.pBuffer_phy =
         (pint8)(pioctl_opengeneric->ioctl_param).soc_shared_input_buffer_addr_phy;
    scrambleStream.sendMem.buf_size = (pioctl_opengeneric->ioctl_param).soc_shared_input_buffer_size
    ;
    __printf("Generic send buffer addr %x phy addr %x size %x\n",
             (uint)scrambleStream.sendMem.pBuffer,(uint)scrambleStream.sendMem.pBuffer_phy,
             scrambleStream.sendMem.buf_size);
    scrambleStream.recvMem.genericMemNum =
         (pioctl_opengeneric->ioctl_param).soc_output_generic_buf_info.soc_output_buffer_num;
    __printf("\nGet %u recv generic buffer from driver\n",scrambleStream.recvMem.genericMemNum);
    for (local_18 = 0; local_18 < scrambleStream.recvMem.genericMemNum; local_18 = local_18 + 1) {
      scrambleStream.recvMem.pBufferGeneric[local_18] =
           (pint8)(pioctl_opengeneric->ioctl_param).soc_output_generic_buf_info.
                  soc_output_buffer_addr[local_18];
      scrambleStream.recvMem.pBufferGeneric_phy[local_18] =
           (pint8)(pioctl_opengeneric->ioctl_param).soc_output_generic_buf_info.
                  soc_output_buffer_addr_phy[local_18];
      __printf("Generic buffer %u: addr %x phy addr %x\n",local_18,
               (uint)scrambleStream.recvMem.pBufferGeneric[local_18],
               (uint)scrambleStream.recvMem.pBufferGeneric_phy[local_18]);
    }
    local_10 = (undefined4)pioctl_opengeneric->stream_handle;
    local_c = *(undefined4 *)((int)&pioctl_opengeneric->stream_handle + 4);
  }
  else {
    local_10 = 0;
    local_c = 0;
  }
  return CONCAT44(local_c,local_10);
}


/* ---- xcode_close_generic @ 00401098 ----
 * Close a generic stream.
 */

uint32 xcode_close_generic(int XCodeHandle,PIOCTLSDEVCLOSEGENERIC pioctl_close_generic,uint64 handle
                          )

{
  uint local_10;
  
  memset(pioctl_close_generic,0,0xb4);
  (pioctl_close_generic->ioctl_header).command = 0x2f;
  (pioctl_close_generic->ioctl_header).size_of_buf = 0xb4;
  (pioctl_close_generic->ioctl_header).time_out_value = 5000;
  (pioctl_close_generic->ioctl_header).return_value = 2;
  pioctl_close_generic->stream_handle = handle;
  ioctl(XCodeHandle,0x17241724);
  local_10 = (uint)((pioctl_close_generic->ioctl_header).return_value == 1);
  return local_10;
}


/* ---- PerformCleanUp @ 00401198 ----
 * Close the xcode stream and device handle; free key memory.
 */

/* WARNING: Unknown calling convention -- yet parameter storage is locked */

void PerformCleanUp(void)

{
  uint32 uVar1;
  
  scrambleStream.stopOperation = 1;
  if ((int)scrambleStream.strm_handle_generic != 0 || scrambleStream.strm_handle_generic._4_4_ != 0)
  {
    uVar1 = xcode_close_generic(hXCode,&ioctl_close_generic,
                                CONCAT44(scrambleStream.strm_handle_generic._4_4_,
                                         (int)scrambleStream.strm_handle_generic));
    if (uVar1 == 0) {
      _IO_puts("\nFailed to close generic");
    }
    else {
      _IO_puts("\nSucceeded in closing generic");
    }
    scrambleStream.strm_handle_generic._0_4_ = 0;
    scrambleStream.strm_handle_generic._4_4_ = 0;
  }
  if (scrambleStream.hSrcFile != 0) {
    close(scrambleStream.hSrcFile);
  }
  if (scrambleStream.hDestFile != 0) {
    close(scrambleStream.hDestFile);
  }
  usleep(500000);
  close(hXCode);
  return;
}


/* ---- xcode_scramble_stream @ 00401308 ----
 * Configure a stream as scrambled/encrypted (secure path for key transfer).
 */

uint32 xcode_scramble_stream
                 (int_ptr XCodeHandle,uint64 hStream,PIOCTLSDEVSCRAMBLER2 pioctl_scrambler,
                 pvoid pSrcBuffer,pvoid pDstBuffer,uint32 line_number,uint32 pitchSize,uint32 mode,
                 uint32 flags,puint8 pControlWord,puint8 pControlWord2,puint8 pIV_seed,
                 puint8 pALU_num,uint32 dst_KS)

{
  bool bVar1;
  uint local_10;
  
  memset(pioctl_scrambler,0,0x15c);
  (pioctl_scrambler->ioctl_header).command = 0x108;
  (pioctl_scrambler->ioctl_header).size_of_buf = 0x15c;
  (pioctl_scrambler->ioctl_header).time_out_value = 5000;
  (pioctl_scrambler->ioctl_header).return_value = 2;
  pioctl_scrambler->stream_handle = hStream;
  (pioctl_scrambler->ioctl_param).scramble_flags = flags;
  (pioctl_scrambler->ioctl_param).scramble_mode = mode;
  *(pvoid *)&(pioctl_scrambler->ioctl_param).pSrc = pSrcBuffer;
  *(int *)((int)&(pioctl_scrambler->ioctl_param).pSrc + 4) = (int)pSrcBuffer >> 0x1f;
  *(undefined4 *)&(pioctl_scrambler->ioctl_param).pDest = 0;
  *(undefined4 *)((int)&(pioctl_scrambler->ioctl_param).pDest + 4) = 0;
  (pioctl_scrambler->ioctl_param).keyStoreIndexOfDstData = dst_KS;
  (pioctl_scrambler->ioctl_param).numOfLine = line_number;
  (pioctl_scrambler->ioctl_param).scrambledBytesPerLine = pitchSize;
  (pioctl_scrambler->ioctl_param).directBytesPerLine = 0;
  (pioctl_scrambler->ioctl_param).sourcePitch = pitchSize;
  (pioctl_scrambler->ioctl_param).destPitch = pitchSize;
  if (pControlWord != (puint8)0x0) {
    memcpy((pioctl_scrambler->ioctl_param).control_words,pControlWord,0x18);
  }
  if (pControlWord2 != (puint8)0x0) {
    memcpy((pioctl_scrambler->ioctl_param).control_words2,pControlWord2,0x18);
  }
  if (pIV_seed != (puint8)0x0) {
    memcpy((pioctl_scrambler->ioctl_param).IV_seed,pIV_seed,0x10);
  }
  if (pALU_num != (puint8)0x0) {
    memcpy((pioctl_scrambler->ioctl_param).ALU_num,pALU_num,0x10);
  }
  ioctl(XCodeHandle,0x17241724);
  bVar1 = (pioctl_scrambler->ioctl_header).return_value != 1;
  if (bVar1) {
    __printf("xcode_scramble_stream() failed!, return value = %x\n",
             (pioctl_scrambler->ioctl_header).return_value);
  }
  local_10 = (uint)!bVar1;
  return local_10;
}


/* ---- RegRead @ 004015a0 ----
 * Read a 32-bit ViXS hardware register via ioctl.
 */

uint32 RegRead(uint32 dwRegOffset)

{
  return *(uint32 *)(RegBaseAddr + dwRegOffset);
}


/* ---- RegWrite @ 004015d8 ----
 * Write a 32-bit value to a ViXS hardware register (ioctl on the xcode
 * device). Used to push key words and control bits into the keystore.
 */

/* WARNING: Restarted to delay deadcode elimination for space: stack */

void RegWrite(uint32 dwRegByteOffset,uint32 dwData)

{
                    /* Unresolved local var: puint8 ptr8@[???]
                       Unresolved local var: uint32 data32@[???] */
  *(uint32 *)(RegBaseAddr + dwRegByteOffset) = dwData;
  return;
}


/* ---- RegPoll @ 00401724 ----
 * Poll a register until (value & mask) == expected, with a retry count /
 * delay. Used to wait for keystore-ready before writing the next word.
 */

int RegPoll(uint32 addr,uint32 value,uint32 mask,uint32 wait,uint32 timeout)

{
  bool bVar1;
  uint local_18;
  uint local_14;
  uint local_10;
  
                    /* Unresolved local var: uint temp@[???]
                       Unresolved local var: uint count@[???]
                       Unresolved local var: int localpassfail@[???] */
  local_14 = 0;
  local_10 = *(uint *)((addr & 0xfffffffc) + RegBaseAddr);
  for (; ((local_10 & mask) != value && (local_14 < timeout)); local_14 = local_14 + 1) {
    usleep((wait * 1000) / 100);
    local_10 = *(uint *)((addr & 0xfffffffc) + RegBaseAddr);
  }
  bVar1 = (local_10 & mask) != value;
  if (bVar1) {
    __printf("%-15s %08lx %08lx %08lx %08lx\n","RegPoll FAIL",addr,value,mask,local_10);
    passfail = 0;
  }
  local_18 = (uint)!bVar1;
  return local_18;
}


/* ---- DMA_HOST2FB @ 004018e4 ----
 * DMA a buffer from host memory into the SoC frame buffer.
 */

void DMA_HOST2FB(uchar *data,ulong addr,uint BLength)

{
  uint local_10;
  
                    /* Unresolved local var: uint i@[???] */
  for (local_10 = 0; local_10 < BLength >> 2; local_10 = local_10 + 1) {
    MemWrite(local_10 * 4 + addr,*(uint32 *)(data + local_10 * 4));
  }
  return;
}


/* ---- free_key_mem @ 00468afc ----
 * Free the in-memory key buffers after loading.
 */

/* WARNING: Removing unreachable block (ram,0x00468b30) */

void free_key_mem(void *mem)

{
  check_free(mem);
  __libc_free(mem);
  return;
}
