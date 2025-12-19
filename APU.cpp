#include "APU.h"
#include "Bus.h"

const uint8_t APU::length_table[32] = {
    10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
    12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};
const uint16_t APU::Noise::period_table[16] = { 4,8,16,32,64,96,128,160,202,254,380,508,762,1016,2034,4068 };
const uint16_t APU::DMC::period_table[16] = { 428,380,340,320,286,254,226,214,190,160,142,128,106,84,72,54 };

APU::APU() {
    pulse2.channel = 2;
    sample_counter = 0.0;
    samples_per_frame = 1789773.0 / 60.0;
}

void APU::reset() {
    pulse1 = {};
    pulse1.channel = 1;
    pulse2 = {};
    pulse2.channel = 2;
    triangle = {};
    noise = {};
    noise.shift_register = 1;
    dmc = {};

    frame_clock_counter = 0;
    irq_active = false;
    sample_counter = 0.0;

    write(0x4015, 0x00);
    write(0x4017, 0x00);
}

void APU::setSampleCallback(std::function<void(float)> callback) {
    sample_callback = callback;
}

void APU::clock() {
    
    triangle.clock_timer();

    
    if (frame_clock_counter % 2 == 0) {
        pulse1.clock_timer();
        pulse2.clock_timer();
        noise.clock_timer();
        dmc.clock_timer(this);
    }

    
    if (frame_mode == 0) { 
        if (frame_clock_counter == 7457) {
            clock_quarter_frame();
        }
        else if (frame_clock_counter == 14913) {
            clock_quarter_frame();
            clock_half_frame();
        }
        else if (frame_clock_counter == 22371) {
            clock_quarter_frame();
        }
        else if (frame_clock_counter == 29829) {
            clock_quarter_frame();
            clock_half_frame();
            if (!frame_irq_inhibit) {
                irq_active = true;
            }
        }
        else if (frame_clock_counter >= 29830) {
            frame_clock_counter = 0;
            if (!frame_irq_inhibit) {
                irq_active = true;
            }
        }
    }
    else { 
        if (frame_clock_counter == 7457) {
            clock_quarter_frame();
        }
        else if (frame_clock_counter == 14913) {
            clock_quarter_frame();
            clock_half_frame();
        }
        else if (frame_clock_counter == 22371) {
            clock_quarter_frame();
        }
        else if (frame_clock_counter == 37281) {
            clock_quarter_frame();
            clock_half_frame();
        }
        else if (frame_clock_counter >= 37282) {
            frame_clock_counter = 0;
        }
    }

    frame_clock_counter++;

    
    sample_counter += 1.0;
    if (sample_counter >= (1789773.0 / 44100.0)) {
        sample_counter -= (1789773.0 / 44100.0);

        float p1 = static_cast<float>(pulse1.output());
        float p2 = static_cast<float>(pulse2.output());
        float t = static_cast<float>(triangle.output());
        float n = static_cast<float>(noise.output());
        float d = static_cast<float>(dmc.output());

        float pulse_out = 0.0f;
        if (p1 + p2 > 0) {
            pulse_out = 95.88f / ((8128.0f / (p1 + p2)) + 100.0f);
        }

        float tnd_out = 0.0f;
        float tnd_sum = (t / 8227.0f) + (n / 12241.0f) + (d / 22638.0f);
        if (tnd_sum > 0) {
            tnd_out = 159.79f / ((1.0f / tnd_sum) + 100.0f);
        }

        float final_output = pulse_out + tnd_out;

        if (sample_callback) {
            sample_callback(final_output);
        }
    }
}

void APU::clock_quarter_frame() {
    pulse1.clock_envelope();
    pulse2.clock_envelope();
    noise.clock_envelope();
    triangle.clock_linear_counter();
}

void APU::clock_half_frame() {
    pulse1.clock_length();
    pulse2.clock_length();
    triangle.clock_length();
    noise.clock_length();
    pulse1.clock_sweep();
    pulse2.clock_sweep();
}

uint8_t APU::read(uint16_t addr) {
    if (addr == 0x4015) {
        uint8_t data = 0;
        if (pulse1.length_counter > 0) data |= 1;
        if (pulse2.length_counter > 0) data |= 2;
        if (triangle.length_counter > 0) data |= 4;
        if (noise.length_counter > 0) data |= 8;
        if (dmc.current_len > 0) data |= 16;
        if (irq_active) data |= 64;
        if (dmc.irq_active) data |= 128;
        irq_active = false;
        return data;
    }
    return 0;
}

