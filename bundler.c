#include <stdbool.h>

#define SB_IMPLEMENTATION
#include "string_builder.h"

#define DA_IMPLEMENTATION
#include "da.h"

#define genf(...) do { printf(__VA_ARGS__); printf(" // %s:%d\n", __FILE__, __LINE__); } while (0)

#define BYTES_PER_LINE 100

char* pop_argv(int* argc, char*** argv) {
    char* arg = **argv;
    *argc -= 1;
    *argv += 1;
    return arg;
}

typedef struct {
    const char* file;
    size_t offset;
    size_t size;
}Resource;

typedef struct {
    Resource* items;
    size_t count;
    size_t capacity;
}Resources;

int main(int argc, char** argv) {
    const char* program = pop_argv(&argc, &argv);

    StringBuilder sb = {0};
    Resources rscs = {0};
    while (argc > 0) {
        const char* file = pop_argv(&argc, &argv);

        size_t offset = sb.count;
        if (!sb_read_file(&sb, file)) continue;
        size_t size = sb.count - offset;

        Resource rsc = {file, offset, size};
        da_append(&rscs, &rsc);
    }

    genf("#ifndef BUNDLE_H_");
    genf("#define BUNDLE_H_");
    genf("#include <stddef.h>");
    genf("typedef struct {");
    genf("    const char* file;");
    genf("    size_t offset;");
    genf("    size_t size;");
    genf("}Resource;");
    genf("const Resource resource[] = {");
    for (size_t i = 0; i < rscs.count; ++i) {
        genf("    { .file = \"%s\", .offset = %ld, .size = %ld },", rscs.items[i].file, rscs.items[i].offset, rscs.items[i].size);
    }
    genf("};");
    genf("const size_t resource_count = %ld;", rscs.count);
    genf("const unsigned char bundle[] = {");
    for (size_t i = 0; i < sb.count; ++i) {
        if (i % BYTES_PER_LINE == 0) {
            if (i != 0) genf("");
            printf("   ");
        }
        printf("0x%02X, ", (unsigned char)sb.items[i]);
    }
    genf("\n};");
    genf("const size_t bundle_size = %ld;", sb.count);
    genf("#endif // BUNDLE_H_");

    return 0;
}
