/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef MM_TLB
/*
 * Memory physical based TLB Cache
 * TLB cache module tlb/tlbcache.c
 *
 * TLB cache is physically memory phy
 * supports random access 
 * and runs at high speed
 */


#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

#define init_tlbcache(mp,sz,...) init_memphy(mp, sz, (1, ##__VA_ARGS__))

/*
 *  tlb_cache_read read TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_read(struct pcb_t* proc, struct memphy_struct * mp, int pid, int pgnum, BYTE* value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   if(mp == NULL){
      return -1;
   } 
   //* 
   uint32_t tlbIndex = (uint32_t)pgnum % mp->maxsz;
   if(mp->storage[tlbIndex] == -1){
      //* do not have that entry in tlb, fail to read
      //* update the tlb entries outside this function
      return -1;
   }
   //* checking pid
   if(pid != mp->pid_hold){
      tlb_flush_tlb_of(proc, mp);
      return 0;
   }
   *value = mp->storage[tlbIndex];
   //* value 32-bit
   return 0;
}

/*
 *  tlb_cache_write write TLB cache device
 *  @mp: memphy struct
 *  @pid: process id
 *  @pgnum: page number
 *  @value: obtained value
 */
int tlb_cache_write(struct pcb_t* proc, struct memphy_struct *mp, int pid, int pgnum, BYTE value)
{
   /* TODO: the identify info is mapped to 
    *      cache line by employing:
    *      direct mapped, associated mapping etc.
    */
   if(mp == NULL){
         return -1;
      }
   if(pid != mp->pid_hold){
      tlb_flush_tlb_of(proc, mp);
      //* update pid hold
      mp->pid_hold = pid;
      return 0;
   }
   uint32_t address = (uint32_t)pgnum % mp->maxsz;
   mp->storage[address] = value;
   return 0;

}

/*
 *  TLBMEMPHY_read natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @value: obtained value
 */
int TLBMEMPHY_read(struct memphy_struct * mp, int addr, BYTE *value)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   *value = mp->storage[addr];

   return 0;
}


/*
 *  TLBMEMPHY_write natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 *  @addr: address
 *  @data: written data
 */
int TLBMEMPHY_write(struct memphy_struct * mp, int addr, BYTE data)
{
   if (mp == NULL)
     return -1;

   /* TLB cached is random access by native */
   mp->storage[addr] = data;

   return 0;
}

/*
 *  TLBMEMPHY_format natively supports MEMPHY device interfaces
 *  @mp: memphy struct
 */


int TLBMEMPHY_dump(struct memphy_struct * mp)
{
   /*TODO dump memphy contnt mp->storage 
    *     for tracing the memory content
    */
   for (int i = 0; i < mp->maxsz; i++){
      printf("TLB[%d]: %d\n", i, mp->storage[i]);
   }
   return 0;
}


/*
 *  Init TLBMEMPHY struct
 */
int init_tlbmemphy(struct memphy_struct *mp, int max_size)
{
   mp->storage = (BYTE *)malloc(max_size*sizeof(BYTE));
   for(int i = 0; i < max_size; i++){
      mp->storage[i] = -1;
   }
   mp->pid_hold = 0;
   mp->maxsz = max_size;
   mp->pid_hold = -1;
   mp->rdmflg = 1;

   return 0;
}

//#endif
