/*
 * 05_xcodeio.cpp.c  --  annotated decompilation of src/libadv7842/xcodeio.cpp
 * ============================================================================
 *
 * Original source: /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libadv7842/xcodeio.cpp
 *
 * Role in av_streamer:
 *   Singleton hardware-I/O wrapper (`xcode_io_t`) used by the ADV7842 front-end
 *   driver (`ADV7842_t` in 06_adv7842.cpp.c and `libadv7842.cpp` in 04_libadv7842.cpp.c)
 *   to talk to `/dev/vixs/xcodedrv` via the ViXS SDK C API (`xcodeapi.cpp` /
 *   07_xcodeapi.cpp.c). It provides:
 *     1. I2C master transactions (`i2c_read`, `i2c_write`, `i2c_write_then_read`)
 *        used to program and poll the ADV7842 HDMI/analog video receiver.
 *     2. Memory-mapped register (MMR) access (`mmreg_read`, `mmreg_write`,
 *        `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits`) used to toggle
 *        ViXS XCode4 GPIO 5 (MMR 0x1100/0x1104 bit 5 = 0x20, external audio mux).
 *     3. Frame-buffer memory helpers (`fb_write`, `fb_read`, `fb_move`, `fb_set`,
 *        `fb_addr_to_ptr`) wrapping the mapped XCode4 frame-buffer base pointer.
 *
 *   Note: The ViXS encoder path (`02_vixs_encoder.c.c`) does NOT use `xcode_io_t`;
 *   it opens `/dev/vixs/xcodedrv` and issues ioctls directly.
 *
 * Main data structure (`xcode_io_t`, 60 bytes from DWARF):
 *   +0x00  _vptr.xcode_io_t    (vtable @ 0x00436138, virtual destructor)
 *   +0x04  DebugDumpPrint      (int, uninitialized -> 0 in .bss; enables I2C error printf)
 *   +0x08  DebugPrint          (int, unused)
 *   +0x0c  FPGADebugDumpPrint  (int, unused)
 *   +0x10  HDMIDebugDumpPrint  (int, unused)
 *   +0x14  m_FRBase            (uchar*, mapped XCode4 frame-buffer base)
 *   +0x18  m_MMRBase           (uint*, mapped XCode4 MMR base)
 *   +0x1c  endian_flag         (int, 0 = native little-endian; 1 = byteswap 32-bit MMR)
 *   +0x20  m_Mutex             (pthread_mutex_t, 24 bytes; initialized/destroyed, NEVER locked)
 *   +0x38  m_XcodeIOHandle     (xcode_handle_t / int fd for /dev/vixs/xcodedrv)
 *
 * Globals owned:
 *   - `get_io()::XocdeIO` (guard variable) and `get_io::lexical_block_0::XocdeIO`
 *     (the singleton `xcode_io_t` instance in .bss; note original typo "XocdeIO").
 */

/*
 * xcode_io_t::xcode_io_t @ 00412a90
 *
 * Purpose:
 *   Constructor for the `xcode_io_t` singleton. Initializes `m_Mutex`, opens
 *   `/dev/vixs/xcodedrv` via `xcode_open_handle()`, maps both the frame buffer
 *   (`m_FRBase`) and memory-mapped registers (`m_MMRBase`), and clears `endian_flag`.
 * Parameters:
 *   this - pointer to `xcode_io_t` instance
 * Return value:
 *   void
 * Callers / Callees:
 *   Called by: `xcode_io_t::get_io()` (00412da4) on first access.
 *   Calls: `pthread_mutex_init`, `xcode_open_handle`, `xcode_get_frame_buffer_map_addr`,
 *          `xcode_get_mmr_map_addr`, `puts`, `printf`.
 * Side effects:
 *   Opens `/dev/vixs/xcodedrv`, maps FB & MMR into process address space, prints
 *   base addresses or error messages to stdout.
 * Quirks:
 *   - `DebugDumpPrint` and sibling debug flags are not initialized here (relying
 *     on `.bss` zero-initialization for the static singleton).
 *   - `xVar1` (`m_XcodeIOHandle`) is not checked for `<= 0` / `< 0` before calling
 *     the map functions.
 */
// ==== xcode_io_t::xcode_io_t @ 00412a90 ====

/* WARNING: Removing unreachable block (ram,0x00412c5c) */
/* WARNING: Restarted to delay deadcode elimination for space: stack */
/* DWARF original prototype: void xcode_io_t(xcode_io_t * this) */

