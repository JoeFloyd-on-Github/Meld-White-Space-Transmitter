/*
 * 10_tables_sdt.c.c  --  annotated decompilation of libdvbpsi tables/sdt.c
 * =========================================================================
 *
 * Original source file: tables/sdt.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_sdt_*` public API
 * (`dvbpsi_sdt_attach`, `dvbpsi_sdt_init`, `dvbpsi_sdt_sections_generate`,
 * `dvbpsi_sdt_service_add`, `dvbpsi_sdt_service_descriptor_add`), and the
 * `dvbpsi_demux_t` sub-decoder attachment model. Note that internal helper
 * `dvbpsi_ReInitSDT` retains the older CamelCase naming convention.
 *
 * Purpose in av_streamer:
 *   Provides DVB Service Description Table (SDT, ETSI EN 300 468, PID 0x0011,
 *   table_id 0x42 actual TS / 0x46 other TS) construction, section
 *   serialization, and section parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genSDT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_sdt_init`, `dvbpsi_sdt_service_add`,
 *   `dvbpsi_sdt_service_descriptor_add`, `dvbpsi_sdt_sections_generate`,
 *   `dvbpsi_sdt_empty`), which emits an SDT packet on PID 0x0011 with
 *   service_id=3 and a DVB service_descriptor (tag 0x48) encoding
 *   "<company>,<service_name>(<sn>,<lat>,<lng>)".
 *   The decoder/demux functions (`dvbpsi_sdt_attach`, `dvbpsi_sdt_detach`,
 *   `dvbpsi_sdt_new`, `dvbpsi_sdt_delete`, `dvbpsi_ReInitSDT`,
 *   `dvbpsi_sdt_sections_decode`, `dvbpsi_sdt_sections_gather`) are linked
 *   in from libdvbpsi but are dead code in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_sdt_t (12 bytes):
 *     +0x00 uint8_t  i_table_id       (0x42 = SDT actual TS, 0x46 = other TS)
 *     +0x02 uint16_t i_extension      (transport_stream_id)
 *     +0x04 uint8_t  i_version        (5-bit version_number)
 *     +0x05 _Bool    b_current_next
 *     +0x06 uint16_t i_network_id     (original_network_id)
 *     +0x08 dvbpsi_sdt_service_t *p_first_service
 *   dvbpsi_sdt_service_t (16 bytes):
 *     +0x00 uint16_t i_service_id
 *     +0x02 _Bool    b_eit_schedule
 *     +0x03 _Bool    b_eit_present
 *     +0x04 uint8_t  i_running_status (3-bit: 4 = running)
 *     +0x05 _Bool    b_free_ca
 *     +0x06 uint16_t i_descriptors_length
 *     +0x08 dvbpsi_descriptor_t *p_first_descriptor
 *     +0x0c dvbpsi_sdt_service_s *p_next
 *   dvbpsi_sdt_decoder_t (52 / 0x34 bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t   (28 bytes = sizeof(dvbpsi_decoder_t))
 *     +0x1c dvbpsi_sdt_callback pf_sdt_callback  (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                   (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_sdt_t current_sdt             (+0x28: {i_version, b_current_next})
 *     +0x30 dvbpsi_sdt_t *p_building_sdt         (Ghidra: p_decoder[1].i_section_max_size)
 *
 * Globals owned: none.
 */

/* dvbpsi_sdt_attach @ 0041ac60
 * Role (upstream libdvbpsi 1.x): Attaches an SDT sub-decoder for (i_table_id,
 *   i_extension) to the demux attached to p_dvbpsi.
 * Parameters:
 *   p_dvbpsi    - libdvbpsi handle (must have a dvbpsi_demux_t attached)
 *   i_table_id  - PSI table ID (typically 0x42 for SDT actual TS)
 *   i_extension - transport_stream_id
 *   pf_callback - callback invoked when a complete SDT is decoded
 *   p_cb_data   - opaque user pointer passed to pf_callback
 * Returns: true on success, false if sub-decoder already exists or alloc fails.
 * Callers/callees: dead code in av_streamer; calls dvbpsi_demuxGetSubDec,
 *   dvbpsi_decoder_new, dvbpsi_NewDemuxSubDecoder, dvbpsi_AttachDemuxSubDecoder,
 *   dvbpsi_decoder_delete, dvbpsi_error.
 * Threading/side effects: allocates 0x34-byte decoder + demux subdec; asserts
 *   on NULL p_dvbpsi or NULL p_dvbpsi->p_decoder. */
