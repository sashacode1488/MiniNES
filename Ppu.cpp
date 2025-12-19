#include "Ppu.h"
#include "Cartridge.h"
#include <cstring>

Ppu::Ppu() {
    sprScreen = SDL_CreateRGBSurfaceWithFormat(0, 256, 240, 32, SDL_PIXELFORMAT_RGBA8888);


    palScreen[0x00] = { 96, 96, 96, 255 };
    palScreen[0x01] = { 0, 40, 160, 255 };
    palScreen[0x02] = { 20, 24, 190, 255 };
    palScreen[0x03] = { 68, 0, 176, 255 };
    palScreen[0x04] = { 100, 0, 140, 255 };
    palScreen[0x05] = { 128, 0, 72, 255 };
    palScreen[0x06] = { 120, 16, 0, 255 };
    palScreen[0x07] = { 84, 36, 0, 255 };
    palScreen[0x08] = { 48, 64, 0, 255 };
    palScreen[0x09] = { 16, 88, 0, 255 };
    palScreen[0x0A] = { 0, 96, 0, 255 };
    palScreen[0x0B] = { 0, 84, 24, 255 };
    palScreen[0x0C] = { 0, 72, 96, 255 };
    palScreen[0x0D] = { 0, 0, 0, 255 };
    palScreen[0x0E] = { 0, 0, 0, 255 };
    palScreen[0x0F] = { 0, 0, 0, 255 };
    palScreen[0x10] = { 144, 144, 144, 255 };
    palScreen[0x11] = { 0, 88, 255, 255 };
    palScreen[0x12] = { 72, 104, 255, 255 };
    palScreen[0x13] = { 124, 88, 255, 255 };
    palScreen[0x14] = { 176, 64, 240, 255 };
    palScreen[0x15] = { 208, 64, 160, 255 };
    palScreen[0x16] = { 200, 88, 80, 255 };
    palScreen[0x17] = { 160, 112, 0, 255 };
    palScreen[0x18] = { 112, 136, 0, 255 };
    palScreen[0x19] = { 56, 152, 0, 255 };
    palScreen[0x1A] = { 0, 160, 0, 255 };
    palScreen[0x1B] = { 0, 144, 64, 255 };
    palScreen[0x1C] = { 0, 128, 168, 255 };
    palScreen[0x1D] = { 0, 0, 0, 255 };
    palScreen[0x1E] = { 0, 0, 0, 255 };
    palScreen[0x1F] = { 0, 0, 0, 255 };
    palScreen[0x20] = { 240, 240, 240, 255 };
    palScreen[0x21] = { 0, 152, 255, 255 };
    palScreen[0x22] = { 124, 168, 255, 255 };
    palScreen[0x23] = { 184, 152, 255, 255 };
    palScreen[0x24] = { 255, 152, 255, 255 };
    palScreen[0x25] = { 255, 152, 216, 255 };
    palScreen[0x26] = { 255, 176, 120, 255 };
    palScreen[0x27] = { 255, 200, 0, 255 };
    palScreen[0x28] = { 224, 224, 0, 255 };
    palScreen[0x29] = { 144, 248, 0, 255 };
    palScreen[0x2A] = { 0, 248, 0, 255 };
    palScreen[0x2B] = { 0, 224, 144, 255 };
    palScreen[0x2C] = { 0, 224, 224, 255 };
    palScreen[0x2D] = { 0, 0, 0, 255 };
    palScreen[0x2E] = { 0, 0, 0, 255 };
    palScreen[0x2F] = { 0, 0, 0, 255 };
    palScreen[0x30] = { 255, 255, 255, 255 };
    palScreen[0x31] = { 180, 255, 255, 255 };
    palScreen[0x32] = { 208, 208, 255, 255 };
    palScreen[0x33] = { 240, 208, 255, 255 };
    palScreen[0x34] = { 255, 208, 255, 255 };
    palScreen[0x35] = { 255, 192, 224, 255 };
    palScreen[0x36] = { 248, 224, 192, 255 };
    palScreen[0x37] = { 255, 240, 184, 255 };
    palScreen[0x38] = { 255, 232, 140, 255 };
    palScreen[0x39] = { 224, 255, 140, 255 };
    palScreen[0x3A] = { 192, 255, 192, 255 };
    palScreen[0x3B] = { 192, 255, 224, 255 };
    palScreen[0x3C] = { 0, 255, 255, 255 };
    palScreen[0x3D] = { 0, 0, 0, 255 };
    palScreen[0x3E] = { 0, 0, 0, 255 };
    palScreen[0x3F] = { 0, 0, 0, 255 };

    pOAM = &OAM[0];


    initializePalette();
}

