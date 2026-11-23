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



