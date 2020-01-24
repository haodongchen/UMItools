#!/bin/sh

if [ "$#" -ne 3 ] ; then
  echo "Filter single UMI consensus reads by group size" >&2
  echo "Usage: $0 INPUT_BAM OUTPUT_BAM MIN_READ" >&2
  exit 1
fi

input_bam=$1
output_bam=$2
min_read=$3

samtools='/path/to/samtools-1.9/samtools'
sentieon='/path/to/sentieon-genomics-201911/bin/sentieon'

$samtools view -@ 3 -h $input_bam | \
awk -v MinR=$min_read -v OFS="\t" '
BEGIN {reads=0; reads2=0; print "["strftime("Time = %m/%d/%Y %H:%M:%S", systime())"]  Executing FilterSingleUMIconsensus."> "/dev/stderr"}
{ if ($0~/^@/) {print;} 
  else {
      for(i=NF;i>=12;i--){ if($i~/^XZ:Z:/) {split($i,a,":");break;} }
      if ( a[3]>=MinR ) {reads2++;print;}
      reads++;
      if (reads % 2000000 == 0) {
          print "["strftime("Time = %m/%d/%Y %H:%M:%S", systime())"]  Processed "reads" reads.  Last read position: "$3":"$4> "/dev/stderr"}
      }
}
END {print "["strftime("Time = %m/%d/%Y %H:%M:%S", systime())"]  Done. Processed "reads" reads.  "reads2" reads passed the filter."> "/dev/stderr" } ' | \
$samtools view -bh - > $output_bam

$sentieon util index $output_bam