void __thiscall xcode_io_t::xcode_io_t(xcode_io_t *this)

{
  xcode_handle_t xVar1;  // fd returned by xcode_open_handle()
  uchar *puVar2;         // mapped frame-buffer base address
  uint *puVar3;          // mapped MMR base address
  
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
  this->endian_flag = 0;  // MIPS32-LE host matches XCode4 LE MMR layout
  return;
}



/*
 * xcode_io_t::~xcode_io_t @ 00412c80 (complete object destructor D1)
 *
 * Purpose:
 *   Closes `m_XcodeIOHandle` if non-zero and destroys `m_Mutex`.
 * Parameters:
 *   this      - pointer to `xcode_io_t`
 *   __in_chrg - GCC C++ ABI in-charge flag (unused here)
 * Callers / Callees:
 *   Called by: deleting destructor `~xcode_io_t` (00412d44) and `__cxa_atexit`
 *              cleanup registered in `get_io()`.
 *   Calls: `xcode_close_handle`, `pthread_mutex_destroy`.
 * Quirk:
 *   Checks `m_XcodeIOHandle != 0` instead of `>= 0` (if `open64` failed and
 *   returned `-1`, this would call `close(-1)`).
 */
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



/*
 * xcode_io_t::~xcode_io_t @ 00412d44 (deleting destructor D0)
 *
 * Purpose:
 *   Virtual deleting destructor: runs `~xcode_io_t(this, __in_chrg)` then frees
 *   the object with `operator delete(this)`.
 * Callers:
 *   Vtable slot for deleting destructor (not reached for static singleton).
 */
// ==== xcode_io_t::~xcode_io_t @ 00412d44 ====

/* DWARF original prototype: void ~xcode_io_t(xcode_io_t * this, int __in_chrg) */

void __thiscall xcode_io_t::~xcode_io_t(xcode_io_t *this,int __in_chrg)

{
  ~xcode_io_t(this,__in_chrg);
  operator_delete(this);
  return;
}



/*
 * xcode_io_t::get_io @ 00412da4
 *
 * Purpose:
 *   Thread-safe Meyers singleton accessor returning `&XocdeIO`.
 * Return value:
 *   Pointer to the single static `xcode_io_t` instance.
 * Callers / Callees:
 *   Called by: `ADV7842_t::ADV7842_t` (both constructors in 06_adv7842.cpp.c).
 *   Calls: `__cxa_guard_acquire`, `xcode_io_t::xcode_io_t`, `__cxa_guard_release`,
 *          `__cxa_atexit`.
 * Threading:
 *   Static initialization is guarded by GCC's `__cxa_guard_acquire` / `release`.
 */
// ==== xcode_io_t::get_io @ 00412da4 ====

xcode_io_t * xcode_io_t::get_io(void)

{
  int iVar1;  // __cxa_guard_acquire result (1 = this thread must construct)
  
  if ((get_io()::XocdeIO == '\0') && (iVar1 = __cxa_guard_acquire(&get_io()::XocdeIO), iVar1 != 0))
  {
                    /* try { // try from 00412e14 to 00412e1b has its CatchHandler @ 00412e90 */
    xcode_io_t(&get_io::lexical_block_0::XocdeIO);
    __cxa_guard_release(&get_io()::XocdeIO);
    __cxa_atexit(~xcode_io_t,&get_io::lexical_block_0::XocdeIO,&__dso_handle);
  }
  return &get_io::lexical_block_0::XocdeIO;
}



/*
 * xcode_io_t::i2c_read @ 00412ed4
 *
 * Purpose:
 *   Performs an I2C read of `n` bytes from `Address` on `I2CChannel` via
 *   `xcode_i2c_read` with hard-coded `atomic_period = 0x43` (67) and
 *   `timeout = 300` ms.
 * Parameters:
 *   this       - `xcode_io_t` instance
 *   I2CChannel - ViXS XCode4 I2C bus/block number (1 for ADV7842)
 *   Address    - 8-bit shifted I2C slave address (e.g. `ADV7842_I2CAddress << 1`)
 *   data       - destination buffer for read bytes
 *   n          - byte count (max 64 in driver struct, unchecked)
 *   start      - 1 to emit I2C START condition
 *   stop       - 1 to emit I2C STOP condition
 * Return value:
 *   0 on success (`IOCTL_RET_CMD_COMPLETE`), otherwise driver `return_value`.
 * Callers / Callees:
 *   Called by: `xcode_io_t::i2c_write_then_read`.
 *   Calls: `xcode_i2c_read`, `printf` (only if `DebugDumpPrint == 1`).
 */
