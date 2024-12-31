Tentei seguir os passos do CUDA e documentação KHRONOS mas não consegui fazer
OPENCL é GPU + CPU, e deparei-me com uns problemas, as tecnologias recentes de AMD cpus rejeitam OpenCL, e para adaptar é um processo muito dificil
https://github.com/intel/llvm/releases/tag/2022-09

A forma como OpenCL é compilado é diferente de CUDA porque temos (Host compilation em CPU e Kernel compilation em GPU) e o Kernel é compilado durante o runtime do programa, então surgem problemas como as variáveis terem de ser todas predefinidas o que gera problemas a usar ficheiros como o md5.h ou deti_coins_vault.
Não temos aquele waterfall effect de quando definimos algo noutro ficheiro como macros, e temos que dar refresh.
Para não falar que as funções e forma como se programa em OpenCL difere de hardware para hardware e com pouca documentação era dificil guiar-me.

Criei dois make files
O de opencl neste momento só está a correr o hello world de teste que fiz
make -f Makefile.opencl clean

