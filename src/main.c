#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define log(vq, ...)                             \
    do                                                   \
    {                                                    \
        switch (vq)                                      \
        {                                                \
        case 'v':                                        \
            if (verbose)                                 \
            {                                            \
                printf("Verbose: "  __VA_ARGS__); \
            }                                            \
            break;                                       \
        case 'q':                                        \
            if (!quiet)                                  \
            {                                            \
                printf(__VA_ARGS__);             \
            }                                            \
            break;                                       \
        }                                                \
    } while (0)

int main(int argc, char **argv)
{
    char *file = NULL;
    char *output = NULL;
    char verbose = 0;
    char quiet = 0;
    int c = 0;
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
    if (output == NULL)
    {
        int len = strlen(file);
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
    log('v', "file: %s, output: %s, verbose: %d, quiet: %d\n", file, output, verbose, quiet);
    file_p = fopen(file, "r");
    if (file_p == NULL)
    {
        log('q', "No such file or directory");
        free(output);
        abort();
    }
    output_p = fopen(output, "w");
    fclose(output_p);
    fclose(file_p);
    return 0;
}