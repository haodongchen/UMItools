#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

struct linebuf {
    char *line;
    size_t size;
    ssize_t len;
};

struct read {
    struct linebuf seqid;
    struct linebuf bases;
    struct linebuf third;
    struct linebuf quals;
};

static void extract(struct read *r1, struct read *r2, struct read *r3)
{
    const char *p, *q;
    int k, n;
    uint8_t t;

    // drop runts
    if (r1->bases.len <= 0 || r2->bases.len <= 0 || r3->bases.len <= 0)
        return;

    p = strchr(r1->seqid.line, ' ');
    if (p)
        r1->seqid.len = p - r1->seqid.line; // discard the comment
    else
        r1->seqid.len--; // strip off the trailing newline
    fwrite(r1->seqid.line, 1, r1->seqid.len, stdout);
    fputc('\t', stdout);
    fwrite("XR:Z:", 1, 5, stdout);
    fwrite(r3->bases.line, 1, r3->bases.len-1, stdout);   // umi barcode
    fputc('\t', stdout);
    fwrite("QX:Z:", 1, 5, stdout);
    fwrite(r3->quals.line, 1, r3->quals.len, stdout);   // umi Qual
    fwrite(r1->bases.line, 1, r1->bases.len, stdout);
    fwrite(r1->third.line, 1, r1->third.len, stdout);
    fwrite(r1->quals.line, 1, r1->quals.len, stdout);

    p = strchr(r2->seqid.line, ' ');
    if (p)
        r2->seqid.len = p - r2->seqid.line; // discard the comment
    else
        r2->seqid.len--; // strip off the trailing newline
    fwrite(r2->seqid.line, 1, r2->seqid.len, stdout);
    fputc('\t', stdout);
    fwrite("XR:Z:", 1, 5, stdout);
    fwrite(r3->bases.line, 1, r3->bases.len-1, stdout);   // umi barcode
    fputc('\t', stdout);
    fwrite("QX:Z:", 1, 5, stdout);
    fwrite(r3->quals.line, 1, r3->quals.len, stdout);   // umi Qual
    fwrite(r2->bases.line, 1, r2->bases.len, stdout);
    fwrite(r2->third.line, 1, r2->third.len, stdout);
    fwrite(r2->quals.line, 1, r2->quals.len, stdout);
}

static ssize_t gzgetline(char **linep, size_t *sizep, gzFile fp)
{
    char *line = *linep;
    size_t size = *sizep;
    ssize_t len = 0, n;

    if (size == 0) {
        size = *sizep = 1024;
        line = *linep = malloc(size);
    }
    while (gzgets(fp, line, size)) {
        n = strlen(line);
        len += n;
        if (line[n-1] == '\n')
            break;
        *sizep *= 2;
        *linep = realloc(*linep, *sizep);
        line = *linep + len;
        size = *sizep - len;
    }
    return len;
}

static int getread(gzFile *fp, struct read *r)
{
    if ((r->seqid.len = gzgetline(&r->seqid.line, &r->seqid.size, fp)) < 0 ||
        (r->bases.len = gzgetline(&r->bases.line, &r->bases.size, fp)) <= 0 ||
        (r->third.len = gzgetline(&r->third.line, &r->third.size, fp)) <= 0 ||
        (r->quals.len = gzgetline(&r->quals.line, &r->quals.size, fp)) <= 0)
        return -1;

    return r->seqid.len ? 1 : 0;
}

static int relread(struct read *r)
{
    free(r->seqid.line);
    free(r->bases.line);
    free(r->third.line);
    free(r->quals.line);
}

int main(int argc, char **argv)
{
    gzFile fp1, fp2, fp3;
    struct read r1, r2, r3;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s fq1 fq2 umi_fq\n", argv[0]);
        return 1;
    }

    fp1 = gzopen(argv[1], "r");
    if (!fp1) {
        fprintf(stderr, "Failed to open input file %s: %s\n", argv[1],
            strerror(errno ? errno : ENOMEM));
        return -1;
    }
    fp2 = gzopen(argv[2], "r");
    if (!fp2) {
        fprintf(stderr, "Failed to open input file %s: %s\n", argv[2],
            strerror(errno ? errno : ENOMEM));
        gzclose(fp1);
        return -1;
    }
    fp3 = gzopen(argv[3], "r");
    if (!fp3) {
        fprintf(stderr, "Failed to open input file %s: %s\n", argv[3],
            strerror(errno ? errno : ENOMEM));
        gzclose(fp1);
        gzclose(fp2);
        return -1;
    }

    // initialize lookup tables
    memset(&r1, 0, sizeof r1);
    memset(&r2, 0, sizeof r2);
    memset(&r3, 0, sizeof r3);
    
    while (getread(fp1, &r1) > 0 && getread(fp2, &r2) > 0 && getread(fp3, &r3) > 0)
        extract(&r1, &r2, &r3);

    relread(&r1);
    relread(&r2);
    relread(&r3);

    gzclose(fp1);
    gzclose(fp2);
    gzclose(fp3);
    return 0;
}

/* vim: set ts=4 sw=4 expandtab: */
