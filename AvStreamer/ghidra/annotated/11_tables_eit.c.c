/*
 * 11_tables_eit.c.c  --  annotated decompilation of libdvbpsi tables/eit.c
 * =========================================================================
 *
 * Original source file: tables/eit.c (VideoLAN libdvbpsi, LGPL, statically
 * linked into av_streamer).
 *
 * Upstream API generation: libdvbpsi 1.x (1.0.x-1.3.x era), evidenced by the
 * `dvbpsi_t *` handle parameter, lowercase `dvbpsi_eit_*` public API
 * (`dvbpsi_eit_attach`, `dvbpsi_eit_init`, `dvbpsi_eit_sections_generate`,
 * `dvbpsi_eit_event_add`, `dvbpsi_eit_nvod_event_add`,
 * `dvbpsi_eit_event_descriptor_add`), and `dvbpsi_demux_t` sub-decoder model.
 * Internal helper `dvbpsi_ReInitEIT` retains the older CamelCase naming, and
 * `NewEITSection` @ 0041bbe0 is a non-static helper not present as a public
 * symbol in upstream libdvbpsi 1.x (likely added or exposed in this fork).
 *
 * Purpose in av_streamer:
 *   Provides DVB Event Information Table (EIT, ETSI EN 300 468, PID 0x0012,
 *   table_id 0x4e..0x6f) construction, section serialization, and section
 *   parsing/decoding.
 *   In av_streamer, ONLY the table-building / section-generation side is
 *   called at runtime — specifically from `genEIT()` in
 *   01_libxcode4encoder.c.c (`dvbpsi_eit_init`, `dvbpsi_eit_event_add`,
 *   `dvbpsi_eit_event_descriptor_add`, `dvbpsi_eit_sections_generate`,
 *   `dvbpsi_eit_empty`), which emits a present/following EIT on PID 0x0012
 *   (table_id=0x4e, service_id=3, ts_id=0, orig_network_id=0x2114) with a
 *   single event starting at current UTC/MJD time, duration 0x05dc (1500 BCD?),
 *   and descriptors: extended_event (0x4e), content (0x54), short_event (0x4d,
 *   "test event-name" / "experiment").
 *   The decoder/demux functions (`dvbpsi_eit_attach`, `dvbpsi_eit_detach`,
 *   `dvbpsi_eit_new`, `dvbpsi_eit_delete`, `dvbpsi_ReInitEIT`,
 *   `dvbpsi_eit_nvod_event_add`, `dvbpsi_eit_sections_decode`,
 *   `dvbpsi_eit_sections_gather`) are dead code in av_streamer.
 *
 * Main data structures (offsets from binary DWARF):
 *   dvbpsi_eit_t (16 bytes):
 *     +0x00 uint8_t  i_table_id                    (0x4e..0x6f)
 *     +0x02 uint16_t i_extension                   (service_id)
 *     +0x04 uint8_t  i_version                     (5-bit version_number)
 *     +0x05 _Bool    b_current_next
 *     +0x06 uint16_t i_ts_id                       (transport_stream_id)
 *     +0x08 uint16_t i_network_id                  (original_network_id)
 *     +0x0a uint8_t  i_segment_last_section_number
 *     +0x0b uint8_t  i_last_table_id
 *     +0x0c dvbpsi_eit_event_t *p_first_event
 *   dvbpsi_eit_event_t (40 / 0x28 bytes):
 *     +0x00 uint16_t i_event_id
 *     +0x08 uint64_t i_start_time                  (40-bit MJD + UTC BCD)
 *     +0x10 uint32_t i_duration                    (24-bit HHMMSS BCD)
 *     +0x14 uint8_t  i_running_status
 *     +0x15 _Bool    b_free_ca
 *     +0x16 _Bool    b_nvod                        (true if start_time==0xfffff000 && status==0)
 *     +0x18 uint16_t i_descriptors_length
 *     +0x1c dvbpsi_descriptor_t *p_first_descriptor
 *     +0x20 dvbpsi_eit_event_s *p_next
 *   dvbpsi_eit_decoder_t (60 / 0x3c bytes):
 *     +0x00..+0x1b dvbpsi_decoder_t                (28 bytes)
 *     +0x1c dvbpsi_eit_callback pf_eit_callback    (Ghidra: p_decoder + 1)
 *     +0x20 void    *p_cb_data                     (Ghidra: &p_decoder[1].b_discontinuity)
 *     +0x24 dvbpsi_eit_t current_eit               (+0x28: {i_version, b_current_next})
 *     +0x34 dvbpsi_eit_t *p_building_eit           (Ghidra: p_decoder[1].i_need)
 *     +0x38 uint8_t  i_first_received_section_number (Ghidra: p_decoder[2].i_magic[0])
 *
 * Globals owned: none.
 */

