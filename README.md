# The Modus Hall Problem

#### Collection of Modus Hall solutions. Done as part of Operational Systems discipline @ UFSCar, Brazil, over 2019's second semester.

O repositório contém 4 soluções distintas para o problema, sendo 3 considerando cada Heathen e cada Prude como uma thread distinta e uma considerando apenas duas threads (cada qual controlando totalmente uma das facções).  

Para que as soluções sejam compiladas, basta executar o comando _make_ no diretório onde estiverem os arquivos .c e o arquivo _Makefile_. O nome do programa a ser executado será o mesmo do que o arquivo de código fonte que o gerou (sem, é claro, a extensão .c) e, assim que são executados, é solicitada ao usuário a inserção da quantidade de Heathens e Prudes que serão utilizadas (comportamento padrão entre os 4 progrmas).

-   modus_sem.c  
    Solução obtida no repositório [ModusHallProblem](https://github.com/DaanMoura/ModusHallProblem), desenvolvida por Luid Duarte, Daniel Moura e Sabrina Miranda também para a disciplina de Sistemas Operacionais e ligeiramente modificada. Considera cada indivíduo de cada facção como uma thread e segue o comportamento padrão para a definição da quantidadde de Heathens e Prudes.
    
-   modus_sem_enxuto.c  
    Solução criada a partir das ideias presentes nos programas do repositório [ModusHallProblem](https://github.com/DaanMoura/ModusHallProblem) e do algoritmo proposto no livro [The Little Book of Semaphores](http://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf). Por meio de dois semáforos e um mutex, considera cada indivíduo de cada facção como uma thread e segue o comportamento padrão para a definição da quantidadde de Heathens e Prudes.
    
-   modus_barr.c  
    Solução criada a partir das ideias presentes nos programas do repositório [ModusHallProblem](https://github.com/DaanMoura/ModusHallProblem), substituindo os semáforos por barreiras. Não segue totalmente a regra da maioria, considera cada indivíduo de cada facção como uma thread e segue o comportamento padrão para a definição da quantidadde de Heathens e Prudes.
    
-   modus_all.c  
  Solução criada a partir das ideias presentes nos programas do repositório [ModusHallProblem](https://github.com/DaanMoura/ModusHallProblem), usando uma thread para controlar toda uma facção. Segue o comportamento padrão para a definição da quantidadde de Heathens e Prudes.