// ==== xcode_io_t::i2c_read @ 00412ed4 ====

/* DWARF original prototype: int i2c_read(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_read
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;  // return code from xcode_i2c_read (0 = success)
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_read(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if ((iVar1 != 0) && (this->DebugDumpPrint == 1)) {
    printf("I2C_READ failed @ %2.2X\n",Address);
  }
  return iVar1;
}



/*
 * xcode_io_t::i2c_write @ 00412fbc
 *
 * Purpose:
 *   Performs an I2C write of `n` bytes to `Address` on `I2CChannel` via
 *   `xcode_i2c_write` with `atomic_period = 0x43` (67) and `timeout = 300` ms.
 * Parameters:
 *   this, I2CChannel, Address, data, n, start, stop - same as `i2c_read`.
 * Return value:
 *   0 on success, otherwise driver `return_value`.
 * Callers / Callees:
 *   Called by: `ADV7842_t::write_register` (06_adv7842.cpp.c) and
 *              `xcode_io_t::i2c_write_then_read`.
 *   Calls: `xcode_i2c_write`, `printf`.
 * Quirk:
 *   Suppresses the `I2C_WRITE failed` message when `Address == 0x40` (ADV7842 IO
 *   map 7-bit 0x20 << 1, likely because software reset at 0x40 reg 0xFF drops
 *   the I2C ACK).
 */
// ==== xcode_io_t::i2c_write @ 00412fbc ====

/* DWARF original prototype: int i2c_write(xcode_io_t * this, int I2CChannel, int Address, void *
   data, size_t n, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write
          (xcode_io_t *this,int I2CChannel,int Address,void *data,size_t n,int start,int stop)

{
  int iVar1;  // return code from xcode_i2c_write (0 = success)
  
                    /* Unresolved local var: int ret@[???] */
  iVar1 = xcode_i2c_write(this->m_XcodeIOHandle,I2CChannel,Address,data,n,0x43,300,start,stop);
  if (((iVar1 != 0) && (Address != 0x40)) && (this->DebugDumpPrint == 1)) {
    printf("I2C_WRITE failed @ %2.2X\n",Address);
  }
  return iVar1;
}



/*
 * xcode_io_t::i2c_write_then_read @ 004130b4
 *
 * Purpose:
 *   Combined I2C register-read transaction: writes `wn` bytes (typically 1
 *   register offset) with `stop = 0` (no STOP), then if that succeeds, issues
 *   a repeated-START (`start = 1`) read of `rn` bytes with `stop`.
 * Parameters:
 *   this       - `xcode_io_t` instance
 *   I2CChannel - I2C bus/block number
 *   Address    - 8-bit I2C slave address
 *   wdata      - write buffer (register subaddress)
 *   rdata      - read destination buffer
 *   wn, rn     - write and read byte counts
 *   start      - START flag for the initial write phase
 *   stop       - STOP flag for the trailing read phase
 * Return value:
 *   0 on success, or non-zero error code from `i2c_write` or `i2c_read`.
 * Callers / Callees:
 *   Called by: `ADV7842_t::read_register` (06_adv7842.cpp.c).
 *   Calls: `xcode_io_t::i2c_write`, `xcode_io_t::i2c_read`.
 */
// ==== xcode_io_t::i2c_write_then_read @ 004130b4 ====

/* DWARF original prototype: int i2c_write_then_read(xcode_io_t * this, int I2CChannel, int Address,
   void * wdata, void * rdata, size_t wn, size_t rn, int start, int stop) */

int __thiscall
xcode_io_t::i2c_write_then_read
          (xcode_io_t *this,int I2CChannel,int Address,void *wdata,void *rdata,size_t wn,size_t rn,
          int start,int stop)

{
  int iVar1;  // error status from i2c_write / i2c_read
  
                    /* Unresolved local var: int error@[???] */
  iVar1 = i2c_write(this,I2CChannel,Address,wdata,wn,start,0);
  if (iVar1 == 0) {
    iVar1 = i2c_read(this,I2CChannel,Address,rdata,rn,1,stop);
  }
  return iVar1;
}



