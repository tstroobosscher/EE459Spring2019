#ifdef DEBUG_86
void fat_dump_partition_table(struct PartitionTable *pt);
void fat16_dump_boot_sector(struct FAT16BootSector *bs);
void fat32_dump_boot_sector(struct FAT32BootSector *bs);
void fat16_dump_entry(struct FAT16Entry *e);
char fat32_dump_entry(struct FAT32Entry *e);
void fat_dump_sizes();
void dump_sector_addr(struct FAT32BootSector *bs, struct PartitionTable *pt);
#endif