#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

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
    char *output = NULL;
    char verbose = 0;
    char quiet = 0;
    int c = 0;
    int len = 0;
    FILE *file_p = NULL;
    FILE *output_p = NULL;
    while ((c = getopt(argc, argv, "-o:vq")) != -1)
    {
        switch (c)
        {
        case '\1':
            file = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'v':
            verbose = 1;
            quiet = 0;
            break;
        case 'q':
            verbose = 0;
            quiet = 1;
            break;
        }
    }
    if (file == NULL)
    {
        log('q', "no input files");
        abort();
    }
    len = strlen(file);
    if (output == NULL)
    {
        int i = 0;
        while ((file[len - i] != '.') && ((len - i) != 0))
        {
            i++;
        }
        if ((len - i) == 0)
        {
            output = (char *)malloc((len + 3) * sizeof(char));
            strcpy(output, file);
            output[len] = '.';
            output[len + 1] = 'h';
            output[len + 2] = '\0';
        }
        else
        {
            output = (char *)malloc((len + 1) * sizeof(char));
            strcpy(output, file);
            output[len - i] = '.';
            output[len - (i - 1)] = 'h';
            output[len - (i - 2)] = '\0';
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
    log('v', "file: %s, array_name: %s, output: %s, verbose: %d, quiet: %d\n", file, array_name, output, verbose, quiet);
    file_p = fopen(file, "r");
    if (file_p == NULL)
    {
        log('q', "No such file or directory");
        free(output);
        abort();
    }
    output_p = fopen(output, "w");
    fprintf(output_p, "#ifndef %s\n#define %s\n\n#include <stdint.h>\n\nconst uint8_t %s = {", array_name, array_name, array_name);
    while (1)
    {
        uint8_t ch[1] = "";
        fread(ch, 1, 1, file_p);
        fprintf(output_p, "%"PRIu8, ch[0]);
        if (feof(file_p))
        {
            fprintf(output_p, "};\n\n#endif");
            break;
        }
        fprintf(output_p, ", ");
    }
    fclose(output_p);
    fclose(file_p);
    return 0;
}