Ppu::~Ppu() {
    SDL_FreeSurface(sprScreen);
}


void Ppu::initializePalette() {

    for (int i = 0; i < 0x40; i++) {
        palScreenRGBA[i] = SDL_MapRGBA(sprScreen->format,
            palScreen[i].r,
            palScreen[i].g,
            palScreen[i].b,
            palScreen[i].a);
    }
}

SDL_Surface* Ppu::getScreen() {
    return sprScreen;
}

void Ppu::connectCartridge(const std::shared_ptr<Cartridge>& cartridge) {
    this->cart = cartridge;
}

void Ppu::reset() {
    fine_x = 0x00;
    address_latch = 0x00;
    ppu_data_buffer = 0x00;
    scanline = 0;
    cycle = 0;
    control.reg = 0x00;
    mask.reg = 0x00;
    status.reg = 0x00;
    vram_addr.reg = 0x0000;
    tram_addr.reg = 0x0000;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 1024; j++) {
            tblName[i][j] = 0x00;
        }
    }

    for (int i = 0; i < 32; i++) {
        tblPalette[i] = 0x00;
    }
}

uint8_t Ppu::cpuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
    switch (addr) {
    case 0x0002:
        data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);
        status.vertical_blank = 0;
        address_latch = 0;
        break;
    case 0x0004:
        data = pOAM[oam_addr];
        break;
    case 0x0007:
        data = ppu_data_buffer;
        ppu_data_buffer = ppuRead(vram_addr.reg);
        if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;
        vram_addr.reg += (control.increment_mode ? 32 : 1);
        break;
    }
    return data;
}

void Ppu::cpuWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
    case 0x0000:
        control.reg = data;
        tram_addr.nametable_x = control.nametable_x;
        tram_addr.nametable_y = control.nametable_y;
        break;
    case 0x0001:
        mask.reg = data;
        break;
    case 0x0003:
        oam_addr = data;
        break;
    case 0x0004:
        pOAM[oam_addr] = data;
        break;
    case 0x0005:
        if (address_latch == 0) {
            fine_x = data & 0x07;
            tram_addr.coarse_x = data >> 3;
            address_latch = 1;
        }
        else {
            tram_addr.fine_y = data & 0x07;
            tram_addr.coarse_y = data >> 3;
            address_latch = 0;
        }
        break;
    case 0x0006:
        if (address_latch == 0) {
            tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
            address_latch = 1;
        }
        else {
            tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
            vram_addr.reg = tram_addr.reg;
            address_latch = 0;
        }
        break;
    case 0x0007:
        ppuWrite(vram_addr.reg, data);
        vram_addr.reg += (control.increment_mode ? 32 : 1);
        break;
    }
}

uint8_t Ppu::ppuRead(uint16_t addr, bool rdonly) {
    uint8_t data = 0x00;
    addr &= 0x3FFF;
    if (cart->ppuRead(addr, data)) {}
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (cart->mirror == Cartridge::VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = tblName[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = tblName[1][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = tblName[0][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = tblName[1][addr & 0x03FF];
        }
        else if (cart->mirror == Cartridge::HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) data = tblName[0][addr & 0x03FF];
            if (addr >= 0x0400 && addr <= 0x07FF) data = tblName[0][addr & 0x03FF];
            if (addr >= 0x0800 && addr <= 0x0BFF) data = tblName[1][addr & 0x03FF];
            if (addr >= 0x0C00 && addr <= 0x0FFF) data = tblName[1][addr & 0x03FF];
        }
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010 || addr == 0x0014 || addr == 0x0018 || addr == 0x001C)
            addr &= 0x000F;
        data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
    }
    return data;
}

void Ppu::ppuWrite(uint16_t addr, uint8_t data) {
    addr &= 0x3FFF;
    if (cart->ppuWrite(addr, data)) {}
    else if (addr >= 0x2000 && addr <= 0x3EFF) {
        addr &= 0x0FFF;
        if (cart->mirror == Cartridge::VERTICAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) tblName[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) tblName[1][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) tblName[0][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) tblName[1][addr & 0x03FF] = data;
        }
        else if (cart->mirror == Cartridge::HORIZONTAL) {
            if (addr >= 0x0000 && addr <= 0x03FF) tblName[0][addr & 0x03FF] = data;
            if (addr >= 0x0400 && addr <= 0x07FF) tblName[0][addr & 0x03FF] = data;
            if (addr >= 0x0800 && addr <= 0x0BFF) tblName[1][addr & 0x03FF] = data;
            if (addr >= 0x0C00 && addr <= 0x0FFF) tblName[1][addr & 0x03FF] = data;
        }
    }
    else if (addr >= 0x3F00 && addr <= 0x3FFF) {
        addr &= 0x001F;
        if (addr == 0x0010 || addr == 0x0014 || addr == 0x0018 || addr == 0x001C)
            addr &= 0x000F;
        tblPalette[addr] = data;
    }
}

