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

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HOMEDIR "/home"

char    *sjk_strdup(const char *);
int      isroot(void);

/*
 * This program checks if there are .forward files existant in users home 
 * directories and if they are not empty.
 *
 * USAGE: chkforward [home path]
 */
int
main(int argc, char *argv[])
{
    DIR             *d;
    struct dirent   *de;
    FILE            *in;
    char            *home = NULL; 
    char             file[256];
    char             buf[256];
    int              total;
    
    if (!isroot()) {
        fprintf(stderr, "sudo required for %s.\n", argv[0]);
        return 0;
    }

    if (argc == 2) 
        home = argv[1];
    else
        home = sjk_strdup(HOMEDIR);

    if ((d = opendir(home)) == NULL) {
        perror("opendir");
        return 1;
    }

    printf("Users without proper .forwards:\n");

    total = 0;
    while (( de = readdir(d)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) 
            continue;

        bzero(file, sizeof(file));
        snprintf(file, sizeof(file), "%s/%s/.forward", home, de->d_name);

        if ((in = fopen(file, "r")) == NULL) {
            //printf("%s does not have a .forward (%s)\n", de->d_name, file);
            printf("%s\n", de->d_name);
            total++;
            continue;
        }

        bzero(buf, sizeof(buf));

        if (fgets(buf, sizeof(buf), in) == NULL) {
            //printf("%s's .forward is malformed (%s)\n", de->d_name, file);
            printf("%s (malformed)\n ", de->d_name);
            total++;
            continue;
        }
        
        fclose(in);
    }

    printf("Total: %d user%s\n", total, (total > 1) ? "s" : "");

    if (closedir(d) == -1) {
        perror("closedir");
        return 1;
    }

    return 0;
}

/*
 * strdup() that exits upon error
 */
char *
sjk_strdup(const char *s)
{
    char *d;
    if ((d = strdup(s)) == NULL) {
        perror("sjk_strdup");
        exit(1);
    }
    return d;
}

/* 
 * Returns 1 if we're root, 0 if not.
 */
int
isroot(void)
{
    uid_t uid = getuid();
    return (uid == 0) ? 1 : 0;
}
