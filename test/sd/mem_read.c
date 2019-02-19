#include <stdio.h>
#include <stdlib.h>

/*
 *	Partition table
 *
 *	Privides a mapping for every disk partition on the physical drive.
 *	Starts at absolute address: 0x1BE, contains 4 entries sequentially.
 *	Offset				Description 					Size (Bytes)
 *	0x00				0x80 if active, 0 otherwise		1
 *	0x01				start address (CHS)				3
 *	0x04				type, see below					1
 *	0x05				end address (CHS)				3
 *	0x80				relative offset (LBA)			4
 *	0x0C				size (sectors)					4
 */

/*
 *	Partition types
 *	0		unused
 *	1		FAT12
 *	4		FAT16 <= 32MiB
 *	5		extended partitions
 *	6		FAT16 > 32MiB
 *	11		FAT32 <= 2GiB
 *	12		FAT32, LBA, no size constraints
 *	14		FAT16, LBA
 *	15		extended partition, LBA
 */

/*
 *	Boot sector
 *	Offset 			Description 						Size (Bytes)
 *	0x000			Intel 80x86 Jump					3
 *	0x003			OEM Name							8
 *	0x00B			Sector size (Bytes)					2
 *	0x00D			Sectors per cluster					1
 *	0x00E			Reserved sectors (boot incl)		2
 *	0x010			Number of FATs						1
 *	0x011			Number of root entries				2
 *	0x013			Number of sectors (<= 32MiB)		2
 *	0x015			Media descriptor (removable?)		1
 *	0x016			Number of sectors for FAT 			2
 *	0x018			Number of sectors per track (CHS)	2
 *	0x01A			Number of heads (CHS)				2
 *	0x01C			Number of hidden sectors 			4
 *	0x020			Total number of sectors (> 32MiB)	4
 *	0x024			FAT32: Number of sectors 			4
 *	0x028			FAT32: Flags for FAT 				2
 *	0x02A			FAT32: File system version number 	2
 *	0x02C			FAT32: Cluster number for root dir 	4
 *	0x030 			FAT32: Sector number for FSInfo 	2
 *	0x032 			FAT32: Sector number for backup 	2
 *	0x034 			FAT32: Reserved for future			12
 *	0x040			Drive number (0x024 FAT12/16)		1
 *	0x041 			Current head (0x025 FAT12/16)		1
 *	0x042 			Boot Signature (0x026 FAT12/16)		1
 *	0x043 			Volume ID (0x027 FAT12/16)			4
 *	0x047			Volume label (0x02B FAT12/16)		11
 *	0x052 			File system type (0x036 FAT12/16)	8
 *	0x08A			Boot code (0x03E FAT12/16)			372 (448 FAT12/16)
 *	0x1FE 			Boot sector signature (0x55AA)		2
 */


int main() {
	FILE *bin = fopen("test.img", "r");
	unsigned int start_sector, length_sectors;

	fseek(bin, 0x1BE, SEEK_SET);

	for(int i = 0; i < 4; i++) {
		printf("Partition entry %d: First byte %02X\n", i, fgetc(bin));
		printf("    Partition start in CHS: %02X:%02X:%02X\n", fgetc(bin), fgetc(bin), fgetc(bin));
		printf("    Partition type %02X\n", fgetc(bin));
		printf("    Partition end in CHS: %02X:%02X:%02X\n", fgetc(bin), fgetc(bin), fgetc(bin));

		fread(&start_sector, 4, 1, bin);
		fread(&length_sectors, 4, 1, bin);
		printf("    Relative LBA address %08X, %d sectors long\n", start_sector, length_sectors);
	}

	fclose(bin);

	return 0;
}