/* NewEITSection @ 0041bbe0
 * Role (non-upstream / fork helper): Allocates a 4094-byte (0xffe) PSI section
 *   and initializes the 8-byte PSI header plus the 6-byte EIT-specific header
 *   (transport_stream_id, original_network_id, last_table_id; note that
 *   p_data[0xc] segment_last_section_number is filled in later by the caller).
 * Parameters:
 *   p_eit            - source EIT metadata
 *   i_table_id       - table_id (0x4e..0x6f)
 *   i_section_number - section_number (0..255)
 * Returns: newly allocated dvbpsi_psi_section_t * (NULL not checked!).
 * Callers: dvbpsi_eit_sections_generate. */
dvbpsi_psi_section_t * NewEITSection(dvbpsi_eit_t *p_eit,int i_table_id,int i_section_number)

{
  _Bool _Var1;                       // scratch p_eit->b_current_next
  uint16_t uVar2;                    // scratch i_extension / i_ts_id
  dvbpsi_psi_section_t *pdVar3;      // allocated PSI section (p_result)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[DW_OP_reg2(v0)] */
  pdVar3 = dvbpsi_NewPSISection(0xffe); // EIT max section size = 4096 (0xffe = 4094)
  pdVar3->i_table_id = (uint8_t)i_table_id;
  uVar2 = p_eit->i_extension;
  pdVar3->b_syntax_indicator = true;
  pdVar3->b_private_indicator = true;
  pdVar3->i_length = 0xf;            // 5B PSI ext + 6B EIT hdr + 4B CRC = 15 (0xf)
  pdVar3->i_extension = uVar2;
  pdVar3->i_version = p_eit->i_version;
  _Var1 = p_eit->b_current_next;
  pdVar3->i_number = (uint8_t)i_section_number;
  uVar2 = p_eit->i_ts_id;
  pdVar3->b_current_next = _Var1;
  pdVar3->p_payload_end = pdVar3->p_payload_end + 0xe; // 8B PSI hdr + 6B EIT hdr = 14 (0xe)
  pdVar3->p_payload_start = pdVar3->p_data + 8;
  pdVar3->p_data[8] = (uint8_t)(uVar2 >> 8);           // transport_stream_id[15:8]
  pdVar3->p_data[9] = (uint8_t)p_eit->i_ts_id;         // transport_stream_id[7:0]
  pdVar3->p_data[10] = (uint8_t)(p_eit->i_network_id >> 8); // original_network_id[15:8]
  pdVar3->p_data[0xb] = (uint8_t)p_eit->i_network_id;       // original_network_id[7:0]
  pdVar3->p_data[0xd] = p_eit->i_last_table_id;        // last_table_id (p_data[0xc] set later)
  return pdVar3;
}



/* dvbpsi_eit_attach @ 0041bcb4
 * Role (upstream libdvbpsi 1.x): Attaches an EIT sub-decoder for (i_table_id,
 *   i_extension) to p_dvbpsi's demuxer.
 * Parameters: p_dvbpsi, i_table_id (0x4e..0x6f), i_extension (service_id),
 *   pf_callback, p_cb_data.
 * Returns: true on success, false if already attached or allocation fails.
 * Callers: dead code in av_streamer. */
_Bool dvbpsi_eit_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_eit_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;     // existing or newly created demux sub-decoder
  dvbpsi_decoder_t *p_decoder;       // allocated dvbpsi_eit_decoder_t (0x3c bytes)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x3c,"dvbpsi_eit_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x3d,"dvbpsi_eit_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    // Allocate 0x3c (60) bytes for dvbpsi_eit_decoder_t
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,i_extension,dvbpsi_eit_detach,dvbpsi_eit_sections_gather,
                          p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_eit_callback *)(p_decoder + 1) = pf_callback; // +0x1c: pf_eit_callback
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;   // +0x20: p_cb_data
        p_decoder[1].i_need = 0;                               // +0x34: p_building_eit = NULL
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"EIT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
  }
  return false;
}



/* dvbpsi_eit_init @ 0041be64
 * Role (upstream libdvbpsi 1.x): Initializes a caller-allocated dvbpsi_eit_t.
 * Parameters: p_eit, i_table_id (0x4e), i_extension (service_id), i_version,
 *   b_current_next, i_ts_id, i_network_id, i_segment_last_section_number,
 *   i_last_table_id.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (0x4e, ext=3, ver=0x10,
 *   b_current_next=false, ts_id=0, net_id=0x2114, seg_last=0, last_tid=0). */
void dvbpsi_eit_init(dvbpsi_eit_t *p_eit,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint16_t i_ts_id,uint16_t i_network_id,
                    uint8_t i_segment_last_section_number,uint8_t i_last_table_id)

{
  p_eit->i_table_id = i_table_id;
  p_eit->i_extension = i_extension;
  p_eit->i_version = i_version;
  p_eit->b_current_next = b_current_next;
  p_eit->i_ts_id = i_ts_id;
  p_eit->i_network_id = i_network_id;
  p_eit->i_segment_last_section_number = i_segment_last_section_number;
  p_eit->i_last_table_id = i_last_table_id;
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



/* dvbpsi_eit_new @ 0041bea0
 * Role (upstream libdvbpsi 1.x): Heap-allocates (16 bytes) and initializes a
 *   dvbpsi_eit_t structure.
 * Returns: newly allocated dvbpsi_eit_t *, or NULL on malloc failure.
 * Callers: dvbpsi_eit_sections_gather (via inlined dvbpsi_AddSectionEIT). */
dvbpsi_eit_t *
dvbpsi_eit_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint16_t i_ts_id,uint16_t i_network_id,uint8_t i_segment_last_section_number,
              uint8_t i_last_table_id)

