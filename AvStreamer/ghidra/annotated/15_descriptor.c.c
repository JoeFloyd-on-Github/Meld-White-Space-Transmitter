/*
 * 15_descriptor.c.c  --  annotated decompilation of libdvbpsi-1.1.0/src/descriptor.c
 * ==================================================================================
 *
 * Original source : /var/lib/tftpboot/Meld_AV_Script/2p08b/avstreamer/src/libdvbpsi-1.1.0/src/descriptor.c
 *                   (DWARF CU @ 0x41f170..0x41f3dc, compiled with GNU C 4.5.3, MIPS32-LE)
 * Library / API   : VideoLAN libdvbpsi 1.1.0 (LGPLv2.1+), statically linked.
 *                   Matches the libdvbpsi 1.x descriptor list API.
 *
 * Role in av_streamer:
 *   Allocates, appends, frees, and inspects MPEG/DVB PSI/SI descriptors
 *   (dvbpsi_descriptor_t). Used by all DVB SI table generators in av_streamer
 *   (NIT, SDT, EIT, TOT) via dvbpsi_NewDescriptor, dvbpsi_AddDescriptor,
 *   and dvbpsi_DeleteDescriptors.
 *
 * Main data structures (from DWARF):
 *   dvbpsi_descriptor_t (struct dvbpsi_descriptor_s, 16 bytes = 0x10):
 *     +0x00  uint8_t i_tag                  (descriptor_tag)
 *     +0x01  uint8_t i_length               (descriptor_length)
 *     +0x04  uint8_t *p_data                (raw payload bytes, length i_length)
 *     +0x08  dvbpsi_descriptor_s *p_next    (singly linked list next)
 *     +0x0c  void *p_decoded                (optional decoded descriptor struct)
 *
 * Globals owned: none.
 * Upstream fidelity: unmodified libdvbpsi 1.1.0 descriptor.c.
 */

/* dvbpsi_IsDescriptorDecoded @ 0x0041f170
 * Purpose  : Check whether a descriptor has already been decoded into p_decoded.
 * Params   : p_descriptor - descriptor to test (assumed non-NULL)
 * Returns  : true if p_descriptor->p_decoded != NULL.
 */
// ==== dvbpsi_IsDescriptorDecoded @ 0041f170 ====

_Bool dvbpsi_IsDescriptorDecoded(dvbpsi_descriptor_t *p_descriptor)

{
  return p_descriptor->p_decoded != (void *)0x0;
}



/* dvbpsi_CanDecodeAsDescriptor @ 0x0041f17c
 * Purpose  : Check whether a descriptor is non-NULL and has the expected tag.
 * Params   : p_descriptor - descriptor to test (may be NULL)
 *            i_tag        - expected descriptor_tag
 * Returns  : true if p_descriptor != NULL && p_descriptor->i_tag == i_tag.
 */
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



/* dvbpsi_NewDescriptor @ 0x0041f19c
 * Purpose  : Allocate a dvbpsi_descriptor_t (0x10 bytes) and a backing p_data
 *            buffer of i_length bytes, copying p_data if non-NULL.
 * Params   : i_tag    - MPEG/DVB descriptor_tag (e.g. 0x40, 0x41, 0x48, 0x4d, 0x4e, 0x54, 0x5a, 0x6c)
 *            i_length - payload length in bytes (0..255)
 *            p_data   - source payload bytes to copy, or NULL
 * Returns  : Newly allocated dvbpsi_descriptor_t *, or NULL on OOM.
 * Callers  : dvbpsi_tot_descriptor_add (13_tables_tot.c.c), NIT/SDT/EIT descriptor_add
 * Callees  : malloc, free, memcpy
 */
// ==== dvbpsi_NewDescriptor @ 0041f19c ====

dvbpsi_descriptor_t * dvbpsi_NewDescriptor(uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *__ptr; // p_descriptor (0x10 bytes)
  uint8_t *__dest;            // allocated p_data buffer of i_length bytes
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  __ptr = malloc(0x10);
  if (__ptr != (dvbpsi_descriptor_t *)0x0) {
    __dest = malloc((size_t)i_length); // Note: if i_length==0, relies on libc malloc(0) != NULL
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



/* dvbpsi_AddDescriptor @ 0x0041f268
 * Purpose  : Append p_descriptor to the tail of singly linked list p_list.
 * Params   : p_list       - head of existing descriptor list (may be NULL)
 *            p_descriptor - descriptor to append (asserted non-NULL)
 * Returns  : Head of the updated list (p_descriptor if p_list was NULL, else p_list).
 * Callers  : dvbpsi_tot_descriptor_add (13_tables_tot.c.c), NIT/SDT/EIT helpers
 * Callees  : __assert_fail
 */
// ==== dvbpsi_AddDescriptor @ 0041f268 ====

dvbpsi_descriptor_t *
dvbpsi_AddDescriptor(dvbpsi_descriptor_t *p_list,dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_t *pdVar1; // p_last cursor
  
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



/* dvbpsi_DeleteDescriptors @ 0x0041f2e0
 * Purpose  : Free an entire singly linked list of dvbpsi_descriptor_t,
 *            including each node's p_data and p_decoded buffers.
 * Params   : p_descriptor - head of descriptor list (may be NULL)
 * Returns  : void
 * Callers  : dvbpsi_tot_empty (13_tables_tot.c.c), NIT/SDT/EIT empty/decode
 * Callees  : free
 */
// ==== dvbpsi_DeleteDescriptors @ 0041f2e0 ====

void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_s *pdVar1; // p_next
  
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



/* dvbpsi_DuplicateDecodedDescriptor @ 0x0041f364
 * Purpose  : Allocate a zero-initialized copy of a decoded descriptor struct
 *            and memcpy i_size bytes from p_decoded.
 * Params   : p_decoded - source decoded struct (may be NULL)
 *            i_size    - byte size of decoded struct
 * Returns  : Newly allocated copy, or NULL if p_decoded == NULL or on OOM.
 * Callees  : calloc, memcpy
 */
// ==== dvbpsi_DuplicateDecodedDescriptor @ 0041f364 ====

void * dvbpsi_DuplicateDecodedDescriptor(void *p_decoded,ssize_t i_size)

{
  void *__dest;              // p_duplicate
  
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


