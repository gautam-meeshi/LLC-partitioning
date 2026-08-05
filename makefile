exe= tests/loads/stride/ldunrolled#this is unrolled 200,000 stride 8 loads
#exe= tests/ldn100k  #100K stride8 unrolled loads
exe2 = tests/loads/stride/ldunrolled#tests/loads/ld2M

compile:
	scons build/X86-MESI3/gem5.opt --default=X86 PROTOCOL=MESI_Three_Level SLICC_HTML=True -j40
run1core1llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=1 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU1C1L.txt	

run2core1llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=2 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU2C1L.txt	
run4core1llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB --space_partitioning_interval=100000 \
	--space_partitioning_algo=4 --q_file_path="/media/marg/gautam/stable/gem5/stdout/qfile.txt" \
	--l2_size=4MB --num-dirs=1 --num-cpus=4 --mem-size=8192MB --topology=MESH_4C_1LLC --network=garnet --llc_dump_interval=100000 > stdout/CU4C1L.txt	

run4core2llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB --space_partitioning_algo=4 --space_partitioning_interval=100000 \
	--l2_size=4MB --num-dirs=1 --num-cpus=4 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=4000000 > stdout/CU4C2L.txt	

run8core1llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU8C1L.txt	
run16core1llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=16MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU16C1L.txt	
run1core2llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=4MB --num-dirs=1 --num-cpus=1 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU1C2L.txt	

run2core2llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=4MB --num-dirs=1 --num-cpus=2 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU2C2L.txt	

runmlm8core:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB --space_partitioning_interval=5000000 \
	--space_partitioning_algo=4 --q_file_path="/media/marg/gautam/stable/gem5/stdout/qfile8c.txt" \
	--l2_size=4MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=config1 --network=garnet --llc_dump_interval=100000 > stdout/CU8C2L.txt
run8core2llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB --bw_partitioning_algo=0 --space_partitioning_algo=2 \
	--l2_size=4MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU8C2L5.txt	
run16core2llc:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB --dump_stats_after=10000 \
	--l2_size=8MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > stdout/CU16C2L.txt	

run1LLC:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU8C1L.txt
run2LLC:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=4MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=config1 --network=garnet --llc_dump_interval=100000 > CU8C2L.txt
run4LLC:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=4 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=2MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU8C4L.txt
run8LLC:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2};${exe2}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU8C8L.txt




run16core1LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=16MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU16C1L.txt
run16core2LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU16C2L.txt
run16core4LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=4 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=4MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=Crossbar --network=garnet --llc_dump_interval=100000 > CU16C4L.txt

runM16core1LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=1 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=16MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=config_16core_1llc --network=garnet --llc_dump_interval=100000 > MU16C1L.txt
runM16core2LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=2 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=config2 --network=garnet --llc_dump_interval=100000 > MU16C2L.txt
runM16core4LLC: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=4 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=4MB --num-dirs=1 --num-cpus=16 --mem-size=16384MB --topology=config_16core_4llc --network=garnet --llc_dump_interval=100000 > MU16C4L.txt

run32core4LLC:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};\
	${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=4 --l0i_size=32kB --l0d_size=32kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=32 --mem-size=32768MB --topology=config3 --network=garnet > check.txt
runMicro8: 
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby --cmd="${exe};${exe};${exe};${exe};${exe};${exe};${exe};${exe}" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=1MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet > check.txt

install_dependency:
	sudo apt install build-essential git m4 scons zlib1g zlib1g-dev \
    libprotobuf-dev protobuf-compiler libprotoc-dev libgoogle-perftools-dev \
    python3-dev python-is-python3 libboost-all-dev pkg-config

runMultiThread:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${mTExe}" --options="10000" \
	--cpu-type=DerivO3CPU --num-l2caches=8 --l0i_size=16kB --l0d_size=16kB --l1d_size=256kB \
	--l2_size=8MB --num-dirs=1 --num-cpus=8 --mem-size=8192MB --topology=Mesh1Dir --network=garnet > check.txt

mix2_cmd = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/lbm_base.amd64-m64-gcc54-nn
mix2_benchmark_options = 3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of;3000 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/reference.dat 0 0 /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000/100_100_130_ldc.of
mix2_cwd_args = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc54-nn.0000
mix2_input_args = ;;;;;;;
MAX_INS = 100000000
FAST_FORWARD = 1000000000
WARMUP_TIME = 50000000
runMix2:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${mix2_cmd}" --options="${mix2_benchmark_options}" --cwd="${mix2_cwd_args}" --input="${mix2_input_args}" \
	--cpu-type=DerivO3CPU --l0d_size=32kB --l0i_size=32kB --l1d_size=256kB --num-l2caches=2 --l2_size=4MB \
	--num-cpus=8 --num-dirs=1 --mem-size=8192MB --topology=config1 \
	--network=garnet -I ${MAX_INS} --fast-forward=${FAST_FORWARD} --dump_stats_after=${WARMUP_TIME} --space_partitioning_algo=1
gamess_exe = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn
mix4_cmd = ${gamess_exe};${gamess_exe};${gamess_exe};${gamess_exe};${gamess_exe};${gamess_exe};${gamess_exe};${gamess_exe}
mix4_benchmark_options = ;;;;;;;
gamess_cwd = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000
mix4_cwd_args = ${gamess_cwd};${gamess_cwd};${gamess_cwd};${gamess_cwd};${gamess_cwd};${gamess_cwd};${gamess_cwd};${gamess_cwd}
gamess_inp = /media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config
mix4_input_args = ${gamess_inp};${gamess_inp};${gamess_inp};${gamess_inp};${gamess_inp};${gamess_inp};${gamess_inp};${gamess_inp}

runMix4:
	build/X86-MESI3/gem5.opt configs/example/se.py --ruby \
	--cmd="${mix4_cmd}" --options="${mix4_benchmark_options}" --cwd="${mix4_cwd_args}" --input="${mix4_input_args}" \
	--cpu-type=DerivO3CPU --l0d_size=32kB --l0i_size=32kB --l1d_size=256kB --num-l2caches=2 --l2_size=4MB \
	--num-cpus=8 --num-dirs=1 --mem-size=8192MB --topology=config1 \
	--network=garnet -I ${MAX_INS} --fast-forward=${FAST_FORWARD} --dump_stats_after=${WARMUP_TIME} --space_partitioning_algo=0

runMix4Algo2:
	/media/marg/gautam/stable/gem5/build/fifo/gem5.opt -d /media/marg/gautam/results/fifo/stable/mixes-ff/mixes-BSP-8C-2LLC-UCP2/mix4/output configs/example/se.py --ruby '--cmd=/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/gamess_base.amd64-m64-gcc54-nn' '--options=;;;;;;;' '--cwd=/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000' '--input=/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config;/media/marg/gautam/benchmarks/SPEC2006-x86/benchspec/CPU2006/416.gamess/run/run_base_ref_amd64-m64-gcc54-nn.0000/triazolium.config' --cpu-type=DerivO3CPU --l0d_size=32kB --l0i_size=32kB --l1d_size=256kB --num-l2caches=2 --l2_size=4MB --num-cpus=8 --num-dirs=1 --mem-size=8192MB --topology=config1 --dump_stats_after=50000000 --space_partitioning_algo=2 --network=garnet --fast-forward=1000000000 -I 100000000