void Ppu::clock() {
    auto IncrementScrollX = [&]() {
        if (mask.render_background || mask.render_sprites) {
            if (vram_addr.coarse_x == 31) {
                vram_addr.coarse_x = 0;
                vram_addr.nametable_x = !vram_addr.nametable_x;
            }
            else {
                vram_addr.coarse_x++;
            }
        }
        };

    auto IncrementScrollY = [&]() {
        if (mask.render_background || mask.render_sprites) {
            if (vram_addr.fine_y < 7) {
                vram_addr.fine_y++;
            }
            else {
                vram_addr.fine_y = 0;
                if (vram_addr.coarse_y == 29) {
                    vram_addr.coarse_y = 0;
                    vram_addr.nametable_y = !vram_addr.nametable_y;
                }
                else if (vram_addr.coarse_y == 31) {
                    vram_addr.coarse_y = 0;
                }
                else {
                    vram_addr.coarse_y++;
                }
            }
        }
        };

    auto TransferAddressX = [&]() {
        if (mask.render_background || mask.render_sprites) {
            vram_addr.nametable_x = tram_addr.nametable_x;
            vram_addr.coarse_x = tram_addr.coarse_x;
        }
        };

    auto TransferAddressY = [&]() {
        if (mask.render_background || mask.render_sprites) {
            vram_addr.fine_y = tram_addr.fine_y;
            vram_addr.nametable_y = tram_addr.nametable_y;
            vram_addr.coarse_y = tram_addr.coarse_y;
        }
        };

    auto LoadBackgroundShifters = [&]() {
        bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
        bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;
        bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) | ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
        bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) | ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
        };

    auto UpdateShifters = [&]() {
        if (mask.render_background) {
            bg_shifter_pattern_lo <<= 1;
            bg_shifter_pattern_hi <<= 1;
            bg_shifter_attrib_lo <<= 1;
            bg_shifter_attrib_hi <<= 1;
        }

        if (mask.render_sprites && cycle >= 1 && cycle < 258) {
            for (int i = 0; i < sprite_count; i++) {
                if (spriteScanline[i].x > 0) {
                    spriteScanline[i].x--;
                }
                else {
                    sprite_shifter_pattern_lo[i] <<= 1;
                    sprite_shifter_pattern_hi[i] <<= 1;
                }
            }
        }
        };

    if (scanline >= -1 && scanline < 240) {
        if (scanline == -1 && cycle == 1) {
            status.vertical_blank = 0;
            status.sprite_overflow = 0;
            status.sprite_zero_hit = 0;
            for (int i = 0; i < 64; i++) {
                sprite_shifter_pattern_lo[i] = 0;
                sprite_shifter_pattern_hi[i] = 0;
            }
        }

        if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
            UpdateShifters();

            switch ((cycle - 1) % 8) {
            case 0:
                LoadBackgroundShifters();
                bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
                break;

            case 2:
            {
                uint16_t addr = 0x23C0 | (vram_addr.nametable_y << 11)
                    | (vram_addr.nametable_x << 10)
                    | ((vram_addr.coarse_y >> 2) << 3)
                    | (vram_addr.coarse_x >> 2);
                bg_next_tile_attrib = ppuRead(addr);

                if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
                if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
                bg_next_tile_attrib &= 0x03;
                break;
            }

            case 4:
                bg_next_tile_lsb = ppuRead((control.pattern_background << 12)
                    + ((uint16_t)bg_next_tile_id << 4)
                    + vram_addr.fine_y + 0);
                break;

            case 6:
                bg_next_tile_msb = ppuRead((control.pattern_background << 12)
                    + ((uint16_t)bg_next_tile_id << 4)
                    + vram_addr.fine_y + 8);
                break;

            case 7:
                IncrementScrollX();
                break;
            }
        }

        if (cycle == 256) {
            IncrementScrollY();
        }

        if (cycle == 257 && scanline >= 0) {
            TransferAddressX();

            std::memset(spriteScanline, 0xFF, 64 * sizeof(sSprite));
            sprite_count = 0;
            status.sprite_overflow = 0;

            uint8_t nOAMEntry = 0;
            while (nOAMEntry < 64) {
                int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry * 4]);
                uint8_t spriteSize = control.sprite_size ? 16 : 8;

                if (diff >= 0 && diff < spriteSize) {
                    memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry * 4], 4);
                    sprite_count++;
                }
                nOAMEntry++;
            }
        }

        if (scanline == -1 && cycle >= 280 && cycle < 305) {
            TransferAddressY();
        }

        if (cycle == 340) {
            for (uint8_t i = 0; i < sprite_count; i++) {
                uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
                uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

                if (!control.sprite_size)
                {
                    if (!(spriteScanline[i].attribute & 0x80))
                    {
                        sprite_pattern_addr_lo =
                            (control.pattern_sprite << 12)
                            | (spriteScanline[i].id << 4)
                            | (scanline - spriteScanline[i].y);
                    }
                    else
                    {
                        sprite_pattern_addr_lo =
                            (control.pattern_sprite << 12)
                            | (spriteScanline[i].id << 4)
                            | (7 - (scanline - spriteScanline[i].y));
                    }
                }
                else
                {
                    if (!(spriteScanline[i].attribute & 0x80))
                    {
                        if (scanline - spriteScanline[i].y < 8)
                        {
                            sprite_pattern_addr_lo =
                                ((spriteScanline[i].id & 0x01) << 12)
                                | ((spriteScanline[i].id & 0xFE) << 4)
                                | ((scanline - spriteScanline[i].y) & 0x07);
                        }
                        else
                        {
                            sprite_pattern_addr_lo =
                                ((spriteScanline[i].id & 0x01) << 12)
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                                | ((scanline - spriteScanline[i].y) & 0x07);
                        }
                    }
                    else
                    {
                        if (scanline - spriteScanline[i].y < 8)
                        {
                            sprite_pattern_addr_lo =
                                ((spriteScanline[i].id & 0x01) << 12)
                                | (((spriteScanline[i].id & 0xFE) + 1) << 4)
                                | (7 - (scanline - spriteScanline[i].y) & 0x07);
                        }
                        else
                        {
                            sprite_pattern_addr_lo =
                                ((spriteScanline[i].id & 0x01) << 12)
                                | ((spriteScanline[i].id & 0xFE) << 4)
                                | (7 - (scanline - spriteScanline[i].y) & 0x07);
                        }
                    }
                }

                sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;
                sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
                sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

                if (spriteScanline[i].attribute & 0x40) {
                    auto flipbyte = [](uint8_t b) {
                        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
                        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
                        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
                        return b;
                        };
                    sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
                    sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
                }

                sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
                sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
            }
        }
    }

    if (scanline == 241 && cycle == 1) {
        status.vertical_blank = 1;
        if (control.enable_nmi) nmi = true;
    }

    uint8_t bg_pixel = 0, bg_palette = 0;
    if (mask.render_background) {
        uint16_t bit_mux = 0x8000 >> fine_x;
        uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
        uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
        uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    uint8_t fg_pixel = 0, fg_palette = 0, fg_priority = 0;
    bool fg_valid = false;

    if (mask.render_sprites) {
        for (int i = 0; i < sprite_count; i++) {
            if (spriteScanline[i].x == 0) {
                uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
                uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
                fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

                if (fg_pixel != 0) {
                    fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
                    fg_priority = (spriteScanline[i].attribute & 0x20) == 0;
                    fg_valid = true;

                    if (i == 0 && bg_pixel != 0 && cycle != 0) {
                        status.sprite_zero_hit = 1;
                    }
                    break;
                }
            }
        }
    }

    uint8_t final_pixel = 0;
    uint8_t final_palette = 0;

    if (bg_pixel == 0 && !fg_valid) {
        final_pixel = 0;
        final_palette = 0;
    }
    else if (bg_pixel == 0 && fg_valid) {
        final_pixel = fg_pixel;
        final_palette = fg_palette;
    }
    else if (bg_pixel != 0 && !fg_valid) {
        final_pixel = bg_pixel;
        final_palette = bg_palette;
    }
    else {
        if (fg_priority) {
            final_pixel = fg_pixel;
            final_palette = fg_palette;
        }
        else {
            final_pixel = bg_pixel;
            final_palette = bg_palette;
        }
    }


    if (scanline >= 0 && scanline < 240 && cycle >= 1 && cycle <= 256) {
        uint32_t* pixels = (uint32_t*)sprScreen->pixels;

        uint8_t palette_index = tblPalette[(final_palette << 2) + final_pixel] & 0x3F;

        pixels[scanline * 256 + (cycle - 1)] = palScreenRGBA[palette_index];
    }

    // ФІКС ДЛЯ MMC3: Виклик scanline для маппера на правильному циклі
    if (cart && (mask.render_background || mask.render_sprites)) {
        // MMC3 очікує виклик scanline на циклі 260 кожного скану
        if (cycle == 260) {
            cart->pMapper->scanline();
        }
    }

    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;

        if (scanline >= 261) {
            scanline = -1;
            frame_complete = true;
        }
    }
}