{
  dvbpsi_eit_t *pdVar1;              // newly allocated dvbpsi_eit_t *
  
                    /* Unresolved local var: dvbpsi_eit_t * p_eit@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x10);             // sizeof(dvbpsi_eit_t) == 16
  if (pdVar1 != (dvbpsi_eit_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_ts_id = i_ts_id;
    pdVar1->i_network_id = i_network_id;
    pdVar1->i_segment_last_section_number = i_segment_last_section_number;
    pdVar1->i_last_table_id = i_last_table_id;
    pdVar1->p_first_event = (dvbpsi_eit_event_t *)0x0;
  }
  return pdVar1;
}



/* dvbpsi_eit_empty @ 0041bf58
 * Role (upstream libdvbpsi 1.x): Frees all events in p_eit->p_first_event and
 *   their descriptor lists, resetting p_first_event to NULL.
 * Callers: genEIT() in 01_libxcode4encoder.c.c, and dvbpsi_eit_delete(). */
void dvbpsi_eit_empty(dvbpsi_eit_t *p_eit)

{
  dvbpsi_eit_event_t *__ptr;         // current event node being freed (p_event)
  dvbpsi_eit_event_s *pdVar1;        // saved next pointer (p_tmp)
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  __ptr = p_eit->p_first_event;
  while (__ptr != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_tmp@[???] */
    pdVar1 = __ptr->p_next;
    dvbpsi_DeleteDescriptors(__ptr->p_first_descriptor);
    free(__ptr);
    __ptr = pdVar1;
  }
  p_eit->p_first_event = (dvbpsi_eit_event_t *)0x0;
  return;
}



/* dvbpsi_eit_delete @ 0041bfcc
 * Role (upstream libdvbpsi 1.x): Empties and frees a heap-allocated dvbpsi_eit_t.
 * Callers: dvbpsi_ReInitEIT, dvbpsi_eit_detach. */
void dvbpsi_eit_delete(dvbpsi_eit_t *p_eit)

{
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_empty(p_eit);
  }
  free(p_eit);
  return;
}



/* dvbpsi_eit_detach @ 0041c010
 * Role (upstream libdvbpsi 1.x): Detaches and frees the EIT sub-decoder for
 *   (i_table_id, i_extension) from p_dvbpsi's demuxer.
 * Quirk: unlike dvbpsi_sdt_detach, does not assert p_subdec->p_decoder != NULL.
 * Callers: dead code in av_streamer. */
void dvbpsi_eit_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;   // located sub-decoder entry
  dvbpsi_eit_t *p_eit;               // p_eit_decoder->p_building_eit (+0x34)
  dvbpsi_demux_t *p_demux;           // demuxer from p_dvbpsi->p_decoder
  dvbpsi_decoder_t *pdVar1;          // p_subdec->p_decoder (dvbpsi_eit_decoder_t *)
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x6d,"dvbpsi_eit_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x6e,"dvbpsi_eit_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,i_extension);
  if (p_subdec == (dvbpsi_demux_subdec_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"EIT Decoder",
                 "No such EIT decoder (table_id == 0x%02x,extension == 0x%02x)",i_table_id,
                 i_extension);
    return;
  }
  pdVar1 = p_subdec->p_decoder;
  p_eit = (dvbpsi_eit_t *)pdVar1[1].i_need; // +0x34: p_building_eit
  if (p_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_eit);
  }
  pdVar1[1].i_need = 0;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



/* dvbpsi_ReInitEIT @ 0041c158
 * Role (upstream libdvbpsi 1.x internal): Resets the EIT decoder state.
 * Quirk: ignores its `b_force` parameter — unconditionally passes `true` to
 *   dvbpsi_decoder_reset() and unconditionally deletes p_building_eit (unlike
 *   dvbpsi_ReInitSDT and dvbpsi_ReInitNIT which guard on b_force).
 * Callers: dvbpsi_eit_sections_gather. */
void dvbpsi_ReInitEIT(dvbpsi_eit_decoder_t *p_decoder,_Bool b_force)

