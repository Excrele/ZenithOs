#ifndef ATA_H
#define ATA_H

// ATA ports
#define ATA_PRIMARY_DATA       0x1F0
#define ATA_PRIMARY_ERROR      0x1F1
#define ATA_PRIMARY_SECTOR     0x1F3
#define ATA_PRIMARY_LBA_LOW    0x1F3
#define ATA_PRIMARY_LBA_MID    0x1F4
#define ATA_PRIMARY_LBA_HI     0x1F5
#define ATA_PRIMARY_DRIVE      0x1F6
#define ATA_PRIMARY_COMMAND    0x1F7
#define ATA_PRIMARY_STATUS     0x1F7
#define ATA_PRIMARY_ALT_STATUS 0x3F6

// ATA commands
#define ATA_CMD_READ_PIO       0x20
#define ATA_CMD_READ_PIO_EXT   0x24
#define ATA_CMD_WRITE_PIO      0x30
#define ATA_CMD_WRITE_PIO_EXT  0x34
#define ATA_CMD_IDENTIFY       0xEC

// ATA status bits
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

// Initialize ATA driver
void ata_init(void);

// Read sectors from ATA device
int ata_read_sectors(unsigned int lba, unsigned int count, unsigned char* buffer);

// Write sectors to ATA device
int ata_write_sectors(unsigned int lba, unsigned int count, unsigned char* buffer);

// Get device information
int ata_identify(unsigned char* buffer);

#endif // ATA_H

