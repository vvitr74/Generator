
#include "stm32g0xx.h"
#include "spiffs.h"

#define W25_CHIP_ERASE 0x60
#define W25_READ_CMD 0x03
#define W25_WRITE_CMD 2
#define W25_STATUS_CMD 5
#define W25_WRITE_ENA_CMD 6

#define W25_4K_ERASE_CMD 0x20
#define W25_4K_ERASE_TIME 50

#define W25_32K_ERASE_CMD 0x52
#define W25_32K_ERASE_TIME 300

#define W25_64K_ERASE_CMD 0xd8
#define W25_64K_ERASE_TIME 500

#define W25_ERASE_TIMEOUT 3000

spiffs fs;

extern uint8_t spiDispCapture;

static u8_t spiffs_work_buf[SPIFFS_CFG_LOG_PAGE_SZ() * 2];
static u8_t spiffs_fds[32 * 4];
static u8_t spiffs_cache_buf[(SPIFFS_CFG_LOG_PAGE_SZ() + 32) * 4];


/**
* Retrieve flash memory status
*/
static uint8_t get_status()
{
    spi_cs_on();
    spi_transfer(W25_STATUS_CMD);
    volatile uint8_t status = spi_transfer(0);
    spi_cs_off();
    
    return status;
}



/**
* Erase entire chip
*/
static void flash_chip_erase()
{
    if(!(get_status() & 2))
    {
        spi_cs_on();
        spi_transfer(W25_WRITE_ENA_CMD);
        spi_cs_off();
    }
    
    spi_cs_on();
    spi_transfer(W25_CHIP_ERASE);
    spi_cs_off();
    delay_ms(25000);
    
}

static s32_t w25_flash_read(int addr, int size, char *buf)
{
  
    spi_cs_on();
    spi_transfer(W25_READ_CMD);
    spi_transfer(addr >> 16);
    spi_transfer(addr >> 8);
    spi_transfer(addr & 0xff);
    //spi_transfer(0);
    
    for(size_t i = 0; i< size; i++)
    {
        buf[i] = spi_transfer(0);
    }
    
    spi_cs_off();
    
    return SPIFFS_OK;
}

/**
* Write flash page
*/
static void w25_write_page(uint32_t addr, uint8_t size, char* buf)
{
    spi_cs_on();
    spi_transfer(W25_WRITE_ENA_CMD);
    spi_cs_off();

    spi_cs_on();
    spi_transfer(W25_WRITE_CMD);
    spi_transfer(addr >> 16);
    spi_transfer(addr >> 8);
    spi_transfer(addr & 0xff);
    
    for(size_t i = 0; i< size; i++)
    {
        spi_transfer(buf[i]);
    }
    
    spi_cs_off();
    delay_ms(1);
}

/**
* Write data into flash, up-to 256 bytes
*/
static s32_t w25_flash_write(int addr, int size, char *buf)
{
   
    do
    {
        uint8_t p_addr = addr & 0xff;
        size_t p_size = ((p_addr + size) >= 0x100) ? (0x100 - p_addr) : size;
        w25_write_page(addr, p_size, buf);
        addr += p_size;
        buf += p_size;
        size -= p_size;
    }
    while(size);
    
    return SPIFFS_OK;
}

/**
* Erase flash page/sector/block 
* @param addr Erase address
* @apram size Size which should be erased
*/
static s32_t w25_flash_erase(int addr, int size)
{
    
    if(!(get_status() & 2))
    {
        spi_cs_on();
        spi_transfer(W25_WRITE_ENA_CMD);
        spi_cs_off();
    }
    
    
    uint16_t time = W25_4K_ERASE_TIME;
    spi_cs_on();
    if(size <= 4096)
    {
         spi_transfer(W25_4K_ERASE_CMD);
    } 
    else if(size <= 32768)
    {
        spi_transfer(W25_32K_ERASE_CMD);
        time = W25_32K_ERASE_TIME;
    }
    else if(size <= 65536)
    {
        spi_transfer(W25_64K_ERASE_CMD);
        time = W25_64K_ERASE_TIME;
    }
    
    spi_transfer(addr >> 16);
    spi_transfer(addr >> 8);
    spi_transfer(addr & 0xff);
    
    spi_cs_off();
    uint16_t timeout = time;
    delay_ms(time);  
    while(1)
    {
        uint8_t status = get_status();
        
        if( status == 0xff)
        {
             return SPIFFS_ERR_ERASE_FAIL;
        }
        
        if((status & 1) == 0)
        {
            break;
        }
        
        delay_ms(time);
        timeout += time;
        if(timeout > W25_ERASE_TIMEOUT)
        {
            return SPIFFS_ERR_ERASE_FAIL;
        }
    }
    
    return SPIFFS_OK;
}

