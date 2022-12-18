#include <defs.h>
#include <list.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_clock.h>
#include <x86.h>

static list_entry_t pra_list_head;

static int _clock_init_mm(struct mm_struct* mm) {
  list_init(&pra_list_head);
  mm->sm_priv = &pra_list_head;
  return 0;
}

static int _clock_map_swappable(
  struct mm_struct* mm, uintptr_t addr, struct Page* page, int swap_in) {
  list_entry_t* head = (list_entry_t*) mm->sm_priv;
  list_entry_t* entry = &(page->pra_page_link);

  assert(entry != NULL && head != NULL);
  list_add(head->prev, entry);
  struct Page* ptr = le2page(entry, pra_page_link);
  pte_t* pte = get_pte(mm->pgdir, ptr->pra_vaddr, 0);
  *pte &= ~PTE_D;
  return 0;
}

static int _clock_swap_out_victim(
  struct mm_struct* mm, struct Page** ptr_page, int in_tick) {
  list_entry_t* head = (list_entry_t*) mm->sm_priv;
  assert(head != NULL);
  assert(in_tick == 0);

  list_entry_t* p = head;
  for (;;) {
    p = list_next(p);
    if (p == head) p = list_next(p);
    struct Page* ptr = le2page(p, pra_page_link);
    pte_t* pte = get_pte(mm->pgdir, ptr->pra_vaddr, 0);
    if ((*pte & PTE_D) == 1) *pte &= ~PTE_D;
    else {
      *ptr_page = ptr;
      list_del(p);
      break;
    }
  }
  return 0;
}

static int _clock_check_swap(void) {
  cprintf("write Virt Page c in clock_check_swap\n");
  *(unsigned char*) 0x3000 = 0x0c;
  assert(pgfault_num == 4);
  cprintf("write Virt Page a in clock_check_swap\n");
  *(unsigned char*) 0x1000 = 0x0a;
  assert(pgfault_num == 4);
  cprintf("write Virt Page d in clock_check_swap\n");
  *(unsigned char*) 0x4000 = 0x0d;
  assert(pgfault_num == 4);
  cprintf("write Virt Page b in clock_check_swap\n");
  *(unsigned char*) 0x2000 = 0x0b;
  assert(pgfault_num == 4);
  cprintf("write Virt Page e in clock_check_swap\n");
  *(unsigned char*) 0x5000 = 0x0e;
  assert(pgfault_num == 5);
  cprintf("write Virt Page b in clock_check_swap\n");
  *(unsigned char*) 0x2000 = 0x0b;
  assert(pgfault_num == 5);
  cprintf("write Virt Page a in clock_check_swap\n");
  *(unsigned char*) 0x1000 = 0x0a;
  assert(pgfault_num == 6);
  cprintf("write Virt Page b in clock_check_swap\n");
  *(unsigned char*) 0x2000 = 0x0b;
  assert(pgfault_num == 7);
  cprintf("write Virt Page c in clock_check_swap\n");
  *(unsigned char*) 0x3000 = 0x0c;
  assert(pgfault_num == 8);
  cprintf("write Virt Page d in clock_check_swap\n");
  *(unsigned char*) 0x4000 = 0x0d;
  assert(pgfault_num == 9);
  cprintf("write Virt Page e in clock_check_swap\n");
  *(unsigned char*) 0x5000 = 0x0e;
  assert(pgfault_num == 10);
  cprintf("write Virt Page a in clock_check_swap\n");
  assert(*(unsigned char*) 0x1000 == 0x0a);
  *(unsigned char*) 0x1000 = 0x0a;
  assert(pgfault_num == 11);
  return 0;
}

static int _clock_init(void) {
  return 0;
}

static int _clock_set_unswappable(struct mm_struct* mm, uintptr_t addr) {
  return 0;
}

static int _clock_tick_event(struct mm_struct* mm) {
  return 0;
}

struct swap_manager swap_manager_clock = {
  .name = "extend_clock swap manager",
  .init = &_clock_init,
  .init_mm = &_clock_init_mm,
  .tick_event = &_clock_tick_event,
  .map_swappable = &_clock_map_swappable,
  .set_unswappable = &_clock_set_unswappable,
  .swap_out_victim = &_clock_swap_out_victim,
  .check_swap = &_clock_check_swap,
};