/*
 * byteswap @ 0041316c
 *
 * Purpose:
 *   In-place 32-bit endianness swap of the 4 bytes pointed to by `ptr`.
 * Parameters:
 *   ptr - pointer to 4-byte buffer
 * Callers:
 *   `xcode_io_t::mmreg_write`, `xcode_io_t::mmreg_read` (when `endian_flag != 0`).
 */
// ==== byteswap @ 0041316c ====

void byteswap(char *ptr)

{
  char cVar1;  // temp byte for swap
  
                    /* Unresolved local var: char c@[???] */
  cVar1 = *ptr;
  *ptr = ptr[3];
  ptr[3] = cVar1;
  cVar1 = ptr[1];
  ptr[1] = ptr[2];
  ptr[2] = cVar1;
  return;
}



/*
 * xcode_io_t::mmreg_write @ 004131f8
 *
 * Purpose:
 *   Writes a 32-bit `value` to XCode4 MMR offset `reg_addr` (4-byte aligned via
 *   `& 0xfffffffc`), byte-swapping first if `endian_flag != 0`.
 * Parameters:
 *   this     - `xcode_io_t` instance
 *   reg_addr - byte offset from `m_MMRBase`
 *   value    - 32-bit value to write
 * Callers:
 *   `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits` (when `endian_flag != 0`).
 *   Unused externally in `av_streamer`.
 */
// ==== xcode_io_t::mmreg_write @ 004131f8 ====

/* DWARF original prototype: void mmreg_write(xcode_io_t * this, uint reg_addr, uint value) */

void __thiscall xcode_io_t::mmreg_write(xcode_io_t *this,uint reg_addr,uint value)

{
  uint local_res8 [2];  // stack copy of value for optional in-place byteswap
  
  if (this->m_MMRBase != (uint *)0x0) {
    local_res8[0] = value;
    if (this->endian_flag != 0) {
      byteswap((char *)local_res8);
    }
    *(uint *)((int)this->m_MMRBase + (reg_addr & 0xfffffffc)) = local_res8[0];
  }
  return;
}



/*
 * xcode_io_t::mmreg_read @ 0041327c
 *
 * Purpose:
 *   Reads a 32-bit value from XCode4 MMR offset `reg_addr` (4-byte aligned),
 *   byte-swapping if `endian_flag != 0`. Returns 0 if `m_MMRBase == NULL`.
 * Callers:
 *   `clear_reg_mask`, `set_reg_mask`, `write_reg_mask_bits` (when `endian_flag != 0`).
 *   Unused externally in `av_streamer`.
 */
// ==== xcode_io_t::mmreg_read @ 0041327c ====

/* DWARF original prototype: uint mmreg_read(xcode_io_t * this, uint reg_addr) */

uint __thiscall xcode_io_t::mmreg_read(xcode_io_t *this,uint reg_addr)

