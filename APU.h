#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include <vector>

class Bus;

class APU {
public:
    APU();
    void reset();
    void clock();
    void write(uint16_t addr, uint8_t data);
    uint8_t read(uint16_t addr);
    void setSampleCallback(std::function<void(float)> callback);
    void connectBus(Bus* bus) { this->bus = bus; }

    
    bool getIRQ() const { return irq_active; }

private:
    Bus* bus = nullptr;

    struct Pulse {
        bool enabled = false;
        uint8_t channel = 1;
        uint8_t duty = 0;
        uint8_t sequence = 0;
        uint16_t timer = 0;
        uint16_t period = 0;
        uint8_t volume = 0;
        bool constant_volume = false;
        bool halt = false;
        uint8_t envelope_counter = 0;
        uint8_t envelope_value = 0;
        bool envelope_start = false;
        uint16_t length_counter = 0;

        
        bool sweep_enable = false;
        bool sweep_reload = false;
        uint8_t sweep_divider = 0;
        uint8_t sweep_period = 0;
        uint8_t sweep_shift = 0;
        bool sweep_negate = false;

        void clock_timer();
        void clock_envelope();
        void clock_sweep();
        void clock_length();
        uint8_t output() const;
    };

    struct Triangle {
        bool enabled = false;
        bool control = false;
        uint8_t linear_counter = 0;
        uint8_t linear_reload = 0;
        bool linear_reload_flag = false;
        uint16_t timer = 0;
        uint16_t period = 0;
        uint8_t sequence = 0;
        uint16_t length_counter = 0;

        void clock_timer();
        void clock_linear_counter();
        void clock_length();
        uint8_t output() const;
    };

    struct Noise {
        bool enabled = false;
        uint16_t shift_register = 1;
        uint16_t timer = 0;
        uint16_t period = 0;
        uint8_t volume = 0;
        bool constant_volume = false;
        bool halt = false;
        uint8_t envelope_counter = 0;
        uint8_t envelope_value = 0;
        bool envelope_start = false;
        bool mode = false;
        uint16_t length_counter = 0;

        void clock_timer();
        void clock_envelope();
        void clock_length();
        uint8_t output() const;
        static const uint16_t period_table[16];
    };

    struct DMC {
        bool enabled = false;
        bool irq_enabled = false;
        bool irq_active = false;
        bool loop = false;
        uint16_t sample_addr = 0;
        uint16_t sample_len = 0;
        uint16_t current_addr = 0;
        uint16_t current_len = 0;
        uint8_t shift_register = 0;
        uint8_t bit_count = 0;
        uint16_t timer = 0;
        uint8_t period_index = 0;
        uint8_t output_level = 0;

        void clock_timer(APU* apu);
        void restart();
        uint8_t output() const;
        static const uint16_t period_table[16];
    };

    Pulse pulse1{};
    Pulse pulse2{};
    Triangle triangle{};
    Noise noise{};
    DMC dmc{};

    
    uint32_t frame_clock_counter = 0;
    uint8_t frame_mode = 0;
    bool frame_irq_inhibit = false;
    bool irq_active = false;

    
    std::function<void(float)> sample_callback;
    double sample_counter = 0.0;
    double samples_per_frame = 0.0;

    static const uint8_t length_table[32];

    
    void clock_quarter_frame();
    void clock_half_frame();
};