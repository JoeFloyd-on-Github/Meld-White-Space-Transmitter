// ==== dvbpsi_IsDescriptorDecoded @ 0041f170 ====

_Bool dvbpsi_IsDescriptorDecoded(dvbpsi_descriptor_t *p_descriptor)

{
  return p_descriptor->p_decoded != (void *)0x0;
}



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



// ==== dvbpsi_NewDescriptor @ 0041f19c ====

dvbpsi_descriptor_t * dvbpsi_NewDescriptor(uint8_t i_tag,uint8_t i_length,uint8_t *p_data)

{
  dvbpsi_descriptor_t *__ptr;
  uint8_t *__dest;
  
                    /* Unresolved local var: dvbpsi_descriptor_t * p_descriptor@[???] */
  __ptr = malloc(0x10);
  if (__ptr != (dvbpsi_descriptor_t *)0x0) {
    __dest = malloc((size_t)i_length);
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



// ==== dvbpsi_AddDescriptor @ 0041f268 ====

dvbpsi_descriptor_t *
dvbpsi_AddDescriptor(dvbpsi_descriptor_t *p_list,dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_t *pdVar1;
  
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



// ==== dvbpsi_DeleteDescriptors @ 0041f2e0 ====

void dvbpsi_DeleteDescriptors(dvbpsi_descriptor_t *p_descriptor)

{
  dvbpsi_descriptor_s *pdVar1;
  
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



// ==== dvbpsi_DuplicateDecodedDescriptor @ 0041f364 ====

void * dvbpsi_DuplicateDecodedDescriptor(void *p_decoded,ssize_t i_size)

{
  void *__dest;
  
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



