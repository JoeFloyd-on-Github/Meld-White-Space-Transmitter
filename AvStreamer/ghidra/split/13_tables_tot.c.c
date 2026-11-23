// ==== dvbpsi_tot_attach @ 0041dd50 ====

_Bool dvbpsi_tot_attach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension,
                       dvbpsi_tot_callback pf_callback,void *p_cb_data)

{
  dvbpsi_demux_subdec_t *pdVar1;
  dvbpsi_decoder_t *p_decoder;
  dvbpsi_demux_t *p_demux;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x3d,"dvbpsi_tot_attach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x3e,"dvbpsi_tot_attach");
  }
  pdVar1 = dvbpsi_demuxGetSubDec(p_demux,i_table_id,0);
  if (pdVar1 == (dvbpsi_demux_subdec_t *)0x0) {
    p_decoder = dvbpsi_decoder_new((dvbpsi_callback_gather_t)0x0,0,true,0x3c);
    if (p_decoder != (dvbpsi_decoder_t *)0x0) {
      pdVar1 = dvbpsi_NewDemuxSubDecoder
                         (i_table_id,0,dvbpsi_tot_detach,dvbpsi_tot_sections_gather,p_decoder);
      if (pdVar1 != (dvbpsi_demux_subdec_t *)0x0) {
        dvbpsi_AttachDemuxSubDecoder(p_demux,pdVar1);
        *(dvbpsi_tot_callback *)(p_decoder + 1) = pf_callback;
        *(void **)&p_decoder[1].b_discontinuity = p_cb_data;
        p_decoder[2].i_magic[0] = 0;
        p_decoder[2].i_magic[1] = 0;
        p_decoder[2].i_magic[2] = 0;
        p_decoder[2].b_complete_header = false;
        return true;
      }
      dvbpsi_decoder_delete(p_decoder);
    }
  }
  else {
    dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                 "Already a decoder for (table_id == 0x%02x,extension == 0x%02x)",i_table_id,0);
  }
  return false;
}



// ==== dvbpsi_tot_init @ 0041def0 ====

void dvbpsi_tot_init(dvbpsi_tot_t *p_tot,uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,
                    _Bool b_current_next,uint64_t i_utc_time)

