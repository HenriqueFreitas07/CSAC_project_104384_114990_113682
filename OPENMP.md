make deti_coins_intel

rm deti_coins_vault.txt

./deti_coins_intel -s7 8m



Correto Output seria:

#########
sort deti_coins_vault.txt | wc
16      65     896

Correto porque obtive 16 moedas.
#########

#########
sort deti_coins_vault.txt | uniq -c
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V00:DETI coin CsAC2024 AAD14 
1 V01:DETI coin CsAC2024 AAD14 
1 V01:DETI coin CsAC2024 AAD14 
1 V01:DETI coin CsAC2024 AAD14 
1 V01:DETI coin CsAC2024 AAD14 
1 V02:DETI coin CsAC2024 AAD14 
1 V02:DETI coin CsAC2024 AAD14 
1 V02:DETI coin CsAC2024 AAD14 
1 V02:DETI coin CsAC2024 AAD14 
1 V03:DETI coin CsAC2024 AAD14 
Correto, está tudo com um 1 atrás, o que signfica que são moedas únicas.
#########


Neste momento está a retornar isto:

joao@hjoao:~/Documents/universidade/universidade/3ano/CSAC/project/CSAC_project_104384_114990_113682$ ./deti_coins_intel -s7 8m
searching for 480 seconds using deti_coins_cpu_OpenMP_search()
THREAD: 0 ; LANE: 0 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 0 ; LANE: 0 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 0 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 3 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 0 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 2 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 0 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 2 ; LANE: 0 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 3 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 0 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 2 ; LANE: 1 ; FOUND ONE: DETI coin CsAC2024 AAD14 
THREAD: 1 ; LANE: 2 ; FOUND ONE: DETI coin CsAC2024 AAD14 
deti_coins_cpu_avx_search: 3 DETI coins found in 18742459756 attempts (expected 4.36 coins)
deti_coins_cpu_avx_search: 6 DETI coins found in 18747412636 attempts (expected 4.36 coins)
deti_coins_cpu_avx_search: 2 DETI coins found in 18748925064 attempts (expected 4.37 coins)
deti_coins_cpu_avx_search: 5 DETI coins found in 18754042892 attempts (expected 4.37 coins)


TODO: Seria retornar no final um print que soma os resultados obtidos por todas as outras threads.
      E indicar nos prints anteriores o numero da thread (já adicionei isto!)