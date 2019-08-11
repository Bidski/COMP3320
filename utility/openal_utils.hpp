#ifndef UTILITY_OPENAL_UTILS_HPP
#define UTILITY_OPENAL_UTILS_HPP

#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// For python style string formatting
#include "fmt/format.h"

// For matrix and vector arithmetic
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// For audio playing
#include "AL/al.h"
#include "AL/alc.h"
#include "sndfile.h"

#include "utility/openal_error_category.hpp"

#include "utility/sndfile_error_category.hpp"

namespace utility {
namespace al {
    // A wrapper for throwing exceptions based on OpenAL error codes
    // If no error is detector (code == AL_NO_ERROR) then nothing is thrown
    // Example: throw_al_error(alGetError(), "Error doing something important");
    // -------------------------------------------------------------------------
    inline void throw_al_error(const int& code, const std::string& msg) {
        if (code != AL_NO_ERROR) {
            throw std::system_error(code, openal_error_category(), msg);
        }
    }

    // A wrapper for throwing exceptions based on SndFile error codes
    // If no error is detector (code == SF_ERR_NO_ERROR) then nothing is thrown
    // Example: throw_sf_error(sf_error(), "Error doing something important");
    // -------------------------------------------------------------------------
    inline void throw_sf_error(const int& code, const std::string& msg) {
        if (code != SF_ERR_NO_ERROR) {
            if (code == static_cast<int>(sndfile_error_code::UNKNOWN)) {
                throw std::system_error(
                    code, sndfile_error_category(), fmt::format("{} - {}", msg, sf_error_number(code)));
            }
            else {
                throw std::system_error(code, sndfile_error_category(), msg);
            }
        }
    }