{
  if (p_tot == (dvbpsi_tot_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_tot","tables/tot.c",0x94,"dvbpsi_tot_init");
  }
  p_tot->i_table_id = i_table_id;
  p_tot->i_extension = i_extension;
  p_tot->i_version = i_version;
  p_tot->b_current_next = b_current_next;
  p_tot->i_utc_time = i_utc_time;
  p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  return;
}



// ==== dvbpsi_tot_new @ 0041df80 ====

dvbpsi_tot_t *
dvbpsi_tot_new(uint8_t i_table_id,uint16_t i_extension,uint8_t i_version,_Bool b_current_next,
              uint64_t i_utc_time)

{
  dvbpsi_tot_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_tot_t * p_tot@[DW_OP_reg2(v0)] */
  pdVar1 = malloc(0x11);
  if (pdVar1 != (dvbpsi_tot_t *)0x0) {
    pdVar1->i_table_id = i_table_id;
    pdVar1->i_extension = i_extension;
    pdVar1->i_version = i_version;
    pdVar1->b_current_next = b_current_next;
    pdVar1->i_utc_time = i_utc_time;
    pdVar1->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  }
  return pdVar1;
}



// ==== dvbpsi_tot_empty @ 0041e028 ====

void dvbpsi_tot_empty(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  
  puVar1 = (undefined1 *)((int)&p_tot->p_first_descriptor + 3);
  uVar2 = (uint)puVar1 & 3;
  uVar3 = (uint)&p_tot->p_first_descriptor & 3;
  dvbpsi_DeleteDescriptors
            ((dvbpsi_descriptor_t *)
             ((*(int *)(puVar1 + -uVar2) << (3 - uVar2) * 8 |
              (uint)p_tot & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
             *(uint *)((int)&p_tot->p_first_descriptor - uVar3) >> uVar3 * 8));
  p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  return;
}



// ==== dvbpsi_tot_delete @ 0041e06c ====

void dvbpsi_tot_delete(dvbpsi_tot_t *p_tot)

{
  undefined1 *puVar1;
  uint uVar2;
  uint uVar3;
  
  if (p_tot != (dvbpsi_tot_t *)0x0) {
    puVar1 = (undefined1 *)((int)&p_tot->p_first_descriptor + 3);
    uVar2 = (uint)puVar1 & 3;
    uVar3 = (uint)&p_tot->p_first_descriptor & 3;
    dvbpsi_DeleteDescriptors
              ((dvbpsi_descriptor_t *)
               ((*(int *)(puVar1 + -uVar2) << (3 - uVar2) * 8 |
                (uint)p_tot & 0xffffffffU >> (uVar2 + 1) * 8) & -1 << (4 - uVar3) * 8 |
               *(uint *)((int)&p_tot->p_first_descriptor - uVar3) >> uVar3 * 8));
    p_tot->p_first_descriptor = (dvbpsi_descriptor_t *)0x0;
  }
  free(p_tot);
  return;
}



// ==== dvbpsi_ReInitTOT @ 0041e0c0 ====

void dvbpsi_ReInitTOT(dvbpsi_tot_decoder_t *p_decoder,_Bool b_force)

{
  undefined3 in_register_00000015;
  
  if (p_decoder == (dvbpsi_tot_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_decoder","tables/tot.c",0xe0,"dvbpsi_ReInitTOT");
  }
  dvbpsi_decoder_reset((dvbpsi_decoder_t *)p_decoder,b_force);
  if ((CONCAT31(in_register_00000015,b_force) != 0) &&
     (p_decoder->p_building_tot != (dvbpsi_tot_t *)0x0)) {
    dvbpsi_tot_delete(p_decoder->p_building_tot);
  }
  p_decoder->p_building_tot = (dvbpsi_tot_t *)0x0;
  return;
}



// ==== dvbpsi_tot_detach @ 0041e14c ====

void dvbpsi_tot_detach(dvbpsi_t *p_dvbpsi,uint8_t i_table_id,uint16_t i_extension)

{
  dvbpsi_demux_subdec_t *p_subdec;
  dvbpsi_demux_t *p_demux;
  dvbpsi_decoder_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_demux_t * p_demux@[???]
                       Unresolved local var: dvbpsi_demux_subdec_t * p_subdec@[???]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x6f,"dvbpsi_tot_detach");
  }
  p_demux = (dvbpsi_demux_t *)p_dvbpsi->p_decoder;
  if (p_demux == (dvbpsi_demux_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x70,"dvbpsi_tot_detach");
  }
  p_subdec = dvbpsi_demuxGetSubDec(p_demux,i_table_id,0);
  pdVar1 = p_subdec->p_decoder;
  if (pdVar1 == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_subdec->p_decoder","tables/tot.c",0x80,"dvbpsi_tot_detach");
  }
  if (*(dvbpsi_tot_t **)(pdVar1 + 2) != (dvbpsi_tot_t *)0x0) {
    dvbpsi_tot_delete(*(dvbpsi_tot_t **)(pdVar1 + 2));
  }
  pdVar1[2].i_magic[0] = 0;
  pdVar1[2].i_magic[1] = 0;
  pdVar1[2].i_magic[2] = 0;
  pdVar1[2].b_complete_header = false;
  dvbpsi_DetachDemuxSubDecoder(p_demux,p_subdec);
  dvbpsi_DeleteDemuxSubDecoder(p_subdec);
  return;
}



// ==== dvbpsi_tot_descriptor_add @ 0041e258 ====

dvbpsi_descriptor_t *
dvbpsi_tot_descriptor_add(dvbpsi_tot_t *p_tot,uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *p_descriptor;
  dvbpsi_descriptor_t *pdVar1;
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  p_descriptor = dvbpsi_NewDescriptor(i_tag,i_length,p_data);
  if (p_descriptor != (dvbpsi_descriptor_t *)0x0) {
    pdVar1 = dvbpsi_AddDescriptor(p_tot->p_first_descriptor,p_descriptor);
    p_tot->p_first_descriptor = pdVar1;
    if (pdVar1 == (dvbpsi_descriptor_t *)0x0) {
                    /* WARNING: Subroutine does not return */
      __assert_fail("p_tot->p_first_descriptor","tables/tot.c",0xd6,"dvbpsi_tot_descriptor_add");
    }
  }
  return p_descriptor;
}



// ==== dvbpsi_tot_sections_decode @ 0041e2f8 ====

void dvbpsi_tot_sections_decode
               (dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot,dvbpsi_psi_section_t *p_section)

{
  byte bVar1;
  byte bVar2;
  uint8_t uVar3;
  byte *pbVar4;
  byte i_tag;
  uint8_t *p_data;
  byte *pbVar5;
  byte *pbVar6;
  uint uVar7;
  
  if (p_section != (dvbpsi_psi_section_t *)0x0) {
    uVar3 = p_section->i_table_id;
                    /* Unresolved local var: uint8_t * p_byte@[???] */
    if ((uVar3 == 0x70) && (p_section->i_length != 5)) {
      dvbpsi_error(p_dvbpsi,"TDT decoder","TDT has an invalid payload size (%d bytes) !!!");
      return;
    }
    pbVar5 = p_section->p_payload_start;
    pbVar4 = p_section->p_payload_end;
    if (pbVar5 + 5 <= pbVar4) {
      bVar1 = *pbVar5;
      *(uint *)&p_tot->i_utc_time =
           (uint)pbVar5[4] | (uint)pbVar5[1] << 0x18 | (uint)pbVar5[2] << 0x10 |
           (uint)pbVar5[3] << 8;
      *(uint *)((int)&p_tot->i_utc_time + 4) = (uint)bVar1;
      uVar3 = p_section->i_table_id;
      pbVar5 = pbVar5 + 5;
    }
    p_data = pbVar5 + 4;
    if (uVar3 == 0x73) {
                    /* Unresolved local var: uint16_t i_loop_length@[???]
                       Unresolved local var: uint8_t * p_end@[???] */
      bVar1 = *pbVar5;
      bVar2 = pbVar5[1];
      pbVar6 = pbVar5 + 2;
      if (p_data <= pbVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2)) {
                    /* Unresolved local var: uint8_t i_tag@[???]
                       Unresolved local var: uint8_t i_length@[???] */
        uVar7 = (uint)pbVar5[3];
        i_tag = *pbVar6;
        if ((int)(uVar7 + 1) < (int)pbVar4 - (int)pbVar6) goto LAB_0041e434;
        while( true ) {
          pbVar6 = pbVar6 + uVar7 + 2;
          p_data = pbVar6 + 2;
          if (pbVar5 + ((bVar1 & 0xf) << 8 | (uint)bVar2) < p_data) break;
          uVar7 = (uint)pbVar6[1];
          i_tag = *pbVar6;
          if ((int)(uVar7 + 1) < (int)p_section->p_payload_end - (int)pbVar6) {
LAB_0041e434:
            dvbpsi_tot_descriptor_add(p_tot,i_tag,(uint8_t)uVar7,p_data);
          }
        }
      }
    }
  }
  return;
}



