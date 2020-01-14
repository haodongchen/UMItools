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
{ if ($0~/^@/) {print;} 
  else {
      for(i=12;i<=NF;i++){ if($i~/^XZ:Z:/) split($i,a,":") }
      if ( a[3]>=MinR ) print;
       }
}' | \
$samtools view -bh - > $output_bam

$sentieon util index $output_bam