{
  if (p_decoder == (dvbpsi_eit_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/eit.c",0x119,"dvbpsi_ReInitEIT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,true);
  if (p_decoder->p_building_eit != (dvbpsi_eit_t *)0x0) {
    dvbpsi_eit_delete(p_decoder->p_building_eit);
  }
  p_decoder->p_building_eit = (dvbpsi_eit_t *)0x0;
  return;
}



/* dvbpsi_eit_event_add @ 0041c1d0
 * Role (upstream libdvbpsi 1.x): Allocates a dvbpsi_eit_event_t (40 bytes)
 *   and appends it to p_eit->p_first_event. Sets b_nvod if the lower 32 bits
 *   of i_start_time have 0xfffff000 set and i_running_status == 0 (NVOD
 *   reference events have undefined start_time = 0xffffffffff).
 * Parameters: p_eit, i_event_id, i_start_time (40-bit MJD+UTC), i_duration
 *   (24-bit BCD), i_running_status, b_free_ca, i_event_descriptor_length.
 * Returns: newly allocated event pointer, or NULL on calloc failure.
 * Callers: genEIT() in 01_libxcode4encoder.c.c, dvbpsi_eit_nvod_event_add(),
 *   and dvbpsi_eit_sections_decode(). */
dvbpsi_eit_event_t *
dvbpsi_eit_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint64_t i_start_time,uint32_t i_duration,
          uint8_t i_running_status,_Bool b_free_ca,uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;        // newly allocated event node (p_event)
  dvbpsi_eit_event_t *pdVar2;        // list traversal cursor (p_last_event)
  uint local_20;                     // low 32 bits of i_start_time for NVOD check
  
                    /* Unresolved local var: dvbpsi_eit_event_t * p_event@[DW_OP_reg2(v0)] */
  pdVar1 = calloc(1,0x28);           // sizeof(dvbpsi_eit_event_t) == 40
  local_20 = (uint)i_start_time;
  if (pdVar1 != (dvbpsi_eit_event_t *)0x0) {
    pdVar2 = p_eit->p_first_event;
    pdVar1->i_event_id = i_event_id;
    pdVar1->i_start_time = i_start_time;
    pdVar1->i_duration = i_duration;
    pdVar1->i_running_status = i_running_status;
    pdVar1->b_free_ca = b_free_ca;
    pdVar1->b_nvod = (local_20 & 0xfffff000) == 0xfffff000 && i_running_status == 0;
    pdVar1->p_next = (dvbpsi_eit_event_s *)0x0;
    pdVar1->i_descriptors_length = i_event_descriptor_length;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
    if (pdVar2 == (dvbpsi_eit_event_t *)0x0) {
      p_eit->p_first_event = pdVar1;
    }
    else {
      while (pdVar2->p_next != (dvbpsi_eit_event_s *)0x0) {
                    /* Unresolved local var: dvbpsi_eit_event_t * p_last_event@[???] */
        pdVar2 = pdVar2->p_next;
      }
      pdVar2->p_next = pdVar1;
    }
  }
  return pdVar1;
}



/* dvbpsi_eit_nvod_event_add @ 0041c2c0
 * Role (upstream libdvbpsi 1.x): Convenience wrapper around
 *   dvbpsi_eit_event_add for Near Video On Demand (NVOD) reference events
 *   (passes start_time = 0xfffff000, running_status = 0).
 * Callers: dead code in av_streamer. */
dvbpsi_eit_event_t *
dvbpsi_eit_nvod_event_add
          (dvbpsi_eit_t *p_eit,uint16_t i_event_id,uint32_t i_duration,_Bool b_free_ca,
          uint16_t i_event_descriptor_length)

{
  dvbpsi_eit_event_t *pdVar1;        // newly added NVOD event node
  
  pdVar1 = dvbpsi_eit_event_add
                     (p_eit,i_event_id,0xfffff000,i_duration,0,b_free_ca,i_event_descriptor_length);
  return pdVar1;
}



/* dvbpsi_eit_event_descriptor_add @ 0041c2fc
 * Role (upstream libdvbpsi 1.x): Creates a descriptor via dvbpsi_NewDescriptor
 *   and appends it to p_event->p_first_descriptor.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (tags 0x4e, 0x54, 0x4d), and
 *   dvbpsi_eit_sections_decode(). */
dvbpsi_descriptor_t *
dvbpsi_eit_event_descriptor_add
          (dvbpsi_eit_event_t *p_event,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor; // newly allocated descriptor
  dvbpsi_descriptor_t *pdVar1;       // updated head of descriptor list
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_event->p_first_descriptor,p_descriptor);
    p_event->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_event->p_first_descriptor","tables/eit.c",0x10f,
                    "dvbpsi_eit_event_descriptor_add");
    }
  }
  return p_descriptor;
}



/* dvbpsi_eit_sections_decode @ 0041c394
 * Role (upstream libdvbpsi 1.x): Parses all linked PSI sections in p_section
 *   and populates p_eit with events and their descriptors.
 * EIT section payload layout (after 8-byte PSI header):
 *   +0..+1: transport_stream_id
 *   +2..+3: original_network_id
 *   +4:     segment_last_section_number
 *   +5:     last_table_id
 *   +6..:   loop of event entries, each 12 bytes + descriptors_loop_length:
 *             +0..+1:  event_id
 *             +2..+6:  start_time (40-bit MJD+UTC)
 *             +7..+9:  duration (24-bit BCD)
 *             +10..+11: running_status[15:13], free_CA_mode[12], descriptors_loop_length[11:0]
 * Quirk: `(bVar1 & 8) == 0` is passed as `b_free_ca` — wait: bit 4 (0x10) is
 *   free_CA_mode in EN 300 468, whereas bit 3 (0x08) is reserved, and `== 0`
 *   inverts it! (Upstream libdvbpsi bug in this version.)
 * Callers: dvbpsi_eit_sections_gather (dead code in av_streamer). */