// ==== dvbpsi_tot_sections_gather @ 0041e484 ====

void dvbpsi_tot_sections_gather
               (dvbpsi_t *p_dvbpsi,dvbpsi_decoder_t *p_decoder,dvbpsi_psi_section_t *p_section)

{
  uint8_t table_id;
  _Bool _Var1;
  undefined3 extraout_var;
  undefined3 extraout_var_00;
  undefined3 extraout_var_01;
  int iVar2;
  int iVar3;
  undefined3 extraout_var_02;
  uint8_t *puVar4;
  dvbpsi_callback_gather_t p_Var5;
  dvbpsi_psi_section_t *pdVar6;
  undefined4 uVar7;
  dvbpsi_tot_t *pdVar8;
  undefined4 uVar9;
  dvbpsi_psi_section_t *p_section_00;
  
                    /* Unresolved local var: uint8_t i_table_id@[DW_OP_reg2(v0)]
                       Unresolved local var: dvbpsi_tot_decoder_t * p_tot_decoder@[???] */
  if (p_dvbpsi == (dvbpsi_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail((char *)&DAT_004244f0,"tables/tot.c",0x139,"dvbpsi_tot_sections_gather");
  }
  if (p_dvbpsi->p_decoder == (dvbpsi_decoder_t *)0x0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_dvbpsi->p_decoder","tables/tot.c",0x13a,"dvbpsi_tot_sections_gather");
  }
  table_id = p_section->i_table_id;
  if ((table_id == 0x70) || (table_id == 0x73)) {
    _Var1 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,table_id,"TDT/TOT decoder");
    iVar3 = CONCAT31(extraout_var_02,_Var1);
  }
  else {
    _Var1 = dvbpsi_CheckPSISection(p_dvbpsi,p_section,0x70,"TDT/TOT decoder");
    iVar3 = CONCAT31(extraout_var,_Var1);
  }
  if (iVar3 == 0) goto LAB_0041e69c;
  if (p_decoder->b_discontinuity == false) {
    iVar3 = *(int *)(p_decoder + 2);
    if (iVar3 != 0) {
                    /* Unresolved local var: _Bool b_reinit@[???]
                       Unresolved local var: char[16] __PRETTY_FUNCTION__@[???] */
      if (p_section->i_extension == *(uint16_t *)(iVar3 + 1)) {
        if (*(uint8_t *)(iVar3 + 3) == p_section->i_version) {
          if (p_decoder->i_last_section_number == p_section->i_last_number) goto LAB_0041e514;
          dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                       "\'last_section_number\' differs whereas no discontinuity has occured");
        }
        else {
          dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                       "\'version_number\' differs whereas no discontinuity has occured");
        }
      }
      else {
        dvbpsi_error(p_dvbpsi,"TDT/TOT decoder",
                     "\'transport_stream_id\' differs whereas no TS discontinuity has occured");
      }
      dvbpsi_ReInitTOT((dvbpsi_tot_decoder_t *)p_decoder,true);
      goto LAB_0041e508;
    }
  }
  else {
    p_decoder->b_discontinuity = false;
LAB_0041e508:
                    /* Unresolved local var: char[21] __PRETTY_FUNCTION__@[???] */
    iVar3._0_1_ = p_decoder[2].i_magic[0];
    iVar3._1_1_ = p_decoder[2].i_magic[1];
    iVar3._2_1_ = p_decoder[2].i_magic[2];
    iVar3._3_1_ = p_decoder[2].b_complete_header;
    if (iVar3 != 0) goto LAB_0041e514;
  }
  puVar4 = p_section->p_payload_start;
  pdVar8 = dvbpsi_tot_new(p_section->i_table_id,p_section->i_extension,p_section->i_version,
                          p_section->b_current_next,
                          (ulonglong)
                          CONCAT14(*puVar4,(uint)puVar4[4] | (uint)puVar4[1] << 0x18 |
                                           (uint)puVar4[2] << 0x10 | (uint)puVar4[3] << 8));
  *(dvbpsi_tot_t **)(p_decoder + 2) = pdVar8;
  if (pdVar8 == (dvbpsi_tot_t *)0x0) {
    dvbpsi_error(p_dvbpsi,"TOT decoder","failed decoding section %d",(uint)p_section->i_number);
LAB_0041e69c:
    dvbpsi_DeletePSISections(p_section);
    return;
  }
  p_decoder->i_last_section_number = p_section->i_last_number;