void APU::write(uint16_t addr, uint8_t data) {
    switch (addr) {
    case 0x4000:
        pulse1.duty = (data >> 6) & 3;
        pulse1.halt = (data >> 5) & 1;
        pulse1.constant_volume = (data >> 4) & 1;
        pulse1.volume = data & 15;
        pulse1.envelope_start = true;
        break;
    case 0x4001:
        pulse1.sweep_enable = (data >> 7) & 1;
        pulse1.sweep_period = (data >> 4) & 7;
        pulse1.sweep_negate = (data >> 3) & 1;
        pulse1.sweep_shift = data & 7;
        pulse1.sweep_reload = true;
        break;
    case 0x4002:
        pulse1.period = (pulse1.period & 0xFF00) | data;
        break;
    case 0x4003:
        pulse1.period = (pulse1.period & 0x00FF) | ((uint16_t)(data & 7) << 8);
        if (pulse1.enabled) {
            pulse1.length_counter = length_table[(data >> 3) & 0x1F];
        }
        pulse1.envelope_start = true;
        pulse1.sequence = 0;
        break;
    case 0x4004:
        pulse2.duty = (data >> 6) & 3;
        pulse2.halt = (data >> 5) & 1;
        pulse2.constant_volume = (data >> 4) & 1;
        pulse2.volume = data & 15;
        pulse2.envelope_start = true;
        break;
    case 0x4005:
        pulse2.sweep_enable = (data >> 7) & 1;
        pulse2.sweep_period = (data >> 4) & 7;
        pulse2.sweep_negate = (data >> 3) & 1;
        pulse2.sweep_shift = data & 7;
        pulse2.sweep_reload = true;
        break;
    case 0x4006:
        pulse2.period = (pulse2.period & 0xFF00) | data;
        break;
    case 0x4007:
        pulse2.period = (pulse2.period & 0x00FF) | ((uint16_t)(data & 7) << 8);
        if (pulse2.enabled) {
            pulse2.length_counter = length_table[(data >> 3) & 0x1F];
        }
        pulse2.envelope_start = true;
        pulse2.sequence = 0;
        break;
    case 0x4008:
        triangle.control = (data >> 7) & 1;
        triangle.linear_reload = data & 127;
        break;
    case 0x400A:
        triangle.period = (triangle.period & 0xFF00) | data;
        break;
    case 0x400B:
        triangle.period = (triangle.period & 0x00FF) | ((uint16_t)(data & 7) << 8);
        if (triangle.enabled) {
            triangle.length_counter = length_table[(data >> 3) & 0x1F];
        }
        triangle.linear_reload_flag = true;
        break;
    case 0x400C:
        noise.halt = (data >> 5) & 1;
        noise.constant_volume = (data >> 4) & 1;
        noise.volume = data & 15;
        noise.envelope_start = true;
        break;
    case 0x400E:
        noise.mode = (data >> 7) & 1;
        noise.period = data & 15;
        break;
    case 0x400F:
        if (noise.enabled) {
            noise.length_counter = length_table[(data >> 3) & 0x1F];
        }
        noise.envelope_start = true;
        break;
    case 0x4010:
        dmc.irq_enabled = (data >> 7) & 1;
        dmc.loop = (data >> 6) & 1;
        dmc.period_index = data & 15;
        if (!dmc.irq_enabled) dmc.irq_active = false;
        break;
    case 0x4011:
        dmc.output_level = data & 127;
        break;
    case 0x4012:
        dmc.sample_addr = 0xC000 | ((uint16_t)data << 6);
        break;
    case 0x4013:
        dmc.sample_len = ((uint16_t)data << 4) | 1;
        break;
    case 0x4015:
        pulse1.enabled = data & 1; if (!pulse1.enabled) pulse1.length_counter = 0;
        pulse2.enabled = data & 2; if (!pulse2.enabled) pulse2.length_counter = 0;
        triangle.enabled = data & 4; if (!triangle.enabled) triangle.length_counter = 0;
        noise.enabled = data & 8; if (!noise.enabled) noise.length_counter = 0;
        dmc.enabled = data & 16;
        if (!dmc.enabled) {
            dmc.current_len = 0;
        }
        else if (dmc.current_len == 0) {
            dmc.restart();
        }
        dmc.irq_active = false;
        break;
    case 0x4017:
        frame_mode = (data >> 7) & 1;
        frame_irq_inhibit = (data >> 6) & 1;
        frame_clock_counter = 0;
        if (frame_mode) {
            clock_quarter_frame();
            clock_half_frame();
        }
        if (frame_irq_inhibit) {
            irq_active = false;
        }
        break;
    }
}


void APU::Pulse::clock_timer() {
    if (timer > 0) {
        timer--;
    }
    else {
        timer = period;
        sequence = (sequence + 1) % 8;
    }
}

void APU::Pulse::clock_length() {
    if (!halt && length_counter > 0) {
        length_counter--;
    }
}

