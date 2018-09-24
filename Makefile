all:
	nvcc -o cuda_conway cuda_conway.cu -gencode arch=compute_60,code=compute_60
	gcc -std=c99 -o non_cuda_conway non_cuda_conway.c
clean:
	rm -f conway non_cuda_conway make_input
input:
	gcc -std=c99 -o make_input make_input.c