LAB_0041e514:
  _Var1 = dvbpsi_decoder_psi_section_add(p_decoder,p_section);
  if (CONCAT31(extraout_var_00,_Var1) != 0) {
    dvbpsi_debug(p_dvbpsi,"TOT decoder","overwrite section number %d",(uint)p_section->i_number);
  }
  _Var1 = dvbpsi_decoder_psi_sections_completed(p_decoder);
  if (CONCAT31(extraout_var_01,_Var1) == 0) {
    return;
  }
  iVar2._0_1_ = p_decoder[1].i_magic[0];
  iVar2._1_1_ = p_decoder[1].i_magic[1];
  iVar2._2_1_ = p_decoder[1].i_magic[2];
  iVar2._3_1_ = p_decoder[1].b_complete_header;
  if (iVar2 == 0) {
                    /* WARNING: Subroutine does not return */
    __assert_fail("p_tot_decoder->pf_tot_callback","tables/tot.c",0x178,"dvbpsi_tot_sections_gather"
                 );
  }
  pdVar8 = *(dvbpsi_tot_t **)(p_decoder + 2);
  p_section_00 = p_decoder->p_sections;
  pdVar6 = *(dvbpsi_psi_section_t **)&pdVar8->b_current_next;
  p_Var5 = *(dvbpsi_callback_gather_t *)((int)&pdVar8->i_utc_time + 3);
  iVar3 = *(int *)((int)&pdVar8->i_utc_time + 7);
  p_decoder[1].p_current_section = *(dvbpsi_psi_section_t **)pdVar8;
  p_decoder[1].pf_gather = p_Var5;
  p_decoder[1].p_sections = pdVar6;
  p_decoder[1].i_section_max_size = iVar3;
  *(undefined1 *)&p_decoder[1].i_need = *(undefined1 *)((int)&pdVar8->p_first_descriptor + 3);
  p_decoder->b_current_valid = true;
  dvbpsi_tot_sections_decode(p_dvbpsi,pdVar8,p_section_00);
  dvbpsi_DeletePSISections(p_decoder->p_sections);
  uVar7._0_1_ = p_decoder[1].b_discontinuity;
  uVar7._1_1_ = p_decoder[1].b_current_valid;
  uVar7._2_1_ = p_decoder[1].i_continuity_counter;
  uVar7._3_1_ = p_decoder[1].i_last_section_number;
  uVar9._0_1_ = p_decoder[2].i_magic[0];
  uVar9._1_1_ = p_decoder[2].i_magic[1];
  uVar9._2_1_ = p_decoder[2].i_magic[2];
  uVar9._3_1_ = p_decoder[2].b_complete_header;
  p_decoder->p_sections = (dvbpsi_psi_section_t *)0x0;
  (**(code **)(p_decoder + 1))(uVar7,uVar9);
  dvbpsi_ReInitTOT((dvbpsi_tot_decoder_t *)p_decoder,false);
  return;
}