void APU::Pulse::clock_envelope() {
    if (envelope_start) {
        envelope_start = false;
        envelope_value = 15;
        envelope_counter = volume;
    }
    else {
        if (envelope_counter > 0) {
            envelope_counter--;
        }
        else {
            envelope_counter = volume;
            if (envelope_value > 0) {
                envelope_value--;
            }
            else if (halt) {
                envelope_value = 15;
            }
        }
    }
}

void APU::Pulse::clock_sweep() {
    bool muting = false;
    uint16_t target_period = period;
    uint16_t change = period >> sweep_shift;

    if (sweep_negate) {
        target_period -= change;
        if (channel == 1) {
            target_period--;
        }
    }
    else {
        target_period += change;
    }

    
    if (period < 8 || target_period > 0x7FF) {
        muting = true;
    }

    
    if (sweep_divider == 0 && sweep_enable && sweep_shift > 0 && !muting) {
        period = target_period;
    }

    
    if (sweep_divider == 0 || sweep_reload) {
        sweep_divider = sweep_period;
        sweep_reload = false;
    }
    else {
        sweep_divider--;
    }
}

uint8_t APU::Pulse::output() const {
    static const uint8_t duty_table[4][8] = {
        {0,1,0,0,0,0,0,0},
        {0,1,1,0,0,0,0,0},
        {0,1,1,1,1,0,0,0},
        {1,0,0,1,1,1,1,1}
    };

    if (!enabled || length_counter == 0) {
        return 0;
    }

    
    if (period < 8) {
        return 0;
    }

    
    uint16_t target_period = period;
    if (sweep_enable && sweep_shift > 0) {
        uint16_t change = period >> sweep_shift;
        if (sweep_negate) {
            target_period -= change;
            if (channel == 1) target_period--;
        }
        else {
            target_period += change;
        }

        if (target_period > 0x7FF) {
            return 0;
        }
    }

    if (duty_table[duty][sequence] == 0) {
        return 0;
    }

    return constant_volume ? volume : envelope_value;
}


void APU::Triangle::clock_timer() {
    if (timer > 0) {
        timer--;
    }
    else {
        timer = period;
        if (linear_counter > 0 && length_counter > 0) {
            sequence = (sequence + 1) % 32;
        }
    }
}

void APU::Triangle::clock_length() {
    if (!control && length_counter > 0) {
        length_counter--;
    }
}

void APU::Triangle::clock_linear_counter() {
    if (linear_reload_flag) {
        linear_counter = linear_reload;
    }
    else if (linear_counter > 0) {
        linear_counter--;
    }

    if (!control) {
        linear_reload_flag = false;
    }
}

uint8_t APU::Triangle::output() const {
    static const uint8_t triangle_table[32] = {
        15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,
        0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    };

    if (!enabled || length_counter == 0 || linear_counter == 0) {
        return 0;
    }

    
    return triangle_table[sequence];
}


void APU::Noise::clock_timer() {
    if (timer > 0) {
        timer--;
    }
    else {
        timer = period_table[period];
        uint16_t feedback_bit = mode ? ((shift_register >> 6) & 1) : ((shift_register >> 1) & 1);
        uint16_t feedback = (shift_register & 1) ^ feedback_bit;
        shift_register >>= 1;
        shift_register |= (feedback << 14);
    }
}

void APU::Noise::clock_envelope() {
    if (envelope_start) {
        envelope_start = false;
        envelope_value = 15;
        envelope_counter = volume;
    }
    else {
        if (envelope_counter > 0) {
            envelope_counter--;
        }
        else {
            envelope_counter = volume;
            if (envelope_value > 0) {
                envelope_value--;
            }
            else if (halt) {
                envelope_value = 15;
            }
        }
    }
}

void APU::Noise::clock_length() {
    if (!halt && length_counter > 0) {
        length_counter--;
    }
}

uint8_t APU::Noise::output() const {
    if (!enabled || length_counter == 0 || (shift_register & 1)) {
        return 0;
    }
    return constant_volume ? volume : envelope_value;
}


void APU::DMC::clock_timer(APU* apu) {
    if (timer > 0) {
        timer--;
    }
    else {
        timer = period_table[period_index];

        if (bit_count == 0) {
            if (current_len > 0) {
                shift_register = apu->bus->cpuRead(current_addr, true);
                current_addr++;
                if (current_addr == 0) current_addr = 0x8000;
                current_len--;
                bit_count = 8;

                if (current_len == 0) {
                    if (loop) {
                        restart();
                    }
                    else if (irq_enabled) {
                        irq_active = true;
                    }
                }
            }
        }

        if (bit_count > 0) {
            if (shift_register & 1) {
                if (output_level <= 125) output_level += 2;
            }
            else {
                if (output_level >= 2) output_level -= 2;
            }
            shift_register >>= 1;
            bit_count--;
        }
    }
}

void APU::DMC::restart() {
    current_addr = sample_addr;
    current_len = sample_len;
}

uint8_t APU::DMC::output() const {
    return output_level;
}