    struct OpenAL {
        OpenAL(const glm::vec3& position = glm::vec3(0.0f)) {
            print_device_list();

            // Get a handle to the default device
            audio_device = alcOpenDevice(nullptr);

            if (!audio_device) {
                throw_al_error(alGetError(), "Failed to initialize an audio device");
            }

            print_device_info();

            // Create a context
            audio_context = alcCreateContext(audio_device, nullptr);
            if (!audio_context) {
                throw_al_error(alGetError(), "Failed to create an audio context");
            }

            if (!alcMakeContextCurrent(audio_context)) {
                throw_al_error(alGetError(), "Failed to make the audio context current");
            }

            // Configure the listener
            set_listener_position(glm::vec3(0.0f, 0.0f, 1.0f));

            // Configure the source
            alGenSources(1, &audio_source);
            throw_al_error(alGetError(), "Failed to create an audio source");
            alSourcef(audio_source, AL_PITCH, 1.0f);
            throw_al_error(alGetError(), "Failed to set source pitch");
            alSourcef(audio_source, AL_GAIN, 1.0f);
            throw_al_error(alGetError(), "Failed to set source gain");
            alSource3f(audio_source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            throw_al_error(alGetError(), "Failed to set source velocity");
            alSourcei(audio_source, AL_LOOPING, AL_FALSE);
            set_source_position(position);

            // Generate a buffer
            alGenBuffers(1, &audio_buffer);
            throw_al_error(alGetError(), "Failed to create an audio buffer");
        }
        ~OpenAL() {
            alDeleteSources(1, &audio_source);
            alDeleteBuffers(1, &audio_buffer);
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(audio_context);
            alcCloseDevice(audio_device);
        }

        void set_listener_position(const glm::vec3& position) {
            alListener3f(AL_POSITION, position.x, position.y, position.z);
            throw_al_error(alGetError(), "Failed to set listener position");
            alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            throw_al_error(alGetError(), "Failed to set listener velocity");
            const std::array<float, 6> orientation = {position.x, position.y, position.z, 0.0f, 1.0f, 0.0f};
            alListenerfv(AL_ORIENTATION, orientation.data());
            throw_al_error(alGetError(), "Failed to set listener orientation");
        }

        void set_source_position(const glm::vec3& position) {
            alSource3f(audio_source, AL_POSITION, position.x, position.y, position.z);
            throw_al_error(alGetError(), "Failed to set source position");
        }

        void load_audio(const std::string& audio_file) {
            SF_INFO info;
            SNDFILE* file = sf_open(audio_file.c_str(), SFM_READ, &info);

            if (!file) {
                throw_sf_error(sf_error(file), fmt::format("Failed to open audio file '{}'", audio_file));
            }

            // Load audio data into a vector
            std::vector<uint16_t> data;
            std::array<int16_t, 4096> read_buf;
            size_t read_size = 0;
            while ((read_size = sf_read_short(file, read_buf.data(), read_buf.size())) != 0) {
                data.insert(data.end(), read_buf.begin(), read_buf.begin() + read_size);
            }

            // Put audio data into the audio buffer
            if (!alcMakeContextCurrent(audio_context)) {
                throw_al_error(alGetError(), "Failed to make the audio context current");
            }
            alBufferData(audio_buffer,
                         info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                         &data.front(),
                         data.size() * sizeof(uint16_t),
                         info.samplerate);
            throw_al_error(alGetError(), "Failed to load audio data");

            // Assign the audio buffer to the audio source
            alSourcei(audio_source, AL_BUFFER, audio_buffer);

            sf_close(file);
            throw_sf_error(sf_error(file), fmt::format("Failed to close audio file '{}'", audio_file));
        }

        void play_sound() {
            if (!alcMakeContextCurrent(audio_context)) {
                throw_al_error(alGetError(), "Failed to make the audio context current");
            }
            alSourcePlay(audio_source);
            throw_al_error(alGetError(), "Failed to play audio source");
        }

        void print_device_list() {
            auto parse_list = [](const char* list) {
                if (!list || *list == '\0') {
                    std::cout << fmt::format("    !!! none !!!") << std::endl;
                }
                else {
                    do {
                        std::cout << fmt::format("    {}", list) << std::endl;
                        list += strlen(list) + 1;
                    } while (*list != '\0');
                }
            };

            std::cout << fmt::format("Available playback devices:") << std::endl;
            if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE) {
                parse_list(alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER));
            }
            else {
                parse_list(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
            }
            std::cout << fmt::format("Available capture devices:") << std::endl;
            parse_list(alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
        }

        void print_device_info() {
            ALCint major, minor;

            if (audio_device) {
                const ALCchar* devname = nullptr;
                if (alcIsExtensionPresent(audio_device, "ALC_ENUMERATE_ALL_EXT") != AL_FALSE) {
                    devname = alcGetString(audio_device, ALC_ALL_DEVICES_SPECIFIER);
                }
                throw_al_error(alcGetError(audio_device), "Failed to get device name");
                if (!devname) {
                    devname = alcGetString(audio_device, ALC_DEVICE_SPECIFIER);
                }
                std::cout << fmt::format("** Info for device \"{}\" **", devname) << std::endl;
            }
            alcGetIntegerv(audio_device, ALC_MAJOR_VERSION, 1, &major);
            alcGetIntegerv(audio_device, ALC_MINOR_VERSION, 1, &minor);
            throw_al_error(alcGetError(audio_device), "Failed to get ALC version information");
            std::cout << fmt::format("ALC version: {}.{}", major, minor) << std::endl;
            if (audio_device) {
                std::cout << fmt::format("ALC Extensions:") << std::endl;
                print_list(alcGetString(audio_device, ALC_EXTENSIONS), ' ');
                throw_al_error(alcGetError(audio_device), "Failed to get extensions");
            }
        }

    private:
        static constexpr size_t MAX_WIDTH = 80;
        void print_list(const char* list, char separator) {
            size_t col         = MAX_WIDTH, len;
            const char* indent = "    ";
            const char* next;

            if (!list || *list == '\0') {
                std::cout << fmt::format("{}!!! none !!!", indent) << std::endl;
                return;
            }

            do {
                next = strchr(list, separator);
                if (next) {
                    len = next - list;
                    do {
                        next++;
                    } while (*next == separator);
                }
                else {
                    len = strlen(list);
                }

                if (len + col + 2 >= MAX_WIDTH) {
                    std::cout << fmt::format("{}", indent);
                    col = strlen(indent);
                }
                else {
                    std::cout << " ";
                    col++;
                }

                std::cout << fmt::format("{:{}s}", list, len);
                col += len;

                if (!next || *next == '\0') {
                    break;
                }
                std::cout << "," << std::endl;
                col++;

                list = next;
            } while (1);
            std::cout << std::endl;
        }

        ALCdevice* audio_device;
        ALCcontext* audio_context;
        ALuint audio_source;
        ALuint audio_buffer;
    };
}  // namespace al
}  // namespace utility


#endif  // UTILITY_OPENAL_UTILS_HPP
