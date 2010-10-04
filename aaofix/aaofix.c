/*
 * Copyright (c) 2010 Svante J. Kvarnstrom <sjk@ankeborg.nu>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILEEXT "aao"
#define VERSION "0.1"

int comp_str(char *, const char *);
char *sjk_asprintf(const char *, ...);

/* 
 * USAGE: aaofix <file>
 * 
 * This small program reads <file>, swaps Swedish letters for
 * proper HTML codes and outputs to <file>.aao.
 *
 * å --> &aring;    Å --> &Aring;
 * ä --> &auml;     Ä --> &Auml;
 * ö --> &ouml;     Ö --> &Ouml;
 */
int
main(int argc, char *argv[])
{
    char *progname = argv[0];   /* Name of program */
    char *readfile = argv[1];   /* File to read */
    char *writefile = NULL;     /* File to write */
    char *p = NULL;             /* Line pointer */
    char buf[512];              /* Line buffer */
    FILE *fpr;                  /* File pointer for reading */
    FILE *fpw;                  /* File pointer for writing */
    int len;                    /* Line length */
    int i;                      /* Counter int */
    int ch;                     /* For getopt */

    static struct option options[] = {
        { "version",    no_argument, NULL, 'v' },
    };

    while ((ch = getopt_long(argc, argv, "v", options, NULL)) != -1) {
        switch (ch) {
        case 'v':
            printf("%s v%s\n", progname, VERSION);
            exit(0);
        }
    }

    argc -= optind;
    argv += optind;

    writefile = sjk_asprintf("%s.%s", readfile, FILEEXT);

    if (argc == 0) { 
        fprintf(stderr, "USAGE: %s <file>\n", progname);
        exit(0);
    }

    if ((fpr = fopen(readfile, "r")) == NULL) {
        fprintf(stderr, "Could not open %s for reading\n", readfile);
        exit(1);
    }

    if ((fpw = fopen(writefile, "w")) == NULL) {
        fprintf(stderr, "Could not open %s for writing\n", writefile);
        exit(1);
    }

    free(writefile);

    while ((p = fgets(buf, sizeof(buf), fpr)) != NULL) {
        len = strlen(p);
    
        for(i = 0; i < len; ) {
            if (comp_str(p+i, "å")) {
                fprintf(fpw, "&aring;");
                i += strlen("å");
                continue;
            }
            
            if (comp_str(p+i, "Å")) {
                fprintf(fpw, "&Aring;");
                i += strlen("Å");
                continue;
            }

            if (comp_str(p+i, "ä")) {
                fprintf(fpw, "&auml;");
                i += strlen("ä");
                continue;
            }

            if (comp_str(p+i, "Ä")) {
                fprintf(fpw, "&Auml;");
                i += strlen("Ä");
                continue;
            }

            if (comp_str(p+i, "ö")) {
                fprintf(fpw, "&ouml;");
                i += strlen("ö");
                continue;
            }

            if (comp_str(p+i, "Ö")) {
                fprintf(fpw, "&Ouml;");
                i += strlen("Ö");
                continue;
            }

            fprintf(fpw, "%c", p[i]);
            i++;
        }
    }
        
    
    return 0;
}
    
/* 
 * Compare two strings. Return 1 if they're the same, 0 if not. 
 */
int 
comp_str(char *c, const char *sc)
{
    if (strncmp(c, sc, strlen(sc)) == 0) 
        return 1;
    else 
        return 0;
}

/* 
 * asprintf() that exits upon error.
 */ 
char *
sjk_asprintf(const char *fmt, ...)
{
    va_list va;
    char *dst = NULL;
    va_start(va, fmt);
    vasprintf(&dst, fmt, va);
    if (dst == NULL) {
        perror("sjk_asprintf");
        exit(1);
    }
    va_end(va);
    return dst;
}

