#include "ata.h"
#include "pmm.h"

// Helper: Output byte to port
static inline void outb(unsigned short port, unsigned char value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Helper: Input byte from port
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Helper: Input word from port
static inline unsigned short inw(unsigned short port) {
    unsigned short ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Helper: Output word to port
static inline void outw(unsigned short port, unsigned short value) {
    asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

// Wait for ATA device to be ready
static int ata_wait_ready(void) {
    unsigned char status;
    int timeout = 100000;
    
    while (timeout--) {
        status = inb(ATA_PRIMARY_STATUS);
        if (!(status & ATA_SR_BSY)) {
            if (status & ATA_SR_ERR) {
                return -1; // Error
            }
            return 0; // Ready
        }
    }
    
    return -1; // Timeout
}

// Wait for data request
static int ata_wait_data(void) {
    unsigned char status;
    int timeout = 100000;
    
    while (timeout--) {
        status = inb(ATA_PRIMARY_STATUS);
        if (status & ATA_SR_ERR) {
            return -1; // Error
        }
        if (status & ATA_SR_DRQ) {
            return 0; // Data ready
        }
    }
    
    return -1; // Timeout
}

// Initialize ATA driver
void ata_init(void) {
    // Select master drive
    outb(ATA_PRIMARY_DRIVE, 0xA0);
    
    // Wait a bit
    for (volatile int i = 0; i < 1000; i++);
    
    // Check if device is present
    if (ata_wait_ready() != 0) {
        // Device not ready - might not be present
        return;
    }
}

// Read sectors from ATA device
int ata_read_sectors(unsigned int lba, unsigned int count, unsigned char* buffer) {
    if (count == 0) {
        return 0;
    }
    
    // Wait for device to be ready
    if (ata_wait_ready() != 0) {
        return -1;
    }
    
    // Select master drive and set LBA
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send sector count
    outb(ATA_PRIMARY_SECTOR, count & 0xFF);
    
    // Send LBA
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    
    // Send read command
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_PIO);
    
    // Read sectors
    for (unsigned int i = 0; i < count; i++) {
        // Wait for data
        if (ata_wait_data() != 0) {
            return -1;
        }
        
        // Read 256 words (512 bytes = 1 sector)
        unsigned short* buf = (unsigned short*)(buffer + (i * 512));
        for (int j = 0; j < 256; j++) {
            buf[j] = inw(ATA_PRIMARY_DATA);
        }
    }
    
    return count;
}

// Write sectors to ATA device
int ata_write_sectors(unsigned int lba, unsigned int count, unsigned char* buffer) {
    if (count == 0) {
        return 0;
    }
    
    // Wait for device to be ready
    if (ata_wait_ready() != 0) {
        return -1;
    }
    
    // Select master drive and set LBA
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send sector count
    outb(ATA_PRIMARY_SECTOR, count & 0xFF);
    
    // Send LBA
    outb(ATA_PRIMARY_LBA_LOW, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    
    // Send write command
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_PIO);
    
    // Write sectors
    for (unsigned int i = 0; i < count; i++) {
        // Wait for data request
        if (ata_wait_data() != 0) {
            return -1;
        }
        
        // Write 256 words (512 bytes = 1 sector)
        unsigned short* buf = (unsigned short*)(buffer + (i * 512));
        for (int j = 0; j < 256; j++) {
            outw(ATA_PRIMARY_DATA, buf[j]);
        }
        
        // Flush cache
        outb(ATA_PRIMARY_COMMAND, 0xE7);
        
        // Wait for completion
        if (ata_wait_ready() != 0) {
            return -1;
        }
    }
    
    return count;
}

// Get device information
int ata_identify(unsigned char* buffer) {
    // Wait for device to be ready
    if (ata_wait_ready() != 0) {
        return -1;
    }
    
    // Select master drive
    outb(ATA_PRIMARY_DRIVE, 0xA0);
    
    // Send identify command
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_IDENTIFY);
    
    // Wait for data
    if (ata_wait_data() != 0) {
        return -1;
    }
    
    // Read 256 words (512 bytes)
    unsigned short* buf = (unsigned short*)buffer;
    for (int i = 0; i < 256; i++) {
        buf[i] = inw(ATA_PRIMARY_DATA);
    }
    
    return 0;
}

