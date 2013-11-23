/*
 * SPI driver for Milandr 1986ВЕ91 microcontroller.
 *
 * Copyright (C) 2010 Serge Vakulenko, <serge@vak.ru>
 *               2011 Dmitry Podkhvatilin, <vatilin@gmail.com>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "COPYING.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "COPY-UOS.txt" for details.
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <milandr/spi.h>
#include <kernel/internal.h>

/*
 * Номера прерываний от интерфейсов SSP.
 */
#define IRQ_SSP1	8
#define IRQ_SSP2	20
#define IRQ_SSP3	19


static inline int
init_hw(milandr_spim_t *spi, unsigned freq, unsigned bits_per_word, unsigned mode)
{
    SSP_t *reg = spi->reg;
    
    if (freq != spi->last_freq) {
        reg->CR1 &= ~ARM_SSP_CR1_SSE;

        unsigned div = (KHZ * (1000000000 / freq) + 1999999) / 2000000 - 1;
        if (div > 0xFF) {
            debug_printf ("SPI Master %d: too low frequency!\n", spi->port);
            spi->last_freq = 0;
            return SPI_ERR_BAD_FREQ;
        }

        reg->CR0 = (reg->CR0 & ~ARM_SSP_CR0_SCR(0xFF)) | ARM_SSP_CR0_SCR(div);
        spi->last_freq = freq;
    }
    if (mode != spi->last_mode) {
        reg->CR1 &= ~ARM_SSP_CR1_SSE;
        
        if (mode & SPI_MODE_LSB_FIRST)
            return SPI_ERR_MODE_NOT_SUPP;
        
        if (mode & SPI_MODE_CPOL) 
            reg->CR0 |= ARM_SSP_CR0_SPO;
        else reg->CR0 &= ~ARM_SSP_CR0_SPO;

        if (mode & SPI_MODE_CPHA) 
            reg->CR0 |= ARM_SSP_CR0_SPH;
        else reg->CR0 &= ~ARM_SSP_CR0_SPH;

        spi->last_mode = mode;
    }
    if (bits_per_word != spi->last_bits) {
        reg->CR1 &= ~ARM_SSP_CR1_SSE;

        if (bits_per_word > 16 || bits_per_word < 4) {
            debug_printf ("SPI Master %d: unsupported number of bits per word: %d\n",
                spi->port, bits_per_word);
            return SPI_ERR_BAD_BITS;
        }
        reg->CR0 = (reg->CR0 & ~ARM_SSP_CR0_DSS(0xF)) | ARM_SSP_CR0_DSS(bits_per_word);

        spi->last_bits = bits_per_word;
    }

    reg->CR1 |= ARM_SSP_CR1_SSE;

    return SPI_ERR_OK;
}

