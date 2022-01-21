
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#define file_size(x, file)        \
    do                            \
    {                             \
        fseek(file, 0, SEEK_END); \
        x = ftell(file);          \
        rewind(file);             \
    } while (0);

#define log(vq, ...)                             \
    do                                           \
    {                                            \
        switch (vq)                              \
        {                                        \
        case 'v':                                \
            if (verbose)                         \
            {                                    \
                printf("Verbose: " __VA_ARGS__); \
            }                                    \
            break;                               \
        case 'q':                                \
            if (!quiet)                          \
            {                                    \
                printf(__VA_ARGS__);             \
            }                                    \
            break;                               \
        }                                        \
    } while (0)

int main(int argc, char **argv)
{
    char *file = NULL;
    char *array_name = NULL;
    char *output_c = NULL;
    char *output_h = NULL;
    char verbose = 0;
    char quiet = 0;
    int c = 0;
    int len = 0;
    FILE *file_p = NULL;
    FILE *output_h_p = NULL;
    FILE *output_c_p = NULL;
    while ((c = getopt(argc, argv, "-o:vqh")) != -1)
    {
        switch (c)
        {
        case '\1':
            file = optarg;
            break;
        case 'o':
            output_c = optarg;
            break;
        case 'v':
            verbose = 1;
            quiet = 0;
            break;
        case 'q':
            verbose = 0;
            quiet = 1;
            break;
        case 'h':
            printf("file2c [-h] [-v] [-q] [-o <file>] <input>\n-h: Display this information\n-v: Provide additional details\n-q: Deactivates all warnings/errors\n-o <file>: Place the output into <file>");
            return 0;
            break;
        }
    }
    if (file == NULL)
    {
        log('q', "no input files");
        return -1;
    }
    len = strlen(file);
    if (output_c == NULL)
    {
        int i = 0;
        while ((file[len - i] != '.') && ((len - i) != 0))
        {
            i++;
        }
        if ((len - i) == 0)
        {
            output_c = (char *)malloc((len + 3) * sizeof(char));
            strcpy(output_c, file);
            output_c[len] = '.';
            output_c[len + 1] = 'c';
            output_c[len + 2] = '\0';
        }
        else
        {
            output_c = (char *)malloc((len + 1) * sizeof(char));
            strcpy(output_c, file);
            output_c[len - i] = '.';
            output_c[len - (i - 1)] = 'c';
            output_c[len - (i - 2)] = '\0';
        }
    }
    array_name = (char *)malloc((len + 1) * sizeof(char));
    strcpy(array_name, file);
    char *array_name_p = array_name;
    while (*array_name_p != '\0')
    {
        if (*array_name_p == '.' || *array_name_p == '/' || *array_name_p == '\\' || *array_name_p == ':')
        {
            *array_name_p = '_';
        }
        array_name_p++;
    }
    log('v', "file: %s, array_name: %s, output: %s, verbose: %d, quiet: %d\n", file, array_name, output_c, verbose, quiet);
    file_p = fopen(file, "rb");
    if (file_p == NULL)
    {
        log('q', "No such file or directory");
        free(output_c);
        return -1;
    }

    output_h = calloc(strlen(output_c) + 1, sizeof(char));
    memcpy(output_h, output_c, strlen(output_c));
    output_h[strlen(output_c) - 1] = 'h';
    output_h_p = fopen(output_h, "w");
    output_c_p = fopen(output_c, "w");
    fprintf(output_c_p, "#include \"%s\"\n\nconst uint8_t %s[] = {", output_h, array_name);
    long f_size = 0;
    file_size(f_size, file_p);
    f_size = (f_size < 1) * 1 + (1 <= f_size) * f_size;
    while (1)
    {
        uint8_t ch[1] = "";
        fread(ch, 1, 1, file_p);
        fprintf(output_c_p, "%" PRIu8, ch[0]);
        f_size--;
        if (!f_size)
        {
            fprintf(output_c_p, "};\nconst size_t %s_size = sizeof(%s);", array_name, array_name);
            break;
        }
        fprintf(output_c_p, ", ");
    }
    fprintf(output_h_p,
            "#ifndef %s_H\n"
            "#define %s_H\n"
            "\n"
            "#include <stdint.h>\n"
            "#include <stddef.h>\n"
            "\n"
            "extern const uint8_t %s[];\n"
            "\n"
            "extern const size_t %s_size;\n"
            "\n"
            "#endif",
            array_name, array_name, array_name, array_name);

    FILE *lib_c = fopen("file2c.c", "r+");
    FILE *lib_h = fopen("file2c.h", "r+");
    if (lib_c == NULL)
    {
        lib_c = fopen("file2c.c", "w");
        fprintf(lib_c,
                "#include \"file2c.h\"\n"
                "#include <stdlib.h>\n"
                "#include <stdio.h>\n"
                "#include <stdint.h>\n"
                "\n"
                "int create_file(const char* name, uint8_t * f, size_t size)\n"
                "{\n"
                "\tFILE* out = fopen(name, \"wb\");\n"
                "\tfwrite(f, sizeof(uint8_t), size, out);\n"
                "\tfclose(out);\n"
                "}\n");
        fclose(lib_c);
    }
    if (lib_h == NULL)
    {
        lib_h = fopen("file2c.h", "w");
        fprintf(lib_h,
                "#ifndef FILE2C_H\n"
                "#define FILE2C_H\n"
                "#include<stdint.h>\n"
                "\n"
                "extern int create_file(const char* name, uint8_t * f, size_t size)\n"
                "\n"
                "#endif // FILE2C_H\n");
        fclose(lib_h);
    }
    free(output_c);
    free(output_h);
    fclose(output_c_p);
    fclose(output_h_p);
    fclose(file_p);
    return 0;
}