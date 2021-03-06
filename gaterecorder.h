#ifndef GATERECORDER_H
#define GATERECORDER_H

#include <kfr/all.hpp>

#include <deque>

#include "jackaudioio.hpp"

class GateRecorder : public JackCpp::AudioIO
{
public:
    using ftype = jack_default_audio_sample_t;
    using buffer_type = std::deque<kfr::univector<ftype>>;
    static constexpr size_t fftsize = 1024;
    static constexpr size_t chunk_size = 128;

    GateRecorder(float loudness,
                 float loudness_p,
                 float cutoff_,
                 float rolloff_,
                 float before_,
                 float after_,
                 float wait_,
                 float event_);

    virtual int audioCallback(jack_nframes_t nframes, audioBufVector inBufs,
        audioBufVector outBufs) noexcept;

    size_t buffers_in_seconds(float seconds) const;
private:
    float loudness_threshold, cutoff, rolloff;
    buffer_type buffers_buffer;
    kfr::audio_format af;

    size_t chunks;
    size_t buffers_past_loud = 0, max_buffers_wait;
    size_t buffers_begin, buffers_end;
    size_t buffer_limit_soft, buffer_limit_hard;
    size_t consecutive_loud_frames = 0, consecutive_loud_buffers_limit;
    bool recording = false, passthrough = false;

    buffer_type bflush(size_t tail_return = 0);

    kfr::zpk<kfr::fbase> filt;
    std::vector<kfr::biquad_params<kfr::fbase>> bqs;

    float passthrough_delta_threshold;
    buffer_type buffers_passthrough;
    unsigned buffers_passed_through;
    ftype loudness_momentary, loudness_short,
            loudness_intergrated, loudness_range_low,
            loudness_range_high;
    kfr::univector<ftype> ebuffer;
    kfr::ebu_r128<ftype> ebur128;
    void update_ebu();

    std::time_t event_time;
    std::string get_filename();
};

#endif // GATERECORDER_H