static int trx(spimif_t *spimif, spi_message_t *msg)
{
    milandr_spim_t      *spi = (milandr_spim_t *) spimif;
    SSP_t               *reg = spi->reg;
    uint8_t             *rxp_8bit;
    uint8_t             *txp_8bit;
    uint16_t            *rxp_16bit;
    uint16_t            *txp_16bit;
    unsigned            i, j;
    unsigned            bits_per_word = SPI_MODE_GET_NB_BITS(msg->mode);
    unsigned            cs_num = SPI_MODE_GET_CS_NUM(msg->mode);
    unsigned            mode = SPI_MODE_GET_MODE(msg->mode);
    int                 res;

    mutex_lock(&spimif->lock);

    res = init_hw(spi, msg->freq, bits_per_word, mode);
    if (res != SPI_ERR_OK) {
        mutex_unlock(&spimif->lock);
        return res;
    }

    // Прочищаем входную FIFO
    for (j = 0; j < 8; ++j) reg->DR;

    // Активируем CS
    // Если функция cs_control не установлена, то считаем, что для выборки 
    // устройства используется вывод FSS, и он работает автоматически
    if (spi->cs_control)
        spi->cs_control(spi->port, cs_num, mode & SPI_MODE_CS_HIGH);

    if (bits_per_word <= 8) {
        rxp_8bit = (uint8_t *) msg->rx_data;
        txp_8bit = (uint8_t *) msg->tx_data;

        i = 0;
        while (i < msg->word_count) {
            while (!(reg->SR & ARM_SSP_SR_TFE));
            for (j = 0; j < 8; ++j) {
                if (txp_8bit)
                    reg->DR = *txp_8bit++;
                else
                    reg->DR = 0;

                ++i;
                if (i >= msg->word_count)
                    break;

                if (rxp_8bit) {
                    while (reg->SR & ARM_SSP_SR_RNE) {
                        *rxp_8bit = reg->DR;
                        rxp_8bit++;
                    }
                }
            }
        }

        while (reg->SR & ARM_SSP_SR_BSY);

        if (rxp_8bit) {
            while (rxp_8bit - (uint8_t *) msg->rx_data < msg->word_count) {
                while (reg->SR & ARM_SSP_SR_RNE) {
                    *rxp_8bit = reg->DR;
                    rxp_8bit++;
                }
            }
        }
    } else if (bits_per_word > 8 && bits_per_word <= 16) {
        rxp_16bit = (uint16_t *) msg->rx_data;
        txp_16bit = (uint16_t *) msg->tx_data;

        i = 0;
        while (i < msg->word_count) {
            while (!(reg->SR & ARM_SSP_SR_TFE));
            for (j = 0; j < 8; ++j) {
                if (txp_16bit) {
                    reg->DR = *txp_16bit++;
                } else {
                    reg->DR = 0;
                }
                ++i;
                if (i >= msg->word_count)
                    break;

                if (rxp_16bit) {
                    while (reg->SR & ARM_SSP_SR_RNE) {
                        *rxp_16bit = reg->DR;
                        rxp_16bit++;
                    }
                }
            }
        }

        while (reg->SR & ARM_SSP_SR_BSY);

        if (rxp_16bit) {
            while (rxp_16bit - (uint16_t *) msg->rx_data < msg->word_count) {
                while (reg->SR & ARM_SSP_SR_RNE) {
                    *rxp_16bit = reg->DR;
                    rxp_16bit++;
                }
            }
        }
    } else {
        mutex_unlock(&spimif->lock);
        return SPI_ERR_BAD_BITS;
    }

    // Деактивируем CS
    // Если функция cs_control не установлена, то считаем, что для выборки 
    // устройства используется вывод FSS, и он работает автоматически
    if (!(mode & SPI_MODE_CS_HOLD) && spi->cs_control)
        spi->cs_control(spi->port, cs_num, !(mode & SPI_MODE_CS_HIGH));
        
    mutex_unlock(&spimif->lock);

    return SPI_ERR_OK;
}


int milandr_spim_init(milandr_spim_t *spi, unsigned port, spi_cs_control_func csc)
{
    memset(spi, 0, sizeof(milandr_spim_t));
    
    spi->port = port;
    spi->spimif.trx = trx;
    spi->cs_control = csc;
    
    if (port == 0) {
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP1;
        ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG1(7)) |
            ARM_SSP_CLOCK_EN1 | ARM_SSP_CLOCK_BRG1(0);
        spi->reg = ARM_SSP1;
    } else if (port == 1) {
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP2;
        ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG2(7)) |
            ARM_SSP_CLOCK_EN2 | ARM_SSP_CLOCK_BRG2(0);
        spi->reg = ARM_SSP2;
#ifdef ARM_1986BE1
    } else if (port == 2) {
        ARM_RSTCLK->PER_CLOCK |= ARM_PER_CLOCK_SSP3;
        ARM_RSTCLK->SSP_CLOCK = (ARM_RSTCLK->SSP_CLOCK & ~ARM_SSP_CLOCK_BRG3(7)) |
            ARM_SSP_CLOCK_EN3 | ARM_SSP_CLOCK_BRG3(0);
        spi->reg = ARM_SSP3;
#endif
    } else {
        return SPI_ERR_BAD_PORT;
    }
    
    spi->reg->CR0 = ARM_SSP_CR0_FRF_SPI;
    spi->reg->CPSR = 2;
    
    return 0;
}

