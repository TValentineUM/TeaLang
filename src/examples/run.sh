for f in *.tl; do 
  echo "Processing $f"
  ./tealang -x "${f%.tl}.xml" -o "${f%.tl}.txt" "$f"
done
