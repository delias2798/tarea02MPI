/home/<USERNAME>/mpich-install

./configure --prefix=/home/<USERNAME>/mpich-install 2>&1 | tee c.txt

---------------------output
*** device configuration: ch3:nemesis
*** nemesis networks: tcp
---------------------output

 make 2>&1 | tee m.txt

make install 2>&1 | tee mi.txt

PATH=/home/<USERNAME>/mpich-install/bin:$PATH ; export PATH

which mpicc
which mpiexec

mpiexec -n <number> ./examples/cpi

mpiexec -f machinefile -n <number> ./examples/cpi