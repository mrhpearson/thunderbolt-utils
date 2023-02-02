#include <linux/vfio.h>

#include "utils.h"

struct vfio_hlvl_params {
	int container;
	int group;
	int device;
	struct vfio_device_info *dev_info;
	struct list_item *bar_regions;
	struct vfio_region_info *pci_cfg_region;
};

bool check_vfio_module(void);
void bind_grp_modules(const char *pci_id, bool bind);
struct vfio_hlvl_params* vfio_dev_init(const char *pci_id);
void get_dev_bar_regions(struct vfio_hlvl_params *params, const char *pci_id);
void get_dev_pci_cfg_region(struct vfio_hlvl_params *params, const char *pci_id);
struct vfio_region_info* find_bar_for_off(const struct list_item *bar_regions, const u64 off);
u32 read_host_mem_long(const struct vfio_hlvl_params *params, const u64 off);
u16 read_host_mem_word(const struct vfio_hlvl_params *params, const u64 off);
u8 read_host_mem_byte(const struct vfio_hlvl_params *params, const u64 off);