void dvbpsi_eit_sections_decode(dvbpsi_eit_t *p_eit,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // scratch byte (byte 10 of event / descriptor length)
  dvbpsi_eit_event_t *p_event;       // newly added event node
  uint8_t *puVar2;                   // p_ev_end (clamped to p_payload_end)
  uint8_t *puVar3;                   // current byte cursor (event start, then descriptor cursor)
  uint uVar4;                        // i_ev_length (12-bit descriptors_loop_length)
  uint8_t *puVar5;                   // p_section->p_payload_end
  
  do {
    if (p_section == (dvbpsi_psi_section_t *)0x0) {
      return;
    }
    puVar5 = p_section->p_payload_end;
    puVar3 = p_section->p_payload_start + 6; // skip 6-byte EIT header
    while (puVar3 < puVar5) {
      while( true ) {
                    /* Unresolved local var: uint16_t i_event_id@[???]
                       Unresolved local var: uint64_t i_start_time@[???]
                       Unresolved local var: uint32_t i_duration@[???]
                       Unresolved local var: uint8_t i_running_status@[???]
                       Unresolved local var: _Bool b_free_ca@[???]
                       Unresolved local var: uint16_t i_ev_length@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???]
                       Unresolved local var: uint8_t * p_ev_end@[???] */
        bVar1 = puVar3[10];
        uVar4 = (bVar1 & 0xf) << 8 | (uint)puVar3[0xb];
        p_event = dvbpsi_eit_event_add
                            (p_eit,CONCAT11(*puVar3,puVar3[1]),
                             (ulonglong)
                             CONCAT14(puVar3[2],
                                      (uint)puVar3[3] << 0x18 | (uint)puVar3[6] |
                                      (uint)puVar3[4] << 0x10 | (uint)puVar3[5] << 8),
                             (uint)puVar3[7] << 0x10 | (uint)puVar3[8] << 8 | (uint)puVar3[9],
                             bVar1 >> 5,(bVar1 & 8) == 0,(uint16_t)uVar4);
        if (p_event == (dvbpsi_eit_event_t *)0x0) goto LAB_0041c4cc;
        puVar3 = puVar3 + 0xc;       // advance past 12-byte event header
        puVar2 = puVar3 + uVar4;
        if (p_section->p_payload_end <= puVar3 + uVar4) {
          puVar2 = p_section->p_payload_end;
        }
        if (puVar2 <= puVar3) break;
        do {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
          bVar1 = puVar3[1];
          if ((int)(bVar1 + 1) < (int)puVar2 - (int)puVar3) {
            dvbpsi_eit_event_descriptor_add(p_event,*puVar3,bVar1,puVar3 + 2);
          }
          puVar3 = puVar3 + bVar1 + 2;
        } while (puVar3 < puVar2);
        if (puVar5 <= puVar3) goto LAB_0041c4cc;
      }
    }
LAB_0041c4cc:
    p_section = p_section->p_next;
  } while( true );
}



/* dvbpsi_eit_sections_gather @ 0041c518
 * Role (upstream libdvbpsi 1.x): Demux gather callback (pf_gather) for EIT
 *   sections. Accepts table_id in [0x4e..0x6f] (actual/other p/f and schedule),
 *   checks TS discontinuity / service_id / version / last_section_number
 *   (inlined dvbpsi_CheckEIT), buffers sections (inlined dvbpsi_AddSectionEIT),
 *   checks EIT segment completion (inlined dvbpsi_IsCompleteEIT using
 *   i_first_received_section_number and segment_last_section_number at
 *   p_payload_start[4]), decodes, and invokes pf_eit_callback.
 * Callers: registered in dvbpsi_eit_attach (dead code in av_streamer). */
void dvbpsi_eit_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_private_decoder,
               dvbpsi_psi_section_t *p_section)

