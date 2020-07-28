# GraphBLAS-based-transitice-closure
the ideal is same as cuSparse veision(F/W).
If exist intermediary vertex X link u->X and X->v, add a path u->v

## Correctness
I test it on toy.dat and and find 38 path totally, which is same as perios work 
```
$ ./Zxie_TR toy.dat 
```
### run it

This project use GraphBLAS Template Library https://github.com/cmu-sei/gbtl which wap some useful function based on GraphBLAS
before running, use 
```
tail +<no_number_line> data.txt > data.txt
```
to remove no_number_line in data, then we can run it:

```
$ ./Zxie_TR <data>
```
