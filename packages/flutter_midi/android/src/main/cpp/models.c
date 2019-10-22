#include <stdint.h>
#include Models.h

struct CNote *create_cnote(uint8_t patchIndex, uint8_t pitch, float velocity) {
    struct CNote *cnote = (struct CNote *)malloc(sizeof(struct CNote));
    cnote->patchIndex = patchIndex;
    cnote->pitch = pitch;
    cnote->velocity = velocity;
}