{
  byte bVar1;                        // current section's i_number during completion scan
  short sVar2;                       // packed {i_version, b_current_next} for comparison
  _Bool _Var3;                       // boolean return from PSI helpers
  undefined3 extraout_var;           // MIPS ABI upper 3 bytes of _Bool return
  int iVar4;                         // scratch: p_building_eit or pf_eit_callback
  undefined3 extraout_var_00;        // MIPS ABI upper 3 bytes
  uint uVar5;                        // i_first_received_section_number (+0x38)
  dvbpsi_psi_section_t *pdVar6;      // section list cursor in completion check
  uint8_t *puVar7;                   // p_section->p_payload_start
  dvbpsi_callback_gather_t p_Var8;   // copy of current_eit bytes +0x08..+0x0b
  dvbpsi_eit_t *pdVar9;              // p_building_eit (dvbpsi_eit_t *)
  undefined4 uVar10;                 // p_eit_decoder->p_cb_data (+0x20)
  undefined1 b_force;                // scratch arg passed to dvbpsi_ReInitEIT (ignored by it)
  dvbpsi_psi_section_t *pdVar11;     // p_section / p_private_decoder->p_sections
  uint8_t table_id;                  // expected table_id passed to dvbpsi_CheckPSISection
  dvbpsi_psi_section_t *pdVar12;     // copy of current_eit bytes +0x00..+0x03
  dvbpsi_decoder_t *pdVar13;         // p_demux (p_dvbpsi->p_decoder)
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_eit_decoder_t * p_eit_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/eit.c",0x1ab,"dvbpsi_eit_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/eit.c",0x1ac,"dvbpsi_eit_sections_gather");
  }
  // Accept table_id in [0x4e, 0x6f]: (uint8_t)(i_table_id - 0x4e) < 0x22
  table_id = 0x4e;
  if ((byte)(p_section->i_table_id + 0xb2) < 0x22) {
    table_id = p_section->i_table_id;
  }
  pdVar11 = p_section;
  _Var3 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"EIT decoder");
  if (CONCAT31(extraout_var,_Var3) == 0) goto LAB_0041c7e8;
  pdVar13 = p_dvbpsi->p_decoder;
  if (pdVar13->b_discontinuity == false) {
    iVar4 = p_private_decoder[1].i_need; // +0x34: p_building_eit
    if (iVar4 == 0) {
      if ((p_private_decoder->b_current_valid != false) &&
         (sVar2._0_1_ = p_section->i_version, sVar2._1_1_ = p_section->b_current_next,
         *(short *)&p_private_decoder[1].p_sections == sVar2)) {
        dvbpsi_debug(p_dvbpsi,"EIT decoder","ignoring already decoded section %d",
                     (uint)p_section->i_number);
        goto LAB_0041c7e8;
      }
    }
    else {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (*(uint16_t *)(iVar4 + 2) == p_section->i_extension) {
        if (*(uint8_t *)(iVar4 + 4) == p_section->i_version) {
          if (p_private_decoder->i_last_section_number == p_section->i_last_number)
          goto LAB_0041c5d8;
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          b_force = 0x20;
          dvbpsi_error(p_dvbpsi,"EIT decoder",
                       "\'version_number\' differs whereas no discontinuity has occurred");
        }
      }
      else {
        b_force = 0x20;
        dvbpsi_error(p_dvbpsi,"EIT decoder",
                     "\'service_id\' differs whereas no TS discontinuity has occurred");
      }
      dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,(_Bool)b_force);
    }
  }
  else {
    dvbpsi_ReInitEIT((dvbpsi_eit_decoder_t *)p_private_decoder,SUB41(pdVar11,0));
    p_private_decoder->b_discontinuity = false;
    pdVar13->b_discontinuity = false;
  }
