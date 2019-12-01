#include <stdint.h>

struct CNote {
    uint8_t patchIndex;
    uint8_t pitch;
    float velocity;
};

enum EventType {
    NoteOn,
    NoteOff,
    ChangeSoundFont
};

struct ScheduledEvent {
    int64_t frame;
    EventType type;

    uint8_t patchIndex;
    uint8_t pitch;
    float velocity;
    const char* filename;
};