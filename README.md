# UMItools
Tools for UMI related works

- umi_extract.c

    Code to extract UMI sequences from the third fastq file. </br>
    Usage: umi_extract fq1 fq2 umi_fq
- FilterSingleUMIconsensus.sh

    Shell script to filter single-UMI consensus reads by group size. </br>
    Usage: sh FilterSingleUMIconsensus.sh INPUT_BAM OUTPUT_BAM MIN_READ
- FilterDuplexUMIconsensus.sh

    Shell script to filter duplex-UMI consensus reads by group sizes. </br>
    Usage: sh FilterDuplexUMIconsensus.sh INPUT_BAM OUTPUT_BAM MIN_READS