LAB_0041c5d8:
  if (p_private_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_eit_decoder","tables/eit.c",0x181,"dvbpsi_AddSectionEIT");
  }
  if (p_private_decoder[1].i_need == 0) {
    puVar7 = p_section->p_payload_start;
    pdVar9 = dvbpsi_eit_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                            p_section->b_current_next,CONCAT11(*puVar7,puVar7[1]),
                            CONCAT11(puVar7[2],puVar7[3]),puVar7[4],puVar7[5]);
    p_private_decoder[1].i_need = (int)pdVar9;
    p_private_decoder->i_last_section_number = p_section->i_last_number;
    p_private_decoder[2].i_magic[0] = p_section->i_number; // +0x38: i_first_received_section_number
    if (pdVar9 == (dvbpsi_eit_t *)0x0) {
      dvbpsi_error(p_dvbpsi,"EIT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041c7e8:
      dvbpsi_DeletePSISections(p_section);
      return;
    }
    p_private_decoder->i_last_section_number = p_section->i_last_number;
  }
  _Var3 = dvbpsi_decoder_psi_section_add(p_private_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var3) != 0) {
    dvbpsi_debug(p_dvbpsi,"EIT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
                    /* Unresolved local var: _Bool b_complete@[???]
                       Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
  // Fast check: table can only be complete when we hit last_section_number (if started at 0)
  // or wrapped back to i_first_received_section_number - 1
  uVar5 = (uint)p_private_decoder[2].i_magic[0];
  if (uVar5 == 0) {
    if (p_section->i_number != p_private_decoder->i_last_section_number) {
      return;
    }
  }
  else if ((uVar5 != p_section->i_number) && ((uint)p_section->i_number != uVar5 - 1)) {
    return;
  }
                    /* Unresolved local var: dvbpsi_psi_section_t * p@[???] */
  // Walk p_sections verifying EIT segment continuity (p_payload_start[4] = segment_last_section_number)
  pdVar11 = p_private_decoder->p_sections;
  if (pdVar11 != (dvbpsi_psi_section_t *)0x0) {
    bVar1 = pdVar11->i_number;
    pdVar6 = pdVar11;
    while( true ) {
      while( true ) {
        if (bVar1 == p_private_decoder->i_last_section_number) {
          iVar4._0_1_ = p_private_decoder[1].i_magic[0];
          iVar4._1_1_ = p_private_decoder[1].i_magic[1];
          iVar4._2_1_ = p_private_decoder[1].i_magic[2];
          iVar4._3_1_ = p_private_decoder[1].b_complete_header;
          if (iVar4 == 0) {
                    /* WARNING: Subroutine does not return */
            __assert_fail("p_eit_decoder->pf_eit_callback","tables/eit.c",0x1eb,
                          "dvbpsi_eit_sections_gather");
          }
          // Copy *p_building_eit into current_eit (+0x24..+0x33)
          pdVar9 = (dvbpsi_eit_t *)p_private_decoder[1].i_need;
          pdVar12 = *(dvbpsi_psi_section_t **)pdVar9;
          pdVar6 = *(dvbpsi_psi_section_t **)&pdVar9->i_version;
          p_Var8 = *(dvbpsi_callback_gather_t *)&pdVar9->i_network_id;
          p_private_decoder[1].i_section_max_size = (int)pdVar9->p_first_event;
          p_private_decoder[1].p_current_section = pdVar12;
          p_private_decoder[1].p_sections = pdVar6;
          p_private_decoder[1].pf_gather = p_Var8;
          p_private_decoder->b_current_valid = true;
          dvbpsi_eit_sections_decode(pdVar9,pdVar11);
          dvbpsi_DeletePSISections(p_private_decoder->p_sections);
          uVar10._0_1_ = p_private_decoder[1].b_discontinuity;
          uVar10._1_1_ = p_private_decoder[1].b_current_valid;
          uVar10._2_1_ = p_private_decoder[1].i_continuity_counter;
          uVar10._3_1_ = p_private_decoder[1].i_last_section_number;
          iVar4 = p_private_decoder[1].i_need;
          p_private_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
          (**(code **)(p_private_decoder + 1))(uVar10,iVar4); // pf_eit_callback(p_cb_data, p_building_eit)
          p_private_decoder[1].i_need = 0; // caller owns p_building_eit; no ReInitEIT needed
          return;
        }
        if (pdVar6->p_payload_start[4] != bVar1) break;
        do {
          pdVar6 = pdVar6->p_next;
          if (pdVar6 == (dvbpsi_psi_section_s *)0x0) {
            return;
          }
          bVar1 = pdVar6->i_number;
        } while (bVar1 < p_private_decoder->i_last_section_number);
      }
      pdVar6 = pdVar6->p_next;
      if (pdVar6 == (dvbpsi_psi_section_s *)0x0) break;
      bVar1 = pdVar6->i_number;
    }
  }
  return;
}



/* dvbpsi_eit_sections_generate @ 0041c960
 * Role (upstream libdvbpsi 1.x): Serializes a dvbpsi_eit_t and its event/
 *   descriptor lists into a linked list of 4094-byte (0xffe) PSI sections via
 *   NewEITSection() and finalizes CRC32 via dvbpsi_BuildPSISection().
 * Parameters: p_dvbpsi, p_eit, i_table_id (e.g. 0x4e).
 * Returns: head of linked dvbpsi_psi_section_t * list, or NULL on alloc failure.
 * Callers: genEIT() in 01_libxcode4encoder.c.c (active path in av_streamer). */
dvbpsi_psi_section_t *
dvbpsi_eit_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_eit_t *p_eit,uint8_t i_table_id)

