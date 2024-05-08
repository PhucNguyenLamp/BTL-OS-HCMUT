/*
 * Copyright (C) 2024 pdnguyen of the HCMC University of Technology
 */
/*
 * Source Code License Grant: Authors hereby grants to Licensee 
 * a personal to use and modify the Licensed Source Code for 
 * the sole purpose of studying during attending the course CO2018.
 */
//#ifdef CPU_TLB
/*
 * CPU TLB
 * TLB module cpu/cpu-tlb.c
 */
 
#include "mm.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

int tlb_change_all_page_tables_of(struct pcb_t *proc,  struct memphy_struct * mp)
{
  /* TODO update all page table directory info 
   *      in flush or wipe TLB (if needed)
   */

  return 0;
}

int tlb_flush_tlb_of(struct pcb_t *proc, struct memphy_struct * mp)
{
  /* TODO flush tlb cached*/
  // printf("Proc %d in tlbflush\n", proc->pid);
  if(mp == NULL){
    return -1;
  }
  int size_tlb = mp->maxsz;
  for(int i = 0; i < size_tlb; i++){
    mp->storage[i]  = -1;
  }
  return 0;
}

/*tlballoc - CPU TLB-based allocate a region memory
 *@proc:  Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
int tlballoc(struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
  int addr, val;
  /* By default using vmaid = 0 */
  val = __alloc(proc, 0, reg_index, size, &addr); 
  printf("\n********************\n");
  printf("Proc %d in tlballoc, size: %d of region index: %d\n", proc->pid,size,  reg_index);
  printf("Rg start and end of region: %ld %ld\n", proc->mm->symrgtbl[reg_index].rg_start, proc->mm->symrgtbl[reg_index].rg_end);
  struct vm_area_struct* cur_vma = get_vma_by_num(proc->mm, 0);
  printf("The start: %ld, end: %ld, srbk: %ld of current vma of this process\n", cur_vma->vm_start, cur_vma->vm_end, cur_vma->sbrk);
  printf("*************************\n");
  /* TODO update TLB CACHED frame num of the new allocated page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  int page_number = PAGING_PGN(addr);
  printf("Addr: %d\n", addr);
  int alloc_size = PAGING_PAGE_ALIGNSZ(size); //* if size is 300, then it returns 512
  int numpage = alloc_size / PAGING_PAGESZ;
  for(int i = 0; i < numpage; i++){
    uint32_t *fpn = malloc(sizeof(uint32_t));
    uint32_t pte = pg_getpage(proc->mm, page_number + i,(int*)fpn, proc);
    // printf("frame number getting from pg_getpage in alloc: %d\n", *fpn);
    tlb_cache_write(proc, proc->tlb, proc->pid, (page_number + i), *fpn);    
  }
  print_pgtbl(proc, 0, -1);
  
  return val;
}

/*pgfree - CPU TLB-based free a region memory
 *@proc: Process executing the instruction
 *@size: allocated size 
 *@reg_index: memory region ID (used to identify variable in symbole table)
 */
#define debugg
int tlbfree_data(struct pcb_t *proc, uint32_t reg_index)
{
  #ifdef debugg
  printf("\n********************\n");
  printf("Proc %d in tlbfree of region index: %d\n", proc->pid, reg_index);
  printf("Rg start and end of region freeid: %ld %ld\n", proc->mm->symrgtbl[reg_index].rg_start, proc->mm->symrgtbl[reg_index].rg_end);
  struct vm_area_struct* cur_vma = get_vma_by_num(proc->mm, 0);
  printf("The start: %ld, end: %ld, srbk: %ld of current vma of this process\n", cur_vma->vm_start, cur_vma->vm_end, cur_vma->sbrk);
  printf("*************************\n");
  #endif
  __free(proc, 0, reg_index);
  int start_region = proc->mm->symrgtbl[reg_index].rg_start;
  int end_region = proc->mm->symrgtbl[reg_index].rg_end;
  int size = end_region - start_region;
  int pgn = PAGING_PGN(start_region);
  int deallocate_sz = PAGING_PAGE_ALIGNSZ(size);
  int freed_pages = deallocate_sz / PAGING_PAGESZ;
  int freepages[freed_pages];
  for(int i = 0; i < freed_pages; i++){
    freepages[i] = 0;
  }
  int size_freepages = 0;
  for(int i = 0; i < PAGING_MAX_SYMTBL_SZ; i++){
    if( i == reg_index){
      continue;
    }
    int start = pgn * PAGING_PAGESZ;
    int end = (pgn + 1) * PAGING_PAGESZ;
    for(int j = 0; j < freed_pages; j++){
      if (proc->mm->symrgtbl[i].rg_start > end){
        freepages[size_freepages] = 1;
        size_freepages ++;
        start = end + 1;
        end = end + PAGING_PAGESZ;
      }
    }
  }
  while (size_freepages > 0){
    int status = freepages[size_freepages - 1];
    if (status){
      uint32_t *fpn = malloc(sizeof(uint32_t));
      uint32_t pte = pg_getpage(proc->mm, pgn + size_freepages, (int*)fpn, proc);
      tlb_cache_write(proc ,proc->tlb, proc->pid, (pgn + size_freepages), *fpn);    
    }
    size_freepages --;
  }
  print_pgtbl(proc, 0, -1);
  return 0;
}