_Bool dvbpsi_sdt_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_sdt_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_sdt_decoder_t (0x34 bytes)
  dvbpsi_demux_t *p_demux;           // demuxer cast from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_sdt_decoder_t * p_sdt_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/sdt.c",0x3b,"dvbpsi_sdt_attach"); // "p_dvbpsi" (inferred)
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/sdt.c",0x3c,"dvbpsi_sdt_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x34 (52) bytes for dvbpsi_sdt_decoder_t; pf_gather=NULL (subdec handles it)
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x34);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_sdt_detach,dvbpsi_sdt_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_sdt_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_sdt_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_section_max_size = 0;                   // +0x30: p_building_sdt = NULL
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



/* dvbpsi_sdt_init @ 0041ae10
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_sdt_t
 *   structure (typically on the stack) before adding services and generating sections.
 * Parameters:
 *   p_sdt          - SDT struct to initialize (asserted non-NULL)
 *   i_table_id     - 0x42 (SDT actual TS) or 0x46 (SDT other TS)
 *   i_extension    - transport_stream_id
 *   i_version      - 5-bit version_number
 *   b_current_next - current_next_indicator
 *   i_network_id   - original_network_id (in av_streamer: 0x2114)
 * Returns: void.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (table_id=0x42, ext=0, ver=0x10,
 *   b_current_next=false [overwritten to true before generate], net_id=0x2114). */
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



/* dvbpsi_sdt_new @ 0041ae84
 * Role (upstream libdvbpsi 1.x): Heap-allocates (12 bytes) and initializes a
 *   dvbpsi_sdt_t structure.
 * Parameters: same fields as dvbpsi_sdt_init.
 * Returns: newly allocated dvbpsi_sdt_t *, or NULL on malloc failure.
 * Callers: dvbpsi_sdt_sections_gather (via inlined dvbpsi_AddSectionSDT). */
dvbpsi_sdt_t *
dvbpsi_sdt_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_network_id)