{
  dvbpsi_psi_section_s **ppdVar1;    // &p_current->p_next in finalization loop
  int iVar2;                         // projected byte offset in section
  dvbpsi_psi_section_t *pdVar3;      // head of section list (p_result)
  dvbpsi_descriptor_t *pdVar4;       // descriptor list cursor
  uint8_t *puVar5;                   // write cursor in current section
  dvbpsi_psi_section_t *pdVar6;      // newly allocated overflow section
  uint uVar7;                        // accumulated byte size of event + descriptors
  uint8_t *puVar8;                   // projected end offset for overflow check (< 0xffb)
  dvbpsi_psi_section_t *pdVar9;      // current section being populated (p_current)
  dvbpsi_eit_event_t *pdVar10;       // event list cursor (p_event)
  uint8_t *puVar11;                  // start of current event entry (p_event_start)
  uint i_section_number;             // running section_number (becomes last_section_number)
  byte *pbVar12;                     // pointer to byte 10 of event entry (descriptors_loop_length hi)
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_psi_section_t * p_current@[???]
                       Unresolved local var: uint8_t i_last_section_number@[???]
                       Unresolved local var: dvbpsi_eit_event_t * p_event@[???] */
  pdVar3 = NewEITSection(p_eit,(uint)i_table_id,0);
  if (pdVar3 != (dvbpsi_psi_section_t *)0x0) {
    pdVar10 = p_eit->p_first_event;
    i_section_number = 0;
    if (pdVar10 != (dvbpsi_eit_event_t *)0x0) {
                    /* Unresolved local var: uint8_t * p_event_start@[???]
                       Unresolved local var: uint16_t i_event_length@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
      pdVar9 = pdVar3;
LAB_0041c9d8:
      do {
        pdVar4 = pdVar10->p_first_descriptor;
        puVar11 = pdVar9->p_payload_end;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          // Estimate space needed for 12B event header + 2B desc hdr + first desc payload
          uVar7 = pdVar4->i_length + 0xe;
          iVar2 = uVar7 - (int)pdVar9->p_data;
          while ((int)(puVar11 + iVar2) < 0xffb) { // 0xffb = 4091 (+ 4B CRC = 4095 <= 4096)
            pdVar4 = pdVar4->p_next;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041ca38;
            uVar7 = uVar7 + pdVar4->i_length + 2 & 0xffff;
            iVar2 = uVar7 - (int)pdVar9->p_data;
          }
          i_section_number = i_section_number + 1 & 0xff;
          pdVar6 = NewEITSection(p_eit,(uint)i_table_id,i_section_number);
          puVar11 = pdVar6->p_payload_end;
          pdVar9->p_next = pdVar6;
          pdVar9 = pdVar6;
        }
LAB_0041ca38:
        pbVar12 = puVar11 + 10;
        *puVar11 = (uint8_t)(pdVar10->i_event_id >> 8);
        puVar11[1] = (uint8_t)pdVar10->i_event_id;
        // Serialize 40-bit start_time (bytes +3..+7 of little-endian uint64_t? Note MIPS-LE layout)
        puVar11[2] = (uint8_t)*(undefined4 *)((int)&pdVar10->i_start_time + 4);
        puVar11[3] = *(uint8_t *)((int)&pdVar10->i_start_time + 3);
        puVar11[4] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_start_time + 2);
        puVar11[5] = (uint8_t)(pdVar10->i_start_time >> 8);
        puVar11[6] = (uint8_t)pdVar10->i_start_time;
        // Serialize 24-bit duration
        puVar11[7] = (uint8_t)*(undefined2 *)((int)&pdVar10->i_duration + 2);
        puVar11[8] = (uint8_t)(pdVar10->i_duration >> 8);
        puVar11[9] = (uint8_t)pdVar10->i_duration;
        puVar11[10] = (pdVar10->b_free_ca & 1U) << 4 | pdVar10->i_running_status << 5;
        pdVar4 = pdVar10->p_first_descriptor;
        puVar5 = pdVar9->p_payload_end + 0xc;
        pdVar9->p_payload_end = puVar5;
        pdVar9->i_length = pdVar9->i_length + 0xc;
        if (pdVar4 != (dvbpsi_descriptor_t *)0x0) {
          puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          while ((int)puVar8 < 0xffb) {
            *puVar5 = pdVar4->i_tag;
            pdVar9->p_payload_end[1] = pdVar4->i_length;
            memcpy(pdVar9->p_payload_end + 2,pdVar4->p_data,(size_t)pdVar4->i_length);
            puVar5 = pdVar9->p_payload_end + pdVar4->i_length + 2;
            pdVar9->p_payload_end = puVar5;
            puVar8 = &pdVar4->i_length;
            pdVar4 = pdVar4->p_next;
            pdVar9->i_length = pdVar9->i_length + 2 + (ushort)*puVar8;
            if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041cb74;
            puVar8 = puVar5 + ((uint)pdVar4->i_length - (int)pdVar9->p_data);
          }
          dvbpsi_error(p_dvbpsi,"EIT generator",
                       "too many descriptors in event, unable to carry all the descriptors");
          puVar5 = pdVar9->p_payload_end;
          *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
          puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
          pdVar10 = pdVar10->p_next;
          if (pdVar10 == (dvbpsi_eit_event_s *)0x0) {
            pdVar9 = pdVar3;
            goto LAB_0041cba4;
          }
          goto LAB_0041c9d8;
        }
LAB_0041cb74:
        // Backfill 12-bit descriptors_loop_length in puVar11[10..11]
        *pbVar12 = (byte)((uint)(puVar5 + (-0xc - (int)puVar11)) >> 8) & 0xf | *pbVar12;
        puVar11[0xb] = (uint8_t)(puVar5 + (-0xc - (int)puVar11));
        pdVar10 = pdVar10->p_next;
      } while (pdVar10 != (dvbpsi_eit_event_s *)0x0);
    }
    pdVar9 = pdVar3;
LAB_0041cba4:
    do {
      pdVar9->p_data[0xc] = (uint8_t)i_section_number; // segment_last_section_number
      pdVar9->i_last_number = (uint8_t)i_section_number;
      dvbpsi_BuildPSISection(p_dvbpsi,pdVar9);
      ppdVar1 = &pdVar9->p_next;
      pdVar9 = *ppdVar1;
    } while (*ppdVar1 != (dvbpsi_psi_section_s *)0x0);
  }
  return pdVar3;
}
