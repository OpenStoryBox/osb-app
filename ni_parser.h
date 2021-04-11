#ifndef NI_PARSER_H
#define NI_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t image_asset_index_in_ri;
    uint32_t sound_asset_index_in_si;
    uint32_t ok_transition_action_node_index_in_li;
    uint32_t ok_transition_number_of_options;
    uint32_t ok_transition_selected_option_index;
    uint32_t home_transition_action_node_index_in_li;
    uint32_t home_transition_number_of_options;
    uint32_t home_transition_selected_option_index;
    bool wheel;
    bool ok;
    bool home;
    bool pause;
    bool auto_play;
    uint16_t unknown;
} ni_node_t;

typedef struct {
    ni_node_t *current;
    char ri_file[13]; // 12 + \0
    char si_file[13]; // 12 + \0
} node_info_t;

uint32_t ni_get_node_index_in_li(uint32_t index_in_li);
void ni_decode_block512(uint8_t *data);
bool ni_get_node_info(uint32_t index, node_info_t *node);
bool ni_parser(const uint8_t *data);
void ni_dump();
void ni_set_ri_block(const uint8_t *data, uint32_t size);
void ni_set_si_block(const uint8_t *data, uint32_t size);
void ni_set_li_block(const uint8_t *data, uint32_t size);

#ifdef	__cplusplus
}
#endif

#endif // NI_PARSER_H
