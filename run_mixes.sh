exe= tests/loads/ldn100k
exe2 = tests/ldn100k

stride1=tests/loads/stride/ld1-100000
stride2=tests/loads/stride/ld2-100000
stride3=tests/loads/stride/ld3-100000
stride4=tests/loads/stride/ld4-100000
stride5=tests/loads/stride/ld5-100000
stride6=tests/loads/stride/ld6-100000
stride7=tests/loads/stride/ld7-100000
stride8=tests/loads/stride/ld8-100000

st1=tests/loads/stride/st1
st2=tests/loads/stride/st2
st3=tests/loads/stride/st3
st4=tests/loads/stride/st4
st5=tests/loads/stride/st5
st6=tests/loads/stride/st6
st7=tests/loads/stride/st7
st8=tests/loads/stride/st8

mTExe = tests/threads

exe1 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn
exe2 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn
exe3 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn
exe4 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/450.soplex/run/run_base_ref_amd64-m64-gcc54-nn.0000/soplex_base.amd64-m64-gcc54-nn
exe5 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/450.soplex/run/run_base_ref_amd64-m64-gcc54-nn.0000/soplex_base.amd64-m64-gcc54-nn
exe6 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/450.soplex/run/run_base_ref_amd64-m64-gcc54-nn.0000/soplex_base.amd64-m64-gcc54-nn
exe7 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/471.omnetpp/run/run_base_ref_amd64-m64-gcc54-nn.0000/omnetpp_base.amd64-m64-gcc54-nn
exe8 = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/471.omnetpp/run/run_base_ref_amd64-m64-gcc54-nn.0000/omnetpp_base.amd64-m64-gcc54-nn

MAX_INSTRUCTION=250000000
FAST_FORWARD=1000000000
WARMUP_TIME=150000000

compile:
	scons build/fifo/gem5.opt --default=X86 PROTOCOL=MESI_Three_Level SLICC_HTML=True -j17


create_dir:
	mkdir output/micromix9
	mkdir output/micromix10
	mkdir output/micromix11
	mkdir output/micromix12
	mkdir output/micromix13
	mkdir output/micromix14
	mkdir output/micromix15

remove_dir:
	rm -r output/micromix1
	rm -r output/micromix2
	rm -r output/micromix3
	rm -r output/micromix4
	rm -r output/micromix5
	rm -r output/micromix6
	rm -r output/micromix7
	rm -r output/micromix8
	rm -r output/micromix9

runAlternate:
	build/fifo/gem5.opt -d output/micromix9 configs/example/se.py --ruby \
	--cmd="${stride1};${stride8};${stride1};${stride8};${stride1};${stride8};${stride1};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix9/stdout.txt 2>output/micromix9/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix10 configs/example/se.py --ruby \
	--cmd="${stride2};${stride8};${stride2};${stride8};${stride2};${stride8};${stride2};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix10/stdout.txt 2>output/micromix10/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix11 configs/example/se.py --ruby \
	--cmd="${stride3};${stride8};${stride3};${stride8};${stride3};${stride8};${stride3};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix11/stdout.txt 2>output/micromix11/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix12 configs/example/se.py --ruby \
	--cmd="${stride4};${stride8};${stride4};${stride8};${stride4};${stride8};${stride4};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix12/stdout.txt 2>output/micromix12/stderr.txt &

	build/fifo/gem5.opt -d output/micromix13 configs/example/se.py --ruby \
	--cmd="${stride5};${stride8};${stride5};${stride8};${stride5};${stride8};${stride5};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix13/stdout.txt 2>output/micromix13/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix14 configs/example/se.py --ruby \
	--cmd="${stride6};${stride8};${stride6};${stride8};${stride6};${stride8};${stride6};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix14/stdout.txt 2>output/micromix14/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix15 configs/example/se.py --ruby \
	--cmd="${stride7};${stride8};${stride7};${stride8};${stride7};${stride8};${stride7};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix15/stdout.txt 2>output/micromix15/stderr.txt &

runStoreAlternate:
	build/fifo/gem5.opt -d output/micromix9 configs/example/se.py --ruby \
	--cmd="${st1};${st8};${st1};${st8};${st1};${st8};${st1};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix9/stdout.txt 2>output/micromix9/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix10 configs/example/se.py --ruby \
	--cmd="${st2};${st8};${st2};${st8};${st2};${st8};${st2};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix10/stdout.txt 2>output/micromix10/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix11 configs/example/se.py --ruby \
	--cmd="${st3};${st8};${st3};${st8};${st3};${st8};${st3};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix11/stdout.txt 2>output/micromix11/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix12 configs/example/se.py --ruby \
	--cmd="${st4};${st8};${st4};${st8};${st4};${st8};${st4};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix12/stdout.txt 2>output/micromix12/stderr.txt &

	build/fifo/gem5.opt -d output/micromix13 configs/example/se.py --ruby \
	--cmd="${st5};${st8};${st5};${st8};${st5};${st8};${st5};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix13/stdout.txt 2>output/micromix13/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix14 configs/example/se.py --ruby \
	--cmd="${st6};${st8};${st6};${st8};${st6};${st8};${st6};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix14/stdout.txt 2>output/micromix14/stderr.txt &
	
	build/fifo/gem5.opt -d output/micromix15 configs/example/se.py --ruby \
	--cmd="${st7};${st8};${st7};${st8};${st7};${st8};${st7};${st8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix15/stdout.txt 2>output/micromix15/stderr.txt &

runMicrobenchCombinations:
	
	build/fifo/gem5.opt -d output/micromix1 configs/example/se.py --ruby \
	--cmd="${stride1};${stride1};${stride1};${stride1};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix1/stdout.txt 2>output/micromix1/stderr.txt &

	build/fifo/gem5.opt -d output/micromix2 configs/example/se.py --ruby \
	--cmd="${stride2};${stride2};${stride2};${stride2};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix2/stdout.txt 2>output/micromix2/stderr.txt &

	build/fifo/gem5.opt -d output/micromix3 configs/example/se.py --ruby \
	--cmd="${stride3};${stride3};${stride3};${stride3};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix3/stdout.txt 2>output/micromix3/stderr.txt &


	build/fifo/gem5.opt -d output/micromix4 configs/example/se.py --ruby \
	--cmd="${stride4};${stride4};${stride4};${stride4};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix4/stdout.txt 2>output/micromix4/stderr.txt &

	build/fifo/gem5.opt -d output/micromix5 configs/example/se.py --ruby \
	--cmd="${stride5};${stride5};${stride5};${stride5};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix5/stdout.txt 2>output/micromix5/stderr.txt &

	build/fifo/gem5.opt -d output/micromix6 configs/example/se.py --ruby \
	--cmd="${stride6};${stride6};${stride6};${stride6};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix6/stdout.txt 2>output/micromix6/stderr.txt &

	build/fifo/gem5.opt -d output/micromix7 configs/example/se.py --ruby \
	--cmd="${stride7};${stride7};${stride7};${stride7};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix7/stdout.txt 2>output/micromix7/stderr.txt &

	build/fifo/gem5.opt -d output/micromix8 configs/example/se.py --ruby \
	--cmd="${stride8};${stride8};${stride8};${stride8};${stride8};${stride8};${stride8};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix8/stdout.txt 2>output/micromix8/stderr.txt &

	build/fifo/gem5.opt -d output/micromix9 configs/example/se.py --ruby \
	--cmd="${stride1};${stride8};${stride1};${stride8};${stride1};${stride8};${stride1};${stride8}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet \
	> output/micromix9/stdout.txt 2>output/micromix9/stderr.txt &