{
  dvbpsi_sdt_t *pdVar1;              // newly allocated dvbpsi_sdt_t *
  
                    /* Unresolved local var: dvbpsi_sdt_t * p_sdt@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0xc);              // sizeof(dvbpsi_sdt_t) == 12
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



/* dvbpsi_sdt_empty @ 0041af0c
 * Role (upstream libdvbpsi 1.x): Frees all services in p_sdt->p_first_service
 *   and their attached descriptor lists, resetting p_first_service to NULL.
 *   Does not free p_sdt itself (safe for stack-allocated structs).
 * Parameters: p_sdt - SDT struct to empty.
 * Callers: genSDT() in 01_libxcode4encoder.c.c, and dvbpsi_sdt_delete(). */
void dvbpsi_sdt_empty(dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_sdt_service_t *__ptr;       // current service node being freed (p_service)
  dvbpsi_sdt_service_s *pdVar1;      // saved next pointer (p_tmp)
  
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



/* dvbpsi_sdt_delete @ 0041af80
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_sdt_t.
 * Parameters: p_sdt - SDT struct to free (NULL-safe).
 * Callers: dvbpsi_ReInitSDT, dvbpsi_sdt_detach. */
void dvbpsi_sdt_delete(dvbpsi_sdt_t *p_sdt)

{
  if (p_sdt != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_empty(p_sdt);
  }
  free(p_sdt);
  return;
}



/* dvbpsi_ReInitSDT @ 0041afc4
 * Role (upstream libdvbpsi 1.x internal): Resets the SDT decoder state after a
 *   completed table or on discontinuity/error. Retains CamelCase name from
 *   earlier libdvbpsi generations.
 * Parameters:
 *   p_decoder - SDT decoder instance (asserted non-NULL)
 *   b_force   - if true, also deletes p_building_sdt (not yet handed to callback)
 * Callers: dvbpsi_sdt_sections_gather. */
void dvbpsi_ReInitSDT(dvbpsi_sdt_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;   // upper 24 bits of MIPS $a1 register (b_force)
  
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



/* dvbpsi_sdt_detach @ 0041b050
 * Role (upstream libdvbpsi 1.x): Detaches and frees the SDT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Parameters: p_dvbpsi, i_table_id, i_extension.
 * Callers: dead code in av_streamer (registered as pf_detach in dvbpsi_sdt_attach). */
void dvbpsi_sdt_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_sdt_decoder_t *)
  
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
  // Free any partially built SDT (+0x30 = p_building_sdt)
  if ((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size != (dvbpsi_sdt_t *)0x0) {
    dvbpsi_sdt_delete((dvbpsi_sdt_t *)pdVar1[1].i_section_max_size);
  }
  pdVar1[1].i_section_max_size = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_sdt_service_add @ 0041b1c0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_sdt_service_t (16 bytes)
 *   and appends it to the end of p_sdt->p_first_service.
 * Parameters:
 *   p_sdt            - target SDT struct
 *   i_service_id     - 16-bit MPEG/DVB service_id (program_number)
 *   b_eit_schedule   - EIT_schedule_flag
 *   b_eit_present    - EIT_present_following_flag
 *   i_running_status - 3-bit running_status (e.g. 4 = running)
 *   b_free_ca        - free_CA_mode (true = scrambled)
 * Returns: pointer to the new service node, or NULL on calloc failure.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (service_id=3, running_status=4),
 *   and dvbpsi_sdt_sections_decode(). */
dvbpsi_sdt_service_t *
dvbpsi_sdt_service_add
          (dvbpsi_sdt_t *p_sdt,uint16_t i_service_id,_Bool b_eit_schedule,_Bool b_eit_present,
          uint8_t i_running_status,_Bool b_free_ca)

{
  dvbpsi_sdt_service_t *pdVar1;      // newly allocated service node (p_service)
  dvbpsi_sdt_service_t *pdVar2;      // list traversal cursor (p_last_service)
  
                    /* Unresolved local var: dvbpsi_sdt_service_t * p_service@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x10);           // sizeof(dvbpsi_sdt_service_t) == 16
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



/* dvbpsi_sdt_service_descriptor_add @ 0041b288
 * Role (upstream libdvbpsi 1.x): Creates a descriptor via dvbpsi_NewDescriptor
 *   and appends it to p_service->p_first_descriptor.
 * Parameters: p_service, i_tag (e.g. 0x48 service_descriptor), i_length, p_data.
 * Returns: newly allocated dvbpsi_descriptor_t *, or NULL on allocation failure.
 * Callers: genSDT() in 01_libxcode4encoder.c.c (tag 0x48), and
 *   dvbpsi_sdt_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_sdt_service_descriptor_add
          (dvbpsi_sdt_service_t *p_service,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
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



/* dvbpsi_sdt_sections_decode @ 0041b320
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_sdt with services and their descriptors.
 * Parameters: p_sdt - target SDT struct; p_section - linked list of PSI sections.
 * SDT section payload layout (after 8-byte PSI header):
 *   +0..+1: original_network_id (16 bits)
 *   +2:     reserved_future_use (0xff)
 *   +3..:   loop of service entries, each 5 bytes + descriptors_loop_length:
 *             +0..+1: service_id
 *             +2:     reserved_future_use[7:2], EIT_schedule_flag[1], EIT_p/f_flag[0]
 *             +3..+4: running_status[15:13], free_CA_mode[12], descriptors_loop_length[11:0]
 * Callers: dvbpsi_sdt_sections_gather (dead code in av_streamer). */
void dvbpsi_sdt_sections_decode(dvbpsi_sdt_t *p_sdt,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (byte 3 of service entry / desc length)
  byte bVar2;                        // byte 4 of service entry (low 8 bits of descriptors_loop_length)
  uint8_t *puVar3;                   // lookahead pointer (p_byte + 4 or p_byte + 2) for bounds check
  dvbpsi_sdt_service_t *p_service;   // newly added service entry
  uint8_t *puVar4;                   // p_section->p_payload_end
  uint8_t *p_data;                   // descriptor payload pointer (p_byte + 2)
  uint8_t *puVar5;                   // current byte cursor (p_byte)
  uint8_t *puVar6;                   // start of current service entry
  uint8_t *puVar7;                   // end of current service's descriptor loop
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar4 = p_section->p_payload_end;
    puVar5 = p_section->p_payload_start + 3; // skip original_network_id (2B) + reserved (1B)
    puVar3 = p_section->p_payload_start + 7; // need at least 5 bytes (puVar5 + 4 < p_payload_end)
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
        puVar7 = puVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2); // end of service descriptors
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



/* dvbpsi_sdt_sections_gather @ 0041b45c
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for SDT
 *   sections. Validates table_id == 0x42 (note: SDT other TS 0x46 is rejected),
 *   checks TS discontinuity / extension / version / last_section_number
 *   (inlined dvbpsi_CheckSDT), buffers sections until complete (inlined
 *   dvbpsi_AddSectionSDT), decodes them, and invokes pf_sdt_callback.
 * Parameters: p_dvbpsi, p_private_decoder (dvbpsi_sdt_decoder_t *), p_section.
 * Callers: registered in dvbpsi_sdt_attach (dead code in av_streamer). */
void dvbpsi_sdt_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  short sVar1;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var2;                       // boolean return from PSI helper calls
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  int iVar3;                         // scratch: p_building_sdt or pf_sdt_callback
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  undefined3 extraout_var_01;        // MIPS ABI upper 3 bytes
  dvbpsi_psi_section_t *pdVar4;      // copy of current_sdt bytes +0x04..+0x07
  dvbpsi_sdt_t *pdVar5;              // p_building_sdt (dvbpsi_sdt_t *)
  undefined4 uVar6;                  // p_sdt_decoder->p_cb_data (+0x20)
  dvbpsi_psi_section_t *p_section_00; // p_private_decoder->p_sections
  dvbpsi_psi_section_t *pdVar7;      // copy of current_sdt bytes +0x00..+0x03
  dvbpsi_decoder_t *pdVar8;          // p_demux (p_dvbpsi->p_decoder)
  
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
  _Var2 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x42,"SDT decoder"); // expects table_id 0x42
  if (CONCAT31(extraout_var,_Var2) == 0) goto LAB_0041b694;
  pdVar8 = p_dvbpsi->p_decoder;
  if (pdVar8->b_discontinuity == false) {
    iVar3 = p_private_decoder[1].i_section_max_size; // +0x30: p_building_sdt
    if (iVar3 == 0) {
      // No table in progress; ignore if version+current_next match current_sdt (+0x28)
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
      // Verify consistency with p_building_sdt (+0x02 i_extension, +0x04 i_version)
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
    // Allocate p_building_sdt with original_network_id from p_payload_start[0..1]
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
  // Copy *p_building_sdt into p_sdt_decoder->current_sdt (+0x24..+0x2f)
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
  (**(code **)(p_private_decoder + 1))(uVar6,iVar3); // pf_sdt_callback(p_cb_data, p_building_sdt)
  dvbpsi_ReInitSDT((dvbpsi_sdt_decoder_t *)p_private_decoder,false);
  return;
}



/* dvbpsi_sdt_sections_generate @ 0041b7f4
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_sdt_t and its service/
 *   descriptor lists into a linked list of 1024-byte max PSI sections
 *   (dvbpsi_NewPSISection(0x400)) and finalizes CRC32 via dvbpsi_BuildPSISection.
 * Parameters: p_dvbpsi - libdvbpsi handle; p_sdt - SDT struct to serialize.
 * Returns: head of linked dvbpsi_psi_section_t * list.
 * Quirk: hardcodes i_table_id = 0x42 (ignoring p_sdt->i_table_id) and
 *   b_private_indicator = true. Max section size = 1024 (0x400), max data
 *   offset before CRC = 0x3fb (1019; + 4B CRC = 1023 <= 1024).
 * Callers: genSDT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t * dvbpsi_sdt_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_sdt_t *p_sdt)

{
  dvbpsi_psi_section_s **ppdVar1;    // &p_section->p_next for finalization loop
  _Bool _Var2;                       // scratch b_current_next
  uint16_t uVar3;                    // scratch i_extension / i_network_id
  uint8_t uVar4;                     // i_last_number (copied from final section)
  dvbpsi_psi_section_t *pdVar5;      // head of section list (p_result)
  dvbpsi_descriptor_t *pdVar6;       // descriptor list cursor
  uint8_t *puVar7;                   // write cursor in current section
  dvbpsi_psi_section_t *pdVar8;      // current section being populated (p_current)
  uint uVar9;                        // accumulated byte size of next service + descriptors
  uint8_t *puVar10;                  // projected end offset for overflow check (< 0x3fb)
  byte bVar11;                       // scratch byte (flags / descriptor length)
  int iVar12;                        // current byte count in section (p_payload_end - p_data)
  dvbpsi_psi_section_t *p_section;   // section cursor in finalization loop
  dvbpsi_psi_section_t *pdVar13;     // previous/current section pointer (p_prev)
  dvbpsi_sdt_service_t *pdVar14;     // service list cursor (p_service)
  uint8_t *puVar15;                  // start of current service entry in section buffer
  byte *pbVar16;                     // pointer to byte 3 of service entry (descriptors_loop_length hi)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_prev@[???]
                       Unresolved local var: dvbpsi_sdt_service_t * p_service@[???] */
  pdVar5 = dvbpsi_NewPSISection(0x400);
  pdVar14 = p_sdt->p_first_service;
  pdVar5->i_table_id = 0x42;         // hardcoded SDT actual TS (0x42)
  uVar3 = p_sdt->i_extension;
  pdVar5->b_syntax_indicator = true;
  pdVar5->b_private_indicator = true;
  pdVar5->i_length = 0xc;            // 5B after section_length + 3B SDT hdr + 4B CRC = 12
  pdVar5->i_extension = uVar3;
  pdVar5->i_version = p_sdt->i_version;
  _Var2 = p_sdt->b_current_next;
  pdVar5->i_number = 0;
  uVar3 = p_sdt->i_network_id;
  pdVar5->b_current_next = _Var2;
  pdVar5->p_payload_end = pdVar5->p_payload_end + 0xb; // 8B PSI hdr + 3B SDT hdr = 11
  pdVar5->p_payload_start = pdVar5->p_data + 8;
  pdVar5->p_data[8] = (uint8_t)(uVar3 >> 8);
  pdVar5->p_data[9] = (uint8_t)p_sdt->i_network_id;
  pdVar5->p_data[10] = 0xff;         // reserved_future_use
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
      uVar9 = 5;                     // 5-byte service header
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
    bVar11 = 0xfc;                   // reserved_future_use[7:2] = 0b111111
    pbVar16 = puVar15 + 3;
    *puVar15 = (uint8_t)(pdVar14->i_service_id >> 8);
    puVar15[1] = (uint8_t)pdVar14->i_service_id;
    if (pdVar14->b_eit_schedule != false) {
      bVar11 = 0xfe;                 // set EIT_schedule_flag (bit 1)
    }
    puVar15[2] = bVar11 | pdVar14->b_eit_present; // bit 0 = EIT_present_following_flag
    puVar15[3] = (pdVar14->b_free_ca & 1U) << 4 | pdVar14->i_running_status << 5;
    pdVar6 = pdVar14->p_first_descriptor;
    puVar7 = pdVar8->p_payload_end + 5;
    pdVar8->p_payload_end = puVar7;
    pdVar8->i_length = pdVar8->i_length + 5;
    pdVar13 = pdVar8;
    if (pdVar6 == (dvbpsi_descriptor_t *)0x0) break;
    puVar10 = puVar7 + ((uint)pdVar6->i_length - (int)pdVar8->p_data);
    while ((int)puVar10 < 0x3fb) {   // ensure room for tag+len+data + 4B CRC within 1024B
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
  // Backfill 12-bit descriptors_loop_length in puVar15[3..4]
  *pbVar16 = (byte)((uint)(puVar7 + (-5 - (int)puVar15)) >> 8) & 0xf | *pbVar16;
  puVar15[4] = (uint8_t)(puVar7 + (-5 - (int)puVar15));
  pdVar14 = pdVar14->p_next;
  goto joined_r0x0041b8c0;
}
