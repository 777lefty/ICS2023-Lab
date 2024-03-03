#include "common.h"
#include <inttypes.h>

void mem_read(uintptr_t block_num, uint8_t *buf);
void mem_write(uintptr_t block_num, const uint8_t *buf);

static uint64_t cycle_cnt = 0;

void cycle_increase(int n) { cycle_cnt += n; }

// TODO: implement the following functions

struct CACHE_SLOT {
  uint8_t data[BLOCK_SIZE];
  uintptr_t tag;
  bool valid;
  bool dirty;
};

static struct CACHE_SLOT *cache_slot;
static uint32_t cache_total_size_width, cache_associativity_width, cache_group_width;
uintptr_t inner_addr_mask, group_number_mask, tag_mask, block_number_mask;

// Function declarations stay the same but are defined more clearly.
static inline uintptr_t extract_inner_addr(uintptr_t addr) {
  return addr & inner_addr_mask;
}

static inline uintptr_t align_inner_addr(uintptr_t addr) {
  return extract_inner_addr(addr) & ~0x3;
}

static inline uintptr_t extract_group_number(uintptr_t addr) {
  return (addr & group_number_mask) >> BLOCK_WIDTH;
}

static inline uintptr_t extract_tag(uintptr_t addr) {
  return (addr & tag_mask) >> (BLOCK_WIDTH + cache_group_width);
}

// Improved clarity for the calculation of cache address mapping
static inline uintptr_t map_to_cache_addr(uintptr_t addr, uint32_t i) {
  return (extract_group_number(addr) << cache_associativity_width) + i;
}

static inline uintptr_t extract_block_number(uintptr_t addr) {
  return (addr & block_number_mask) >> BLOCK_WIDTH;
}

static inline uintptr_t construct_block_number(uintptr_t tag, uint32_t index) {
  return (tag << cache_group_width) | index;
}

static void write_back(struct CACHE_SLOT *slot, uintptr_t addr) {
  mem_write(construct_block_number(slot->tag, extract_group_number(addr)), slot->data);
  slot->dirty = false;
}

static void read_into_slot(uintptr_t addr, uint32_t index) {
  struct CACHE_SLOT *slot = &cache_slot[map_to_cache_addr(addr, index)];
  
  mem_read(extract_block_number(addr), slot->data);
  slot->valid = true;
  slot->dirty = false;
  slot->tag = extract_tag(addr);
}

static inline uint32_t choose(uint32_t n) { 
	return rand() % n; 
	}

uint32_t cache_read(uintptr_t addr) {
  uintptr_t tag = extract_tag(addr);
  uintptr_t aligned_addr = align_inner_addr(addr);
  uint32_t associativity = (uint32_t)exp2(cache_associativity_width);

  for (uint32_t i = 0; i < associativity; ++i) {
    struct CACHE_SLOT *slot = &cache_slot[map_to_cache_addr(addr, i)];
    if (slot->valid && slot->tag == tag) {
      return *(uint32_t*)&slot->data[aligned_addr];
    }
  }

  uint32_t index = choose(associativity);
  struct CACHE_SLOT *slot = &cache_slot[map_to_cache_addr(addr, index)];

  if (slot->dirty) {
    write_back(slot, addr);
  }
  
  read_into_slot(addr, index);
  return *(uint32_t*)&slot->data[aligned_addr];
}

void cache_write(uintptr_t addr, uint32_t data, uint32_t wmask) {
  uintptr_t tag = extract_tag(addr);
  uintptr_t aligned_addr = align_inner_addr(addr);
  bool hit = false;
  struct CACHE_SLOT *slot = NULL;
  uint32_t associativity = (uint32_t)exp2(cache_associativity_width);

  for (uint32_t i = 0; i < associativity; ++i) {
    struct CACHE_SLOT *current_slot = &cache_slot[map_to_cache_addr(addr, i)];
    if (current_slot->valid && current_slot->tag == tag) {
      hit = true;
      slot = current_slot;
      break;
    }
  }

  if (!hit) {
    uint32_t index = choose(associativity);
    slot = &cache_slot[map_to_cache_addr(addr, index)];

    if (slot->valid && slot->dirty) {
      write_back(slot, addr);
    }
    
    read_into_slot(addr, index);
  }

  uint32_t *data_location = (uint32_t*)&(slot->data[aligned_addr]);
  *data_location = (*data_location & ~wmask) | (data & wmask);
  slot->dirty = true;
}

void init_cache(int total_size_width, int associativity_width) {
  cache_total_size_width = total_size_width;
  cache_associativity_width = associativity_width;
  cache_group_width = cache_total_size_width - BLOCK_WIDTH - cache_associativity_width;

  inner_addr_mask = mask_with_len(BLOCK_WIDTH);
  group_number_mask = mask_with_len(cache_group_width) << BLOCK_WIDTH;
  tag_mask = ~mask_with_len(BLOCK_WIDTH + cache_group_width);
  block_number_mask = ~mask_with_len(BLOCK_WIDTH);

  cache_slot = calloc((size_t)exp2(cache_total_size_width), sizeof(struct CACHE_SLOT));
  assert(cache_slot);
}

void display_statistic(void) {
  // Function content remains unimplemented as per the original TODO.
}

