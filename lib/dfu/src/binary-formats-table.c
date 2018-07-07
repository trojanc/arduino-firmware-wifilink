/* Automatically generated by ./build_src_zip on Thu, Jun 15, 2017  5:41:59 PM */

#include "dfu-internal.h"

extern int binary_probe(struct dfu_binary_file *);
	
extern int binary_decode_chunk(struct dfu_binary_file *bf, void *out_buf, unsigned long out_sz, phys_addr_t *addr);

extern int ihex_probe(struct dfu_binary_file *);
	
extern int ihex_decode_chunk(struct dfu_binary_file *bf, void *out_buf, unsigned long out_sz, phys_addr_t *addr);

const struct dfu_format_ops registered_formats_start[] = {
	{
		 ihex_probe, ihex_decode_chunk
	},
	{
		 binary_probe, binary_decode_chunk
	},
	{ NULL, NULL, }, 
};
/* Dummy symbol registered_formats_end */
const struct dfu_format_ops registered_formats_end[] = { NULL, NULL, };