#include "test.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main(void) {
    const char *data1[] = {"eins", "zwei", "drei", "vierundvierzig", "fünfundfünfzig", "sechs", "7", "acht"};
    const char *data2[] = {"Merkur", "Venus", "Erde", "Mars", "Jupiter", "Saturn", "Neptun", "Uranus", "Pluto"};
    const char *data3[] = {"zweite", "nach eins"};
    const char *data4[] = {"ßchar", "Füße", "Gänsefüßchen", "über", "Änderung"};

    struct Canfigger *list = canfigger_parse_file(SOURCE_DIR "/german.conf", ',');
    assert (list);

    int i = 0;
    while (list) {
        printf("\nKey: %s\nValue:\n", list->key);
        const char **current_data_set;
        size_t data_set_size;

        switch (i) {
            case 0: current_data_set = data1; data_set_size = sizeof(data1) / sizeof(data1[0]); break;
            case 1: current_data_set = data2; data_set_size = sizeof(data2) / sizeof(data2[0]); break;
            case 2: current_data_set = data3; data_set_size = sizeof(data3) / sizeof(data3[0]); break;
            case 3: current_data_set = data4; data_set_size = sizeof(data4) / sizeof(data4[0]); break;
            default: fprintf(stderr, "Unexpected data set index: %d\n", i); exit(EXIT_FAILURE);
        }

        size_t j = 0;
        char *attr = NULL;
        //
        // Pass '&addr' to this function and it will get assigned an
        // attribute, or NULL if there are none.
        canfigger_free_current_attr_str_advance(list->attributes, &attr);
        while (attr) {
            fprintf(stderr, "Attribute: %s\n", attr);
            fprintf(stderr, "current_data_set[j]: %s\n", current_data_set[j]);
            assert(j < data_set_size);
            assert(strcmp(attr, current_data_set[j]) == 0);
            canfigger_free_current_attr_str_advance(list->attributes, &attr);
            j++;
        }

        assert(j == data_set_size); // Ensure we've matched all attributes for the current key
        i++;
        canfigger_free_current_key_node_advance(&list);
    }

    assert(i == 4); // Ensure we've processed all 4 lines

    return 0;
}