{
  uint local_10 [2];  // read-back register value (swapped in-place if endian_flag != 0)
  
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



/*
 * xcode_io_t::clear_reg_mask @ 00413330
 *
 * Purpose:
 *   Clears bits specified by `mask` in MMR `reg_addr` (`reg &= ~mask`).
 *   Unused in `av_streamer`.
 * Threading:
 *   Non-atomic read-modify-write; `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::clear_reg_mask @ 00413330 ====

/* DWARF original prototype: void clear_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::clear_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;  // temp register value in byteswap path
  
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



/*
 * xcode_io_t::set_reg_mask @ 00413404
 *
 * Purpose:
 *   Sets bits specified by `mask` in MMR `reg_addr` (`reg |= mask`).
 *   Unused in `av_streamer`.
 * Threading:
 *   Non-atomic read-modify-write; `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::set_reg_mask @ 00413404 ====

/* DWARF original prototype: void set_reg_mask(xcode_io_t * this, uint reg_addr, uint mask) */

void __thiscall xcode_io_t::set_reg_mask(xcode_io_t *this,uint reg_addr,uint mask)

{
  uint uVar1;  // temp register value in byteswap path
  
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



/*
 * xcode_io_t::write_reg_mask_bits @ 004134d0
 *
 * Purpose:
 *   Clears `mask` bits and ORs in `value` in MMR `reg_addr`.
 * Callers:
 *   - `04_libadv7842.cpp.c`: `adv7842_program_input_internal` sets MMR `0x1100`
 *     mask `0x20` to `0x20` (HDMI audio) or `0` (external audio), and `0x1104`
 *     mask `0x20` to `0x20` (GPIO 5 output enable).
 *   - `06_adv7842.cpp.c`: `ADV7842_t::ADV7842_t(int)` and `program_input_source`
 *     similarly drive GPIO 5 via `0x1100`/`0x1104`.
 * Quirks:
 *   - Does NOT mask `value` with `mask` (`value` is ORed directly).
 *   - In the `endian_flag == 0` path, performs two separate writes to `m_MMRBase`
 *     (first clearing `mask`, then ORing `value`), creating a brief glitch window
 *     where the masked bits are 0.
 *   - `m_Mutex` is NOT locked.
 */
// ==== xcode_io_t::write_reg_mask_bits @ 004134d0 ====

/* DWARF original prototype: void write_reg_mask_bits(xcode_io_t * this, uint reg_addr, uint mask,
   uint value) */

void __thiscall xcode_io_t::write_reg_mask_bits(xcode_io_t *this,uint reg_addr,uint mask,uint value)

{
  uint uVar1;  // temp register value in byteswap path
  
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



/*
 * xcode_io_t::fb_write @ 004135f0
 *
 * Purpose:
 *   Copies `size` bytes from host `buf` into XCode4 frame-buffer memory at
 *   offset `fb_addr`. Returns destination pointer or NULL if `m_FRBase == NULL`.
 *   Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_write @ 004135f0 ====

/* DWARF original prototype: void * fb_write(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_write(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;  // memcpy return value (destination pointer) or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(this->m_FRBase + fb_addr,buf,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_read @ 00413680
 *
 * Purpose:
 *   Copies `size` bytes from XCode4 frame-buffer offset `fb_addr` to host `buf`.
 *   Returns `buf` or NULL if `m_FRBase == NULL`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_read @ 00413680 ====

/* DWARF original prototype: void * fb_read(xcode_io_t * this, uint fb_addr, void * buf, size_t
   size) */

void * __thiscall xcode_io_t::fb_read(xcode_io_t *this,uint fb_addr,void *buf,size_t size)

{
  void *pvVar1;  // memcpy return value (buf) or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memcpy(buf,this->m_FRBase + fb_addr,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_move @ 00413710
 *
 * Purpose:
 *   `memmove` within XCode4 frame-buffer memory from `src_fb_addr` to
 *   `dst_fb_addr`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_move @ 00413710 ====

/* DWARF original prototype: void * fb_move(xcode_io_t * this, uint dst_fb_addr, uint src_fb_addr,
   size_t size) */

void * __thiscall
xcode_io_t::fb_move(xcode_io_t *this,uint dst_fb_addr,uint src_fb_addr,size_t size)

{
  void *pvVar1;  // memmove return value or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memmove(this->m_FRBase + dst_fb_addr,this->m_FRBase + src_fb_addr,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_set @ 004137b0
 *
 * Purpose:
 *   `memset` `size` bytes at XCode4 frame-buffer offset `fb_addr` to `value`.
 *   Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_set @ 004137b0 ====

/* DWARF original prototype: void * fb_set(xcode_io_t * this, uint fb_addr, int value, size_t size)
    */

void * __thiscall xcode_io_t::fb_set(xcode_io_t *this,uint fb_addr,int value,size_t size)

{
  void *pvVar1;  // memset return value or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    pvVar1 = (void *)0x0;
  }
  else {
    pvVar1 = memset(this->m_FRBase + fb_addr,value,size);
  }
  return pvVar1;
}



/*
 * xcode_io_t::fb_addr_to_ptr @ 00413840
 *
 * Purpose:
 *   Converts a frame-buffer offset `fb_addr` to a user-space virtual pointer
 *   (`m_FRBase + fb_addr`), or NULL if `m_FRBase == NULL`. Unused in `av_streamer`.
 */
// ==== xcode_io_t::fb_addr_to_ptr @ 00413840 ====

/* DWARF original prototype: void * fb_addr_to_ptr(xcode_io_t * this, uint fb_addr) */

void * __thiscall xcode_io_t::fb_addr_to_ptr(xcode_io_t *this,uint fb_addr)

{
  uchar *puVar1;  // virtual pointer into mapped frame buffer, or NULL
  
  if (this->m_FRBase == (uchar *)0x0) {
    puVar1 = (uchar *)0x0;
  }
  else {
    puVar1 = this->m_FRBase + fb_addr;
  }
  return puVar1;
}