// ==== dvbpsi_tot_sections_generate @ 0041e808 ====

dvbpsi_psi_section_t * dvbpsi_tot_sections_generate(dvbpsi_t *p_dvbpsi,dvbpsi_tot_t *p_tot)

{
  dvbpsi_psi_section_t *p_section;
  uint8_t *puVar1;
  uint8_t uVar2;
  uint8_t *puVar3;
  dvbpsi_descriptor_t *pdVar4;
  
                    /* Unresolved local var: dvbpsi_psi_section_t * p_result@[???]
                       Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  pdVar4 = p_tot->p_first_descriptor;
  if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
    p_section = dvbpsi_NewPSISection(8);
    uVar2 = 0x70;
  }
  else {
    p_section = dvbpsi_NewPSISection(0x1000);
    uVar2 = 0x73;
  }
  puVar1 = p_section->p_data;
  p_section->i_table_id = uVar2;
  p_section->b_syntax_indicator = false;
  p_section->b_private_indicator = false;
  p_section->i_length = 5;
  p_section->p_payload_start = puVar1 + 3;
  p_section->p_payload_end = puVar1 + 8;
  puVar1[3] = (uint8_t)*(undefined4 *)((int)&p_tot->i_utc_time + 4);
  p_section->p_data[4] = (uint8_t)(p_tot->i_utc_time >> 0x18);
  p_section->p_data[5] = (uint8_t)(p_tot->i_utc_time >> 0x10);
  p_section->p_data[6] = (uint8_t)(p_tot->i_utc_time >> 8);
  p_section->p_data[7] = (uint8_t)p_tot->i_utc_time;
  if (p_section->i_table_id == 0x73) {
    puVar1 = p_section->p_payload_end + 2;
    p_section->p_payload_end = puVar1;
    p_section->i_length = p_section->i_length + 2;
    if (pdVar4 == (dvbpsi_descriptor_t *)0x0) {
LAB_0041eb04:
      p_section->p_payload_start[5] = 0xf0;
      p_section->p_payload_start[6] = (char)p_section->i_length - 7;
    }
    else {
      puVar3 = puVar1 + ((uint)pdVar4->i_length - (int)p_section->p_data);
      while ((int)puVar3 < 0xffb) {
        *puVar1 = pdVar4->i_tag;
        p_section->p_payload_end[1] = pdVar4->i_length;
        memcpy(p_section->p_payload_end + 2,pdVar4->p_data,(size_t)pdVar4->i_length);
        puVar1 = p_section->p_payload_end + pdVar4->i_length + 2;
        p_section->p_payload_end = puVar1;
        puVar3 = &pdVar4->i_length;
        pdVar4 = pdVar4->p_next;
        p_section->i_length = p_section->i_length + 2 + (ushort)*puVar3;
        if (pdVar4 == (dvbpsi_descriptor_s *)0x0) goto LAB_0041eb04;
        puVar3 = puVar1 + ((uint)pdVar4->i_length - (int)p_section->p_data);
      }
      dvbpsi_error(p_dvbpsi,"TDT/TOT generator",
                   "TOT does not fit into one section as it ought to be !!!");
      p_section->p_payload_start[5] = 0xf0;
      p_section->p_payload_start[6] = (char)p_section->i_length - 7;
    }
  }
  dvbpsi_BuildPSISection(p_dvbpsi,p_section);
  if (p_section->i_table_id == 0x73) {
    p_section->i_length = p_section->i_length + 4;
    p_section->p_payload_end = p_section->p_payload_end + 4;
    return p_section;
  }
  if ((p_section->i_table_id == 0x70) && (p_section->i_length != 5)) {
    dvbpsi_error(p_dvbpsi,"TDT decoder","TDT has an invalid payload size (%d bytes) !!!");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","********************************************");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","*  Generated TDT/TOT section is invalid.   *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","* THIS IS A BUG, PLEASE REPORT TO THE LIST *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","*  ---  libdvbpsi-devel@videolan.org  ---  *");
    dvbpsi_error(p_dvbpsi,"TDT/TOT generator","********************************************");
    return p_section;
  }
  return p_section;
}