/**
* @brief Write part of file to fs
* @param filename Filename, null-terminated string
* @param fname_len Filename length
* @param offset of writing data
* @param data Data itself
* @param data_len Length of data
* @return 0 - if there no error
*/
int spiffs_write_file_part(const char *filename, size_t fname_len, uint32_t offset, uint8_t *data, size_t data_len)
{
    if(! (GPIOB->ODR & (1<<2)))
    {        
        GPIOB->BSRR = GPIO_BSRR_BS2;
        delay_ms(10);
    }
    
    spiffs_file fd = SPIFFS_open(&fs, filename, SPIFFS_CREAT | SPIFFS_RDWR, 0);

    
    if (SPIFFS_lseek(&fs, fd, offset, SPIFFS_SEEK_SET) < 0)
    {
        return SPIFFS_errno(&fs);
    }
    
    if (SPIFFS_write(&fs, fd, data, data_len) < 0)
    {
        return SPIFFS_errno(&fs);
    }

    SPIFFS_close(&fs, fd);

    return 0;
}

/**
* @brief Erase freq files from FS
* @return Error, if happend, otherwise 0
*/
int spiffs_erase_all()
{
    spiffs_DIR d;
    struct spiffs_dirent e;
    struct spiffs_dirent *pe = &e;
    int res;

    SPIFFS_opendir(&fs, "/", &d);
    while ((pe = SPIFFS_readdir(&d, pe)))
    {
        if (SPIFFS_remove(&fs, (char *)pe->name) < 0)
        {
            return SPIFFS_errno(&fs);
        }
    }

    SPIFFS_closedir(&d);
    return 0;
}

/**
* Initialize and configure spiffs 
*/
int spiffs_init()
{
    spiffs_config cfg;
   
    GPIOB->BSRR = GPIO_BSRR_BS2;
    
    cfg.hal_read_f = w25_flash_read;
    cfg.hal_write_f = w25_flash_write;
    cfg.hal_erase_f = w25_flash_erase;

    spi_cs_off(); 
    
   
    delay_ms(10);
    
    //flash_chip_erase();
    if(!(get_status() & 2))
    {
        spi_cs_on();
        spi_transfer(W25_WRITE_ENA_CMD);
        spi_cs_off();
    }
    
    
    int32_t res = SPIFFS_mount(&fs,
                               &cfg,
                               spiffs_work_buf,
                               spiffs_fds,
                               sizeof(spiffs_fds),
                               spiffs_cache_buf,
                               sizeof(spiffs_cache_buf),
                               0);
   
                               
    if (res == SPIFFS_ERR_NOT_A_FS)
    {
        SPIFFS_unmount(&fs);
        
        SPIFFS_format(&fs);
        return SPIFFS_mount(&fs,
                            &cfg,
                            spiffs_work_buf,
                            spiffs_fds,
                            sizeof(spiffs_fds),
                            spiffs_cache_buf,
                            sizeof(spiffs_cache_buf),
                            0);
    }

    return res;
}


/**
* Callback from write file method in freemodbus
* @param buf Input data
* @param len Length of data
* @return Error code
*/
int on_modbus_write_file(uint8_t* buf, size_t len)
{
    uint8_t fname_len = buf[0];
    char fname[18] = {};
  
    if(buf[0] >= sizeof(fname) - 1)
    {
        return 0x81;
    }
    
    char* ptr = (char*) &buf[1];
    memcpy(&fname[0], ptr, fname_len);        
    ptr += fname_len;
    
    uint32_t offset = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
    ptr += sizeof(uint32_t);
        
    int res = spiffs_write_file_part(fname, ptr - (char*)buf, offset, ptr, len - (ptr - (char*)buf)); 
    
    return res;
}