/*tlbread - CPU TLB-based read a region memory
 *@proc: Process executing the instruction
 *@source: index of source register
 *@offset: source address = [source] + [offset]
 *@destination: destination storage
 */
int tlbread(struct pcb_t * proc, uint32_t source,
            uint32_t offset, 	uint32_t destination) 
{
  printf("Proc %d in tlbread data\n", proc->pid);
 BYTE data, frmnum = -1;
	
  /* TODO retrieve TLB CACHED frame num of accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/
  /* frmnum is return value of tlb_cache_read/write value*/
  int start_region = proc->mm->symrgtbl[source].rg_start + offset;
  int pgn = PAGING_PGN(start_region);
  uint32_t fpn_retrieved_from_tlb = 0;
  frmnum = tlb_cache_read(proc ,proc->tlb, proc->pid, pgn, &fpn_retrieved_from_tlb);
	
#ifdef IODUMP
  if (frmnum >= 0)
    printf("TLB hit at read region=%d offset=%d, and destination = %d\n", 
	         source, offset, destination);
  else 
    printf("TLB miss at read region=%d offset=%d\n", 
	         source, offset);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif

  int val = __read(proc, 0, source, offset, &data);
  // destination = (uint32_t) data;
  int val2 = __write(proc, 0, destination, 0, data);
  // int start_region2 = proc->mm->symrgtbl[destination].rg_start + 0;

  
  

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

  int fpn = pg_getpage(proc->mm, pgn, &fpn, proc);
  int status = tlb_cache_write(proc, proc->tlb, proc->pid, pgn, fpn);
  return val;
}


int tlbwrite(struct pcb_t * proc, BYTE data,
             uint32_t destination, uint32_t offset)
{

  printf("Proc %d in tlbwrite data\n", proc->pid);
  int val;
  BYTE frmnum = -1;

  /* TODO retrieve TLB CACHED frame num of accessing page(s))*/
  /* by using tlb_cache_read()/tlb_cache_write()
  frmnum is return value of tlb_cache_read/write value*/
  int start_region = proc->mm->symrgtbl[destination].rg_start + offset;
  // printf("Start region: %d\n", start_region);
  int pgn = PAGING_PGN(start_region);
  uint32_t fpn_retrieved_from_tlb = 0;
  frmnum = tlb_cache_read(proc ,proc->tlb, proc->pid, pgn, &fpn_retrieved_from_tlb);
  printf("Inside tlbwrite, print fpn_retrieved from tlb: %d\n", fpn_retrieved_from_tlb);

#ifdef IODUMP
  if (frmnum >= 0)
    printf("TLB hit at write region=%d offset=%d value=%d\n",
	          destination, offset, data);
	else
    printf("TLB miss at write region=%d offset=%d value=%d\n",
            destination, offset, data);
#ifdef PAGETBL_DUMP
  print_pgtbl(proc, 0, -1); //print max TBL
#endif
  MEMPHY_dump(proc->mram);
#endif

  val = __write(proc, 0, destination, offset, data);

  /* TODO update TLB CACHED with frame num of recent accessing page(s)*/
  /* by using tlb_cache_read()/tlb_cache_write()*/

  int fpn = pg_getpage(proc->mm, pgn, &fpn, proc);
  int status = tlb_cache_write(proc ,proc->tlb, proc->pid, pgn, fpn);
  return val;
}

//#endif
