input_bam=$1
output_bam=$2
min_reads=$3

# min_reads has three values, e.g. 2,1,1
# The first value is applies to the total group size,
# the second value to one single-strand consensus, 
# and the last value to the other single-strand consensus.

samtools='/home/release/other_tools/samtools-1.9/samtools'
sentieon='/home/release/sentieon-genomics-201911/bin/sentieon'

$samtools view -h $input_bam | \
awk -v MinR=$min_reads -v OFS="\t" ' 
function min(b) {
   return b[1]>b[2]?b[2]:b[1]
}
function max(b) {
   return b[1]>b[2]?b[1]:b[2]
}
function sum(b) {
   return b[1]+b[2]
}
BEGIN {split(MinR,tmp,","); mr1=tmp[1]; mr2=tmp[2]<tmp[3]?tmp[2]:tmp[3]; mr3=tmp[2]<tmp[3]?tmp[3]:tmp[2]}
{ if ($0~/^@/) {print;} 
  else {
      for(i=12;i<=NF;i++){ if($i~/^XZ:Z:/){split($i,a,":");split(a[3],b,",")}}
      if (sum(b)>=mr1 && min(b)>=mr2 && max(b)>=mr3 ) print;
       }
}' | \
$samtools view -bh - > $output_bam

$sentieon util index $output_bam